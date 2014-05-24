cmake_minimum_required(VERSION 2.8)
project(cpsl)

find_package(BISON)
find_package(FLEX)

BISON_TARGET(CpslParser cpsl.y ${CMAKE_CURRENT_BINARY_DIR}/cpsl.cpp)
FLEX_TARGET(CpslScanner cpsl.lex ${CMAKE_CURRENT_BINARY_DIR}/lex.cpp)
ADD_FLEX_BISON_DEPENDENCY(CpslScanner CpslParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(cpsl_srcs
  main.cpp
  ${BISON_CpslParser_OUTPUTS}
  ${FLEX_CpslScanner_OUTPUTS}

  )
add_executable(cpsl ${cpsl_srcs})
source_group(cpsl FILES ${cpsl_srcs})

target_link_libraries(cpsl ${FLEX_LIBRARIES} ${BISON_LIBRARIES})
