#include <vulkan/vulkan.h>
#include <iostream>

int main()
{
   //vulkan triangle rendering

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Renderer";
	appInfo.apiVersion = VK_API_VERSION_1_0;
	//DEFINING A STRUCTURE THAT DESCRIBE THE APPLICATION TO VULKAN 
	//VULKAN USE U STYPE TO IDENTIFY STRUCT TYPE 
	//{} IT INITIALIZE ALL THE FEILD TO ZERO OR NULL 

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

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

	//Cleanup
	vkDestroyInstance(instance, nullptr);
	return 0;

}

