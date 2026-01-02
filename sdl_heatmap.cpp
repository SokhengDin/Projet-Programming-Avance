/**
 * @file sdl_heatmap.cpp
 * @brief Temperature visualization implementation
 */

#include "sdl_heatmap.hpp"
#include <algorithm>
#include <cmath>

namespace sdl {

// Helper: 7-segment digit rendering
static void draw_digit(SDL_Renderer* rend, int x, int y, int digit) {
    static const bool segments[10][7] = {
        {1,1,1,1,1,1,0}, // 0
        {0,1,1,0,0,0,0}, // 1
        {1,1,0,1,1,0,1}, // 2
        {1,1,1,1,0,0,1}, // 3
        {0,1,1,0,0,1,1}, // 4
        {1,0,1,1,0,1,1}, // 5
        {1,0,1,1,1,1,1}, // 6
        {1,1,1,0,0,0,0}, // 7
        {1,1,1,1,1,1,1}, // 8
        {1,1,1,1,0,1,1}  // 9
    };

    const int w = 4;
    const int h = 5;

    if (segments[digit][0]) SDL_RenderDrawLine(rend, x, y, x+w, y);
    if (segments[digit][1]) SDL_RenderDrawLine(rend, x+w, y, x+w, y+h);
    if (segments[digit][2]) SDL_RenderDrawLine(rend, x+w, y+h, x+w, y+2*h);
    if (segments[digit][3]) SDL_RenderDrawLine(rend, x, y+2*h, x+w, y+2*h);
    if (segments[digit][4]) SDL_RenderDrawLine(rend, x, y+h, x, y+2*h);
    if (segments[digit][5]) SDL_RenderDrawLine(rend, x, y, x, y+h);
    if (segments[digit][6]) SDL_RenderDrawLine(rend, x, y+h, x+w, y+h);
}

// Helper: Letter rendering upper case
static void draw_letter(SDL_Renderer* rend, int x, int y, char c) {
    const int w = 4;
    const int h = 10;
    switch (c) {
        case 'A':
            SDL_RenderDrawLine(rend, x, y+h, x+w/2, y);
            SDL_RenderDrawLine(rend, x+w/2, y, x+w, y+h);
            SDL_RenderDrawLine(rend, x+1, y+h/2, x+w-1, y+h/2);
            break;
        case 'C':
            SDL_RenderDrawLine(rend, x+w, y, x, y);
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x, y+h, x+w, y+h);
            break;
        case 'D':
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x, y, x+w-1, y+2);
            SDL_RenderDrawLine(rend, x+w-1, y+2, x+w-1, y+h-2);
            SDL_RenderDrawLine(rend, x+w-1, y+h-2, x, y+h);
            break;
        case 'E':
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x, y, x+w, y);
            SDL_RenderDrawLine(rend, x, y+h/2, x+w-1, y+h/2);
            SDL_RenderDrawLine(rend, x, y+h, x+w, y+h);
            break;
        case 'F':
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x, y, x+w, y);
            SDL_RenderDrawLine(rend, x, y+h/2, x+w-1, y+h/2);
            break;
        case 'G':
            SDL_RenderDrawLine(rend, x+w, y+1, x+1, y);
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x, y+h, x+w, y+h);
            SDL_RenderDrawLine(rend, x+w, y+h, x+w, y+h/2);
            SDL_RenderDrawLine(rend, x+w, y+h/2, x+w/2, y+h/2);
            break;
        case 'I':
            SDL_RenderDrawLine(rend, x+w/2, y, x+w/2, y+h);
            SDL_RenderDrawLine(rend, x, y, x+w, y);
            SDL_RenderDrawLine(rend, x, y+h, x+w, y+h);
            break;
        case 'K':
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x+w, y, x, y+h/2);
            SDL_RenderDrawLine(rend, x, y+h/2, x+w, y+h);
            break;
        case 'L':
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x, y+h, x+w, y+h);
            break;
        case 'M':
            SDL_RenderDrawLine(rend, x, y+h, x, y);
            SDL_RenderDrawLine(rend, x, y, x+w/2, y+h/3);
            SDL_RenderDrawLine(rend, x+w/2, y+h/3, x+w, y);
            SDL_RenderDrawLine(rend, x+w, y, x+w, y+h);
            break;
        case 'N':
            SDL_RenderDrawLine(rend, x, y+h, x, y);
            SDL_RenderDrawLine(rend, x, y, x+w, y+h);
            SDL_RenderDrawLine(rend, x+w, y+h, x+w, y);
            break;
        case 'O':
            SDL_RenderDrawLine(rend, x, y, x+w, y);
            SDL_RenderDrawLine(rend, x+w, y, x+w, y+h);
            SDL_RenderDrawLine(rend, x+w, y+h, x, y+h);
            SDL_RenderDrawLine(rend, x, y+h, x, y);
            break;
        case 'P':
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x, y, x+w, y);
            SDL_RenderDrawLine(rend, x+w, y, x+w, y+h/2);
            SDL_RenderDrawLine(rend, x+w, y+h/2, x, y+h/2);
            break;
        case 'R':
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x, y, x+w, y);
            SDL_RenderDrawLine(rend, x+w, y, x+w, y+h/2);
            SDL_RenderDrawLine(rend, x+w, y+h/2, x, y+h/2);
            SDL_RenderDrawLine(rend, x+w/2, y+h/2, x+w, y+h);
            break;
        case 'S':
            SDL_RenderDrawLine(rend, x+w, y, x, y);
            SDL_RenderDrawLine(rend, x, y, x, y+h/2);
            SDL_RenderDrawLine(rend, x, y+h/2, x+w, y+h/2);
            SDL_RenderDrawLine(rend, x+w, y+h/2, x+w, y+h);
            SDL_RenderDrawLine(rend, x+w, y+h, x, y+h);
            break;
        case 'T':
            SDL_RenderDrawLine(rend, x, y, x+w, y);
            SDL_RenderDrawLine(rend, x+w/2, y, x+w/2, y+h);
            break;
        case 'U':
            SDL_RenderDrawLine(rend, x, y, x, y+h);
            SDL_RenderDrawLine(rend, x, y+h, x+w, y+h);
            SDL_RenderDrawLine(rend, x+w, y+h, x+w, y);
            break;
        case 'V':
            SDL_RenderDrawLine(rend, x, y, x+w/2, y+h);
            SDL_RenderDrawLine(rend, x+w/2, y+h, x+w, y);
            break;
        case 'X':
            SDL_RenderDrawLine(rend, x, y, x+w, y+h);
            SDL_RenderDrawLine(rend, x+w, y, x, y+h);
            break;
        case 'Y':
            SDL_RenderDrawLine(rend, x, y, x+w/2, y+h/2);
            SDL_RenderDrawLine(rend, x+w, y, x+w/2, y+h/2);
            SDL_RenderDrawLine(rend, x+w/2, y+h/2, x+w/2, y+h);
            break;
        default:
            // Unknown letter - draw rectangle
            SDL_Rect r = {x, y, w, h};
            SDL_RenderDrawRect(rend, &r);
            break;
    }
}

SDLHeatmap::SDLHeatmap(SDLWindow& win, double t_min, double t_max)
    : win_(win), t_min_(t_min), t_max_(t_max) {}

void SDLHeatmap::set_range(double t_min, double t_max) {
    t_min_ = t_min;
    t_max_ = t_max;
}

void SDLHeatmap::auto_range(const std::vector<double>& temps) {
    if (temps.empty()) return;
    auto minmax     = std::minmax_element(temps.begin(), temps.end());
    double margin   = (*minmax.second - *minmax.first) * 0.05;
    t_min_ = *minmax.first - margin;
    t_max_ = *minmax.second + margin;
    if (t_max_ - t_min_ < 1.0) {
        t_min_ -= 0.5;
        t_max_ += 0.5;
    }
}

void SDLHeatmap::auto_range_2d(const std::vector<std::vector<double>>& temps) {
    if (temps.empty()) return;
    double min_v = temps[0][0];
    double max_v = temps[0][0];
    for (const auto& row : temps) {
        for (double v : row) {
            min_v = std::min(min_v, v);
            max_v = std::max(max_v, v);
        }
    }
    double margin = (max_v - min_v) * 0.05;
    t_min_ = min_v - margin;
    t_max_ = max_v + margin;
    if (t_max_ - t_min_ < 1.0) {
        t_min_ -= 0.5;
        t_max_ += 0.5;
    }
}

