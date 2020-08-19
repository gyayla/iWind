#===========================================================================#
# FILE:  iWIND/CMakeLists.txt                                           #
# DATE:  orig file from git repo: 2015/12/07, last changes Aug 2020 (Gokay)#
# INFO:  Source-level CMakeLists.txt file for the moos-ivp-extend project   #
# NAME:  Git repo Maintained by Mohamed Saad IBN SEDDIK                     #
#        from original file moos-ivp-extend/CMakeLists.txt                  #
#        KU Leuven additions, Gerben Peeters, Gokay Yayla                   #
#===========================================================================#

FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

SET(SRC
  WIND.cpp
  WIND_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iWIND ${SRC})

TARGET_LINK_LIBRARIES(iWIND
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  mbutil
  m
  pthread
)
