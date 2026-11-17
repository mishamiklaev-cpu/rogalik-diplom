#include "Game.h"
#include "GameLevel.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

// подготовка до игрового цикла — бывшее начало main. Поля уже созданы пустыми,
// здесь им задаются размеры, тексты и места; порядок тот же, что был в main
Game::Game()
{
    int seed = (int)time(nullptr); // текущее время: каждый запуск — новое зерно, значит новая карта
    srand(seed);

    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Dungeon Escape");
    // удержание клавиши не присылает повторов: одно нажатие — одно событие KeyPressed
    window.setKeyRepeatEnabled(false);

    dungeon.GenerateLevel(MAP_CELLS_WIDTH, MAP_CELLS_HEIGHT);

    // вид камеры = нижние 11/12 окна: верхняя 1/12 отдана панели HUD, поэтому игровой мир никогда не рисуется под панелью
    camera.setSize(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
                                static_cast<float>(WINDOW_HEIGHT) * 11.0f / 12.0f));
    camera.setViewport(sf::FloatRect(0.0f, 1.0f / 12.0f, 1.0f, 11.0f / 12.0f));
    camera.setCenter(
        sf::Vector2f(static_cast<float>((MAP_CELLS_WIDTH * TILE_SIZE) / 2.f),
                     static_cast<float>((MAP_CELLS_HEIGHT * TILE_SIZE) / 2.f))); // центрируем

    player.SpawnPlayerInRandomRoom(dungeon.rooms);

    door.SpawnDoor(dungeon.rooms, player.xCellPlayer, player.yCellPlayer);

    SpawnEnemies(enemies, dungeon, player, door, levelNumber);

    SpawnPotions(potions, dungeon.rooms, player, door, levelNumber);

    fadeShape.setSize(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
                                   static_cast<float>(WINDOW_HEIGHT)));
    fadeShape.setFillColor(sf::Color(0, 0, 0, 0));

    textures.LoadTextures();

    // игра открывается в меню — сразу его музыка
    sounds.LoadSounds();
    sounds.menuMusic.play();

    fontLoaded = font.loadFromFile("Fonts/Roboto-Black.ttf");
    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile("Resources/Fonts/Roboto-Black.ttf");
    }

    hpText.setString("HP: " + std::to_string(player.hpPlayer));
    hpText.setFont(font);
    hpText.setCharacterSize(24);
    hpText.setFillColor(sf::Color::White);
    hpText.setPosition(10.f, 13.f);

    levelText.setString("Level: " + std::to_string(levelNumber));
    levelText.setFont(font);
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::White);
    levelText.setPosition(130.f, 13.f);

    hintText.setString("Move: Arrows   Pause: Esc");
    hintText.setFont(font);
    hintText.setCharacterSize(18);
    hintText.setFillColor(sf::Color::White);
    hintText.setPosition(WINDOW_WIDTH - hintText.getGlobalBounds().width - 12.f, 15.f);

    gameOverTitle.setString("Game Over");
    gameOverTitle.setFont(font);
    gameOverTitle.setCharacterSize(60);
    gameOverTitle.setFillColor(sf::Color::Red);

    gameOverStats.setFont(font);
    gameOverStats.setCharacterSize(28);
    gameOverStats.setFillColor(sf::Color::White);

    panelShape.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT / 12.0f));
    panelShape.setPosition(0.f, 0.f);
    panelShape.setFillColor(sf::Color(30, 30, 30));

    menuPicture.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    if (textures.menuLoaded)
    {
        menuPicture.setTexture(&textures.menuTexture);
    }

    menuPanel.setSize(sf::Vector2f(200.f, 175.f));
    menuPanel.setPosition((WINDOW_WIDTH - 200.f) / 2.0f, MENU_ITEMS_Y - 5.f);
    menuPanel.setFillColor(sf::Color(0, 0, 0, 150));

    menuTitle.setString("Dungeon Escape");
    menuTitle.setFont(font);
    menuTitle.setCharacterSize(40);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setPosition((WINDOW_WIDTH - menuTitle.getGlobalBounds().width) / 2.0f, MENU_ITEMS_Y - 60.f);

    BuildMenuItems(menuItems, menuNames, MENU_ITEMS_COUNT, font, MENU_ITEMS_Y);

    gameRecords.LoadRecords();

    recordsTitle.setString("Records");
    recordsTitle.setFont(font);
    recordsTitle.setCharacterSize(44);
    recordsTitle.setFillColor(sf::Color::White);
    recordsTitle.setPosition((WINDOW_WIDTH - recordsTitle.getGlobalBounds().width) / 2.0f, 30.f);

    recordLine.setFont(font);
    recordLine.setCharacterSize(26);
    recordLine.setFillColor(sf::Color::White);

    backHint.setString("Esc - Back");
    backHint.setFont(font);
    backHint.setCharacterSize(22);
    backHint.setFillColor(sf::Color(180, 180, 180));
    backHint.setPosition((WINDOW_WIDTH - backHint.getGlobalBounds().width) / 2.0f, 555.f);

    optionsTitle.setString("Options");
    optionsTitle.setFont(font);
    optionsTitle.setCharacterSize(44);
    optionsTitle.setFillColor(sf::Color::White);
    optionsTitle.setPosition((WINDOW_WIDTH - optionsTitle.getGlobalBounds().width) / 2.0f, 150.f);

    BuildMenuItems(optionsItems, optionsNames, OPTIONS_ITEMS_COUNT, font, 250.f);

    namePrompt.setString("Enter your name:");
    namePrompt.setFont(font);
    namePrompt.setCharacterSize(24);
    namePrompt.setFillColor(sf::Color::White);
    namePrompt.setPosition((WINDOW_WIDTH - namePrompt.getGlobalBounds().width) / 2.0f, 330.f);

    nameText.setFont(font);
    nameText.setCharacterSize(30);
    nameText.setFillColor(sf::Color(240, 180, 40));

    nameSkipHint.setString("Enter - Save     Esc - Skip");
    nameSkipHint.setFont(font);
    nameSkipHint.setCharacterSize(22);
    nameSkipHint.setFillColor(sf::Color(180, 180, 180));
    nameSkipHint.setPosition((WINDOW_WIDTH - nameSkipHint.getGlobalBounds().width) / 2.0f, 430.f);

    pauseShape.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    pauseShape.setFillColor(sf::Color(0, 0, 0, 150));

    pauseTitle.setString("Pause");
    pauseTitle.setFont(font);
    pauseTitle.setCharacterSize(60);
    pauseTitle.setFillColor(sf::Color::White);
    pauseTitle.setPosition((WINDOW_WIDTH - pauseTitle.getGlobalBounds().width) / 2.0f, 170.f);

    BuildMenuItems(pauseItems, pauseNames, PAUSE_ITEMS_COUNT, font, 260.f);

    confirmTitle.setString("Are you sure?");
    confirmTitle.setFont(font);
    confirmTitle.setCharacterSize(44);
    confirmTitle.setFillColor(sf::Color::White);
    confirmTitle.setPosition((WINDOW_WIDTH - confirmTitle.getGlobalBounds().width) / 2.0f, 180.f);

    BuildMenuItems(confirmItems, confirmNames, CONFIRM_ITEMS_COUNT, font, 260.f);

    menuConfirmTitle.setString("Are you sure?");
    menuConfirmTitle.setFont(font);
    menuConfirmTitle.setCharacterSize(24);
    menuConfirmTitle.setFillColor(sf::Color::White);
    menuConfirmTitle.setPosition((WINDOW_WIDTH - menuConfirmTitle.getGlobalBounds().width) / 2.0f, MENU_ITEMS_Y + 5.f);
    BuildMenuItems(menuConfirmItems, confirmNames, CONFIRM_ITEMS_COUNT, font, MENU_ITEMS_Y + 50.f);

    BuildMenuItems(gameOverItems, gameOverNames, GAMEOVER_ITEMS_COUNT, font, 330.f);
}

