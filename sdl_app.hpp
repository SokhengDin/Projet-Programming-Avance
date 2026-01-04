/**
 * @file sdl_app.hpp
 * @brief SDL2 application for heat equation visualization
 */

#ifndef SDL_APP_HPP
#define SDL_APP_HPP

#include "SDL.h"
#include "sdl_window.hpp"
#include "sdl_heatmap.hpp"
#include "material.hpp"
#include "heat_equation_solver.hpp"
#include <memory>

namespace sdl {

/**
 * @class SDLApp
 * @brief Heat simulation with fullscreen visualization
 *
 * Controls: SPACE=pause, R=reset, UP/DOWN=speed, ESC=quit
 */
class SDLApp {
public:
    /**
     * @brief Type of simulation
     */
    enum class SimType { 
        BAR_1D,       ///< 1D heat equation (bar)
        PLATE_2D };   ///< 2D heat equation (plate)

private:
    std::unique_ptr<SDLWindow> window_;
    std::unique_ptr<SDLHeatmap> heatmap_;
    std::unique_ptr<ensiie::HeatEquationSolver1D> solver_1d_;
    std::unique_ptr<ensiie::HeatEquationSolver2D> solver_2d_;

    SimType sim_type_;           ///< Simulation type
    ensiie::Material material_;  ///< Selected material

    double L_;       ///< Domain size
    double tmax_;    ///< Maximum simulation time
    double u0_;      ///< Initial temperature
    double f_;       ///< Source intensity
    int n_;          ///< Grid resolution

    bool paused_;    ///< Pause state
    int speed_;      ///< Simulation speed
    bool running_;   ///< Application state
    bool grid_mode_; ///< Multi-material grid mode

    // For grid mode: 4 solvers (one per material)
    std::unique_ptr<ensiie::HeatEquationSolver1D> solvers_1d_[4];
    std::unique_ptr<ensiie::HeatEquationSolver2D> solvers_2d_[4];
    ensiie::Material materials_[4];

    void render();
    void render_grid();
    void process_events(SDL_Event& event);
    void start_simulation();
    void start_grid_simulation();

public:
    /**
     * @brief Create application for single material simulation
     */
    SDLApp(
        SimType type,
        const ensiie::Material& mat,
        double L,
        double tmax,
        double u0,
        double f
    );

    /**
     * @brief Create application in grid mode (all materials)
     */
    SDLApp(
        SimType type,
        double L,
        double tmax,
        double u0,
        double f
    );

    /**
     * @brief Run the application main loop
     */
    void run();
};

}

#endif
