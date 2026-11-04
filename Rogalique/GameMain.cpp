#include "Constants.h"
#include "Door.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "GameTextures.h"
#include "Particle.h"
#include "Player.h"
#include "Potion.h"
#include "Room.h"
#include <cmath>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

// вспышка частиц из точки события:
// разлёт в случайные стороны, позиции слегка разбросаны вокруг точки
void SpawnBurst(std::vector<Particle> &particles, float xPoint, float yPoint,
                sf::Color color)
{
    for (int i = 0; i < PARTICLE_COUNT; i++)
    {
        Particle particle;
        particle.xParticle = xPoint + static_cast<float>(rand() % 21 - 10);
        particle.yParticle = yPoint + static_cast<float>(rand() % 21 - 10);
        particle.dxParticle = static_cast<float>(rand() % 101 - 50); // -50..+50 px/c
        particle.dyParticle = static_cast<float>(rand() % 101 - 50);
        particle.lifeParticle = PARTICLE_LIFE_TIME;
        particle.colorParticle = color;
        particles.push_back(particle);
    }
}

// зелья здоровья: 1-3 уровень — нет, 4-7 — одно, с 8 — два. Спавним в случайные комнаты,
// проверяем, чтобы клетка не совпала с игроком, дверью выхода и уже
// поставленными зельями
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

// враги уровня: число ищеек и шатунов зависит от номера уровня.
// В векторе лежат unique_ptr — указатель владеет врагом, и clear() сам
// освобождает память старых врагов.
// Враги ставятся после двери: клетку игрока, двери и уже поставленных
// врагов надо обходить, а встать можно только на пол
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

// переход на новый уровень: обновляем весь мир за один кадр.
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

