cmake_minimum_required(VERSION 3.20)
PROJECT(POOLS)


set(TEST_CODE TestSample)
add_executable(${PROJECT_NAME})
#add_executable(${TEST_CODE})
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
#target_sources(${TEST_CODE} PRIVATE test/test_type_trait.cpp)



target_include_directories(${PROJECT_NAME} PRIVATE include)
#target_include_directories(${TEST_CODE} PRIVATE include)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    CXX_STANDARD 23
)

#set_target_properties(${TEST_CODE} PROPERTIES 
#    CXX_STANDARD 23
#)
add_subdirectory(test)