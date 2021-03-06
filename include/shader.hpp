#pragma once

#include <glad/glad.h>
#include <string>
#include <sstream>
#include <iostream>

namespace GameOfLife
{
class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexPath, const GLchar* fragmentPath);

    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};
}

