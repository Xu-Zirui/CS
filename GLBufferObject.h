#ifndef GLBUFFEROBJECT_H
#define GLBUFFEROBJECT_H

#include <QOpenGLFunctions_4_5_Core>
#include <string>

class GLBufferObject : protected QOpenGLFunctions_4_5_Core {
public:
    explicit GLBufferObject(GLenum target, const std::string& name = {}, bool owns = true);
    GLBufferObject(GLenum target, GLuint existingId, const std::string& name = {});
    virtual ~GLBufferObject();

    void Create(std::size_t size, const void* data = nullptr, GLenum usage = GL_DYNAMIC_DRAW);
    void Resize(std::size_t newSize, const void* data = nullptr, GLenum usage = GL_DYNAMIC_DRAW);
    void UploadData(const void* data, std::size_t size, GLintptr offset = 0);
    void BindToIndex(GLuint index);

    GLuint Id() const;
    const std::string& Name() const;

protected:
    GLenum target_;
    GLuint id_ = 0;
    std::string name_;
    bool ownsBuffer_ = true;
};

#endif // GLBUFFEROBJECT_H
