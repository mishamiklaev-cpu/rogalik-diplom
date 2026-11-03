#pragma once

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float TILE_SIZE = 40.f;
const int MAP_CELLS_WIDTH = 40;
const int MAP_CELLS_HEIGHT = 30;

// полное здоровье игрока: старт, рестарт и потолок лечения зельем
const int MAX_HP_PLAYER = 100;

// урон за поимку ищейкой: 4 поимки = 100 урона = конец игры
const int DAMAGE_ON_CATCH = 25;

// шатун: бьёт слабее ищейки
const int DAMAGE_SHATUN = 20;

// пауза между шагами врагов (секунды); игрок шагает раз в 0.1 с
const float STEP_TIME_HOUND = 0.15f;
const float STEP_TIME_SHATUN = 0.3f; // шатун вдвое медленнее ищейки — от него можно уйти

// восстановление от зелья здоровья
const int POTION_HEAL = 20;

// частицы: базовая жизнь (сек) и число в одной вспышке
const float PARTICLE_LIFE_TIME = 0.6f;
const int PARTICLE_COUNT = 12;

// пауза между ударами, пока враг стоит на клетке игрока (секунды)
const float CONTACT_DAMAGE_INTERVAL = 0.8f;

// скорость медленного затемнения при смерти: один шаг за 0.5 с (в 4 раза медленнее перехода)
const float GAMEOVER_FADE_STEP_TIME = 0.5f;
const int WIDTH_NEW_ROOM = 8;
const int HEIGHT_NEW_ROOM = 4;
