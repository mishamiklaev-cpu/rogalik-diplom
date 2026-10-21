#pragma once
#include "Dungeon.h"
#include "Player.h"
#include "Room.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Enemy
{
  public:
    Enemy();

    int xCellEnemy;
    int yCellEnemy;
    int widthEnemy;
    int heightEnemy;

    void SetPositionEnemy(int xCellEnemy, int yCellEnemy);
    void SpawnEnemy(const std::vector<Room> &rooms);
    void DrawEnemy(sf::RenderWindow &window);
    void FindPlayer(const Player &player, const Dungeon &dungeon);
    bool canMove(int xDirection, int yDirection, const Dungeon &dungeon);
    void MoveEnemy(int xDirection, int yDirection, const Dungeon &dungeon);
};