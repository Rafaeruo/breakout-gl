#include "Sprite.h" 

bool checkLeftBound(Sprite &ball, GLuint WIDTH) {
    float ballLeft = ball.getPosition().x - ball.getDimensions().x / 2.0f;
    if (ballLeft < 0.0f) {
        return true;
    }
    return false;
}

bool checkRightBound(Sprite &ball, GLuint WIDTH) {
    float ballRight = ball.getPosition().x + ball.getDimensions().x / 2.0f;
    if (ballRight > WIDTH) {
        return true;
    }
    return false;
}

bool checkTopBound(Sprite &ball, GLuint HEIGHT) {
    float ballTop = ball.getPosition().y + ball.getDimensions().y / 2.0f;
    if (ballTop > HEIGHT) {
        return true;
    }
    return false;
}

bool checkBottomBound(Sprite &ball, GLuint HEIGHT) {
    float ballBottom = ball.getPosition().y - ball.getDimensions().y / 2.0f;
    if (ballBottom < 0.0f) {
        return true;
    }
    return false;
}

bool checkLeftCollision(Sprite &rect1, Sprite &rect2) {
    float rect1Left = rect1.getPosition().x - rect1.getDimensions().x / 2.0f;
    float rect2Right = rect2.getPosition().x + rect2.getDimensions().x / 2.0f;
    return rect1Left < rect2Right;
}

bool checkRightCollision(Sprite &rect1, Sprite &rect2) {
    float rect1Right = rect1.getPosition().x + rect1.getDimensions().x / 2.0f;
    float rect2Left = rect2.getPosition().x - rect2.getDimensions().x / 2.0f;
    return rect1Right > rect2Left;
}

bool checkTopCollision(Sprite &rect1, Sprite &rect2) {
    float rect1Top = rect1.getPosition().y + rect1.getDimensions().y / 2.0f;
    float rect2Bottom = rect2.getPosition().y - rect2.getDimensions().y / 2.0f;
    return rect1Top > rect2Bottom;
}

bool checkBottomCollision(Sprite &rect1, Sprite &rect2) {
    float rect1Bottom = rect1.getPosition().y - rect1.getDimensions().y / 2.0f;
    float rect2Top = rect2.getPosition().y + rect2.getDimensions().y / 2.0f;
    return rect1Bottom < rect2Top;
}

bool checkCollision(Sprite &rect1, Sprite &rect2) {
    return checkLeftCollision(rect1, rect2) &&
           checkRightCollision(rect1, rect2) &&
           checkTopCollision(rect1, rect2) &&
           checkBottomCollision(rect1, rect2);
}