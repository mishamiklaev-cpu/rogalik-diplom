#include "Dungeon.h"
#include "Constants.h"
#include <cstdlib>

void Dungeon::GenerateField(int widthMap, int heightMap)
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

void Dungeon::GenerateLevel(int widthMap, int heightMap)
{
    // пустое поле целиком из стен
    GenerateField(widthMap, heightMap);

    // на новом уровне не должны остаться комнаты предыдущего 
    rooms.clear();

    // генерируем 10 попыток создания непересекающихся комнат
    for (int attempt = 0; attempt < 10; attempt++)
    {
        // случайная комната 4х8
        Room newRoom;
        newRoom.widthRoom = rand() % (widthNewRoom - heightNewRoom + 1) + heightNewRoom;
        newRoom.heightRoom = rand() % (widthNewRoom - heightNewRoom + 1) + heightNewRoom;

        // случайное местоположение комнаты по x и y
        newRoom.xRoom = rand() % (CAMERA_CELL_WIDTH - newRoom.widthRoom) + 1;
        newRoom.yRoom = rand() % (CAMERA_CELL_HEIGHT - newRoom.heightRoom) + 1;

        bool noCollide = true;

        for (const Room &existingRoom : rooms)
        {
            if (IsRoomCollide(newRoom, existingRoom))
            {
                noCollide = false;
                break;
            }
        }

        // если пересечений нет — добавляем комнату и вырезаем её в поле
        if (noCollide)
        {
            rooms.push_back(newRoom);
            CarveRoom(newRoom);
        }
    }

    // коридоры между центрами соседних комнат
    for (size_t i = 0; i < rooms.size() - 1; i++)
    {
        int xRoomCentr1 = rooms[i].xRoom + (rooms[i].widthRoom / 2);
        int yRoomCentr1 = rooms[i].yRoom + (rooms[i].heightRoom / 2);
        int xRoomCentr2 = rooms[i + 1].xRoom + (rooms[i + 1].widthRoom / 2);
        int yRoomCentr2 = rooms[i + 1].yRoom + (rooms[i + 1].heightRoom / 2);
        CarveCorridor(xRoomCentr1, yRoomCentr1, xRoomCentr2, yRoomCentr2);
    }
}

void Dungeon::CarveRoom(const Room &room)
{
    if (room.xRoom + room.widthRoom <= widthField && room.yRoom + room.heightRoom <= heightField)

    {
        for (int yRoom = room.yRoom;
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

bool Dungeon::IsRoomCollide(const Room &a, const Room &b)
{
    // 1. Проверяем пересечение по оси X (горизонталь)
    bool CollideX = (a.xRoom < b.xRoom + b.widthRoom) && (a.xRoom + a.widthRoom > b.xRoom);
    // 2. Проверяем пересечение по оси Y (вертикаль)
    bool CollideY = (a.yRoom < b.yRoom + b.heightRoom) && (a.yRoom + a.heightRoom > b.yRoom);
    // 3. Если есть пересечение И там,И там — значит,комнаты наложились
    return CollideX && CollideY;
}
