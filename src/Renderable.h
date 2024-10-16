#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "../deps/include/glad/glad.h"
#include <glm/glm.hpp>

using namespace glm;

class Renderable {
protected:
    GLuint VAO;
    vec3 position;
    vec3 dimensions;
    float angle;

public:
    Renderable(vec3 position, vec3 dimensions, float angle);
    virtual ~Renderable();
    virtual void setup();

    vec3& getPosition();
    void setPosition(const vec3& pos);

    vec3 getDimensions() const;
    void setDimensions(const vec3& dim);

    float getAngle() const;
    void setAngle(float ang);

    virtual void render(GLuint shaderID) = 0;
};

#endif