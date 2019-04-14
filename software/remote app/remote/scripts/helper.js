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
    var returnString = hours + ":" + convertToTwoDigits(minutes) + ":" + convertToTwoDigits(seconds);
    return returnString;
}

function convertToTwoDigits(n){
    return n > 9 ? "" + n: "0" + n;
}

function convertToPercentage (value) {
    return Math.round(value/255*100);
}
