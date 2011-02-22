// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Test module for steady UFEM heat conduction"

#include <boost/assign/list_of.hpp>
#include <boost/test/unit_test.hpp>

#include "Common/CreateComponent.hpp"
#include "Common/CRoot.hpp"
#include "Common/LibLoader.hpp"
#include "Common/Log.hpp"
#include "Common/OSystem.hpp"
#include "Common/XmlHelpers.hpp"

#include "Mesh/CMeshReader.hpp"
#include "Mesh/CMeshWriter.hpp"

using namespace CF;
using namespace CF::Common;
using namespace CF::Mesh;

BOOST_AUTO_TEST_SUITE( HeatUnsteadySuite )

BOOST_AUTO_TEST_CASE( HeatLinearUnsteady )
{
  // Load the required libraries (we assume the working dir is the binary path)
  LibLoader& loader = *OSystem::instance().lib_loader();
  
  const std::vector< boost::filesystem::path > lib_paths = boost::assign::list_of
                                                           ("../src")
                                                           ("../../../src/Mesh/Neu")
                                                           ("../../../src/Mesh/Gmsh");
  loader.set_search_paths(lib_paths);
  
  loader.load_library("coolfluid_ufem");
  loader.load_library("coolfluid_mesh_neutral");
  loader.load_library("coolfluid_mesh_gmsh");
  
  int    argc = boost::unit_test::framework::master_test_suite().argc;
  char** argv = boost::unit_test::framework::master_test_suite().argv;
  
  // One argument needed, containing the path to the meshes dir
  BOOST_CHECK_EQUAL(argc, 2);
  
  boost::filesystem::path input_file = boost::filesystem::path(argv[1]) / boost::filesystem::path("ring2d-quads.neu");
  boost::filesystem::path output_file("ring2d-unsteady.msh");
  
  CRoot::Ptr root = Core::instance().root();
  
  // Create the wizard
  Component::Ptr setup_ufem = create_component_abstract_type<Component>("CF.UFEM.SetupLinearSystem", "SetupUFEM");
  root->add_component(setup_ufem);
  
  // Build the model
  boost::shared_ptr<XmlDoc> create_model_root = XmlOps::create_doc();
  XmlNode& create_model_node = *XmlOps::goto_doc_node(*create_model_root.get());
  XmlNode& create_model_frame = *XmlOps::add_signal_frame(create_model_node, "", "", "", true );
  XmlParams create_model_p ( create_model_frame );
  create_model_p.add_option<std::string>("Model name", "UFEMHeat");
  create_model_p.add_option<std::string>("Solver", "CF.UFEM.HeatConductionLinearUnsteady");
  
  setup_ufem->call_signal("create_model", create_model_frame);
  
  Component::Ptr ufem_model = root->get_child("UFEMHeat");
  BOOST_CHECK(ufem_model);
  
  CMeshReader::Ptr mesh_reader = ufem_model->get_child<CMeshReader>("NeutralReader");
  BOOST_CHECK(mesh_reader);
  
  CMesh::Ptr mesh = ufem_model->get_child("Domain")->create_component<CMesh>("Mesh");
  mesh_reader->read_from_to(input_file, mesh);
  
  Component::Ptr ufem_method = ufem_model->get_child("LinearModel");
  BOOST_CHECK(ufem_method);
  
  Component::Ptr heat_eq = ufem_method->get_child("HeatEquation");
  BOOST_CHECK(heat_eq);
  
  // Configure region and variable names
  heat_eq->configure_property("Region", URI("cpath://Root/UFEMHeat/Domain/Mesh/topology/ring2d-quads"));
  heat_eq->configure_property("TemperatureFieldName", std::string("Temperature"));
  heat_eq->configure_property("TemperatureVariableName", std::string("T"));
  
  // Set heat source field
  heat_eq->configure_property("HeatFieldName", std::string("Heat"));
  heat_eq->configure_property("HeatVariableName", std::string("q"));
  heat_eq->configure_property("HeatIsConst", true);
  heat_eq->configure_property("HeatValue", 0.);
  
  // Material properties (copper)
  heat_eq->configure_property("k", /*398.*/1.);
  heat_eq->configure_property("alpha", /*11.57e-5*/1.);
  
  // Time stepping parameters
  ufem_method->configure_property("Timestep", 0.01);
  ufem_method->configure_property("StopTime", 0.2);
  
  // Initial condition value
  boost::shared_ptr<XmlDoc> init_root = XmlOps::create_doc();
  XmlNode& init_node = *XmlOps::goto_doc_node(*init_root.get());
  XmlNode& init_frame = *XmlOps::add_signal_frame(init_node, "", "", "", true );
  XmlParams init_p ( init_frame );
  init_p.add_option<std::string>("Name", "InitialTemperature");
  init_p.add_option<std::string>("FieldName", "Temperature");
  init_p.add_option<std::string>("VariableName", "T");
  
  ufem_method->call_signal("add_initial_condition", init_frame);
  Component::Ptr init = ufem_method->get_child("InitialTemperature");
  BOOST_CHECK(init);
  init->configure_property("Region", URI("cpath://Root/UFEMHeat/Domain/Mesh/topology"));
  init->configure_property("InitialTemperature", 500.);
  
  // Inside boundary condition
  boost::shared_ptr<XmlDoc> inside_bc_root = XmlOps::create_doc();
  XmlNode& inside_bc_node = *XmlOps::goto_doc_node(*inside_bc_root.get());
  XmlNode& inside_bc_frame = *XmlOps::add_signal_frame(inside_bc_node, "", "", "", true );
  XmlParams inside_bc_p ( inside_bc_frame );
  inside_bc_p.add_option<std::string>("BCName", "Inside");
  inside_bc_p.add_option<std::string>("FieldName", "Temperature");
  inside_bc_p.add_option<std::string>("VariableName", "T");
  
  ufem_method->call_signal("add_dirichlet_bc", inside_bc_frame);
  Component::Ptr inside_bc = ufem_method->get_child("Inside");
  BOOST_CHECK(inside_bc);
  inside_bc->configure_property("Region", URI("cpath://Root/UFEMHeat/Domain/Mesh/topology/ring2d-quads/inner"));
  inside_bc->configure_property("Inside", 300.);
  
  // Outside boundary condition
  boost::shared_ptr<XmlDoc> outside_bc_root = XmlOps::create_doc();
  XmlNode& outside_bc_node = *XmlOps::goto_doc_node(*outside_bc_root.get());
  XmlNode& outside_bc_frame = *XmlOps::add_signal_frame(outside_bc_node, "", "", "", true );
  XmlParams outside_bc_p ( outside_bc_frame );
  outside_bc_p.add_option<std::string>("BCName", "Outside");
  outside_bc_p.add_option<std::string>("FieldName", "Temperature");
  outside_bc_p.add_option<std::string>("VariableName", "T");
  
  ufem_method->call_signal("add_dirichlet_bc", outside_bc_frame);
  Component::Ptr outside_bc = ufem_method->get_child("Outside");
  BOOST_CHECK(outside_bc);
  outside_bc->configure_property("Region", URI("cpath://Root/UFEMHeat/Domain/Mesh/topology/ring2d-quads/outer"));
  outside_bc->configure_property("Outside", 300.);
  
  // Run the solver
  boost::shared_ptr<XmlDoc> run_root = XmlOps::create_doc();
  XmlNode& run_node = *XmlOps::goto_doc_node(*run_root.get());
  XmlNode& run_frame = *XmlOps::add_signal_frame(run_node, "", "", "", true );
  
  ufem_method->call_signal("initialize", run_frame);
  ufem_method->call_signal("run", run_frame);
  
  // Write the solution
  CMeshWriter::Ptr writer = create_component_abstract_type<CMeshWriter>("CF.Mesh.Gmsh.CWriter","meshwriter");
  ufem_model->add_component(writer);
  writer->configure_property( "Fields", std::vector<URI>(1, URI("cpath://Root/UFEMHeat/Domain/Mesh/Temperature") ) );
  writer->write_from_to(mesh, output_file);
}

BOOST_AUTO_TEST_SUITE_END()