// игровой цикл — бывший while в main
void Game::Run()
{
    while (window.isOpen())
    {
        HandleEvents();

        // часы перезапускаем в любом состоянии: иначе после паузы deltaTime
        // окажется равен всей её длине и таймеры сработают разом
        float deltaTime = clock.restart().asSeconds();

        Update(deltaTime);
        Draw();
    }
}

// шаг 1 кадра: события клавиатуры, затем сброс мира, если выбрали новую игру или меню
void Game::HandleEvents()
{
    // выбор пункта только ставит флаг, сам сброс мира один — после цикла событий
    bool startNewGame = false;
    bool goToMenu = false;

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        // TextEntered приходит с уже готовым символом с учётом раскладки и регистра.
        // Только латиница и цифры: имя пишется в файл через пробел, в нём не должно быть пробелов
        if (event.type == sf::Event::TextEntered && gameOver && nameEntryOpen
            && fadeBlack >= 255.0f && playerName.size() < MAX_NAME_LENGTH)
        {
            sf::Uint32 symbol = event.text.unicode;
            if ((symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z')
                || (symbol >= '0' && symbol <= '9'))
            {
                playerName += static_cast<char>(symbol);
            }
        }

        // клавиши меню и паузы — события, а не isKeyPressed: одно нажатие — одно действие.
        // Цепочка else if: нажатие обрабатывает только один экран
        if (event.type == sf::Event::KeyPressed)
        {
            sf::Keyboard::Key key = event.key.code;

            if (gameState == GameState::Menu && menuConfirmOpen)
            {
                MoveMenuSelection(selectedConfirmItem, CONFIRM_ITEMS_COUNT, key, sounds);
                if (key == sf::Keyboard::Escape)
                {
                    menuConfirmOpen = false;
                }
                else if (key == sf::Keyboard::Enter)
                {
                    menuConfirmOpen = false;
                    if (selectedConfirmItem == 0) // Yes
                    {
                        window.close();
                    }
                }
            }
            else if (gameState == GameState::Menu)
            {
                MoveMenuSelection(selectedMenuItem, MENU_ITEMS_COUNT, key, sounds);
                if (key == sf::Keyboard::Enter)
                {
                    if (selectedMenuItem == 0)
                    {
                        gameState = GameState::Playing;
                        sounds.menuMusic.stop();
                        sounds.gameMusic.play();
                    }
                    else if (selectedMenuItem == 1)
                    {
                        gameState = GameState::Records;
                    }
                    else if (selectedMenuItem == 2)
                    {
                        gameState = GameState::Options;
                        selectedOptionsItem = 0;
                    }
                    else
                    {
                        menuConfirmOpen = true;
                        selectedConfirmItem = 1; // по умолчанию No, как и на паузе
                    }
                }
            }
            else if (gameState == GameState::Records)
            {
                if (key == sf::Keyboard::Escape || key == sf::Keyboard::Enter)
                {
                    gameState = GameState::Menu;
                }
            }
            else if (gameState == GameState::Options)
            {
                MoveMenuSelection(selectedOptionsItem, OPTIONS_ITEMS_COUNT, key, sounds);
                if (key == sf::Keyboard::Escape)
                {
                    gameState = GameState::Menu;
                }
                else if (key == sf::Keyboard::Enter)
                {
                    if (selectedOptionsItem == 0)
                    {
                        sounds.SetMusicOn(!sounds.musicOn);
                        if (sounds.musicOn)
                        {
                            optionsNames[0] = "Music: On";
                        }
                        else
                        {
                            optionsNames[0] = "Music: Off";
                        }
                    }
                    else if (selectedOptionsItem == 1)
                    {
                        sounds.SetSoundsOn(!sounds.soundsOn);
                        if (sounds.soundsOn)
                        {
                            optionsNames[1] = "Sounds: On";
                        }
                        else
                        {
                            optionsNames[1] = "Sounds: Off";
                        }
                    }
                    else
                    {
                        gameState = GameState::Menu;
                    }
                    // ширина текста изменилась: пункты центрируем заново
                    BuildMenuItems(optionsItems, optionsNames, OPTIONS_ITEMS_COUNT, font, 250.f);
                }
            }
            else if (gameState == GameState::Paused && pauseConfirmOpen)
            {
                MoveMenuSelection(selectedConfirmItem, CONFIRM_ITEMS_COUNT, key, sounds);
                if (key == sf::Keyboard::Escape)
                {
                    pauseConfirmOpen = false; // передумал: обратно в меню паузы
                }
                else if (key == sf::Keyboard::Enter)
                {
                    pauseConfirmOpen = false;
                    if (selectedConfirmItem == 0) // Yes
                    {
                        if (selectedPauseItem == 1)
                        {
                            startNewGame = true;
                        }
                        else
                        {
                            goToMenu = true;
                        }
                    }
                }
            }
            else if (gameState == GameState::Paused)
            {
                MoveMenuSelection(selectedPauseItem, PAUSE_ITEMS_COUNT, key, sounds);
                if (key == sf::Keyboard::Escape)
                {
                    gameState = GameState::Playing;
                    sounds.gameMusic.play(); // после pause() продолжает с того же места
                }
                else if (key == sf::Keyboard::Enter)
                {
                    if (selectedPauseItem == 0)
                    {
                        gameState = GameState::Playing;
                        sounds.gameMusic.play();
                    }
                    else
                    {
                        pauseConfirmOpen = true;
                        selectedConfirmItem = 1; // по умолчанию No: случайный Enter не сотрёт забег
                    }
                }
            }
            else if (gameOver)
            {
                // клавиши работают, только когда экран смерти уже показан.
                // Сначала ввод имени, после него — мини-меню
                if (fadeBlack >= 255.0f && nameEntryOpen)
                {
                    if (key == sf::Keyboard::BackSpace && !playerName.empty())
                    {
                        playerName.pop_back();
                    }
                    else if (key == sf::Keyboard::Enter)
                    {
                        if (playerName.empty())
                        {
                            playerName = "Player";
                        }
                        gameRecords.AddRecord(playerName, levelNumber - 1);
                        nameEntryOpen = false;
                        sounds.selectSound.play();
                    }
                    else if (key == sf::Keyboard::Escape)
                    {
                        nameEntryOpen = false; // пропуск: рекорд не записывается
                        sounds.selectSound.play();
                    }
                }
                else if (fadeBlack >= 255.0f)
                {
                    MoveMenuSelection(selectedGameOverItem, GAMEOVER_ITEMS_COUNT, key, sounds);
                    if (key == sf::Keyboard::Enter)
                    {
                        if (selectedGameOverItem == 0)
                        {
                            startNewGame = true;
                        }
                        else
                        {
                            goToMenu = true;
                        }
                    }
                }
            }
            else if (key == sf::Keyboard::Escape)
            {
                gameState = GameState::Paused;
                selectedPauseItem = 0;
                sounds.gameMusic.pause();
            }
        }
    }

    // новая игра или выход в меню: мир собирается заново и все таймеры сбрасываются
    if (startNewGame || goToMenu)
    {
        player.hpPlayer = MAX_HP_PLAYER;
        player.directionPlayer = Direction::Down;
        player.framePlayer = 0;
        hpText.setString("HP: " + std::to_string(player.hpPlayer));
        // сначала сброс номера, потом генерация: иначе первый уровень получит зелья прошлого забега
        levelNumber = 1;
        StartNextLevel(dungeon, player, enemies, door, potions, levelNumber);
        levelText.setString("Level: 1");
        particles.clear();
        gameOver = false;
        freezeTime = false;
        freezeTimeTimer = 0.0f;
        fadeBlack = 0.0f;
        switchDarkness = 0;
        fadeStep = 0.0f;
        moveCooldown = 0.0f;
        contactDamageTimer = 0.0f;
        selectedGameOverItem = 0;
        sounds.gameMusic.stop(); // stop, а не pause: следующий play() начнёт трек сначала

        if (startNewGame)
        {
            gameState = GameState::Playing;
            sounds.gameMusic.play();
        }
        else
        {
            gameState = GameState::Menu;
            sounds.menuMusic.play();
            selectedMenuItem = 0;
        }
    }
}

