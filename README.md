# Pictorio

Manage your images in an organised way.

Pictorio is an image viewer and photo library for KDE Plasma.

- **On its own** it is a simple image viewer: open an image from Dolphin, zoom, rotate, step through
  the folder, run a slideshow.
- **Connected to a [pictorio-db](https://github.com/OesusDevelopmentStudios/pictorio-db) server**
  it becomes a tag-based photo library: search by tags, browse a thumbnail grid, edit tags, upload
  photos.

> **Status:** early development. The application opens a single image (from Dolphin or the command
> line) and can zoom and rotate it.

## Building

### Dependencies

| Dependency | Minimum version |
|---|---|
| CMake | 3.25 |
| C++23 compiler (GCC or Clang) | |
| Qt (Core, Concurrent, Gui, Qml, Quick, QuickControls2, Widgets, Test) | 6.8 |
| KDE Frameworks (Extra CMake Modules, Kirigami, KI18n, KCoreAddons) | 6.21 |
| KDE desktop style for Qt Quick (qqc2-desktop-style), needed at run time | |
| Qt and KDE image format plugins (qtimageformats, kimageformats), needed at run time for WebP, TIFF, AVIF, HEIF and JPEG XL | |

On Fedora:

```sh
sudo dnf install cmake gcc-c++ extra-cmake-modules \
    qt6-qtbase-devel qt6-qtdeclarative-devel \
    kf6-kirigami-devel kf6-ki18n-devel kf6-kcoreaddons-devel \
    kf6-qqc2-desktop-style qt6-qtimageformats kf6-kimageformats
```

HEIF images also need an HEVC decoder for libheif, which Fedora does not ship. The Flatpak build
opens them.

### Build and run

```sh
cmake -B build
cmake --build build
./build/bin/pictorio *path_to_image_file*
```

Build options:

| Option | Default | Meaning |
|---|---|---|
| `BUILD_TESTING` | `ON` | Build the unit tests |
| `PICTORIO_WARNINGS_AS_ERRORS` | `ON` | Treat compiler warnings as errors. Turn off for packaging |

Example: `cmake -B build -DBUILD_TESTING=OFF -DPICTORIO_WARNINGS_AS_ERRORS=OFF`

### Tests

```sh
ctest --test-dir build --output-on-failure
```

Check the QML files with Qt's linter:

```sh
cmake --build build --target pictorio_qmllint
```

### Flatpak

Needs `flatpak-builder` and the KDE runtime from Flathub
(`flatpak install flathub org.kde.Sdk//6.11 org.kde.Platform//6.11`). From the repository root:

```sh
flatpak-builder --user --install --force-clean build-flatpak \
    packaging/flatpak/io.github.oesusdevelopmentstudios.pictorio.yml
flatpak run io.github.oesusdevelopmentstudios.pictorio
```

This builds the current source tree, including uncommitted changes.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for the code layout and coding style.

## License

Pictorio is free software, licensed under the
[GNU General Public License, version 3 or later](LICENSE) (`GPL-3.0-or-later`).
