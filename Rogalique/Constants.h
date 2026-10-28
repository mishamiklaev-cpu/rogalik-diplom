#pragma once

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float TILE_SIZE = 40.f;
const int MAP_CELLS_WIDTH = 40;
const int MAP_CELLS_HEIGHT = 30;

// урон за поимку ищейкой: 4 поимки = 100 урона = конец игры
const int DAMAGE_ON_CATCH = 25;

// восстановление от зелья здоровья
const int POTION_HEAL = 20;

// пауза между ударами, пока враг стоит на клетке игрока (секунды)
const float CONTACT_DAMAGE_INTERVAL = 0.8f;

// скорость медленного затемнения при смерти: один шаг за 0.5 с (в 4 раза медленнее перехода)
const float GAMEOVER_FADE_STEP_TIME = 0.5f;
const int widthNewRoom = 8;
const int heightNewRoom = 4;
