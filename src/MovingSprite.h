#ifndef MOVINGSPRITE_H
#define MOVINGSPRITE_H

#include "Sprite.h"

class MovingSprite : public Sprite {
public:
    MovingSprite(GLuint texID, vec3 position, vec3 dimensions, float angle, int nFrames, int nAnimations);

    const vec2& getVelocity() const;
    void setVelocity(const vec2& vel);
    void update(float now);

private:
    vec2 velocity;
    float lastTime = 0.0f;
};

#endif
