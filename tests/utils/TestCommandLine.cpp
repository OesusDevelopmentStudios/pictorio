/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "utils/CommandLine.hpp"

#include <QTest>

#include "utils/AppIdentity.hpp"

namespace pictorio::utils
{

class TestCommandLine : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void readsImagePaths_data();
    void readsImagePaths();
    void keepsOrderOfImages();
    void offersAboutOptions_data();
    void offersAboutOptions();

private:
    // Parses the arguments as if they followed the program name.
    CommandLineOptions parse(const QStringList& arguments);
};

void TestCommandLine::initTestCase()
{
    registerApplicationIdentity();
}

CommandLineOptions TestCommandLine::parse(const QStringList& arguments)
{
    QCommandLineParser parser;
    addCommandLineOptions(parser);
    if (!parser.parse(QStringList{QStringLiteral("pictorio")} + arguments))
    {
        qFatal("Cannot parse command line: %s", qPrintable(parser.errorText()));
    }

    return readCommandLineOptions(parser, QStringLiteral("/home/user/photos"));
}

void TestCommandLine::readsImagePaths_data()
{
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<QStringList>("expectedImagePaths");

    QTest::newRow("no arguments") << QStringList{} << QStringList{};
    QTest::newRow("absolute path") << QStringList{QStringLiteral("/tmp/beach.jpg")}
                                   << QStringList{QStringLiteral("/tmp/beach.jpg")};
    QTest::newRow("relative path") << QStringList{QStringLiteral("2024/beach.jpg")}
                                   << QStringList{QStringLiteral("/home/user/photos/2024/beach.jpg")};
    QTest::newRow("file URL") << QStringList{QStringLiteral("file:///tmp/beach%20party.jpg")}
                              << QStringList{QStringLiteral("/tmp/beach party.jpg")};
    QTest::newRow("web URL is skipped") << QStringList{QStringLiteral("https://example.com/beach.jpg")}
                                        << QStringList{};
}

void TestCommandLine::readsImagePaths()
{
    QFETCH(QStringList, arguments);
    QFETCH(QStringList, expectedImagePaths);

    QCOMPARE(parse(arguments).imagePaths, expectedImagePaths);
}

void TestCommandLine::keepsOrderOfImages()
{
    const QStringList arguments{
        QStringLiteral("/tmp/c.jpg"), QStringLiteral("/tmp/a.jpg"), QStringLiteral("/tmp/b.jpg")};

    QCOMPARE(parse(arguments).imagePaths, arguments);
}

void TestCommandLine::offersAboutOptions_data()
{
    QTest::addColumn<QString>("option");

    QTest::newRow("help") << QStringLiteral("--help");
    QTest::newRow("version") << QStringLiteral("--version");
    QTest::newRow("author") << QStringLiteral("--author");
    QTest::newRow("license") << QStringLiteral("--license");
}

void TestCommandLine::offersAboutOptions()
{
    QFETCH(QString, option);
    QCommandLineParser parser;
    addCommandLineOptions(parser);

    // parse() fails on unknown options
    QVERIFY2(parser.parse({QStringLiteral("pictorio"), option}), qPrintable(parser.errorText()));
}

}  // namespace pictorio::utils

QTEST_GUILESS_MAIN(pictorio::utils::TestCommandLine)

#include "TestCommandLine.moc"
