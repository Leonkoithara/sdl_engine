#ifndef WORLDGEN
#define WORLDGEN
#include <iostream>

#include "game_object.h"
#include "scene.h"

#define TERRAIN_TYPES 6


enum terrain { plains = 0, mountains, forests, water, human_civ, civ_ruins };

void start_create_world(int, GameObject *);
void create_new_world(Scene*, std::string, long = 0);
#endif
