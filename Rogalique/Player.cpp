#include "Player.h"
#include "Constants.h"
#include <cstdlib>

Player::Player()
{
    xCellPlayer = 0;
    yCellPlayer = 0;
    widthPlayer = 1;
    heightPlayer = 1;
    hpPlayer = MAX_HP_PLAYER; // полное здоровье на старте
    directionPlayer = Direction::Down; // на старте смотрит на зрителя
    framePlayer = 0;
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

void Player::DrawPlayer(sf::RenderWindow &window, const GameTextures &textures)
{
    sf::RectangleShape playerShape;
    playerShape.setSize(sf::Vector2f(widthPlayer * TILE_SIZE, heightPlayer * TILE_SIZE));
    if (textures.playerLoaded)
    {
        // окошко в спрайт-лист: столбец — фаза бега, ряд — направление
        int tileSize = static_cast<int>(TILE_SIZE);
        playerShape.setTexture(&textures.playerTexture);
        playerShape.setTextureRect(sf::IntRect(framePlayer * tileSize,
                                               static_cast<int>(directionPlayer) * tileSize,
                                               tileSize, tileSize));
    }
    else
    {
        playerShape.setFillColor(sf::Color::Green);
    }
    playerShape.setPosition(sf::Vector2f(xCellPlayer * TILE_SIZE, yCellPlayer * TILE_SIZE));
    window.draw(playerShape);
};

void Player::MovePlayer(int xDirection, int yDirection, const Dungeon &dungeon)
{
    // поворачиваемся всегда, даже если впереди стена
    if (xDirection > 0)
    {
        directionPlayer = Direction::Right;
    }
    else if (xDirection < 0)
    {
        directionPlayer = Direction::Left;
    }
    else if (yDirection > 0)
    {
        directionPlayer = Direction::Down;
    }
    else if (yDirection < 0)
    {
        directionPlayer = Direction::Up;
    }

    xCellPlayer += xDirection;
    yCellPlayer += yDirection;

    if (xCellPlayer < 0 || xCellPlayer >= dungeon.widthField || yCellPlayer < 0 ||
        yCellPlayer >= dungeon.heightField || dungeon.cell[yCellPlayer][xCellPlayer] == Tile::Wall)
    {
        xCellPlayer -= xDirection;
        yCellPlayer -= yDirection;
    }
    else
    {
        framePlayer = 1 - framePlayer; // реальный шаг: 0 становится 1, а 1 становится 0
    }
}
