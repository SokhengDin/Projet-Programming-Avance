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
    std::cout << "  1. 1D Bar\n";
    std::cout << "  2. 2D Plate\n";
    std::cout << "  3. 1D Bar  (All 4 Materials - 2x2 Grid)\n";
    std::cout << "  4. 2D Plate (All 4 Materials - 2x2 Grid)\n";
    std::cout << "  0. Quit\n";
    std::cout << "Choice: ";

    int choice;
    if (!(std::cin >> choice)) {
        clear_input();
        return -1;
    }
    return choice;
}

int select_material() {
    std::cout << "\nSELECT MATERIAL\n";
    std::cout << "---------------\n";
    std::cout << "  1. Copper      (lambda=389.0, rho=8940, c=380)\n";
    std::cout << "  2. Iron        (lambda=80.2,  rho=7874, c=440)\n";
    std::cout << "  3. Glass       (lambda=1.2,   rho=2530, c=840)\n";
    std::cout << "  4. Polystyrene (lambda=0.1,   rho=1040, c=1200)\n";
    std::cout << "  0. Back\n";
    std::cout << "Choice: ";

    int choice;
    if (!(std::cin >> choice)) {
        clear_input();
        return -1;
    }
    if (choice == 0) return -1;
    if (choice >= 1 && choice <= 4) return choice - 1;
    return -1;
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

bool confirm_and_start(int sim_type, int material_idx, double L, double tmax, double u0, double f) {
    const char* sim_names[] = {"1D Bar", "2D Plate"};
    const char* mat_names[] = {"Copper", "Iron", "Glass", "Polystyrene"};

    std::cout << "\nCONFIGURATION\n";
    std::cout << "-------------\n";
    std::cout << "  Type:     " << sim_names[sim_type - 1] << "\n";
    std::cout << "  Material: " << mat_names[material_idx] << "\n";
    std::cout << "  L=" << L << " m, tmax=" << tmax << " s\n";
    std::cout << "  u0=" << u0 << " C, f=" << f << " C\n\n";
    std::cout << "Controls: SPACE=pause, R=reset, UP/DOWN=speed, ESC=quit\n\n";
    std::cout << "[S]tart  [B]ack  [Q]uit: ";

    char choice;
    std::cin >> choice;
    return (std::tolower(choice) == 's');
}

bool confirm_and_start_grid(int sim_type, double L, double tmax, double u0, double f) {
    const char* sim_names[] = {"1D Bar", "2D Plate"};

    std::cout << "\nCONFIGURATION (2x2 Grid - All Materials)\n";
    std::cout << "----------------------------------------\n";
    std::cout << "  Type:      " << sim_names[sim_type == 3 ? 0 : 1] << "\n";
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
        if (sim_type < 1 || sim_type > 4) {
            std::cout << "\nInvalid choice.\n";
            continue;
        }

        double L = 1.0, tmax = 16.0, u0 = 13.0, f = 80.0;

        // Grid mode (options 3 and 4)
        if (sim_type == 3 || sim_type == 4) {
            if (!get_parameters(L, tmax, u0, f)) continue;
            if (!confirm_and_start_grid(sim_type, L, tmax, u0, f)) continue;

            std::cout << "\nStarting grid simulation...\n";

            try {
                sdl::SDLCore::init();

                sdl::SDLApp::SimType type = (sim_type == 3)
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
        // Single material mode (options 1 and 2)
        else {
            int material_idx = select_material();
            if (material_idx < 0) continue;

            if (!get_parameters(L, tmax, u0, f)) continue;
            if (!confirm_and_start(sim_type, material_idx, L, tmax, u0, f)) continue;

            ensiie::Material material;
            switch (material_idx) {
                case 0: material = ensiie::Materials::COPPER; break;
                case 1: material = ensiie::Materials::IRON; break;
                case 2: material = ensiie::Materials::GLASS; break;
                case 3: material = ensiie::Materials::POLYSTYRENE; break;
                default: material = ensiie::Materials::COPPER; break;
            }

            std::cout << "\nStarting simulation...\n";

            try {
                sdl::SDLCore::init();

                sdl::SDLApp::SimType type = (sim_type == 1)
                    ? sdl::SDLApp::SimType::BAR_1D
                    : sdl::SDLApp::SimType::PLATE_2D;

                sdl::SDLApp app(type, material, L, tmax, u0, f);
                app.run();

                sdl::SDLCore::quit();
                std::cout << "\nReturning to menu...\n";

            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                sdl::SDLCore::quit();
            }
        }
    }

    return 0;
}
