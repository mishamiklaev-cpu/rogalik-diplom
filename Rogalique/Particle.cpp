#include "Particle.h"
#include <cstdlib>

Particle::Particle()
{
    xParticle = 0.f;
    yParticle = 0.f;
    dxParticle = 0.f;
    dyParticle = 0.f;
    lifeParticle = 0.f;
    colorParticle = sf::Color::White;
}

// движение частицы: позиции меняются по скорости, жизнь тает
void Particle::UpdateParticle(float deltaTime)
{
    xParticle += dxParticle * deltaTime;
    yParticle += dyParticle * deltaTime;
    lifeParticle -= deltaTime; // life <= 0 — частица мертва (убирает цикл внешнего массива)
}

void Particle::DrawParticle(sf::RenderWindow &window) const
{
    // мелкий квадрат 8 px — пятая часть клетки (40 px), визуально «искры»
    sf::RectangleShape particleShape(sf::Vector2f(8.f, 8.f));
    particleShape.setFillColor(colorParticle);
    particleShape.setPosition(sf::Vector2f(xParticle, yParticle));
    window.draw(particleShape);
}

void SpawnBurst(std::vector<Particle> &particles, float xPoint, float yPoint,
                sf::Color color)
{
    for (int i = 0; i < PARTICLE_COUNT; i++)
    {
        Particle particle;
        particle.xParticle = xPoint + static_cast<float>(rand() % 21 - 10);
        particle.yParticle = yPoint + static_cast<float>(rand() % 21 - 10);
        particle.dxParticle = static_cast<float>(rand() % 101 - 50); // -50..+50 px/c
        particle.dyParticle = static_cast<float>(rand() % 101 - 50);
        particle.lifeParticle = PARTICLE_LIFE_TIME;
        particle.colorParticle = color;
        particles.push_back(particle);
    }
}
