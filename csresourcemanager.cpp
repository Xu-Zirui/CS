#include "csResourceManager.h"

void ResourceManager::LoadShaders(const std::map<std::string, std::string>& shaderFiles) {
    for (const auto& [name, path] : shaderFiles) {
        auto shader = std::make_shared<ComputeShader>(path);
        shaders_[name] = shader;
        std::cout << "Loaded shader: " << name << " from " << path << std::endl;
    }
}

void ResourceManager::CreateSSBOs(const std::map<std::string, size_t>& ssboSizes, GLenum usage) {
    for (const auto& [name, size] : ssboSizes) {
        auto ssbo = std::make_shared<SSBO>(name);
        ssbo->Create(size, nullptr,usage);
        ssbos_[name] = ssbo;
        std::cout << "Created SSBO: " << name << " size: " << size << std::endl;
    }
}
// 批量注册外部已有的 SSBO
void ResourceManager::AddExternalSSBOs(const std::map<std::string, GLuint>& externalSSBOs) {
    for (const auto& kv : externalSSBOs) {
        const std::string& name = kv.first;
        GLuint bufferId = kv.second;
        auto ssbo = std::make_shared<SSBO>(name, bufferId);
        ssbos_[name] = ssbo;
        std::cout << "Registered external SSBO from GPU buffer id: " << bufferId << ", name: " << name << std::endl;
    }
}

void ResourceManager::CreateUBOs(const std::map<std::string, size_t>& uboSizes) {
    for (const auto& [name, size] : uboSizes) {
        auto ubo = std::make_shared<UBO>(size);
        ubos_[name] = ubo;
        std::cout << "Created UBO: " << name << " size: " << size << std::endl;
    }
}
std::shared_ptr<ComputeShader> ResourceManager::GetShader(const std::string& name) {
    auto it = shaders_.find(name);
    if (it == shaders_.end()) {
        std::cerr << "Shader not found: " << name << std::endl;
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<SSBO> ResourceManager::GetSSBO(const std::string& name) {
    auto it = ssbos_.find(name);
    if (it == ssbos_.end()) {
        std::cerr << "SSBO not found: " << name << std::endl;
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<UBO> ResourceManager::GetUBO(const std::string& name) {
    auto it = ubos_.find(name);
    if (it == ubos_.end()) {
        std::cerr << "UBO not found: " << name << std::endl;
        return nullptr;
    }
    return it->second;
}
void  ResourceManager::ReleaseAll()
{
    shaders_.clear();
    ssbos_.clear();
    ubos_.clear();


}
