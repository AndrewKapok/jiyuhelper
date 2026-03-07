cmake_minimum_required(VERSION 3.10)
project(MemoryErasure)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -s -O3 -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections -fmerge-all-constants -fno-stack-protector -mconsole")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

add_executable(MemoryErasure MemoryErasure.cpp)

target_link_libraries(MemoryErasure kernel32 psapi)

find_program(UPX_EXECUTABLE upx)
if(UPX_EXECUTABLE)
    add_custom_command(TARGET MemoryErasure POST_BUILD
        COMMAND ${UPX_EXECUTABLE} --best --overlay=strip --force $<TARGET_FILE:MemoryErasure> || exit 0
        COMMENT "Compressing executable with UPX..."
        VERBATIM
    )
else()
    message(WARNING "UPX not found, skipping compression")
endif()
