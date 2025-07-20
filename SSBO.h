#ifndef SSBO_H
#define SSBO_H

#include <QOpenGLFunctions_4_5_Core>
#include <string>

class SSBO : protected QOpenGLFunctions_4_5_Core {
public:
    explicit SSBO(const std::string& name)
        : m_name(name) {
        initializeOpenGLFunctions();
        glCreateBuffers(1, &m_id);
    }

    ~SSBO() {
        if (m_id != 0) {
            glDeleteBuffers(1, &m_id);
            m_id = 0;
        }
    }

    void Create(std::size_t size, const void* data = nullptr, GLenum usage = GL_DYNAMIC_DRAW) {
        glNamedBufferData(m_id, size, data, usage);
    }

    void BindToIndex(GLuint index) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_id);
    }

    void Bind()  { glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id); }
    void Unbind()  { glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); }

    GLuint Id() const { return m_id; }
    const std::string& Name() const { return m_name; }

private:
    GLuint m_id = 0;
    std::string m_name;
};

#endif // SSBO_H
