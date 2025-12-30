/**
 * @file sdl_heatmap.hpp
 * @brief Temperature visualization using Inferno colormap
 */

#ifndef SDL_HEATMAP_HPP
#define SDL_HEATMAP_HPP

#include "sdl_window.hpp"
#include <vector>

namespace sdl {

/**
 * @class SDLHeatmap
 * @brief Fullscreen temperature visualization
 *
 * Uses Inferno colormap (perceptually uniform).
 * 2D uses bilinear interpolation for smooth gradients.
 */
class SDLHeatmap {
private:
    SDLWindow& win_;
    double t_min_;
    double t_max_;

    void temp_to_rgb(double t, Uint8& r, Uint8& g, Uint8& b) const;

public:
    SDLHeatmap(SDLWindow& win, double t_min, double t_max);

    void set_range(double t_min, double t_max);
    void auto_range(const std::vector<double>& temps);
    void auto_range_2d(const std::vector<std::vector<double>>& temps);

    void draw_1d_fullscreen(const std::vector<double>& temps);
    void draw_2d_fullscreen(const std::vector<std::vector<double>>& temps);

    double get_min() const { return t_min_; }
    double get_max() const { return t_max_; }
};

}

#endif
