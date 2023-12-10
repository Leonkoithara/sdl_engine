#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "button.h"
#include "world_gen.h"


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

void create_new_world(Scene *prometheus, std::string world_name, long seed)
{
    std::string world_filename = "res/save/world.dat";
    int world_size = 50;
    int probablity_matrix[world_size][world_size][TERRAIN_TYPES];
    terrain terrain_matrix[world_size][world_size];
    if (seed == 0)
        srand(time(NULL));
    else
        srand(seed);

    int p = 100/TERRAIN_TYPES;
    for (int i=0; i<world_size; i++)
    {
        for (int j=0; j<world_size; j++)
        {
            for (int k=plains; k<TERRAIN_TYPES; k++)
            {
                if (i == 0 || j == 0 || i == world_size-1 || j == world_size-1)
                    probablity_matrix[i][j][k] = k==water||k==mountains?50:0;
                else
                    probablity_matrix[i][j][k] = p;
            }
        }
    }

    FILE *world = fopen(world_filename.c_str(), "rw");
    for (int i=0; i<world_size; i++)
    {
        for (int j=0; j<world_size; j++)
        {
            terrain t = get_terrain(probablity_matrix[i][j]);
            terrain_matrix[i][j] = t;
            switch (t)
			{
                case plains:
                case forests:
                    for (int k=plains; k<TERRAIN_TYPES; k++)
                    {
                        if (i+1 < world_size-1)
                            probablity_matrix[i+1][j][k] += k==t?(TERRAIN_TYPES-1)*5:-5;
                        if (j+1 < world_size-1)
                            probablity_matrix[i][j+1][k] += k==t?(TERRAIN_TYPES-1)*5:-5;
                        if (i+1 < world_size-1 && j+1 < world_size-1)
                            probablity_matrix[i+1][j+1][k] += k==t?(TERRAIN_TYPES-1)*5:-5;
                        if (i-1 > 0 && j+1 < world_size-1)
                            probablity_matrix[i-1][j+1][k] += k==t?(TERRAIN_TYPES-1)*5:-5;
                    }
                    break;
                case mountains:
                case water:
                    for (int k=plains; k<TERRAIN_TYPES; k++)
                    {
                        if (i+1 < world_size)
                            probablity_matrix[i+1][j][k] += k==t?(TERRAIN_TYPES-1)*7:-7;
                        if (j+1 < world_size)
                            probablity_matrix[i][j+1][k] += k==t?(TERRAIN_TYPES-1)*7:-7;
                    }
                    break;
                case human_civ:
                case civ_ruins:
                    for (int k=plains; k<TERRAIN_TYPES; k++)
                    {
                        int rad = 3;
                        for (int xr=-1*rad; xr<=rad; xr++)
						{
                            for (int yr=-1*rad; yr<=rad; yr++)
							{
                                if (i+xr < world_size && j+yr < world_size && i+xr >= 0 && j+yr >=0 && xr != 0 && yr != 0)
                                    probablity_matrix[i+xr][j+yr][k] += k==human_civ||k==civ_ruins?-15:15/(TERRAIN_TYPES-1);
                            }
                        }
                    }
                    break;
                default:
                    std::cout << "Unknown terrain" << std::endl;
            }
        }
    }

    int terrain_count[6] = {0, 0, 0, 0, 0};
    std::string texturefile_map[6] = {
        "plains.png", "hill.png", "tree.png", "water.png", "human_civ.png", "civ_ruins.png"
    };
    const char* terrain_name[6] = {
        "plains", "hills", "trees", "water", "human_civ", "civ_ruins"
    };
    for (int i=0; i<world_size; i++)
	{
        for (int j=0; j<world_size; j++)
        {
            char go_name[15];
            terrain_count[terrain_matrix[i][j]]++;
            snprintf(go_name, 15, "%s_%d", terrain_name[terrain_matrix[i][j]], terrain_count[terrain_matrix[i][j]]);
            Button *new_symbol = new Button(std::string(go_name));
            new_symbol->set_position({(float)25*i-320, (float)25*j-240});
            new_symbol->add_texturefile("res/textures/" + texturefile_map[terrain_matrix[i][j]], 0);
            prometheus->add_game_object(new_symbol);
        }
    }
}
