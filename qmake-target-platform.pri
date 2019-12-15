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

win32 {
    CONFIG += PLATFORM_WIN
    message(PLATFORM_WIN)
    win32-g++ {
        CONFIG += COMPILER_GCC
        message(COMPILER_GCC)
    }
    win32-msvc* {
        # looks like they changed the config values with Qt 5.9: https://stackoverflow.com/a/46050538
        # Where's the damn documentation of available environment variables? Try and error with non-working Stackoverflow solutions ain't funny!
        #message($$CONFIG)
        # last resort: print everything to find the magic variable name...
        # WARNING: ONLY FOR DEBUGGING! This is slow and prints a LONG list
        #for(var, $$list($$enumerate_vars())) {
        #    message($$var)
        #    message($$eval($$var))
        #}
        MSVC_VER = $$(VisualStudioVersion)
        equals(MSVC_VER, 14.0){
            message("Visual Studio version: $$(VisualStudioVersion): MSVC 2015")
            CONFIG += COMPILER_MSVC2015
            message(COMPILER_MSVC2015)
        }
        equals(MSVC_VER, 15.0){
            message("Visual Studio version: $$(VisualStudioVersion): MSVC 2017")
            CONFIG += COMPILER_MSVC2017
            message(COMPILER_MSVC2017)
        }
        # untested!
        equals(MSVC_VER, 16.0){
            message("Visual Studio version: $$(VisualStudioVersion): MSVC 2019")
            CONFIG += COMPILER_MSVC2019
            message(COMPILER_MSVC2019)
        }

       # Probably no longer required. Initially taken from a Qt5 book which used no longer valid win32-msvc2017 directives
        #win32-msvc:QMAKE_TARGET.arch = x86_64
    }

}

linux {
    CONFIG += PLATFORM_LINUX
    message(PLATFORM_LINUX)
    # Make QMAKE_TARGET arch available for Linux
    !contains(QT_ARCH, x86_64){
        QMAKE_TARGET.arch = x86
    } else {
        QMAKE_TARGET.arch = x86_64
    }
    linux-g++{
        CONFIG += COMPILER_GCC
        message(COMPILER_GCC)
    }
}

macx {
    CONFIG += PLATFORM_OSX
    message(PLATFORM_OSX)
    macx-clang {
        CONFIG += COMPILER_CLANG
        message(COMPILER_CLANG)
        QMAKE_TARGET.arch = x86_64
    }
    macx-clang-32{
        CONFIG += COMPILER_CLANG
        message(COMPILER_CLANG)
        QMAKE_TARGET.arch = x86
    }
}

contains(QMAKE_TARGET.arch, x86_64) {
    CONFIG += PROCESSOR_x64
    message(PROCESSOR_x64)
} else {
    # TODO also test for arm?
    CONFIG += PROCESSOR_x86
    message(PROCESSOR_x86)
}

CONFIG(debug, release|debug) {
    CONFIG += BUILD_DEBUG
    message(BUILD_DEBUG)
} else {
    CONFIG += BUILD_RELEASE
    message(BUILD_RELEASE)
}
