//game.h
#pragma once

#ifndef GAME_H

#include "summerjam.h"
#include "audio.h"
#include <math.h>
#include "useful_summerjam.cpp"
#include "vector2_summer.h"

#define M_PI 3.14159265358979323846  /* pi */

struct Gameplay_Data;

typedef Input_State Update_Bot(Gameplay_Data data, int player_number);

typedef struct Entity {
	Vector2 pos;
	Vector2 velocity;
	f32 rotation;	
	f32 width;
	f32 height;
	Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool is_active = true;
	int health = 100;
	f32 t_rot;
} Entity;

typedef struct Tank {
	Entity ent;
	float fire_timer;
} Tank;

#define NUM_BLOCKS_MAP 1000
#define NUM_BULLETS 50

typedef struct Gameplay_Data {
	bool32 IsInitialized;
	Tank player1 = {};
	Tank player2 = {};
	Entity blocks[NUM_BLOCKS_MAP];
	Entity bullets[NUM_BULLETS];
	int block_count;
	Vector2 Camera_Pos = {};
	Vector2 starting_pos;
	Loaded_Sound MusicSound;
	Texture tank_texture;
	Texture turret_texture;
	Texture block_texture;
	Texture bullet_texture;
	texture_data map_tex;
	Update_Bot * update_player1_func;
	Update_Bot * update_player2_func;
} Gameplay_Data;

void UpdateGamePlay(platform_api* PlatformAPI, Game_Memory* memory, Input_State Input, Input_State Input2, f32 dt);

void RenderGameplay(platform_api* PlatformAPI, Game_Memory* memory, RenderBuffer &render_buffer);

#define GAME_h
#endif //GAME_H