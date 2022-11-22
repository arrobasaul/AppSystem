#include "Utils.hpp"

namespace AppEngine {
    std::vector<char> readFile(const std::string& filepath) {
        std::string enginePath = ENGINE_DIR + filepath;
        std::ifstream file{enginePath, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + enginePath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }
}