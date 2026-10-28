#pragma once
#include "Constants.h"
#include <SFML/Graphics.hpp>

// частица — мелкая искра события: родилась, разлетелась, погасла.
// Всё её поведение строится на таймере жизни (наш знакомый приём)
class Particle
{
  public:
    float xParticle;   // позиция в ПИКСЕЛЯХ (частицы живут не в клетках)
    float yParticle;
    float dxParticle;  // скорость: сколько пикселей за секунду
    float dyParticle;
    float lifeParticle;   // сколько секунд осталось жить
    sf::Color colorParticle;

    Particle();

    void UpdateParticle(float deltaTime);          // движение + таяние жизни
    void DrawParticle(sf::RenderWindow &window) const; // рисование не меняет частицу
};
