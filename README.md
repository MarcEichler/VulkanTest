# VulkanTest

Test project for the Vulkan API, following the book *Learning Vulkan* by 
Parminder Singh.

## Requirements
The project requires the Vulkan headers:

**Arch Linux**
Install the Vulkan development group (*vulkan-devel*)


The project also requires the [LunarG Vulkan SDK](https://www.lunarg.com/vulkan-sdk/).
Download and install the SDK to a folder of your choice 
(preferably ~/VulkanSDK). 

## Build
After installing the dependencies, run
```
git clone https://github.com/MarcEichler/VulkanTest.git
cd VulkanTest
```

and

`cmake` or `cmake -DVULKAN_SDK=<path to Vulkan SDK>` if you unpacked the 
SDK to a folder other than ~/VulkanSDK.