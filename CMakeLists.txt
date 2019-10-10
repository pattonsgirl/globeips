cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(GlobeIPs)

#must set directory for v7.1.1
set(VTK_DIR "/home/kduncan/VTK-Release-build")

#in sample online:
#find(VTK REQUIRED)
#include(${VTK_USE_FILE})
#tell cmake we want to compile x.cxx to a binary called x
#add_executable(X X.cxx)
#tell cmake we need to link X against VTK libraries
#target_link_libraries(X ${VTK_LIBRARIES})

find_package(VTK REQUIRED)
#find_package(VTK COMPONENTS 
#  vtkCommonColor
#  vtkCommonCore
#  vtkCommonDataModel
#  vtkIOGeometry
#  vtkGeovisCore
#  vtkInteractionStyle
#  vtkRenderingContextOpenGL2
#  vtkRenderingCore
#  vtkRenderingFreeType
#  vtkRenderingGL2PSOpenGL2
#  vtkRenderingOpenGL2 QUIET)
if (NOT VTK_FOUND)
  message("Skipping Compile: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
#we should only be working in if statement, since we are using old verison
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(GlobeIPs MACOSX_BUNDLE GlobeIPs.cxx)
  target_link_libraries(GlobeIPs PRIVATE ${VTK_LIBRARIES})
else ()
  # include all components
  add_executable(GlobeIPs MACOSX_BUNDLE GlobeIPs.cxx )
  target_link_libraries(GlobeIPs PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS GlobeIPs
    MODULES ${VTK_LIBRARIES}
    )
endif ()
