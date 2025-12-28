#ifndef HEAT_EQUATION_SOLVER_2D_HPP
#define HEAT_EQUATION_SOLVER_2D_HPP

#include "material.hpp"
#include <vector>

namespace ensiie {
    /**
     * @class HeatEquationSolver2D
     * @brief Solves 2D heat equation with implicit finite differences
     * 
     * Boundary conditions:
     * - Neumann at x=0, y=0
     * - Dirichlet at x=L, y=L
     */

    class HeatEquationSolver2D {
        private:
            Material mat_;              ///< Material properties
            double L_;                  ///< Plate side length
            double tmax_;               ///< Max simulation time
            double dx_;                 ///< Spatial step
            double dt_;                 ///< Time step
            double u0_kelvin_;          ///< Initial temp in Kelvin
            double t_;                  ///< Current time

            int n_;                     ///< Number of points per dimension

            std::vector<double> u_;     ///< Temperature field (row-major)
            std::vector<double> F_;     ///< Heat source term

            /**
             * @brief Convert 2D index to 1D
             * @param i X index
             * @param j Y index
             * @return Linear index
             */
            int idx(int i, int j) const { return j * n_ + i; }

            /**
             * @brief Initialize heat source F(x,y)
             * @param f Heat source amplitude (Celsius)
             * 
             * Four heat sources at symmetric positions:
             * F(x,y) = tmax * f**2 on [L/6, 2L/6] x [L/6, 2L/6]
             * F(x,y) = tmax * f**2 on [4L/6, 5L/6] x [L/6, 2L/6]
             * F(x,y) = tmax * f**2 on [L/6, 2L/6] x [4L/6, 5L/6]
             * F(x,y) = tmax * f**2 on [4L/6, 5L/6] x [4L/6, 5L/6]
             * F(x,y) = 0 otherwise
             */
            void init_source(double f);

        public:
            /**
             * @brief Constructor
             * @param mat Material Properties
             * @param L Side length of square plate (m)
             * @param tmax Maximum simulation time (s)
             * @param u0 Initial temperature (Celsius)
             * @param f Heat source amplitude (Celsius)
             * @param n Number of points per dimension
             */
            HeatEquationSolver2D(
                const Material& mat
                , double L
                , double tmax
                , double u0
                , double f
                , int n
            );

            /**
             * @brief Solution by one time step
             * @return true if simulation continues, false if finished
             */
            bool step();

            /**
             * @brief Get temperature at grid point
             * @param i X index
             * @param j Y index
             * @return Temperature in Kelvin
             */
            double get_temperature(int i, int j) const { return u_[idx(i, j)]; }

            /**
             * @brief Get temperature field as 2D vector
             * @return 2D temperature array [row][col] in Kelvin
             */
            std::vector<std::vector<double>> get_temperature_2d() const;

            /**
             * @brief Get current simulation time
             * @return Time in seconds
             */
            double get_time() const { return t_; }

            /**
             * @brief Get maximum simulation time
             * @return tmax in seconds
             */
            double get_tmax() const { return tmax_; }

            /**
             * @brief Get number of points per dimension
             */
            int get_n() const { return n_; }

            /**
             * @brief Reset simulation to initial state
             */
            void reset();
    };
}

#endif