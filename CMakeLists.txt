## Copyright (c) 2016-2017 Ingo Wald
## 
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
## 
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.

CONFIGURE_MPI()

# simple display-wall driver
OPTION(OSPRAY_MODULE_DISPLAY_WALD "DisplayWald" ON)

IF (OSPRAY_MODULE_DISPLAY_WALD)
  
  SET(OSPRAY_DISPLAY_WALD_VERSION_MAJOR 0)
  SET(OSPRAY_DISPLAY_WALD_VERSION_MINOR 1)
  SET(OSPRAY_DISPLAY_WALD_VERSION_PATCH 0)
  
  SET(OSPRAY_DISPLAY_WALD_VERSION
    ${OSPRAY_DISPLAY_WALD_VERSION_MAJOR}.${OSPRAY_DISPLAY_WALD_VERSION_MINOR}.${OSPRAY_DISPLAY_WALD_VERSION_PATCH})


  SET(OSPRAY_DEFAULT_COMPONENT displayWall)
  OSPRAY_BUILD_COMPONENT(mpiCommon)

  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ospray/include)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  ADD_SUBDIRECTORY(common)
  ADD_SUBDIRECTORY(service)
  ADD_SUBDIRECTORY(client)

  # the ospray pixelop one can use to get a ospray frame buffer to the
  # display wall
  ADD_SUBDIRECTORY(ospray)

  SET(DISPLAY_WALD_DIR  "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "" FORCE)

  ADD_EXECUTABLE(launch_nasa
    tools/launchHelpers/nasaHyperWall.cpp
    )
  TARGET_LINK_LIBRARIES(launch_nasa
    ospray
    )

ENDIF()
