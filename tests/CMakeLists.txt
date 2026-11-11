# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

include(ECMAddTests)

# One Qt Test executable per source file. The CTest name is the file name without .cpp.

ecm_add_test(utils/TestAppIdentity.cpp
    LINK_LIBRARIES pictorio_utils Qt6::Test KF6::CoreAddons KF6::I18n
)
target_compile_definitions(TestAppIdentity PRIVATE
    EXPECTED_VERSION="${PROJECT_VERSION}"
)

ecm_add_test(utils/TestCommandLine.cpp
    LINK_LIBRARIES pictorio_utils Qt6::Test
)

ecm_add_test(services/image_loader/TestImageLoader.cpp
    LINK_LIBRARIES pictorio_image_loader Qt6::Test KF6::I18n LibExiv2::LibExiv2
)
kde_target_enable_exceptions(TestImageLoader PRIVATE)  # exiv2 writes the test EXIF data

ecm_add_test(services/viewer/TestViewer.cpp
    LINK_LIBRARIES pictorio_viewer Qt6::Test KF6::I18n
)

ecm_add_test(services/view_transform/TestViewTransform.cpp
    LINK_LIBRARIES pictorio_view_transform Qt6::Test
)

ecm_add_test(services/folder_scanner/TestFolderScanner.cpp
    LINK_LIBRARIES pictorio_folder_scanner Qt6::Test
)

ecm_add_test(services/navigator/TestNavigator.cpp
    LINK_LIBRARIES pictorio_navigator Qt6::Test
)

ecm_add_test(packaging/TestDesktopFile.cpp
    LINK_LIBRARIES pictorio_image_loader Qt6::Gui Qt6::Test KF6::I18n
)
target_compile_definitions(TestDesktopFile PRIVATE
    DESKTOP_FILE="${PROJECT_SOURCE_DIR}/packaging/io.github.oesusdevelopmentstudios.pictorio.desktop"
)
