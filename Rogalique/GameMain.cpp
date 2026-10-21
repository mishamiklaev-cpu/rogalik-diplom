#include "Constants.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "Player.h"
#include "Room.h"
#include <cmath>
#include <ctime>

int main()
{
    sf::Event event;
    // Game game; game.Run();

    // генератор случайных чисел
    sf::Clock clock;
    int seed = (int)time(nullptr); //??
    srand(seed);

    // генератор окна
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ROGALIC_PRACTICE_Dungeon");

    // создаем объект Dungeon, вызвать Generate Room.
    Dungeon dungeon;
    dungeon.Generate(CAMERA_CELL_WIDTH, CAMERA_CELL_HEIGHT);
    Room room;

    sf::View camera;                                           // создаем камеру
    camera.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT)); // ее размеры
    camera.setCenter(
        sf::Vector2f(static_cast<float>((CAMERA_CELL_WIDTH * TILE_SIZE) / 2.f),
                     static_cast<float>((CAMERA_CELL_HEIGHT * TILE_SIZE) / 2.f))); // центрируем

    // rand() % (b - a + 1) + a

    // генерируем 10 попыток создания непересекаю-ся комнат

    for (int attempt = 0; attempt < 10; attempt++)
    {
        // вызываем рандом room по ширине, высоте. room 4х8

        Room newRoom; // создали комнату
        newRoom.widthRoom = rand() % (widthNewRoom - heightNewRoom + 1) + heightNewRoom;
        newRoom.heightRoom = rand() % (widthNewRoom - heightNewRoom + 1) + heightNewRoom;

        // вызываем рандом местоположению room по x и y
        newRoom.xRoom = rand() % (CAMERA_CELL_WIDTH - newRoom.widthRoom) + 1;
        newRoom.yRoom = rand() % (CAMERA_CELL_HEIGHT - newRoom.heightRoom) + 1;

        bool noCollide = true;

        for (const Room &existingRoom : dungeon.rooms)
        {
            if (dungeon.isRoomCollide(newRoom, existingRoom))
            {
                noCollide = false;
                break;
            }
        }

        // если пересечений нет, добавляем комнату
        if (noCollide)
        {
            dungeon.rooms.push_back(newRoom);
            dungeon.CarveRoom(newRoom);
        }
    }

    for (size_t i = 0; i < dungeon.rooms.size() - 1; i++)
    {
        int xRoomCentr1 = dungeon.rooms[i].xRoom + (dungeon.rooms[i].widthRoom / 2);
        int yRoomCentr1 = dungeon.rooms[i].yRoom + (dungeon.rooms[i].heightRoom / 2);
        int xRoomCentr2 = dungeon.rooms[i + 1].xRoom + (dungeon.rooms[i + 1].widthRoom / 2);
        int yRoomCentr2 = dungeon.rooms[i + 1].yRoom + (dungeon.rooms[i + 1].heightRoom / 2);
        dungeon.CarveCorridor(xRoomCentr1, yRoomCentr1, xRoomCentr2, yRoomCentr2);
    }

    Player player;
    player.SpawnPlayerInRandomRoom(dungeon.rooms);

    Enemy enemy;
    int spawnAttempts = 0;
    int distance = 0;

    do
    {
        enemy.SpawnEnemy(dungeon.rooms);
        // Проверяем, не слишком ли близко к игроку
        distance = std::abs(enemy.xCellEnemy - player.xCellPlayer) +
                   std::abs(enemy.yCellEnemy - player.yCellPlayer);
        spawnAttempts++;

    } while (distance < 10 && spawnAttempts < 50);
    // Если ближе 10 клеток — переспавниваем

    float moveCooldown = 0.0f;
    float enemyCooldown = 0.0f;

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        float deltaTime = clock.restart().asSeconds();
        moveCooldown -= deltaTime;
        enemyCooldown -= deltaTime;

        if (moveCooldown <= 0.0f)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                player.MovePlayer(0, -1, dungeon);
                moveCooldown = 0.1f;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                player.MovePlayer(0, 1, dungeon);
                moveCooldown = 0.1f;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                player.MovePlayer(-1, 0, dungeon);
                moveCooldown = 0.1f;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                player.MovePlayer(1, 0, dungeon);
                moveCooldown = 0.1f;
            }
        }

        if (enemyCooldown <= 0.0f)
        {
            enemy.FindPlayer(player, dungeon);
            enemyCooldown = 0.15f;
        }

        camera.setCenter(sf::Vector2f(player.xCellPlayer * TILE_SIZE + (TILE_SIZE / 2.f),
                                      player.yCellPlayer * TILE_SIZE + (TILE_SIZE / 2.f)));

        window.clear();
        window.setView(camera);
        dungeon.Draw(window);
        player.DrawPlayer(window);
        enemy.DrawEnemy(window);
        window.display();
    }
}