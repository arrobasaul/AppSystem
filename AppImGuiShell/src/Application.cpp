#include "Application.h"

//
// Adapted from Dear ImGui Vulkan example
//

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <stdio.h>	// printf, fprintf
#include <stdlib.h> // abort
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <iostream>
#include <algorithm>

// Emedded font
#include "ImGui/Roboto-Regular.embed"

extern bool g_ApplicationRunning;

ImGuiID dock_id_top, dock_id_right, dock_id_right2;
// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

static VkAllocationCallbacks *g_Allocator = NULL;
static VkInstance g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice g_Device = VK_NULL_HANDLE;
static uint32_t g_QueueFamily = (uint32_t)-1;
static VkQueue g_Queue = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static int g_MinImageCount = 2;
static bool g_SwapChainRebuild = false;

// Per-frame-in-flight
static std::vector<std::vector<VkCommandBuffer>> s_AllocatedCommandBuffers;
static std::vector<std::vector<std::function<void()>>> s_ResourceFreeQueue;

// Unlike g_MainWindowData.FrameIndex, this is not the the swapchain image index
// and is always guaranteed to increase (eg. 0, 1, 2, 0, 1, 2)
static uint32_t s_CurrentFrameIndex = 0;

static AppSystem::Application *s_Instance = nullptr;

void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

