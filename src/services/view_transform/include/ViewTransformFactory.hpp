/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include "ViewTransform.hpp"

namespace pictorio::view_transform
{

std::unique_ptr<ViewTransform> createViewTransform();

}  // namespace pictorio::view_transform
