#ifndef SPRITE_H
#define SPRITE_H

#include "Renderable.h"
#include <glm/glm.hpp>

class Sprite : public Renderable {
protected:
    GLuint texID;

    // Animation control
    int nAnimations;
    int nFrames;
    int iAnimation;
    int iFrame;
    glm::vec2 d; // Frame offset or similar
    float FPS;
    float lastTime;

public:
    // Constructor for animated sprite
    Sprite(GLuint texID, vec3 position, vec3 dimensions, float angle, int nFrames, int nAnimations, float FPS = 24.0f);

    // Setup functions
    void setup() override;

    // Update function for animations
    void update(float deltaTime);

    // Override the render method from Renderable
    void render(GLuint shaderID) override;
    float getTextureOffsetS();
};

#endif // SPRITE_H
