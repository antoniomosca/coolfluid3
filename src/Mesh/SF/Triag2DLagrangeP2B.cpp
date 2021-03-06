// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/CBuilder.hpp"

#include "LibSF.hpp"
#include "Triag2DLagrangeP2B.hpp"
#include "Line2DLagrangeP1.hpp"
#include "SFTriagLagrangeP2B.hpp"

namespace CF {
namespace Mesh {
namespace SF {

////////////////////////////////////////////////////////////////////////////////

Common::ComponentBuilder < Triag2DLagrangeP2B, ElementType, LibSF > aTriag2DLagrangeP2B_Builder;

////////////////////////////////////////////////////////////////////////////////

Triag2DLagrangeP2B::Triag2DLagrangeP2B(const std::string& name) : Triag2D(name)
{
  m_nb_nodes = nb_nodes;
  m_order = order;
}

Real Triag2DLagrangeP2B::compute_volume(const NodesT& coord) const
{
  return volume(coord);
}
	
bool Triag2DLagrangeP2B::is_coord_in_element(const RealVector& coord, const NodesT& nodes) const
{
	return in_element(coord,nodes);
}

const CF::Mesh::ElementType::FaceConnectivity& Triag2DLagrangeP2B::faces()
{
  static FaceConnectivity connectivity;

  if(connectivity.face_first_nodes.empty())
  {
    connectivity.face_node_counts.assign(nb_nodes, 3);
    connectivity.face_first_nodes = boost::assign::list_of(0)(3)(6);
    connectivity.face_nodes = boost::assign::list_of(0)(1)(3)
                                                    (1)(2)(4)
                                                    (2)(0)(5);
  }
  return connectivity;
}

const CF::Mesh::ElementType::FaceConnectivity& Triag2DLagrangeP2B::face_connectivity() const
{
  return faces();
}

const CF::Mesh::ElementType& Triag2DLagrangeP2B::face_type(const CF::Uint face) const
{
  throw Common::NotImplemented( FromHere(), "Line2DLagrangeP2 does not exist yet" );

  static const Line2DLagrangeP1 facetype;
  return facetype;
}

////////////////////////////////////////////////////////////////////////////////

void Triag2DLagrangeP2B::shape_function_value(const MappedCoordsT& map_coord, ShapeFunctionsT& shapef)
{
  SFTriagLagrangeP2B::compute_value(map_coord,shapef);
}

////////////////////////////////////////////////////////////////////////////////

void Triag2DLagrangeP2B::mapped_coordinates(const CoordsT& coord, const NodeMatrixT& nodes, MappedCoordsT& map_coord)
{
  throw Common::NotImplemented( FromHere(), "" );

    cf_assert(coord.size() == 2);
    cf_assert(map_coord.size() == 2);
    cf_assert(nodes.size() == 7);
  
  const Real invDet = 1. / jacobian_determinant(nodes);

  map_coord[KSI] = invDet * ((nodes(2, YY) - nodes(0, YY))*coord[XX] + (nodes(0, XX) - nodes(2, XX))*coord[YY] - nodes(0, XX)*nodes(2, YY) + nodes(2, XX)*nodes(0, YY));
  map_coord[ETA] = invDet * ((nodes(0, YY) - nodes(1, YY))*coord[XX] + (nodes(1, XX) - nodes(0, XX))*coord[YY] + nodes(0, XX)*nodes(1, YY) - nodes(1, XX)*nodes(0, YY));

}

////////////////////////////////////////////////////////////////////////////////

void Triag2DLagrangeP2B::shape_function_gradient(const MappedCoordsT& map_coord, MappedGradientT& result)
{
  SFTriagLagrangeP2B::compute_gradient(map_coord,result);
}

////////////////////////////////////////////////////////////////////////////////

Real Triag2DLagrangeP2B::jacobian_determinant(const MappedCoordsT& map_coord, const NodeMatrixT& nodes)
{
  return jacobian_determinant(nodes);
}

////////////////////////////////////////////////////////////////////////////////

void Triag2DLagrangeP2B::jacobian(const MappedCoordsT& map_coord, const NodeMatrixT& nodes, JacobianT& result)
{

    /// @todo function Triag2DLagrangeP2B::jacobian needs to be updated for curvilinear elements

  result(KSI,XX) = nodes(1, XX) - nodes(0, XX);
  result(KSI,YY) = nodes(1, YY) - nodes(0, YY);
  result(ETA,XX) = nodes(2, XX) - nodes(0, XX);
  result(ETA,YY) = nodes(2, YY) - nodes(0, YY);
}

////////////////////////////////////////////////////////////////////////////////

void Triag2DLagrangeP2B::jacobian_adjoint(const MappedCoordsT& map_coord, const NodeMatrixT& nodes, JacobianT& result)
{
  throw Common::NotImplemented( FromHere(), "" );

}

////////////////////////////////////////////////////////////////////////////////

Real Triag2DLagrangeP2B::volume(const NodeMatrixT& nodes)
{
  throw Common::NotImplemented( FromHere(), "" );
}

////////////////////////////////////////////////////////////////////////////////

bool Triag2DLagrangeP2B::in_element(const CoordsT& coord, const NodeMatrixT& nodes)
{
  throw Common::NotImplemented( FromHere(), "" );

  MappedCoordsT mapped_coord;
  mapped_coordinates(coord, nodes, mapped_coord);
  if( (mapped_coord[KSI] >= -Math::Consts::eps()) &&
      (mapped_coord[ETA] >= -Math::Consts::eps()) &&
      (mapped_coord.sum() <= 1.))
  {
    return true;
  }
  else
  {
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////

Real Triag2DLagrangeP2B::jacobian_determinant(const NodeMatrixT& nodes)
{
  throw Common::NotImplemented( FromHere(), "" );

  const Real x0 = nodes(0, XX);
  const Real x1 = nodes(1, XX);
  const Real x2 = nodes(2, XX);
  const Real y0 = nodes(0, YY);
  const Real y1 = nodes(1, YY);
  const Real y2 = nodes(2, YY);

  return (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0);
}

////////////////////////////////////////////////////////////////////////////////

} // SF
} // Mesh
} // CF
