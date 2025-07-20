#ifndef SSBO_H
#define SSBO_H

#include "GLBufferObject.h"

class SSBO : public GLBufferObject {
public:
    explicit SSBO(const std::string& name)
        : GLBufferObject(GL_SHADER_STORAGE_BUFFER, name, true) {}

    SSBO(const std::string& name, GLuint externalId)
        : GLBufferObject(GL_SHADER_STORAGE_BUFFER, externalId, name) {}
};
class UBO : public GLBufferObject {
public:
    UBO(std::size_t size, GLenum usage = GL_DYNAMIC_DRAW)
        : GLBufferObject(GL_UNIFORM_BUFFER, "UBO", true) {
        Create(size, nullptr, usage);
    }

    UBO(GLuint externalId)
        : GLBufferObject(GL_UNIFORM_BUFFER, externalId, "UBO") {}
};
#endif // SSBO_H
