/**
 * @file sdl_core.hpp
 * @brief SDL2 initialization and event handling wrapper.
 *
 * This module provides a minimal RAII-style wrapper around SDL2
 * initialization, shutdown, and event polling.
 */

#ifndef SDL_CORE_HPP
#define SDL_CORE_HPP

#include <SDL.h>
#include <string>
#include <stdexcept>

namespace sdl {
/**
 * @class SDLException
 * @brief Exception thrown when an SDL error occurs
 */
class SDLException : public std::runtime_error {
public:
    /**
     * @brief Construct an SDL exception with SDL error message
     * @param msg Custom error message
     */
    explicit SDLException(const std::string& msg)
        : std::runtime_error(msg + ": " + SDL_GetError()) {}
};

/**
 * @class SDLCore
 * @brief Static wrapper for SDL initialization and event handling
 */
class SDLCore {
private:
    static bool initialized_; ///< SDL initialization state

public:
    /**
     * @brief Initialize SDL
     * @param flag SDL initialization flags (default: SDL_INIT_VIDEO)
     */
    static void init(Uint32 flag = SDL_INIT_VIDEO);

    /**
     * @brief Quit SDL properly
     */
    static void quit();

    /**
     * @brief Poll SDL events and detect quit request
     * @return true if user wants to quit
     */
    static bool poll_quit();

    /**
     * @brief Delay execution
     * @param ms Milliseconds to wait
     */
    static void delay(Uint32 ms);

    /**
     * @brief Check if SDL is initialized
     * @return true if initialized
     */
    static bool is_initialized() { return initialized_; }
};

}

#endif
