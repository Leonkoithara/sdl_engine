#ifndef GAME_MANAGER
#define GAME_MANAGER

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

#include "game_object.h"
#include "scene.h"
#include "type_structs.h"

class GameManager
{
private:
    bool running;
    vec3D screen_size;
    std::unordered_map<std::string, Scene*> scenes;
    std::vector<Scene*> delete_scene_queue;
public:
    GameManager();
    ~GameManager();

    void update();
    void event_handler();
    void render();
    void quit();

    GameObject* instantiate_game_object(std::string, std::string, float, float);

    bool get_running_stat() { return running; }
    vec3D get_screen_size() { return screen_size; }
    Scene* get_scene_by_name(std::string);

    void add_empty_scene(std::string, int, int, int, int);
    void add_scene(Scene*);
    void add_game_object(GameObject*, std::string);

    void delete_scene(std::string);
};

extern GameManager gm;
#endif
