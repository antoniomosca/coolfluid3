// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/Foreach.hpp"
#include "Common/CBuilder.hpp"

#include "Solver/Actions/Proto/CProtoElementsAction.hpp"
#include "Solver/Actions/Proto/CProtoNodesAction.hpp"

#include "Solver/CEigenLSS.hpp"

#include "HeatConductionLinearUnsteady.hpp"

namespace CF {
namespace UFEM {

using namespace Common;
using namespace Mesh;
using namespace Solver;
using namespace Solver::Actions;
using namespace Solver::Actions::Proto;

CF::Common::ComponentBuilder < HeatConductionLinearUnsteady, LinearSystem, LibUFEM > aHeatConductionLinearUnsteady_Builder;

HeatConductionLinearUnsteady::HeatConductionLinearUnsteady(const std::string& name) : LinearSystemUnsteady(name)
{
}

void HeatConductionLinearUnsteady::add_actions()
{
  MeshTerm<0, ScalarField> temperature("Temperature", "T");
  MeshTerm<1, ScalarField> heat("Heat", "q");
  
  StoredReference<Real> alpha = add_configurable_constant("alpha", "Thermal diffusivity (m2/s)", 1.);
  StoredReference<Real> k = add_configurable_constant("k", "Thermal conductivity (W/(mK))", 1.);
  
  append_elements_action
  (
    "HeatEquation",
    ASSEMBLY,
    group <<
    (
      _A(temperature) = alpha * integral<1>(laplacian_elm(temperature) * jacobian_determinant),
      _T(temperature) = integral<1>(value_elm(temperature) * jacobian_determinant), // note: we skip multiplying by invdt() so we can reuse this in the source terms
      system_matrix( lss() ) += invdt() * _T + 0.5 * _A,
      system_rhs( lss() )    += (boost::proto::lit(alpha) / k) * _T * heat - _A * temperature
    )
  );
}

} // UFEM
} // CF
