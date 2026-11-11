/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "impl/KConfigSettings.hpp"

namespace
{

// Group and key names as they appear in the config file
const QString slideshowGroupName = QStringLiteral("Slideshow");
constexpr const char* intervalKey = "IntervalSeconds";
constexpr const char* orderKey = "Order";

}  // namespace

namespace pictorio::settings
{

KConfigSettings::KConfigSettings(KSharedConfigPtr config)
    : m_config(std::move(config))
{}

std::optional<int> KConfigSettings::slideshowIntervalSeconds() const
{
    const KConfigGroup group = slideshowGroup();
    if (!group.hasKey(intervalKey))
    {
        return std::nullopt;
    }

    return group.readEntry(intervalKey, 0);
}

void KConfigSettings::setSlideshowIntervalSeconds(int intervalSeconds)
{
    writeSlideshowEntry(intervalKey, intervalSeconds);
}

std::optional<QString> KConfigSettings::slideshowOrder() const
{
    const KConfigGroup group = slideshowGroup();
    if (!group.hasKey(orderKey))
    {
        return std::nullopt;
    }

    return group.readEntry(orderKey, QString());
}

void KConfigSettings::setSlideshowOrder(const QString& order)
{
    writeSlideshowEntry(orderKey, order);
}

KConfigGroup KConfigSettings::slideshowGroup() const
{
    return m_config->group(slideshowGroupName);
}

void KConfigSettings::writeSlideshowEntry(const char* key, const QVariant& value)
{
    KConfigGroup group = slideshowGroup();
    group.writeEntry(key, value);
    m_config->sync();  // saved at once, so a crash does not lose the change
}

}  // namespace pictorio::settings
