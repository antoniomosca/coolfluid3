// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_LibCGNS_hpp
#define CF_LibCGNS_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Common/LibraryRegister.hpp"
#include "Common/ExportAPI.hpp"

////////////////////////////////////////////////////////////////////////////////

/// Define the macro CGNS_API
/// @note build system defines CGNS_EXPORTS when compiling CGNS files
#ifdef CGNS_EXPORTS
#   define CGNS_API      CF_EXPORT_API
#   define CGNS_TEMPLATE
#else
#   define CGNS_API      CF_IMPORT_API
#   define CGNS_TEMPLATE CF_TEMPLATE_EXTERN
#endif

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {
namespace CGNS {

////////////////////////////////////////////////////////////////////////////////

/// Class defines the CGNS mesh format operations
/// @author Willem Deconinck
class LibCGNS :
    public CF::Common::LibraryRegister<LibCGNS>
{
public:

  /// Static function that returns the module name.
  /// Must be implemented for the LibraryRegister template
  /// @return name of the module
  static std::string library_name() { return "CGNS"; }

  /// Static function that returns the description of the module.
  /// Must be implemented for the LibraryRegister template
  /// @return descripton of the module
  static std::string library_description()
  {
    return "This library implements the CGNS mesh format operations.";
  }

  /// Gets the Class name
  static std::string type_name() { return "LibCGNS"; }

  /// Start profiling
  virtual void initiate();

  /// Stop profiling
  virtual void terminate();

}; // end LibCGNS

////////////////////////////////////////////////////////////////////////////////

} // namespace CGNS
} // namespace Mesh
} // namespace CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_CGNS_hpp