#include "sdl_app.hpp"
#include "sdl_core.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

#ifdef __APPLE__
    static const char* FONT_PATH = "/System/Library/Fonts/Helvetica.ttc";
#elif defined(_WIN32)
    static const char* FONT_PATH = "C:\\Windows\\Fonts\\arial.ttf";
#else
    static const char* FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
#endif

namespace sdl {

    SDLApp::SDLApp()
        : window_(std::make_unique<SDLWindow>("Heat Equation Simulator", 1400, 900, false))
        , heatmap_(std::make_unique<SDLHeatmap>(*window_, 280.0, 380.0))
        , title_font_(std::make_unique<SDLFont>(FONT_PATH, 32))
        , label_font_(std::make_unique<SDLFont>(FONT_PATH, 18))
        , button_font_(std::make_unique<SDLFont>(FONT_PATH, 22))
        , small_font_(std::make_unique<SDLFont>(FONT_PATH, 16))
        , solver_1d_(nullptr)
        , solver_2d_(nullptr)
        , mode_(Mode::MENU)
        , sim_type_(SimType::BAR_1D)
        , material_(ensiie::Materials::COPPER)
        , selected_sim_type_(0)
        , selected_material_(0)
        , dragging_slider_(-1)
        , L_(1.0)
        , tmax_(16.0)
        , u0_(13.0)
        , f_(80.0)
        , n_(1001)
        , paused_(false)
        , speed_(10)
        , running_(true)
        , panel_x_(0)
        , panel_y_(0)
        , panel_w_(0)
        , panel_h_(0) {
    }

    void SDLApp::draw_rect(
        int x
        , int y
        , int w
        , int h
        , Uint8 r
        , Uint8 g
        , Uint8 b
        , bool fill
    ) {
        SDL_Renderer* rend = window_->get_renderer();
        SDL_SetRenderDrawColor(rend, r, g, b, 255);
        SDL_Rect rect = {x, y, w, h};
        if (fill) {
            SDL_RenderFillRect(rend, &rect);
        } else {
            SDL_RenderDrawRect(rend, &rect);
        }
    }

    void SDLApp::draw_text_box(
        const std::string& text
        , int x
        , int y
        , int w
        , int h
        , bool selected
    ) {
        if (selected) {
            draw_rect(x, y, w, h, 100, 150, 200, true);
        } else {
            draw_rect(x, y, w, h, 60, 60, 60, true);
        }

        Uint8 border_c = selected ? 255 : 200;
        for (int i = 0; i < 2; i++) {
            draw_rect(x - i, y - i, w + 2*i, h + 2*i, border_c, border_c, border_c, false);
        }

        button_font_->render_centered(
            window_->get_renderer()
            , text
            , x
            , y
            , w
            , h
            , {255, 255, 255, 255}
        );
    }

    void SDLApp::draw_slider(
        const std::string& label
        , double value
        , double min_v
        , double max_v
        , int x
        , int y
        , int slider_w
    ) {
        SDL_Renderer* rend = window_->get_renderer();
        int slider_h = 25;

        small_font_->render(rend, label, x, y, {200, 200, 200, 255});

        int track_y = y + 20;
        draw_rect(x, track_y, slider_w, slider_h, 50, 50, 50, true);
        draw_rect(x, track_y, slider_w, slider_h, 100, 100, 100, false);

        double ratio    = (value - min_v) / (max_v - min_v);
        ratio           = std::max(0.0, std::min(1.0, ratio));
        int fill_w      = static_cast<int>(ratio * slider_w);
        draw_rect(x, track_y, fill_w, slider_h, 80, 140, 200, true);

        int knob_x = x + fill_w - 6;
        draw_rect(knob_x, track_y - 2, 12, slider_h + 4, 255, 255, 255, true);

        std::ostringstream oss;
        if (max_v > 100) {
            oss << static_cast<int>(value);
        } else {
            oss << std::fixed << std::setprecision(1) << value;
        }
        small_font_->render(rend, oss.str(), x + slider_w + 10, track_y + 3, {255, 255, 255, 255});
    }

    bool SDLApp::is_in_rect(
        int mx
        , int my
        , int x
        , int y
        , int w
        , int h
    ) {
        return mx >= x && mx < x + w && my >= y && my < y + h;
    }

