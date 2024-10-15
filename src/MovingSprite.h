#ifndef MOVINGSPRITE_H
#define MOVINGSPRITE_H

#include "Sprite.h"

class MovingSprite : public Sprite {
public:
    // Constructor
    MovingSprite(GLuint texID, vec3 position, vec3 dimensions, float angle, int nFrames, int nAnimations)
        : Sprite(texID, position, dimensions, angle, nFrames, nAnimations), velocity(0.0f, 0.0f) {}

    // Getter for velocity
    const vec2& getVelocity() const {
        return velocity;
    }

    // Setter for velocity
    void setVelocity(const vec2& vel) {
        velocity = vel;
    }

    // Update the position based on velocity and delta time
    void update(float now) {
        float deltaTime = now - this->lastTime;
        position += vec3(velocity.x, velocity.y, 0.0f);
        Sprite::update(deltaTime); // Call the base class update if necessary
    }

private:
    vec2 velocity; // Velocity of the sprite
};

#endif // MOVINGSPRITE_H