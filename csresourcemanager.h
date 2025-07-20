#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "ComputeShader.h"
#include "ssbo.h"

class ResourceManager {
public:
    ResourceManager() = default;
    // 禁止拷贝
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // 允许移动
    ResourceManager(ResourceManager&&) = default;
    ResourceManager& operator=(ResourceManager&&) = default;

    ~ResourceManager() {
        ReleaseAll();
    }
    // 加载ComputeShader文件，传入名字和路径
    void LoadShaders(const std::map<std::string, std::string>& shaderFiles);

    // 创建SSBO，传入名字和大小（字节）
    void CreateSSBOs(const std::map<std::string, size_t>& ssboSizes, GLenum usage= GL_DYNAMIC_DRAW);

    void CreatExternalSSBO(const std::string& name, GLuint existingBufferId) ;
    // 创建UBO，传入名字和大小（字节）
    void CreateUBOs(const std::map<std::string, size_t>& uboSizes);
    void AddExternalSSBOs(const std::map<std::string, GLuint>& externalSSBOs);
    template<typename T>
    void CreateSSBOWithData(const std::string& name, const std::vector<T>& data) {
        auto ssbo = std::make_shared<SSBO>(name);
        ssbo->Create(sizeof(T) * data.size(), data.data());
        ssbos_[name] = ssbo;
        std::cout << "Created SSBO with data: " << name << " count: " << data.size() << std::endl;
    }

    template<typename T>
    bool UploadUBOData(const std::string& name, const std::vector<T>& data, GLintptr offset) {
        auto it = ubos_.find(name);
        if (it == ubos_.end()) {
            std::cerr << "UBO not found: " << name << std::endl;
            return false;
        }
        it->second->UploadData(data.data(), sizeof(T) * data.size(), offset);
        return true;
    }

    // 获取资源接口
    std::shared_ptr<ComputeShader> GetShader(const std::string& name);
    std::shared_ptr<SSBO> GetSSBO(const std::string& name);
    std::shared_ptr<UBO> GetUBO(const std::string& name);

    // 获取所有资源，方便遍历
    const std::map<std::string, std::shared_ptr<ComputeShader>>& GetAllShaders() const { return shaders_; }
    const std::map<std::string, std::shared_ptr<SSBO>>& GetAllSSBOs() const { return ssbos_; }
    const std::map<std::string, std::shared_ptr<UBO>>& GetAllUBOs() const { return ubos_; }

private:
    std::map<std::string, std::shared_ptr<ComputeShader>> shaders_;
    std::map<std::string, std::shared_ptr<SSBO>> ssbos_;
    std::map<std::string, std::shared_ptr<UBO>> ubos_;
    void ReleaseAll();
};




#endif // RESOURCEMANAGER_H
