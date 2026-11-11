/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "utils/AppIdentity.hpp"

#include <KAboutData>
#include <KLocalizedString>

namespace pictorio::utils
{

void registerApplicationIdentity()
{
    // Must come first: the i18nc() calls below already need the translation domain.
    KLocalizedString::setApplicationDomain(applicationComponentName.toUtf8());

    KAboutData aboutData(applicationComponentName,
                         i18nc("@title", "Pictorio"),
                         QStringLiteral(PICTORIO_VERSION),
                         i18nc("@info:credit", "Image viewer and photo library"),
                         KAboutLicense::Unknown,
                         i18nc("@info:credit", "© 2026 OesusDevelopmentStudios"));
    aboutData.setLicense(KAboutLicense::GPL_V3, KAboutLicense::OrLaterVersions);
    aboutData.setDesktopFileName(applicationId);
    aboutData.setOrganizationDomain(organizationDomain);

    KAboutData::setApplicationData(aboutData);
}

}  // namespace pictorio::utils
