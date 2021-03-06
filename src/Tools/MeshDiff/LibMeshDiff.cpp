// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/RegistLibrary.hpp"

#include "Tools/MeshDiff/LibMeshDiff.hpp"

namespace CF {
namespace Tools {
namespace MeshDiff {

CF::Common::RegistLibrary<LibMeshDiff> libMeshDiff;

////////////////////////////////////////////////////////////////////////////////

void LibMeshDiff::initiate_impl()
{
}

void LibMeshDiff::terminate_impl()
{
}

////////////////////////////////////////////////////////////////////////////////

} // MeshDiff
} // Tools
} // CF
