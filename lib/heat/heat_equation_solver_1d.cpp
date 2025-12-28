#include "heat_equation_solver_1d.hpp"
#include <cmath>

/// Celsius to Kelvin conversion
constexpr double KELVIN_OFFSET = 273.15;

namespace ensiie {
    HeatEquationSolver1D::HeatEquationSolver1D(
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
    , dt_(tmax / 1000.0)  // 1001 time points
    , u0_kelvin_(u0 + KELVIN_OFFSET)
    , t_(0.0)
    , n_(n)
    , u_(n, u0_kelvin_)
    , F_(n, 0.0)
    {
        init_source(f);
    }

    void HeatEquationSolver1D::init_source(double f) {
        // F(x) = tmax * f^2       on [L/10, 2L/10]
        // F(x) = (3/4)*tmax*f^2   on [5L/10, 6L/10]
        // F(x) = 0                otherwise
        double f1 = tmax_ * f * f;
        double f2 = 0.75 * tmax_ * f * f;

    
        for (int i = 0; i < n_; i++) {
            double x = i * dx_;

            // first source
            if ( x >= L_ / 10.0 && x <= 2*L_ / 10.0) {
                F_[i] = f1;
            }
            // second source
            else if (x >= 5.0 * L_ / 10.0 && x <= 6.0 * L_ / 10.0) {
                F_[i] = f2;
            } else {
                F_[i] = 0.0;
            }
        }
    }

    bool HeatEquationSolver1D::step() 
    {
        // Check if simulation is done
        if (t_ >= tmax_) {
            return false;
        }

        // Thermal diffusion alpha = lambda / (rho * c)
        double alpha = mat_.alpha();

        // Rate condutivity r = alpha * delta_t / delta_x
        double r     = alpha * dt_ / (dx_ * dx_);

        // Coefficient
        double coef  = dt_ / (mat_.rho * mat_.c);

        // Build tridiagonal system for implicit scheme:
        // -r*u[i-1]^{n+1} + (1+2r)*u[i]^{n+1} - r*u[i+1]^{n+1} = u[i]^n + Δt/(ρc)*F[i]
        //
        // Matrix form: A * u^{n+1} = d
        // where A is tridiagonal with:
        //   a[i] = -r        (lower diagonal)
        //   b[i] = 1 + 2r    (main diagonal)
        //   c[i] = -r        (upper diagonal)
        std::vector<double> a(n_, -r);
        std::vector<double> b(n_, 1.0 + 2.0 * r);
        std::vector<double> c(n_, -r);
        std::vector<double> d(n_);

        // RHS: d[i] = u[i]^n + delta_t/(rho * c) * F[i]
        for (int i = 0; i < n_; i++) {
            d[i] = u_[i] + coef * F_[i];
        }

        // Boundary conditions

        /// Neumann conditions
        b[0] = 1.0 + r;
        c[0] = -r;

        /// Dirchlet conditions
        b[n_ - 1] = 1.0;
        a[n_ - 1] = 0.0;
        c[n_ - 1] = 0.0;
        d[n_ - 1] = u0_kelvin_;

        // Solve equation
        std::vector<double> u_sol(n_);
        solve_tridiagonal(a, b, c, d, u_sol);

        // update solution
        u_ = u_sol;

        // indexing tiume
        t_ += dt_;

        return true;
    }


    void HeatEquationSolver1D::solve_tridiagonal(
        const std::vector<double>& a
        , const std::vector<double>& b
        , const std::vector<double>& c
        , std::vector<double>& d
        , std::vector<double>& x 
    ) {
        // Thomas algorithm (TDMA - TriDiagonal Matrix Algorithm)
        // Solves: a[i]*x[i-1] + b[i]*x[i] + c[i]*x[i+1] = d[i]

        int n = static_cast<int>(b.size());

        // Temp array forward
        std::vector<double> c_prime(n);
        std::vector<double> d_prime(n);

        // Forward
        c_prime[0] = c[0] / b[0];
        d_prime[0] = d[0] / b[0];

        // Iterate over spatial points
        for (int i = 1; i < n; i++)
        {
            double denom = b[i] - a[i] * c_prime[i - 1];
            c_prime[i]   = c[i] / denom;
            d_prime[i]   = (d[i] - a[i] * d_prime[i - 1]) / denom;
        }

        // Back sub
        x[n-1]  = d_prime[n-1];

        for (int i = n - 2; i >= 0; --i) {
            x[i] = d_prime[i] - c_prime[i] * x[i+1];
        }
    }

    void HeatEquationSolver1D::reset() {
        t_ = 0.0;
        std::fill(u_.begin(), u_.end(), u0_kelvin_);
    }

}