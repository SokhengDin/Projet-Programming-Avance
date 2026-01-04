/**
 * @file material.hpp
 * @brief Definition of physical material properties for heat simulations.
 */

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>

namespace ensiie {
/**
 * @struct Material
 * @brief Physical properties of a material used in heat simulations.
 */
struct Material {
    std::string name;    ///< Material name
    double lambda;       ///< Thermal conductivity W/(mK)
    double rho;          ///< Density kg/m^{3}
    double c;            ///< Specific heat J/(kgK)

    /**
     * @brief Compute thermal diffusivity.
     * @return Thermal diffusivity α = λ / (ρc) in m²/s
     */
    double alpha() const { return lambda / (rho * c); }
};

/**
 * @namespace Materials
 * @brief Predefined common materials.
 */
namespace Materials {
    const Material COPPER      = {"Cuivre",      389.0, 8940.0,  380.0};
    const Material IRON        = {"Fer",          80.2, 7874.0,  440.0};
    const Material GLASS       = {"Verre",         1.2, 2530.0,  840.0};
    const Material POLYSTYRENE = {"Polystyrène",   0.1, 1040.0, 1200.0};
}

} // namespace ensiie

#endif