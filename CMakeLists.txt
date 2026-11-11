# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

cmake_minimum_required(VERSION 3.25)

project(pictorio VERSION 0.1.0 LANGUAGES CXX)

# C++23, no compiler-specific extensions. C++23 is well supported by the GCC in the KDE Flatpak SDK;
# Set before KDECompilerSettings is included, so that ECM does not pick its own default.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Writes compile_commands.json into the build folder, so editors (clangd, cpptools) know the exact
# include paths and flags.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Oldest versions we support. The KDE Flatpak runtime we target provides newer ones.
set(QT_MIN_VERSION "6.8.0")
set(KF_MIN_VERSION "6.21.0")  # 6.21: KI18n.i18n*() in QML, checkable by qmllint

# Extra CMake Modules: standard KDE install paths, compiler warnings and build settings.
find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQmlModule)       # ecm_add_qml_module(): QML files compiled into the executable
include(ECMFindQmlModule)   # ecm_find_qmlmodule(): checks QML modules needed at run time
include(FeatureSummary)

# Warnings are errors during development. Packaging (e.g. Flatpak) turns this off, because a newer
# compiler in the runtime can add new warnings and must not break a release build.
option(PICTORIO_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
if(PICTORIO_WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
endif()

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Concurrent
    Gui
    Qml
    Quick
    QuickControls2
    Widgets      # QApplication, required by the KDE desktop style for Qt Quick Controls
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Kirigami     # QML user interface components
    I18n         # translations
    CoreAddons   # application metadata (KAboutData)
)

# Image metadata: the date a photo was taken. ECM's find module, because exiv2's own CMake package
# accepts only the exact version asked for.
find_package(LibExiv2 0.28 REQUIRED)

# Imported by the QML files at run time, so CMake cannot see them through linking.
ecm_find_qmlmodule(org.kde.kirigami REQUIRED)
ecm_find_qmlmodule(org.kde.ki18n REQUIRED)

add_subdirectory(src)
add_subdirectory(packaging)

# BUILD_TESTING is ON by default (set by KDECMakeSettings). Turn it off with -DBUILD_TESTING=OFF,
# e.g. for packaging. Run the tests with: ctest --test-dir <build dir>
if(BUILD_TESTING)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test)
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
