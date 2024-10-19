#include "world_gen.h"


void set_elevation(long x, long y)
{
    // currently set everything to constant val
    world->submap_generated_flag[x][y] = true;
    for( int i=0; i<world->world_size; i++)
    {
        for( int j=0; j<world->world_size; j++)
            world->submap_data_matrix[x][y].elevation_matrix[i][j] = 0;
    }
}

void create_submap(int x, GameObject *icon)
{
    std::cout << "Clicked: " << icon->get_tag("terrain")->strval << ", at: " << icon->get_tag("xpos")->longval << "," << icon->get_tag("ypos")->longval << std::endl;
    set_elevation(icon->get_tag("xpos")->longval, icon->get_tag("ypos")->longval);
}
