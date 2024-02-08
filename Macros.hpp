#pragma once

#include <iostream>
#include <array>
#include <bitset>
#include <cstddef>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define TEXTURE_ROWS 300
#define TEXTURE_COLS 300

#define MAX_U8BIT 255
#define MAX_U16BIT 65535

#define MIN_BRUSH_SIZE 1
#define MAX_BRUSH_SIZE 9

#define OOB 255
#define EMPTY 0
#define WALL 1
#define SAND 2
#define WATER 3
#define FIRE 4
#define SMOKE 5
#define METAL 6
#define STEAM 7
#define TORCH 8
#define MOLTENMETAL 9

constexpr uint16_t EMPTY_BASETEMP = 295;
constexpr uint16_t WALL_BASETEMP = 295;
constexpr uint16_t SAND_BASETEMP = 295;
constexpr uint16_t WATER_BASETEMP = 295;
constexpr uint16_t FIRE_BASETEMP = 2000;
constexpr uint16_t SMOKE_BASETEMP = 295;
constexpr uint16_t METAL_BASETEMP = 295;
constexpr uint16_t STEAM_BASETEMP = 295;
constexpr uint16_t TORCH_BASETEMP = 2200;

#define FIRE_MINTEMP 1900
#define SMOKE_MINTEMP 70

#define EMPTY_MAXTEMP 255
#define FIRE_MAXTEMP 1000
#define WALL_MAXTEMP 255
#define SAND_MAXTEMP 200
#define WATER_MAXTEMP 100
#define SMOKE_MAXTEMP 254
#define METAL_MAXTEMP 200
#define STEAM_MAXTEMP 255 
#define TORCH_MAXTEMP 300

constexpr uint8_t SAND_COLOR[3] = { 170, 120, 20 };
constexpr uint8_t WATER_COLOR[3] = { 20, 20, 130 };
constexpr uint8_t FIRE_COLOR[3] = { 180, 130, 20};
constexpr uint8_t SMOKE_COLOR[3] = { 150, 150, 150 };
constexpr uint8_t METAL_COLOR[3] = { 70, 70, 70 };
constexpr uint8_t STEAM_COLOR[3] = { 120, 120, 150 };
constexpr uint8_t MOLTENMETAL_COLOR[3] = { 230, 230, 230 };

constexpr uint8_t SAND_R[2] = { 170, 255 };
constexpr uint8_t SAND_G[2] = { 120, 255 };
constexpr uint8_t SAND_B[2] = { 20, 255 };

// water

constexpr uint8_t FIRE_R[2] = { 180, 220 };
constexpr uint8_t FIRE_G[2] = { 0, 150 };
constexpr uint8_t FIRE_B[2] = { 25, 20 };

constexpr uint8_t METAL_R[2] = { 70, 230 };