// Inferno colormap (matplotlib)
static const int INFERNO_SIZE = 256;
static const unsigned char INFERNO_MAP[256][3] = {
    {0,0,4},{1,0,5},{1,1,6},{1,1,8},{2,1,10},{2,2,12},{2,2,14},{3,2,16},
    {4,3,18},{4,3,20},{5,4,23},{6,4,25},{7,5,27},{8,5,29},{9,6,32},{10,6,34},
    {11,7,36},{12,7,39},{13,8,41},{14,8,43},{16,9,46},{17,9,48},{18,10,51},{20,10,53},
    {21,11,56},{22,11,58},{24,12,61},{25,12,63},{27,12,66},{28,13,68},{30,13,71},{31,13,73},
    {33,13,76},{35,14,78},{36,14,81},{38,14,83},{40,14,86},{41,14,88},{43,14,91},{45,14,93},
    {47,14,95},{48,14,98},{50,14,100},{52,14,102},{54,14,105},{56,14,107},{57,14,109},{59,14,111},
    {61,13,113},{63,13,115},{65,13,117},{67,13,119},{69,13,121},{70,13,123},{72,13,125},{74,12,127},
    {76,12,128},{78,12,130},{80,12,132},{82,11,133},{84,11,135},{86,11,136},{88,10,138},{90,10,139},
    {92,10,140},{94,10,142},{96,9,143},{98,9,144},{100,9,145},{102,9,146},{104,9,147},{106,8,148},
    {108,8,149},{110,8,150},{112,8,151},{114,8,152},{116,8,152},{118,8,153},{120,8,154},{122,8,154},
    {124,8,155},{126,8,155},{128,8,156},{130,8,156},{132,8,156},{134,9,157},{136,9,157},{138,9,157},
    {140,10,157},{142,10,157},{144,10,157},{146,11,157},{148,11,157},{150,12,157},{152,12,157},{154,13,157},
    {156,14,157},{158,14,156},{160,15,156},{162,16,156},{164,17,155},{166,17,155},{168,18,154},{170,19,154},
    {172,20,153},{174,21,152},{176,22,152},{178,23,151},{180,24,150},{182,25,149},{184,27,148},{186,28,147},
    {188,29,146},{190,30,145},{192,32,144},{193,33,143},{195,35,142},{197,36,141},{199,38,139},{200,39,138},
    {202,41,137},{204,43,135},{206,44,134},{207,46,133},{209,48,131},{210,50,130},{212,52,128},{214,54,127},
    {215,56,125},{217,58,124},{218,60,122},{220,62,121},{221,64,119},{223,66,117},{224,68,116},{226,71,114},
    {227,73,112},{228,75,111},{230,77,109},{231,79,107},{232,82,105},{234,84,104},{235,86,102},{236,89,100},
    {237,91,98},{238,93,97},{239,96,95},{240,98,93},{241,100,91},{242,103,89},{243,105,88},{244,108,86},
    {245,110,84},{246,113,82},{246,115,80},{247,118,79},{248,120,77},{249,123,75},{249,125,73},{250,128,71},
    {250,130,70},{251,133,68},{252,135,66},{252,138,64},{253,141,62},{253,143,60},{254,146,59},{254,148,57},
    {254,151,55},{255,153,53},{255,156,51},{255,159,50},{255,161,48},{255,164,46},{255,166,45},{255,169,43},
    {255,172,41},{255,174,40},{255,177,38},{255,180,37},{255,182,35},{255,185,34},{255,188,32},{255,190,31},
    {255,193,30},{255,196,29},{255,199,27},{255,201,26},{255,204,25},{255,207,24},{255,210,24},{255,212,23},
    {255,215,22},{255,218,22},{255,221,21},{255,223,21},{255,226,21},{255,229,21},{255,231,21},{255,234,21},
    {255,237,22},{255,239,22},{255,242,23},{255,244,24},{255,247,25},{254,249,27},{254,252,28},{252,254,30},
    {251,255,31},{249,255,33},{248,255,35},{246,255,37},{244,255,39},{243,255,41},{241,255,43},{239,255,46},
    {238,255,48},{236,255,50},{234,255,53},{232,255,55},{231,255,58},{229,255,60},{227,255,63},{225,255,66},
    {223,255,68},{222,255,71},{220,255,74},{218,255,77},{216,255,80},{214,255,83},{212,255,86},{210,255,89},
    {208,255,92},{206,255,95},{204,255,98},{202,255,101},{200,255,104},{198,255,107},{196,255,111},{194,255,114},
    {192,255,117},{190,255,120},{188,255,124},{186,255,127},{184,255,130},{182,255,134},{180,255,137},{178,255,141},
    {175,255,144},{173,255,148},{171,255,151},{169,255,155},{167,255,159},{165,255,162},{163,255,166},{252,255,164}
};

void SDLHeatmap::temp_to_rgb(double t, Uint8& r, Uint8& g, Uint8& b) const {
    double norm = (t - t_min_) / (t_max_ - t_min_);
    norm = std::max(0.0, std::min(1.0, norm));

    double idx = norm * (INFERNO_SIZE - 1);
    int i0 = static_cast<int>(idx);
    int i1 = std::min(i0 + 1, INFERNO_SIZE - 1);
    double frac = idx - i0;

    r = static_cast<Uint8>(INFERNO_MAP[i0][0] * (1 - frac) + INFERNO_MAP[i1][0] * frac);
    g = static_cast<Uint8>(INFERNO_MAP[i0][1] * (1 - frac) + INFERNO_MAP[i1][1] * frac);
    b = static_cast<Uint8>(INFERNO_MAP[i0][2] * (1 - frac) + INFERNO_MAP[i1][2] * frac);
}

// Number rendering display
void SDLHeatmap::draw_number(SDL_Renderer* rend, int x, int y, double value) const {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.1f", value);

    int offset = 0;
    for (int i = 0; buffer[i] != '\0'; ++i) {
        char c = buffer[i];
        if (c == '.') {
            SDL_Rect dot = {x + offset, y + 8, 2, 2};
            SDL_RenderFillRect(rend, &dot);
            offset += 3;
        } else if (c >= '0' && c <= '9') {
            int digit = c - '0';
            draw_digit(rend, x + offset, y, digit);
            offset += 7;
        } else if (c == '-') {
            SDL_RenderDrawLine(rend, x + offset, y + 5, x + offset + 4, y + 5);
            offset += 6;
        }
    }
}

// Text rendering using 7-segment digits
void SDLHeatmap::draw_text(SDL_Renderer* rend, int x, int y, const char* text) const {
    int offset = 0;
    for (int i = 0; text[i] != '\0'; ++i) {
        char c = text[i];
        if (c == ' ') {
            offset += 5;
        } else if (c == '.') {
            SDL_Rect dot = {x + offset, y + 8, 2, 2};
            SDL_RenderFillRect(rend, &dot);
            offset += 3;
        } else if (c == ':') {
            SDL_Rect dot1 = {x + offset + 1, y + 3, 2, 2};
            SDL_Rect dot2 = {x + offset + 1, y + 7, 2, 2};
            SDL_RenderFillRect(rend, &dot1);
            SDL_RenderFillRect(rend, &dot2);
            offset += 5;
        } else if (c == '=') {
            SDL_RenderDrawLine(rend, x + offset, y + 3, x + offset + 4, y + 3);
            SDL_RenderDrawLine(rend, x + offset, y + 7, x + offset + 4, y + 7);
            offset += 6;
        } else if (c == '/') {
            SDL_RenderDrawLine(rend, x + offset + 4, y, x + offset, y + 10);
            offset += 6;
        } else if (c == '[') {
            SDL_RenderDrawLine(rend, x + offset, y, x + offset, y + 10);
            SDL_RenderDrawLine(rend, x + offset, y, x + offset + 2, y);
            SDL_RenderDrawLine(rend, x + offset, y + 10, x + offset + 2, y + 10);
            offset += 4;
        } else if (c == ']') {
            SDL_RenderDrawLine(rend, x + offset + 2, y, x + offset + 2, y + 10);
            SDL_RenderDrawLine(rend, x + offset, y, x + offset + 2, y);
            SDL_RenderDrawLine(rend, x + offset, y + 10, x + offset + 2, y + 10);
            offset += 4;
        } else if (c >= '0' && c <= '9') {
            draw_digit(rend, x + offset, y, c - '0');
            offset += 7;
        } else if (c >= 'A' && c <= 'Z') {
            draw_letter(rend, x + offset, y, c);
            offset += 6;
        } else if (c >= 'a' && c <= 'z') {
            draw_letter(rend, x + offset, y, c - 32); // Convert to uppercase
            offset += 6;
        }
    }
}

// Colorbar: vertical gradient showing ΔT scale
void SDLHeatmap::draw_colorbar(SDL_Renderer* rend, int x, int y, int w, int h) const {
    // Draw gradient
    for (int i = 0; i < h; ++i) {
        double t = t_max_ - (i * (t_max_ - t_min_)) / h;
        Uint8 r, g, b;
        temp_to_rgb(t, r, g, b);
        SDL_SetRenderDrawColor(rend, r, g, b, 255);
        SDL_RenderDrawLine(rend, x, y + i, x + w, y + i);
    }

    // Border
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_Rect border = {x - 1, y - 1, w + 2, h + 2};
    SDL_RenderDrawRect(rend, &border);

    // ΔT labels
    int num_labels = 5;
    for (int i = 0; i <= num_labels; ++i) {
        int ly = y + (i * h) / num_labels;
        double temp = t_max_ - (i * (t_max_ - t_min_)) / num_labels;
        SDL_RenderDrawLine(rend, x + w, ly, x + w + 3, ly);
        draw_number(rend, x + w + 5, ly - 5, temp);
    }

    // Label: ΔT [K]
    SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
    draw_text(rend, x - 5, y - 15, "DT [K]");
}

