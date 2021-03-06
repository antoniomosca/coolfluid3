// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/CBuilder.hpp"

#include "LibSF.hpp"
#include "SFLineLagrangeP0.hpp"

namespace CF {
namespace Mesh {
namespace SF {

////////////////////////////////////////////////////////////////////////////////

Common::ComponentBuilder < SFLineLagrangeP0, ShapeFunction, LibSF > SFLineLagrangeP0_Builder;

////////////////////////////////////////////////////////////////////////////////

SFLineLagrangeP0::SFLineLagrangeP0(const std::string& name) : ShapeFunction(name)
{
  m_dimensionality = dimensionality;
  m_nb_nodes = nb_nodes;
  m_order = order;
  m_shape = shape;
}

////////////////////////////////////////////////////////////////////////////////

void SFLineLagrangeP0::compute_value(const MappedCoordsT& mapped_coord, ValueT& result)
{
  result[0] = 1.0;
}

////////////////////////////////////////////////////////////////////////////////

void SFLineLagrangeP0::compute_gradient(const MappedCoordsT& mappedCoord, GradientT& result)
{
  result(KSI, 0) = 0.;
}

////////////////////////////////////////////////////////////////////////////////

RealMatrix SFLineLagrangeP0::s_mapped_sf_nodes =  ( RealMatrix(1,1) <<
    0.
).finished();

////////////////////////////////////////////////////////////////////////////////

} // SF
} // Mesh
} // CF