    void SDLApp::handle_slider_drag(int mx, int slider_idx, int slider_x, int slider_w) {
        double ratio = static_cast<double>(mx - slider_x) / slider_w;
        ratio = std::max(0.0, std::min(1.0, ratio));

        switch (slider_idx) {
            case 0:
                L_ = 0.1 + ratio * 4.9;
                break;
            case 1:
                tmax_ = 1.0 + ratio * 59.0;
                break;
            case 2:
                u0_ = -20.0 + ratio * 70.0;
                break;
            case 3:
                f_ = 10.0 + ratio * 190.0;
                break;
            case 4:
                if (sim_type_ == SimType::BAR_1D) {
                    n_ = 101 + static_cast<int>(ratio * 1900);
                } else {
                    n_ = 51 + static_cast<int>(ratio * 150);
                }
                break;
        }
    }

    void SDLApp::start_simulation() {
        mode_ = Mode::SIMULATION;
        paused_ = false;

        if (sim_type_ == SimType::BAR_1D) {
            speed_ = 10;
            solver_1d_ = std::make_unique<ensiie::HeatEquationSolver1D>(
                material_
                , L_
                , tmax_
                , u0_
                , f_
                , n_
            );
            solver_2d_.reset();
        } else {
            speed_ = 5;
            solver_2d_ = std::make_unique<ensiie::HeatEquationSolver2D>(
                material_
                , L_
                , tmax_
                , u0_
                , f_
                , n_
            );
            solver_1d_.reset();
        }
    }

    void SDLApp::stop_simulation() {
        mode_ = Mode::MENU;
        solver_1d_.reset();
        solver_2d_.reset();
    }

    void SDLApp::render_menu() {
        window_->clear(30, 30, 40);
        SDL_Renderer* rend = window_->get_renderer();

        int w = window_->get_width();
        int h = window_->get_height();

        int panel_w = 700;
        int panel_x = (w - panel_w) / 2;
        int start_y = 30;

        draw_rect(panel_x, start_y, panel_w, 55, 70, 130, 180, true);
        draw_rect(panel_x, start_y, panel_w, 55, 120, 180, 230, false);
        title_font_->render_centered(rend, "HEAT EQUATION SIMULATOR", panel_x, start_y, panel_w, 55, {255, 255, 255, 255});

        int y = start_y + 75;
        label_font_->render(rend, "Simulation Type", panel_x + 10, y, {200, 200, 200, 255});
        y += 22;
        int box_w = (panel_w - 30) / 2;
        draw_text_box("1D Bar", panel_x, y, box_w, 45, selected_sim_type_ == 0);
        draw_text_box("2D Plate", panel_x + box_w + 30, y, box_w, 45, selected_sim_type_ == 1);

        y += 65;
        label_font_->render(rend, "Material", panel_x + 10, y, {200, 200, 200, 255});
        y += 22;
        const char* materials[] = {"Copper", "Iron", "Glass", "Polystyrene"};
        int mat_w = (panel_w - 30) / 2;
        for (int i = 0; i < 4; i++) {
            int bx = panel_x + (i % 2) * (mat_w + 30);
            int by = y + (i / 2) * 52;
            draw_text_box(materials[i], bx, by, mat_w, 45, selected_material_ == i);
        }

        y += 120;
        label_font_->render(rend, "Parameters", panel_x + 10, y, {180, 180, 180, 255});
        y += 28;

        int slider_w = panel_w - 80;
        draw_slider("Length L (m)", L_, 0.1, 5.0, panel_x, y, slider_w);
        draw_slider("Max Time tmax (s)", tmax_, 1.0, 60.0, panel_x, y + 55, slider_w);
        draw_slider("Initial Temp u0 (C)", u0_, -20.0, 50.0, panel_x, y + 110, slider_w);
        draw_slider("Heat Source f (C)", f_, 10.0, 200.0, panel_x, y + 165, slider_w);

        std::string grid_label = (sim_type_ == SimType::BAR_1D)
            ? "Grid Points n" : "Grid Points n (per axis)";
        draw_slider(grid_label, static_cast<double>(n_),
            (sim_type_ == SimType::BAR_1D) ? 101.0 : 51.0,
            (sim_type_ == SimType::BAR_1D) ? 2001.0 : 201.0,
            panel_x, y + 220, slider_w);

        y += 290;
        draw_rect(panel_x, y, panel_w, 55, 50, 50, 50, true);
        draw_rect(panel_x, y, panel_w, 55, 100, 100, 100, false);

        std::ostringstream info;
        info << "Material: " << material_.name
             << " | alpha = " << std::fixed << std::setprecision(6) << material_.alpha() << " m2/s";
        small_font_->render(rend, info.str(), panel_x + 10, y + 10, {180, 180, 180, 255});

        std::ostringstream info2;
        info2 << "lambda=" << material_.lambda << " W/(m.K)"
              << " | rho=" << material_.rho << " kg/m3"
              << " | c=" << material_.c << " J/(kg.K)";
        small_font_->render(rend, info2.str(), panel_x + 10, y + 32, {150, 150, 150, 255});

        y += 70;
        int btn_w = 280;
        int btn_x = (w - btn_w) / 2;
        draw_text_box("START SIMULATION", btn_x, y, btn_w, 50, false);

        small_font_->render(rend, "Press SPACE or ENTER to start | ESC to quit", panel_x + 120, h - 30, {120, 120, 120, 255});

        window_->present();
    }

