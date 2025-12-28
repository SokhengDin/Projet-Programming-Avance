#ifndef SDL_FONT_HPP
#define SDL_FONT_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <memory>

namespace sdl {

    /**
     * @class SDLFont
     * @brief RAII wrapper for TTF_Font with text rendering utilities
     */
    class SDLFont {
        private:
            TTF_Font* font_;
            int size_;

        public:
            SDLFont(const std::string& path, int size);
            ~SDLFont();

            SDLFont(const SDLFont&) = delete;
            SDLFont& operator=(const SDLFont&) = delete;
            SDLFont(SDLFont&& other) noexcept;
            SDLFont& operator=(SDLFont&& other) noexcept;

            void render(
                SDL_Renderer* renderer
                , const std::string& text
                , int x
                , int y
                , SDL_Color color = {255, 255, 255, 255}
            );

            void render_centered(
                SDL_Renderer* renderer
                , const std::string& text
                , int x
                , int y
                , int w
                , int h
                , SDL_Color color = {255, 255, 255, 255}
            );

            void get_text_size(const std::string& text, int* w, int* h);
            TTF_Font* get() const { return font_; }
            int size() const { return size_; }
    };

}

#endif
