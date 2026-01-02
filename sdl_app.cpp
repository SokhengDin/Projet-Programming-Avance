/**
 * @file sdl_app.cpp
 * @brief SDL2 application implementation
 */

#include "sdl_app.hpp"
#include "sdl_core.hpp"
#include <algorithm>

namespace sdl {

SDLApp::SDLApp(
    SimType type,
    const ensiie::Material& mat,
    double L,
    double tmax,
    double u0,
    double f
)
    : window_(std::make_unique<SDLWindow>("Heat Equation", 800, 600, false))
    , heatmap_(std::make_unique<SDLHeatmap>(*window_, 280.0, 380.0))
    , solver_1d_(nullptr)
    , solver_2d_(nullptr)
    , sim_type_(type)
    , material_(mat)
    , L_(L)
    , tmax_(tmax)
    , u0_(u0)
    , f_(f)
    , n_(1001)
    , paused_(false)
    , speed_(10)
    , running_(true)
{
    start_simulation();
}

void SDLApp::start_simulation() {
    paused_ = false;

    if (sim_type_ == SimType::BAR_1D) {
        n_ = 1001;
        speed_ = 5;  // Same speed as 2D
        solver_1d_ = std::make_unique<ensiie::HeatEquationSolver1D>(
            material_, L_, tmax_, u0_, f_, n_
        );
        solver_2d_.reset();
    } else {
        n_ = 101;
        speed_ = 5;
        solver_2d_ = std::make_unique<ensiie::HeatEquationSolver2D>(
            material_, L_, tmax_, u0_, f_, n_
        );
        solver_1d_.reset();
    }
}

void SDLApp::render() {
    window_->clear(0, 0, 0);

    // Build simulation info for display
    SimInfo info;
    info.material_name = material_.name;
    info.alpha = material_.alpha();
    info.L = L_;
    info.tmax = tmax_;
    info.u0 = u0_ + 273.15;  // Convert to Kelvin for display
    info.speed = speed_;
    info.paused = paused_;

    if (sim_type_ == SimType::BAR_1D && solver_1d_) {
        info.time = solver_1d_->get_time();
        auto temps = solver_1d_->get_temperature();
        if (!temps.empty()) {
            heatmap_->auto_range(temps);
            heatmap_->draw_1d_fullscreen(temps, info);
        }
    } else if (sim_type_ == SimType::PLATE_2D && solver_2d_) {
        info.time = solver_2d_->get_time();
        auto temps = solver_2d_->get_temperature_2d();
        if (!temps.empty() && !temps[0].empty()) {
            heatmap_->auto_range_2d(temps);
            heatmap_->draw_2d_fullscreen(temps, info);
        }
    }

    window_->present();
}

void SDLApp::process_events(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                running_ = false;
                break;
            case SDLK_SPACE:
                paused_ = !paused_;
                break;
            case SDLK_r:
                if (solver_1d_) solver_1d_->reset();
                if (solver_2d_) solver_2d_->reset();
                paused_ = false;
                break;
            case SDLK_UP:
                speed_ = std::min(sim_type_ == SimType::BAR_1D ? 50 : 20, speed_ + 5);
                break;
            case SDLK_DOWN:
                speed_ = std::max(1, speed_ - 5);
                break;
        }
    }
}

void SDLApp::run() {
    while (running_) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_WINDOWEVENT &&
                 event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                running_ = false;
            }
            process_events(event);
        }

        if (!running_) break;

        if (!paused_) {
            for (int i = 0; i < speed_; i++) {
                if (sim_type_ == SimType::BAR_1D && solver_1d_) {
                    if (!solver_1d_->step()) {
                        paused_ = true;
                        break;
                    }
                } else if (sim_type_ == SimType::PLATE_2D && solver_2d_) {
                    if (!solver_2d_->step()) {
                        paused_ = true;
                        break;
                    }
                }
            }
        }

        render();
        SDLCore::delay(16);
    }

    heatmap_.reset();
    window_.reset();
}

}