    void SDLApp::render_sim_control_panel(int x, int y, int pw, int ph, double current_time) {
        SDL_Renderer* rend = window_->get_renderer();

        draw_rect(x, y, pw, ph, 35, 35, 45, true);
        draw_rect(x, y, pw, ph, 60, 60, 70, false);

        int px = x + 15;
        int py = y + 15;

        label_font_->render(rend, "Control Panel", px, py, {200, 200, 200, 255});
        py += 35;

        draw_rect(px, py, pw - 30, 2, 60, 60, 70, true);
        py += 15;

        if (paused_) {
            draw_rect(px, py, pw - 30, 30, 180, 80, 80, true);
            button_font_->render_centered(rend, "PAUSED", px, py, pw - 30, 30, {255, 255, 255, 255});
        } else {
            draw_rect(px, py, pw - 30, 30, 80, 180, 80, true);
            button_font_->render_centered(rend, "RUNNING", px, py, pw - 30, 30, {255, 255, 255, 255});
        }
        py += 45;

        small_font_->render(rend, "Time Progress", px, py, {180, 180, 180, 255});
        py += 20;

        std::ostringstream time_str;
        time_str << std::fixed << std::setprecision(2) << current_time << " / " << tmax_ << " s";
        label_font_->render(rend, time_str.str(), px, py, {255, 255, 255, 255});
        py += 25;

        double progress = current_time / tmax_;
        draw_rect(px, py, pw - 30, 10, 50, 50, 60, true);
        draw_rect(px, py, static_cast<int>(progress * (pw - 30)), 10, 80, 180, 80, true);
        draw_rect(px, py, pw - 30, 10, 80, 80, 90, false);
        py += 25;

        draw_rect(px, py, pw - 30, 2, 60, 60, 70, true);
        py += 15;

        small_font_->render(rend, "Speed", px, py, {180, 180, 180, 255});
        py += 20;

        std::ostringstream speed_str;
        speed_str << speed_ << "x";
        label_font_->render(rend, speed_str.str(), px, py, {255, 255, 255, 255});

        int btn_w = (pw - 40) / 2;
        draw_rect(px, py + 25, btn_w, 28, 60, 60, 80, true);
        draw_rect(px, py + 25, btn_w, 28, 80, 80, 100, false);
        button_font_->render_centered(rend, "-", px, py + 25, btn_w, 28, {255, 255, 255, 255});

        draw_rect(px + btn_w + 10, py + 25, btn_w, 28, 60, 60, 80, true);
        draw_rect(px + btn_w + 10, py + 25, btn_w, 28, 80, 80, 100, false);
        button_font_->render_centered(rend, "+", px + btn_w + 10, py + 25, btn_w, 28, {255, 255, 255, 255});
        py += 70;

        draw_rect(px, py, pw - 30, 2, 60, 60, 70, true);
        py += 15;

        small_font_->render(rend, "Parameters", px, py, {180, 180, 180, 255});
        py += 22;

        std::ostringstream p1;
        p1 << "L = " << std::fixed << std::setprecision(2) << L_ << " m";
        small_font_->render(rend, p1.str(), px, py, {150, 150, 150, 255});
        py += 18;

        std::ostringstream p2;
        p2 << "n = " << n_ << " points";
        small_font_->render(rend, p2.str(), px, py, {150, 150, 150, 255});
        py += 18;

        std::ostringstream p3;
        p3 << "u0 = " << std::fixed << std::setprecision(0) << u0_ << " C";
        small_font_->render(rend, p3.str(), px, py, {150, 150, 150, 255});
        py += 18;

        std::ostringstream p4;
        p4 << "f = " << std::fixed << std::setprecision(0) << f_ << " C";
        small_font_->render(rend, p4.str(), px, py, {150, 150, 150, 255});
        py += 30;

        draw_rect(px, py, pw - 30, 2, 60, 60, 70, true);
        py += 15;

        small_font_->render(rend, "Actions", px, py, {180, 180, 180, 255});
        py += 22;

        draw_rect(px, py, pw - 30, 35, paused_ ? 80 : 60, paused_ ? 140 : 60, paused_ ? 200 : 80, true);
        draw_rect(px, py, pw - 30, 35, 100, 100, 120, false);
        button_font_->render_centered(rend, paused_ ? "PLAY" : "PAUSE", px, py, pw - 30, 35, {255, 255, 255, 255});
        py += 45;

        draw_rect(px, py, pw - 30, 35, 70, 130, 180, true);
        draw_rect(px, py, pw - 30, 35, 100, 100, 120, false);
        button_font_->render_centered(rend, "RESET", px, py, pw - 30, 35, {255, 255, 255, 255});
        py += 45;

        draw_rect(px, py, pw - 30, 35, 180, 100, 80, true);
        draw_rect(px, py, pw - 30, 35, 100, 100, 120, false);
        button_font_->render_centered(rend, "MENU", px, py, pw - 30, 35, {255, 255, 255, 255});
    }

