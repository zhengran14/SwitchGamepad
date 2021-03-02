# SwitchGamepad

SwitchGamepad可以直接用电脑，控制你的ns。并且拥有图形化界面，方便使用。支持编写自动化脚本，也支持远程控制、手机控制。软件支持Windows和macOS。

为了实现控制ns，需要将单片机刷成模拟手柄的固件。

# 已知BUG
- 停止脚本时候，需要释放按键。

# 可能会开发
- 语言切换（中文）
- 完善的函数选择器
- 内置函数说明
- 多行注释、取消注释
- 编辑器采用web开发

# 功能
- 直接通过电脑/键盘控制ns。
- 支持编写自动化脚本。
- 支持捕捉游戏画面，并且支持图片查找功能。
- 支持客户端/服务端模式(把你ns放家里，在外面就可以控制)。
- 小窗口模式。
- 跨平台开发（支持Windows、macOS，没有试过Linux，但是理论上支持）。
- 支持iOS, iOS端地址：https://github.com/zhengran14/SwitchGamepad-iOS.

# 0. 准备
- 单片机（我用的是Arduino UNO R3）
- 串口通讯（TTL）
- 采集卡（如果想使用图片查找功能）

# 1. 将Arduino模拟成手柄
## 在macOS上（我没成功）
```
$ brew install avr-dude osx-cross/avr/avr-gcc

$ git clone --recursive https://github.com/ebith/Switch-Fightstick.git

$ cd Switch-Fightstick
$ make

# 写入固件需要把单片机处于DFU模式。
# 用测试线连接RST和GND，并在几秒钟内执行。
# m32u4对应的CPU是atmega32u4，atmega16u2用m16u2。
# 检查下你的单片机cpu型号，修改相应参数。
& avrdude -pm32u4 -cavr109 -D -P$(ls /dev/tty.usbmodem*) -b57600 -Uflash:w:Joystick.hex
```

## 在Windows上（我成功了）
可以参考这个教程：https://www.bilibili.com/read/cv4171076.

# 2. 编译SwitchGamepad
## 直接下载编译后的
等待完善
## 自己编译
1. 安装Qt。
2. 编译并安装QtAV：https://github.com/wang-bin/QtAV。
3. 安装ffmpeg。（Windows不需要，macOS可通过brew安装）
4. 安装opencv。（Windows可以直接解压使用‘opencv_win32.7z’，macOS可通过brew安装）
5. 下载代码。
   ```
   $ git clone https://github.com/zhengran14/SwitchGamepad.git
   ```
6. 用QtCreator打开工程。
7. 修改pro文件，将ffmpeg、qtav、opencv的路径修改为自己的路径。（只需要修改对应的系统就行，Windows或者是macOS）
8. 编译工程并运行。
9. 如果需要直接运行编译出来的.exe或者.app，需要使用deployqt来增加运行库。
    > 在macOS下，如运行提示：
    > ```
    > $ Qt缺失libiodbc.2.dylib和libpq.5.dylib
    > ```
    > 则可以参考该教程：https://blog.csdn.net/yutianyue126/article/details/106911948。

# 如何使用SwitchGamepad
等待完善。

# 脚本demo
[圣柱王demo脚本](https://github.com/zhengran14/SwitchGamepad/blob/master/scripts%20demo/%E5%9C%A3%E6%9F%B1%E7%8E%8Bdemo%E8%84%9A%E6%9C%AC.js)

# 脚本函数
```
// 休眠
// 可以直接用软件上方选择想要的时间添加该函数
gp.sleep(秒); // 可以是小数来表示毫秒
gp.sleep(0.7);
gp.sleep(5.5);

// 设置文本到状态栏（软件上的状态栏）
gp.statusText(字符串);
gp.statusText(‘Start’);

// 按下一个按钮并且延迟一定时间后弹起
// 可以直接用软件上方选择想要的按钮和时间添加该函数
gp.pressButton(按钮名称, 秒); // 可以是小数来表示毫秒
gp.pressButton('Button X', 0.1);
gp.pressButton('LX MIN’, 5.5);

// 发送邮件
// 目前只支持qq邮箱
gp.mail(发件人邮箱, 邮箱的授权密码, 收件人邮箱, 邮件标题, 邮件内容);
gp.mail(‘xxx@qq.com', 'xxxxxx’, ‘xxx@qq.com', ‘title’, ’content’);

// 弹出对话框
gp.messageBox(标题, 内容);
gp.messageBox(‘title’,’content’);

// 查找画面里目标图片所在坐标
// 返回true或者false，是否是所要的坐标
gp.judgeCapture(目标图片路径, x坐标, y坐标); // 坐标必须是整数，路径必须需使用双斜杠
gp.judgeCapture('C:\\xxx\\000.jpg', 110, 590); // 000.jpg是否在画面的110、590的坐标位置，是返回true，不是返回false

// 测试函数：查找画面里目标图片所在坐标
// 返回字符串，目标图片在画面中的坐标位置
gp.judgeCaptureTest(目标图片路径);
gp.judgeCaptureTest('C:\\xxx\\000.jpg’); // 如果目标图片在110、590，则返回“110x590”


// 生成查找目标图片，图片直接回保存到保存路径
gp.capture(保存路径, x坐标, y坐标, 图片宽度, 图片高度); // 坐标和宽高必须是整数，路径必须需使用双斜杠
gp.capture('C:\\xxx\\000.jpg', 110, 590, 100, 45); // 截取画面中的110、590未知的，长宽为100x45的图片保存在路径中
```
