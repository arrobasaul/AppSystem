#include "Image.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"

#include "Application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace AppSystem
{

	namespace Utils
	{

		static uint32_t GetVulkanMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits)
		{
			VkPhysicalDeviceMemoryProperties prop;
			vkGetPhysicalDeviceMemoryProperties(Application::GetPhysicalDevice(), &prop);
			for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
			{
				if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
					return i;
			}

			return 0xffffffff;
		}

		static uint32_t BytesPerPixel(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA:
				return 4;
			case ImageFormat::RGBA32F:
				return 16;
			}
			return 0;
		}

		static VkFormat WalnutFormatToVulkanFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA:
				return VK_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::RGBA32F:
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			return (VkFormat)0;
		}

	}

	Image::Image(std::string_view path)
		: m_Filepath(path)
	{
		int width, height, channels;
		uint8_t *data = nullptr;

		if (stbi_is_hdr(m_Filepath.c_str()))
		{
			data = (uint8_t *)stbi_loadf(m_Filepath.c_str(), &width, &height, &channels, 4);
			m_Format = ImageFormat::RGBA32F;
		}
		else
		{
			data = stbi_load(m_Filepath.c_str(), &width, &height, &channels, 4);
			m_Format = ImageFormat::RGBA;
		}

		m_Width = width;
		m_Height = height;

		AllocateMemory(m_Width * m_Height * Utils::BytesPerPixel(m_Format));
		SetData(data);
	}

	Image::Image(uint32_t width, uint32_t height, ImageFormat format, const void *data)
		: m_Width(width), m_Height(height), m_Format(format)
	{
		AllocateMemory(m_Width * m_Height * Utils::BytesPerPixel(m_Format));
		if (data)
			SetData(data);
	}

	Image::~Image()
	{
		Release();
	}

	void Image::AllocateMemory(uint64_t size)
	{
		VkDevice device = Application::GetDevice();

		VkResult err;

		VkFormat vulkanFormat = Utils::WalnutFormatToVulkanFormat(m_Format);

		// Create the Image
		{
			VkImageCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.imageType = VK_IMAGE_TYPE_2D;
			info.format = vulkanFormat;
			info.extent.width = m_Width;
			info.extent.height = m_Height;
			info.extent.depth = 1;
			info.mipLevels = 1;
			info.arrayLayers = 1;
			info.samples = VK_SAMPLE_COUNT_1_BIT;
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			err = vkCreateImage(device, &info, nullptr, &m_Image);
			check_vk_result(err);
			VkMemoryRequirements req;
			vkGetImageMemoryRequirements(device, m_Image, &req);
			VkMemoryAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = req.size;
			alloc_info.memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
			err = vkAllocateMemory(device, &alloc_info, nullptr, &m_Memory);
			check_vk_result(err);
			err = vkBindImageMemory(device, m_Image, m_Memory, 0);
			check_vk_result(err);
		}

		// Create the Image View:
		{
			VkImageViewCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.image = m_Image;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = vulkanFormat;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.layerCount = 1;
			err = vkCreateImageView(device, &info, nullptr, &m_ImageView);
			check_vk_result(err);
		}

		// Create sampler:
		{
			VkSamplerCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			info.magFilter = VK_FILTER_LINEAR;
			info.minFilter = VK_FILTER_LINEAR;
			info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.minLod = -1000;
			info.maxLod = 1000;
			info.maxAnisotropy = 1.0f;

			info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			info.magFilter = VK_FILTER_LINEAR;
			info.minFilter = VK_FILTER_LINEAR;
			info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			info.anisotropyEnable = VK_FALSE;
			info.maxAnisotropy = 1.0f;
			info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			info.unnormalizedCoordinates = VK_FALSE;
			info.compareEnable = VK_FALSE;
			info.compareOp = VK_COMPARE_OP_ALWAYS;
			info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			info.mipLodBias = 0.0f;
			info.minLod = 0.0f;
			info.maxLod = 0.0f;
			VkResult err = vkCreateSampler(device, &info, nullptr, &m_Sampler);
			check_vk_result(err);
		}

		// Create the Descriptor Set:
		m_DescriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Image::Release()
	{
		Application::SubmitResourceFree([sampler = m_Sampler, imageView = m_ImageView, image = m_Image,
										 memory = m_Memory, stagingBuffer = m_StagingBuffer, stagingBufferMemory = m_StagingBufferMemory]()
										{
			VkDevice device = Application::GetDevice();

			vkDestroySampler(device, sampler, nullptr);
			vkDestroyImageView(device, imageView, nullptr);
			vkDestroyImage(device, image, nullptr);
			vkFreeMemory(device, memory, nullptr);
			vkDestroyBuffer(device, stagingBuffer, nullptr);
			vkFreeMemory(device, stagingBufferMemory, nullptr); });

		m_Sampler = nullptr;
		m_ImageView = nullptr;
		m_Image = nullptr;
		m_Memory = nullptr;
		m_StagingBuffer = nullptr;
		m_StagingBufferMemory = nullptr;
	}

	void Image::SetData(const void *data)
	{
		VkDevice device = Application::GetDevice();

		size_t upload_size = m_Width * m_Height * Utils::BytesPerPixel(m_Format);

		VkResult err;

		if (!m_StagingBuffer)
		{
			// Create the Upload Buffer
			{
				VkBufferCreateInfo buffer_info = {};
				buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				buffer_info.size = upload_size;
				buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				err = vkCreateBuffer(device, &buffer_info, nullptr, &m_StagingBuffer);
				check_vk_result(err);
				VkMemoryRequirements req;
				vkGetBufferMemoryRequirements(device, m_StagingBuffer, &req);
				m_AlignedSize = req.size;
				VkMemoryAllocateInfo alloc_info = {};
				alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				alloc_info.allocationSize = req.size;
				alloc_info.memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
				err = vkAllocateMemory(device, &alloc_info, nullptr, &m_StagingBufferMemory);
				check_vk_result(err);
				err = vkBindBufferMemory(device, m_StagingBuffer, m_StagingBufferMemory, 0);
				check_vk_result(err);
			}
		}

		// Upload to Buffer
		{
			char *map = NULL;
			err = vkMapMemory(device, m_StagingBufferMemory, 0, m_AlignedSize, 0, (void **)(&map));
			check_vk_result(err);
			memcpy(map, data, upload_size);
			VkMappedMemoryRange range[1] = {};
			range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range[0].memory = m_StagingBufferMemory;
			range[0].size = m_AlignedSize;
			err = vkFlushMappedMemoryRanges(device, 1, range);
			check_vk_result(err);
			vkUnmapMemory(device, m_StagingBufferMemory);
		}

		// Copy to Image
		{
			VkCommandBuffer command_buffer = Application::GetCommandBuffer(true);

			VkImageMemoryBarrier copy_barrier = {};
			copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier.image = m_Image;
			copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copy_barrier.subresourceRange.levelCount = 1;
			copy_barrier.subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copy_barrier);

			VkBufferImageCopy region = {};
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.layerCount = 1;
			region.imageExtent.width = m_Width;
			region.imageExtent.height = m_Height;
			region.imageExtent.depth = 1;
			vkCmdCopyBufferToImage(command_buffer, m_StagingBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			VkImageMemoryBarrier use_barrier = {};
			use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier.image = m_Image;
			use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			use_barrier.subresourceRange.levelCount = 1;
			use_barrier.subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &use_barrier);

			Application::FlushCommandBuffer(command_buffer);
		}
	}

	void Image::Resize(uint32_t width, uint32_t height)
	{
		if (m_Image && m_Width == width && m_Height == height)
			return;

		// TODO: max size?

		m_Width = width;
		m_Height = height;

		Release();
		AllocateMemory(m_Width * m_Height * Utils::BytesPerPixel(m_Format));
	}
	uint32_t Image::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDevice device = Application::GetPhysicalDevice();

		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(device, &mem_properties);

		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
			if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		return 0xFFFFFFFF; // Unable to find memoryType
	}
	bool Image::LoadTextureFromFile(const char* filename)
	{
		VkDevice device = Application::GetDevice();
		int width, height, channels = 4;
		
		// Specifying 4 channels forces stb to load the image in RGBA which is an easy format for Vulkan
		//tex_data->Channels = 4;
		unsigned char* image_data = stbi_load(filename, &width, &height, 0, channels);

		if (image_data == NULL)
			return false;

		// Calculate allocation size (in number of bytes)
		size_t image_size = width * height * channels;

		m_Width = width;
		m_Height = height;

		VkResult err;

		// Create the Vulkan image.
		{
			VkImageCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.imageType = VK_IMAGE_TYPE_2D;
			info.format = VK_FORMAT_R8G8B8A8_UNORM;
			info.extent.width = m_Width;
			info.extent.height = m_Height;
			info.extent.depth = 1;
			info.mipLevels = 1;
			info.arrayLayers = 1;
			info.samples = VK_SAMPLE_COUNT_1_BIT;
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			err = vkCreateImage(device, &info, nullptr, &m_Image);
			check_vk_result(err);
			VkMemoryRequirements req;
			vkGetImageMemoryRequirements(device, m_Image, &req);
			VkMemoryAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = req.size;
			alloc_info.memoryTypeIndex = findMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			err = vkAllocateMemory(device, &alloc_info, nullptr, &m_Memory);
			check_vk_result(err);
			err = vkBindImageMemory(device, m_Image, m_Memory, 0);
			check_vk_result(err);
		}

		// Create the Image View
		{
			VkImageViewCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.image = m_Image;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = VK_FORMAT_R8G8B8A8_UNORM;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.layerCount = 1;
			err = vkCreateImageView(device, &info, nullptr, &m_ImageView);
			check_vk_result(err);
		}

		// Create Sampler
		{
			VkSamplerCreateInfo sampler_info{};
			sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampler_info.magFilter = VK_FILTER_LINEAR;
			sampler_info.minFilter = VK_FILTER_LINEAR;
			sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // outside image bounds just use border color
			sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampler_info.minLod = -1000;
			sampler_info.maxLod = 1000;
			sampler_info.maxAnisotropy = 1.0f;
			err = vkCreateSampler(device, &sampler_info, nullptr, &m_Sampler);
			check_vk_result(err);
		}

		// Create Descriptor Set using ImGUI's implementation
		m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		// Create Upload Buffer
		{
			VkBufferCreateInfo buffer_info = {};
			buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_info.size = image_size;
			buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			err = vkCreateBuffer(device, &buffer_info, nullptr, &m_StagingBuffer);
			check_vk_result(err);
			VkMemoryRequirements req;
			vkGetBufferMemoryRequirements(device, m_StagingBuffer, &req);
			VkMemoryAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = req.size;
			alloc_info.memoryTypeIndex = findMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			err = vkAllocateMemory(device, &alloc_info, nullptr, &m_StagingBufferMemory);
			check_vk_result(err);
			err = vkBindBufferMemory(device, m_StagingBuffer, m_StagingBufferMemory, 0);
			check_vk_result(err);
		}

		// Upload to Buffer:
		{
			void* map = NULL;
			err = vkMapMemory(device, m_StagingBufferMemory, 0, image_size, 0, &map);
			check_vk_result(err);
			memcpy(map, image_data, image_size);
			VkMappedMemoryRange range[1] = {};
			range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range[0].memory = m_StagingBufferMemory;
			range[0].size = image_size;
			err = vkFlushMappedMemoryRanges(device, 1, range);
			check_vk_result(err);
			vkUnmapMemory(device, m_StagingBufferMemory);
		}

		// Release image memory using stb
		stbi_image_free(image_data);

		// Create a command buffer that will perform following steps when hit in the command queue.
		// TODO: this works in the example, but may need input if this is an acceptable way to access the pool/create the command buffer.
		
		/*VkCommandPool command_pool = g_MainWindowData.Frames[g_MainWindowData.FrameIndex].CommandPool;
		VkCommandBuffer command_buffer;
		{
			VkCommandBufferAllocateInfo alloc_info{};
			alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			alloc_info.commandPool = command_pool;
			alloc_info.commandBufferCount = 1;

			err = vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);
			check_vk_result(err);

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(command_buffer, &begin_info);
			check_vk_result(err);
		}*/
		VkCommandBuffer command_buffer = Application::GetCommandBuffer(true);
		// Copy to Image
		{
			VkImageMemoryBarrier copy_barrier[1] = {};
			copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier[0].image = m_Image;
			copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copy_barrier[0].subresourceRange.levelCount = 1;
			copy_barrier[0].subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);

			VkBufferImageCopy region = {};
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.layerCount = 1;
			region.imageExtent.width = m_Width;
			region.imageExtent.height = m_Height;
			region.imageExtent.depth = 1;
			vkCmdCopyBufferToImage(command_buffer, m_StagingBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			VkImageMemoryBarrier use_barrier[1] = {};
			use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier[0].image = m_Image;
			use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			use_barrier[0].subresourceRange.levelCount = 1;
			use_barrier[0].subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);
		}

		// End command buffer
		/*{
			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			err = vkEndCommandBuffer(command_buffer);
			check_vk_result(err);
			err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
			check_vk_result(err);
			err = vkDeviceWaitIdle(device);
			check_vk_result(err);
		}*/
		Application::FlushCommandBuffer(command_buffer);

		return true;
	}

	// Helper function to cleanup an image loaded with LoadTextureFromFile
	void Image::RemoveTexture()
	{
		VkDevice device = Application::GetDevice();
		vkFreeMemory(device, m_StagingBufferMemory, nullptr);
		vkDestroyBuffer(device, m_StagingBuffer, nullptr);
		vkDestroySampler(device, m_Sampler, nullptr);
		vkDestroyImageView(device, m_ImageView, nullptr);
		vkDestroyImage(device, m_Image, nullptr);
		vkFreeMemory(device, m_StagingBufferMemory, nullptr);
		//ImGui_ImplVulkan_RemoveTexture(m_DescriptorSet);
	}
}