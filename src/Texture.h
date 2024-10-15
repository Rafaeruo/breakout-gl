#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <string>
#include "../deps/include/glad/glad.h"

// Declare the function to load a texture
int loadTexture(std::string filePath, int &imgWidth, int &imgHeight);

#endif // TEXTURE_LOADER_H
