#include "Vks.h"


//VulkanStructures
std::vector<char> rlms::vks::readFile (const std::string& filename) {
	std::ifstream file (filename, std::ios::ate | std::ios::binary);

	if (!file.is_open ()) {
		throw std::runtime_error ("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg ();
	std::vector<char> buffer (fileSize);

	file.seekg (0);
	file.read (buffer.data (), fileSize);

	file.close ();

	return buffer;
}

VKAPI_ATTR VkBool32 VKAPI_CALL rlms::vks::debugCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::ostringstream oss ("");
	oss << "validation layer: " << pCallbackData->pMessage << "\n";
	GraphicsManager::GetLogger ()->print (LogTags::Error, oss.str ());
	return VK_FALSE;
}
