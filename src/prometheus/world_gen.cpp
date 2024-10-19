#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "button.h"
#include "camera.h"
#include "scene.h"
#include "game_manager.h"
#include "game_object.h"
#include "type_structs.h"
#include "world_gen.h"


worldgen_data *world;

terrain get_terrain(int *probablity_row)
{
    int rand_num = rand()%101;
    int sum = 0;
    for (int i=plains; i<TERRAIN_TYPES; i++)
	{
        if (rand_num < sum+*(probablity_row+i))
            return static_cast<terrain>(i);
        sum += *(probablity_row+i);
    }
    return plains;
}

void modify_probablities(int *probablity_row, int p_delta, terrain t)
{
    int changed = 0;
    int temp = *(probablity_row+t) + p_delta;

    if (temp < 0)
    {
        p_delta = -1 * (*(probablity_row+t));
        *(probablity_row+t) = 0;
    }
    else if (temp > 100)
    {
        p_delta = 100 - *(probablity_row+t);
        *(probablity_row+t) = 100;
    }
    else
        *(probablity_row+t) += p_delta;

    int remains = p_delta;

    while (std::abs(changed) < std::abs(p_delta))
    {
        int change_each = remains/(TERRAIN_TYPES-1);
        if (change_each == 0)
            change_each = remains/std::abs(remains);
        int old_changed = changed;

        for (int i=plains; i<TERRAIN_TYPES; i++)
        {
            int change = 0;
            if (i != t && std::abs(changed) < std::abs(p_delta))
            {
                temp = *(probablity_row+i) - change_each;
                if (temp < 0)
				{
                    change = *(probablity_row+i);
                    *(probablity_row+i) = 0;
                }
                else if (temp > 100)
                {
                    change = *(probablity_row+i) - 100;
                    *(probablity_row+i) = 100;
                }
                else
                {
                    change = change_each;
                    *(probablity_row+i) -= change_each;
                }
            }
            changed -= change;
            remains -= change;
        }

        if (changed == old_changed)
		{
            std::cout << probablity_row[0] << " " << probablity_row[1] << " " << probablity_row[2] << " " << probablity_row[3] << " " << probablity_row[4] << " " << probablity_row[5] << " " << std::endl;
            std::cout << "Cannot change " << probablity_row[t] << " by " << p_delta << std::endl;
            break;
        }
    }
}

void camera_translate(int x, GameObject *obj)
{
    enum direction { up = 0, down, right, left };
    int dir = obj->get_tag("direction")->longval;

    switch (dir)
	{
        case up:
            cam.camera_translate({0, -10});
            break;
        case down:
            cam.camera_translate({0, 10});
            break;
        case right:
            cam.camera_translate({10, 0});
            break;
        case left:
            cam.camera_translate({-10, 0});
            break;
    }
}

void start_create_world(int x, GameObject *)
{
    Scene *new_world_gen = new Scene("prometheus");
    int world_size = 100;
    std::string world_name = "begins";

    int winx = world_size*25, winy = world_size*25;
    vec3D screen_size = gm.get_screen_size();
    if (winx > screen_size.x)
        winx = screen_size.x;
    if (winy > screen_size.y)
        winy = screen_size.y;

    Button *scroll_up = new Button("worldmap_w", 'w');
    scroll_up->add_tag("direction", (long)0);
    Button *scroll_down = new Button("worldmap_s", 's');
    scroll_down->add_tag("direction", 1);
    Button *scroll_right = new Button("worldmap_d", 'd');
    scroll_right->add_tag("direction", 2);
    Button *scroll_left = new Button("worldmap_a", 'a');
    scroll_left->add_tag("direction", 3);
    scroll_up->set_onclickevent(&camera_translate);
    scroll_down->set_onclickevent(&camera_translate);
    scroll_right->set_onclickevent(&camera_translate);
    scroll_left->set_onclickevent(&camera_translate);

    new_world_gen->create_window("Prometheus", 0, 0, winx, winy, false);
    new_world_gen->add_game_object(scroll_up);
    new_world_gen->add_game_object(scroll_down);
    new_world_gen->add_game_object(scroll_right);
    new_world_gen->add_game_object(scroll_left);

    create_new_world(new_world_gen, world_name, world_size);

    gm.add_scene(new_world_gen);
    gm.delete_scene("main_menu");
}

