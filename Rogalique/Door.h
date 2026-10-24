#pragma once
#include "Room.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Door
{
  public:
    int xCellDoor = 0;
    int yCellDoor = 0;

    // выбирает случайную комнату и ставит выход в её центр, не ближе 10 клеток к игроку
    void SpawnDoor(const std::vector<Room> &rooms, int playerX, int playerY);
    void DrawDoor(sf::RenderWindow &window);
};
