/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <QCommandLineParser>
#include <QString>
#include <QStringList>

namespace pictorio::utils
{

struct CommandLineOptions
{
    QStringList imagePaths;  // absolute local paths, in the order given
};

// Adds the image arguments and KAboutData's options (--help, --version, --author, --license).
// Call after registerApplicationIdentity().
void addCommandLineOptions(QCommandLineParser& parser);

// Reads a parsed command line. Relative paths are resolved against workingDirectory;
// non-local URLs (http://...) are skipped.
CommandLineOptions readCommandLineOptions(const QCommandLineParser& parser, const QString& workingDirectory);

}  // namespace pictorio::utils
