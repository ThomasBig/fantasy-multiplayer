---
marp: true
theme: bespoke
_class: invert
---

# **Fantasy Chat:** *Simple Roleplaying Multiplayer Game*

Tomáš Janoušek

---

# Project Information
* players choose **ancestry** (elf, dwarf, human) and their **name**
* **dynamic time**: morning, afternoon, evening, night
* **weather**: it rains at random intervals
* players choose to write message to all players or a single player

---
<!-- class: invert -->


# **First iteration**
* Programs Installed
* C++ Cmake Project
* ASIO included
* Git setup

---
<!-- class: default -->

# Programs
* Visual Studio 2020 C++ compiler: to compile C++20 code
* Git: to download ASIO library and versioning
* VS Code to write code
  * C/C++ Extension: IntelliSense, Debugging
  * CMake Extension: Syntax for CMake files
  * CMake Tools Extension: Sleek buttons

---

# Project Setup

1. Create an empty folder and open it with vs code.

2. Clone asio of a 1.32.0 version. Please use following command:

    `git clone --depth 1 --branch asio-1-32-0 https://github.com/chriskohlhoff/asio.git`

    > Argument `--depth 1` skips downloading the whole history and downloads a specific version.

* Create `main.cpp` with a sample C++/Asio program

---

# Wait for it... Hello World!

```c++
#include <iostream>
#include <asio.hpp>

int main() {
    asio::io_context context;
    asio::steady_timer timer(context, asio::chrono::seconds(3));
    std::cout << "Wait for it... ";
    timer.async_wait([](asio::error_code _err){
        std::cout << "Hello, world!\n";
    });
    context.run();
    return 0;
}
```

---

# CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(FantasyChat)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    # Windows 10 minimum requirement
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

# Add the include directory for ASIO
include_directories(asio/asio/include)

add_executable(FantasyChat main.cpp)
```

---

# Git Setup

Once in root of the project, use following command
```
git init
```

Also, create a new file `.gitignore` with following content

```
asio/
build/
```

## Commit changes
Use VS Code left panel or git commands.