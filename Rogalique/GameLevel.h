#pragma once
#include "Door.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "Player.h"
#include "Potion.h"
#include "Room.h"
#include <memory>
#include <vector>

// раскладка уровня: кто и где стоит на новой карте.
// Порядок важен: карта -> игрок -> дверь -> враги -> зелья, каждый следующий обходит предыдущих

// зелья здоровья: 1-3 уровень — нет, 4-7 — одно, с 8 — два. Спавним в случайные комнаты,
// проверяем, чтобы клетка не совпала с игроком, дверью выхода и уже
// поставленными зельями
void SpawnPotions(std::vector<Potion> &potions, const std::vector<Room> &rooms,
                  const Player &player, const Door &door, int level);

// враги уровня: число ищеек и шатунов зависит от номера уровня.
// В векторе лежат unique_ptr — указатель владеет врагом, и clear() сам
// освобождает память старых врагов.
// Враги ставятся после двери: клетку игрока, двери и уже поставленных
// врагов надо обходить, а встать можно только на пол
void SpawnEnemies(std::vector<std::unique_ptr<Enemy>> &enemies,
                  const Dungeon &dungeon, const Player &player, const Door &door,
                  int level);

// переход на новый уровень: обновляем весь мир за один кадр.
void StartNextLevel(Dungeon &dungeon, Player &player,
                    std::vector<std::unique_ptr<Enemy>> &enemies, Door &door,
                    std::vector<Potion> &potions, int level);
