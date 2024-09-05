# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\testGraphic_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\testGraphic_autogen.dir\\ParseCache.txt"
  "testGraphic_autogen"
  )
endif()
