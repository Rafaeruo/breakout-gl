#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "Sprite.h"

// Colisões em cada direção com os bounds da tela
bool checkLeftBound(Sprite &ball, GLuint WIDTH);
bool checkRightBound(Sprite &ball, GLuint WIDTH);
bool checkTopBound(Sprite &ball, GLuint HEIGHT);
bool checkBottomBound(Sprite &ball, GLuint HEIGHT);

// Colisõpes em cada direção
bool checkLeftCollision(Sprite &rect1, Sprite &rect2);
bool checkRightCollision(Sprite &rect1, Sprite &rect2);
bool checkTopCollision(Sprite &rect1, Sprite &rect2);
bool checkBottomCollision(Sprite &rect1, Sprite &rect2);

// Colições em todas as direções
bool checkCollision(Sprite &rect1, Sprite &rect2);

// Funções especializadas para colisões da bola
void checkBallBoundsCollision(MovingSprite &ball, int WIDTH, int HEIGHT);
void changeBallVelocityAfterCollision(MovingSprite &ball, Sprite &character);

#endif
