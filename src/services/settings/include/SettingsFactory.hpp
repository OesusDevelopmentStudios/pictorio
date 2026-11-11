/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include <QString>

#include "Settings.hpp"

namespace pictorio::settings
{

// Settings in the application's config file (~/.config/pictoriorc).
std::unique_ptr<Settings> createSettings();

// Settings in the given file, e.g. a temporary one in tests.
std::unique_ptr<Settings> createSettings(const QString& configFilePath);

}  // namespace pictorio::settings
