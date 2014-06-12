cmake_minimum_required(VERSION 2.8)
project(cpsl)

find_package(BISON)
find_package(FLEX)

BISON_TARGET(CpslParser cpsl.y ${CMAKE_CURRENT_BINARY_DIR}/cpsl.cpp)
FLEX_TARGET(CpslScanner cpsl.lex ${CMAKE_CURRENT_BINARY_DIR}/lex.cpp)
ADD_FLEX_BISON_DEPENDENCY(CpslScanner CpslParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR} headers)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-deprecated-register -Wno-unneeded-internal-declaration -ggdb")
set(cpsl_srcs
  main.cpp
  SymbolTable.cpp
  SymbolTableLevel.cpp
  Expression.cpp
  Register.cpp
  Symbol.cpp
  Type.cpp
  ${BISON_CpslParser_OUTPUTS}
  ${FLEX_CpslScanner_OUTPUTS}

  )

set(cpsl_headers
    headers/SymbolTable.h
    headers/SymbolTableLevel.h
    headers/Symbol.h
    headers/Expression.h
    headers/Register.h
    headers/Type.h
    headers/ListWrapper.h
    headers/Log.h
    )

add_executable(cpsl ${cpsl_srcs} ${cpsl_headers})
source_group(cpsl FILES ${cpsl_srcs} ${cpsl_headers})

target_link_libraries(cpsl ${FLEX_LIBRARIES} ${BISON_LIBRARIES})
