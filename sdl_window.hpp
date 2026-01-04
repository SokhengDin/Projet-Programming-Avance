/**
 * @file sdl_window.hpp
 * @brief SDL2 window and renderer wrapper
 */

#ifndef SDL_WINDOW_HPP
#define SDL_WINDOW_HPP

#include "SDL.h"
#include <string>

namespace sdl {

/**
 * @class SDLWindow
 * @brief Encapsulates an SDL window and renderer.
 *
 * This class manages the lifetime of an SDL_Window and SDL_Renderer.
 * It provides helper methods for clearing, presenting, fullscreen
 * toggling, and window title management.
 */
class SDLWindow {
private:
    SDL_Window* window_;     ///< SDL window handle
    SDL_Renderer* renderer_; ///< SDL renderer handle
    int width_;              ///< Window width in pixels
    int height_;             ///< Window height in pixels
    bool fullscreen_;        ///< Fullscreen state

public:
    /**
     * @brief Construct an SDL window and renderer.
     * @param title Window title
     * @param width Window width in pixels
     * @param height Window height in pixels
     * @param fullscreen Whether to start in fullscreen mode
     */
    SDLWindow(const std::string& title, int width, int height, bool fullscreen = false);

    /**
     * @brief Destroy the SDL window and renderer.
     */
    ~SDLWindow();

    SDLWindow(const SDLWindow&) = delete;
    SDLWindow& operator=(const SDLWindow&) = delete;

    /**
     * @brief Clear the window with a given color.
     * @param r Red component (0–255)
     * @param g Green component (0–255)
     * @param b Blue component (0–255)
     */
    void clear(Uint8 r = 0, Uint8 g = 0, Uint8 b = 0);

    /**
     * @brief Present the rendered frame on screen.
     */   
    void present();

    /**
     * @brief Change the window title.
     * @param title New window title
     */
    void set_title(const std::string& title);

    /**
     * @brief Toggle fullscreen mode.
     */
    void toggle_fullscreen();

    /**
     * @brief Check if the window is in fullscreen mode.
     */
    bool is_fullscreen() const { return fullscreen_; }

    /**
     * @brief Get the underlying SDL window.
     */
    SDL_Window* get_window() const { return window_; }

    /**
     * @brief Get the SDL renderer.
     */
    SDL_Renderer* get_renderer() const { return renderer_; }

    /**
     * @brief Get window width.
     */
    int get_width() const { return width_; }
    
    /**
     * @brief Get window height.
     */
    int get_height() const { return height_; }
};

}

#endif
