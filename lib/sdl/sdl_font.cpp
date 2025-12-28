#include "sdl_font.hpp"
#include "sdl_core.hpp"

namespace sdl {

    SDLFont::SDLFont(const std::string& path, int size)
        : font_(nullptr)
        , size_(size) {
        font_ = TTF_OpenFont(path.c_str(), size);
        if (!font_) {
            throw TTFException("Failed to load font: " + path);
        }
    }

    SDLFont::~SDLFont() {
        if (font_) {
            TTF_CloseFont(font_);
            font_ = nullptr;
        }
    }

    SDLFont::SDLFont(SDLFont&& other) noexcept
        : font_(other.font_)
        , size_(other.size_) {
        other.font_ = nullptr;
    }

    SDLFont& SDLFont::operator=(SDLFont&& other) noexcept {
        if (this != &other) {
            if (font_) {
                TTF_CloseFont(font_);
            }
            font_ = other.font_;
            size_ = other.size_;
            other.font_ = nullptr;
        }
        return *this;
    }

    void SDLFont::render(
        SDL_Renderer* renderer
        , const std::string& text
        , int x
        , int y
        , SDL_Color color
    ) {
        if (!font_ || text.empty()) return;

        SDL_Surface* surface = TTF_RenderText_Blended(font_, text.c_str(), color);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect dst = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }

    void SDLFont::render_centered(
        SDL_Renderer* renderer
        , const std::string& text
        , int x
        , int y
        , int w
        , int h
        , SDL_Color color
    ) {
        if (!font_ || text.empty()) return;

        SDL_Surface* surface = TTF_RenderText_Blended(font_, text.c_str(), color);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect dst = {
                x + (w - surface->w) / 2,
                y + (h - surface->h) / 2,
                surface->w,
                surface->h
            };
            SDL_RenderCopy(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }

    void SDLFont::get_text_size(const std::string& text, int* w, int* h) {
        if (font_ && !text.empty()) {
            TTF_SizeText(font_, text.c_str(), w, h);
        } else {
            if (w) *w = 0;
            if (h) *h = 0;
        }
    }

}
