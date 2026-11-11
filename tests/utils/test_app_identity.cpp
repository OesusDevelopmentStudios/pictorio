/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <KAboutData>
#include <KLocalizedString>
#include <QCoreApplication>
#include <QTest>

#include "utils/app_identity.hpp"

namespace pictorio::utils
{

// Checks that registerApplicationIdentity() hands the right identity to KDE and Qt. Mistakes here are
// easy to miss by hand: a wrong desktop file name only shows up as a missing window icon under Wayland,
// a wrong translation domain only as an untranslated user interface.
class TestAppIdentity : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void setsTranslationDomain();
    void registersComponentNameWithQt();
    void registersProjectVersion();
    void usesApplicationIdAsDesktopFileName();
    void usesOwnOrganizationDomain();
    void declaresGplVersion3OrLater();
};

void TestAppIdentity::initTestCase()
{
    registerApplicationIdentity();
}

void TestAppIdentity::setsTranslationDomain()
{
    QCOMPARE(KLocalizedString::applicationDomain(), QByteArrayLiteral("pictorio"));
}

void TestAppIdentity::registersComponentNameWithQt()
{
    QCOMPARE(QCoreApplication::applicationName(), QStringLiteral("pictorio"));
}

void TestAppIdentity::registersProjectVersion()
{
    QCOMPARE(QCoreApplication::applicationVersion(), QStringLiteral(EXPECTED_VERSION));
}

void TestAppIdentity::usesApplicationIdAsDesktopFileName()
{
    QCOMPARE(KAboutData::applicationData().desktopFileName(),
             QStringLiteral("io.github.OesusDevelopmentStudios.Pictorio"));
}

void TestAppIdentity::usesOwnOrganizationDomain()
{
    QCOMPARE(KAboutData::applicationData().organizationDomain(), QStringLiteral("OesusDevelopmentStudios.github.io"));
}

void TestAppIdentity::declaresGplVersion3OrLater()
{
    const auto licenses = KAboutData::applicationData().licenses();

    QCOMPARE(licenses.size(), 1);
    // KAboutLicense uses the older SPDX spelling "GPL-3.0+", which means the same as "GPL-3.0-or-later".
    QCOMPARE(licenses.first().spdx(), QStringLiteral("GPL-3.0+"));
}

}  // namespace pictorio::utils

QTEST_GUILESS_MAIN(pictorio::utils::TestAppIdentity)

#include "test_app_identity.moc"
