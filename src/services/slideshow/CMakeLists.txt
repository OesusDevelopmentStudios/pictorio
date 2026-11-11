# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

# Headers are listed so that AUTOMOC finds the Q_OBJECT classes in include/.
add_library(pictorio_slideshow STATIC
    include/Slideshow.hpp
    include/SlideshowFactory.hpp
    Slideshow.cpp
    SlideshowFactory.cpp
)

target_include_directories(pictorio_slideshow PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pictorio_slideshow PUBLIC
    pictorio_navigator
    pictorio_settings
    Qt6::Core
)

# Slideshow is exposed to QML (app_manager/QmlForeignTypes.hpp); QML tools read its properties from these metatypes.
qt_extract_metatypes(pictorio_slideshow)
