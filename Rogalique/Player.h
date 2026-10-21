#pragma once
#include "Dungeon.h"
#include "Room.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Player
{
  public:
    Player();

    int xCellPlayer;
    int yCellPlayer;
    int widthPlayer;
    int heightPlayer;

    void SetPositionPlayer(int xCellPlayer, int yCellPlayer);
    void SpawnPlayerInRandomRoom(const std::vector<Room> &rooms);
    void DrawPlayer(sf::RenderWindow &window);
    void MovePlayer(int xDirection, int yDirection, const Dungeon &dungeon);
};