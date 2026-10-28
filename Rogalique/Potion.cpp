#include "Potion.h"

// зелье появляется в случайной комнате, в её центре
void Potion::SpawnPotion(const std::vector<Room> &rooms)
{
    int randomRoom = rand() % rooms.size();
    const Room &chosenRoom = rooms[randomRoom];

    xPotion = chosenRoom.xRoom + (chosenRoom.widthRoom / 2);
    yPotion = chosenRoom.yRoom + (chosenRoom.heightRoom / 2);
}

void Potion::DrawPotion(sf::RenderWindow &window) const
{
    // зелье меньше игрока и врага: квадрат в полклетки,
    // нарисован в центре своей клетки
    sf::RectangleShape potionShape(sf::Vector2f(TILE_SIZE / 2.f, TILE_SIZE / 2.f));
    potionShape.setFillColor(sf::Color(240, 180, 40)); // золотистая фляжка
    potionShape.setPosition(
        xPotion * TILE_SIZE + TILE_SIZE / 4.0f,
        yPotion * TILE_SIZE + TILE_SIZE / 4.0f);
    window.draw(potionShape);
}
