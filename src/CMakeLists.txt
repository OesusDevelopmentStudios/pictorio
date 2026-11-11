# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

add_executable(pictorio)

# The executable is also a QML module: QML files are compiled into it, and main.cpp loads them with
# loadFromModule(). The URI is the app ID. It must differ from the target name, because Qt creates a
# folder named after the URI next to the executable (a URI "pictorio" would collide with bin/pictorio).
ecm_add_qml_module(pictorio URI io.github.OesusDevelopmentStudios.Pictorio)

target_sources(pictorio PRIVATE
    main.cpp
)

ecm_target_qml_sources(pictorio SOURCES
    qml/Main.qml
)

target_compile_definitions(pictorio PRIVATE
    PICTORIO_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(pictorio PRIVATE
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    KF6::I18n
    KF6::I18nQml
    KF6::CoreAddons
)

install(TARGETS pictorio ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
