PROJECT(POOLS_TEST)
set(POOL_TEST pool_test)
add_executable(${POOL_TEST})
target_sources(${POOL_TEST} PRIVATE test_type_trait.cpp)



target_include_directories(${POOL_TEST} PRIVATE 


${CMAKE_SOURCE_DIR}/include
${CMAKE_CURRENT_LIST_DIR}/include
)

set_target_properties(${POOL_TEST} PROPERTIES 
    CXX_STANDARD 23
)

