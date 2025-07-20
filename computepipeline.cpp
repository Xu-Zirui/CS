#include "ComputePipeline.h"

ComputePipeline::ComputePipeline() {
    initializeOpenGLFunctions();
}

void ComputePipeline::AddShader(const std::string& name, std::shared_ptr<ComputeShader> shader) {
    shaders_[name] = std::move(shader);
}

void ComputePipeline::AddSSBO(const std::string& name, std::shared_ptr<SSBO> ssbo) {
    ssbos_[name] = std::move(ssbo);
}

void ComputePipeline::AddUBO(const std::string& name, std::shared_ptr<UBO> ubo) {
    ubos_[name] = std::move(ubo);
}

bool ComputePipeline::Build(const std::string& shaderName) {
    auto it = shaders_.find(shaderName);
    if (it == shaders_.end()) {
        std::cerr << "Shader not found: " << shaderName << std::endl;
        return false;
    }

    auto program = std::make_unique<QOpenGLShaderProgram>();
    if (!program->addShaderFromSourceCode(QOpenGLShader::Compute, QString::fromStdString(it->second->Source()))) {
        std::cerr << "Shader compile error: " << program->log().toStdString() << std::endl;
        return false;
    }
    if (!program->link()) {
        std::cerr << "Link error: " << program->log().toStdString() << std::endl;
        return false;
    }

    GLuint programId = program->programId();

    for (const auto& [ssboName, ssboPtr] : ssbos_) {
        GLuint index = glGetProgramResourceIndex(programId, GL_SHADER_STORAGE_BLOCK, ssboName.c_str());
        if (index == GL_INVALID_INDEX) {
            continue;
        }

        GLenum props[] = { GL_BUFFER_BINDING };
        GLuint binding = 0;
        GLint length = 0;
        glGetProgramResourceiv(programId, GL_SHADER_STORAGE_BLOCK, index, 1, props, 1, &length, reinterpret_cast<GLint*>(&binding));

        glShaderStorageBlockBinding(programId, index, binding);
        ssboPtr->BindToIndex(binding);
    }

    for (const auto& [uboName, uboPtr] : ubos_) {
        GLuint index = glGetProgramResourceIndex(programId, GL_UNIFORM_BLOCK, uboName.c_str());
        if (index == GL_INVALID_INDEX) continue;

        GLenum props[] = { GL_BUFFER_BINDING };
        GLuint binding = 0;
        GLint length = 0;
        glGetProgramResourceiv(programId, GL_UNIFORM_BLOCK, index, 1, props, 1, &length, reinterpret_cast<GLint*>(&binding));

        glUniformBlockBinding(programId, index, binding);
        uboPtr->BindToIndex(binding);
    }

    programs_[shaderName] = std::move(program);
    return true;
}

void ComputePipeline::Dispatch(const std::string& shaderName, GLuint x, GLuint y, GLuint z) {
    auto it = programs_.find(shaderName);
    if (it == programs_.end()) {
        std::cerr << "Program not built for shader: " << shaderName << std::endl;
        return;
    }

    it->second->bind();
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    it->second->release();
}

bool ComputePipeline::SetUniform(const std::string& shaderName, const std::string& uniformName, int value) {
    return SetUniformImpl(shaderName, uniformName, [&](int location) {
        programs_[shaderName]->setUniformValue(location, value);
    });
}

bool ComputePipeline::SetUniform(const std::string& shaderName, const std::string& uniformName, unsigned int value) {
    return SetUniformImpl(shaderName, uniformName, [&](int location) {
        programs_[shaderName]->setUniformValue(location, static_cast<int>(value));
    });
}

bool ComputePipeline::SetUniform(const std::string& shaderName, const std::string& uniformName, float value) {
    return SetUniformImpl(shaderName, uniformName, [&](int location) {
        programs_[shaderName]->setUniformValue(location, value);
    });
}

bool ComputePipeline::SetUniform(const std::string& shaderName, const std::string& uniformName, const QVector2D& value) {
    return SetUniformImpl(shaderName, uniformName, [&](int location) {
        programs_[shaderName]->setUniformValue(location, value);
    });
}

bool ComputePipeline::SetUniform(const std::string& shaderName, const std::string& uniformName, const QVector3D& value) {
    return SetUniformImpl(shaderName, uniformName, [&](int location) {
        programs_[shaderName]->setUniformValue(location, value);
    });
}

bool ComputePipeline::SetUniform(const std::string& shaderName, const std::string& uniformName, const QVector4D& value) {
    return SetUniformImpl(shaderName, uniformName, [&](int location) {
        programs_[shaderName]->setUniformValue(location, value);
    });
}

bool ComputePipeline::SetUniform(const std::string& shaderName, const std::string& uniformName, const QMatrix4x4& value) {
    return SetUniformImpl(shaderName, uniformName, [&](int location) {
        programs_[shaderName]->setUniformValue(location, value);
    });
}

template<typename Setter>
bool ComputePipeline::SetUniformImpl(const std::string& shaderName, const std::string& uniformName, Setter setter) {
    auto it = programs_.find(shaderName);
    if (it == programs_.end()) {
        std::cerr << "Program not found: " << shaderName << std::endl;
        return false;
    }

    QOpenGLShaderProgram* program = it->second.get();
    program->bind();
    int loc = program->uniformLocation(QString::fromStdString(uniformName));
    if (loc < 0) {
        std::cerr << "Uniform not found: " << uniformName << std::endl;
        program->release();
        return false;
    }

    setter(loc);
    program->release();
    return true;
}

