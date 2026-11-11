# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

cmake_minimum_required(VERSION 3.25)

project(pictorio VERSION 0.1.0 LANGUAGES CXX)

# C++23, no compiler-specific extensions. C++23 is well supported by the GCC in the KDE Flatpak SDK;
# Set before KDECompilerSettings is included, so that ECM does not pick its own default.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Oldest versions we support. The KDE Flatpak runtime we target provides newer ones.
set(QT_MIN_VERSION "6.8.0")
set(KF_MIN_VERSION "6.10.0")

# Extra CMake Modules: standard KDE install paths, compiler warnings and build settings.
find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

# Warnings are errors during development. Packaging (e.g. Flatpak) turns this off, because a newer
# compiler in the runtime can add new warnings and must not break a release build.
option(PICTORIO_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
if(PICTORIO_WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
endif()

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Kirigami     # QML user interface components
    I18n         # translations
    CoreAddons   # application metadata (KAboutData)
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