// шаг 3 кадра: игровая логика
void Game::Update(float deltaTime)
{
    // сбрасываем каждый кадр вне if: тряска камеры в Draw читает его в любом состоянии
    enemyCatchesPlayer = false;

    // игровая логика идёт только в состоянии Playing
    if (gameState == GameState::Playing)
    {
        moveCooldown -= deltaTime;

        if (!freezeTime && moveCooldown <= 0.0f)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                player.MovePlayer(0, -1, dungeon);
                moveCooldown = 0.1f;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                player.MovePlayer(0, 1, dungeon);
                moveCooldown = 0.1f;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                player.MovePlayer(-1, 0, dungeon);
                moveCooldown = 0.1f;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
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
                sounds.potionSound.play();
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
            freezeTimeTimer = 1.0f;
            sounds.doorSound.play(); // звучит в паузе перед затемнением
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
                        // на нём работает мини-меню смерти
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
                    enemies[i]->FindPlayer(player, dungeon, door, enemies);

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
                    // только теперь известно, состоялся ли шаг: сначала проверка, потом анимация
                    enemies[i]->AnimateStepEnemy(xCellOld, yCellOld);
                    enemies[i]->stepTimerEnemy = enemies[i]->stepTimeEnemy;
                }
            }
        }

        // проверяем, догнал ли игрока враг; на клетке игрока стоит максимум один
        // (шаг на занятую клетку отменяется), бьёт он своим уроном
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
                nameEntryOpen = true;
                playerName.clear();
                sounds.gameMusic.stop();
                sounds.gameOverSound.play();
                freezeTime = true;
                freezeTimeTimer = 0.0f;              // без паузы перед затемнением
                switchDarkness = 1;                   // сразу темнеем
                fadeStep = GAMEOVER_FADE_STEP_TIME;   // но медленно: шаг за 0.5 с

                // счёт пройденных уровней: умерли на levelNumber —
                // значит, завершили levelNumber − 1
                gameOverStats.setString("Levels passed: " + std::to_string(levelNumber - 1));

                // центрируем надписи по горизонтали (по ширине каждой)
                gameOverTitle.setPosition((WINDOW_WIDTH - gameOverTitle.getGlobalBounds().width) / 2.0f, 180.f);
                gameOverStats.setPosition((WINDOW_WIDTH - gameOverStats.getGlobalBounds().width) / 2.0f, 265.f);
            }
            else
            {
                sounds.damageSound.play(); // смертельный удар озвучивает gameOverSound
            }
        }
    }
}

