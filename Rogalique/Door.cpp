#include "Door.h"
#include "Constants.h"
#include <cmath>
#include <cstdlib>

void Door::SpawnDoor(const std::vector<Room> &rooms, int playerX, int playerY)
{
    int attempts = 0;
    int distance = 0;

    do
    {
        // случайная комната из списка; выход ставим в её центр,
        // так дверь всегда в проходимой зоне карты
        int roomIndex = rand() % static_cast<int>(rooms.size());
        xCellDoor = rooms[roomIndex].xRoom + rooms[roomIndex].widthRoom / 2;
        yCellDoor = rooms[roomIndex].yRoom + rooms[roomIndex].heightRoom / 2;

        // та же метрика, что в спавне врага: расстояние по осям (манхэттенское)
        distance = std::abs(xCellDoor - playerX) + std::abs(yCellDoor - playerY);
        attempts++;

    } while (distance < 10 && attempts < 50);
}

void Door::DrawDoor(sf::RenderWindow &window)
{
    // дверь занимает ровно одну клетку — размер с тайл карты
    sf::RectangleShape doorShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    doorShape.setFillColor(sf::Color::Black); 
    doorShape.setPosition(sf::Vector2f(xCellDoor * TILE_SIZE, yCellDoor * TILE_SIZE));
    window.draw(doorShape);
}
