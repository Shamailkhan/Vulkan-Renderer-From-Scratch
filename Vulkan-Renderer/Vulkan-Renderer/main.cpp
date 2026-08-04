#include <vulkan/vulkan.h>
#include <iostream>
#include<vector>
#include<string>
#include <stdexcept>


/**
 * Convert VkPhysicalDeviceType to human-readable string.
 * Makes debug output easier to read.
 */
std::string GetDeviceTypeName(VkPhysicalDeviceType type) {
	switch (type) {
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		return "Discrete GPU (Dedicated)";
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		return "Integrated GPU (iGPU)";
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		return "Virtual GPU";
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		return "CPU Rendering";
	default:
		return "Unknown";
	}
}
int ScoreGPU(VkPhysicalDeviceType type) {
	switch (type) {
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		return 1000;  // Dedicated GPU - best for games
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		return 100;   // Integrated GPU - acceptable
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		return 50;    // Virtual GPU - avoid if possible
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		return 1;     // CPU rendering - very slow
	default:
		return 0;
	}
}
/**
 * Convert VkQueueFlags to human-readable string.
 * VkQueueFlags is a bitmask, so we check each bit individually.
 *
 * Possible flags:
 * - VK_QUEUE_GRAPHICS_BIT (can render)
 * - VK_QUEUE_COMPUTE_BIT (can run compute shaders)
 * - VK_QUEUE_TRANSFER_BIT (can copy memory)
 * - VK_QUEUE_SPARSE_BINDING_BIT (advanced)
 */
std::string GetQueueFlagsString(VkQueueFlags flags) {
	std::string result;
	if (flags & VK_QUEUE_GRAPHICS_BIT) result += "Graphics ";
	if (flags & VK_QUEUE_COMPUTE_BIT) result += "Compute ";
	if (flags & VK_QUEUE_TRANSFER_BIT) result += "Transfer ";
	if (flags & VK_QUEUE_SPARSE_BINDING_BIT) result += "SparseBinding ";

	return result.empty() ? "None" : result;
}

