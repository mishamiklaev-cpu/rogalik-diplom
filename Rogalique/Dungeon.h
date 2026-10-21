#pragma once
#include "Room.h"
#include "Tile.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Dungeon
{
  public:
    int widthField = 0;
    int heightField = 0;
    bool isRoomCollide(const Room &a, const Room &b);

    std::vector<std::vector<Tile>> cell;
    std::vector<Room> rooms;

    void Generate(int widthMap, int heightMap);
    void CarveRoom(const Room &room);
    void CarveCorridor(int xRoomCentr1, int yRoomCentr1, int xRoomCentr2, int yRoom2Centr2);
    void Draw(sf::RenderWindow &window);
};
