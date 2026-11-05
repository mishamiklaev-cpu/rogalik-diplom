#pragma once
#include "Dungeon.h"
#include "GameTextures.h"
#include "Room.h"
#include <SFML/Graphics.hpp>
#include <vector>

// куда смотрит персонаж. Порядок совпадает с рядами в спрайт-листе:
// Down — ряд 0, Up — ряд 1, Right — ряд 2, Left — ряд 3
enum class Direction
{
    Down,
    Up,
    Right,
    Left
};

class Player
{
  public:
    Player();

    int xCellPlayer;
    int yCellPlayer;
    int widthPlayer;
    int heightPlayer;

    int hpPlayer;

    Direction directionPlayer; // куда смотрит: выбирает ряд в спрайт-листе
    int framePlayer;           // фаза бега, 0 или 1: выбирает столбец

    // пока не вызывается: задел под «продолжить игру» — вернуть игрока на сохранённую клетку
    void SetPositionPlayer(int xCellPlayer, int yCellPlayer);
    void SpawnPlayerInRandomRoom(const std::vector<Room> &rooms);
    void DrawPlayer(sf::RenderWindow &window, const GameTextures &textures);
    void MovePlayer(int xDirection, int yDirection, const Dungeon &dungeon);
};