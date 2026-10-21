#include "Enemy.h"
#include "Constants.h"
#include <cmath>
#include <cstdlib>

Enemy::Enemy()
{
    xCellEnemy = 0;
    yCellEnemy = 0;
    widthEnemy = 1;
    heightEnemy = 1;
}

void Enemy::SetPositionEnemy(int x, int y)
{
    xCellEnemy = x;
    yCellEnemy = y;
}

void Enemy::SpawnEnemy(const std::vector<Room> &rooms)
{
    int randomRoom = rand() % rooms.size();
    const Room &chosenRoom = rooms[randomRoom];

    xCellEnemy = chosenRoom.xRoom + (chosenRoom.widthRoom / 2);
    yCellEnemy = chosenRoom.yRoom + (chosenRoom.heightRoom / 2);
};

void Enemy::DrawEnemy(sf::RenderWindow &window)
{
    sf::RectangleShape EnemyShape;
    EnemyShape.setSize(sf::Vector2f(widthEnemy * TILE_SIZE, heightEnemy * TILE_SIZE));
    EnemyShape.setFillColor(sf::Color::Magenta);
    EnemyShape.setPosition(sf::Vector2f(xCellEnemy * TILE_SIZE, yCellEnemy * TILE_SIZE));
    window.draw(EnemyShape);
};

void Enemy::FindPlayer(const Player &player, const Dungeon &dungeon)
{
    int diffX = player.xCellPlayer - xCellEnemy;
    int diffY = player.yCellPlayer - yCellEnemy;

    int absX = std::abs(diffX);
    int absY = std::abs(diffY);

    // Определяем направление по X и Y
    int moveX = 0;
    int moveY = 0;

    if (diffX > 0)
        moveX = 1;
    else if (diffX < 0)
        moveX = -1;

    if (diffY > 0)
        moveY = 1;
    else if (diffY < 0)
        moveY = -1;

    // Пробуем сначала по оси с большим расстоянием
    if (absX > absY)
    {
        // ПРИОРИТЕТ: Ось X
        if (moveX != 0 && canMove(moveX, 0, dungeon))
        {
            MoveEnemy(moveX, 0, dungeon);
            return; // Успешно пошли по X, выходим
        }
        // Не получилось по X, пробуем Y
        if (moveY != 0 && canMove(0, moveY, dungeon))
        {
            MoveEnemy(0, moveY, dungeon);
            return;
        }
    }
    else
    {
        // ПРИОРИТЕТ: Ось Y
        if (moveY != 0 && canMove(0, moveY, dungeon))
        {
            MoveEnemy(0, moveY, dungeon);
            return; // Успешно пошли по Y, выходим
        }
        // Не получилось по Y, пробуем X
        if (moveX != 0 && canMove(moveX, 0, dungeon))
        {
            MoveEnemy(moveX, 0, dungeon);
            return;
        }
    }
}

bool Enemy::canMove(int xDirection, int yDirection, const Dungeon &dungeon)
{
    int newX = xCellEnemy + xDirection;
    int newY = yCellEnemy + yDirection;

    // Проверяем границы карты
    if (newX < 0 || newX >= dungeon.widthField || newY < 0 || newY >= dungeon.heightField)
    {
        return false;
    }

    // Проверяем, не стена ли там
    if (dungeon.cell[newY][newX] == Tile::Wall)
    {
        return false;
    }

    return true; // Можно двигаться!
}

void Enemy::MoveEnemy(int xDirection, int yDirection, const Dungeon &dungeon)
{
    xCellEnemy += xDirection;
    yCellEnemy += yDirection;

    if (xCellEnemy < 0 || xCellEnemy >= dungeon.widthField || yCellEnemy < 0 ||
        yCellEnemy >= dungeon.heightField || dungeon.cell[yCellEnemy][xCellEnemy] == Tile::Wall)
    {
        xCellEnemy -= xDirection;
        yCellEnemy -= yDirection;
    }
}