void create_new_world(Scene *prometheus, std::string world_name, int world_size)
{
    std::string world_filename = "res/save/" + world_name + "/world->dat";
    world = new worldgen_data(world_size, world_name);
    int probablity_matrix[world->world_size][world->world_size][TERRAIN_TYPES];

    // currently always 0, todo add optional seed input
    if (world->seed == 0)
        srand(time(NULL));
    else
        srand(world->seed);

    int p = 100/TERRAIN_TYPES, rem = 100%TERRAIN_TYPES;
    for (int i=0; i<world->world_size; i++)
    {
        for (int j=0; j<world->world_size; j++)
        {
            for (int k=plains; k<TERRAIN_TYPES; k++)
            {
                probablity_matrix[i][j][k] = p;
            }
            probablity_matrix[i][j][rand()%TERRAIN_TYPES] += rem;
        }
    }

    for (int i=0; i<world->world_size; i++)
    {
        for (int j=0; j<world->world_size; j++)
        {
            terrain t = get_terrain(probablity_matrix[i][j]);
            world->terrain_matrix[i][j] = t;
            int rad = 3;
            switch (t)
			{
                case plains:
                case forests:
                    if (i+1 < world->world_size)
                        modify_probablities(probablity_matrix[i+1][j], (TERRAIN_TYPES-1)*5.5, t);
                    if (j+1 < world->world_size)
                        modify_probablities(probablity_matrix[i][j+1], (TERRAIN_TYPES-1)*5.5, t);
                    if (i+1 < world->world_size && j+1 < world->world_size)
                        modify_probablities(probablity_matrix[i+1][j+1], (TERRAIN_TYPES-1)*5.5, t);
                    if (i-1 > 0 && j+1 < world->world_size)
                        modify_probablities(probablity_matrix[i-1][j+1], (TERRAIN_TYPES-1)*5.5, t);
                    break;
                case mountains:
                    if (i+1 < world->world_size)
                        modify_probablities(probablity_matrix[i+1][j], (TERRAIN_TYPES-1)*7, t);
                    if (j+1 < world->world_size)
                        modify_probablities(probablity_matrix[i][j+1], (TERRAIN_TYPES-1)*7, t);
                    break;
                case water:
                    if (i+1 < world->world_size)
                        modify_probablities(probablity_matrix[i+1][j], (TERRAIN_TYPES-1)*8, t);
                    if (j+1 < world->world_size)
                        modify_probablities(probablity_matrix[i][j+1], (TERRAIN_TYPES-1)*8, t);
                    break;
                case human_civ:
                case civ_ruins:
                    for (int xr=-1*rad; xr<=rad; xr++)
                    {
                        for (int yr=-1*rad; yr<=rad; yr++)
                        {
                            if (i+xr < world->world_size && j+yr < world->world_size && i+xr >= 0 && j+yr >=0 && xr != 0 && yr != 0)
                            {
                                modify_probablities(probablity_matrix[i+xr][j+yr], -15, human_civ);
                                modify_probablities(probablity_matrix[i+xr][j+yr], -15, civ_ruins);
                            }
                        }
                    }
                    break;
                default:
                    std::cout << "Unknown terrain" << std::endl;
            }
        }
    }

    int terrain_count[6] = {0, 0, 0, 0, 0};                                                   // Count in map for each terrain
    std::string texturefile_map[6] = {
        "plains", "hill", "tree", "water", "human_civ", "civ_ruins"
    };
    const char* terrain_name[6] = {
        "plains", "hills", "trees", "water", "human_civ", "civ_ruins"
    };
    for (int i=0; i<world->world_size; i++)
	{
        for (int j=0; j<world->world_size; j++)
        {
            char go_name[15];
            terrain_count[world->terrain_matrix[i][j]]++;
            snprintf(go_name, 15, "%s_%d", terrain_name[world->terrain_matrix[i][j]], terrain_count[world->terrain_matrix[i][j]]);
            Button *new_symbol = new Button(std::string(go_name));
            new_symbol->add_tag("terrain", texturefile_map[world->terrain_matrix[i][j]].c_str());
            new_symbol->add_tag("xpos", i);
            new_symbol->add_tag("ypos", j);
            new_symbol->set_onclickevent(create_submap);
            new_symbol->set_position({(float)25*i, (float)25*j});
            new_symbol->add_texturefile("res/textures/" + texturefile_map[world->terrain_matrix[i][j]] + ".png", 0);
            prometheus->add_game_object(new_symbol);
        }
    }
}
