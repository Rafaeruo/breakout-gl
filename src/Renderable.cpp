#include "Renderable.h"
#include "../deps/include/glad/glad.h"

Renderable::Renderable(vec3 position, vec3 dimensions, float angle)
    : position(position), dimensions(dimensions), angle(angle) {
}

Renderable::~Renderable() {
}

void Renderable::setup()
{
    GLfloat blockVertices[] = {
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f,  0.5f, 0.0f
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blockVertices), blockVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->VAO = VAO;

}

vec3& Renderable::getPosition() {
    return position;
}

void Renderable::setPosition(const vec3& pos) {
    position = pos;
}

vec3 Renderable::getDimensions() const {
    return dimensions;
}

void Renderable::setDimensions(const vec3& dim) {
    dimensions = dim;
}

float Renderable::getAngle() const {
    return angle;
}

void Renderable::setAngle(float ang) {
    angle = ang;
}