# Setting up your environment

The software of the remote is written in [Qt](https://www.qt.io/). In order to get the code running on your machine you should download and install Qt for Windows, Mac or Linux the [Qt open source license edition](https://www.qt.io/download-qt-installer). When running the installer, make sure you select the following components (versions at the time of writing this document):

## Mac OSX

  - macOS
  - Sources
  - Qt Virtual Keyboard
  * You can select iOS if you wish to run it on your Apple mobile device too

## Windows

  - All MSVC entries
  - MinGW 7.3.0 64-bit
  - Sources
  - Qt Virtual Keyboard
  - Qt Debug Information Files
  
## Linux (Ubuntu)

  - Desktop gcc 64-bit
  - Sources
  - Qt Virtual Keyboard
  - Qt Network Authorization
  
  * Getting error: cannot find -lGL & collect2: error: Id returned 1 exit status ? **Fix**: sudo apt-get install libgles2-mesa-dev
  
The Developer and Designer Tools are preselected and you can leave that.

> In case you need to add, remove or change components post-install, open the file "MaintenanceTool.exe" in the root of the Install directory (C:\Qt by default)

After setting up your environment you can continue to configure your [remote app](./remote%20app) project according to the information over there.
