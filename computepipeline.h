#ifndef COMPUTEPIPELINE_H
#define COMPUTEPIPELINE_H




#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <memory>
#include <map>
#include <string>
#include <iostream>
#include "ComputeShader.h"
#include "ssbo.h"

class ComputePipeline : protected QOpenGLFunctions_4_5_Core {
public:
    ComputePipeline();

    void AddShader(const std::string& name, std::shared_ptr<ComputeShader> shader);
    void AddSSBO(const std::string& name, std::shared_ptr<SSBO> ssbo);
    void AddUBO(const std::string& name, std::shared_ptr<UBO> ubo);
    bool UpdateSSBO(const std::string& name, size_t newSize, const void* data);
    bool Build(const std::string& shaderName);

    void Dispatch(const std::string& shaderName, GLuint x, GLuint y = 1, GLuint z = 1);

    template<typename T>
    bool SetUniform(const std::string& shaderName, const std::string& uniformName, const T& value) {
        return SetUniformImpl(shaderName, uniformName, [&](int location) {
            programs_[shaderName]->setUniformValue(location, value);
        });
    }

    // Uniform setters
    std::map<std::string, std::shared_ptr<SSBO>> GetSsbos(){return ssbos_;};
private:
    template<typename Setter>
    bool SetUniformImpl(const std::string& shaderName, const std::string& uniformName, Setter setter);

private:
    std::map<std::string, std::shared_ptr<ComputeShader>> shaders_;
    std::map<std::string, std::shared_ptr<SSBO>> ssbos_;
    std::map<std::string, std::unique_ptr<QOpenGLShaderProgram>> programs_;
    std::map<std::string, std::shared_ptr<UBO>> ubos_;
};



#endif // COMPUTEPIPELINE_H
