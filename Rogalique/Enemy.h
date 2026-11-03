#pragma once
#include "Door.h"
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
    int damageEnemy;      // урон за удар
    float stepTimeEnemy;  // пауза между шагами: чем больше, тем медленнее вид
    float stepTimerEnemy; // сколько осталось до следующего шага

    // поставить на заданную клетку: откат шага на занятую клетку, задел под «продолжить игру»
    void SetPositionEnemy(int xCellEnemy, int yCellEnemy);
    void SpawnEnemy(const std::vector<Room> &rooms);
    void DrawEnemy(sf::RenderWindow &window);
    // чисто виртуальная (= 0): своего поиска у базового врага нет, поэтому
    // просто Enemy создать нельзя — только конкретный вид
    virtual void FindPlayer(const Player &player, const Dungeon &dungeon, const Door &door) = 0;
    // шаг возможен, если клетка на карте, не стена и не дверь (дверь только для игрока)
    bool CanMove(int xDirection, int yDirection, const Dungeon &dungeon, const Door &door);
    void MoveEnemy(int xDirection, int yDirection);
};

// ищейка: знает, где игрок, и идёт к нему кратчайшим путём (BFS)
class EnemyHound : public Enemy
{
  public:
    EnemyHound();
    void FindPlayer(const Player &player, const Dungeon &dungeon, const Door &door) override;
};

// шатун: видит игрока по прямой под любым углом, если на линии нет стен;
// увидел — шагает к нему, не видит — стоит
class EnemyShatun : public Enemy
{
  public:
    EnemyShatun();
    void FindPlayer(const Player &player, const Dungeon &dungeon, const Door &door) override;
};