// Info panel: material, time, status, speed
void SDLHeatmap::draw_info_panel(SDL_Renderer* rend, const SimInfo& info) const {
    int x = 10;
    int y = 5;

    SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);

    // Material name
    draw_text(rend, x, y, info.material_name.c_str());

    // Alpha value
    char alpha_buf[64];
    snprintf(alpha_buf, sizeof(alpha_buf), "a=%.2e", info.alpha);
    draw_text(rend, x + 100, y, alpha_buf);

    // Time and progress
    char time_buf[64];
    snprintf(time_buf, sizeof(time_buf), "t=%.2f/%.1f s", info.time, info.tmax);
    draw_text(rend, x + 220, y, time_buf);

    // Progress bar
    int bar_x = x + 380;
    int bar_w = 80;
    int bar_h = 10;
    double progress = info.time / info.tmax;

    SDL_SetRenderDrawColor(rend, 80, 80, 80, 255);
    SDL_Rect bar_bg = {bar_x, y + 2, bar_w, bar_h};
    SDL_RenderFillRect(rend, &bar_bg);
    SDL_SetRenderDrawColor(rend, 100, 200, 100, 255);
    SDL_Rect bar_fg = {bar_x, y + 2, static_cast<int>(bar_w * progress), bar_h};
    SDL_RenderFillRect(rend, &bar_fg);
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderDrawRect(rend, &bar_bg);

    // Speed indicator
    char speed_buf[16];
    snprintf(speed_buf, sizeof(speed_buf), "X%d", info.speed);
    SDL_SetRenderDrawColor(rend, 150, 200, 255, 255);
    draw_text(rend, bar_x + bar_w + 10, y, speed_buf);

    // Paused indicator
    if (info.paused) {
        SDL_SetRenderDrawColor(rend, 255, 200, 50, 255);
        draw_text(rend, x + 540, y, "PAUSED");
    }
}

// Grid lines
void SDLHeatmap::draw_grid(SDL_Renderer* rend, int x0, int y0, int w, int h, int nx, int ny) const {
    SDL_SetRenderDrawColor(rend, 100, 100, 100, 128);

    // Vertical lines
    for (int i = 1; i < nx; ++i) {
        int x = x0 + (i * w) / nx;
        for (int y = y0; y < y0 + h; y += 4) {
            SDL_RenderDrawPoint(rend, x, y);
        }
    }

    // Horizontal lines
    for (int j = 1; j < ny; ++j) {
        int y = y0 + (j * h) / ny;
        for (int x = x0; x < x0 + w; x += 4) {
            SDL_RenderDrawPoint(rend, x, y);
        }
    }
}

void SDLHeatmap::draw_1d_fullscreen(const std::vector<double>& temps, const SimInfo& info) {
    if (temps.empty()) return;

    SDL_Renderer* rend = win_.get_renderer();
    int win_w = win_.get_width();
    int win_h = win_.get_height();
    int n = static_cast<int>(temps.size());

    // Margins for axes and info panel
    int margin_left     = 60;
    int margin_right    = 80;   
    int margin_top      = 25;    
    int margin_bottom   = 50;

    int plot_w = win_w - margin_left - margin_right;
    int plot_h = win_h - margin_top - margin_bottom;

    // Draw info panel at top
    draw_info_panel(rend, info);

    // Draw heatmap
    for (int i = 0; i < n; i++) {
        Uint8 r, g, b;
        temp_to_rgb(temps[i], r, g, b);

        int x1 = margin_left + (i * plot_w) / n;
        int x2 = margin_left + ((i + 1) * plot_w) / n;

        SDL_SetRenderDrawColor(rend, r, g, b, 255);
        SDL_Rect rect = {x1, margin_top, x2 - x1 + 1, plot_h};
        SDL_RenderFillRect(rend, &rect);
    }

    // Draw grid lines
    draw_grid(rend, margin_left, margin_top, plot_w, plot_h, 5, 5);

    // Draw temperature profile line (white curve)
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    for (int i = 0; i < n - 1; i++) {
        double norm1 = (temps[i] - t_min_) / (t_max_ - t_min_);
        double norm2 = (temps[i+1] - t_min_) / (t_max_ - t_min_);
        norm1 = std::max(0.0, std::min(1.0, norm1));
        norm2 = std::max(0.0, std::min(1.0, norm2));

        int x1 = margin_left + (i * plot_w) / n;
        int x2 = margin_left + ((i + 1) * plot_w) / n;
        int y1 = margin_top + plot_h - static_cast<int>(norm1 * plot_h);
        int y2 = margin_top + plot_h - static_cast<int>(norm2 * plot_h);

        SDL_RenderDrawLine(rend, x1, y1, x2, y2);
    }

    // Find min and max temperature positions
    int min_idx = 0, max_idx = 0;
    double min_temp = temps[0], max_temp = temps[0];
    for (int i = 1; i < n; i++) {
        if (temps[i] < min_temp) { min_temp = temps[i]; min_idx = i; }
        if (temps[i] > max_temp) { max_temp = temps[i]; max_idx = i; }
    }

    // Draw min marker (blue circle)
    int min_x       = margin_left + (min_idx * plot_w) / n;
    double min_norm = (min_temp - t_min_) / (t_max_ - t_min_);
    int min_y       = margin_top + plot_h - static_cast<int>(min_norm * plot_h);
    SDL_SetRenderDrawColor(rend, 100, 150, 255, 255);
    for (int dx = -4; dx <= 4; dx++) {
        for (int dy = -4; dy <= 4; dy++) {
            if (dx*dx + dy*dy <= 16) SDL_RenderDrawPoint(rend, min_x + dx, min_y + dy);
        }
    }

    // Draw max marker (red circle)
    int max_x = margin_left + (max_idx * plot_w) / n;
    double max_norm = (max_temp - t_min_) / (t_max_ - t_min_);
    int max_y = margin_top + plot_h - static_cast<int>(max_norm * plot_h);
    SDL_SetRenderDrawColor(rend, 255, 100, 100, 255);
    for (int dx = -4; dx <= 4; dx++) {
        for (int dy = -4; dy <= 4; dy++) {
            if (dx*dx + dy*dy <= 16) SDL_RenderDrawPoint(rend, max_x + dx, max_y + dy);
        }
    }

    // Draw colorbar on right side
    draw_colorbar(rend, win_w - 70, margin_top, 15, plot_h);

    // Draw axes
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

    // X-axis
    SDL_RenderDrawLine(rend, margin_left, win_h - margin_bottom,
                       margin_left + plot_w, win_h - margin_bottom);

    // Y-axis (temperature)
    SDL_RenderDrawLine(rend, margin_left, margin_top,
                       margin_left, win_h - margin_bottom);

    // X-axis ticks and labels unit meters
    int num_x_ticks = 5;
    for (int i = 0; i <= num_x_ticks; i++) {
        int x = margin_left + (i * plot_w) / num_x_ticks;
        SDL_RenderDrawLine(rend, x, win_h - margin_bottom,
                          x, win_h - margin_bottom + 5);

        double pos = (i * info.L) / num_x_ticks;
        draw_number(rend, x - 10, win_h - margin_bottom + 10, pos);
    }

    // X-axis label
    SDL_SetRenderDrawColor(rend, 180, 180, 180, 255);
    draw_text(rend, margin_left + plot_w / 2 - 20, win_h - 15, "X [M]");

    // Y-axis ticks (temperature)
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    int num_y_ticks = 5;
    for (int i = 0; i <= num_y_ticks; i++) {
        int y = win_h - margin_bottom - (i * plot_h) / num_y_ticks;
        SDL_RenderDrawLine(rend, margin_left - 5, y, margin_left, y);

        double temp = t_min_ + (i * (t_max_ - t_min_)) / num_y_ticks;
        draw_number(rend, margin_left - 50, y - 5, temp);
    }

    // Boundary condition labels
    SDL_SetRenderDrawColor(rend, 150, 255, 150, 255);
    draw_text(rend, margin_left - 5, margin_top + plot_h + 25, "NEUMANN");

    SDL_SetRenderDrawColor(rend, 255, 180, 100, 255);
    char dirichlet_buf[32];
    snprintf(dirichlet_buf, sizeof(dirichlet_buf), "U=%.0fK", info.u0);
    draw_text(rend, margin_left + plot_w - 40, margin_top + plot_h + 25, dirichlet_buf);

    // Heat source labels
    // Source 1: [L/10, 2L/10]
    int src1_x1 = margin_left + static_cast<int>((1.0/10.0) * plot_w);
    int src1_x2 = margin_left + static_cast<int>((2.0/10.0) * plot_w);
    int src1_center = (src1_x1 + src1_x2) / 2;

    // Source 2: [5L/10, 6L/10]
    int src2_x1 = margin_left + static_cast<int>((5.0/10.0) * plot_w);
    int src2_x2 = margin_left + static_cast<int>((6.0/10.0) * plot_w);
    int src2_center = (src2_x1 + src2_x2) / 2;

    // Draw source region
    // Source 1 
    SDL_SetRenderDrawColor(rend, 0, 255, 255, 255);
    SDL_Rect src1_rect = {src1_x1, margin_top, src1_x2 - src1_x1, plot_h};
    SDL_RenderDrawRect(rend, &src1_rect);

    // Source 2 
    SDL_Rect src2_rect = {src2_x1, margin_top, src2_x2 - src2_x1, plot_h};
    SDL_RenderDrawRect(rend, &src2_rect);

    // Draw source region brackets at bottom
    int bracket_y = win_h - margin_bottom + 35;

    // Source 1 bracket with arrow pointing up
    SDL_RenderDrawLine(rend, src1_x1, bracket_y, src1_x1, bracket_y - 5);
    SDL_RenderDrawLine(rend, src1_x1, bracket_y - 5, src1_x2, bracket_y - 5);
    SDL_RenderDrawLine(rend, src1_x2, bracket_y, src1_x2, bracket_y - 5);

    // Arrow pointing to source region
    int arrow_x1 = src1_center;
    SDL_RenderDrawLine(rend, arrow_x1, bracket_y - 5, arrow_x1, bracket_y - 12);
    SDL_RenderDrawLine(rend, arrow_x1 - 3, bracket_y - 9, arrow_x1, bracket_y - 12);
    SDL_RenderDrawLine(rend, arrow_x1 + 3, bracket_y - 9, arrow_x1, bracket_y - 12);

    // Source 2 bracket with arrow
    SDL_RenderDrawLine(rend, src2_x1, bracket_y, src2_x1, bracket_y - 5);
    SDL_RenderDrawLine(rend, src2_x1, bracket_y - 5, src2_x2, bracket_y - 5);
    SDL_RenderDrawLine(rend, src2_x2, bracket_y, src2_x2, bracket_y - 5);
    int arrow_x2 = src2_center;
    SDL_RenderDrawLine(rend, arrow_x2, bracket_y - 5, arrow_x2, bracket_y - 12);
    SDL_RenderDrawLine(rend, arrow_x2 - 3, bracket_y - 9, arrow_x2, bracket_y - 12);
    SDL_RenderDrawLine(rend, arrow_x2 + 3, bracket_y - 9, arrow_x2, bracket_y - 12);

    // Draw source labels with power percentage
    SDL_SetRenderDrawColor(rend, 255, 200, 0, 255);  // Yellow for high power
    draw_text(rend, src1_center - 30, bracket_y + 2, "F1 100");

    SDL_SetRenderDrawColor(rend, 200, 150, 50, 255);  // Dimmer for lower power
    draw_text(rend, src2_center - 25, bracket_y + 2, "F2 75");
}

