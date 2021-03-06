// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Test module for component factory"

#include <boost/test/unit_test.hpp>

#include "Common/Log.hpp"
#include "Common/Core.hpp"
#include "Common/MPI/PE.hpp"

using namespace std;
using namespace boost;
using namespace CF;
using namespace CF::Common;

//////////////////////////////////////////////////////////////////////////////

struct Core_fixture
{
  /// common setup for each test case
  Core_fixture() {}

  /// common tear-down for each test case
  ~Core_fixture() {}
};

//////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_SUITE( CFactoryTest, Core_fixture )

//////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( initiate )
{
  Core::instance().initiate(boost::unit_test::framework::master_test_suite().argc,
                            boost::unit_test::framework::master_test_suite().argv);
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( terminate )
{
  Core::instance().terminate();
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////

