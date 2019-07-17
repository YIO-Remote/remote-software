function mapValues (x,a,b,c,d) {
    var leftSpan = b-a;
    var rightSpan = d-c;

    var valueScaled = parseFloat(x-a) / parseFloat(leftSpan);

    return parseInt(c+(valueScaled * rightSpan));

}

function secondsToHours (value) {
    var hours = Math.floor(value / 3600);
    value %= 3600;
    var minutes = Math.floor(value / 60);
    var seconds = value % 60;

    if (hours == 0) {
        hours = "";
    } else {
        hours = hours + "h ";
    }

    if (minutes == 0) {
        minutes = "";
    } else {
        minutes = minutes + "m ";
    }

    seconds = seconds + "s";

    var returnString = hours + minutes + seconds;
    return returnString;
}

function convertToTwoDigits(n){
    return n > 9 ? "" + n: "0" + n;
}

function convertToPercentage (value) {
    return Math.round(value/255*100);
}

function saveConfig() {
    //language
    config.language = language;

    //entities
    for (var i=0; i<loaded_entities.length; i++) {
        for (var k=0; k<config.entities.length; k++) {
            if (config.entities[k].type == loaded_entities[i].obj) {

                for (var x=0; x<config.entities[k].data.length; x++) {
                    var e = entities.get(config.entities[k].data[x].entity_id);
                    config.entities[k].data[x] = e.getDataToSave();
                }
            }
        }
    }

    //save config file
    jsonConfig.write(config);
}
