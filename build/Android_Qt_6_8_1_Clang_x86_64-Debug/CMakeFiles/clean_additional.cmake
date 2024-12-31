# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\SimpleChatLib_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SimpleChatLib_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\SimpleChat_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SimpleChat_autogen.dir\\ParseCache.txt"
  "SimpleChatLib_autogen"
  "SimpleChat_autogen"
  "tests\\CMakeFiles\\SimpleChatTests_autogen.dir\\AutogenUsed.txt"
  "tests\\CMakeFiles\\SimpleChatTests_autogen.dir\\ParseCache.txt"
  "tests\\SimpleChatTests_autogen"
  )
endif()
