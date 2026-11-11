/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <QDateTime>
#include <QString>

namespace pictorio::image_loader
{

// When the photo was taken, from the EXIF data (DateTimeOriginal, else DateTimeDigitized).
// Invalid if the file has no such date or cannot be read. EXIF dates carry no time zone; they are
// read as local time. Thread-safe.
QDateTime readExifCaptureDate(const QString& filePath);

}  // namespace pictorio::image_loader
