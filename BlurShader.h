#pragma once
#include "ComputeShaderBase.h"

class BlurShader : public ComputeShaderBase {
public:
    BlurShader(const QString& path, GLuint ssboInput, GLuint ssboOutput)
        : ComputeShaderBase(path), inputBuffer(ssboInput), outputBuffer(ssboOutput) {}

    void bindSSBOs() override {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);
    }

private:
    GLuint inputBuffer;
    GLuint outputBuffer;
};
