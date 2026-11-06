# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/home/william/MPLABProjects/opgave1_atmega/out/My_Project/default.cmf"
  "/home/william/MPLABProjects/opgave1_atmega/out/My_Project/default.hex"
  "/home/william/MPLABProjects/opgave1_atmega/out/My_Project/default.hxl"
  "/home/william/MPLABProjects/opgave1_atmega/out/My_Project/default.mum"
  "/home/william/MPLABProjects/opgave1_atmega/out/My_Project/default.o"
  "/home/william/MPLABProjects/opgave1_atmega/out/My_Project/default.sdb"
  "/home/william/MPLABProjects/opgave1_atmega/out/My_Project/default.sym"
  )
endif()
