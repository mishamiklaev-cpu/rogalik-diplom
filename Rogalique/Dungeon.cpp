#include "Dungeon.h"
#include "Constants.h"

void Dungeon::Generate(int widthMap, int heightMap)
{

    widthField = widthMap; // поле создается и присуждается к размерам карты
    heightField = heightMap;

    cell.resize(heightField); // resize устанавливает ограничения в виде строк на поле

    for (int y = 0; y < heightField; y++)
    {
        cell[y].resize(widthField); // и суем на каждую строку ширину(столбцы)
    }

    // помещаем внутрь поля (строки+столбцы) объект - стены
    for (int yField = 0; yField < heightField; yField++)
    {
        for (int xField = 0; xField < widthField; xField++)
        {
            cell[yField][xField] = Tile::Wall;
        }
    }
}

void Dungeon::CarveRoom(const Room &room)
{
    if (room.xRoom + room.widthRoom <= widthField && room.yRoom + room.heightRoom <= heightField)

    {
        for (int yRoom

             = room.yRoom;
             yRoom < (room.yRoom + room.heightRoom); yRoom++)
        {

            for (int xRoom = room.xRoom; xRoom < (room.xRoom + room.widthRoom); xRoom++)
            {
                cell[yRoom][xRoom] = Tile::Floor;
            }
        }
    }
}

void Dungeon::CarveCorridor(int xRoomCentr1, int yRoomCentr1, int xRoomCentr2, int yRoomCentr2)
{
    // for (int x = xRoomCentr1; x <= xRoomCentr2;
    //	x++) {
    //	cell[yRoomCentr1][x] = Tile::Floor;
    // }
    // for (int y = yRoomCentr1; y <= yRoom2Centr2;
    //	y++) {
    //	cell[y][xRoomCentr2] = Tile::Floor;
    // }

    int x = xRoomCentr1;
    while (x != xRoomCentr2)
    {
        cell[yRoomCentr1][x] = Tile::Floor;
        if (x < xRoomCentr2)
        {
            x++;
        }
        else
        {
            x--;
        }
    }
    int y = yRoomCentr1;
    while (y != yRoomCentr2)
    {
        cell[y][xRoomCentr2] = Tile::Floor;
        if (y < yRoomCentr2)
        {
            y++;
        }
        else
        {
            y--;
        }
    }
}

void Dungeon::Draw(sf::RenderWindow &window)
{
    for (int yField = 0; yField < heightField; yField++)
    {
        for (int xField = 0; xField < widthField; xField++)
        {

            sf::RectangleShape objectField;
            objectField.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            objectField.setPosition(sf::Vector2f(xField * TILE_SIZE, yField * TILE_SIZE));

            if (cell[yField][xField] == Tile::Wall)
            {
                objectField.setFillColor(sf::Color::Red);
            }
            else if (cell[yField][xField] == Tile::Floor)
            {
                objectField.setFillColor(sf::Color::White);
            }
            window.draw(objectField);
        }
    }
}

bool Dungeon::isRoomCollide(const Room &a, const Room &b)
{
    // 1. Проверяем пересечение по оси X (горизонталь)
    bool CollideX = (a.xRoom < b.xRoom + b.widthRoom) && (a.xRoom + a.widthRoom > b.xRoom);
    // 2. Проверяем пересечение по оси Y (вертикаль)
    bool CollideY = (a.yRoom < b.yRoom + b.heightRoom) && (a.yRoom + a.heightRoom > b.yRoom);
    // 3. Если есть пересечение И там,И там — значит,комнаты наложились
    return CollideX && CollideY;
}
