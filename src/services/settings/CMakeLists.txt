# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

# Settings kept between runs (see include/Settings.hpp).
add_library(pictorio_settings STATIC
    include/Settings.hpp
    include/SettingsFactory.hpp
    impl/KConfigSettings.hpp
    impl/KConfigSettings.cpp
    Settings.cpp
    SettingsFactory.cpp
)

# Users of the service see only include/; impl/ stays private to the service.
target_include_directories(pictorio_settings
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pictorio_settings
    PUBLIC
        Qt6::Core
    PRIVATE
        KF6::ConfigCore
)
