#include "MovingSprite.h"

MovingSprite::MovingSprite(GLuint texID, vec3 position, vec3 dimensions, float angle, int nFrames, int nAnimations)
    : Sprite(texID, position, dimensions, angle, nFrames, nAnimations), velocity(0.0f, 0.0f), lastTime(0.0f) {}

const vec2& MovingSprite::getVelocity() const {
    return velocity;
}

void MovingSprite::setVelocity(const vec2& vel) {
    velocity = vel;
}

void MovingSprite::update(float now) {
    this->lastTime = now;
    position += vec3(velocity.x, velocity.y, 0.0f);
    Sprite::update(now);
}
