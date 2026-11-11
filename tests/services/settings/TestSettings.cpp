/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <memory>

#include <KConfig>
#include <KConfigGroup>
#include <QTemporaryDir>
#include <QTest>

#include "Settings.hpp"
#include "SettingsFactory.hpp"

namespace pictorio::settings
{

// Each test uses its own temporary config file, never the user's real settings.
class TestSettings : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();

    void returnsNothingForValuesNeverStored();
    void keepsValuesAfterRestart();
    void writesReadableConfigFile();

private:
    QString configFilePath() const;

    std::unique_ptr<QTemporaryDir> m_folder;
};

void TestSettings::init()
{
    m_folder = std::make_unique<QTemporaryDir>();
    QVERIFY(m_folder->isValid());
}

void TestSettings::returnsNothingForValuesNeverStored()
{
    const auto settings = createSettings(configFilePath());

    QCOMPARE(settings->slideshowIntervalSeconds(), std::nullopt);
    QCOMPARE(settings->slideshowOrder(), std::nullopt);
}

void TestSettings::keepsValuesAfterRestart()
{
    {
        const auto settings = createSettings(configFilePath());
        settings->setSlideshowIntervalSeconds(12);
        settings->setSlideshowOrder(QStringLiteral("date"));
    }

    const auto settingsAfterRestart = createSettings(configFilePath());

    QCOMPARE(settingsAfterRestart->slideshowIntervalSeconds(), 12);
    QCOMPARE(settingsAfterRestart->slideshowOrder(), QStringLiteral("date"));
}

// Group and key names must not change, or users lose their settings with an update.
void TestSettings::writesReadableConfigFile()
{
    const auto settings = createSettings(configFilePath());
    settings->setSlideshowIntervalSeconds(7);
    settings->setSlideshowOrder(QStringLiteral("random"));

    const KConfig configFile(configFilePath(), KConfig::SimpleConfig);
    const KConfigGroup slideshowGroup = configFile.group(QStringLiteral("Slideshow"));

    QCOMPARE(slideshowGroup.readEntry("IntervalSeconds", 0), 7);
    QCOMPARE(slideshowGroup.readEntry("Order", QString()), QStringLiteral("random"));
}

QString TestSettings::configFilePath() const
{
    return m_folder->filePath(QStringLiteral("pictoriorc"));
}

}  // namespace pictorio::settings

QTEST_GUILESS_MAIN(pictorio::settings::TestSettings)

#include "TestSettings.moc"
