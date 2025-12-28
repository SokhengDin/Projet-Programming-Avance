#include "heat_equation_solver_2d.hpp"
#include <algorithm>
#include <cmath>


/// Celsius to Kelvin conversion
constexpr double KELVIN_OFFSET = 273.15;

namespace ensiie {
    HeatEquationSolver2D::HeatEquationSolver2D(
        const Material& mat
        , double L
        , double tmax
        , double u0
        , double f
        , int n
    )
    : mat_(mat)
    , L_(L)
    , tmax_(tmax)
    , dx_(L / (n - 1))
    , dt_(tmax / 1000.0)
    , u0_kelvin_(u0 + KELVIN_OFFSET)
    , t_(0.0)
    , n_(n)
    , u_(n * n, u0_kelvin_)
    , F_(n * n, 0.0)
    {
        init_source(f);   
    }

    void HeatEquationSolver2D::init_source(
        double f
    )
    {
        double f_val = tmax_ * f * f;

        for (int i = 0; i < n_; i++)
        {
            for (int j = 0; j < n_; j++) {
                double x = i * dx_;
                double y = j * dx_;

                bool in_source = false;

                // Source 1: [L/6, 2L/6] x [L/6, 2L/6]
                if (x >= L_ / 6.0 && x <= 2.0 * L_ / 6.0 &&
                    y >= L_ / 6.0 && y <= 2.0 * L_ / 6.0) {
                    in_source = true;
                }
                // Source 2: [4L/6, 5L/6] x [L/6, 2L/6]
                if (x >= 4.0 * L_ / 6.0 && x <= 5.0 * L_ / 6.0 &&
                    y >= L_ / 6.0 && y <= 2.0 * L_ / 6.0) {
                    in_source = true;
                }
                // Source 3: [L/6, 2L/6] x [4L/6, 5L/6]
                if (x >= L_ / 6.0 && x <= 2.0 * L_ / 6.0 &&
                    y >= 4.0 * L_ / 6.0 && y <= 5.0 * L_ / 6.0) {
                    in_source = true;
                }
                // Source 4: [4L/6, 5L/6] x [4L/6, 5L/6]
                if (x >= 4.0 * L_ / 6.0 && x <= 5.0 * L_ / 6.0 &&
                    y >= 4.0 * L_ / 6.0 && y <= 5.0 * L_ / 6.0) {
                    in_source = true;
                }

                F_[idx(i, j)] = in_source ? f_val : 0.0;

            }
        }
    }

    bool HeatEquationSolver2D::step() {
        if ( t_ >= tmax_) {
            return false;
        }

        double alpha    = mat_.alpha();
        double r        = alpha * dt_ / (dx_ * dx_);
        double src_coef = dt_ / (mat_.rho * mat_.c);

        std::vector<double> u_sol = u_;

        const int max_iter = 100;
        const double tol = 1e-6;

        for (int iter = 0; iter < max_iter; iter++)
        {
            double max_diff = 0.0;

            for (int j = 0; j < n_; ++j) {
                for (int i = 0; i < n_; ++i) {
                    // Dirichlet BC at x=L or y=L
                    if (i == n_ - 1 || j == n_ - 1) {
                        u_sol[idx(i, j)] = u0_kelvin_;
                        continue;
                    }

                    double old_val = u_sol[idx(i, j)];

                    // Neighbors with Neumann BC at i=0, j=0
                    double u_left  = (i > 0) ? u_sol[idx(i - 1, j)] : u_sol[idx(1, j)];
                    double u_right = u_sol[idx(i + 1, j)];
                    double u_down  = (j > 0) ? u_sol[idx(i, j - 1)] : u_sol[idx(i, 1)];
                    double u_up    = u_sol[idx(i, j + 1)];

                    double rhs     = u_[idx(i, j)] + src_coef * F_[idx(i, j)];
                    u_sol[idx(i, j)] = (rhs + r * (u_left + u_right + u_down + u_up)) 
                                       / (1.0 + 4.0 * r);

                    max_diff = std::max(max_diff, std::abs(u_sol[idx(i, j)] - old_val));
                }
            }

            if (max_diff < tol) {
                break;
            }
        }

        u_ = u_sol;
        t_ += dt_;

        return true;
    }

    std::vector<std::vector<double>> HeatEquationSolver2D::get_temperature_2d() const {
        std::vector<std::vector<double>> result(n_, std::vector<double>(n_));

        for ( int j = 0; j < n_; j++) {
            for (int i = 0; i < n_; i++) {
                result[j][i] = u_[idx(i, j)];
            }
        }
        
        return result;
    }

    void HeatEquationSolver2D::reset()
    {
        t_ = 0.0;
        std::fill(u_.begin(), u_.end(), u0_kelvin_);
    }
}