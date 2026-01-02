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
    enum class SimType { BAR_1D, PLATE_2D };

private:
    std::unique_ptr<SDLWindow> window_;
    std::unique_ptr<SDLHeatmap> heatmap_;
    std::unique_ptr<ensiie::HeatEquationSolver1D> solver_1d_;
    std::unique_ptr<ensiie::HeatEquationSolver2D> solver_2d_;

    SimType sim_type_;
    ensiie::Material material_;

    double L_;
    double tmax_;
    double u0_;
    double f_;
    int n_;

    bool paused_;
    int speed_;
    bool running_;
    bool grid_mode_;  // 2x2 grid mode for all materials

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
    // Single material mode
    SDLApp(
        SimType type,
        const ensiie::Material& mat,
        double L,
        double tmax,
        double u0,
        double f
    );

    // Grid mode: all 4 materials
    SDLApp(
        SimType type,
        double L,
        double tmax,
        double u0,
        double f
    );

    void run();
};

}

#endif
