#pragma once
#include "Constants.h"
#include "Door.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "GameMenu.h"
#include "GameRecords.h"
#include "GameSounds.h"
#include "GameTextures.h"
#include "Particle.h"
#include "Player.h"
#include "Potion.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

// вся игра целиком. Бывшие локальные переменные main стали полями класса
// (названия те же), а игровой цикл разделён на шаги кадра: события, логика, рисование
class Game
{
  public:
    Game();     // подготовка до цикла: всё создаётся один раз
    void Run(); // игровой цикл: один круг — один кадр

  private:
    void HandleEvents();          // шаг 1: клавиши меню, паузы, ввода имени и сброс мира
    void Update(float deltaTime); // шаг 3: логика кадра — ходы, зелья, дверь, враги, урон
    void Draw();                  // шаг 4: рисование меню, рекордов, опций или игры

    sf::Event event;

    // генератор случайных чисел
    sf::Clock clock;

    // генератор окна
    sf::RenderWindow window;

    // объект Dungeon: уровень собирается одним методом класса
    Dungeon dungeon;

    sf::View camera;

    Player player;

    // выход с уровня: ставится после спавна игрока, чтобы знать, от кого мерить дистанцию
    Door door;

    // счётчик уровня объявлен заранее: его читают враги и зелья при раскладке
    int levelNumber = 1;               // номер текущего уровня (расписание спавна)

    // все враги уровня (после двери — чтобы не встать на неё);
    // указатели — потому что в векторе разные наследники Enemy
    std::vector<std::unique_ptr<Enemy>> enemies;

    // вектор зелий уровня: количество зависит от номера уровня (см. SpawnPotions)
    std::vector<Potion> potions;

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
    sf::RectangleShape fadeShape;

    // текстуры: загружаем один раз до игрового цикла, рисование только использует готовые
    GameTextures textures;

    // звуки: так же один раз до игрового цикла
    GameSounds sounds;

    // шрифт для игровой надписи и экрана смерти.
    sf::Font font;
    bool fontLoaded = false;

    // индикатор здоровья: значение читается прямо из игрока
    sf::Text hpText;

    // надпись счётчика уровня
    sf::Text levelText;

    // подсказка справа на панели: прижимаем к краю окна по ширине текста
    sf::Text hintText;

    // надписи экрана смерти. По вертикали — фиксированные места
    // (заголовок / счёт); по горизонтали центрируются в момент
    // смерти, когда известны ширины всех текстов
    sf::Text gameOverTitle;

    // сколько уровней пройдено к моменту смерти
    sf::Text gameOverStats;

    // панель поверх экрана: тёмная полоса высотой 1/12 окна (уже, чем раньше),
    // на ней читаются белые надписи HP и Level
    sf::RectangleShape panelShape;

    // что сейчас происходит в игре
    GameState gameState = GameState::Menu;

    // главное меню: картинка с заголовком на всё окно, пункты лежат на полу, на тёмной плашке
    sf::RectangleShape menuPicture;

    const float MENU_ITEMS_Y = 425.f; // верх первого пункта: начало пола на картинке

    sf::RectangleShape menuPanel;

    // заголовок нужен, только если картинки нет: на самой картинке он уже есть
    sf::Text menuTitle;

    // static const: число известно при сборке, поэтому им можно задать размер массива-поля
    static const int MENU_ITEMS_COUNT = 4;
    const std::string menuNames[MENU_ITEMS_COUNT] = { "Start", "Records", "Options", "Exit" };
    sf::Text menuItems[MENU_ITEMS_COUNT];
    int selectedMenuItem = 0; // номер выбранного пункта: просто число, не адрес в памяти

    // экран рекордов: строки собираются при рисовании из таблицы
    GameRecords gameRecords;

    sf::Text recordsTitle;

    sf::Text recordLine;

    sf::Text backHint;

    // экран опций: подписи On и Off меняются при переключении
    sf::Text optionsTitle;

    static const int OPTIONS_ITEMS_COUNT = 3;
    std::string optionsNames[OPTIONS_ITEMS_COUNT] = { "Music: On", "Sounds: On", "Back" };
    sf::Text optionsItems[OPTIONS_ITEMS_COUNT];
    int selectedOptionsItem = 0;

    // ввод имени на экране смерти: после Enter вместо него появляется мини-меню
    sf::Text namePrompt;

    sf::Text nameText;

    sf::Text nameSkipHint;

    static const int MAX_NAME_LENGTH = 12;
    std::string playerName;
    bool nameEntryOpen = false;

    // экран паузы: полупрозрачная пелена, поверх неё надписи по центру окна
    sf::RectangleShape pauseShape;

    sf::Text pauseTitle;

    static const int PAUSE_ITEMS_COUNT = 3;
    const std::string pauseNames[PAUSE_ITEMS_COUNT] = { "Continue", "New Game", "Main Menu" };
    sf::Text pauseItems[PAUSE_ITEMS_COUNT];
    int selectedPauseItem = 0;

    // подтверждение на паузе: текущий забег будет потерян
    sf::Text confirmTitle;

    static const int CONFIRM_ITEMS_COUNT = 2;
    const std::string confirmNames[CONFIRM_ITEMS_COUNT] = { "Yes", "No" };
    sf::Text confirmItems[CONFIRM_ITEMS_COUNT];
    int selectedConfirmItem = 1;
    bool pauseConfirmOpen = false;

    // подтверждение выхода в главном меню: на той же плашке, вместо пунктов
    sf::Text menuConfirmTitle;
    sf::Text menuConfirmItems[CONFIRM_ITEMS_COUNT];
    bool menuConfirmOpen = false;

    // мини-меню экрана смерти: забег уже окончен, поэтому без подтверждения
    static const int GAMEOVER_ITEMS_COUNT = 2;
    const std::string gameOverNames[GAMEOVER_ITEMS_COUNT] = { "New Game", "Main Menu" };
    sf::Text gameOverItems[GAMEOVER_ITEMS_COUNT];
    int selectedGameOverItem = 0;

    // смерть и рестарт
    bool gameOver = false;
    float contactDamageTimer = 0.0f;   // пауза между ударами, пока враг стоит на игроке

    // враг на клетке игрока в этом кадре: пишет логика (Update), читает тряска камеры (Draw)
    bool enemyCatchesPlayer = false;
};