void SDLHeatmap::draw_2d_fullscreen(const std::vector<std::vector<double>>& temps, const SimInfo& info) {
    if (temps.empty()) return;

    SDL_Renderer* rend = win_.get_renderer();
    int win_w = win_.get_width();
    int win_h = win_.get_height();

    int ny = static_cast<int>(temps.size());
    int nx = static_cast<int>(temps[0].size());

    // Margins for axes and info panel
    int margin_left     = 60;
    int margin_right    = 80;  // Space for colorbar
    int margin_top      = 25;    // Space for info panel
    int margin_bottom   = 50;

    int plot_w = win_w - margin_left - margin_right;
    int plot_h = win_h - margin_top - margin_bottom;

    // Draw panel at top
    draw_info_panel(rend, info);

    // Bilinear interpolation with subsampling
    int sub = 2;
    int render_nx = (nx - 1) * sub;
    int render_ny = (ny - 1) * sub;

    for (int sj = 0; sj < render_ny; ++sj) {
        for (int si = 0; si < render_nx; ++si) {
            double fi = static_cast<double>(si) / sub;
            double fj = static_cast<double>(sj) / sub;

            int i0 = static_cast<int>(fi);
            int j0 = static_cast<int>(fj);
            int i1 = std::min(i0 + 1, nx - 1);
            int j1 = std::min(j0 + 1, ny - 1);

            double fx = fi - i0;
            double fy = fj - j0;

            double t = temps[j0][i0] * (1-fx) * (1-fy)
                     + temps[j0][i1] * fx * (1-fy)
                     + temps[j1][i0] * (1-fx) * fy
                     + temps[j1][i1] * fx * fy;

            Uint8 r, g, b;
            temp_to_rgb(t, r, g, b);

            int x1 = margin_left + (si * plot_w) / render_nx;
            int x2 = margin_left + ((si + 1) * plot_w) / render_nx;

            // Flip Y-axis: y=0 (Neumann) at bottom, y=L (Dirichlet) at top
            int y1 = margin_top + plot_h - ((sj + 1) * plot_h) / render_ny;
            int y2 = margin_top + plot_h - (sj * plot_h) / render_ny;

            SDL_SetRenderDrawColor(rend, r, g, b, 255);
            SDL_Rect rect = {x1, y1, x2 - x1 + 1, y2 - y1 + 1};
            SDL_RenderFillRect(rend, &rect);
        }
    }

    // Draw grid lines
    draw_grid(rend, margin_left, margin_top, plot_w, plot_h, 5, 5);

    // Find and mark min/max temperature positions
    int min_i = 0, min_j = 0, max_i = 0, max_j = 0;
    double min_temp = temps[0][0], max_temp = temps[0][0];
    for (int j = 0; j < ny; j++) {
        for (int i = 0; i < nx; i++) {
            if (temps[j][i] < min_temp) { min_temp = temps[j][i]; min_i = i; min_j = j; }
            if (temps[j][i] > max_temp) { max_temp = temps[j][i]; max_i = i; max_j = j; }
        }
    }

    // Draw min marker (blue circle)
    int min_x = margin_left + (min_i * plot_w) / nx;
    int min_y = margin_top + plot_h - (min_j * plot_h) / ny;  // Flip Y
    SDL_SetRenderDrawColor(rend, 100, 150, 255, 255);
    for (int dx = -5; dx <= 5; dx++) {
        for (int dy = -5; dy <= 5; dy++) {
            if (dx*dx + dy*dy <= 25 && dx*dx + dy*dy >= 9)
                SDL_RenderDrawPoint(rend, min_x + dx, min_y + dy);
        }
    }

    // Draw max marker (red circle)
    int max_x = margin_left + (max_i * plot_w) / nx;
    int max_y = margin_top + plot_h - (max_j * plot_h) / ny;  // Flip Y
    SDL_SetRenderDrawColor(rend, 255, 100, 100, 255);
    for (int dx = -5; dx <= 5; dx++) {
        for (int dy = -5; dy <= 5; dy++) {
            if (dx*dx + dy*dy <= 25 && dx*dx + dy*dy >= 9)
                SDL_RenderDrawPoint(rend, max_x + dx, max_y + dy);
        }
    }

    // Draw colorbar on right side
    draw_colorbar(rend, win_w - 70, margin_top, 15, plot_h);

    // Draw axes
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

    // X-axis
    SDL_RenderDrawLine(rend, margin_left, win_h - margin_bottom,
                       margin_left + plot_w, win_h - margin_bottom);

    // Y-axis
    SDL_RenderDrawLine(rend, margin_left, margin_top,
                       margin_left, win_h - margin_bottom);

    // X-axis ticks
    int num_ticks = 5;
    for (int i = 0; i <= num_ticks; i++) {
        int x = margin_left + (i * plot_w) / num_ticks;
        SDL_RenderDrawLine(rend, x, win_h - margin_bottom,
                          x, win_h - margin_bottom + 5);

        double pos = (i * info.L) / num_ticks;
        draw_number(rend, x - 10, win_h - margin_bottom + 10, pos);
    }

    // X-axis label
    SDL_SetRenderDrawColor(rend, 180, 180, 180, 255);
    draw_text(rend, margin_left + plot_w / 2 - 20, win_h - 15, "X [M]");

    // Y-axis ticks
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    for (int i = 0; i <= num_ticks; i++) {
        int y = win_h - margin_bottom - (i * plot_h) / num_ticks;
        SDL_RenderDrawLine(rend, margin_left - 5, y, margin_left, y);

        double pos = (i * info.L) / num_ticks;
        draw_number(rend, 10, y - 5, pos);
    }

    // Y-axis label
    SDL_SetRenderDrawColor(rend, 180, 180, 180, 255);
    draw_text(rend, 5, margin_top + plot_h / 2 - 5, "Y[M]");

    // Boundary condition labels for 2D
    // Bottom-left corner: Neumann (x=0, y=0)
    SDL_SetRenderDrawColor(rend, 150, 255, 150, 255);
    draw_text(rend, margin_left - 5, margin_top + plot_h + 25, "NEUMANN");

    // Top-right: Dirichlet (x=L, y=L)
    SDL_SetRenderDrawColor(rend, 255, 180, 100, 255);
    char dirichlet_buf[32];
    snprintf(dirichlet_buf, sizeof(dirichlet_buf), "U=%.0fK", info.u0);
    draw_text(rend, margin_left + plot_w - 40, margin_top - 12, dirichlet_buf);

    // Calculate source positions in screen coordinates
    double src_x1_norm = 1.0/6.0, src_x2_norm = 2.0/6.0;
    double src_x3_norm = 4.0/6.0, src_x4_norm = 5.0/6.0;
    double src_y1_norm = 1.0/6.0, src_y2_norm = 2.0/6.0;
    double src_y3_norm = 4.0/6.0, src_y4_norm = 5.0/6.0;

    // Screen coordinates for source regions
    int sx1 = margin_left + static_cast<int>(src_x1_norm * plot_w);
    int sx2 = margin_left + static_cast<int>(src_x2_norm * plot_w);
    int sx3 = margin_left + static_cast<int>(src_x3_norm * plot_w);
    int sx4 = margin_left + static_cast<int>(src_x4_norm * plot_w);

    int sy1 = margin_top + plot_h - static_cast<int>(src_y2_norm * plot_h);
    int sy2 = margin_top + plot_h - static_cast<int>(src_y1_norm * plot_h);
    int sy3 = margin_top + plot_h - static_cast<int>(src_y4_norm * plot_h);
    int sy4 = margin_top + plot_h - static_cast<int>(src_y3_norm * plot_h);

    // Draw source region rectangles
    SDL_SetRenderDrawColor(rend, 0, 255, 255, 255);
    SDL_Rect src_bl = {sx1, sy1, sx2 - sx1, sy2 - sy1};  // Bottom-left
    SDL_Rect src_br = {sx3, sy1, sx4 - sx3, sy2 - sy1};  // Bottom-right
    SDL_Rect src_tl = {sx1, sy3, sx2 - sx1, sy4 - sy3};  // Top-left
    SDL_Rect src_tr = {sx3, sy3, sx4 - sx3, sy4 - sy3};  // Top-right

    // Outer border
    SDL_RenderDrawRect(rend, &src_bl);
    SDL_RenderDrawRect(rend, &src_br);
    SDL_RenderDrawRect(rend, &src_tl);
    SDL_RenderDrawRect(rend, &src_tr);

    // Inner border
    SDL_Rect src_bl_inner = {sx1 + 1, sy1 + 1, sx2 - sx1 - 2, sy2 - sy1 - 2};
    SDL_Rect src_br_inner = {sx3 + 1, sy1 + 1, sx4 - sx3 - 2, sy2 - sy1 - 2};
    SDL_Rect src_tl_inner = {sx1 + 1, sy3 + 1, sx2 - sx1 - 2, sy4 - sy3 - 2};
    SDL_Rect src_tr_inner = {sx3 + 1, sy3 + 1, sx4 - sx3 - 2, sy4 - sy3 - 2};
    SDL_RenderDrawRect(rend, &src_bl_inner);
    SDL_RenderDrawRect(rend, &src_br_inner);
    SDL_RenderDrawRect(rend, &src_tl_inner);
    SDL_RenderDrawRect(rend, &src_tr_inner);

    // Draw corner marks for each source
    int mark_len = 5;

    // Bottom-left source corners
    SDL_RenderDrawLine(rend, sx1 - mark_len, sy1, sx1, sy1);
    SDL_RenderDrawLine(rend, sx1, sy1 - mark_len, sx1, sy1);
    SDL_RenderDrawLine(rend, sx2, sy2, sx2 + mark_len, sy2);
    SDL_RenderDrawLine(rend, sx2, sy2, sx2, sy2 + mark_len);

    // Draw source labels with "F" for flux/heat source
    SDL_SetRenderDrawColor(rend, 255, 200, 0, 255);  // Yellow
    draw_text(rend, (sx1 + sx2) / 2 - 5, (sy1 + sy2) / 2 - 5, "F1");
    draw_text(rend, (sx3 + sx4) / 2 - 5, (sy1 + sy2) / 2 - 5, "F2");
    draw_text(rend, (sx1 + sx2) / 2 - 5, (sy3 + sy4) / 2 - 5, "F3");
    draw_text(rend, (sx3 + sx4) / 2 - 5, (sy3 + sy4) / 2 - 5, "F4");
}

