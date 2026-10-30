#include "Particle.h"

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
    // мелкий квадрат: чуть больше клетки/16 — визуально «искры»
    sf::RectangleShape particleShape(sf::Vector2f(8.f, 8.f));
    particleShape.setFillColor(colorParticle);
    particleShape.setPosition(sf::Vector2f(xParticle, yParticle));
    window.draw(particleShape);
}
