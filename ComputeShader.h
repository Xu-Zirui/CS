#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ComputeShader {
public:
    // 构造时加载文件
    explicit ComputeShader(const std::string& filePath)
        : m_path(filePath) {
        std::ifstream file(filePath, std::ios::in);
        if (!file.is_open()) {
            std::cerr << "Cannot open shader file: " << filePath << std::endl;
            return;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        m_source = buffer.str();
    }

    const std::string& Source() const { return m_source; }
    const std::string& Path() const { return m_path; }

private:
    std::string m_path;
    std::string m_source;
};

#endif // COMPUTESHADER_H
