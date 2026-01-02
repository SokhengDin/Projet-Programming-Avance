/**
 * @file main.cpp
 * @brief Console menu and SDL2 visualization launcher
 */

#include "sdl_core.hpp"
#include "sdl_app.hpp"
#include "material.hpp"

#include <iostream>
#include <string>
#include <limits>
#include <iomanip>

void clear_input() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void print_header() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "   HEAT EQUATION SIMULATOR\n";
    std::cout << "   ENSIIE - Master 1\n";
    std::cout << "========================================\n\n";
}

int select_simulation_type() {
    std::cout << "SELECT SIMULATION TYPE\n";
    std::cout << "----------------------\n";
    std::cout << "  1. 1D Bar  (All 4 Materials - 2x2 Grid)\n";
    std::cout << "  2. 2D Plate (All 4 Materials - 2x2 Grid)\n";
    std::cout << "  0. Quit\n";
    std::cout << "Choice: ";

    int choice;
    if (!(std::cin >> choice)) {
        clear_input();
        return -1;
    }
    return choice;
}

bool get_parameters(double& L, double& tmax, double& u0, double& f) {
    std::cout << "\nPARAMETERS (Enter for default, 'b' to go back)\n";
    std::cout << "----------------------------------------------\n";

    std::string input;
    clear_input();

    std::cout << "Domain length L [1.0] m: ";
    std::getline(std::cin, input);
    if (input == "b" || input == "B") return false;
    if (!input.empty()) {
        try { L = std::stod(input); } catch (...) { L = 1.0; }
    }

    std::cout << "Max time tmax [16.0] s: ";
    std::getline(std::cin, input);
    if (input == "b" || input == "B") return false;
    if (!input.empty()) {
        try { tmax = std::stod(input); } catch (...) { tmax = 16.0; }
    }

    std::cout << "Initial temp u0 [13.0] C: ";
    std::getline(std::cin, input);
    if (input == "b" || input == "B") return false;
    if (!input.empty()) {
        try { u0 = std::stod(input); } catch (...) { u0 = 13.0; }
    }

    std::cout << "Source amplitude f [80.0] C: ";
    std::getline(std::cin, input);
    if (input == "b" || input == "B") return false;
    if (!input.empty()) {
        try { f = std::stod(input); } catch (...) { f = 80.0; }
    }

    return true;
}

bool confirm_and_start_grid(int sim_type, double L, double tmax, double u0, double f) {
    const char* sim_names[] = {"1D Bar", "2D Plate"};

    std::cout << "\nCONFIGURATION (2x2 Grid - All Materials)\n";
    std::cout << "----------------------------------------\n";
    std::cout << "  Type:      " << sim_names[sim_type - 1] << "\n";
    std::cout << "  Materials: Copper, Iron, Glass, Polystyrene\n";
    std::cout << "  L=" << L << " m, tmax=" << tmax << " s\n";
    std::cout << "  u0=" << u0 << " C, f=" << f << " C\n\n";
    std::cout << "Controls: SPACE=pause, R=reset, UP/DOWN=speed, ESC=quit\n\n";
    std::cout << "[S]tart  [B]ack  [Q]uit: ";

    char choice;
    std::cin >> choice;
    return (std::tolower(choice) == 's');
}

int main() {
    bool running = true;

    while (running) {
        print_header();

        int sim_type = select_simulation_type();
        if (sim_type == 0) {
            std::cout << "\nExit.\n";
            break;
        }
        if (sim_type < 1 || sim_type > 2) {
            std::cout << "\nInvalid choice.\n";
            continue;
        }

        double L = 1.0, tmax = 16.0, u0 = 13.0, f = 80.0;

        // Grid mode (all 4 materials)
        if (!get_parameters(L, tmax, u0, f)) continue;
        if (!confirm_and_start_grid(sim_type, L, tmax, u0, f)) continue;

        std::cout << "\nStarting grid simulation...\n";

        try {
            sdl::SDLCore::init();

            sdl::SDLApp::SimType type = (sim_type == 1)
                ? sdl::SDLApp::SimType::BAR_1D
                : sdl::SDLApp::SimType::PLATE_2D;

            sdl::SDLApp app(type, L, tmax, u0, f);  // Grid mode constructor
            app.run();

            sdl::SDLCore::quit();
            std::cout << "\nReturning to menu...\n";

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            sdl::SDLCore::quit();
        }
    }

    return 0;
}