// шаг 4 кадра: рисование. Экраны меню рисуются целиком и сразу выходят (return)
void Game::Draw()
{
    // в меню мир не рисуем: кадр собран, остальное рисование пропускаем
    if (gameState == GameState::Menu)
    {
        window.clear();
        window.setView(window.getDefaultView());
        if (textures.menuLoaded)
        {
            window.draw(menuPicture);
        }
        else if (fontLoaded)
        {
            window.draw(menuTitle);
        }
        window.draw(menuPanel);
        if (fontLoaded && menuConfirmOpen)
        {
            window.draw(menuConfirmTitle);
            DrawMenuItems(window, menuConfirmItems, CONFIRM_ITEMS_COUNT, selectedConfirmItem);
        }
        else if (fontLoaded)
        {
            DrawMenuItems(window, menuItems, MENU_ITEMS_COUNT, selectedMenuItem);
        }
        window.display();
        return;
    }

    // экраны рекордов и опций: чёрный фон, мир тоже не рисуем
    if (gameState == GameState::Records)
    {
        window.clear();
        window.setView(window.getDefaultView());
        if (fontLoaded)
        {
            window.draw(recordsTitle);
            if (gameRecords.records.empty())
            {
                recordLine.setString("No records yet");
                recordLine.setPosition((WINDOW_WIDTH - recordLine.getGlobalBounds().width) / 2.0f, 250.f);
                window.draw(recordLine);
            }
            for (int i = 0; i < gameRecords.records.size(); i++)
            {
                recordLine.setString(gameRecords.records[i].nameRecord + " - "
                                     + std::to_string(gameRecords.records[i].levelsRecord));
                recordLine.setPosition((WINDOW_WIDTH - recordLine.getGlobalBounds().width) / 2.0f, 110.f + i * 42.f);
                window.draw(recordLine);
            }
            window.draw(backHint);
        }
        window.display();
        return;
    }

    if (gameState == GameState::Options)
    {
        window.clear();
        window.setView(window.getDefaultView());
        if (fontLoaded)
        {
            window.draw(optionsTitle);
            DrawMenuItems(window, optionsItems, OPTIONS_ITEMS_COUNT, selectedOptionsItem);
        }
        window.display();
        return;
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
        potion.DrawPotion(window, textures);
    }

    door.DrawDoor(window, textures); // выход рисуем под юнитами: пол → дверь → игрок → враг
    player.DrawPlayer(window, textures);
    for (const std::unique_ptr<Enemy> &enemy : enemies)
    {
        enemy->DrawEnemy(window, textures);
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
        window.draw(hintText);
    }

    fadeShape.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(fadeBlack)));
    window.draw(fadeShape);

    if (fontLoaded && gameOver && fadeBlack >= 255.0f)
                                 // экран смерти: только когда затемнение ДОШЛО
                                 // до полного чёрного — сначала тьма, потом надписи
    {
        window.draw(gameOverTitle);
        window.draw(gameOverStats);
        if (nameEntryOpen)
        {
            window.draw(namePrompt);
            nameText.setString(playerName + "_"); // чёрточка показывает, куда идёт ввод
            nameText.setPosition((WINDOW_WIDTH - nameText.getGlobalBounds().width) / 2.0f, 375.f);
            window.draw(nameText);
            window.draw(nameSkipHint);
        }
        else
        {
            DrawMenuItems(window, gameOverItems, GAMEOVER_ITEMS_COUNT, selectedGameOverItem);
        }
    }

    if (gameState == GameState::Paused)
    {
        window.draw(pauseShape);
        if (fontLoaded && pauseConfirmOpen)
        {
            window.draw(confirmTitle);
            DrawMenuItems(window, confirmItems, CONFIRM_ITEMS_COUNT, selectedConfirmItem);
        }
        else if (fontLoaded)
        {
            window.draw(pauseTitle);
            DrawMenuItems(window, pauseItems, PAUSE_ITEMS_COUNT, selectedPauseItem);
        }
    }

    window.display();
}
