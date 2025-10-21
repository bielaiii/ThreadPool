PROJECT(POOLS_TEST)
set(IPOOL_TEST pool_test)
set(POOL_TEST_BASIC pool_test_basic)
set(POOL_TEST_CONCURRENCY pool_test_concurrency)
set(POOL_TEST_EXCEPTION pool_test_exception)
add_library(${IPOOL_TEST} INTERFACE)
add_executable(${POOL_TEST_BASIC} )
add_executable(${POOL_TEST_CONCURRENCY} )
add_executable(${POOL_TEST_EXCEPTION} )


target_sources(${POOL_TEST_BASIC} PRIVATE test_threadpool_basic.cpp)
target_sources(${POOL_TEST_CONCURRENCY} PRIVATE test_threadpool_concurrency.cpp)
target_sources(${POOL_TEST_EXCEPTION} PRIVATE test_threadpool_exception.cpp)

target_include_directories(${IPOOL_TEST} INTERFACE 

${CMAKE_SOURCE_DIR}/include
${CMAKE_CURRENT_LIST_DIR}/include
)

target_compile_options(${IPOOL_TEST} INTERFACE 
-std=c++23
)

target_link_libraries(${POOL_TEST_BASIC} PRIVATE ${IPOOL_TEST})
target_link_libraries(${POOL_TEST_CONCURRENCY} PRIVATE ${IPOOL_TEST})
target_link_libraries(${POOL_TEST_EXCEPTION} PRIVATE ${IPOOL_TEST})