int main()
{
   //vulkan triangle rendering

	/////////////STEP 1///////////////////////
	//DEFINING A STRUCTURE THAT DESCRIBE THE APPLICATION TO VULKAN 
	
	//VULKAN USE  STYPE TO IDENTIFY STRUCT TYPE 
	//{} IT INITIALIZE ALL THE FEILD TO ZERO OR NULL 
	// An Instance is your connection to the Vulkan driver.
	// We need to tell Vulkan who we are (application info).
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Renderer";
	appInfo.apiVersion = VK_API_VERSION_1_0;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	

	//////////////////////STEP 2//////////////////////////////
	   // This struct tells Vulkan *how* to create the Instance.
		// Think of it as "Instance configuration."

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledExtensionNames = nullptr;
	createInfo.enabledExtensionCount = 0;
	createInfo.ppEnabledExtensionNames = nullptr;


	//////////////////////STEP 3//////////////////////////////
	
	//this is where we tell vulkan driver to initialize 
	VkInstance instance;
	VkResult result=vkCreateInstance(&createInfo,nullptr,&instance);

	if (result == VK_SUCCESS)
	{
		std::cout << "[SUCCESS] Vulkan Instance created successfully.\n";
		std::cout << "VkResult = " << result << std::endl;
		std::cout << "Instance Handle = " << instance << std::endl;
	}
	else
	{
		std::cout << "[ERROR] Failed to create Vulkan Instance.\n";
		std::cout << "VkResult = " << result << std::endl;
		return -1;
	}

	//////////////////////STEP 4//////////////////////////////

	//add Physical Device Enumeration

		// Query how many GPUs are available on this system.
		// This uses the "count then get" pattern:
		//   1. First call with nullptr to get the count
		//   2. Allocate array of that size
		//   3. Second call to get the actual devices
	// Step 1: Ask "How many GPUs do you have?"
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);
	//we dont know ahead of time how many GPU exist so
	//first call with nullptr ->get the count
	//Allocat array of that size
	
	if (deviceCount == 0)
	{
		throw std::runtime_error("NO vulkan compatible GPU found");
	}

	std::cout << "\nFound " << deviceCount << " GPU(s)." << std::endl;

	//get all the GPU handles

	std::vector<VkPhysicalDevice> device(deviceCount);
	// get the data
	vkEnumeratePhysicalDevices(instance,&deviceCount,device.data());
	
	//////////////////////STEP 5//////////////////////////////
	//Query Properties of each GPU
	// For each GPU, get its properties (name, type, capabilities).
	// We'll use this information to select the best GPU.
	std::cout << "\nAvailable GPUs:" << std::endl;
	std::cout << "=================" << std::endl;

	for (uint32_t i = 0; i < deviceCount; ++i)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(device[i], &props);
		std::cout << "\nGPU " << i << ":" << std::endl;
		std::cout << "  Name: " << props.deviceName << std::endl;
		std::cout << "  Type: " << GetDeviceTypeName(props.deviceType) << std::endl;
		std::cout << "  Vulkan Version: "
			<< VK_VERSION_MAJOR(props.apiVersion) << "."
			<< VK_VERSION_MINOR(props.apiVersion) << "."
			<< VK_VERSION_PATCH(props.apiVersion) << std::endl;
		std::cout << "  Driver Version: " << props.driverVersion << std::endl;
	}

	///////////////////Step 6///////////////////////
	//select best GPU
	// Iterate through all GPUs and select the one with the highest score.
	// This uses a simple scoring system: dedicated GPUs score higher
	// than integrated GPUs, which score higher than virtual/CPU rendering.

	int bestDeviceIndex = 0;
	int bestScore = -1;
	for (uint32_t i = 0; i < deviceCount; ++i)
	{

		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(device[i], &props);

		int score = ScoreGPU(props.deviceType);

		if (score > bestScore) {
			bestScore = score;
			bestDeviceIndex = i;
		}
	}

	VkPhysicalDevice physicalDevice = device[bestDeviceIndex];
	std::cout << "best score" << bestScore << std::endl;
	///////////////////Step 7///////////////////////

	 VkPhysicalDeviceProperties selectedProps;
     vkGetPhysicalDeviceProperties(physicalDevice, &selectedProps);

	 std::cout << "\n=================" << std::endl;
	 std::cout << "Selected GPU (Index " << bestDeviceIndex << "): "
		 << selectedProps.deviceName << std::endl;
	 std::cout << "Type: " << GetDeviceTypeName(selectedProps.deviceType) << std::endl;
	 std::cout << "=================" << std::endl;

	 ///===============================================////////////////
	 //Logical Device
	 // Every GPU has "queue families" - groups of queues with identical capabilities.
		// We need to find which family supports graphics rendering.
		//
		// Why this matters:
		// - Some queues do graphics rendering
		// - Some queues do compute
		// - Some queues do memory transfers
		// We need to pick the right family for rendering.
	 //get total number of different queue families
	 uint32_t queueFamilyCount = 0;
	 vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&queueFamilyCount,nullptr);

	 //GET PROPERTIES
	 std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	 vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&queueFamilyCount,queueFamilies.data());

	 std::cout << "\nQueue Families:" << std::endl;
	 std::cout << "=================" << std::endl;

		// Search through queue families to find one that supports graphics.
		// For rendering, we need VK_QUEUE_GRAPHICS_BIT capability.



	 int graphicsQueueFamilyIndex = -1;

	 for (uint32_t i = 0; i < queueFamilyCount; ++i)
	 {
		 std::cout << "Family " << i << ":" << std::endl;
		 std::cout << "  Capabilities: "
			 << GetQueueFlagsString(queueFamilies[i].queueFlags)
			 << std::endl;

		 std::cout << "  Queue Count: "
			 << queueFamilies[i].queueCount
			 << std::endl;

		 // Check if this queue family supports graphics.
		 if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		 {
			 graphicsQueueFamilyIndex = i;

			 std::cout << "  Graphics queue found!" << std::endl;

			 break;
		 }
	 }

	 if (graphicsQueueFamilyIndex == -1) {
		 throw std::runtime_error("No graphics queue family found!");
	 }

	 std::cout << "\nSelected graphics queue family: " << graphicsQueueFamilyIndex << std::endl;
	 
	 //Create Logical Device
	 // Physical = hardware, Logical = your connection to it.
	   // A Logical Device is your exclusive connection to the GPU.
		// Think of it as "opening an account at the bank."
		// Multiple programs can have different logical devices from the same physical GPU.

		// First, specify which queues we want to create.
		// We want 1 graphics queue from the graphics family.
	  
	  
	 float queuePriority = 1.0f; //Maximum priority(range: 0.0 - 1.0)
	 VkDeviceQueueCreateInfo queueCreateInfo{};
	 queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	 queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
	 queueCreateInfo.queueCount = 1;
	 queueCreateInfo.pQueuePriorities = &queuePriority;

	 // Now create the Logical Device.
	 // We tell Vulkan:
	 // - Which queue families to use
	 // - Which extensions we need (none for now)
	 // - Which features we want (none for now)

	 VkDeviceCreateInfo deviceCreateInfo{};
	 deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	 deviceCreateInfo.queueCreateInfoCount = 1;  // We're creating 1 queue
	 deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	 deviceCreateInfo.enabledExtensionCount = 0;
	 deviceCreateInfo.ppEnabledExtensionNames = nullptr;
	 deviceCreateInfo.pEnabledFeatures = nullptr;  // Use default feature

	 VkDevice logicalDevice;
	 result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);

	 if (result != VK_SUCCESS) {
		 std::string errorMsg = "Failed to create logical device: ";
		 switch (result) {
		 case VK_ERROR_OUT_OF_HOST_MEMORY:
			 errorMsg += "Out of host memory";
			 break;
		 case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			 errorMsg += "Out of device memory";
			 break;
		 case VK_ERROR_INITIALIZATION_FAILED:
			 errorMsg += "Initialization failed";
			 break;
		 case VK_ERROR_EXTENSION_NOT_PRESENT:
			 errorMsg += "Requested extension not available";
			 break;
		 case VK_ERROR_FEATURE_NOT_PRESENT:
			 errorMsg += "Requested feature not available";
			 break;
		 default:
			 errorMsg += "Unknown error";
		 }
		 throw std::runtime_error(errorMsg);
	 }

	 std::cout << " Logical Device created successfully!" << std::endl;
	 // ========================================
	  // STEP 10: Get Queue Handle
	  // ========================================

	   // After creating a Logical Device with queues, we need to get the actual queue handles.
		// These handles are what we'll use to submit rendering commands later.

	 VkQueue graphicQueue;
	 vkGetDeviceQueue(logicalDevice,graphicsQueueFamilyIndex,0,&graphicQueue);
	 //								Which queue in the family (0 = first queue)
	 // 
	 std::cout << "Graphics queue retrieved successfully!" << std::endl;


	 std::cout << "\n=================" << std::endl;
	 std::cout << "VULKAN SETUP COMPLETE" << std::endl;
	 std::cout << "=================" << std::endl;
	 std::cout << "Instance: Created" << std::endl;
	 std::cout << "Physical Device: Selected (" << selectedProps.deviceName << ")" << std::endl;
	 std::cout << "Logical Device: Created" << std::endl;
	 std::cout << "Graphics Queue: Obtained" << std::endl;
	 std::cout << "=================" << std::endl;



	 //up till this point we can talk to gpu
	 // can not store vertex data on gpu
	 //next step is memeory allocation 
	 //cpu data lives in system ram 
		//application data lives here
	 //gpu memeory (Video Memory)
		// rendering core acess data here 
	 //problem is data transfer is slow 

	 // different GPU memeory 
		//device local memory (VRAM)
		/*Data flow:
			CPU (app data)
			  └─→ Host Visible Buffer (staging)
					└─→ vkCmdCopyBuffer (transfer queue)
						  └─→ Device Local Buffer (GPU fast memory)*/
		

	  // Step 9: Query Memory Properties













	 // ========================================
	 // STEP 8: Clean Up (Reverse Order!)
	 // ========================================
	 // IMPORTANT: Destroy in reverse order of creation!
	 // This prevents trying to destroy the Instance while it's still in use.
	 //
	 // Destruction order:
	 // 1. Logical Device (created last)
	 // 2. Instance (created first)

	 vkDestroyDevice(logicalDevice, nullptr);
	 std::cout << "\n Logical Device destroyed." << std::endl;

	 vkDestroyInstance(instance, nullptr);
	 std::cout << " Vulkan Instance destroyed." << std::endl;

	 return 0;


}

