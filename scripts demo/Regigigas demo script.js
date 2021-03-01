var total = 8000;
var result = false;
var i = 1;
for (i = 1; i <= total; i++) {
    gp.statusText(i + "/" + total);

    // Got to home and reopen game
    gp.pressButton('Button HOME',0.1);
    gp.sleep(0.5);
    gp.pressButton('Button X',0.1);
    gp.sleep(0.2);
    gp.pressButton('Button A', 0.1);
    gp.sleep(3);
    gp.pressButton('Button A', 0.1);
    gp.sleep(1);
    gp.pressButton('Button A', 0.1);
    gp.sleep(1);
    gp.pressButton('Button A', 0.1);
    gp.sleep(16);
    gp.pressButton('Button A', 0.1);
    gp.sleep(7);
    gp.pressButton('Button A', 0.1);
    gp.sleep(4);
    gp.pressButton('Button A', 0.1);
    gp.sleep(0.5);
    gp.pressButton('Button A', 0.1);

    // judge shine
    gp.sleep(19);
//    gp.capture('C:\\xxx\\000.jpg',110,590,100,45);
    if (!gp.judgeCapture('C:\\xxx\\000.jpg',110,590)) {
        gp.pressButton('Button CAPTURE',0.1);
        gp.sleep(0.5);
        gp.pressButton('Button CAPTURE',0.1);
        gp.sleep(0.5);
        gp.pressButton('Button CAPTURE',0.1);
        result = true;
        break;
    }
}

var currentDate = new Date();
var currentHour = currentDate.getHours();
if (currentHour >= 0 && currentHour <= 7) {
    gp.pressButton('Button HOME',2);
    gp.pressButton('Button A',0.1);
}
else {
    gp.pressButton('Button HOME',0.1);
}

if (result) {
    gp.statusText('Shine Got: ' + i);
    gp.mail('xxx@qq.com', 'password', 'xxx@qq.com', '!!!!!','Shine Got: ' + i);
    gp.messageBox('!!!!!', 'Shine Got: ' + i);
}
else {
    gp.statusText('No Shine: ' + i);
    gp.mail('xxx@qq.com', 'password', 'xxx@qq.com', '!!!!!','No Shine: ' + i);
    gp.messageBox('!!!!!', 'No Shine: ' + i);
}