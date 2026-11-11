/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "ViewTransformFactory.hpp"

namespace pictorio::view_transform
{

std::unique_ptr<ViewTransform> createViewTransform()
{
    return std::make_unique<ViewTransform>();
}

}  // namespace pictorio::view_transform