int main()
{
    sf::Event event;

    // генератор случайных чисел
    sf::Clock clock;
    int seed = (int)time(nullptr); // текущее время: каждый запуск — новое зерно, значит новая карта
    srand(seed);

    // генератор окна
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Dungeon Escape");

    // создаем объект Dungeon: уровень собирается одним методом класса
    Dungeon dungeon;
    dungeon.GenerateLevel(MAP_CELLS_WIDTH, MAP_CELLS_HEIGHT);

    sf::View camera;  // создаем камеру
    // вид камеры = нижние 11/12 окна: верхняя 1/12 отдана панели HUD, поэтому игровой мир никогда не рисуется под панелью
    camera.setSize(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
                                static_cast<float>(WINDOW_HEIGHT) * 11.0f / 12.0f));
    camera.setViewport(sf::FloatRect(0.0f, 1.0f / 12.0f, 1.0f, 11.0f / 12.0f));
    camera.setCenter(
        sf::Vector2f(static_cast<float>((MAP_CELLS_WIDTH * TILE_SIZE) / 2.f),
                     static_cast<float>((MAP_CELLS_HEIGHT * TILE_SIZE) / 2.f))); // центрируем

    Player player;
    player.SpawnPlayerInRandomRoom(dungeon.rooms);

    // выход с уровня: ставится после спавна игрока, чтобы знать, от кого мерить дистанцию
    Door door;
    door.SpawnDoor(dungeon.rooms, player.xCellPlayer, player.yCellPlayer);

    // счётчик уровня объявлен заранее: его читают враги и зелья при раскладке
    int levelNumber = 1;               // номер текущего уровня (расписание спавна)

    // все враги уровня (после двери — чтобы не встать на неё);
    // указатели — потому что в векторе разные наследники Enemy
    std::vector<std::unique_ptr<Enemy>> enemies;
    SpawnEnemies(enemies, dungeon, player, door, levelNumber);

    // вектор зелий уровня: количество зависит от номера уровня (см. SpawnPotions)
    std::vector<Potion> potions;
    SpawnPotions(potions, dungeon.rooms, player, door, levelNumber);

    // живые частицы: рождаются при событиях (удар, зелье), умирают по таймеру
    std::vector<Particle> particles;

    float moveCooldown = 0.0f; // у врагов таймер шага свой (stepTimerEnemy): виды ходят с разной скоростью

    // переход на новый уровень: сразу после касания весь мир замирает,
    // таймер держит паузу — задел под будущую музыку/анимацию перехода
    bool freezeTime = false;
    float freezeTimeTimer = 0.0f;

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

    // текстуры: загружаем один раз до игрового цикла, рисование только использует готовые
    GameTextures textures;
    textures.LoadTextures();

    // шрифт для игровой надписи и экрана смерти.
    sf::Font font;
    bool fontLoaded = font.loadFromFile("Fonts/Roboto-Black.ttf");
    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile("Resources/Fonts/Roboto-Black.ttf");
    }

    // индикатор здоровья: значение читается прямо из игрока
    sf::Text hpText("HP: " + std::to_string(player.hpPlayer), font, 24);
    hpText.setFillColor(sf::Color::White);
    hpText.setPosition(10.f, 13.f);

    // счётчик уровня уже объявлен выше (зелья его читали): здесь только надпись
    sf::Text levelText("Level: " + std::to_string(levelNumber), font, 24);
    levelText.setFillColor(sf::Color::White);
    levelText.setPosition(130.f, 13.f);

    // надпись экрана смерти, центрируется после создания (знаем ширину текста)
    sf::Text gameOverText("Press Space to Restart", font, 32);
    gameOverText.setFillColor(sf::Color::White);

    // надписи экрана смерти. По вертикали — фиксированные места
    // (заголовок / счёт / подсказка); по горизонтали центрируются в момент
    // смерти, когда известны ширины всех текстов
    sf::Text gameOverTitle("Game Over", font, 60);
    gameOverTitle.setFillColor(sf::Color::Red);

    // сколько уровней пройдено к моменту смерти
    sf::Text gameOverStats("", font, 28);
    gameOverStats.setFillColor(sf::Color::White);

    // панель поверх экрана: тёмная полоса высотой 1/12 окна (уже, чем раньше),
    // на ней читаются белые надписи HP и Level
    sf::RectangleShape panelShape(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT / 12.0f));
    panelShape.setPosition(0.f, 0.f);
    panelShape.setFillColor(sf::Color(30, 30, 30));

    // смерть и рестарт
    bool gameOver = false;
    float contactDamageTimer = 0.0f;   // пауза между ударами, пока враг стоит на игроке

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

        if (!freezeTime && moveCooldown <= 0.0f)
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

        // подбор зелья: игрок встал на клетку зелья — здоровье растёт,
        // но не выше MAX_HP_PLAYER; зелье извлекается из вектора
        for (int i = 0; i < potions.size(); i++)
        {
            if (player.xCellPlayer == potions[i].xPotion
                && player.yCellPlayer == potions[i].yPotion)
            {
                int healAmount = POTION_HEAL;
                if (player.hpPlayer + POTION_HEAL > MAX_HP_PLAYER)
                {
                    healAmount = MAX_HP_PLAYER - player.hpPlayer; // лечим только до полного
                }
                player.hpPlayer += healAmount;
                hpText.setString("HP: " + std::to_string(player.hpPlayer));

                // зелёная вспышка в момент выпитого зелья
                SpawnBurst(particles,
                           static_cast<float>(player.xCellPlayer * TILE_SIZE) + TILE_SIZE / 2.0f,
                           static_cast<float>(player.yCellPlayer * TILE_SIZE) + TILE_SIZE / 2.0f,
                           sf::Color(30, 220, 60));

                potions.erase(potions.begin() + i);
                i--; 
            }
        }

        // игрок встал на выход — переход запускается один раз и дальше идёт по таймеру
        if (!freezeTime && player.xCellPlayer == door.xCellDoor && player.yCellPlayer == door.yCellDoor)
        {
            freezeTime = true;
            freezeTimeTimer = 3.0f;
        }

        // жизнь частиц: двигаем всех, затем мёртвых убираем из вектора
        for (int i = 0; i < particles.size(); i++)
        {
            particles[i].UpdateParticle(deltaTime);
            if (particles[i].lifeParticle <= 0.0f)
            {
                particles.erase(particles.begin() + i);
                i--; // после erase не пропустить следующую частицу
            }
        }

        if (freezeTime)
        {
            if (freezeTimeTimer > 0.0f)
            {
                // фаза 1: пауза под будущую музыку перехода, мир заморожен
                freezeTimeTimer -= deltaTime;
                if (freezeTimeTimer <= 0.0f)
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
                    if (gameOver)
                    {
                        fadeStep= GAMEOVER_FADE_STEP_TIME; // смерть темнеет медленно
                    }
                    else
                    {
                        fadeStep= FADE_STEP_TIME;           // переход — в обычном темпе
                    }
                    fadeBlack+= switchDarkness * FADE_STEP;

                    if (fadeBlack>= 255.0f)
                    {
                        // экран полностью чёрный — глаз не видит подмены
                        fadeBlack= 255.0f;
                        if (!gameOver)
                        {
                            // обычный переход: смена мира под чёрным
                            // номер растёт ДО генерации: зелья нового уровня считаются по его номеру
                            levelNumber++;
                            StartNextLevel(dungeon, player, enemies, door, potions, levelNumber);
                            levelText.setString("Level: " + std::to_string(levelNumber));
                            switchDarkness = -1;
                        }
                        // при gameOver чёрный экран просто держится —
                        // пробел проверяется отдельно, каждый кадр (ниже)
                    }
                    else if (fadeBlack<= 0.0f)
                    {
                        fadeBlack= 0.0f;
                        switchDarkness = 0;
                        freezeTime = false; // переход завершён
                    }
                }
            }
        }

        // рестарт на экране смерти: проверяем ПРОБЕЛ каждый кадр
        if (gameOver && fadeBlack >= 255.0f &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            player.hpPlayer = MAX_HP_PLAYER;
            hpText.setString("HP: " + std::to_string(player.hpPlayer));
            // сначала сброс номера, потом генерация: иначе первый уровень получит зелья уровня смерти
            levelNumber = 1;
            StartNextLevel(dungeon, player, enemies, door, potions, levelNumber);
            levelText.setString("Level: 1");
            gameOver = false;
            switchDarkness = -1;                 // осветляемся на новом уровне
            fadeStep = GAMEOVER_FADE_STEP_TIME;  // первая ступень осветления через 0.5 с, дальше обычный темп (gameOver уже false)
        }

        // у каждого врага свой таймер шага: шатун ходит реже ищейки
        if (!freezeTime)
        {
            for (int i = 0; i < enemies.size(); i++)
            {
                enemies[i]->stepTimerEnemy -= deltaTime;
                if (enemies[i]->stepTimerEnemy <= 0.0f)
                {
                    int xCellOld = enemies[i]->xCellEnemy;
                    int yCellOld = enemies[i]->yCellEnemy;
                    enemies[i]->FindPlayer(player, dungeon, door);

                    // клетка занята другим врагом — шаг отменяется: враги не
                    // накладываются друг на друга (и на игроке стоит только один)
                    for (int j = 0; j < enemies.size(); j++)
                    {
                        if (j != i && enemies[i]->xCellEnemy == enemies[j]->xCellEnemy
                            && enemies[i]->yCellEnemy == enemies[j]->yCellEnemy)
                        {
                            enemies[i]->SetPositionEnemy(xCellOld, yCellOld);
                        }
                    }
                    enemies[i]->stepTimerEnemy = enemies[i]->stepTimeEnemy;
                }
            }
        }

        // проверяем, догнал ли игрока враг; на клетке игрока стоит максимум один
        // (шаг на занятую клетку отменяется), бьёт он своим уроном
        bool enemyCatchesPlayer = false;
        int damageCatch = 0;
        for (const std::unique_ptr<Enemy> &enemy : enemies)
        {
            if (enemy->xCellEnemy == player.xCellPlayer
                && enemy->yCellEnemy == player.yCellPlayer)
            {
                enemyCatchesPlayer = true;
                if (enemy->damageEnemy > damageCatch)
                {
                    damageCatch = enemy->damageEnemy;
                }
            }
        }

        // урон при поимке: враг стоит на клетке игрока — здоровье тратится
        // через CONTACT_DAMAGE_INTERVAL секунд, пока контакт не прервётся
        contactDamageTimer -= deltaTime; // тик таймера урона (каждый кадр)

        if (enemyCatchesPlayer && !gameOver && contactDamageTimer <= 0.0f)
        {
            player.hpPlayer -= damageCatch;
            // после зелья HP не кратно урону и может уйти в минус: на панели показываем 0
            if (player.hpPlayer < 0)
            {
                player.hpPlayer = 0;
            }
            hpText.setString("HP: " + std::to_string(player.hpPlayer));
            contactDamageTimer = CONTACT_DAMAGE_INTERVAL; // пауза до следующего удара

            // красные искры из центра клетки удара
            SpawnBurst(particles,
                       static_cast<float>(player.xCellPlayer * TILE_SIZE) + TILE_SIZE / 2.0f,
                       static_cast<float>(player.yCellPlayer * TILE_SIZE) + TILE_SIZE / 2.0f,
                       sf::Color(220, 30, 30));

            if (player.hpPlayer <= 0)
            {
                // смерть: замороженное медленное затемнение
                gameOver = true;
                freezeTime = true;
                freezeTimeTimer = 0.0f;              // без 3-секундной паузы
                switchDarkness = 1;                   // сразу темнеем
                fadeStep = GAMEOVER_FADE_STEP_TIME;   // но медленно: шаг за 0.5 с

                // счёт пройденных уровней: умерли на levelNumber —
                // значит, завершили levelNumber − 1
                gameOverStats.setString("Levels passed: " + std::to_string(levelNumber - 1));

                // центрируем все три надписи по горизонтали (по ширине каждой)
                gameOverTitle.setPosition((WINDOW_WIDTH - gameOverTitle.getGlobalBounds().width) / 2.0f, 200.f);
                gameOverStats.setPosition((WINDOW_WIDTH - gameOverStats.getGlobalBounds().width) / 2.0f, 280.f);
                gameOverText.setPosition((WINDOW_WIDTH - gameOverText.getGlobalBounds().width) / 2.0f, 330.f);
            }
        }

        // позиция камеры: центр клетки игрока
        sf::Vector2f cameraPosition(player.xCellPlayer* TILE_SIZE + (TILE_SIZE / 2.f),
            player.yCellPlayer* TILE_SIZE + (TILE_SIZE / 2.f));

        const float edgeScreenX = camera.getSize().x / 2.0f;
        const float edgeScreenY = camera.getSize().y / 2.0f;
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
        dungeon.Draw(window, textures);

        // зелья: лежат на полу, под юнитами (пол → зелья → дверь → игрок → враг)
        for (const Potion &potion : potions)
        {
            potion.DrawPotion(window);
        }

        door.DrawDoor(window, textures); // выход рисуем под юнитами: пол → дверь → игрок → враг
        player.DrawPlayer(window);
        for (const std::unique_ptr<Enemy> &enemy : enemies)
        {
            enemy->DrawEnemy(window);
        }

        // частицы поверх юнитов — они часть мира, но «верх» по слоям
        for (const Particle &particle : particles)
        {
            particle.DrawParticle(window);
        }

        // затемнение рисуется последним и в экранных координатах:
        // переключаемся на камеру по умолчанию, чтобы занавес накрыл ровно весь экран
        window.setView(window.getDefaultView());

        // панель всегда: рисуем поверх мира, под занавесом
        // (текстам нужен шрифт, панели — нет; поэтому разные условия)
        window.draw(panelShape);

        if (fontLoaded)
        {
            window.draw(hpText);
            window.draw(levelText);
        }

        fadeShape.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(fadeBlack)));
        window.draw(fadeShape);

        if (fontLoaded && gameOver && fadeBlack >= 255.0f)
                                     // экран смерти: только когда затемнение ДОШЛО
                                     // до полного чёрного — сначала тьма, потом надписи
        {
            window.draw(gameOverTitle);
            window.draw(gameOverStats);
            window.draw(gameOverText);
        }

        window.display();
    }
}