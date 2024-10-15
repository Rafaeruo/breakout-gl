// CollisionDetection.h
#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "Sprite.h"  // For Sprite class

// Function prototypes for checking bounds
bool checkLeftBound(Sprite &ball, GLuint WIDTH);
bool checkRightBound(Sprite &ball, GLuint WIDTH);
bool checkTopBound(Sprite &ball, GLuint HEIGHT);
bool checkBottomBound(Sprite &ball, GLuint HEIGHT);

// Function prototypes for collision detection between sprites
bool checkLeftCollision(Sprite &rect1, Sprite &rect2);
bool checkRightCollision(Sprite &rect1, Sprite &rect2);
bool checkTopCollision(Sprite &rect1, Sprite &rect2);
bool checkBottomCollision(Sprite &rect1, Sprite &rect2);

// Function to check collision using all sides
bool checkCollision(Sprite &rect1, Sprite &rect2);

#endif // COLLISION_DETECTION_H
