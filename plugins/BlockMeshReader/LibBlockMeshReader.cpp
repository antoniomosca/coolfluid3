// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/RegistLibrary.hpp"

#include "BlockMeshReader/LibBlockMeshReader.hpp"

namespace CF {
namespace BlockMeshReader {

CF::Common::RegistLibrary<LibBlockMeshReader> libBlockMeshReader;

////////////////////////////////////////////////////////////////////////////////

void LibBlockMeshReader::initiate()
{
}

void LibBlockMeshReader::terminate()
{
}

////////////////////////////////////////////////////////////////////////////////

} // BlockMeshReader
} // CF
