#pragma once
#include <SFML/Graphics.hpp>

// все текстуры игры: загружаются один раз при старте и живут, пока идёт игра.
// Фигура хранит только адрес текстуры, поэтому текстура должна жить дольше фигуры
class GameTextures
{
  public:
    sf::Texture wallTexture;
    bool wallLoaded = false; // файл не нашёлся — стена рисуется цветом, а не белым пятном
    sf::Texture floorTexture;
    bool floorLoaded = false;
    sf::Texture doorTexture;
    bool doorLoaded = false;
    sf::Texture potionTexture;
    bool potionLoaded = false;
    sf::Texture playerTexture; // спрайт-лист: 2 столбца (фазы бега), 4 ряда (направления)
    bool playerLoaded = false;

    void LoadTextures();
};
