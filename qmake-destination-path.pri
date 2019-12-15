###############################################################################
 #
 # Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
 #
 # This file is part of the YIO-Remote software project.
 #
 # YIO-Remote software is free software: you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # YIO-Remote software is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 #
 # SPDX-License-Identifier: GPL-3.0-or-later
 #############################################################################/

platform_path = unknown-platform
compiler_path = unknown-compiler
processor_path = unknown-processor
build_path = unknown-build

PLATFORM_WIN {
    platform_path = windows
}
PLATFORM_OSX {
    platform_path = osx
}
PLATFORM_LINUX {
    platform_path = linux
}

COMPILER_GCC {
    compiler_path = gcc
}
COMPILER_MSVC2017 {
    compiler_path = msvc2017
}
COMPILER_CLANG {
    compiler_path = clang
}

PROCESSOR_x64 {
    processor_path = x64
}
PROCESSOR_x86 {
    processor_path = x86
}

# Special treatment for cross compiling!
equals(QT_ARCH, arm): {
    platform_path = linux
    compiler_path = gcc
    processor_path = arm
}

BUILD_DEBUG {
    build_path = debug
} else {
    build_path = release
}

DESTINATION_PATH = $${platform_path}-$${compiler_path}-$${processor_path}/$$build_path
message(Dest path: $${DESTINATION_PATH})

# Configure destination path by "Operating System/Compiler/Processor Architecture/Build Configuration"
DESTDIR = $$(YIO_BIN)
DESTDIR_BIN = $$(YIO_BIN)
isEmpty(DESTDIR) {
    DESTDIR = $$(YIO_SRC)
    isEmpty(DESTDIR) {
        DESTDIR_BIN = $$clean_path($$PWD/../binaries)
        DESTDIR = $$DESTDIR_BIN/$$DESTINATION_PATH
        warning(Environment variables YIO_BIN and YIO_SRC not defined! Using '$$DESTDIR' as binary output directory.)
    } else {
        DESTDIR_BIN = $$clean_path($$(YIO_SRC)/binaries)
        DESTDIR = $$DESTDIR_BIN/$$DESTINATION_PATH
        message(YIO_SRC is set: using '$$DESTDIR' as binary output directory.)
    }
} else {
    message(YIO_BIN defined '$$DESTDIR' as binary output directory.)
}
