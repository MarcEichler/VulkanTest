# VulkanTest

Test project for the Vulkan API, following the book *Learning Vulkan* by 
Parminder Singh.

## Build
The project requires the 
[LunarG Vulkan SDK](https://www.lunarg.com/vulkan-sdk/).
Download and install the SDK to a folder of your choice 
(preferably ~/VulkanSDK).

Then run
```
git clone https://github.com/MarcEichler/VulkanTest.git
cd VulkanTest
```

and

`cmake` or `cmake -DVULKAN_SDK=<path to Vulkan SDK>` if you unpacked the 
SDK to a folder other than ~/VulkanSDK.