#ifndef SDL_CORE_HPP
#define SDL_CORE_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <stdexcept>

namespace sdl {

    /**
     * @class SDLException
     * @brief Exception for SDL errors
     */
    class SDLException : public std::runtime_error {
        public:
            explicit SDLException(const std::string& msg)
                : std::runtime_error(msg + ": " + SDL_GetError()) {}
    };

    /**
     * @class TTFException
     * @brief Exception for SDL_ttf errors
     */
    class TTFException : public std::runtime_error {
        public:
            explicit TTFException(const std::string& msg)
                : std::runtime_error(msg + ": " + TTF_GetError()) {}
    };

    /**
     * @class SDLCore
     * @brief Manage SDL and SDL_ttf initialization
     */
    class SDLCore {
        private:
            static bool initialized_;
            static bool ttf_initialized_;

        public:
            static void init(Uint32 flag = SDL_INIT_VIDEO);
            static void quit();
            static bool poll_quit();
            static void delay(Uint32 ms);
            static bool is_initialized() { return initialized_; }
            static bool is_ttf_initialized() { return ttf_initialized_; }
    };

}

#endif