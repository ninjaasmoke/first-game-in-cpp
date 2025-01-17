#include <enemy.h>
#include <tileRenderer.h>

void Enemy::render(gl2d::Renderer2D &renderer, gl2d::Texture &sprites, gl2d::TextureAtlasPadding &atlas)
{
    // renderSpaceShip(renderer, position, enemyShipSize, sprites, atlas.get(type.x, type.y), angle);

    float spaceShipAngle = atan2(viewDirection.y, -viewDirection.x);

    renderer.renderRectangle(
        {position - glm::vec2(enemyShipSize / 2, enemyShipSize / 2), enemyShipSize, enemyShipSize},
        sprites,
        Colors_White,
        {},
        glm::degrees(spaceShipAngle) - 90.0f,
        atlas.get(type.x, type.y));
}

bool Enemy::update(float deltaTime, glm::vec2 playerPos)
{
    // wandering offset periodically
    wanderTimer += deltaTime;
    if (wanderTimer >= wanderInterval)
    {
        // new random offset
        float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI;
        wanderOffset = glm::vec2(cos(angle), sin(angle)) * wanderRadius;
        wanderTimer = 0;
    }

    // direction to player with wander offset
    glm::vec2 targetPos = playerPos + wanderOffset;
    glm::vec2 directionToTarget = targetPos - position;

    bool shoot = (glm::length(directionToTarget + viewDirection) >= fireRange);
    {
        if (shoot)
        {
            if (firedTime <= 0.f)
            {
                firedTime = fireTimeReset;
            }
            else
            {
                shoot = false;
            }
        }
    }

    firedTime -= deltaTime;
    if (firedTime < 0)
    {
        firedTime = 0.f;
    }

    if (glm::length(directionToTarget) == 0)
    {
        directionToTarget = {1, 0};
    }
    else
    {
        directionToTarget = glm::normalize(directionToTarget);
    }

    // smoothrotate towards target direction
    glm::vec2 newDirection = {};
    if (glm::length(directionToTarget + viewDirection) <= 0.1f)
    {
        newDirection = glm::vec2(directionToTarget.y, -directionToTarget.x);
    }
    else
    {
        newDirection = deltaTime * turnSpeed * directionToTarget + viewDirection;
    }
    viewDirection = glm::normalize(newDirection);

    // acceleration with some randomness
    float randomFactor = 1.0f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.4f;
    glm::vec2 acceleration = viewDirection * accelerationRate * randomFactor;

    // add periodic speed variation
    float speedMultiplier = 1.0f + 0.2f * sin(totalTime * 2.0f);
    glm::vec2 velocity = speed * acceleration * deltaTime * speedMultiplier;

    // cap velocity to maxSpeed
    if (glm::length(velocity) > maxSpeed)
    {
        velocity = glm::normalize(velocity) * maxSpeed;
    }

    position += velocity * deltaTime;
    totalTime += deltaTime;

    return shoot;
}
