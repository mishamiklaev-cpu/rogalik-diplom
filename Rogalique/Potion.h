#pragma once
#include "Constants.h"
#include "Room.h"
#include <SFML/Graphics.hpp>
#include <vector>

// зелье здоровья: маленькая золотая фляжка на полу.
// Появляется в случайной комнате; выпил — восстановил здоровье
class Potion
{
  public:
    int xPotion;
    int yPotion;

    void SpawnPotion(const std::vector<Room> &rooms);
    void DrawPotion(sf::RenderWindow &window) const; // рисование не меняет зелье
};
