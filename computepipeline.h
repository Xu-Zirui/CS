#ifndef COMPUTEPIPELINE_H
#define COMPUTEPIPELINE_H

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <memory>
#include <map>
#include <string>
#include <iostream>
#include "ComputeShader.h"
#include "ssbo.h"

class ComputePipeline : protected QOpenGLFunctions_4_5_Core {
public:
    ComputePipeline() {
        initializeOpenGLFunctions();
    }

    // 添加shader
    void AddShader(const std::string& name, std::shared_ptr<ComputeShader> shader) {
        m_shaders[name] = std::move(shader);
    }

    // 添加SSBO
    void AddSSBO(const std::string& name, std::shared_ptr<SSBO> ssbo) {
        m_ssbos[name] = std::move(ssbo);
    }

    // 编译并链接指定shader，自动根据shader中SSBO的binding自动绑定SSBO
    bool Build(const std::string& shaderName) {
        auto it = m_shaders.find(shaderName);
        if (it == m_shaders.end()) {
            std::cerr << "Shader not found: " << shaderName << std::endl;
            return false;
        }

        m_program = std::make_unique<QOpenGLShaderProgram>();
        if (!m_program->addShaderFromSourceCode(QOpenGLShader::Compute, QString::fromStdString(it->second->Source()))) {
            std::cerr << "Shader compile error: " << m_program->log().toStdString() << std::endl;
            return false;
        }
        if (!m_program->link()) {
            std::cerr << "Link error: " << m_program->log().toStdString() << std::endl;
            return false;
        }

        GLuint programId = m_program->programId();

        // 遍历所有已添加的SSBO，查找shader是否使用了对应名字的SSBO
        for (const auto& [ssboName, ssboPtr] : m_ssbos) {
            GLuint index = glGetProgramResourceIndex(programId, GL_SHADER_STORAGE_BLOCK, ssboName.c_str());
            if (index == GL_INVALID_INDEX) {
                // 当前shader未使用此SSBO，跳过
                continue;
            }

            // 查询该SSBO的绑定点binding（由shader中layout(binding=XX)指定）
            GLenum props[] = { GL_BUFFER_BINDING };
            GLuint binding = 0;
            GLint length = 0;
            glGetProgramResourceiv(programId, GL_SHADER_STORAGE_BLOCK, index, 1, props, 1, &length, reinterpret_cast<GLint*>(&binding));

            // 绑定shader中的SSBO块binding点与GPU SSBO对象
            glShaderStorageBlockBinding(programId, index, binding);
            ssboPtr->BindToIndex(binding);
        }

        return true;
    }

    // 调用dispatch compute
    void Dispatch(GLuint x, GLuint y = 1, GLuint z = 1) {
        m_program->bind();
        glDispatchCompute(x, y, z);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        m_program->release();
    }

private:
    std::map<std::string, std::shared_ptr<ComputeShader>> m_shaders;
    std::map<std::string, std::shared_ptr<SSBO>> m_ssbos;

    std::unique_ptr<QOpenGLShaderProgram> m_program;
};

#endif // COMPUTEPIPELINE_H
