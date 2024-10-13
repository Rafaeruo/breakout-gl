#include <string>

class Shader
{
public:
    Shader(const std::string path);
    std::string getCode();
private:
    std::string code;
};