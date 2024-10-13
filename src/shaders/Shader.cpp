#include "Shader.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string path)
{
    std::string code;
    std::ifstream file;

    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        file.open(path);
        std::stringstream shaderStream;
        shaderStream << file.rdbuf();		
        file.close();
        this->code = shaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " 
                  << e.what() << std::endl;
    }
}

std::string Shader::getCode()
{
    return this->code;
}
