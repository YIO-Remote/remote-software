## How to get it runnig on your computer

1. Download an install [Qt](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)

2. Edit the [config.json](config.json) file. Enter your Home Assistant ip:port and your token.

3. Edit line 76 and 109 in [main.qml](main.qml) to change the path.

That's it you're ready to try it out.


## Folder structure
```
app folder
↳ basic_ui
    Contains basic UI elements like charging screen, status bar etc.
  ↳ main_navigation
    Contains files for the navigation that appears on the bottom of the screen
  ↳ components
    Folder for the various components that control an entity: for example light. Each component has its own subfolder.
    ↳ light
      Button.qml
      This is the UI for the component. It has an open and closed state.
      
      Main.qml
      This is the main qml file for the component.
      This file gets loaded after the configuration is loaded in main.qml. When the component is loaded it looks for which integration to load and loads it.
      It has two variables:
      - entities: holds the entities that are defined in the config.json file
      - ComponentIntegration: object that holds the loaded integration
      
      homeassistant.qml
      There should be a file for every integration. The name should match the file in /integrations.
      This file gets the data from the main integration and processes it.
``` 
