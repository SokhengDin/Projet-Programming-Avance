#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>

namespace ensiie {
/**
 * @brief Physical properties of materials for heat simulation
 */
struct Material {
    std::string name;    ///< Material name
    double lambda;       ///< Thermal conductivity W/(mK)
    double rho;          ///< Density kg/m^{3}
    double c;            ///< Specific heat J/(kgK)

    /**
     * @brief Compute thermal diffusivity
     * @return Thermal diffusivity in m^2/s
     */
    double alpha() const { return lambda / (rho * c); }
};

// Predefined materials
namespace Materials {
    const Material COPPER      = {"Cuivre",      389.0, 8940.0,  380.0};
    const Material IRON        = {"Fer",          80.2, 7874.0,  440.0};
    const Material GLASS       = {"Verre",         1.2, 2530.0,  840.0};
    const Material POLYSTYRENE = {"Polystyrène",   0.1, 1040.0, 1200.0};
}
}

#endif