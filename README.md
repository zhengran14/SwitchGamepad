# SwitchGamepad

[中文README](https://github.com/zhengran14/SwitchGamepad/blob/master/README_zh_cn.md)

SwitchGamepad can communicate with microcomputer such as Arduino to control Nintendo Switch with UI instead of python or terminal.

The microcomputer need to write binaries to simulate ns gamepad.

# Known BUG
- When stop script, need to release button.

# Maybe to develop
- Language switch (Chinese)
- Full function selector
- Build-in function description
- Multi-line comment/uncomment
- Redevelop script editor by web

# What can I do
- Control ns by computer (also by keyboard).
- Control ns by scirpt (auto run).
- Capture game screen and judge something by pic search.
- Client/Server mode (Put your ns at home and control it outside).
- Mini tool mode.
- Cross-platform, run anywhere (macOS or Windows, not try on Linux).
- Supprot iOS, iOS client: https://github.com/zhengran14/SwitchGamepad-iOS.

# 0. Prepare
- Microcomputer (I use Arduino UNO R3)
- USB to serial adapter (TTL)
- Video Capture (if you need to use script 'judgeCapture()').

# 1. Simulate Arduino as Gamepad
## On macOS (Failed)
```
$ brew install avr-dude osx-cross/avr/avr-gcc

$ git clone --recursive https://github.com/ebith/Switch-Fightstick.git

$ cd Switch-Fightstick
$ make

# Pro Micro needs to be in DFU mode to write binaries.
# Connect the RST and GND of the board with a test wire and execute within a few seconds.
# The option m32u4 is when the CPU is atmega32u4. For atmega16u2, use m16u2.
# You need change the CPU to your own.
& avrdude -pm32u4 -cavr109 -D -P$(ls /dev/tty.usbmodem*) -b57600 -Uflash:w:Joystick.hex
```

## On Windows (Successed)
Follow by (Chinese): https://www.bilibili.com/read/cv4171076.

# 2. Build SwitchGamepad
## Download Release
Wait for
## Build yourself
1. Install Qt.
2. Build and install QtAV: https://github.com/wang-bin/QtAV.
3. Install ffmpeg. (No need for Windows, you can use brew for macOS)
4. Install opencv. (You can unzip and use 'opencv_win32.7z' for Windows, for macOS can install by brew)
5. Download code.
   ```
   $ git clone https://github.com/zhengran14/SwitchGamepad.git
   ```
6. Open project by QtCreator.
7. Edit .pro, replace the path of ffmpeg, qtav and opencv to your own. (Replace the part of your system, Windowsor macOS)
8. Build and run the project.
9. If you need to run .exe or .app you build directly，please use deployqt to add dependent libraries.
    > For macOS, if show
    > ```
    > $ missing libiodbc.2.dylib and libpq.5.dylib
    > ```
    > you can see (Chinese): https://blog.csdn.net/yutianyue126/article/details/106911948.

# How to use SwitchGamepad
Wait for finish.

# Script demo
[Regigigas demo script](https://github.com/zhengran14/SwitchGamepad/blob/master/scripts%20demo/Regigigas%20demo%20script.js)

# Function of Script
Wait for comment.
```
gp.sleep(second);
gp.sleep(0.7);
gp.sleep(5.5);

gp.statusText(string);
gp.statusText(‘Start’);

gp.pressButton(Button code, second);
gp.pressButton('Button X', 0.1);
gp.pressButton('LX MIN’, 5.5);

gp.pressDownButton(Button code);
gp.pressDownButton('Button X' code);
gp.releaseButton();

// Only support qq
gp.mail(sender, sender, receiver, title, content);
gp.mail(‘xxx@qq.com', 'xxxxxx’, ‘xxx@qq.com', ‘title’, ’content’);

gp.messageBox(title, content);
gp.messageBox(‘title’, ’content’);

gp.judgeCapture(pic path, x, y);
gp.judgeCapture('C:\\xxx\\000.jpg', 110, 590);

gp.judgeCaptureTest(pic path);
gp.judgeCaptureTest('C:\\xxx\\000.jpg’);


gp.capture(pic save path, x, y, width, height);
gp.capture('C:\\xxx\\000.jpg', 110, 590, 100, 45);
```
