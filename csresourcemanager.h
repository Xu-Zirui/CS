#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "ComputeShader.h"
#include "ssbo.h"  // 里面包含SSBO和UBO定义

class ResourceManager {
public:
    ResourceManager() = default;

    // 加载ComputeShader文件，传入名字和路径
    void LoadShaders(const std::map<std::string, std::string>& shaderFiles);

    // 创建SSBO，传入名字和大小（字节）
    void CreateSSBOs(const std::map<std::string, size_t>& ssboSizes);



    // 创建UBO，传入名字和大小（字节）
    void CreateUBOs(const std::map<std::string, size_t>& uboSizes);

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
};




#endif // RESOURCEMANAGER_H
