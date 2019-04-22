function checkForUpdate(update) {
    var url = "https://api.github.com/repos/martonborzak/yio-remote/releases/latest"
    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.responseType = 'json';
    xhr.onreadystatechange = function() {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                var obj = JSON.parse(xhr.responseText);
                if (obj.tag_name >= _current_version + 0.1) {
                    console.debug("New version is available")
                    console.debug(obj.assets[0].browser_download_url);
                    _new_version = obj.tag_name;
                    updateURL = obj.assets[0].browser_download_url;
                    updateAvailable = true;
                    if (autoUpdate) {
                        console.debug("Updating the app")
                        var command = "/usr/bin/remote/updater.sh " + obj.assets[0].browser_download_url;
                        mainLauncher.launch(command);
                        updateAvailable = false;
                    }
                } else {
                    console.debug("No update");
                    updateAvailable = false;
                }
            }
            if (xhr.status === 404) {
                console.debug("Update error")
            }
        }
    }
    xhr.send();
}
