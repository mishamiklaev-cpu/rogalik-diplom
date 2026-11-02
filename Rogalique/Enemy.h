#pragma once
#include "Dungeon.h"
#include "Player.h"
#include "Room.h"
#include <SFML/Graphics.hpp>
#include <vector>

// базовый враг: общее для всех видов — клетка, спавн, отрисовка, шаг.
// Как искать игрока, решает каждый вид сам (FindPlayer)
class Enemy
{
  public:
    Enemy();
    // виртуальный деструктор: враг-наследник удаляется через указатель на Enemy (unique_ptr<Enemy>)
    virtual ~Enemy() {}

    int xCellEnemy;
    int yCellEnemy;
    int widthEnemy;
    int heightEnemy;
    sf::Color colorEnemy; // цвет вида врага: задаёт конструктор наследника

    // пока не вызывается: задел под «продолжить игру» и новых врагов — поставить на заданную клетку
    void SetPositionEnemy(int xCellEnemy, int yCellEnemy);
    void SpawnEnemy(const std::vector<Room> &rooms);
    void DrawEnemy(sf::RenderWindow &window);
    // чисто виртуальная (= 0): своего поиска у базового врага нет, поэтому
    // просто Enemy создать нельзя — только конкретный вид
    virtual void FindPlayer(const Player &player, const Dungeon &dungeon) = 0;
    bool CanMove(int xDirection, int yDirection, const Dungeon &dungeon);
    void MoveEnemy(int xDirection, int yDirection);
};

// ищейка: знает, где игрок, и идёт к нему кратчайшим путём (BFS)
class EnemyHound : public Enemy
{
  public:
    EnemyHound();
    void FindPlayer(const Player &player, const Dungeon &dungeon) override;
};