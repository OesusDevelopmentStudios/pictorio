# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

# Lists the images in a folder (see include/FolderScanner.hpp).
add_library(pictorio_folder_scanner STATIC
    include/FolderScanner.hpp
    include/FolderScannerFactory.hpp
    impl/QtFolderScanner.hpp
    impl/QtFolderScanner.cpp
    FolderScanner.cpp
    FolderScannerFactory.cpp
)

# Users of the service see only include/; impl/ stays private to the service.
target_include_directories(pictorio_folder_scanner
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pictorio_folder_scanner
    PUBLIC
        Qt6::Core
    PRIVATE
        Qt6::Gui
)
