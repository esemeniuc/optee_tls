cmake_minimum_required(VERSION 3.5)

project(optee_example_hello_world C)

set(SRC host/main.c
        ta/hello_world_ta.c
        ta/mbed_optee_compat.c)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME}
        PRIVATE ta/include
        PRIVATE include
        PRIVATE ~/trustedcapsules/code/optee_client/public
        PRIVATE ~/trustedcapsules/code/optee_os/lib/libutee/include
        PRIVATE ~/trustedcapsules/code/optee_os/lib/libmbedtls/mbedtls/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE teec)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
