### prints the build summary

# get some variables
site_name(CF_BUILD_HOSTNAME)

coolfluid_get_date(CF_BUILD_DATE)

get_property( CF_LANGUAGES GLOBAL PROPERTY ENABLED_LANGUAGES )

set( print_counter 0 )

# print the header with versions and operating system

coolfluid_log( "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++" )
coolfluid_log( " coolfluid configuration summary " )
coolfluid_log( "---------------------------------------------------------" )
coolfluid_log( " version & kernel      : [${CF_VERSION}] @ [${CF_KERNEL_VERSION}]" )
if(coolfluid_svn_revision)
  coolfluid_log( " coolfluid revision    : [${coolfluid_svn_revision}]" )
endif()
coolfluid_log( " hostname & date       : [${CF_BUILD_HOSTNAME}] @ [${CF_BUILD_DATE}]" )
coolfluid_log( " operating system      : [${CMAKE_SYSTEM}] @ [${CF_OS_BITS}] bits" )

# print cmake information

coolfluid_log( "---------------------------------------------------------" )
coolfluid_log( " cmake version         : [${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}]" )
coolfluid_log( " cmake generator       : [${CMAKE_GENERATOR}]" )
coolfluid_log( " build type            : [${CMAKE_BUILD_TYPE}]" )
coolfluid_log( " boost version         : [${Boost_LIB_VERSION}]" )

# print the compiler information ( put to the log file )

coolfluid_log_file( "---------------------------------------------------------" )
coolfluid_log_file( " compiler id & version : [${CMAKE_CXX_COMPILER_ID} ${CF_CXX_COMPILER_VERSION}]" )
coolfluid_log_file( " common linker flags   : [${LINK_FLAGS}]" )
coolfluid_log_file( " shared linker flags   : [${CMAKE_SHARED_LINKER_FLAGS}]" )
coolfluid_log_file( " link libraries        : [${LINK_LIBRARIES}]" )
coolfluid_log( " enabled languages     : [${CF_LANGUAGES}]")
foreach( lang ${CF_LANGUAGES} )
  coolfluid_log_file( " language ${lang} " )
  coolfluid_log_file( "    compiler           : [${CMAKE_${lang}_COMPILER}]" )
  coolfluid_log_file( "    flags              : [${CMAKE_${lang}_FLAGS} ${CMAKE_${lang}_FLAGS_${CMAKE_BUILD_TYPE}}]" )
  coolfluid_log_file( "    link flags         : [${CMAKE_CXX_LINK_FLAGS}]" )
endforeach()

# print most important build options

coolfluid_log( "---------------------------------------------------------" )
coolfluid_log( " Documentation         : [${CF_ENABLE_DOCS}]")
coolfluid_log( " Unit Tests            : [${CF_ENABLE_UNIT_TESTS}]")
coolfluid_log( " Acceptance Tests      : [${CF_ENABLE_ACCEPTANCE_TESTS}]")
coolfluid_log( " GUI                   : [${CF_ENABLE_GUI}]")
coolfluid_log( " Sandbox               : [${CF_ENABLE_SANDBOX}]")
coolfluid_log( " Assertions            : [${CF_ENABLE_ASSERTIONS}]")
coolfluid_log( " Code coverage         : [${CF_ENABLE_CODECOVERAGE}]")
coolfluid_log( " Explicit Templates    : [${CF_HAVE_CXX_EXPLICIT_TEMPLATES}]")

# print install path

coolfluid_log( "---------------------------------------------------------" )
coolfluid_log( " install path          : [${CMAKE_INSTALL_PREFIX}]" )
coolfluid_log( "---------------------------------------------------------" )
coolfluid_log( "")

# list the active kernel libraries

set( list_kernel_libs "" )
set( print_counter 0 )
foreach( klib ${CF_KERNEL_LIBS} )

  string(REGEX REPLACE "coolfluid_(.+)" "\\1" klib ${klib} )

  set( list_kernel_libs "${list_kernel_libs} ${klib}" )

  # break line if necessary
  math( EXPR print_counter '${print_counter}+1'  )
  if( print_counter GREATER 5 )
    set( print_counter 0 )
    set( list_kernel_libs "${list_kernel_libs}\n\t\t " )
  endif()

endforeach()
coolfluid_log( " Kernel libs: ${list_kernel_libs}")
coolfluid_log( "")


# list the active plugins

set( list_plugins "" )
set( print_counter 0 )
foreach( plugin ${CF_PLUGIN_LIST} )

  set( list_plugins "${list_plugins} ${plugin}" )

  # break line if necessary
  math( EXPR print_counter '${print_counter}+1'  )
  if( print_counter GREATER 5 )
    set( print_counter 0 )
    set( list_plugins "${list_plugins}\n\t\t " )
  endif()

endforeach()
coolfluid_log( " Plugins:     ${list_plugins}")
coolfluid_log( "")

# list the active features

if( COMMAND feature_summary )

#  feature_summary( WHAT ENABLED_FEATURES
#                   DESCRIPTION "Enabled Features:"
#                   VAR CF_ENABLED_FEATURES )
#  coolfluid_log( "${CF_ENABLED_FEATURES}" )

  set( list_features "" )
  set( print_counter 0 )
  get_property( CF_ENABLED_FEATURES  GLOBAL PROPERTY ENABLED_FEATURES )
  foreach( feature ${CF_ENABLED_FEATURES})

    set( list_features "${list_features} ${feature}")

    # break line if necessary
    math( EXPR print_counter '${print_counter}+1'  )
    if( print_counter GREATER 5 )
      set( print_counter 0 )
      set( list_features "${list_features}\n\t\t " )
    endif()

  endforeach()
  coolfluid_log( " Features:    ${list_features}")
  coolfluid_log( "")


  feature_summary(WHAT ALL
                  FILENAME ${PROJECT_LOG_FILE} APPEND)

endif()

# warn if this is a static build
# TODO: test stati building

if(CF_ENABLE_STATIC)
  coolfluid_log( ">>>> ------------------------------" )
  coolfluid_log( ">>>> IMPORTANT -- STATIC BUILD <<<<" )
  coolfluid_log( ">>>> ------------------------------" )
endif()

