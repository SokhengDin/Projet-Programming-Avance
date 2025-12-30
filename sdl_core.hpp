/**
 * @file sdl_core.hpp
 * @brief SDL2 initialization wrapper
 */

#ifndef SDL_CORE_HPP
#define SDL_CORE_HPP

#include <SDL.h>
#include <string>
#include <stdexcept>

namespace sdl {

class SDLException : public std::runtime_error {
public:
    explicit SDLException(const std::string& msg)
        : std::runtime_error(msg + ": " + SDL_GetError()) {}
};

class SDLCore {
private:
    static bool initialized_;

public:
    static void init(Uint32 flag = SDL_INIT_VIDEO);
    static void quit();
    static bool poll_quit();
    static void delay(Uint32 ms);
    static bool is_initialized() { return initialized_; }
};

}

#endif