    void SDLApp::render_simulation() {
        window_->clear(25, 25, 30);
        SDL_Renderer* rend = window_->get_renderer();

        int w = window_->get_width();
        int h = window_->get_height();

        int vis_w = static_cast<int>(w * 0.70);
        panel_w_ = w - vis_w - 20;
        panel_x_ = vis_w + 10;
        panel_y_ = 10;
        panel_h_ = h - 20;

        double current_time = 0.0;

        draw_rect(5, 5, vis_w - 10, h - 10, 30, 30, 35, true);
        draw_rect(5, 5, vis_w - 10, h - 10, 50, 50, 60, false);

        std::string sim_title = (sim_type_ == SimType::BAR_1D)
            ? "1D Heat Equation - Bar"
            : "2D Heat Equation - Plate";
        title_font_->render(rend, sim_title, 20, 15, {255, 255, 255, 255});

        std::ostringstream mat_info;
        mat_info << "Material: " << material_.name
                 << " | alpha = " << std::fixed << std::setprecision(6) << material_.alpha() << " m2/s";
        small_font_->render(rend, mat_info.str(), 20, 50, {180, 180, 180, 255});

        if (sim_type_ == SimType::BAR_1D && solver_1d_) {
            current_time = solver_1d_->get_time();
            auto temps = solver_1d_->get_temperature();
            if (!temps.empty()) {
                heatmap_->auto_range(temps);

                int bar_x = 20;
                int bar_y = 90;
                int bar_w = vis_w - 100;
                int bar_h = h - 180;

                draw_rect(bar_x - 5, bar_y - 5, bar_w + 10, bar_h + 10, 35, 35, 40, true);

                heatmap_->draw_bar_1d(temps, bar_x, bar_y, bar_w, bar_h);
                heatmap_->draw_boundary_markers_1d(bar_x, bar_y, bar_w, bar_h);
                heatmap_->draw_heat_flow_1d(temps, bar_x, bar_y, bar_w, bar_h);
                heatmap_->draw_heat_sources_1d(bar_x, bar_y, bar_w, bar_h);

                small_font_->render(rend, "x = 0", bar_x, h - 70, {150, 150, 150, 255});
                std::ostringstream x_max;
                x_max << "x = " << std::fixed << std::setprecision(2) << L_ << " m";
                small_font_->render(rend, x_max.str(), bar_x + bar_w - 80, h - 70, {150, 150, 150, 255});

                heatmap_->draw_colorbar(vis_w - 60, bar_y, 20, bar_h);

                int stats_w = 360;
                int stats_x = bar_x + (bar_w - stats_w) / 2;
                heatmap_->draw_stats(temps, stats_x, 70);
            }

        } else if (sim_type_ == SimType::PLATE_2D && solver_2d_) {
            current_time = solver_2d_->get_time();
            auto temps = solver_2d_->get_temperature_2d();
            if (!temps.empty() && !temps[0].empty()) {
                heatmap_->auto_range_2d(temps);

                int plate_x = 20;
                int plate_y = 80;
                int plate_size = std::min(vis_w - 120, h - 160);

                draw_rect(plate_x - 5, plate_y - 5, plate_size + 10, plate_size + 10, 35, 35, 40, true);

                heatmap_->draw_plate_2d(temps, plate_x, plate_y, plate_size);
                heatmap_->draw_boundary_markers_2d(plate_x, plate_y, plate_size);
                heatmap_->draw_heat_flow_2d(temps, plate_x, plate_y, plate_size);
                heatmap_->draw_heat_sources_2d(plate_x, plate_y, plate_size);

                small_font_->render(rend, "(0,0)", plate_x, plate_y + plate_size + 10, {150, 150, 150, 255});
                std::ostringstream xy_max;
                xy_max << "(" << std::fixed << std::setprecision(1) << L_ << "," << L_ << ")";
                small_font_->render(rend, xy_max.str(), plate_x + plate_size - 50, plate_y - 18, {150, 150, 150, 255});

                heatmap_->draw_colorbar(vis_w - 60, plate_y, 20, plate_size);

                int stats_w = 360;
                int stats_x = plate_x + (plate_size - stats_w) / 2;
                heatmap_->draw_stats_2d(temps, stats_x, 60);
            }
        }

        render_sim_control_panel(panel_x_, panel_y_, panel_w_, panel_h_, current_time);

        window_->present();
    }

