/**
 * @file sdl_core.cpp
 * @brief SDL2 initialization implementation
 */

#include "sdl_core.hpp"

namespace sdl {

bool SDLCore::initialized_ = false;

void SDLCore::init(Uint32 flag) {
    if (!initialized_) {
        if (SDL_Init(flag) < 0) {
            throw SDLException("SDL_Init failed");
        }
        initialized_ = true;
    }
}

void SDLCore::quit() {
    if (initialized_) {
        SDL_Quit();
        initialized_ = false;
    }
}

bool SDLCore::poll_quit() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return true;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) return true;
            if (event.key.keysym.sym == SDLK_q) return true;
        }
    }
    return false;
}

void SDLCore::delay(Uint32 ms) {
    SDL_Delay(ms);
}

}
