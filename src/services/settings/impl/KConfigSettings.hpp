/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <KConfigGroup>
#include <KSharedConfig>

#include "Settings.hpp"

namespace pictorio::settings
{

// Stores the settings with KConfig, the usual KDE config file format.
class KConfigSettings : public Settings
{
public:
    explicit KConfigSettings(KSharedConfigPtr config);

    std::optional<int> slideshowIntervalSeconds() const override;
    void setSlideshowIntervalSeconds(int intervalSeconds) override;

    std::optional<QString> slideshowOrder() const override;
    void setSlideshowOrder(const QString& order) override;

private:
    KConfigGroup slideshowGroup() const;
    void writeSlideshowEntry(const char* key, const QVariant& value);

    KSharedConfigPtr m_config;
};

}  // namespace pictorio::settings