    void SDLApp::process_menu_events(SDL_Event& event) {
        int w = window_->get_width();
        int panel_w = 700;
        int panel_x = (w - panel_w) / 2;
        int slider_w = panel_w - 80;
        int start_y = 30;

        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mx = event.button.x;
            int my = event.button.y;

            int y = start_y + 75 + 22;
            int box_w = (panel_w - 30) / 2;

            if (is_in_rect(mx, my, panel_x, y, box_w, 45)) {
                selected_sim_type_ = 0;
                sim_type_ = SimType::BAR_1D;
                n_ = 1001;
            } else if (is_in_rect(mx, my, panel_x + box_w + 30, y, box_w, 45)) {
                selected_sim_type_ = 1;
                sim_type_ = SimType::PLATE_2D;
                n_ = 101;
            }

            y += 65 + 22;
            int mat_w = (panel_w - 30) / 2;
            const ensiie::Material mats[] = {
                ensiie::Materials::COPPER,
                ensiie::Materials::IRON,
                ensiie::Materials::GLASS,
                ensiie::Materials::POLYSTYRENE
            };

            for (int i = 0; i < 4; i++) {
                int bx = panel_x + (i % 2) * (mat_w + 30);
                int by = y + (i / 2) * 52;
                if (is_in_rect(mx, my, bx, by, mat_w, 45)) {
                    selected_material_ = i;
                    material_ = mats[i];
                }
            }

            y += 120 + 28;
            for (int i = 0; i < 5; i++) {
                int sy = y + i * 55 + 20;
                if (is_in_rect(mx, my, panel_x - 10, sy - 5, slider_w + 20, 35)) {
                    dragging_slider_ = i;
                    handle_slider_drag(mx, i, panel_x, slider_w);
                }
            }

            y += 290 + 55 + 15;
            int btn_w = 280;
            int btn_x = (w - btn_w) / 2;
            if (is_in_rect(mx, my, btn_x, y, btn_w, 50)) {
                start_simulation();
            }
        }

