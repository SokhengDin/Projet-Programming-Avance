/**
 * @file heat_equation_solver.cpp
 * @brief Implementation of 1D/2D heat equation solvers
 */

#include "heat_equation_solver.hpp"
#include <cmath>
#include <algorithm>

constexpr double KELVIN_OFFSET = 273.15;

namespace ensiie {

// =============================================================================
// 1D SOLVER
// =============================================================================

HeatEquationSolver1D::HeatEquationSolver1D(
    const Material& mat,
    double L,
    double tmax,
    double u0,
    double f,
    int n
)
    : mat_(mat)
    , L_(L)
    , tmax_(tmax)
    , dx_(L / (n - 1))
    , dt_(tmax / 1000.0)
    , u0_kelvin_(u0 + KELVIN_OFFSET)
    , t_(0.0)
    , n_(n)
    , u_(n, u0_kelvin_)
    , F_(n, 0.0)
{
    init_source(f);
}

void HeatEquationSolver1D::init_source(double f) {
    // Source regions: [L/10, 2L/10] and [5L/10, 6L/10]
    double f1 = tmax_ * f * f;
    double f2 = 0.75 * tmax_ * f * f;

    for (int i = 0; i < n_; i++) {
        double x = i * dx_;
        if (x >= L_ / 10.0 && x <= 2.0 * L_ / 10.0) {
            F_[i] = f1;
        } else if (x >= 5.0 * L_ / 10.0 && x <= 6.0 * L_ / 10.0) {
            F_[i] = f2;
        } else {
            F_[i] = 0.0;
        }
    }
}

bool HeatEquationSolver1D::step() {
    if (t_ >= tmax_) return false;

    // Implicit scheme coefficients
    double alpha = mat_.alpha();
    double r = alpha * dt_ / (dx_ * dx_);
    double coef = dt_ / (mat_.rho * mat_.c);

    std::vector<double> a(n_, -r);
    std::vector<double> b(n_, 1.0 + 2.0 * r);
    std::vector<double> c(n_, -r);
    std::vector<double> d(n_);

    // RHS
    for (int i = 0; i < n_; i++) {
        d[i] = u_[i] + coef * F_[i];
    }

    // Neumann BC at x=0
    b[0] = 1.0 + r;
    c[0] = -r;

    // Dirichlet BC at x=L
    b[n_ - 1] = 1.0;
    a[n_ - 1] = 0.0;
    c[n_ - 1] = 0.0;
    d[n_ - 1] = u0_kelvin_;

    std::vector<double> u_new(n_);
    solve_tridiagonal(a, b, c, d, u_new);

    u_ = u_new;
    t_ += dt_;
    return true;
}

void HeatEquationSolver1D::solve_tridiagonal(
    const std::vector<double>& a,
    const std::vector<double>& b,
    const std::vector<double>& c,
    std::vector<double>& d,
    std::vector<double>& x
) {
    // Thomas algorithm (TDMA)
    int n = static_cast<int>(b.size());
    std::vector<double> c_prime(n);
    std::vector<double> d_prime(n);

    // Forward elimination
    c_prime[0] = c[0] / b[0];
    d_prime[0] = d[0] / b[0];

    for (int i = 1; i < n; i++) {
        double denom = b[i] - a[i] * c_prime[i - 1];
        c_prime[i] = c[i] / denom;
        d_prime[i] = (d[i] - a[i] * d_prime[i - 1]) / denom;
    }

    // Back substitution
    x[n - 1] = d_prime[n - 1];
    for (int i = n - 2; i >= 0; --i) {
        x[i] = d_prime[i] - c_prime[i] * x[i + 1];
    }
}

void HeatEquationSolver1D::reset() {
    t_ = 0.0;
    std::fill(u_.begin(), u_.end(), u0_kelvin_);
}


// =============================================================================
// 2D SOLVER
// =============================================================================

HeatEquationSolver2D::HeatEquationSolver2D(
    const Material& mat,
    double L,
    double tmax,
    double u0,
    double f,
    int n
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

void HeatEquationSolver2D::init_source(double f) {
    // Four symmetric sources at corners
    double f_val = tmax_ * f * f;

    for (int j = 0; j < n_; j++) {
        for (int i = 0; i < n_; i++) {
            double x = i * dx_;
            double y = j * dx_;

            bool in_source = false;

            // Bottom-left
            if (x >= L_/6.0 && x <= 2.0*L_/6.0 && y >= L_/6.0 && y <= 2.0*L_/6.0)
                in_source = true;
            // Bottom-right
            if (x >= 4.0*L_/6.0 && x <= 5.0*L_/6.0 && y >= L_/6.0 && y <= 2.0*L_/6.0)
                in_source = true;
            // Top-left
            if (x >= L_/6.0 && x <= 2.0*L_/6.0 && y >= 4.0*L_/6.0 && y <= 5.0*L_/6.0)
                in_source = true;
            // Top-right
            if (x >= 4.0*L_/6.0 && x <= 5.0*L_/6.0 && y >= 4.0*L_/6.0 && y <= 5.0*L_/6.0)
                in_source = true;

            F_[idx(i, j)] = in_source ? f_val : 0.0;
        }
    }
}

bool HeatEquationSolver2D::step() {
    if (t_ >= tmax_) return false;

    // Implicit scheme with 5-point stencil
    double alpha = mat_.alpha();
    double r = alpha * dt_ / (dx_ * dx_);
    double src_coef = dt_ / (mat_.rho * mat_.c);

    std::vector<double> u_new = u_;

    // Gauss-Seidel parameters
    const int max_iter = 100;
    const double tol = 1e-6;

    for (int iter = 0; iter < max_iter; iter++) {
        double max_diff = 0.0;

        for (int j = 0; j < n_; ++j) {
            for (int i = 0; i < n_; ++i) {
                // Dirichlet BC at right and top edges
                if (i == n_ - 1 || j == n_ - 1) {
                    u_new[idx(i, j)] = u0_kelvin_;
                    continue;
                }

                double old_val = u_new[idx(i, j)];

                // Neighbors with Neumann BC (mirror at i=0, j=0)
                double u_left  = (i > 0) ? u_new[idx(i-1, j)] : u_new[idx(1, j)];
                double u_right = u_new[idx(i+1, j)];
                double u_down  = (j > 0) ? u_new[idx(i, j-1)] : u_new[idx(i, 1)];
                double u_up    = u_new[idx(i, j+1)];

                double rhs = u_[idx(i, j)] + src_coef * F_[idx(i, j)];
                u_new[idx(i, j)] = (rhs + r * (u_left + u_right + u_down + u_up)) / (1.0 + 4.0 * r);

                max_diff = std::max(max_diff, std::abs(u_new[idx(i, j)] - old_val));
            }
        }

        if (max_diff < tol) break;
    }

    u_ = u_new;
    t_ += dt_;
    return true;
}

std::vector<std::vector<double>> HeatEquationSolver2D::get_temperature_2d() const {
    std::vector<std::vector<double>> result(n_, std::vector<double>(n_));
    for (int j = 0; j < n_; j++) {
        for (int i = 0; i < n_; i++) {
            result[j][i] = u_[idx(i, j)];
        }
    }
    return result;
}

void HeatEquationSolver2D::reset() {
    t_ = 0.0;
    std::fill(u_.begin(), u_.end(), u0_kelvin_);
}

} // namespace ensiie
