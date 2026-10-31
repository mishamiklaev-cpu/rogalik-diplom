#include "Enemy.h"
#include "Constants.h"
#include <cmath>
#include <cstdlib>
#include <queue>

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
    sf::RectangleShape enemyShape;
    enemyShape.setSize(sf::Vector2f(widthEnemy * TILE_SIZE, heightEnemy * TILE_SIZE));
    enemyShape.setFillColor(sf::Color::Magenta);
    enemyShape.setPosition(sf::Vector2f(xCellEnemy * TILE_SIZE, yCellEnemy * TILE_SIZE));
    window.draw(enemyShape);
};

void Enemy::FindPlayer(const Player &player, const Dungeon &dungeon)
{
    // направление искать уже не нужно
    if (xCellEnemy == player.xCellPlayer && yCellEnemy == player.yCellPlayer)
    {
        return;
    }

    // BFS 
    // расстояние: сколько шагов от этой клетки до игрока по проходам.
    // dist = -1 значит "волна сюда не дошл" (стены не пускают)
    const int heightField = dungeon.heightField;
    const int widthField = dungeon.widthField;

    std::vector<std::vector<int>> dist(heightField, std::vector<int>(widthField, -1)); //30 строк, 40 столбцов
    std::queue<std::pair<int, int>> waveCell; // очередь: клетки, откуда волна пойдёт дальше

    dist[player.yCellPlayer][player.xCellPlayer] = 0;//позиция игрока 
    waveCell.push({ player.xCellPlayer, player.yCellPlayer });//суем его в fifo 

    // шаги волны в четыре стороны (без диагоналей)
    const int stepDx[4] = { 1, -1, 0, 0 };
    const int stepDy[4] = { 0, 0, 1, -1 };

    while (!waveCell.empty())
    {
        std::pair<int, int> pairXYCell = waveCell.front();
        waveCell.pop(); //в fifo front положил, pop удалил 

        for (int i = 0; i < 4; i++)
        {
            int nx = pairXYCell.first + stepDx[i];
            int ny = pairXYCell.second + stepDy[i];

            if (nx < 0 || nx >= widthField || ny < 0 ||
                ny >= heightField) // если итог шага за пределами карты, то волна туда не пойдёт
            {
                continue; // край карты
            }
            if (dungeon.cell[ny][nx] == Tile::Wall)
            {
                continue; // в стену волна не льётся
            }
            if (dist[ny][nx] != -1)
            {
                continue; // клетка уже отмечена 
            }

            dist[ny][nx] = dist[pairXYCell.second][pairXYCell.first] + 1;//преписан шаг расстояния 
            waveCell.push({ nx, ny });//снова в fifo  для продолжения 
        }
    }

    // среди четырёх соседей врага выбираем наименьшее расстояние
    int bestStep = -1;
    int bestDx = 0;
    int bestDy = 0;

    for (int i = 0; i < 4; i++)
    {
        if (!CanMove(stepDx[i], stepDy[i], dungeon))
        {
            continue; // в стену или за край — не ход
        }

        int ny = yCellEnemy + stepDy[i];
        int nx = xCellEnemy + stepDx[i];

        if (dist[ny][nx] == -1)
        {
            continue; // клетка в закрытой зоне — до него нет пути вообще
        }

        if (bestStep == -1 || dist[ny][nx] < bestStep)
        {
            bestStep = dist[ny][nx];
            bestDx = stepDx[i];
            bestDy = stepDy[i];
        }
    }

    if (bestStep != -1)
    {
        MoveEnemy(bestDx, bestDy, dungeon);
    }
}

bool Enemy::CanMove(int xDirection, int yDirection, const Dungeon &dungeon)
{
    int newX = xCellEnemy + xDirection;
    int newY = yCellEnemy + yDirection;

    if (newX < 0 || newX >= dungeon.widthField || newY < 0 || newY >= dungeon.heightField)
    {
        return false;
    }

    if (dungeon.cell[newY][newX] == Tile::Wall)
    {
        return false;
    }

    return true; 
}

void Enemy::MoveEnemy(int xDirection, int yDirection, const Dungeon &dungeon)
{
    xCellEnemy += xDirection;
    yCellEnemy += yDirection;
}
