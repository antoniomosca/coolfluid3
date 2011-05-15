// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_AdvectionDiffusion_State_hpp
#define CF_AdvectionDiffusion_State_hpp

#include "Solver/State.hpp"
#include "AdvectionDiffusion/Physics.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace AdvectionDiffusion {

////////////////////////////////////////////////////////////////////////////////

/// @brief Base class to interface the State
/// @author Willem Deconinck
class AdvectionDiffusion_API State : public Solver::State {

public: // typedefs

  typedef boost::shared_ptr<State> Ptr;
  typedef boost::shared_ptr<State const> ConstPtr;

public: // functions

  /// Contructor
  State( const std::string& name = type_name() ) : Solver::State(name)
  {
    m_var_names.resize(1,"Q");
  }
  
  /// Virtual destructor
  virtual ~State() {}

  /// Gets the Class name
  static std::string type_name() { return "State"; }

  virtual Solver::Physics create_physics()
  {
    AdvectionDiffusion::Physics p;
    return static_cast<Solver::Physics>(p);
  }

  virtual Uint size() { return 1; }

  virtual void set_state( const RealVector& state, Solver::Physics& p)
  {
    p.init();
    p.set_var(AdvectionDiffusion::Physics::Vx, 1.);
    p.set_var(AdvectionDiffusion::Physics::S , state[0]);
  }

  virtual void get_state( const Solver::Physics& p, RealVector& state)
  {
    state[0] = p.var(AdvectionDiffusion::Physics::S);
  }


  virtual void compute_flux( const Solver::Physics& p,
                            const RealVector& normal,
                            RealVector& flux)
  {
    flux[0] = p.var(Physics::Vx)*p.var(Physics::S)*normal[XX];
  }

  virtual void compute_fluxjacobian_right_eigenvectors( const Solver::Physics& p,
                                                        const RealVector& normal,
                                                        RealMatrix& rv)
  {
    rv(0,0) = 1. * normal[XX];
  }

  virtual void compute_fluxjacobian_left_eigenvectors( const Solver::Physics& p,
                                                       const RealVector& normal,
                                                       RealMatrix& lv)
  {
    lv(0,0) = 1. * normal[XX];
  }

  virtual void compute_fluxjacobian_eigenvalues( const Solver::Physics& p,
                                                 const RealVector& normal,
                                                 RealVector& ev)
  {
    ev[0] = p.var(Physics::Vx) * normal[XX];
  }

  virtual Real max_eigen_value ( const Solver::Physics& p, const RealVector& normal )
  {
    return p.var(Physics::Vx) * normal[XX];
  }

  virtual void linearize( const std::vector<Solver::Physics>& states, Solver::Physics& p )
  {
    Real S=0;
    for (Uint i=0; i<states.size(); ++i)
      S += states[i].var(AdvectionDiffusion::Physics::S);
    S /= static_cast<Real>(states.size());

    p.init();
    p.set_var(AdvectionDiffusion::Physics::Vx, 1.);
    p.set_var(AdvectionDiffusion::Physics::S,  S );
  }

}; // State

////////////////////////////////////////////////////////////////////////////////

} // AdvectionDiffusion
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_AdvectionDiffusion_State_hpp