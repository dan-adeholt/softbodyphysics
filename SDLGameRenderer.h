#ifndef __SDLGAMERENDERER_H
#define __SDLGAMERENDERER_H

class Game;
struct SDL_Renderer;

class SDLGameRenderer {
public:
    SDLGameRenderer();
    ~SDLGameRenderer();

    void render(SDL_Renderer *renderer, Game* game);
private:
    
};

#endif