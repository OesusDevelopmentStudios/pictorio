# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

# Headers are listed so that AUTOMOC finds the Q_OBJECT classes in include/.
add_library(pictorio_viewer STATIC
    include/Viewer.hpp
    include/ViewerFactory.hpp
    Viewer.cpp
    ViewerFactory.cpp
)

target_include_directories(pictorio_viewer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pictorio_viewer PUBLIC
    pictorio_image_loader
    Qt6::Core
)
