/**
 * @file sdl_heatmap.hpp
 * @brief Temperature visualization using Inferno colormap
 */

#ifndef SDL_HEATMAP_HPP
#define SDL_HEATMAP_HPP

#include "sdl_window.hpp"
#include <vector>
#include <string>

namespace sdl {

/**
 * @brief Runtime simulation metadata for display
 */
struct SimInfo {
    std::string material_name;
    double alpha;           // Thermal diffusivity [m²/s]
    double time;            // Current simulation time [s]
    double tmax;            // Maximum simulation time [s]
    double L;               // Domain length [m]
    double u0;              // Boundary temperature [K]
    int speed;              // Simulation speed multiplier
    bool paused;
};

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
    void draw_number(SDL_Renderer* rend, int x, int y, double value) const;
    void draw_text(SDL_Renderer* rend, int x, int y, const char* text) const;
    void draw_colorbar(SDL_Renderer* rend, int x, int y, int w, int h) const;
    void draw_info_panel(SDL_Renderer* rend, const SimInfo& info) const;
    void draw_grid(SDL_Renderer* rend, int x0, int y0, int w, int h, int nx, int ny) const;

public:
    SDLHeatmap(SDLWindow& win, double t_min, double t_max);

    void set_range(double t_min, double t_max);
    void auto_range(const std::vector<double>& temps);
    void auto_range_2d(const std::vector<std::vector<double>>& temps);

    void draw_1d_fullscreen(const std::vector<double>& temps, const SimInfo& info);
    void draw_2d_fullscreen(const std::vector<std::vector<double>>& temps, const SimInfo& info);

    double get_min() const { return t_min_; }
    double get_max() const { return t_max_; }
};

}

#endif
