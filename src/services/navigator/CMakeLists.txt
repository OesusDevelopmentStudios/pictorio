# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

# Headers are listed so that AUTOMOC finds the Q_OBJECT classes in include/.
add_library(pictorio_navigator STATIC
    include/Navigator.hpp
    include/NavigatorFactory.hpp
    Navigator.cpp
    NavigatorFactory.cpp
)

target_include_directories(pictorio_navigator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pictorio_navigator
    PUBLIC
        pictorio_folder_scanner
        Qt6::Core
    PRIVATE
        Qt6::Concurrent  # lists the folder in the background
)

# Navigator is exposed to QML (app_manager/QmlForeignTypes.hpp); QML tools read its properties from these metatypes.
qt_extract_metatypes(pictorio_navigator)
