#include "GLBufferObject.h"
#include <QDebug>

GLBufferObject::GLBufferObject(GLenum target, const std::string& name, bool owns)
    : target_(target), name_(name), ownsBuffer_(owns) {
    initializeOpenGLFunctions();
    if (ownsBuffer_)
        glCreateBuffers(1, &id_);
}

GLBufferObject::GLBufferObject(GLenum target, GLuint existingId, const std::string& name)
    : target_(target), id_(existingId), name_(name), ownsBuffer_(false) {
    initializeOpenGLFunctions();
}

GLBufferObject::~GLBufferObject() {
    if (ownsBuffer_ && id_) {
        glDeleteBuffers(1, &id_);
        id_ = 0;
    }
}

void GLBufferObject::Create(std::size_t size, const void* data, GLenum usage) {
    if (!ownsBuffer_) {
        qWarning() << "Cannot create buffer data for external ID buffer:" << QString::fromStdString(name_);
        return;
    }
    glNamedBufferData(id_, size, data, usage);
}

void GLBufferObject::UploadData(const void* data, std::size_t size, GLintptr offset) {
    glNamedBufferSubData(id_, offset, size, data);
}


void GLBufferObject::Resize(std::size_t newSize, const void* data , GLenum usage) {
    if (!ownsBuffer_) {
        qWarning() << "Cannot resize external ID buffer:" << QString::fromStdString(name_);
        return;
    }
    // 重新分配缓冲区空间，会丢弃之前数据
    glNamedBufferData(id_, newSize, data, usage);
}

void GLBufferObject::BindToIndex(GLuint index) {
    glBindBufferBase(target_, index, id_);
}

GLuint GLBufferObject::Id() const {
    return id_;
}

const std::string& GLBufferObject::Name() const {
    return name_;
}
