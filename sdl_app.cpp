/**
 * @file sdl_app.cpp
 * @brief SDL2 application implementation
 */

#include "sdl_app.hpp"
#include "sdl_core.hpp"
#include <algorithm>

namespace sdl {

// Single material constructor
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
    , grid_mode_(false)
{
    start_simulation();
}

// Grid mode constructor (all 4 materials)
SDLApp::SDLApp(
    SimType type,
    double L,
    double tmax,
    double u0,
    double f
)
    : window_(nullptr)
    , heatmap_(nullptr)
    , solver_1d_(nullptr)
    , solver_2d_(nullptr)
    , sim_type_(type)
    , material_(ensiie::Materials::COPPER)
    , L_(L)
    , tmax_(tmax)
    , u0_(u0)
    , f_(f)
    , n_(1001)
    , paused_(false)
    , speed_(10)
    , running_(true)
    , grid_mode_(true)
{
    // Use 0,0 to trigger maximized window mode
    window_ = std::make_unique<SDLWindow>("Heat Equation - All Materials", 0, 0, false);
    heatmap_ = std::make_unique<SDLHeatmap>(*window_, 280.0, 380.0);

    // Initialize 4 materials
    materials_[0] = ensiie::Materials::COPPER;
    materials_[1] = ensiie::Materials::IRON;
    materials_[2] = ensiie::Materials::GLASS;
    materials_[3] = ensiie::Materials::POLYSTYRENE;

    start_grid_simulation();
}

void SDLApp::start_simulation() {
    paused_ = false;

    if (sim_type_ == SimType::BAR_1D) {
        n_ = 1001;
        speed_ = 1;  
        solver_1d_ = std::make_unique<ensiie::HeatEquationSolver1D>(
            material_, L_, tmax_, u0_, f_, n_
        );
        solver_2d_.reset();
    } else {
        n_ = 101;
        speed_ = 1;  
        solver_2d_ = std::make_unique<ensiie::HeatEquationSolver2D>(
            material_, L_, tmax_, u0_, f_, n_
        );
        solver_1d_.reset();
    }
}

void SDLApp::start_grid_simulation() {
    paused_ = false;

    if (sim_type_ == SimType::BAR_1D) {
        n_ = 1001;
        speed_ = 1;  
        for (int i = 0; i < 4; i++) {
            solvers_1d_[i] = std::make_unique<ensiie::HeatEquationSolver1D>(
                materials_[i], L_, tmax_, u0_, f_, n_
            );
            solvers_2d_[i].reset();
        }
    } else {
        n_ = 101;
        speed_ = 1;  
        for (int i = 0; i < 4; i++) {
            solvers_2d_[i] = std::make_unique<ensiie::HeatEquationSolver2D>(
                materials_[i], L_, tmax_, u0_, f_, n_
            );
            solvers_1d_[i].reset();
        }
    }
}

void SDLApp::render() {
    window_->clear(0, 0, 0);

    SimInfo info;
    info.material_name = material_.name;
    info.alpha = material_.alpha();
    info.L = L_;
    info.tmax = tmax_;
    info.u0 = u0_ + 273.15;
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

void SDLApp::render_grid() {
    window_->clear(0, 0, 0);

    int win_w = window_->get_width();
    int win_h = window_->get_height();
    int cell_w = win_w / 2;
    int cell_h = win_h / 2;

    // Grid positions: [0]=top-left, [1]=top-right, [2]=bottom-left, [3]=bottom-right
    int cell_x[4] = {0, cell_w, 0, cell_w};
    int cell_y[4] = {0, 0, cell_h, cell_h};

    // Reference temperature for ΔT calculation
    double u0_kelvin = u0_ + 273.15;

    // Find global min/max ΔT (temperature increase from u0)
    double global_min = 0.0;  // ΔT minimum is 0 (no heating)
    double global_max = 0.0;

    if (sim_type_ == SimType::BAR_1D) {
        for (int i = 0; i < 4; i++) {
            if (solvers_1d_[i]) {
                auto temps = solvers_1d_[i]->get_temperature();
                for (double t : temps) {
                    double delta_t = t - u0_kelvin;
                    global_max = std::max(global_max, delta_t);
                }
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            if (solvers_2d_[i]) {
                auto temps = solvers_2d_[i]->get_temperature_2d();
                for (const auto& row : temps) {
                    for (double t : row) {
                        double delta_t = t - u0_kelvin;
                        global_max = std::max(global_max, delta_t);
                    }
                }
            }
        }
    }

    // Set range for ΔT: 0 to max with small margin
    double margin = global_max * 0.05;
    if (global_max < 0.1) global_max = 1.0;  // Minimum range for visibility
    heatmap_->set_range(global_min, global_max + margin);

    // Render each material in its cell
    for (int i = 0; i < 4; i++) {
        SimInfo info;
        info.material_name = materials_[i].name;
        info.alpha = materials_[i].alpha();
        info.L = L_;
        info.tmax = tmax_;
        info.u0 = u0_kelvin;
        info.speed = speed_;
        info.paused = paused_;

        if (sim_type_ == SimType::BAR_1D && solvers_1d_[i]) {
            info.time = solvers_1d_[i]->get_time();
            auto temps = solvers_1d_[i]->get_temperature();
            if (!temps.empty()) {
                // Convert to ΔT
                std::vector<double> delta_temps(temps.size());
                for (size_t j = 0; j < temps.size(); j++) {
                    delta_temps[j] = temps[j] - u0_kelvin;
                }
                heatmap_->draw_1d_cell(delta_temps, info, cell_x[i], cell_y[i], cell_w, cell_h);
            }
        } else if (sim_type_ == SimType::PLATE_2D && solvers_2d_[i]) {
            info.time = solvers_2d_[i]->get_time();
            auto temps = solvers_2d_[i]->get_temperature_2d();
            if (!temps.empty() && !temps[0].empty()) {
                // Convert to ΔT
                std::vector<std::vector<double>> delta_temps(temps.size());
                for (size_t j = 0; j < temps.size(); j++) {
                    delta_temps[j].resize(temps[j].size());
                    for (size_t k = 0; k < temps[j].size(); k++) {
                        delta_temps[j][k] = temps[j][k] - u0_kelvin;
                    }
                }
                heatmap_->draw_2d_cell(delta_temps, info, cell_x[i], cell_y[i], cell_w, cell_h);
            }
        }
    }

    // Draw grid separators 
    SDL_Renderer* rend = window_->get_renderer();
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    // Vertical separator (3 pixels wide)
    for (int dx = -1; dx <= 1; dx++) {
        SDL_RenderDrawLine(rend, cell_w + dx, 0, cell_w + dx, win_h);
    }
    // Horizontal separator (3 pixels wide)
    for (int dy = -1; dy <= 1; dy++) {
        SDL_RenderDrawLine(rend, 0, cell_h + dy, win_w, cell_h + dy);
    }

    // Add corner markers
    SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
    int corner_size = 10;
    
    // Center cross highlight
    SDL_RenderDrawLine(rend, cell_w - corner_size, cell_h, cell_w + corner_size, cell_h);
    SDL_RenderDrawLine(rend, cell_w, cell_h - corner_size, cell_w, cell_h + corner_size);

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
                if (grid_mode_) {
                    for (int i = 0; i < 4; i++) {
                        if (solvers_1d_[i]) solvers_1d_[i]->reset();
                        if (solvers_2d_[i]) solvers_2d_[i]->reset();
                    }
                } else {
                    if (solver_1d_) solver_1d_->reset();
                    if (solver_2d_) solver_2d_->reset();
                }
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
            for (int s = 0; s < speed_; s++) {
                if (grid_mode_) {
                    bool all_done = true;
                    for (int i = 0; i < 4; i++) {
                        if (sim_type_ == SimType::BAR_1D && solvers_1d_[i]) {
                            if (solvers_1d_[i]->step()) all_done = false;
                        } else if (sim_type_ == SimType::PLATE_2D && solvers_2d_[i]) {
                            if (solvers_2d_[i]->step()) all_done = false;
                        }
                    }
                    if (all_done) {
                        paused_ = true;
                        break;
                    }
                } else {
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
        }

        if (grid_mode_) {
            render_grid();
        } else {
            render();
        }
        SDLCore::delay(16);
    }

    heatmap_.reset();
    window_.reset();
}

}
