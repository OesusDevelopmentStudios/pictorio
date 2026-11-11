# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

# Reads size, orientation and type of an image file (see include/ImageLoader.hpp).
add_library(pictorio_image_loader STATIC
    include/ImageLoader.hpp
    include/ImageLoaderFactory.hpp
    impl/QtImageLoader.hpp
    ImageLoader.cpp
    ImageLoaderFactory.cpp
    impl/QtImageLoader.cpp
)

# Users of the service see only include/; impl/ stays private to the service.
target_include_directories(pictorio_image_loader
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pictorio_image_loader
    PUBLIC
        Qt6::Gui        # QImageIOHandler, QSize in the public header
    PRIVATE
        KF6::I18n
)
