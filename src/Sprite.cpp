#include "Sprite.h"
#include "../deps/include/glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sprite::Sprite(GLuint texID, vec3 position, vec3 dimensions, float angle, int nFrames, int nAnimations, float FPS)
    : Renderable(position, dimensions, angle), texID(texID), nFrames(nFrames), nAnimations(nAnimations), iAnimation(0), iFrame(0), FPS(FPS), lastTime(0.0f) {
        this->texID = texID;
	d.s = 1.0 / (float)nFrames;
	d.t = 1.0 / (float)nAnimations;
	
	FPS = 12.0;
	lastTime = 0.0;
}

void Sprite::setup()
{
	GLfloat vertices[] = {
		// x   y     z    s     		t
		// T0
		-0.5, -0.5, 0.0, 0.0, 0.0, // V0
		-0.5, 0.5, 0.0, 0.0, d.t,  // V1
		0.5, -0.5, 0.0, d.s, 0.0,  // V2
		0.5, 0.5, 0.0, d.s, d.t	   // V3

	};

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	this->VAO = VAO;
}

void Sprite::update(float now) {
    if (nFrames > 1 || nAnimations > 1) {
        float dt = now - this->lastTime;
		if (dt >= 1.0 / this->FPS)
		{
			this->iFrame = (this->iFrame + 1) % this->nFrames; // incrementando ciclicamente o indice do Frame
			this->lastTime = now;
		}	
    }

}

void Sprite::render(GLuint shaderID) {
    glBindVertexArray(this->VAO);

	glBindTexture(GL_TEXTURE_2D, this->texID); 

	mat4 model = mat4(1);
	model = translate(model, this->position);
	model = rotate(model, radians(this->angle), vec3(0.0, 0.0, 1.0));
	model = scale(model, this->dimensions);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

float Sprite::getTextureOffsetS()
{
    return this->iFrame * this->d.s;
}