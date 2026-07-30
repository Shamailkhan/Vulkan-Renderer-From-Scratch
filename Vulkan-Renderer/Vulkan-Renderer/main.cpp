#include <vulkan/vulkan.h>
#include <iostream>
#include<vector>
#include<string>

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
int main()
{
   //vulkan triangle rendering

	/////////////STEP 1///////////////////////
	//DEFINING A STRUCTURE THAT DESCRIBE THE APPLICATION TO VULKAN 
	//VULKAN USE U STYPE TO IDENTIFY STRUCT TYPE 
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



	//Cleanup
	vkDestroyInstance(instance, nullptr);
	return 0;

}

