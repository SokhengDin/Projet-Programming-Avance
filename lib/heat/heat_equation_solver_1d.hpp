#ifndef HEAT_EQUATION_SOLVER_1D
#define HEAT_EQUATION_SOLVER_1D

#include "material.hpp"
#include <vector>

namespace ensiie {
    /**
     * @class HeatEquationSolver1D
     * @brief This class solve 1D heat equation in finite differences
     */

    class HeatEquationSolver1D
    {
        private:
            Material mat_;              ///< Material properties
            double L_;                  ///< Bar length
            double tmax_;               ///< Max simulation time
            double dx_;                 ///< Spatial step
            double dt_;                 ///< Time step
            double u0_kelvin_;          ///< Initial temp in Kelvin
            double t_;                  ///< Current time

            int n_;                     ///< Number of spatial points

            std::vector<double> u_;     ///< Temperature field
            std::vector<double> F_;     ///< Heat source term

            /**
             * @brief Initialize heat source F(x)
             * @param f Heat source amplitude (Celsius)
             * 
             * F(x) = tmax * f**2 on [L/10, 2L/10]
             * F(x) = (3/4) * tmax * f**2 on [5L/10, 6L/10]
             * F(x) = 0 otherwise
             */
            void init_source(double f);

            /**
             * @brief Solve tridiagonal system (Thomas Algorithms)
             */
            void solve_tridiagonal(
                const std::vector<double>& a
                , const std::vector<double>& b
                , const std::vector<double>& c
                , std::vector<double>& d 
                , std::vector<double>& x
            );


        public:
            /**
             * @brief Constructor
             * @param mat Material Properties
             * @param L Lenght of bar unit (m)
             * @param tmax Maximum simulation time (s)
             * @param n Number of spatial points 
             * @param u0 Initial temperature unit (Celsius)
             * @param f Heat source temperature amplitude (Celsius)
             */
            HeatEquationSolver1D(
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
             * @brief Get current temperature distribution
             * @return Vector of temperature in Kelvin
             */
            const std::vector<double>& get_temperature() const { return u_; }

            /**
             * @brief Get current simulation time 
             * @return Time in seconds
             */
            double get_time() const { return t_;}

            /**
             * @brief Get number of spatial point
             */
            int get_n() const { return n_; }


            /**
             * @brief Reset simulation to initial state
             */

            void reset();

    };
}
#endif