# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/home/william/MPLABProjects/atme/out/atme/default.cmf"
  "/home/william/MPLABProjects/atme/out/atme/default.hex"
  "/home/william/MPLABProjects/atme/out/atme/default.hxl"
  "/home/william/MPLABProjects/atme/out/atme/default.mum"
  "/home/william/MPLABProjects/atme/out/atme/default.o"
  "/home/william/MPLABProjects/atme/out/atme/default.sdb"
  "/home/william/MPLABProjects/atme/out/atme/default.sym"
  )
endif()
