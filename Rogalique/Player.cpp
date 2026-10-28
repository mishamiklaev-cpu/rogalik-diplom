#include "Player.h"
#include "Constants.h"
#include <cstdlib>

Player::Player()
{
    xCellPlayer = 0;
    yCellPlayer = 0;
    widthPlayer = 1;
    heightPlayer = 1;
    hpPlayer = 100; // полное здоровье на старте
}

void Player::SetPositionPlayer(int x, int y)
{
    xCellPlayer = x;
    yCellPlayer = y;
}

void Player::SpawnPlayerInRandomRoom(const std::vector<Room> &rooms)
{
    int randomRoom = rand() % rooms.size();
    const Room &chosenRoom = rooms[randomRoom];

    xCellPlayer = chosenRoom.xRoom + (chosenRoom.widthRoom / 2);
    yCellPlayer = chosenRoom.yRoom + (chosenRoom.heightRoom / 2);
};

void Player::DrawPlayer(sf::RenderWindow &window)
{
    sf::RectangleShape playerShape;
    playerShape.setSize(sf::Vector2f(widthPlayer * TILE_SIZE, heightPlayer * TILE_SIZE));
    playerShape.setFillColor(sf::Color::Green);
    playerShape.setPosition(sf::Vector2f(xCellPlayer * TILE_SIZE, yCellPlayer * TILE_SIZE));
    window.draw(playerShape);
};

void Player::MovePlayer(int xDirection, int yDirection, const Dungeon &dungeon)
{
    xCellPlayer += xDirection;
    yCellPlayer += yDirection;

    if (xCellPlayer < 0 || xCellPlayer >= dungeon.widthField || yCellPlayer < 0 ||
        yCellPlayer >= dungeon.heightField || dungeon.cell[yCellPlayer][xCellPlayer] == Tile::Wall)
    {
        xCellPlayer -= xDirection;
        yCellPlayer -= yDirection;
    }
}
