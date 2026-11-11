/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "SettingsFactory.hpp"

#include <KSharedConfig>

#include "impl/KConfigSettings.hpp"

namespace pictorio::settings
{

std::unique_ptr<Settings> createSettings()
{
    return std::make_unique<KConfigSettings>(KSharedConfig::openConfig());
}

std::unique_ptr<Settings> createSettings(const QString& configFilePath)
{
    return std::make_unique<KConfigSettings>(KSharedConfig::openConfig(configFilePath, KConfig::SimpleConfig));
}

}  // namespace pictorio::settings
