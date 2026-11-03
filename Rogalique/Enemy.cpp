#include "Enemy.h"
#include "Constants.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <queue>

Enemy::Enemy()
{
    xCellEnemy = 0;
    yCellEnemy = 0;
    widthEnemy = 1;
    heightEnemy = 1;
    colorEnemy = sf::Color::White; // наследник перекрасит в свой цвет
    damageEnemy = DAMAGE_ON_CATCH;
    stepTimeEnemy = STEP_TIME_HOUND;
    stepTimerEnemy = 0.0f;
}

EnemyHound::EnemyHound()
{
    // сначала уже отработал конструктор Enemy, здесь только своё
    colorEnemy = sf::Color::Magenta;
    damageEnemy = DAMAGE_ON_CATCH;
    stepTimeEnemy = STEP_TIME_HOUND;
}

EnemyShatun::EnemyShatun()
{
    colorEnemy = sf::Color(128, 128, 128); // серый
    damageEnemy = DAMAGE_SHATUN;
    stepTimeEnemy = STEP_TIME_SHATUN;
}

void EnemyShatun::FindPlayer(const Player &player, const Dungeon &dungeon, const Door &door)
{
    // уже стоит на клетке игрока — шагать некуда
    if (xCellEnemy == player.xCellPlayer && yCellEnemy == player.yCellPlayer)
    {
        return;
    }

    int xDistance = player.xCellPlayer - xCellEnemy;
    int yDistance = player.yCellPlayer - yCellEnemy;

    // взгляд: идём по отрезку от шатуна к игроку равными шагами (по шагу на клетку
    // вдоль длинной оси) и проверяем клетку под каждой точкой; стена — взгляд упёрся
    int lookSteps = std::max(std::abs(xDistance), std::abs(yDistance));
    for (int k = 1; k < lookSteps; k++)
    {
        int xCellLook = static_cast<int>(std::round(xCellEnemy + xDistance * k / static_cast<float>(lookSteps)));
        int yCellLook = static_cast<int>(std::round(yCellEnemy + yDistance * k / static_cast<float>(lookSteps)));
        if (dungeon.cell[yCellLook][xCellLook] == Tile::Wall)
        {
            return; // не видит — стоит
        }
    }

    // видит — шаг к игроку, как у всех, только по четырём сторонам
    int xDirection = 0;
    int yDirection = 0;
    if (xDistance > 0)
    {
        xDirection = 1;
    }
    else if (xDistance < 0)
    {
        xDirection = -1;
    }
    if (yDistance > 0)
    {
        yDirection = 1;
    }
    else if (yDistance < 0)
    {
        yDirection = -1;
    }

    // сначала по оси, где до игрока дальше; там стена или дверь — пробуем другую ось
    if (std::abs(xDistance) >= std::abs(yDistance))
    {
        if (xDirection != 0 && CanMove(xDirection, 0, dungeon, door))
        {
            MoveEnemy(xDirection, 0);
        }
        else if (yDirection != 0 && CanMove(0, yDirection, dungeon, door))
        {
            MoveEnemy(0, yDirection);
        }
    }
    else
    {
        if (yDirection != 0 && CanMove(0, yDirection, dungeon, door))
        {
            MoveEnemy(0, yDirection);
        }
        else if (xDirection != 0 && CanMove(xDirection, 0, dungeon, door))
        {
            MoveEnemy(xDirection, 0);
        }
    }
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

    // случайная клетка комнаты, а не центр: центров на всех не хватает
    // (в центрах стоят игрок и дверь, а врагов на уровне до 7)
    xCellEnemy = chosenRoom.xRoom + rand() % chosenRoom.widthRoom;
    yCellEnemy = chosenRoom.yRoom + rand() % chosenRoom.heightRoom;
};

void Enemy::DrawEnemy(sf::RenderWindow &window)
{
    sf::RectangleShape enemyShape;
    enemyShape.setSize(sf::Vector2f(widthEnemy * TILE_SIZE, heightEnemy * TILE_SIZE));
    enemyShape.setFillColor(colorEnemy);
    enemyShape.setPosition(sf::Vector2f(xCellEnemy * TILE_SIZE, yCellEnemy * TILE_SIZE));
    window.draw(enemyShape);
};

void EnemyHound::FindPlayer(const Player &player, const Dungeon &dungeon, const Door &door)
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
            if (nx == door.xCellDoor && ny == door.yCellDoor)
            {
                continue; // через дверь враги не ходят — волна её обтекает
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
        if (!CanMove(stepDx[i], stepDy[i], dungeon, door))
        {
            continue; // в стену, на дверь или за край — не ход
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
        MoveEnemy(bestDx, bestDy);
    }
}

bool Enemy::CanMove(int xDirection, int yDirection, const Dungeon &dungeon, const Door &door)
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

    if (newX == door.xCellDoor && newY == door.yCellDoor)
    {
        return false; // дверь — выход только для игрока
    }

    return true; 
}

// стену и дверь здесь не проверяем: шаг уже проверен в CanMove до вызова
void Enemy::MoveEnemy(int xDirection, int yDirection)
{
    xCellEnemy += xDirection;
    yCellEnemy += yDirection;
}
