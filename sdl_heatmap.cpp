/**
 * @file sdl_heatmap.cpp
 * @brief Temperature visualization implementation
 */

#include "sdl_heatmap.hpp"
#include <algorithm>
#include <cmath>

namespace sdl {

SDLHeatmap::SDLHeatmap(SDLWindow& win, double t_min, double t_max)
    : win_(win), t_min_(t_min), t_max_(t_max) {}

void SDLHeatmap::set_range(double t_min, double t_max) {
    t_min_ = t_min;
    t_max_ = t_max;
}

void SDLHeatmap::auto_range(const std::vector<double>& temps) {
    if (temps.empty()) return;
    auto minmax = std::minmax_element(temps.begin(), temps.end());
    double margin = (*minmax.second - *minmax.first) * 0.05;
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

void SDLHeatmap::draw_1d_fullscreen(const std::vector<double>& temps) {
    if (temps.empty()) return;

    SDL_Renderer* rend = win_.get_renderer();
    int win_w = win_.get_width();
    int win_h = win_.get_height();
    int n = static_cast<int>(temps.size());

    for (int i = 0; i < n; i++) {
        Uint8 r, g, b;
        temp_to_rgb(temps[i], r, g, b);

        int x1 = (i * win_w) / n;
        int x2 = ((i + 1) * win_w) / n;

        SDL_SetRenderDrawColor(rend, r, g, b, 255);
        SDL_Rect rect = {x1, 0, x2 - x1 + 1, win_h};
        SDL_RenderFillRect(rend, &rect);
    }
}

void SDLHeatmap::draw_2d_fullscreen(const std::vector<std::vector<double>>& temps) {
    if (temps.empty()) return;

    SDL_Renderer* rend = win_.get_renderer();
    int win_w = win_.get_width();
    int win_h = win_.get_height();

    int ny = static_cast<int>(temps.size());
    int nx = static_cast<int>(temps[0].size());

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

            int x1 = (si * win_w) / render_nx;
            int x2 = ((si + 1) * win_w) / render_nx;
            int y1 = (sj * win_h) / render_ny;
            int y2 = ((sj + 1) * win_h) / render_ny;

            SDL_SetRenderDrawColor(rend, r, g, b, 255);
            SDL_Rect rect = {x1, y1, x2 - x1 + 1, y2 - y1 + 1};
            SDL_RenderFillRect(rend, &rect);
        }
    }
}

}
