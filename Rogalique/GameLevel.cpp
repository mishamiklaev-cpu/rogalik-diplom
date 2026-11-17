#include "GameLevel.h"
#include "Constants.h"
#include <cmath>

void SpawnPotions(std::vector<Potion> &potions, const std::vector<Room> &rooms,
                  const Player &player, const Door &door, int level)
{
    potions.clear(); // перегенерация уровня — старые зелья удаляются
    int count = 0;
    if (level >= 8)
    {
        count = 2;
    }
    else if (level >= 4)
    {
        count = 1;
    }

    for (int i = 0; i < count; i++)
    {
        int attempts = 0;
        bool placed = false;
        while (!placed && attempts < 50)
        {
            Potion potion;
            potion.SpawnPotion(rooms);

            bool sameCell = false;
            if (potion.xPotion == player.xCellPlayer
                && potion.yPotion == player.yCellPlayer)
            {
                sameCell = true; // зелье под ногами игрока — нечестно
            }
            if (potion.xPotion == door.xCellDoor
                && potion.yPotion == door.yCellDoor)
            {
                sameCell = true; // зелье на клетке выхода — дверь скрыта, запутает
            }
            for (const Potion &already : potions)
            {
                if (potion.xPotion == already.xPotion
                    && potion.yPotion == already.yPotion)
                {
                    sameCell = true; // два зелья в одной клетке — мыльно и путает
                }
            }

            if (!sameCell)
            {
                potions.push_back(potion);
                placed = true;
            }
            attempts++; // защита от цикла: максимум 50 попыток на зелье
        }
    }
}

void SpawnEnemies(std::vector<std::unique_ptr<Enemy>> &enemies,
                  const Dungeon &dungeon, const Player &player, const Door &door,
                  int level)
{
    enemies.clear();

    // ищейки: до 10 уровня нет, 10-14 — одна, с 15 — две
    int countHound = 0;
    if (level >= 15)
    {
        countHound = 2;
    }
    else if (level >= 10)
    {
        countHound = 1;
    }

    // шатуны: 1 ур — 1, 2-3 — 2, 4 — 3, 5-7 — 4, с 8 — 5
    int countShatun = 1;
    if (level >= 8)
    {
        countShatun = 5;
    }
    else if (level >= 5)
    {
        countShatun = 4;
    }
    else if (level >= 4)
    {
        countShatun = 3;
    }
    else if (level >= 2)
    {
        countShatun = 2;
    }

    for (int i = 0; i < countHound; i++)
    {
        enemies.push_back(std::make_unique<EnemyHound>());
    }
    for (int i = 0; i < countShatun; i++)
    {
        enemies.push_back(std::make_unique<EnemyShatun>());
    }

    for (int i = 0; i < enemies.size(); i++)
    {
        int spawnAttempts = 0;
        int distance = 0;
        bool badCell = false;
        do
        {
            enemies[i]->SpawnEnemy(dungeon.rooms);
            int xCell = enemies[i]->xCellEnemy;
            int yCell = enemies[i]->yCellEnemy;

            // проверяем, не слишком ли близко к игроку
            distance = std::abs(xCell - player.xCellPlayer) +
                       std::abs(yCell - player.yCellPlayer);

            badCell = false;
            if (xCell < 0 || xCell >= dungeon.widthField
                || yCell < 0 || yCell >= dungeon.heightField)
            {
                badCell = true; // за краем карты
            }
            else if (dungeon.cell[yCell][xCell] != Tile::Floor)
            {
                badCell = true; // в стене — в том числе у края карты
            }
            if (distance == 0)
            {
                badCell = true; // враг на клетке игрока — урон с первого кадра
            }
            if (xCell == door.xCellDoor && yCell == door.yCellDoor)
            {
                badCell = true; // враг на двери — дверь скрыта
            }
            for (int j = 0; j < i; j++) // только уже поставленные враги
            {
                if (xCell == enemies[j]->xCellEnemy && yCell == enemies[j]->yCellEnemy)
                {
                    badCell = true; // два врага в одной клетке — выглядят как один
                }
            }
            spawnAttempts++;

            // «не ближе 10 клеток» — желательно (50 попыток, потом соглашаемся),
            // а стена, клетка игрока, дверь и чужой враг — запрет без лимита попыток:
            // в самой маленькой комнате 4х4 = 16 клеток пола, а занято максимум 9
            // (игрок, дверь, 7 врагов), поэтому свободная клетка всегда найдётся
        } while (badCell || (distance < 10 && spawnAttempts < 50));
    }
}

void StartNextLevel(Dungeon &dungeon, Player &player,
                    std::vector<std::unique_ptr<Enemy>> &enemies, Door &door,
                    std::vector<Potion> &potions, int level)
{
    dungeon.GenerateLevel(MAP_CELLS_WIDTH, MAP_CELLS_HEIGHT);
    player.SpawnPlayerInRandomRoom(dungeon.rooms);

    // дверь нового уровня меряем от нового игрока
    door.SpawnDoor(dungeon.rooms, player.xCellPlayer, player.yCellPlayer);
    SpawnEnemies(enemies, dungeon, player, door, level);

    // зелья нового уровня (количество зависит от номера уровня)
    SpawnPotions(potions, dungeon.rooms, player, door, level);
}
