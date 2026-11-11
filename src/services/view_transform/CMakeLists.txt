# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

# Headers are listed so that AUTOMOC finds the Q_OBJECT classes in include/.
add_library(pictorio_view_transform STATIC
    include/ViewTransform.hpp
    include/ViewTransformFactory.hpp
    ViewTransform.cpp
    ViewTransformFactory.cpp
)

target_include_directories(pictorio_view_transform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pictorio_view_transform PUBLIC
    Qt6::Core
)
