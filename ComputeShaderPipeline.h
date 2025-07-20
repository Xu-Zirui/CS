#pragma once
#include "ComputeShaderBase.h"
#include <memory>
#include <vector>

class ComputeShaderPipeline {
public:
    template<typename T, typename... Args>
    void addShader(Args&&... args) {
        shaders.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    bool loadAll() {
        for (auto& s : shaders)
            if (!s->load()) return false;
        return true;
    }

    bool runAll(GLuint x = 1, GLuint y = 1, GLuint z = 1) {
        for (auto& s : shaders)
            if (!s->dispatch(x, y, z)) return false;
        return true;
    }

private:
    std::vector<std::unique_ptr<ComputeShaderBase>> shaders;
};
