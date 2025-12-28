#include "sdl_heatmap.hpp"
#include <algorithm>
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

    SDLHeatmap::SDLHeatmap(
        SDLWindow& win
        , double t_min
        , double t_max
    )
        : win_(win)
        , t_min_(t_min)
        , t_max_(t_max)
        , font_(std::make_unique<SDLFont>(FONT_PATH, 14))
        , label_font_(std::make_unique<SDLFont>(FONT_PATH, 18)) {
    }

    void SDLHeatmap::set_range(double t_min, double t_max) {
        t_min_ = t_min;
        t_max_ = t_max;
    }

    void SDLHeatmap::auto_range(const std::vector<double>& temps) {
        if (temps.empty()) return;
        auto [min_it, max_it] = std::minmax_element(temps.begin(), temps.end());
        double margin = (*max_it - *min_it) * 0.05;
        t_min_ = *min_it - margin;
        t_max_ = *max_it + margin;
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

    // Inferno colormap - scientific visualization colormap
    // Perceptually uniform, from matplotlib
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

        // Use Inferno colormap with smooth interpolation
        double idx = norm * (INFERNO_SIZE - 1);
        int i0 = static_cast<int>(idx);
        int i1 = std::min(i0 + 1, INFERNO_SIZE - 1);
        double frac = idx - i0;

        // Linear interpolation between colormap entries for smooth gradient
        r = static_cast<Uint8>(INFERNO_MAP[i0][0] * (1 - frac) + INFERNO_MAP[i1][0] * frac);
        g = static_cast<Uint8>(INFERNO_MAP[i0][1] * (1 - frac) + INFERNO_MAP[i1][1] * frac);
        b = static_cast<Uint8>(INFERNO_MAP[i0][2] * (1 - frac) + INFERNO_MAP[i1][2] * frac);
    }

    void SDLHeatmap::draw_bar_1d(
        const std::vector<double>& temps
        , int bx
        , int by
        , int bw
        , int bh
    ) {
        if (temps.empty()) return;

        SDL_Renderer* rend = win_.get_renderer();
        int n = static_cast<int>(temps.size());

        for (int i = 0; i < n; i++) {
            Uint8 r, g, b;
            temp_to_rgb(temps[i], r, g, b);

            int x1 = bx + (i * bw) / n;
            int x2 = bx + ((i + 1) * bw) / n;

            SDL_SetRenderDrawColor(rend, r, g, b, 255);
            SDL_Rect rect = {x1, by, x2 - x1 + 1, bh};
            SDL_RenderFillRect(rend, &rect);
        }

        SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
        SDL_Rect border = {bx, by, bw, bh};
        SDL_RenderDrawRect(rend, &border);
    }

    void SDLHeatmap::draw_plate_2d(
        const std::vector<std::vector<double>>& temps
        , int px
        , int py
        , int ps
    ) {
        SDL_Renderer* rend = win_.get_renderer();
        int ny = static_cast<int>(temps.size());
        if (ny == 0) return;
        int nx = static_cast<int>(temps[0].size());

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

                double t00 = temps[j0][i0];
                double t10 = temps[j0][i1];
                double t01 = temps[j1][i0];
                double t11 = temps[j1][i1];

                double t = t00 * (1 - fx) * (1 - fy)
                         + t10 * fx * (1 - fy)
                         + t01 * (1 - fx) * fy
                         + t11 * fx * fy;

                Uint8 r, g, b;
                temp_to_rgb(t, r, g, b);

                int x1 = px + (si * ps) / render_nx;
                int x2 = px + ((si + 1) * ps) / render_nx;
                int y1 = py + (sj * ps) / render_ny;
                int y2 = py + ((sj + 1) * ps) / render_ny;

                SDL_SetRenderDrawColor(rend, r, g, b, 255);
                SDL_Rect rect = {x1, y1, x2 - x1 + 1, y2 - y1 + 1};
                SDL_RenderFillRect(rend, &rect);
            }
        }

        SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
        SDL_Rect border = {px, py, ps, ps};
        SDL_RenderDrawRect(rend, &border);
    }

    void SDLHeatmap::draw_colorbar(int x, int y, int w, int h) {
        SDL_Renderer* rend = win_.get_renderer();

        for (int i = 0; i < h; i++) {
            double t = t_max_ - (t_max_ - t_min_) * i / (h - 1);
            Uint8 r, g, b;
            temp_to_rgb(t, r, g, b);

            SDL_SetRenderDrawColor(rend, r, g, b, 255);
            SDL_Rect rect = {x, y + i, w, 1};
            SDL_RenderFillRect(rend, &rect);
        }

        SDL_SetRenderDrawColor(rend, 200, 200, 200, 255);
        SDL_Rect border = {x, y, w, h};
        SDL_RenderDrawRect(rend, &border);

        std::ostringstream oss_max;
        oss_max << std::fixed << std::setprecision(0) << (t_max_ - 273.15) << " C";
        font_->render(rend, oss_max.str(), x + w + 5, y - 2, {200, 200, 200, 255});

        std::ostringstream oss_mid;
        oss_mid << std::fixed << std::setprecision(0) << ((t_min_ + t_max_) / 2 - 273.15) << " C";
        font_->render(rend, oss_mid.str(), x + w + 5, y + h / 2 - 7, {200, 200, 200, 255});

        std::ostringstream oss_min;
        oss_min << std::fixed << std::setprecision(0) << (t_min_ - 273.15) << " C";
        font_->render(rend, oss_min.str(), x + w + 5, y + h - 14, {200, 200, 200, 255});
    }

    void SDLHeatmap::draw_info(
        const std::string& material
        , double time
        , double tmax
        , bool paused
        , int x
        , int y
    ) {
        SDL_Renderer* rend = win_.get_renderer();

        std::ostringstream oss;
        oss << material << " | t = " << std::fixed << std::setprecision(2) << time
            << " / " << std::setprecision(1) << tmax << " s";
        if (paused) {
            oss << " [PAUSED]";
        }
        font_->render(rend, oss.str(), x, y, {255, 255, 255, 255});

        int progress_w = 200;
        double ratio = time / tmax;
        int fill_w = static_cast<int>(ratio * progress_w);

        SDL_SetRenderDrawColor(rend, 50, 50, 50, 255);
        SDL_Rect bg = {x, y + 20, progress_w, 8};
        SDL_RenderFillRect(rend, &bg);

        SDL_SetRenderDrawColor(rend, 80, 180, 80, 255);
        SDL_Rect fill = {x, y + 20, fill_w, 8};
        SDL_RenderFillRect(rend, &fill);

        SDL_SetRenderDrawColor(rend, 100, 100, 100, 255);
        SDL_RenderDrawRect(rend, &bg);
    }

    void SDLHeatmap::draw_heat_sources_1d(
        int bx
        , int by
        , int bw
        , int bh
    ) {
        SDL_Renderer* rend = win_.get_renderer();

        int x1_start = bx + static_cast<int>((1.0 / 10.0) * bw);
        int x1_end = bx + static_cast<int>((2.0 / 10.0) * bw);
        int x2_start = bx + static_cast<int>((5.0 / 10.0) * bw);
        int x2_end = bx + static_cast<int>((6.0 / 10.0) * bw);

        SDL_SetRenderDrawColor(rend, 255, 100, 50, 180);
        for (int i = 0; i < 3; ++i) {
            SDL_Rect r1 = {x1_start - i, by + bh + 5, x1_end - x1_start + 2 * i, 4};
            SDL_RenderFillRect(rend, &r1);
            SDL_Rect r2 = {x2_start - i, by + bh + 5, x2_end - x2_start + 2 * i, 4};
            SDL_RenderFillRect(rend, &r2);
        }

        int lx1 = x1_start + (x1_end - x1_start) / 2 - 12;
        int lx2 = x2_start + (x2_end - x2_start) / 2 - 12;
        int ly = by + bh + 10;

        SDL_SetRenderDrawColor(rend, 40, 40, 40, 220);
        SDL_Rect bg1 = {lx1 - 2, ly - 2, 28, 22};
        SDL_Rect bg2 = {lx2 - 2, ly - 2, 28, 22};
        SDL_RenderFillRect(rend, &bg1);
        SDL_RenderFillRect(rend, &bg2);

        label_font_->render(rend, "F1", lx1, ly, {255, 180, 100, 255});
        label_font_->render(rend, "F2", lx2, ly, {255, 180, 100, 255});
    }

    void SDLHeatmap::draw_heat_sources_2d(
        int px
        , int py
        , int ps
    ) {
        SDL_Renderer* rend = win_.get_renderer();

        auto draw_source = [&](double x0, double x1, double y0, double y1) {
            int sx0 = px + static_cast<int>(x0 * ps);
            int sx1 = px + static_cast<int>(x1 * ps);
            int sy0 = py + static_cast<int>(y0 * ps);
            int sy1 = py + static_cast<int>(y1 * ps);

            SDL_SetRenderDrawColor(rend, 255, 100, 50, 200);
            SDL_Rect border = {sx0, sy0, sx1 - sx0, sy1 - sy0};
            SDL_RenderDrawRect(rend, &border);
            SDL_Rect border2 = {sx0 + 1, sy0 + 1, sx1 - sx0 - 2, sy1 - sy0 - 2};
            SDL_RenderDrawRect(rend, &border2);
        };

        draw_source(1.0 / 6.0, 2.0 / 6.0, 1.0 / 6.0, 2.0 / 6.0);
        draw_source(4.0 / 6.0, 5.0 / 6.0, 1.0 / 6.0, 2.0 / 6.0);
        draw_source(1.0 / 6.0, 2.0 / 6.0, 4.0 / 6.0, 5.0 / 6.0);
        draw_source(4.0 / 6.0, 5.0 / 6.0, 4.0 / 6.0, 5.0 / 6.0);
    }

    void SDLHeatmap::draw_stats(
        const std::vector<double>& temps
        , int x
        , int y
    ) {
        if (temps.empty()) return;

        SDL_Renderer* rend = win_.get_renderer();

        auto [min_it, max_it] = std::minmax_element(temps.begin(), temps.end());
        double t_min = *min_it - 273.15;
        double t_max = *max_it - 273.15;
        double sum = 0.0;
        for (double t : temps) {
            sum += t;
        }
        double t_avg = sum / temps.size() - 273.15;

        std::ostringstream oss_min;
        oss_min << "Min: " << std::fixed << std::setprecision(1) << t_min << "C";

        std::ostringstream oss_max;
        oss_max << "Max: " << std::fixed << std::setprecision(1) << t_max << "C";

        std::ostringstream oss_avg;
        oss_avg << "Avg: " << std::fixed << std::setprecision(1) << t_avg << "C";

        int spacing = 120;
        font_->render(rend, oss_min.str(), x, y, {100, 180, 255, 255});
        font_->render(rend, oss_max.str(), x + spacing, y, {255, 120, 120, 255});
        font_->render(rend, oss_avg.str(), x + spacing * 2, y, {220, 220, 220, 255});
    }

    void SDLHeatmap::draw_stats_2d(
        const std::vector<std::vector<double>>& temps
        , int x
        , int y
    ) {
        if (temps.empty()) return;

        SDL_Renderer* rend = win_.get_renderer();

        double t_min = temps[0][0];
        double t_max = temps[0][0];
        double sum = 0.0;
        int count = 0;

        for (const auto& row : temps) {
            for (double t : row) {
                t_min = std::min(t_min, t);
                t_max = std::max(t_max, t);
                sum += t;
                ++count;
            }
        }

        t_min -= 273.15;
        t_max -= 273.15;
        double t_avg = sum / count - 273.15;

        std::ostringstream oss_min;
        oss_min << "Min: " << std::fixed << std::setprecision(1) << t_min << "C";

        std::ostringstream oss_max;
        oss_max << "Max: " << std::fixed << std::setprecision(1) << t_max << "C";

        std::ostringstream oss_avg;
        oss_avg << "Avg: " << std::fixed << std::setprecision(1) << t_avg << "C";

        int spacing = 120;
        font_->render(rend, oss_min.str(), x, y, {100, 180, 255, 255});
        font_->render(rend, oss_max.str(), x + spacing, y, {255, 120, 120, 255});
        font_->render(rend, oss_avg.str(), x + spacing * 2, y, {220, 220, 220, 255});
    }

    void SDLHeatmap::draw_boundary_markers_1d(
        int bx
        , int by
        , int bw
        , int bh
    ) {
        SDL_Renderer* rend = win_.get_renderer();

        SDL_SetRenderDrawColor(rend, 100, 200, 100, 255);
        SDL_Rect neumann = {bx - 5, by - 5, 5, bh + 10};
        SDL_RenderFillRect(rend, &neumann);

        SDL_SetRenderDrawColor(rend, 200, 100, 100, 255);
        SDL_Rect dirichlet = {bx + bw, by - 5, 5, bh + 10};
        SDL_RenderFillRect(rend, &dirichlet);

        SDL_SetRenderDrawColor(rend, 30, 30, 30, 220);
        SDL_Rect bg_n = {bx - 10, by - 22, 18, 22};
        SDL_Rect bg_d = {bx + bw - 5, by - 22, 18, 22};
        SDL_RenderFillRect(rend, &bg_n);
        SDL_RenderFillRect(rend, &bg_d);

        label_font_->render(rend, "N", bx - 8, by - 20, {100, 230, 100, 255});
        label_font_->render(rend, "D", bx + bw - 3, by - 20, {230, 100, 100, 255});
    }

    void SDLHeatmap::draw_boundary_markers_2d(
        int px
        , int py
        , int ps
    ) {
        SDL_Renderer* rend = win_.get_renderer();

        SDL_SetRenderDrawColor(rend, 100, 200, 100, 255);
        SDL_Rect neumann_left = {px - 5, py, 5, ps};
        SDL_RenderFillRect(rend, &neumann_left);
        SDL_Rect neumann_top = {px, py - 5, ps, 5};
        SDL_RenderFillRect(rend, &neumann_top);

        SDL_SetRenderDrawColor(rend, 200, 100, 100, 255);
        SDL_Rect dirichlet_right = {px + ps, py, 5, ps};
        SDL_RenderFillRect(rend, &dirichlet_right);
        SDL_Rect dirichlet_bottom = {px, py + ps, ps, 5};
        SDL_RenderFillRect(rend, &dirichlet_bottom);

        SDL_SetRenderDrawColor(rend, 30, 30, 30, 220);
        SDL_Rect bg_n = {px - 10, py + ps / 2 - 10, 18, 22};
        SDL_Rect bg_d = {px + ps, py + ps / 2 - 10, 18, 22};
        SDL_RenderFillRect(rend, &bg_n);
        SDL_RenderFillRect(rend, &bg_d);

        label_font_->render(rend, "N", px - 8, py + ps / 2 - 8, {100, 230, 100, 255});
        label_font_->render(rend, "D", px + ps + 2, py + ps / 2 - 8, {230, 100, 100, 255});
    }

    void SDLHeatmap::draw_heat_flow_1d(
        const std::vector<double>& temps
        , int bx
        , int by
        , int bw
        , int bh
    ) {
        if (temps.size() < 3) return;

        SDL_Renderer* rend = win_.get_renderer();
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

        int n = static_cast<int>(temps.size());
        int num_arrows = 10;
        int step = n / (num_arrows + 1);
        int arrow_y = by + bh + 25;

        for (int a = 1; a <= num_arrows; ++a) {
            int idx = a * step;
            if (idx <= 0 || idx >= n - 1) continue;

            double grad = temps[idx + 1] - temps[idx - 1];
            if (std::abs(grad) < 0.001) continue;

            int cx = bx + (idx * bw) / n;
            int arrow_len = 18;
            int dir = (grad < 0) ? 1 : -1;

            SDL_SetRenderDrawColor(rend, 255, 200, 100, 255);

            int x1 = cx - dir * arrow_len / 2;
            int x2 = cx + dir * arrow_len / 2;
            SDL_RenderDrawLine(rend, x1, arrow_y, x2, arrow_y);
            SDL_RenderDrawLine(rend, x1, arrow_y + 1, x2, arrow_y + 1);

            int head_size = 6;
            SDL_RenderDrawLine(rend, x2, arrow_y, x2 - dir * head_size, arrow_y - head_size);
            SDL_RenderDrawLine(rend, x2, arrow_y, x2 - dir * head_size, arrow_y + head_size);
        }
    }

    void SDLHeatmap::draw_heat_flow_2d(
        const std::vector<std::vector<double>>& temps
        , int px
        , int py
        , int ps
    ) {
        int ny = static_cast<int>(temps.size());
        if (ny < 3) return;
        int nx = static_cast<int>(temps[0].size());
        if (nx < 3) return;

        SDL_Renderer* rend = win_.get_renderer();
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

        int grid_arrows = 6;
        int step_x = nx / (grid_arrows + 1);
        int step_y = ny / (grid_arrows + 1);

        for (int ay = 1; ay <= grid_arrows; ++ay) {
            for (int ax = 1; ax <= grid_arrows; ++ax) {
                int j = ay * step_y;
                int i = ax * step_x;

                if (i <= 0 || i >= nx - 1 || j <= 0 || j >= ny - 1) continue;

                double gx = temps[j][i + 1] - temps[j][i - 1];
                double gy = temps[j + 1][i] - temps[j - 1][i];

                double mag = std::sqrt(gx * gx + gy * gy);
                if (mag < 0.001) continue;

                gx = -gx / mag;
                gy = -gy / mag;

                int cx = px + (i * ps) / nx;
                int cy = py + (j * ps) / ny;
                int arrow_len = 14;

                int x2 = cx + static_cast<int>(gx * arrow_len);
                int y2 = cy + static_cast<int>(gy * arrow_len);

                SDL_SetRenderDrawColor(rend, 255, 200, 100, 220);

                SDL_RenderDrawLine(rend, cx, cy, x2, y2);

                double angle = std::atan2(gy, gx);
                int head = 5;
                double a1 = angle + 2.6;
                double a2 = angle - 2.6;
                SDL_RenderDrawLine(rend, x2, y2
                    , x2 + static_cast<int>(head * std::cos(a1))
                    , y2 + static_cast<int>(head * std::sin(a1)));
                SDL_RenderDrawLine(rend, x2, y2
                    , x2 + static_cast<int>(head * std::cos(a2))
                    , y2 + static_cast<int>(head * std::sin(a2)));
            }
        }
    }

}