void SDLHeatmap::draw_1d_cell(const std::vector<double>& temps, const SimInfo& info,
                               int cell_x, int cell_y, int cell_w, int cell_h) {
    if (temps.empty()) return;

    SDL_Renderer* rend = win_.get_renderer();
    int n = static_cast<int>(temps.size());

    // Margins for cell mode
    int margin_left     = 50;
    int margin_right    = 60;
    int margin_top      = 20;
    int margin_bottom   = 40;

    int plot_x = cell_x + margin_left;
    int plot_y = cell_y + margin_top;
    int plot_w = cell_w - margin_left - margin_right;
    int plot_h = cell_h - margin_top - margin_bottom;

    // Draw material name and alpha at top
    SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
    draw_text(rend, cell_x + 5, cell_y + 5, info.material_name.c_str());

    char alpha_buf[32];
    snprintf(alpha_buf, sizeof(alpha_buf), "A=%.1E", info.alpha);
    draw_text(rend, cell_x + 80, cell_y + 5, alpha_buf);

    // Draw time
    char time_buf[32];
    snprintf(time_buf, sizeof(time_buf), "T=%.1FS", info.time);
    draw_text(rend, cell_x + cell_w - 70, cell_y + 5, time_buf);

    // Draw heatmap
    for (int i = 0; i < n; i++) {
        Uint8 r, g, b;
        temp_to_rgb(temps[i], r, g, b);

        int x1 = plot_x + (i * plot_w) / n;
        int x2 = plot_x + ((i + 1) * plot_w) / n;

        SDL_SetRenderDrawColor(rend, r, g, b, 255);
        SDL_Rect rect = {x1, plot_y, x2 - x1 + 1, plot_h};
        SDL_RenderFillRect(rend, &rect);
    }

    // Draw temperature profile line (white curve)
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    for (int i = 0; i < n - 1; i++) {
        double norm1 = (temps[i] - t_min_) / (t_max_ - t_min_);
        double norm2 = (temps[i+1] - t_min_) / (t_max_ - t_min_);
        norm1 = std::max(0.0, std::min(1.0, norm1));
        norm2 = std::max(0.0, std::min(1.0, norm2));

        int x1 = plot_x + (i * plot_w) / n;
        int x2 = plot_x + ((i + 1) * plot_w) / n;
        int y1 = plot_y + plot_h - static_cast<int>(norm1 * plot_h);
        int y2 = plot_y + plot_h - static_cast<int>(norm2 * plot_h);

        SDL_RenderDrawLine(rend, x1, y1, x2, y2);
    }

    // Temperature value projections at key positions
    // Temperature values at: x=0 (Neumann), center of sources, x=L (Dirichlet)
    int key_indices[] = {0, n/10 + n/20, n/2 + n/20, n-1};  // x=0, src1 center, src2 center, x=L
    const char* key_labels[] = {"", "S1", "S2", ""};

    for (int k = 0; k < 4; k++) {
        int idx = key_indices[k];
        if (idx >= n) idx = n - 1;

        double temp_val = temps[idx];
        double norm     = (temp_val - t_min_) / (t_max_ - t_min_);
        norm = std::max(0.0, std::min(1.0, norm));

        int px = plot_x + (idx * plot_w) / n;
        int py = plot_y + plot_h - static_cast<int>(norm * plot_h);

        // Draw projection line to Y-axis (dashed)
        SDL_SetRenderDrawColor(rend, 180, 180, 180, 255);
        for (int x = plot_x; x < px; x += 4) {
            SDL_RenderDrawPoint(rend, x, py);
        }

        // Draw small marker at the point
        SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);
        SDL_Rect marker = {px - 2, py - 2, 5, 5};
        SDL_RenderFillRect(rend, &marker);

        // Draw temperature value near the projection on Y-axis
        SDL_SetRenderDrawColor(rend, 255, 255, 150, 255);
        char val_buf[16];
        snprintf(val_buf, sizeof(val_buf), "%.0f", temp_val);

        // Offset labels to avoid overlap
        int label_y = py - 3 + (k % 2) * 12;
        if (k == 0) {
            draw_text(rend, plot_x - 25, label_y, val_buf);
        } else if (k == 3) {
            draw_text(rend, px + 3, label_y, val_buf);
        }
    }

    // Draw colorbar on right side
    int cb_x = cell_x + cell_w - margin_right + 5;
    int cb_w = 12;
    int cb_h = plot_h;

    for (int i = 0; i < cb_h; ++i) {
        double t = t_max_ - (i * (t_max_ - t_min_)) / cb_h;
        Uint8 r, g, b;
        temp_to_rgb(t, r, g, b);
        SDL_SetRenderDrawColor(rend, r, g, b, 255);
        SDL_RenderDrawLine(rend, cb_x, plot_y + i, cb_x + cb_w, plot_y + i);
    }
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_Rect cb_border = {cb_x - 1, plot_y - 1, cb_w + 2, cb_h + 2};
    SDL_RenderDrawRect(rend, &cb_border);

    // Colorbar labels (min/max)
    draw_number(rend, cb_x + cb_w + 3, plot_y - 3, t_max_);
    draw_number(rend, cb_x + cb_w + 3, plot_y + cb_h - 8, t_min_);

    // Draw axes
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderDrawLine(rend, plot_x, plot_y + plot_h, plot_x + plot_w, plot_y + plot_h);  // X-axis
    SDL_RenderDrawLine(rend, plot_x, plot_y, plot_x, plot_y + plot_h);  // Y-axis

    // X-axis labels (0 and L)
    draw_number(rend, plot_x - 5, plot_y + plot_h + 5, 0.0);
    draw_number(rend, plot_x + plot_w - 15, plot_y + plot_h + 5, info.L);

    // Y-axis label "ΔT [K]" for temperature increase
    SDL_SetRenderDrawColor(rend, 180, 180, 180, 255);
    draw_text(rend, plot_x - 45, plot_y + plot_h / 2 - 5, "DT[K]");

    // Y-axis labels (temp range)
    SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
    draw_number(rend, plot_x - 45, plot_y - 3, t_max_);
    draw_number(rend, plot_x - 45, plot_y + plot_h - 8, t_min_);

    // X-axis label
    SDL_SetRenderDrawColor(rend, 180, 180, 180, 255);
    draw_text(rend, plot_x + plot_w / 2 - 10, plot_y + plot_h + 25, "X[M]");

    // Boundary condition labels
    SDL_SetRenderDrawColor(rend, 150, 255, 150, 255);
    draw_text(rend, plot_x - 5, plot_y + plot_h + 15, "N");  // Neumann at x=0

    SDL_SetRenderDrawColor(rend, 255, 180, 100, 255);
    draw_text(rend, plot_x + plot_w - 10, plot_y + plot_h + 15, "D");  // Dirichlet at x=L

    // Heat source regions - improved visualization
    int src1_x1 = plot_x + static_cast<int>((1.0/10.0) * plot_w);
    int src1_x2 = plot_x + static_cast<int>((2.0/10.0) * plot_w);
    int src2_x1 = plot_x + static_cast<int>((5.0/10.0) * plot_w);
    int src2_x2 = plot_x + static_cast<int>((6.0/10.0) * plot_w);

    // Draw semi-transparent filled regions for heat sources
    SDL_SetRenderDrawColor(rend, 255, 200, 0, 255);  // Yellow/orange for heat
    // Source 1 
    for (int y = plot_y; y < plot_y + plot_h; y += 3) {
        for (int x = src1_x1; x < src1_x2; x += 3) {
            SDL_RenderDrawPoint(rend, x, y);
        }
    }
    // Source 2 
    for (int y = plot_y; y < plot_y + plot_h; y += 3) {
        for (int x = src2_x1; x < src2_x2; x += 3) {
            SDL_RenderDrawPoint(rend, x, y);
        }
    }

    // Draw thick borders around heat sources
    SDL_SetRenderDrawColor(rend, 255, 150, 0, 255);  // Orange border

    // Source 1
    SDL_Rect src1 = {src1_x1, plot_y, src1_x2 - src1_x1, plot_h};
    SDL_Rect src1_inner = {src1_x1 + 1, plot_y + 1, src1_x2 - src1_x1 - 2, plot_h - 2};
    SDL_RenderDrawRect(rend, &src1);
    SDL_RenderDrawRect(rend, &src1_inner);

    // Source 2
    SDL_Rect src2 = {src2_x1, plot_y, src2_x2 - src2_x1, plot_h};
    SDL_Rect src2_inner = {src2_x1 + 1, plot_y + 1, src2_x2 - src2_x1 - 2, plot_h - 2};
    SDL_RenderDrawRect(rend, &src2);
    SDL_RenderDrawRect(rend, &src2_inner);

    // Draw arrows pointing down into the sources (heat input indicator)
    SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);  // Bright yellow arrows
    int arrow_y = plot_y - 8;
    int src1_cx = (src1_x1 + src1_x2) / 2;
    int src2_cx = (src2_x1 + src2_x2) / 2;

    // Arrow 1 pointing down
    SDL_RenderDrawLine(rend, src1_cx, arrow_y, src1_cx, plot_y - 2);
    SDL_RenderDrawLine(rend, src1_cx - 3, plot_y - 5, src1_cx, plot_y - 2);
    SDL_RenderDrawLine(rend, src1_cx + 3, plot_y - 5, src1_cx, plot_y - 2);

    // Arrow 2 pointing down
    SDL_RenderDrawLine(rend, src2_cx, arrow_y, src2_cx, plot_y - 2);
    SDL_RenderDrawLine(rend, src2_cx - 3, plot_y - 5, src2_cx, plot_y - 2);
    SDL_RenderDrawLine(rend, src2_cx + 3, plot_y - 5, src2_cx, plot_y - 2);

    // Find and draw min/max temperature markers
    int min_idx = 0, max_idx = 0;
    double min_temp = temps[0], max_temp = temps[0];
    for (int i = 1; i < n; i++) {
        if (temps[i] < min_temp) { min_temp = temps[i]; min_idx = i; }
        if (temps[i] > max_temp) { max_temp = temps[i]; max_idx = i; }
    }

    // Min marker (blue circle)
    int min_x = plot_x + (min_idx * plot_w) / n;
    double min_norm = (min_temp - t_min_) / (t_max_ - t_min_);
    min_norm        = std::max(0.0, std::min(1.0, min_norm));
    int min_y       = plot_y + plot_h - static_cast<int>(min_norm * plot_h);
    SDL_SetRenderDrawColor(rend, 100, 150, 255, 255);
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            if (dx*dx + dy*dy <= 9) SDL_RenderDrawPoint(rend, min_x + dx, min_y + dy);
        }
    }

    // Max marker (red circle)
    int max_x = plot_x + (max_idx * plot_w) / n;
    double max_norm = (max_temp - t_min_) / (t_max_ - t_min_);
    max_norm = std::max(0.0, std::min(1.0, max_norm));
    int max_y = plot_y + plot_h - static_cast<int>(max_norm * plot_h);
    SDL_SetRenderDrawColor(rend, 255, 100, 100, 255);
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            if (dx*dx + dy*dy <= 9) SDL_RenderDrawPoint(rend, max_x + dx, max_y + dy);
        }
    }

    // Progress bar
    int bar_x = cell_x + cell_w - 65;
    int bar_y = cell_y + cell_h - 18;
    int bar_w = 55;
    int bar_h = 8;
    double progress = info.time / info.tmax;

    SDL_SetRenderDrawColor(rend, 60, 60, 60, 255);
    SDL_Rect bar_bg = {bar_x, bar_y, bar_w, bar_h};
    SDL_RenderFillRect(rend, &bar_bg);
    SDL_SetRenderDrawColor(rend, 80, 180, 80, 255);
    SDL_Rect bar_fg = {bar_x, bar_y, static_cast<int>(bar_w * progress), bar_h};
    SDL_RenderFillRect(rend, &bar_fg);
    SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
    SDL_RenderDrawRect(rend, &bar_bg);

    // Speed indicator
    char speed_buf[16];
    snprintf(speed_buf, sizeof(speed_buf), "X%d", info.speed);
    SDL_SetRenderDrawColor(rend, 150, 200, 255, 255);
    draw_text(rend, cell_x + cell_w - 130, cell_y + cell_h - 18, speed_buf);

    // Paused indicator
    if (info.paused) {
        SDL_SetRenderDrawColor(rend, 255, 200, 50, 255);
        draw_text(rend, cell_x + 5, cell_y + cell_h - 18, "PAUSED");
    }

    // Draw border
    SDL_SetRenderDrawColor(rend, 100, 100, 100, 255);
    SDL_Rect border = {plot_x, plot_y, plot_w, plot_h};
    SDL_RenderDrawRect(rend, &border);
}

