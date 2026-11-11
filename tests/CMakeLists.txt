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
    LINK_LIBRARIES pictorio_image_loader Qt6::Test KF6::I18n
)

ecm_add_test(services/viewer/TestViewer.cpp
    LINK_LIBRARIES pictorio_viewer Qt6::Test KF6::I18n
)
