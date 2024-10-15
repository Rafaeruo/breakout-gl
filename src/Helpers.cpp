#include "Helpers.h"

glm::vec3 generateRandomVelocity(float speed) {
    srand(static_cast<unsigned int>(time(0)));

    float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 * M_PI; // Random angle between 0 and 2π

    float velX = cos(angle) * speed;
    float velY = sin(angle) * speed;

    return glm::vec3(velX, velY, 0.0f);
}