void SDLHeatmap::draw_2d_cell(const std::vector<std::vector<double>>& temps, const SimInfo& info,
                               int cell_x, int cell_y, int cell_w, int cell_h) {
    if (temps.empty()) return;

    SDL_Renderer* rend = win_.get_renderer();

    int ny = static_cast<int>(temps.size());
    int nx = static_cast<int>(temps[0].size());

    // Margins for cell mode
    int margin_left     = 50;
    int margin_right    = 60;
    int margin_top      = 20;
    int margin_bottom   = 40;

    int plot_x = cell_x + margin_left;
    int plot_y = cell_y + margin_top;
    int plot_w = cell_w - margin_left - margin_right;
    int plot_h = cell_h - margin_top - margin_bottom;

    // Draw material name and alpha at top
    SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
    draw_text(rend, cell_x + 5, cell_y + 5, info.material_name.c_str());

    char alpha_buf[32];
    snprintf(alpha_buf, sizeof(alpha_buf), "A=%.1E", info.alpha);
    draw_text(rend, cell_x + 80, cell_y + 5, alpha_buf);

    // Draw time
    char time_buf[32];
    snprintf(time_buf, sizeof(time_buf), "T=%.1FS", info.time);
    draw_text(rend, cell_x + cell_w - 70, cell_y + 5, time_buf);

    // Draw heatmap with bilinear interpolation
    int sub       = 2;
    int render_nx = (nx - 1) * sub;
    int render_ny = (ny - 1) * sub;

    for (int sj = 0; sj < render_ny; ++sj) {
        for (int si = 0; si < render_nx; ++si) {
            double fi = static_cast<double>(si) / sub;
            double fj = static_cast<double>(sj) / sub;

            int i0 = static_cast<int>(fi);
            int j0 = static_cast<int>(fj);
            int i1 = std::min(i0 + 1, nx - 1);
            int j1 = std::min(j0 + 1, ny - 1);

            double fx = fi - i0;
            double fy = fj - j0;

            double t = temps[j0][i0] * (1-fx) * (1-fy)
                     + temps[j0][i1] * fx * (1-fy)
                     + temps[j1][i0] * (1-fx) * fy
                     + temps[j1][i1] * fx * fy;

            Uint8 r, g, b;
            temp_to_rgb(t, r, g, b);

            int x1 = plot_x + (si * plot_w) / render_nx;
            int x2 = plot_x + ((si + 1) * plot_w) / render_nx;
            // Flip Y-axis
            int y1 = plot_y + plot_h - ((sj + 1) * plot_h) / render_ny;
            int y2 = plot_y + plot_h - (sj * plot_h) / render_ny;

            SDL_SetRenderDrawColor(rend, r, g, b, 255);
            SDL_Rect rect = {x1, y1, x2 - x1 + 1, y2 - y1 + 1};
            SDL_RenderFillRect(rend, &rect);
        }
    }

    // Draw heat flow arrows (negative gradient shows direction of heat flow)
    // Heat flows from hot to cold regions
    int arrow_grid  = 8; // 8x8 grids
    int arrow_len   = std::min(plot_w, plot_h) / (arrow_grid * 2);

    for (int aj = 0; aj < arrow_grid; aj++) {
        for (int ai = 0; ai < arrow_grid; ai++) {
            // Sample point in the grid
            int i = (ai * (nx - 1)) / arrow_grid;
            int j = (aj * (ny - 1)) / arrow_grid;

            if (i <= 0 || i >= nx - 1 || j <= 0 || j >= ny - 1) continue;

            // Calculate gradient
            double dTdx = (temps[j][i+1] - temps[j][i-1]) / (2.0 * (info.L / nx));
            double dTdy = (temps[j+1][i] - temps[j-1][i]) / (2.0 * (info.L / ny));

            // Heat flow is 
            double flow_x = -dTdx;
            double flow_y = -dTdy;

            // Normalize and scale arrow
            double mag = std::sqrt(flow_x * flow_x + flow_y * flow_y);
            if (mag < 0.1) continue; 

            double scale = arrow_len / mag;
            if (scale > arrow_len) scale = arrow_len;  // Limit max length

            int ax = static_cast<int>(flow_x * scale);
            int ay = static_cast<int>(flow_y * scale);

            // Arrow base position
            int bx = plot_x + (i * plot_w) / nx;
            int by = plot_y + plot_h - (j * plot_h) / ny;  // Y flipped

            // Arrow tip
            int tx = bx + ax;
            int ty = by - ay;  // Y flipped

            // Draw arrow line 
            SDL_SetRenderDrawColor(rend, 0, 255, 200, 255);
            SDL_RenderDrawLine(rend, bx, by, tx, ty);

            // Draw arrowhead
            double angle = std::atan2(-ay, ax);
            int head_len = 4;
            int hx1 = tx - static_cast<int>(head_len * std::cos(angle - 0.5));
            int hy1 = ty - static_cast<int>(head_len * std::sin(angle - 0.5));
            int hx2 = tx - static_cast<int>(head_len * std::cos(angle + 0.5));
            int hy2 = ty - static_cast<int>(head_len * std::sin(angle + 0.5));
            SDL_RenderDrawLine(rend, tx, ty, hx1, hy1);
            SDL_RenderDrawLine(rend, tx, ty, hx2, hy2);
        }
    }

    // Draw contour lines (isotherms) 
    int num_contours = 5;
    for (int c = 1; c < num_contours; c++) {
        double contour_temp = t_min_ + c * (t_max_ - t_min_) / num_contours;

        // Color based on temperature level
        Uint8 cr, cg, cb;
        temp_to_rgb(contour_temp, cr, cg, cb);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

        for (int j = 0; j < ny - 1; j++) {
            for (int i = 0; i < nx - 1; i++) {
                double t00 = temps[j][i];
                double t10 = temps[j][i+1];
                double t01 = temps[j+1][i];
                double t11 = temps[j+1][i+1];

                double cell_min = std::min({t00, t10, t01, t11});
                double cell_max = std::max({t00, t10, t01, t11});

                if (contour_temp >= cell_min && contour_temp <= cell_max) {
                    int cx = plot_x + ((i * 2 + 1) * plot_w) / (2 * nx);
                    int cy = plot_y + plot_h - ((j * 2 + 1) * plot_h) / (2 * ny);
                    // Draw small cross for contour point
                    SDL_RenderDrawPoint(rend, cx, cy);
                    SDL_RenderDrawPoint(rend, cx+1, cy);
                    SDL_RenderDrawPoint(rend, cx-1, cy);
                    SDL_RenderDrawPoint(rend, cx, cy+1);
                    SDL_RenderDrawPoint(rend, cx, cy-1);
                }
            }
        }
    }

    // Draw temperature profile along bottom edge (Y=0, varying X)
    int profile_h = 25;  // Height of profile graph
    int profile_y = plot_y + plot_h + 5;  // Below the main plot

    SDL_SetRenderDrawColor(rend, 100, 200, 255, 255);  // Light blue for X profile
    for (int i = 0; i < nx - 1; i++) {
        double t1 = temps[0][i];
        double t2 = temps[0][i+1];
        double norm1 = (t1 - t_min_) / (t_max_ - t_min_);
        double norm2 = (t2 - t_min_) / (t_max_ - t_min_);

        norm1 = std::max(0.0, std::min(1.0, norm1));
        norm2 = std::max(0.0, std::min(1.0, norm2));

        int x1 = plot_x + (i * plot_w) / nx;
        int x2 = plot_x + ((i + 1) * plot_w) / nx;
        int y1 = profile_y + profile_h - static_cast<int>(norm1 * profile_h);
        int y2 = profile_y + profile_h - static_cast<int>(norm2 * profile_h);

        SDL_RenderDrawLine(rend, x1, y1, x2, y2);
    }

    // Draw temperature profile along left edge (X=0, varying Y)
    int profile_w = 20;  // Width of profile graph
    int profile_x = plot_x - profile_w - 5;  // Left of the main plot

    SDL_SetRenderDrawColor(rend, 255, 200, 100, 255);  // Orange for Y profile
    for (int j = 0; j < ny - 1; j++) {
        double t1 = temps[j][0];
        double t2 = temps[j+1][0];
        double norm1 = (t1 - t_min_) / (t_max_ - t_min_);
        double norm2 = (t2 - t_min_) / (t_max_ - t_min_);
        norm1 = std::max(0.0, std::min(1.0, norm1));
        norm2 = std::max(0.0, std::min(1.0, norm2));

        int y1 = plot_y + plot_h - (j * plot_h) / ny;
        int y2 = plot_y + plot_h - ((j + 1) * plot_h) / ny;
        int x1 = profile_x + static_cast<int>(norm1 * profile_w);
        int x2 = profile_x + static_cast<int>(norm2 * profile_w);

        SDL_RenderDrawLine(rend, x1, y1, x2, y2);
    }

    // Temperature values at key positions
    struct ProjPt2D { int i; int j; };
    ProjPt2D proj_pts_2d[] = {
        {0, 0},           // Bottom-left (Neumann corner)
        {nx-1, ny-1},     // Top-right (Dirichlet corner)
        {nx/2, ny/2}      // Center
    };

    for (int k = 0; k < 3; k++) {
        int pi = proj_pts_2d[k].i;
        int pj = proj_pts_2d[k].j;
        double temp_val = temps[pj][pi];

        int px = plot_x + (pi * plot_w) / nx;
        int py = plot_y + plot_h - (pj * plot_h) / ny;  // Y flipped

        // Draw marker
        SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);
        for (int dx = -2; dx <= 2; dx++) {
            for (int dy = -2; dy <= 2; dy++) {
                if (dx*dx + dy*dy <= 4) SDL_RenderDrawPoint(rend, px + dx, py + dy);
            }
        }

        // Draw temperature value
        SDL_SetRenderDrawColor(rend, 255, 255, 150, 255);
        char val_buf[16];
        snprintf(val_buf, sizeof(val_buf), "%.0f", temp_val);

        // Position label
        int lx = px + 4, ly = py - 5;
        if (pi == 0) lx = px - 22;
        if (pj == ny-1) ly = py + 2;

        draw_text(rend, lx, ly, val_buf);
    }

    // Draw colorbar on right side
    int cb_x = cell_x + cell_w - margin_right + 5;
    int cb_w = 12;
    int cb_h = plot_h;
    for (int i = 0; i < cb_h; ++i) {
        double t = t_max_ - (i * (t_max_ - t_min_)) / cb_h;
        Uint8 r, g, b;
        temp_to_rgb(t, r, g, b);
        SDL_SetRenderDrawColor(rend, r, g, b, 255);
        SDL_RenderDrawLine(rend, cb_x, plot_y + i, cb_x + cb_w, plot_y + i);
    }

    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

    SDL_Rect cb_border = {cb_x - 1, plot_y - 1, cb_w + 2, cb_h + 2};
    SDL_RenderDrawRect(rend, &cb_border);

    // Colorbar labels (min/max)
    draw_number(rend, cb_x + cb_w + 3, plot_y - 3, t_max_);
    draw_number(rend, cb_x + cb_w + 3, plot_y + cb_h - 8, t_min_);

    // Draw axes
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderDrawLine(rend, plot_x, plot_y + plot_h, plot_x + plot_w, plot_y + plot_h);  // X-axis
    SDL_RenderDrawLine(rend, plot_x, plot_y, plot_x, plot_y + plot_h);  // Y-axis

    // X-axis labels (0 and L)
    draw_number(rend, plot_x - 5, plot_y + plot_h + 5, 0.0);
    draw_number(rend, plot_x + plot_w - 15, plot_y + plot_h + 5, info.L);

    // Y-axis labels (0 and L)
    draw_number(rend, plot_x - 30, plot_y + plot_h - 8, 0.0);
    draw_number(rend, plot_x - 30, plot_y - 3, info.L);

    // Axis labels with ΔT for temperature increase (colorbar)
    SDL_SetRenderDrawColor(rend, 180, 180, 180, 255);
    draw_text(rend, plot_x + plot_w / 2 - 10, plot_y + plot_h + 25, "X[M]");
    draw_text(rend, plot_x - 30, plot_y + plot_h / 2 - 5, "Y");
    // ΔT label next to colorbar
    draw_text(rend, cb_x - 10, plot_y - 12, "DT[K]");

    // Boundary condition labels
    SDL_SetRenderDrawColor(rend, 150, 255, 150, 255);
    draw_text(rend, plot_x - 5, plot_y + plot_h + 15, "N");  // Neumann at x=0, y=0

    SDL_SetRenderDrawColor(rend, 255, 180, 100, 255);
    draw_text(rend, plot_x + plot_w - 10, plot_y + plot_h + 15, "D");  // Dirichlet at x=L
    draw_text(rend, plot_x + plot_w - 10, plot_y - 10, "D");  // Dirichlet at y=L

    // Heat source regions
    double src_x1 = 1.0/6.0, src_x2 = 2.0/6.0;
    double src_x3 = 4.0/6.0, src_x4 = 5.0/6.0;
    double src_y1 = 1.0/6.0, src_y2 = 2.0/6.0;
    double src_y3 = 4.0/6.0, src_y4 = 5.0/6.0;

    int sx1 = plot_x + static_cast<int>(src_x1 * plot_w);
    int sx2 = plot_x + static_cast<int>(src_x2 * plot_w);
    int sx3 = plot_x + static_cast<int>(src_x3 * plot_w);
    int sx4 = plot_x + static_cast<int>(src_x4 * plot_w);

    // Y flipped
    int sy1 = plot_y + plot_h - static_cast<int>(src_y2 * plot_h);
    int sy2 = plot_y + plot_h - static_cast<int>(src_y1 * plot_h);
    int sy3 = plot_y + plot_h - static_cast<int>(src_y4 * plot_h);
    int sy4 = plot_y + plot_h - static_cast<int>(src_y3 * plot_h);

    // Draw dithered fill pattern for heat sources (yellow/orange dots)
    SDL_SetRenderDrawColor(rend, 255, 200, 0, 255);
    // Bottom-left source
    for (int y = sy1; y < sy2; y += 4) {
        for (int x = sx1; x < sx2; x += 4) {
            SDL_RenderDrawPoint(rend, x, y);
        }
    }
    // Bottom-right source
    for (int y = sy1; y < sy2; y += 4) {
        for (int x = sx3; x < sx4; x += 4) {
            SDL_RenderDrawPoint(rend, x, y);
        }
    }
    // Top-left source
    for (int y = sy3; y < sy4; y += 4) {
        for (int x = sx1; x < sx2; x += 4) {
            SDL_RenderDrawPoint(rend, x, y);
        }
    }
    // Top-right source
    for (int y = sy3; y < sy4; y += 4) {
        for (int x = sx3; x < sx4; x += 4) {
            SDL_RenderDrawPoint(rend, x, y);
        }
    }

    // Draw thick orange borders around heat sources
    SDL_SetRenderDrawColor(rend, 255, 150, 0, 255);
    SDL_Rect src_bl = {sx1, sy1, sx2 - sx1, sy2 - sy1};
    SDL_Rect src_br = {sx3, sy1, sx4 - sx3, sy2 - sy1};
    SDL_Rect src_tl = {sx1, sy3, sx2 - sx1, sy4 - sy3};
    SDL_Rect src_tr = {sx3, sy3, sx4 - sx3, sy4 - sy3};

    // Double border for visibility
    SDL_RenderDrawRect(rend, &src_bl);
    SDL_RenderDrawRect(rend, &src_br);
    SDL_RenderDrawRect(rend, &src_tl);
    SDL_RenderDrawRect(rend, &src_tr);
    SDL_Rect src_bl_in = {sx1+1, sy1+1, sx2-sx1-2, sy2-sy1-2};
    SDL_Rect src_br_in = {sx3+1, sy1+1, sx4-sx3-2, sy2-sy1-2};
    SDL_Rect src_tl_in = {sx1+1, sy3+1, sx2-sx1-2, sy4-sy3-2};
    SDL_Rect src_tr_in = {sx3+1, sy3+1, sx4-sx3-2, sy4-sy3-2};
    SDL_RenderDrawRect(rend, &src_bl_in);
    SDL_RenderDrawRect(rend, &src_br_in);
    SDL_RenderDrawRect(rend, &src_tl_in);
    SDL_RenderDrawRect(rend, &src_tr_in);

    // Find and draw min/max temperature markers
    int min_i = 0, min_j = 0, max_i = 0, max_j = 0;
    double min_temp = temps[0][0], max_temp = temps[0][0];
    for (int j = 0; j < ny; j++) {
        for (int i = 0; i < nx; i++) {
            if (temps[j][i] < min_temp) { min_temp = temps[j][i]; min_i = i; min_j = j; }
            if (temps[j][i] > max_temp) { max_temp = temps[j][i]; max_i = i; max_j = j; }
        }
    }

    // Min marker (blue circle ring)
    int minx = plot_x + (min_i * plot_w) / nx;
    int miny = plot_y + plot_h - (min_j * plot_h) / ny;  // Y flipped
    SDL_SetRenderDrawColor(rend, 100, 150, 255, 255);
    for (int dx = -4; dx <= 4; dx++) {
        for (int dy = -4; dy <= 4; dy++) {
            if (dx*dx + dy*dy <= 16 && dx*dx + dy*dy >= 4)
                SDL_RenderDrawPoint(rend, minx + dx, miny + dy);
        }
    }

    // Max marker (red circle ring)
    int maxx = plot_x + (max_i * plot_w) / nx;
    int maxy = plot_y + plot_h - (max_j * plot_h) / ny;  // Y flipped
    SDL_SetRenderDrawColor(rend, 255, 100, 100, 255);
    for (int dx = -4; dx <= 4; dx++) {
        for (int dy = -4; dy <= 4; dy++) {
            if (dx*dx + dy*dy <= 16 && dx*dx + dy*dy >= 4)
                SDL_RenderDrawPoint(rend, maxx + dx, maxy + dy);
        }
    }

    // Progress bar
    int bar_x = cell_x + cell_w - 65;
    int bar_y = cell_y + cell_h - 18;
    int bar_w = 55;
    int bar_h = 8;
    double progress = info.time / info.tmax;


    SDL_SetRenderDrawColor(rend, 60, 60, 60, 255);
    SDL_Rect bar_bg = {bar_x, bar_y, bar_w, bar_h};
    SDL_RenderFillRect(rend, &bar_bg);
    SDL_SetRenderDrawColor(rend, 80, 180, 80, 255);
    SDL_Rect bar_fg = {bar_x, bar_y, static_cast<int>(bar_w * progress), bar_h};
    SDL_RenderFillRect(rend, &bar_fg);
    SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
    SDL_RenderDrawRect(rend, &bar_bg);

    // Speed indicator
    char speed_buf[16];
    snprintf(speed_buf, sizeof(speed_buf), "X%d", info.speed);
    SDL_SetRenderDrawColor(rend, 150, 200, 255, 255);
    draw_text(rend, cell_x + cell_w - 130, cell_y + cell_h - 18, speed_buf);

    // Paused indicator
    if (info.paused) {
        SDL_SetRenderDrawColor(rend, 255, 200, 50, 255);
        draw_text(rend, cell_x + 5, cell_y + cell_h - 18, "PAUSED");
    }

    // Draw border
    SDL_SetRenderDrawColor(rend, 100, 100, 100, 255);
    SDL_Rect border = {plot_x, plot_y, plot_w, plot_h};
    SDL_RenderDrawRect(rend, &border);
}

}
