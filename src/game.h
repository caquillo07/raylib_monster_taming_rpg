//
// Created by Hector Mejia on 5/4/24.
//

#ifndef RAYLIB_POKEMON_CLONE_GAME_H
#define RAYLIB_POKEMON_CLONE_GAME_H

typedef struct Game {

} Game;

Game* game_new();
void game_handle_input(Game *game);
void game_destroy(Game *game);
void game_update(Game *game);
void game_draw(Game *game);

#endif //RAYLIB_POKEMON_CLONE_GAME_H
