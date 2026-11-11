# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

include(ECMAddTests)

# One Qt Test executable per source file. The CTest name is the file name without .cpp.

ecm_add_test(utils/test_app_identity.cpp
    LINK_LIBRARIES pictorio_utils Qt6::Test KF6::CoreAddons KF6::I18n
)
target_compile_definitions(test_app_identity PRIVATE
    EXPECTED_VERSION="${PROJECT_VERSION}"
)
