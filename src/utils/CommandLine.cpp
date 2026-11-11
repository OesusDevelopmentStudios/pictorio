/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "utils/CommandLine.hpp"

#include <KAboutData>
#include <KLocalizedString>
#include <QUrl>

namespace
{

// Accepts plain paths as well as file:// URLs, which some file managers pass.
QString toLocalPath(const QString& argument, const QString& workingDirectory)
{
    const QUrl url = QUrl::fromUserInput(argument, workingDirectory, QUrl::AssumeLocalFile);
    return url.isLocalFile() ? url.toLocalFile() : QString();
}

}  // namespace

namespace pictorio::utils
{

void addCommandLineOptions(QCommandLineParser& parser)
{
    KAboutData::applicationData().setupCommandLine(&parser);
    parser.addPositionalArgument(
        QStringLiteral("images"), i18nc("@info:shell", "Images to open"), QStringLiteral("[images...]"));
}

CommandLineOptions readCommandLineOptions(const QCommandLineParser& parser, const QString& workingDirectory)
{
    CommandLineOptions options;
    for (const QString& argument : parser.positionalArguments())
    {
        const QString localPath = toLocalPath(argument, workingDirectory);
        if (!localPath.isEmpty())
        {
            options.imagePaths.append(localPath);
        }
    }

    return options;
}

}  // namespace pictorio::utils
