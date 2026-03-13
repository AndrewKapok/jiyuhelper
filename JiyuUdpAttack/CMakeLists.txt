cmake_minimum_required(VERSION 3.16)
project(juyuudpattack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(SOURCES
    main.cpp
    jiyuudpattack.cpp
)

set(HEADERS
    jiyuudpattack.h
)

add_executable(juyuudpattack ${SOURCES} ${HEADERS})

target_link_libraries(juyuudpattack
    Qt6::Core
    Qt6::Network
)

if(WIN32)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET juyuudpattack POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                $<TARGET_FILE:juyuudpattack>
            COMMENT "Deploying Qt dependencies..."
        )
    else()
        message(WARNING "windeployqt not found, skipping Qt deployment")
    endif()

    find_program(UPX_EXECUTABLE upx)
    if(UPX_EXECUTABLE)
        add_custom_command(TARGET juyuudpattack POST_BUILD
            COMMAND ${UPX_EXECUTABLE} --best --overlay=strip --force $<TARGET_FILE:juyuudpattack> || exit 0
            COMMENT "Compressing executable with UPX..."
            VERBATIM
        )
    else()
        message(WARNING "UPX not found, skipping compression")
    endif()
endif()
