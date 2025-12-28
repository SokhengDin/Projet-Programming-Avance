#ifndef SDL_APP_HPP
#define SDL_APP_HPP

#include "SDL.h"
#include "sdl_window.hpp"
#include "sdl_font.hpp"
#include "sdl_heatmap.hpp"
#include "material.hpp"
#include "heat_equation_solver_1d.hpp"
#include "heat_equation_solver_2d.hpp"
#include <string>
#include <memory>

namespace sdl {

    /**
     * @class SDLApp
     * @brief Single fullscreen application with menu and simulation integrated
     */
    class SDLApp {
        public:
            enum class Mode { MENU, SIMULATION };
            enum class SimType { BAR_1D, PLATE_2D };

        private:
            std::unique_ptr<SDLWindow> window_;
            std::unique_ptr<SDLHeatmap> heatmap_;
            std::unique_ptr<SDLFont> title_font_;
            std::unique_ptr<SDLFont> label_font_;
            std::unique_ptr<SDLFont> button_font_;
            std::unique_ptr<SDLFont> small_font_;

            std::unique_ptr<ensiie::HeatEquationSolver1D> solver_1d_;
            std::unique_ptr<ensiie::HeatEquationSolver2D> solver_2d_;

            Mode mode_;
            SimType sim_type_;
            ensiie::Material material_;
            int selected_sim_type_;
            int selected_material_;
            int dragging_slider_;

            double L_;
            double tmax_;
            double u0_;
            double f_;
            int n_;

            bool paused_;
            int speed_;
            bool running_;

            void draw_rect(
                int x
                , int y
                , int w
                , int h
                , Uint8 r
                , Uint8 g
                , Uint8 b
                , bool fill = false
            );

            void draw_text_box(
                const std::string& text
                , int x
                , int y
                , int w
                , int h
                , bool selected
            );

            void draw_slider(
                const std::string& label
                , double value
                , double min_v
                , double max_v
                , int x
                , int y
                , int slider_w
            );

            bool is_in_rect(
                int mx
                , int my
                , int x
                , int y
                , int w
                , int h
            );

            void handle_slider_drag(int mx, int slider_idx, int slider_x, int slider_w);

            void render_menu();
            void render_simulation();
            void render_sim_control_panel(int x, int y, int w, int h, double current_time);

            void process_menu_events(SDL_Event& event);
            void process_simulation_events(SDL_Event& event);

            int panel_x_;
            int panel_y_;
            int panel_w_;
            int panel_h_;

            void start_simulation();
            void stop_simulation();

        public:
            SDLApp();

            /**
             * @brief Main application loop
             */
            void run();
    };

}

#endif
