cmake_minimum_required(VERSION 3.10.0)
project(
  protey-hometask-6
  VERSION 0.1.0
  LANGUAGES C CXX)

add_executable(protey-hometask-6-server src/server/server.cpp
                                        src/common/expression.cpp)
add_executable(protey-hometask-6-client src/client/client.cpp
                                        src/common/expression.cpp)
add_executable(test test.cpp src/common/expression.cpp)

target_include_directories(protey-hometask-6-server PRIVATE src/common
                                                            src/server)

target_include_directories(protey-hometask-6-client PRIVATE src/common
                                                            src/client)
target_include_directories(test PRIVATE src/common
                                                            src/client)
