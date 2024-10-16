#ifndef SPRITE_H
#define SPRITE_H

#include "Renderable.h"
#include <glm/glm.hpp>

class Sprite : public Renderable {
protected:
    GLuint texID;

    // Controle de animação
    int nAnimations;
    int nFrames;
    int iAnimation;
    int iFrame;
    glm::vec2 d; // Offset de frame
    float FPS;
    float lastTime;

public:
    Sprite(GLuint texID, vec3 position, vec3 dimensions, float angle, int nFrames, int nAnimations, float FPS = 24.0f);

    void setup() override;
    void update(float deltaTime);
    void render(GLuint shaderID) override;
    float getTextureOffsetS();
};

#endif
