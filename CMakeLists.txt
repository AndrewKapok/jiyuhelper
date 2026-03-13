cmake_minimum_required(VERSION 3.10)
project(jiyukiller)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_JIYUUDPATTACK "Build JiyuUdpAttack project" ON)

add_subdirectory(MemoryErasure)

if(BUILD_JIYUUDPATTACK)
    add_subdirectory(JiyuUdpAttack)
endif()

add_custom_target(clean_juyuudpattack
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/JiyuUdpAttack/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/JiyuUdpAttack/juyuudpattack_autogen
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/JiyuUdpAttack/cmake_install.cmake
    COMMENT "Cleaning up JiyuUdpAttack build files..."
)

add_custom_target(clean_memoryerasure
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/MemoryErasure/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/MemoryErasure/cmake_install.cmake
    COMMENT "Cleaning up MemoryErasure build files..."
)

add_custom_target(clean_all
    COMMENT "Cleaning up all unnecessary build files..."
)
if(BUILD_JIYUUDPATTACK)
    add_dependencies(clean_all clean_juyuudpattack)
endif()
add_dependencies(clean_all clean_memoryerasure)
