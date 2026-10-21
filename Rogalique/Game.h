#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct Cell
{
    int x = 0;
    int y = 0;
};
enum class Direction
{
    Up,
    Down,
    Left,
    Right,
    Start
};

class Game
{
  private:
    Direction direction = Direction::Start;

  public:
    Game();

    sf::RenderWindow window;
    sf::Event event;
    sf::Clock clock;
    sf::Clock moveTimer;
    float moveInterval = 0.2f;
    std::vector<Cell> snake;
    Cell apple;
    bool isGameOver = false;
    sf::Font font;
    sf::Text gameOverText;
    sf::Text restartText;
    sf::Text pauseText;
    sf::Text pauseHintText;
    bool isPauseGame = false;

    void Reset();
    void Run();
    void LoadResources();
    void ProcessInput();
    void Update(float deltaTime);
    void DrawGameOver();
    void DrawGame();
};