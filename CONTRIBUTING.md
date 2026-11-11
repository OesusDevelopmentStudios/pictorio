# Contributing to Pictorio

Readability comes first. The code should be easy to understand and maintain for someone who has
never seen it before.

## Workflow

- **Small commits.** Each commit does one thing, builds, passes the tests and can be reviewed on its
  own.
- **Tests come with the code.** New logic comes with a unit test in `tests/`.
- Before committing:
  ```sh
  clang-format -i <changed C++ files>
  cmake --build build && ctest --test-dir build --output-on-failure
  cmake --build build --target pictorio_qmllint
  ```

## Code layout

```
src/
├── main.cpp            starts the application; connects the pieces, contains no logic
├── app_manager/        the primary manager: creates the services and makes them available to QML
├── services/           one folder per service, each with a single responsibility
├── utils/              small helpers shared by several services
└── qml/                user interface (presentation only)
tests/                  mirrors src/: tests/utils/test_app_identity.cpp tests src/utils/app_identity.cpp
```

**QML contains presentation only.** Decisions (sorting, paging, what to upload, when to delete) live in
C++ services, where they can be unit-tested.

### Service layout

```
services/<service_name>/
├── include/
│   ├── <service_name>.hpp            public interface: the only header other code may include
│   └── <service_name>_factory.hpp    creates the service
├── impl/                             implementation files (.cpp/.hpp)
├── hlp/                              helpers used only by this service (.cpp/.hpp)
├── <service_name>.cpp
└── <service_name>_factory.cpp
```

- Code lives in `namespace pictorio::<service_name>`.
- Services never create each other. The app manager creates them through their factories and passes
  in their dependencies. Tests can then pass in fakes.
- Each service is built as a static library, so the application and the tests link the same code.

## C++ style

Formatting is enforced by [.clang-format](.clang-format): Allman braces, 4 spaces, at most 120
columns, one argument per line when a call does not fit on one line.

### Naming

| What | Style | Example |
|---|---|---|
| Classes, structs, enums | `PascalCase` | `FolderScanner` |
| Functions, variables | `camelCase` | `registerApplicationIdentity()` |
| Member variables | `m_` + `camelCase` | `m_currentIndex` |
| Files | `snake_case` | `app_identity.cpp` |
| Namespaces | `pictorio::snake_case` | `pictorio::folder_scanner` |
| Test files | `test_<file under test>.cpp` | `test_app_identity.cpp` |

- **Names are verbose.** No single-letter names, not even for loop counters: `imageIndex`, not `i`.
- **Each function has a single responsibility.**

### `.cpp` file layout

```cpp
/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "own_header.hpp"          // 1. this file's own header

#include <vector>                  // 2. standard library

#include <QString>                 // 3. external libraries (Qt, KDE Frameworks, ...)

#include "utils/app_identity.hpp"  // 4. common helpers

#include "impl/local_file.hpp"     // 5. local files of this service

namespace
{
// constants and helper functions used only in this file
}  // namespace

namespace pictorio::service_name
{
// service functions
}  // namespace pictorio::service_name
```

Groups are separated by a blank line; clang-format sorts includes only within a group.

### `.hpp` file layout

```cpp
/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

namespace pictorio::service_name
{

class Example
{
    // structs / enums used only by this class, then type aliases

public:
    Example();
    ~Example();

    // public functions

private:
    // private functions

    // member variables
};

}  // namespace pictorio::service_name
```

## QML style

QML uses the usual QML brace style (`Item {` on the same line) and 4 spaces. User-visible strings go
through `KI18n.i18nc("@context", "Text")` (`import org.kde.ki18n`), so translators get context and
qmllint can check the calls.

## License headers

Every source file starts with an SPDX header:

| File type | Header |
|---|---|
| C++ | `/* SPDX-License-Identifier: GPL-3.0-or-later ... */` block, as above |
| QML | `// SPDX-License-Identifier: GPL-3.0-or-later` + `// SPDX-FileCopyrightText: ...` |
| CMake, YAML, `.clang-format` | `# SPDX-License-Identifier: GPL-3.0-or-later` + `# SPDX-FileCopyrightText: ...` |
