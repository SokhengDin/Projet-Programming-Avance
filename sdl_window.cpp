/**
 * @file sdl_window.cpp
 * @brief SDL2 window implementation
 */

#include "sdl_window.hpp"
#include "sdl_core.hpp"

namespace sdl {

SDLWindow::SDLWindow(const std::string& title, int width, int height, bool fullscreen)
    : window_(nullptr)
    , renderer_(nullptr)
    , width_(width)
    , height_(height)
    , fullscreen_(fullscreen)
{
    Uint32 flags = SDL_WINDOW_SHOWN;

    // If width/height are 0, use maximized window
    if (width == 0 || height == 0) {
        flags |= SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE;
        // Get display bounds for initial size
        SDL_DisplayMode dm;
        SDL_GetCurrentDisplayMode(0, &dm);
        width  = dm.w;
        height = dm.h;
    }

    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        flags
    );

    if (!window_) {
        throw SDLException("SDL_CreateWindow failed");
    }

    if (fullscreen_) {
        SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    // Get actual window size
    SDL_GetWindowSize(window_, &width_, &height_);

    renderer_ = SDL_CreateRenderer(
        window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer_) {
        SDL_DestroyWindow(window_);
        throw SDLException("SDL_CreateRenderer failed");
    }
}

SDLWindow::~SDLWindow() {
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
}

void SDLWindow::clear(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
    SDL_RenderClear(renderer_);
}

void SDLWindow::present() {
    SDL_RenderPresent(renderer_);
}

void SDLWindow::set_title(const std::string& title) {
    SDL_SetWindowTitle(window_, title.c_str());
}

void SDLWindow::toggle_fullscreen() {
    fullscreen_ = !fullscreen_;
    SDL_SetWindowFullscreen(window_, fullscreen_ ? SDL_WINDOW_FULLSCREEN : 0);
    SDL_GetWindowSize(window_, &width_, &height_);
}

}
