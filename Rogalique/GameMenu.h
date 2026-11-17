#pragma once
#include "GameSounds.h"
#include <SFML/Graphics.hpp>
#include <string>

// всё общее для экранов меню: какой экран сейчас открыт и как устроены пункты.
// Одни и те же три функции работают для главного меню, опций, паузы, подтверждений и экрана смерти

// одно поле вместо набора bool: два состояния сразу быть не может
enum class GameState
{
    Menu,
    Playing,
    Paused,
    Records,
    Options
};

// собирает пункты меню: текст и место по центру окна, столбиком сверху вниз
void BuildMenuItems(sf::Text items[], const std::string names[], int count,
                    const sf::Font &font, float yFirst);

// стрелки двигают выбор по кругу и щёлкают, Enter звучит подтверждением
void MoveMenuSelection(int &selected, int count, sf::Keyboard::Key key, GameSounds &sounds);

// рисует пункты: выбранный золотой, остальные белые
void DrawMenuItems(sf::RenderWindow &window, sf::Text items[], int count, int selected);
