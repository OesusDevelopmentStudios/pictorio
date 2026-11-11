# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

# Shared helpers. A static library, so that tests can link the same code as the application.
add_library(pictorio_utils STATIC
    utils/AppIdentity.cpp
)

# Lets every target that links pictorio_utils write #include "utils/AppIdentity.hpp".
target_include_directories(pictorio_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(pictorio_utils PRIVATE
    PICTORIO_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(pictorio_utils
    PUBLIC
        Qt6::Core
    PRIVATE
        KF6::CoreAddons
        KF6::I18n
)

add_subdirectory(services/image_loader)
add_subdirectory(services/viewer)

# The application. main.cpp only wires things together; logic lives in the libraries above.
add_executable(pictorio)

# The executable is also a QML module: QML files are compiled into it, and main.cpp loads them with
# loadFromModule(). The URI is the app ID. It must differ from the target name, because Qt creates a
# folder named after the URI next to the executable (a URI "pictorio" would collide with bin/pictorio).
ecm_add_qml_module(pictorio URI io.github.oesusdevelopmentstudios.pictorio)

target_sources(pictorio PRIVATE
    main.cpp
)

ecm_target_qml_sources(pictorio SOURCES
    qml/Main.qml
)

target_link_libraries(pictorio PRIVATE
    pictorio_utils
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    KF6::I18n
    KF6::I18nQml
)

install(TARGETS pictorio ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
