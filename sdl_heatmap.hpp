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
 * 
 * This structure contains all necessary information to display
 * simulation status and material properties in the visualization.
 */
struct SimInfo {
    std::string material_name;  ///< Name of the material being simulated
    double alpha;               ///< Thermal diffusivity [m²/s]
    double time;                ///< Current simulation time [s]
    double tmax;                ///< Maximum simulation time [s]
    double L;                   ///< Domain length [m]
    double u0;                  ///< Boundary temperature [K]
    int speed;                  ///< Simulation speed multiplier
    bool paused;                ///< Simulation pause state
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
    SDLWindow& win_;  ///< Reference to SDL window
    double t_min_;    ///< Minimum temperature for colormap scaling [K]
    double t_max_;    ///< Maximum temperature for colormap scaling [K]

    /**
     * @brief Convert temperature to RGB color using Inferno colormap
     * @param t Temperature value to convert [K]
     * @param r Output red component (0-255)
     * @param g Output green component (0-255)
     * @param b Output blue component (0-255)
     */
    void temp_to_rgb(double t, Uint8& r, Uint8& g, Uint8& b) const;

    /**
     * @brief Draw a numerical value at specified position
     * @param rend SDL renderer
     * @param x X coordinate in pixels
     * @param y Y coordinate in pixels
     * @param value Numerical value to display
     */
    void draw_number(SDL_Renderer* rend, int x, int y, double value) const;

    /**
     * @brief Draw text string at specified position
     * @param rend SDL renderer
     * @param x X coordinate in pixels
     * @param y Y coordinate in pixels
     * @param text C-string to display
     */
    void draw_text(SDL_Renderer* rend, int x, int y, const char* text) const;

    /**
     * @brief Draw vertical colorbar legend
     * @param rend SDL renderer
     * @param x X coordinate of top-left corner
     * @param y Y coordinate of top-left corner
     * @param w Width in pixels
     * @param h Height in pixels
     */
    void draw_colorbar(SDL_Renderer* rend, int x, int y, int w, int h) const;

    /**
     * @brief Draw information panel with simulation metadata
     * @param rend SDL renderer
     * @param info Simulation information to display
     */
    void draw_info_panel(SDL_Renderer* rend, const SimInfo& info) const;

    /**
     * @brief Draw grid lines for spatial reference
     * @param rend SDL renderer
     * @param x0 Starting X coordinate
     * @param y0 Starting Y coordinate
     * @param w Grid width in pixels
     * @param h Grid height in pixels
     * @param nx Number of vertical grid lines
     * @param ny Number of horizontal grid lines
     */
    void draw_grid(SDL_Renderer* rend, int x0, int y0, int w, int h, int nx, int ny) const;

public:
    /**
     * @brief Construct heatmap visualizer
     * @param win Reference to SDL window for rendering
     * @param t_min Initial minimum temperature for scaling [K]
     * @param t_max Initial maximum temperature for scaling [K]
     */
    SDLHeatmap(SDLWindow& win, double t_min, double t_max);

    /**
     * @brief Manually set temperature range for colormap scaling
     * @param t_min Minimum temperature [K]
     * @param t_max Maximum temperature [K]
     */
    void set_range(double t_min, double t_max);

    /**
     * @brief Automatically determine range from 1D temperature data
     * @param temps Vector of temperature values [K]
     */
    void auto_range(const std::vector<double>& temps);

    /**
     * @brief Automatically determine range from 2D temperature data
     * @param temps 2D vector of temperature values [K]
     */
    void auto_range_2d(const std::vector<std::vector<double>>& temps);

    /**
     * @brief Draw 1D temperature distribution in fullscreen mode
     * @param temps Vector of temperature values [K]
     * @param info Simulation metadata for display
     */
    void draw_1d_fullscreen(const std::vector<double>& temps, const SimInfo& info);

    /**
     * @brief Draw 2D temperature distribution in fullscreen mode
     * @param temps 2D grid of temperature values [K]
     * @param info Simulation metadata for display
     */
    void draw_2d_fullscreen(const std::vector<std::vector<double>>& temps, const SimInfo& info);

    /**
     * @brief Draw 1D temperature distribution in a grid cell (2x2 mode)
     * @param temps Vector of temperature values [K]
     * @param info Simulation metadata for display
     * @param cell_x X coordinate of cell top-left corner
     * @param cell_y Y coordinate of cell top-left corner
     * @param cell_w Cell width in pixels
     * @param cell_h Cell height in pixels
     */
    void draw_1d_cell(const std::vector<double>& temps, const SimInfo& info,
                      int cell_x, int cell_y, int cell_w, int cell_h);

    /**
     * @brief Draw 2D temperature distribution in a grid cell (2x2 mode)
     * @param temps 2D grid of temperature values [K]
     * @param info Simulation metadata for display
     * @param cell_x X coordinate of cell top-left corner
     * @param cell_y Y coordinate of cell top-left corner
     * @param cell_w Cell width in pixels
     * @param cell_h Cell height in pixels
     */
    void draw_2d_cell(const std::vector<std::vector<double>>& temps, const SimInfo& info,
                      int cell_x, int cell_y, int cell_w, int cell_h);

    /**
     * @brief Get current minimum temperature of colormap range
     * @return Minimum temperature [K]
     */
    double get_min() const { return t_min_; }

    /**
     * @brief Get current maximum temperature of colormap range
     * @return Maximum temperature [K]
     */
    double get_max() const { return t_max_; }
};

}

#endif