        if (event.type == SDL_MOUSEBUTTONUP) {
            dragging_slider_ = -1;
        }

        if (event.type == SDL_MOUSEMOTION && dragging_slider_ >= 0) {
            handle_slider_drag(event.motion.x, dragging_slider_, panel_x, slider_w);
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_1:
                    selected_sim_type_ = 0;
                    sim_type_ = SimType::BAR_1D;
                    n_ = 1001;
                    break;
                case SDLK_2:
                    selected_sim_type_ = 1;
                    sim_type_ = SimType::PLATE_2D;
                    n_ = 101;
                    break;
                case SDLK_RETURN:
                case SDLK_SPACE:
                    start_simulation();
                    break;
                case SDLK_ESCAPE:
                    running_ = false;
                    break;
            }
        }
    }

    void SDLApp::process_simulation_events(SDL_Event& event) {
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mx = event.button.x;
            int my = event.button.y;

            int px = panel_x_ + 15;
            int btn_w = panel_w_ - 30;
            int half_btn = (panel_w_ - 40) / 2;

            int speed_btn_y = panel_y_ + 240;
            if (is_in_rect(mx, my, px, speed_btn_y, half_btn, 28)) {
                speed_ = std::max(1, speed_ - (sim_type_ == SimType::BAR_1D ? 5 : 2));
            }
            if (is_in_rect(mx, my, px + half_btn + 10, speed_btn_y, half_btn, 28)) {
                if (sim_type_ == SimType::BAR_1D) {
                    speed_ = std::min(50, speed_ + 5);
                } else {
                    speed_ = std::min(20, speed_ + 2);
                }
            }

            int play_pause_y = panel_y_ + 443;
            int reset_y = panel_y_ + 488;
            int menu_y = panel_y_ + 533;

            if (is_in_rect(mx, my, px, play_pause_y, btn_w, 35)) {
                paused_ = !paused_;
            }
            if (is_in_rect(mx, my, px, reset_y, btn_w, 35)) {
                if (solver_1d_) solver_1d_->reset();
                if (solver_2d_) solver_2d_->reset();
                paused_ = false;
            }
            if (is_in_rect(mx, my, px, menu_y, btn_w, 35)) {
                stop_simulation();
            }
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running_ = false;
                    break;
                case SDLK_m:
                    stop_simulation();
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
                    if (sim_type_ == SimType::BAR_1D) {
                        speed_ = std::min(50, speed_ + 5);
                    } else {
                        speed_ = std::min(20, speed_ + 2);
                    }
                    break;
                case SDLK_DOWN:
                    speed_ = std::max(1, speed_ - (sim_type_ == SimType::BAR_1D ? 5 : 2));
                    break;
                case SDLK_PLUS:
                case SDLK_EQUALS:
                case SDLK_KP_PLUS:
                    tmax_ = std::min(120.0, tmax_ + 5.0);
                    break;
                case SDLK_MINUS:
                case SDLK_KP_MINUS:
                    tmax_ = std::max(5.0, tmax_ - 5.0);
                    break;
            }
        }
    }

    void SDLApp::run() {
        while (running_) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running_ = false;
                }
                if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        running_ = false;
                    }
                }

                if (mode_ == Mode::MENU) {
                    process_menu_events(event);
                } else {
                    process_simulation_events(event);
                }
            }

            if (!running_) break;

            if (mode_ == Mode::SIMULATION && !paused_) {
                for (int i = 0; i < speed_; i++) {
                    if (sim_type_ == SimType::BAR_1D && solver_1d_) {
                        if (solver_1d_->get_time() >= tmax_ || !solver_1d_->step()) {
                            paused_ = true;
                            break;
                        }
                    } else if (sim_type_ == SimType::PLATE_2D && solver_2d_) {
                        if (solver_2d_->get_time() >= tmax_ || !solver_2d_->step()) {
                            paused_ = true;
                            break;
                        }
                    }
                }
            }

            if (mode_ == Mode::MENU) {
                render_menu();
            } else {
                render_simulation();
            }

            SDLCore::delay(16);
        }
    }

}