#ifdef IMGUI_VULKAN_DEBUG_REPORT
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData)
{
	(void)flags;
	(void)object;
	(void)location;
	(void)messageCode;
	(void)pUserData;
	(void)pLayerPrefix; // Unused arguments
	fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
	return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT


void StyleColors() 
{
	ImGui::StyleColorsLight();
	ImGuiStyle& style = ImGui::GetStyle();

	// light style from Pac�me Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;
	style.WindowRounding = 3.f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	//style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	//style.Colors[ImGuiCol_Header] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	//style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	float alpha_ = 0.95f;
	if (false) //dark
	{
		for (int i = 0; i <= ImGuiCol_COUNT; i++)
		{
			ImVec4& col = style.Colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if (S < 0.1f)
			{
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
			if (col.w < 1.00f)
			{
				col.w *= alpha_;
			}
		}
	}
	else
	{
		for (int i = 0; i <= ImGuiCol_COUNT; i++)
		{
			ImVec4& col = style.Colors[i];
			if (col.w < 1.00f)
			{
				col.x *= alpha_;
				col.y *= alpha_;
				col.z *= alpha_;
				col.w *= alpha_;
			}
		}
	}
}
static void SetupVulkan(const char **extensions, uint32_t extensions_count)
{
	VkResult err;

	// Create Vulkan Instance
	{
		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.enabledExtensionCount = extensions_count;
		create_info.ppEnabledExtensionNames = extensions;
#ifdef IMGUI_VULKAN_DEBUG_REPORT
		// Enabling validation layers
		const char *layers[] = {"VK_LAYER_KHRONOS_validation"};
		create_info.enabledLayerCount = 1;
		create_info.ppEnabledLayerNames = layers;

		// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
		const char **extensions_ext = (const char **)malloc(sizeof(const char *) * (extensions_count + 1));
		memcpy(extensions_ext, extensions, extensions_count * sizeof(const char *));
		extensions_ext[extensions_count] = "VK_EXT_debug_report";
		create_info.enabledExtensionCount = extensions_count + 1;
		create_info.ppEnabledExtensionNames = extensions_ext;

		// Create Vulkan Instance
		err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
		check_vk_result(err);
		free(extensions_ext);

		// Get the function pointer (required for any extensions)
		auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT");
		IM_ASSERT(vkCreateDebugReportCallbackEXT != NULL);

		// Setup the debug report callback
		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = debug_report;
		debug_report_ci.pUserData = NULL;
		err = vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci, g_Allocator, &g_DebugReport);
		check_vk_result(err);
#else
		// Create Vulkan Instance without any debug feature
		err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
		check_vk_result(err);
		IM_UNUSED(g_DebugReport);
#endif
	}

	// Select GPU
	{
		uint32_t gpu_count;
		err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, NULL);
		check_vk_result(err);
		IM_ASSERT(gpu_count > 0);

		VkPhysicalDevice *gpus = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * gpu_count);
		err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus);
		check_vk_result(err);

		// If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
		// most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
		// dedicated GPUs) is out of scope of this sample.
		int use_gpu = 0;
		for (int i = 0; i < (int)gpu_count; i++)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(gpus[i], &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				use_gpu = i;
				break;
			}
		}

		g_PhysicalDevice = gpus[use_gpu];
		free(gpus);
	}

	// Select graphics queue family
	{
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, NULL);
		VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * count);
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
		for (uint32_t i = 0; i < count; i++)
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				g_QueueFamily = i;
				break;
			}
		free(queues);
		IM_ASSERT(g_QueueFamily != (uint32_t)-1);
	}

	// Create Logical Device (with 1 queue)
	{
		int device_extension_count = 1;
		const char *device_extensions[] = {"VK_KHR_swapchain"};
		const float queue_priority[] = {1.0f};
		VkDeviceQueueCreateInfo queue_info[1] = {};
		queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info[0].queueFamilyIndex = g_QueueFamily;
		queue_info[0].queueCount = 1;
		queue_info[0].pQueuePriorities = queue_priority;
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		create_info.pQueueCreateInfos = queue_info;
		create_info.enabledExtensionCount = device_extension_count;
		create_info.ppEnabledExtensionNames = device_extensions;
		err = vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device);
		check_vk_result(err);
		vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
	}

	// Create Descriptor Pool
	{
		VkDescriptorPoolSize pool_sizes[] =
			{
				{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
				{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
				{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
				{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
				{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
				{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
				{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
				{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
		check_vk_result(err);
	}
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
static void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height)
{
	wd->Surface = surface;

	// Check for WSI support
	VkBool32 res;
	vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
	if (res != VK_TRUE)
	{
		fprintf(stderr, "Error no WSI support on physical device 0\n");
		exit(-1);
	}

	// Select Surface Format
	const VkFormat requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
	const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

	// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
	VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
#else
	VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
	wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	// printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

	// Create SwapChain, RenderPass, Framebuffer, etc.
	IM_ASSERT(g_MinImageCount >= 2);
	ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
}

static void CleanupVulkan()
{
	vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
	// Remove the debug report callback
	auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugReportCallbackEXT");
	vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport, g_Allocator);
#endif // IMGUI_VULKAN_DEBUG_REPORT

	vkDestroyDevice(g_Device, g_Allocator);
	vkDestroyInstance(g_Instance, g_Allocator);
}

static void CleanupVulkanWindow()
{
	ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
}

static void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data)
{
	VkResult err;

	VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
	{
		g_SwapChainRebuild = true;
		return;
	}
	check_vk_result(err);

	s_CurrentFrameIndex = (s_CurrentFrameIndex + 1) % g_MainWindowData.ImageCount;

	ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
	{
		err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX); // wait indefinitely instead of periodically checking
		check_vk_result(err);

		err = vkResetFences(g_Device, 1, &fd->Fence);
		check_vk_result(err);
	}

	{
		// Free resources in queue
		for (auto &func : s_ResourceFreeQueue[s_CurrentFrameIndex])
			func();
		s_ResourceFreeQueue[s_CurrentFrameIndex].clear();
	}
	{
		// Free command buffers allocated by Application::GetCommandBuffer
		// These use g_MainWindowData.FrameIndex and not s_CurrentFrameIndex because they're tied to the swapchain image index
		auto &allocatedCommandBuffers = s_AllocatedCommandBuffers[wd->FrameIndex];
		if (allocatedCommandBuffers.size() > 0)
		{
			vkFreeCommandBuffers(g_Device, fd->CommandPool, (uint32_t)allocatedCommandBuffers.size(), allocatedCommandBuffers.data());
			allocatedCommandBuffers.clear();
		}

		err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
		check_vk_result(err);
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
		check_vk_result(err);
	}
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = wd->RenderPass;
		info.framebuffer = fd->Framebuffer;
		info.renderArea.extent.width = wd->Width;
		info.renderArea.extent.height = wd->Height;
		info.clearValueCount = 1;
		info.pClearValues = &wd->ClearValue;
		vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	// Record dear imgui primitives into command buffer
	ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

	// Submit command buffer
	vkCmdEndRenderPass(fd->CommandBuffer);
	{
		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &image_acquired_semaphore;
		info.pWaitDstStageMask = &wait_stage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &fd->CommandBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(fd->CommandBuffer);
		check_vk_result(err);
		err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
		check_vk_result(err);
	}
}

static void FramePresent(ImGui_ImplVulkanH_Window *wd)
{
	if (g_SwapChainRebuild)
		return;
	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_complete_semaphore;
	info.swapchainCount = 1;
	info.pSwapchains = &wd->Swapchain;
	info.pImageIndices = &wd->FrameIndex;
	VkResult err = vkQueuePresentKHR(g_Queue, &info);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
	{
		g_SwapChainRebuild = true;
		return;
	}
	check_vk_result(err);
	wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
}

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

namespace AppSystem
{

	Application::Application(const ApplicationSpecification &specification)
		: m_Specification(specification)
	{
		s_Instance = this;

		Init();
	}
	void Application::DoOpenFile(const std::string& path)
	{
		File file;
		file.fname = path;
		file.time[0] = fs::last_write_time(file.fname);
		std::error_code err;
		file.time[1] = fs::last_write_time(file.codeGen.AltFName(file.fname), err);
		file.rootNode = file.codeGen.Import(file.fname, messageBox.error);
		if (!file.rootNode) {
			messageBox.title = "CodeGen";
			messageBox.message = "Unsuccessful import because of errors";
			messageBox.buttons = ImRad::Ok;
			messageBox.OpenPopup();
			return;
		}

		auto it = stx::find_if(fileTabs, [&](const File& f) { return f.fname == file.fname; });
		if (it == fileTabs.end()) {
			fileTabs.push_back({});
			it = fileTabs.begin() + fileTabs.size() - 1;
		}
		int idx = int(it - fileTabs.begin());
		fileTabs[idx] = std::move(file);
		ActivateTab(idx);

		if (messageBox.error != "") {
			messageBox.title = "CodeGen";
			messageBox.message = "Import finished with errors";
			messageBox.buttons = ImRad::Ok;
			messageBox.OpenPopup();
		}
	}
	void Application::OpenFile()
	{
		nfdchar_t *outPath = NULL;
		// nfdresult_t result = NFD_OpenDialog("h", NULL, &outPath);
		//if (result != NFD_OKAY)
		//	return;
		
		ctx.snapMode = false;
		ctx.selected.clear();
		auto it = stx::find_if(fileTabs, [&](const File& f) { return f.fname == outPath; });
		if (it != fileTabs.end()) 
		{
			ActivateTab(int(it - fileTabs.begin()));
			if (it->modified) {
				messageBox.message = "Reload and lose unsaved changes?";
				messageBox.error = "";
				messageBox.buttons = ImRad::Yes | ImRad::No;
				messageBox.OpenPopup([=](ImRad::ModalResult mr) {
					if (mr == ImRad::Yes)
						DoOpenFile(outPath);
					});
			}
			else {
				DoOpenFile(outPath);
			}
		}
		else {
			DoOpenFile(outPath);
		}
	}

	void Application::ReloadFiles()
	{
		for (auto& tab : fileTabs)
		{
			if (tab.fname == "")
				continue;
			auto time1 = fs::last_write_time(tab.fname);
			std::error_code err;
			auto time2 = fs::last_write_time(tab.codeGen.AltFName(tab.fname), err);
			if (time1 == tab.time[0] && time2 == tab.time[1])
				continue;
			tab.time[0] = time1;
			tab.time[1] = time2;
			auto fn = fs::path(tab.fname).filename().string();
			messageBox.message = "File content of '" + fn + "' has changed. Reload?";
			messageBox.error = "";
			messageBox.buttons = ImRad::Yes | ImRad::No;
			messageBox.OpenPopup([&](ImRad::ModalResult mr) {
				if (mr != ImRad::Yes)
					return;
				tab.rootNode = tab.codeGen.Import(tab.fname, messageBox.error);
				tab.modified = false;
				if (&tab == &fileTabs[activeTab])
					ctx.snapMode = false;
					ctx.selected.clear();
				});
		}
	}

	void Application::DoCloseFile()
	{
		ctx.selected.clear();
		fileTabs.erase(fileTabs.begin() + activeTab);
		ActivateTab(activeTab);
	}

	void Application::CloseFile()
	{
		if (activeTab < 0)
			return;
		if (fileTabs[activeTab].modified) {
			messageBox.title = "Confirmation";
			std::string fname = fs::path(fileTabs[activeTab].fname).filename().string();
			if (fname.empty())
				fname = UNTITLED;
			messageBox.message = "Save changes to " + fname + "?";
			messageBox.buttons = ImRad::Yes | ImRad::No | ImRad::Cancel;
			messageBox.OpenPopup([=](ImRad::ModalResult mr) {
				if (mr == ImRad::Yes)
					SaveFile(true);
				else if (mr == ImRad::No)
					DoCloseFile();
				else
					glfwSetWindowShouldClose(m_WindowHandle, false);
				});
		}
		else {
			DoCloseFile();
		}
	}
	void Application::SaveFile(bool thenClose)
	{
		auto& tab = fileTabs[activeTab];
		bool trunc = false;
		if (tab.fname == "") {
			nfdchar_t *outPath = NULL;
			/*nfdresult_t result = NFD_SaveDialog("h", NULL, &outPath);
			if (result != NFD_OKAY) {
				if (thenClose)
					DoCloseFile();
				return;
			}*/
			tab.fname = outPath;
			if (!fs::path(tab.fname).has_extension())
				tab.fname += ".h";
			ctx.fname = tab.fname;
			tab.codeGen.SetNamesFromId(
				fs::path(tab.fname).stem().string());
			trunc = true;
		}
		
		if (!tab.codeGen.Export(tab.fname, trunc, tab.rootNode.get(), messageBox.error))
		{
			messageBox.title = "CodeGen";
			messageBox.message = "Unsuccessful export due to errors";
			messageBox.buttons = ImRad::Ok;
			messageBox.OpenPopup([=](ImRad::ModalResult) {
				if (thenClose)
					DoCloseFile();
				});
			return;
		}
		
		tab.modified = false;
		tab.time[0] = fs::last_write_time(tab.fname);
		std::error_code err;
		tab.time[1] = fs::last_write_time(tab.codeGen.AltFName(tab.fname), err);
		if (messageBox.error != "")
		{
			messageBox.title = "CodeGen";
			messageBox.message = "Export finished with errors";
			messageBox.buttons = ImRad::Ok;
			messageBox.OpenPopup([=](ImRad::ModalResult) {
				if (thenClose)
					DoCloseFile();
				});
			return;
		}

		if (thenClose)
			DoCloseFile();
	}

	void Application::ShowCode()
	{
		if (activeTab < 0)
			return;
		std::string path = (fs::temp_directory_path() / "imrad-preview.cpp").string();
		std::ofstream fout(path);
		ctx.ind = "";
		auto* root = fileTabs[activeTab].rootNode.get();
		root->Export(fout, ctx);
		
		if (ctx.errors.size()) {
			fout << "\n// Export finished with errors\n";
			for (const std::string& e : ctx.errors)
				fout << "// " << e <<  "\n";
		}
		fout.close();
	#ifdef WIN32
		//ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
	#else
		system(("xdg-open " + path).c_str());
	#endif
	}

	void Application::NewWidget(const std::string& name)
	{
		activeButton = name;
		if (name == "") {
			ctx.selected.clear();
			ctx.snapMode = false;
		}
		else {
			newNode = Widget::Create(name, ctx);
			ctx.selected.clear();
			ctx.snapMode = true;
		}
	}

	void Application::ActivateTab(int i)
	{
		if (i >= fileTabs.size())
			i = (int)fileTabs.size() - 1;
		if (i < 0) {
			activeTab = -1;
			ctx.selected.clear();
			ctx.codeGen = nullptr;
			return;
		}
		activeTab = i;
		auto& tab = fileTabs[i];
		ctx.selected = { tab.rootNode.get() };
		ctx.codeGen = &tab.codeGen;
		ctx.fname = tab.fname;
	}
	void Application::NewFile()
	{
		fileTabs.push_back({});
		fileTabs.back().rootNode = std::make_unique<TopWindow>(ctx);
		ActivateTab((int)fileTabs.size() - 1);
	}
	void Application::ToolbarUI()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 0));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, TB_SIZE));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = 0
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::Begin("TOOLBAR", nullptr, window_flags);
		ImGui::PopStyleVar();

		const float BTN_SIZE = 30;
		const auto& io = ImGui::GetIO();
		/*if (m_MenubarCallback)
		{
			if (ImGui::BeginMenuBar())
			{
				m_MenubarCallback();
				ImGui::EndMenuBar();
			}
		}*/
		if (m_MenubarCallback)
		{
			m_MenubarCallback();
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FILE) ||
			(ImGui::IsKeyPressed(ImGuiKey_N, false) && io.KeyCtrl)) 
			NewFile();
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("New File (Ctrl+N)");
		
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FOLDER_OPEN) ||
			(ImGui::IsKeyPressed(ImGuiKey_O, false) && io.KeyCtrl))
			OpenFile();
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("Open File (Ctrl+O)");
		
		ImGui::BeginDisabled(activeTab < 0);
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FLOPPY_DISK) ||
			(ImGui::IsKeyPressed(ImGuiKey_S, false) && io.KeyCtrl))
			SaveFile(false);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("Save File (Ctrl+S)");
		
		/*ImGui::SameLine();
		if (ImGui::Button(ICON_FA_WINDOW_RESTORE))
			SaveAll();
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("Save All (Ctrl+Shift+S)");*/
		
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
		ImGui::Text("Theme:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::Combo("##style", &styleIdx, "Dark\0Light\0Classic\0\0");
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		
		ImGui::SameLine();
		ImGui::BeginDisabled(activeTab < 0);
		if (ImGui::Button(ICON_FA_BOLT) || // ICON_FA_BOLT, ICON_FA_RIGHT_TO_BRACKET) ||
			(ImGui::IsKeyPressed(ImGuiKey_P, false) && io.KeyCtrl)) 
			ShowCode();
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("Preview Code (Ctrl+P)");
		ImGui::EndDisabled();
		
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_CUBES))
		{
			classWizard.codeGen = ctx.codeGen;
			classWizard.root = fileTabs[activeTab].rootNode.get();
			classWizard.OpenPopup();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("Class Wizard");
		ImGui::EndDisabled();
		
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_CIRCLE_INFO) ||
			(ImGui::IsKeyPressed(ImGuiKey_F1, false)))
		{
			aboutDlg.OpenPopup();
		}

		ImGui::End();

		float sp = ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetNextWindowPos({ viewport->Size.x - 520, 100 }, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize({ 140, 260 }, ImGuiCond_FirstUseEver);
		ImGui::Begin("Widgets", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
		int n = (int)std::max(1.f, ImGui::GetContentRegionAvail().x / (BTN_SIZE + sp));
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.5f, 0.5f });
		for (const auto& cat : tbButtons)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::CollapsingHeader(cat.first.c_str()))
			{
				int rows = (stx::ssize(cat.second) + n - 1) / n;
				float h = rows * BTN_SIZE + (rows - 1) * 5;
				ImGui::BeginChild(("cat" + cat.first).c_str(), { 0, h });
				ImGui::Columns(n, nullptr, false);
				for (const auto& tb : cat.second)
				{
					if (ImGui::Selectable(tb.label.c_str(), activeButton == tb.name, 0, ImVec2(BTN_SIZE, BTN_SIZE)))
						NewWidget(tb.name);
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) && tb.name != "")
						ImGui::SetTooltip(tb.name.c_str());

					ImGui::NextColumn();
				}
				ImGui::EndChild();
			}
		}
		ImGui::PopStyleVar();
		ImGui::End();
	}
	void Application::ToolbarUIApp()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 0));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, TB_SIZE));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = 0
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::Begin("TOOLBAR", nullptr, window_flags);
		ImGui::PopStyleVar();

		const float BTN_SIZE = 30;
		const auto& io = ImGui::GetIO();
		if (m_MenubarCallback)
		{
			m_MenubarCallback();
		}
		ImGui::SameLine();
		ImGui::End();
	}

	void Application::TabsUI()
	{
		/*ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + TB_SIZE));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, TAB_SIZE));
		ImGui::SetNextWindowViewport(viewport->ID);
		*/
		ImGuiWindowFlags window_flags = 0
			//| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			//| ImGuiWindowFlags_NoSavedSettings
			;
		/*bool tmp;
		ImGui::Begin("Moje.cpp", &tmp, window_flags);
		ImGui::End();
		ImGui::DockBuilderDockWindow("Tvoje.cpp", dock_id_top);
		ImGui::Begin("Tvoje.cpp", &tmp, window_flags);
		ImGui::End();
		*/
		bool notClosed = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 0.0f));
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
		ImGui::SetNextWindowClass(&window_class);
		ImGui::Begin("FileTabs", 0, window_flags);
		if (ImGui::BeginTabBar(".Tabs", ImGuiTabBarFlags_NoTabListScrollingButtons))
		{
			int untitled = 0;
			for (int i = 0; i < (int)fileTabs.size(); ++i)
			{
				const auto& tab = fileTabs[i];
				std::string fname = fs::path(tab.fname).filename().string();
				if (fname == "")
					fname = UNTITLED + std::to_string(++untitled);
				if (tab.modified)
					fname += " *";
				if (ImGui::BeginTabItem(fname.c_str(), &notClosed, i == activeTab ? ImGuiTabItemFlags_SetSelected : 0))
				{
					ImGui::EndTabItem();
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) && tab.fname != "")
					ImGui::SetTooltip(tab.fname.c_str());

				if (!i)
				{
					auto min = ImGui::GetItemRectMin();
					auto max = ImGui::GetItemRectMax();
					ctx.wpos.x = min.x + 20;
					ctx.wpos.y = max.y + 20;
					//const auto* viewport = ImGui::GetMainViewport();
					//ctx.wpos = viewport->GetCenter() + ImVec2(min.x, max.y) / 2;
				}
				if (!notClosed)
				{
					ActivateTab(i);
					CloseFile();
				}
				else if (ImGui::IsItemActivated() && i != activeTab)
				{
					ActivateTab(i);
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
	void Application::TabsUIApp()
	{
		
		
		ImGuiWindowFlags window_flags = 0
			//| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			//| ImGuiWindowFlags_NoSavedSettings
			;
		/*bool tmp;
		ImGui::Begin("Moje.cpp", &tmp, window_flags);
		ImGui::End();
		ImGui::DockBuilderDockWindow("Tvoje.cpp", dock_id_top);
		ImGui::Begin("Tvoje.cpp", &tmp, window_flags);
		ImGui::End();
		*/
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		/*ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + TB_SIZE));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, TAB_SIZE));
		ImGui::SetNextWindowViewport(viewport->ID);*/
		bool notClosed = true;
		// ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0.0f));
		ImGuiWindowClass window_class;
		// window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
		ImGui::SetNextWindowClass(&window_class);
		ImGui::Begin("FileTabs"); // , 0, window_flags);
		if (ImGui::BeginTabBar(".Tabs")) //, ImGuiTabBarFlags_NoTabListScrollingButtons))
		{
			int untitled = 0;
			for (int i = 0; i < (int)m_LayerStack.size(); ++i)
			{
				const auto& tab = m_LayerStack[i];
				if (ImGui::BeginTabItem("Modulo " + i, &notClosed, i == activeTab ? ImGuiTabItemFlags_SetSelected : 0))
				{
					
					
					// int fl = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings;
					
					//ImGui::Begin("Ventana " + i, &notClosed );
						tab->OnUIRender();
					//ImGui::End();
					ImGui::EndTabItem();
				}

				if (!i)
				{
					
					//const auto* viewport = ImGui::GetMainViewport();
					//ctx.wpos = viewport->GetCenter() + ImVec2(min.x, max.y) / 2;
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
		//ImGui::PopStyleVar();
	}

	void Application::HierarchyUI()
	{
		ImGui::Begin("Hierarchy");
		if (activeTab >= 0 && fileTabs[activeTab].rootNode) 
			fileTabs[activeTab].rootNode->TreeUI(ctx);
		ImGui::End();
	}
	void Application::HierarchyUIApp()
	{
		int layercCount = 0;
		ImGui::Begin("Hierarchy");
		for (auto& layer : m_LayerStack)
		{
			ImGui::Button("Boton" + layercCount);
			layercCount++;
		}
		
		ImGui::End();
	}

	void Application::PropertyRowsUI(bool pr)
	{
		int keyPressed = 0;
		if (addInputCharacter)
		{
			//select all text when activating
			auto* g = ImGui::GetCurrentContext();
			g->NavNextActivateFlags &= ~ImGuiActivateFlags_TryToPreserveState;
			g->NavActivateFlags &= ~ImGuiActivateFlags_TryToPreserveState;
			//and send a character to overwrite it
			ImGui::GetIO().AddInputCharacter(addInputCharacter);
			addInputCharacter = 0;
		}
		if (!ImGui::GetIO().WantTextInput && !(ImGui::GetIO().KeyMods & ~(1 << 13)))
		{
			for (int key = ImGuiKey_A; key <= ImGuiKey_Z; ++key)
				if (ImGui::IsKeyPressed((ImGuiKey)key)) {
					keyPressed = key - ImGuiKey_A + (ImGui::GetIO().KeyShift ? 'A' : 'a');
					break;
				}
		}

		ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
		if (ImGui::BeginTable(pr ? "pg" : "pge", 2, flags))
		{
			ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);

			//determine common properties for a selection set
			std::vector<std::string_view> pnames;
			std::vector<std::string> pvals;
			for (auto* node : ctx.selected)
			{
				std::vector<std::string_view> pn;
				auto props = pr ? node->Properties() : node->Events();
				for (auto& p : props)
					pn.push_back(p.name);
				stx::sort(pn);
				if (node == ctx.selected[0])
					pnames = std::move(pn);
				else {
					std::vector<std::string_view> pres;
					stx::set_intersection(pnames, pn, std::back_inserter(pres));
					pnames = std::move(pres);
				}
			}
			//edit first widget
			auto props = pr ? ctx.selected[0]->Properties() : ctx.selected[0]->Events();
			std::string_view pname;
			std::string pval;
			ImGui::Indent(); //to align TreeNodes in the table
			for (int i = 0; i < (int)props.size(); ++i)
			{
				if (!stx::count(pnames, props[i].name))
					continue;
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				if (keyPressed && props[i].kbdInput) {
					addInputCharacter = keyPressed;
					ImGui::SetKeyboardFocusHere();
				}
				bool change = pr ? ctx.selected[0]->PropertyUI(i, ctx) : ctx.selected[0]->EventUI(i, ctx);
				if (change) {
					fileTabs[activeTab].modified = true;
					if (props[i].property) {
						pname = props[i].name;
						pval = props[i].property->to_arg();
					}
				}
			}
			ImGui::Unindent();
			ImGui::EndTable();

			//copy changes to other widgets
			for (size_t i = 1; i < ctx.selected.size(); ++i)
			{
				auto props = pr ? ctx.selected[i]->Properties() : ctx.selected[i]->Events();
				for (auto& p : props)
				{
					if (p.name == pname) {
						auto prop = const_cast<property_base*>(p.property);
						prop->set_from_arg(pval);
					}
				}
			}
		}
	}
	void Application::DerechosApp()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("Derechos");
			ImGui::Text("Derechos reservados");
		ImGui::End();

		ImGui::PopStyleVar();
	}
	void Application::PropertyUI()
	{
		bool tmp = false;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("Events");
		if (!ctx.selected.empty())
		{
			PropertyRowsUI(0);
		}
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
			tmp = true;
		ImGui::End();

		ImGui::Begin("Properties");
		if (!ctx.selected.empty())
		{
			PropertyRowsUI(1);
		}
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
			tmp = true;
		ImGui::End();
		ImGui::PopStyleVar();
		pgFocused = tmp;
	}

	void Application::PopupUI()
	{
		newFieldPopup.Draw();

		tableColumns.Draw();

		comboDlg.Draw();

		messageBox.Draw();

		classWizard.Draw();

		aboutDlg.Draw();

		bindingDlg.Draw();
	}

	void Application::Draw()
	{
		if (activeTab >= 0 && fileTabs[activeTab].rootNode)
		{
			auto tmpStyle = ImGui::GetStyle();
			switch (styleIdx) {
				case 0: ImGui::StyleColorsDark(); break;
				case 1: ImGui::StyleColorsLight(); break;
				case 2: ImGui::StyleColorsClassic(); break;
			}
			ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive];

			fileTabs[activeTab].rootNode->Draw(ctx);
		
			ImGui::GetStyle() = tmpStyle;
		}
	}

	std::pair<UINode*, size_t> Application::FindParentIndex(UINode* node, const UINode* n)
	{
		for (size_t i = 0; i < node->children.size(); ++i)
		{
			if (node->children[i].get() == n)
				return { node, i };
			auto pi = FindParentIndex(node->children[i].get(), n);
			if (pi.first)
				return pi;
		}
		return { nullptr, 0 };
	}

	void Application::Work()
	{
		if (ImGui::GetTopMostAndVisiblePopupModal())
			return;

		if (ctx.snapMode)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				ctx.snapMode = false;
				activeButton = "";
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && 
				ctx.snapParent)
			{
				ctx.selected = { newNode.get() };
				newNode->sameLine = ctx.snapSameLine;
				if (newNode->sameLine)
					newNode->spacing = 1;
				newNode->nextColumn = ctx.snapNextColumn;
				newNode->beginGroup = ctx.snapBeginGroup;
				if (ctx.snapIndex < ctx.snapParent->children.size())
				{
					auto& next = ctx.snapParent->children[ctx.snapIndex];
					if (ctx.snapSetNextSameLine) {
						next->beginGroup = false; 
						next->nextColumn = false;
						next->sameLine = true;
						next->spacing = std::max((int)next->spacing, 1);
					}
					if (next->sameLine)
						next->indent = 0; //otherwise creates widgets overlaps
				}
				ctx.snapParent->children.insert(ctx.snapParent->children.begin() + ctx.snapIndex, std::move(newNode));
				
				ctx.snapMode = false;
				activeButton = "";
				fileTabs[activeTab].modified = true;
			}
		}
		else if (!ctx.selected.empty())
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !pgFocused)
			{
				fileTabs[activeTab].modified = true;
				auto pi1 = FindParentIndex(fileTabs[activeTab].rootNode.get(), ctx.selected[0]);
				for (UINode* node : ctx.selected)
				{
					if (node == fileTabs[activeTab].rootNode.get())
						continue;
					auto pi = FindParentIndex(fileTabs[activeTab].rootNode.get(), node);
					if (!pi.first)
						continue;
					Widget* wdg = dynamic_cast<Widget*>(node);
					bool sameLine = wdg->sameLine;
					bool nextColumn = wdg->nextColumn;
					bool beginGroup = wdg->beginGroup;
					pi.first->children.erase(pi.first->children.begin() + pi.second);
					if (pi.second < pi.first->children.size())
					{
						wdg = dynamic_cast<Widget*>(pi.first->children[pi.second].get());
						if (nextColumn)
							wdg->nextColumn = true;
						if (!sameLine)
							wdg->sameLine = false;
						if (beginGroup)
							wdg->beginGroup = true;
					}
				}
				//move selection. Useful for things like menu items
				if (ctx.selected.size() == 1 && pi1.first)
				{
					if (pi1.second < pi1.first->children.size())
						ctx.selected[0] = pi1.first->children[pi1.second].get();
					else if (pi1.second)
						ctx.selected[0] = pi1.first->children[pi1.second - 1].get();
					else
						ctx.selected[0] = pi1.first;
				}
				else
				{
					ctx.selected.clear();
				}
			}
		}
	}
	Application::~Application()
	{
		Shutdown();

		s_Instance = nullptr;
	}

	Application &Application::Get()
	{
		return *s_Instance;
	}

	void Application::Init()
	{
		// Setup GLFW window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
		{
			std::cerr << "Could not initalize GLFW!\n";
			return;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_WindowHandle = glfwCreateWindow(m_Specification.Width, m_Specification.Height, m_Specification.Name.c_str(), NULL, NULL);

		// Setup Vulkan
		if (!glfwVulkanSupported())
		{
			std::cerr << "GLFW: Vulkan not supported!\n";
			return;
		}
		uint32_t extensions_count = 0;
		const char **extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
		SetupVulkan(extensions, extensions_count);

		// Create Window Surface
		VkSurfaceKHR surface;
		VkResult err = glfwCreateWindowSurface(g_Instance, m_WindowHandle, g_Allocator, &surface);
		check_vk_result(err);

		// Create Framebuffers
		int w, h;
		glfwGetFramebufferSize(m_WindowHandle, &w, &h);
		ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;
		SetupVulkanWindow(wd, surface, w, h);

		s_AllocatedCommandBuffers.resize(wd->ImageCount);
		s_ResourceFreeQueue.resize(wd->ImageCount);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	// Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
		// io.ConfigViewportsNoAutoMerge = true;
		// io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle &style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = g_Instance;
		init_info.PhysicalDevice = g_PhysicalDevice;
		init_info.Device = g_Device;
		init_info.QueueFamily = g_QueueFamily;
		init_info.Queue = g_Queue;
		init_info.PipelineCache = g_PipelineCache;
		init_info.DescriptorPool = g_DescriptorPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = g_MinImageCount;
		init_info.ImageCount = wd->ImageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = g_Allocator;
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);

		// Load default font
		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		ImFont *robotoFont = io.Fonts->AddFontFromMemoryTTF((void *)g_RobotoRegular, sizeof(g_RobotoRegular), 20.0f, &fontConfig);
		io.FontDefault = robotoFont;

		ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig icons_config; 
		icons_config.MergeMode = true; 
		icons_config.PixelSnapH = true;
		io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAR, 18.0f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 18.0f, &icons_config, icons_ranges);
		// Upload Fonts
		{
			// Use any command queue
			VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
			VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

			err = vkResetCommandPool(g_Device, command_pool, 0);
			check_vk_result(err);
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(command_buffer, &begin_info);
			check_vk_result(err);

			ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			err = vkEndCommandBuffer(command_buffer);
			check_vk_result(err);
			err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
			check_vk_result(err);

			err = vkDeviceWaitIdle(g_Device);
			check_vk_result(err);
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}

	void Application::Shutdown()
	{
		for (auto &layer : m_LayerStack)
			dynamic_cast<Service*>(layer.get())->OnDetach();

		m_LayerStack.clear();

		// Cleanup
		VkResult err = vkDeviceWaitIdle(g_Device);
		check_vk_result(err);

		// Free resources in queue
		for (auto &queue : s_ResourceFreeQueue)
		{
			for (auto &func : queue)
				func();
		}
		s_ResourceFreeQueue.clear();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		CleanupVulkanWindow();
		CleanupVulkan();

		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();

		g_ApplicationRunning = false;
	}

	void Application::Run()
	{
		m_Running = true;

		ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGuiIO &io = ImGui::GetIO();
		StyleColors();
		// NewFile();
		firstTime = true;
		bool lastVisible = true;
		// Main loop
		while (!glfwWindowShouldClose(m_WindowHandle) && m_Running)
		{
			// Poll and handle events (inputs, window resize, etc.)
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			glfwPollEvents();

			// for (auto &layer : m_LayerStack)
				// layer->OnUpdate(m_TimeStep);

			// Resize swap chain?
			if (g_SwapChainRebuild)
			{
				int width, height;
				glfwGetFramebufferSize(m_WindowHandle, &width, &height);
				if (width > 0 && height > 0)
				{
					ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
					ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
					g_MainWindowData.FrameIndex = 0;

					// Clear allocated command buffers from here since entire pool is destroyed
					s_AllocatedCommandBuffers.clear();
					s_AllocatedCommandBuffers.resize(g_MainWindowData.ImageCount);

					g_SwapChainRebuild = false;
				}
			}
			
			// Start the Dear ImGui frame
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			bool visible = glfwGetWindowAttrib(m_WindowHandle, GLFW_FOCUSED);
			if (visible && !lastVisible)
				ReloadFiles();
			lastVisible = visible;
			{
				static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
				// static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

				// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
				// because it would be confusing to have two docking targets within each others.
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
				/*if (m_MenubarCallback)
					window_flags |= ImGuiWindowFlags_MenuBar;*/

				const ImGuiViewport *viewport = ImGui::GetMainViewport();
				// ImGui::SetNextWindowPos(viewport->WorkPos);
				ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, TB_SIZE + 0));
				// ImGui::SetNextWindowSize(viewport->WorkSize);
				ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, TB_SIZE + 0));
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

				// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
				// and handle the pass-thru hole, so we ask Begin() to not render a background.
				if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				{
					dockspace_flags |= ImGuiDockNodeFlags_DockSpace;
					window_flags |= ImGuiWindowFlags_NoBackground;
				}

				// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
				// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
				// all active windows docked into it will lose their parent and become undocked.
				// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
				// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::Begin("DockSpace", nullptr, window_flags);
				ImGui::PopStyleVar();

				ImGui::PopStyleVar(2);

				// Submit the DockSpace
				ImGuiIO &io = ImGui::GetIO();
				//if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
				//{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);
				//}

				/*if (m_MenubarCallback)
				{
					if (ImGui::BeginMenuBar())
					{
						m_MenubarCallback();
						ImGui::EndMenuBar();
					}
				}*/

				// for (auto &layer : m_LayerStack)
					// layer->OnUIRender();
				if (firstTime)
				{
					ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
					ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
					ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
					// ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 350.f / viewport->Size.x, nullptr, &dockspace_id);
					// ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 300.f / (viewport->Size.x - 350), nullptr, &dockspace_id);
					ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 300.f / (viewport->Size.x - 350), nullptr, &dockspace_id);
					// ImGuiID dock_id_right1, dock_id_right2;
					// ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 230.f / (viewport->Size.y - TB_SIZE), &dock_id_right1, &dock_id_right2);
					float vh = viewport->Size.y - TB_SIZE;
					dock_id_top = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, viewport->Size.x, nullptr, &dockspace_id);
					// ImGuiID dock_id_down;
					// dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, viewport->Size.x, nullptr, &dockspace_id);
					ImGui::DockBuilderDockWindow("FileTabs", dock_id_top);
					ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
					// ImGui::DockBuilderDockWindow("Widgets", dock_id_right1);
					// ImGui::DockBuilderDockWindow("Properties", dock_id_right2);
					// ImGui::DockBuilderDockWindow("Events", dock_id_right2);
					// ImGui::DockBuilderDockWindow("Derechos", dock_id_down);
					ImGui::DockBuilderFinish(dockspace_id);
				}
				ImGui::End();
			}
			// ToolbarUI();
			ToolbarUIApp();
			// TabsUI();
			TabsUIApp();
			// HierarchyUI();
			HierarchyUIApp();
			// DerechosApp();
			// PropertyUI();
			// PopupUI();
			// Draw();
			// Work();
			firstTime = false;
			// Rendering
			ImGui::Render();

			ImDrawData *main_draw_data = ImGui::GetDrawData();
			const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
			wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
			wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
			wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
			wd->ClearValue.color.float32[3] = clear_color.w;
			if (!main_is_minimized)
				FrameRender(wd, main_draw_data);

			// Update and Render additional Platform Windows
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			// Present Main Platform Window
			if (!main_is_minimized)
				FramePresent(wd);

			float time = GetTime();
			m_FrameTime = time - m_LastFrameTime;
			m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
			m_LastFrameTime = time;
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

	float Application::GetTime()
	{
		return (float)glfwGetTime();
	}

	VkInstance Application::GetInstance()
	{
		return g_Instance;
	}

	VkPhysicalDevice Application::GetPhysicalDevice()
	{
		return g_PhysicalDevice;
	}

	VkDevice Application::GetDevice()
	{
		return g_Device;
	}

	VkCommandBuffer Application::GetCommandBuffer(bool begin)
	{
		ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;

		// Use any command queue
		VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;

		VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
		cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocateInfo.commandPool = command_pool;
		cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocateInfo.commandBufferCount = 1;

		VkCommandBuffer &command_buffer = s_AllocatedCommandBuffers[wd->FrameIndex].emplace_back();
		auto err = vkAllocateCommandBuffers(g_Device, &cmdBufAllocateInfo, &command_buffer);

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(command_buffer, &begin_info);
		check_vk_result(err);

		return command_buffer;
	}

	void Application::FlushCommandBuffer(VkCommandBuffer commandBuffer)
	{
		const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &commandBuffer;
		auto err = vkEndCommandBuffer(commandBuffer);
		check_vk_result(err);

		// Create fence to ensure that the command buffer has finished executing
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		VkFence fence;
		err = vkCreateFence(g_Device, &fenceCreateInfo, nullptr, &fence);
		check_vk_result(err);

		err = vkQueueSubmit(g_Queue, 1, &end_info, fence);
		check_vk_result(err);

		err = vkWaitForFences(g_Device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
		check_vk_result(err);

		vkDestroyFence(g_Device, fence, nullptr);
	}

	void Application::SubmitResourceFree(std::function<void()> &&func)
	{
		s_ResourceFreeQueue[s_CurrentFrameIndex].emplace_back(func);
	}

}
