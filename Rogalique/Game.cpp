#include "Game.h"
#include "Constants.h"
#include "Grid.h"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <vector>

Game::Game()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake"),
      grid(GRID_WIDTH_IN_CELLS, GRID_HEIGHT_IN_CELLS, CELL_SIZE)
{
    Reset();
    LoadResources();
}

void Game::Reset()
{
    snake = {{3, 5}, {2, 5}, {1, 5}};
    apple = {10, 7};
    isGameOver = false;
    moveTimer.restart();
    direction = Direction::Start;
}

void Game::Run()
{

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        ProcessInput();
        Update(deltaTime);
        if (isGameOver)
        {
            DrawGameOver();
        }
        else
        {
            DrawGame();
        }
    }
}

void Game::LoadResources()
{

    font.loadFromFile("Resources/Fonts/Roboto-Black.ttf");
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER");

    sf::FloatRect boundsGO;
    boundsGO = gameOverText.getLocalBounds();
    float centerXGO = (WINDOW_WIDTH / 2) - (boundsGO.width / 2);
    float centerYGO = (WINDOW_HEIGHT / 2) - (boundsGO.height / 2);
    gameOverText.setPosition(centerXGO, centerYGO);

    restartText.setFont(font);
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::Green);
    restartText.setString("Press R to Restart");

    sf::FloatRect boundsR;
    boundsR = restartText.getLocalBounds();
    float centerXR = (WINDOW_WIDTH / 2) - (boundsR.width / 2);
    float centerYR = ((WINDOW_HEIGHT / 2) + (boundsGO.height / 2) + 15);
    restartText.setPosition(centerXR, centerYR);

    pauseText.setFont(font);
    pauseText.setCharacterSize(50);
    pauseText.setFillColor(sf::Color::Green);
    pauseText.setString("PAUSE");

    sf::FloatRect boundsP;
    boundsP = pauseText.getLocalBounds();
    float centerXP = (WINDOW_WIDTH / 2) - (boundsP.width / 2);
    float centerYP = (WINDOW_HEIGHT / 2) - (boundsP.height / 2);
    pauseText.setPosition(centerXP, centerYP);

    pauseHintText.setFont(font);
    pauseHintText.setCharacterSize(20);
    pauseHintText.setFillColor(sf::Color::Green);
    pauseHintText.setString("Press P for PAUSE");

    sf::FloatRect boundsPH;
    boundsPH = pauseHintText.getLocalBounds();
    float centerXPH = WINDOW_WIDTH - boundsPH.width - 10;
    float centerYPH = 10;
    pauseHintText.setPosition(centerXPH, centerYPH);
}

void Game::ProcessInput()
{

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        if ((!isGameOver) && (event.type == sf::Event::KeyPressed) &&
            (event.key.code == sf::Keyboard::P))
        {
            isPauseGame = !isPauseGame;
            if (!isPauseGame)
            {
                moveTimer.restart();
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        if (direction != Direction::Down)
        {
            direction = Direction::Up;
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        if (direction != Direction::Up)
        {
            direction = Direction::Down;
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        if (direction != Direction::Right)
        {
            direction = Direction::Left;
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        if (direction != Direction::Left)
        {
            direction = Direction::Right;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && (isGameOver))
    {
        Reset();
    }
}

void Game::Update(float deltaTime)
{

    if (direction == Direction::Start)
    {
        return;
    }
    if (isGameOver)
    {

        return;
    }
    if (isPauseGame)
    {
        return;
    }

    if (moveTimer.getElapsedTime().asSeconds() >=
        moveInterval) // посмотри на секундомер переведи в секунды
    {
        moveTimer.restart();
        Cell newHead = snake[0];
        switch (direction)
        {
        case Direction::Up:
            newHead.y -= 1;
            break;
        case Direction::Down:
            newHead.y += 1;
            break;
        case Direction::Left:
            newHead.x -= 1;
            break;
        case Direction::Right:
            newHead.x += 1;
            break;
        }

        if (newHead.x < 0 || newHead.y < 0 || newHead.x >= grid.GetWidthInCells() ||
            newHead.y >= grid.GetHeightInCells())
        {
            isGameOver = true;
            return;
        }

        for (const auto &cell : snake)
        {
            if (newHead.x == cell.x && newHead.y == cell.y)
            {
                isGameOver = true;
                return;
            }
        }

        snake.insert(snake.begin(), newHead);
        if (newHead.x == apple.x && newHead.y == apple.y)
        {
            apple.x = rand() % grid.GetWidthInCells();
            apple.y = rand() % grid.GetHeightInCells();
        }
        else
        {
            snake.pop_back();
        }
    }
}

void Game::DrawGame()
{
    window.clear(sf::Color::Black);

    sf::RectangleShape cellShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    cellShape.setFillColor(sf::Color::Green);
    sf::RectangleShape appleShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    appleShape.setFillColor(sf::Color::Red);

    appleShape.setPosition(grid.CellToPixel(apple.x), grid.CellToPixel(apple.y));

    if (isPauseGame)
    {
        window.draw(pauseText);
    }
    else
    {
        for (const auto &cell : snake)
        {
            cellShape.setPosition(grid.CellToPixel(cell.x), grid.CellToPixel(cell.y));
            window.draw(cellShape);
        }
        window.draw(appleShape);
        window.draw(pauseHintText);
    }
    window.display();
}

void Game::DrawGameOver()
{
    window.clear(sf::Color::Black);
    if (isGameOver)
    {
        window.draw(gameOverText);
        window.draw(restartText);
    }
    window.display();
}
