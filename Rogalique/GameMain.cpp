#include "Constants.h"
#include "Door.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "Player.h"
#include "Room.h"
#include <cmath>
#include <ctime>

// переход на новый уровень: обновляем весь мир за один кадр.
void StartNextLevel(Dungeon &dungeon, Player &player, Enemy &enemy, Door &door)
{
    dungeon.GenerateLevel(MAP_CELLS_WIDTH, MAP_CELLS_HEIGHT);
    player.SpawnPlayerInRandomRoom(dungeon.rooms);

    int spawnAttempts = 0;
    int distance = 0;
    do
    {
        enemy.SpawnEnemy(dungeon.rooms);
        distance = std::abs(enemy.xCellEnemy - player.xCellPlayer) +
                   std::abs(enemy.yCellEnemy - player.yCellPlayer);
        spawnAttempts++;

    } while (distance < 10 && spawnAttempts < 50);

    // дверь нового уровня меряем от нового игрока
    door.SpawnDoor(dungeon.rooms, player.xCellPlayer, player.yCellPlayer);
}

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

    // создаем объект Dungeon: уровень собирается одним методом класса
    Dungeon dungeon;
    dungeon.GenerateLevel(MAP_CELLS_WIDTH, MAP_CELLS_HEIGHT);

    sf::View camera;  // создаем камеру
    camera.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT)); // ее размеры
    camera.setCenter(
        sf::Vector2f(static_cast<float>((MAP_CELLS_WIDTH * TILE_SIZE) / 2.f),
                     static_cast<float>((MAP_CELLS_HEIGHT * TILE_SIZE) / 2.f))); // центрируем

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

    // выход с уровня: ставится после спавна игрока, чтобы знать, от кого мерить дистанцию
    Door door;
    door.SpawnDoor(dungeon.rooms, player.xCellPlayer, player.yCellPlayer);

    float moveCooldown = 0.0f;
    float enemyCooldown = 0.0f;

    // переход на новый уровень: сразу после касания весь мир замирает,
    // таймер держит паузу — задел под будущую музыку/анимацию перехода
    bool FreezeTime = false;
    float FreezeTimeTimer = 0.0f;

    // ступени мигания: каждые FADE_STEP_TIME секунд альфа меняется на один «квадрат»
    const float FADE_STEP = 255.0f / 5.0f; // пять ступеней: светлый... темнее... чёрный
    const float FADE_STEP_TIME = 0.125f;
    float fadeBlack= 0.0f; // 0 = прозрачно, 255 = полный чёрный
    int switchDarkness = 0;  // 0 — не мигаем, +1 — затемняемся, -1 — осветляемся
    float fadeStep= 0.0f;

    // «занавес»: чёрный прямоугольник поверх всего экрана,
    // видимой становится только его прозрачность
    sf::RectangleShape fadeShape(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
                                              static_cast<float>(WINDOW_HEIGHT)));
    fadeShape.setFillColor(sf::Color(0, 0, 0, 0));

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

        if (!FreezeTime && moveCooldown <= 0.0f)
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

        // игрок встал на выход — переход запускается один раз и дальше идёт по таймеру
        if (!FreezeTime && player.xCellPlayer == door.xCellDoor && player.yCellPlayer == door.yCellDoor)
        {
            FreezeTime = true;
            FreezeTimeTimer = 3.0f;
        }

        if (FreezeTime)
        {
            if (FreezeTimeTimer > 0.0f)
            {
                // фаза 1: пауза под будущую музыку перехода, мир заморожен
                FreezeTimeTimer -= deltaTime;
                if (FreezeTimeTimer <= 0.0f)
                {
                    switchDarkness = 1; // пауза кончилась — начинаем затемнение
                    fadeStep= FADE_STEP_TIME;
                }
            }
            else
            {
                // фаза 2/3: ступени затемнения, смена мира под чёрным, осветление
                fadeStep-= deltaTime;
                if (fadeStep<= 0.0f)
                {
                    fadeStep= FADE_STEP_TIME;
                    fadeBlack+= switchDarkness * FADE_STEP;

                    if (fadeBlack>= 255.0f)
                    {
                        // экран полностью чёрный — вот здесь меняется мир, глаз не видит подмены
                        fadeBlack= 255.0f;
                        StartNextLevel(dungeon, player, enemy, door);
                        switchDarkness = -1;
                    }
                    else if (fadeBlack<= 0.0f)
                    {
                        fadeBlack= 0.0f;
                        switchDarkness = 0;
                        FreezeTime = false; // переход завершён
                    }
                }
            }
        }

        if (!FreezeTime && enemyCooldown <= 0.0f)
        {
            enemy.FindPlayer(player, dungeon);
            enemyCooldown = 0.15f;
        }

        //проверяем, догнал ли враг игрока 
        bool enemyCatchesPlayer = (enemy.xCellEnemy == player.xCellPlayer
            && enemy.yCellEnemy == player.yCellPlayer);

        // позиция камеры: центр клетки игрока
        sf::Vector2f cameraPosition(player.xCellPlayer* TILE_SIZE + (TILE_SIZE / 2.f),
            player.yCellPlayer* TILE_SIZE + (TILE_SIZE / 2.f));

        // зажим камеры в границы мира: центр не выйдет за пол-окна от края
        // Зажим идёт ДО тряски — базовая позиция всегда внутри мира
        const float edgeScreenX = WINDOW_WIDTH / 2.0f;
        const float edgeScreenY = WINDOW_HEIGHT / 2.0f;
        const float worldWidth = dungeon.widthField * TILE_SIZE;
        const float worldHeight = dungeon.heightField * TILE_SIZE;

        cameraPosition.x = std::max(edgeScreenX, std::min(cameraPosition.x, worldWidth - edgeScreenX));
        cameraPosition.y = std::max(edgeScreenY, std::min(cameraPosition.y, worldHeight - edgeScreenY));

        // тряска при поимке: случайный сдвиг от -10 до +10 по обеим осям.
        // отражаем его в противоположную сторону: дрожь остаётся полной
        // даже у края карты и черноту не показывает
        if (enemyCatchesPlayer)
        {
            int shakeX = rand() % 21 - 10;
            int shakeY = rand() % 21 - 10;

            if (cameraPosition.x + shakeX < edgeScreenX
                || cameraPosition.x + shakeX > worldWidth - edgeScreenX)
            {
                shakeX = -shakeX;
            }
            if (cameraPosition.y + shakeY < edgeScreenY
                || cameraPosition.y + shakeY > worldHeight - edgeScreenY)
            {
                shakeY = -shakeY;
            }

            cameraPosition.x += static_cast<float>(shakeX);
            cameraPosition.y += static_cast<float>(shakeY);
        }

        camera.setCenter(cameraPosition);

        window.clear();
        window.setView(camera);
        dungeon.Draw(window);
        door.DrawDoor(window); // выход рисуем под юнитами: пол → дверь → игрок → враг
        player.DrawPlayer(window);
        enemy.DrawEnemy(window);

        // затемнение рисуется последним и в экранных координатах:
        // переключаемся на камеру по умолчанию, чтобы занавес накрыл ровно весь экран
        window.setView(window.getDefaultView());
        fadeShape.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(fadeBlack)));
        window.draw(fadeShape);

        window.display();
    }
}