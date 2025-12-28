#ifndef SDL_WINDOW_HPP
#define SDL_WINDOW_HPP


#include "SDL.h"
#include <string>

namespace sdl {
    /**
     * @class SDL Window
     * @brief SDL Window and renderer wrapper
     */
    class SDLWindow {
        private:
            SDL_Window* window_;
            SDL_Renderer* renderer_;
            int width_;
            int height_;
            bool fullscreen_;

        public:
            /**
             * @brief Construct a window
             * @param title Window title
             * @param width Window width
             * @param height Window height
             * @param fullscreen Start in fullscreen mode
             */
            SDLWindow(
                const std::string& title
                , int width
                , int height
                , bool fullscreen = false
            );
            ~SDLWindow();

            SDLWindow(const SDLWindow&) = delete;
            SDLWindow& operator=(const SDLWindow&) = delete;

            void clear(Uint8 r = 0, Uint8 g = 0, Uint8 b = 0);
            void present();
            void set_title(const std::string& title);

            /**
             * @brief Toggle fullscreen mode
             */
            void toggle_fullscreen();

            /**
             * @brief Check if window is fullscreen
             */
            bool is_fullscreen() const { return fullscreen_; }

            /**
             * @brief Get the SDL window pointer
             */
            SDL_Window* get_window() const { return window_; }

            SDL_Renderer* get_renderer() const { return renderer_; }
            int get_width() const { return width_; }
            int get_height() const { return height_; }

    };
}


#endif