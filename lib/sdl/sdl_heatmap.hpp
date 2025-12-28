#ifndef SDL_HEATMAP_HPP
#define SDL_HEATMAP_HPP

#include "sdl_window.hpp"
#include "sdl_font.hpp"
#include <vector>
#include <memory>

namespace sdl {

    /**
     * @class SDLHeatmap
     * @brief Draw temperature as colors with color bar and overlay
     */
    class SDLHeatmap {
        private:
            SDLWindow& win_;
            double t_min_;
            double t_max_;
            std::unique_ptr<SDLFont> font_;
            std::unique_ptr<SDLFont> label_font_;

            void temp_to_rgb(double t, Uint8& r, Uint8& g, Uint8& b) const;

        public:
            SDLHeatmap(SDLWindow& win, double t_min, double t_max);

            void set_range(double t_min, double t_max);
            void auto_range(const std::vector<double>& temps);
            void auto_range_2d(const std::vector<std::vector<double>>& temps);

            void draw_bar_1d(
                const std::vector<double>& temps
                , int x
                , int y
                , int width
                , int height
            );

            void draw_plate_2d(
                const std::vector<std::vector<double>>& temps
                , int x
                , int y
                , int size
            );

            void draw_colorbar(int x, int y, int w, int h);

            void draw_info(
                const std::string& material
                , double time
                , double tmax
                , bool paused
                , int x
                , int y
            );

            void draw_heat_sources_1d(
                int x
                , int y
                , int width
                , int height
            );

            void draw_heat_sources_2d(
                int x
                , int y
                , int size
            );

            void draw_stats(
                const std::vector<double>& temps
                , int x
                , int y
            );

            void draw_stats_2d(
                const std::vector<std::vector<double>>& temps
                , int x
                , int y
            );

            void draw_boundary_markers_1d(
                int x
                , int y
                , int width
                , int height
            );

            void draw_boundary_markers_2d(
                int x
                , int y
                , int size
            );

            void draw_heat_flow_1d(
                const std::vector<double>& temps
                , int x
                , int y
                , int width
                , int height
            );

            void draw_heat_flow_2d(
                const std::vector<std::vector<double>>& temps
                , int x
                , int y
                , int size
            );

            double get_min() const { return t_min_; }
            double get_max() const { return t_max_; }
    };

}

#endif