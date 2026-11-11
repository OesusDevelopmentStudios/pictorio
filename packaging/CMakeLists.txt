# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

# Files that integrate the application into the desktop. All are named after the application ID,
# which Flatpak requires for every file it exports to the host.

# Start menu entry and "Open with" registration for the listed image types
install(FILES io.github.OesusDevelopmentStudios.Pictorio.desktop
    DESTINATION ${KDE_INSTALL_APPDIR}
)

# App description for software centres; checked by the appstreamtest test after installation
install(FILES io.github.OesusDevelopmentStudios.Pictorio.metainfo.xml
    DESTINATION ${KDE_INSTALL_METAINFODIR}
)

install(FILES icons/io.github.OesusDevelopmentStudios.Pictorio.svg
    DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps
)
