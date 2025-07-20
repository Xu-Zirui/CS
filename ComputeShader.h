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
        : path_(filePath) {
        std::ifstream file(filePath, std::ios::in);
        if (!file.is_open()) {
            std::cerr << "Cannot open shader file: " << filePath << std::endl;
            return;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        source_ = buffer.str();
    }

    const std::string& Source() const { return source_; }
    const std::string& Path() const { return path_; }

private:
    std::string path_;
    std::string source_;
};

#endif // COMPUTESHADER_H
