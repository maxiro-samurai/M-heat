# M-heat 迷你加热台项目

焊接电路使用电烙铁太麻烦了，尤其遇到0402、QFN这种封装的很难受，想做一个加热板并且还要够mini。本次项目主要参考了 

* 找羊大佬 [加热台量产计划](https://oshwhub.com/sheep_finder/pcb-heng-wen-jia-re-tai)

* 小O和小Q [[PD协议 | 高颜值]mini加热台](https://oshwhub.com/littleoandlittleq/bian-xie-jia-re-tai)

* 稚晖君 [OpenHeat] (https://github.com/peng-zhihui/OpenHeat)

# 项目介绍

主要结构部分采用mini加热台整体框架，外壳自己重新设计。OLED屏幕换成更大一点的128*64，主控芯片采用ESP32，不使用外接电源只需要一根TYPE-C，功率为65W。














# 记录
* 2024/9/12   目前正在打板验证，本次提交是创建项目仓库。

* 2024/9/14   电源部分设计有问题，DC-DC降压模块稳定工作需要一定压降，TPS5430和MP1584EN输入5V输出必须比5V低。准备更改电源方案，输入转3.3V，3.3V再升压得到5V。

* 2024/11/19  近期在完善打印机，准备加快速度完成本项目，之前电路已经更改暂时未打板验证。

* 2024/12/3   更改电路踩坑记录: 1.type-c母座不要虚焊，cc1 cc2引脚可以不用下拉电阻，ch224k芯片内部已经内置下拉5.1K电阻。2.ch224k芯片背部GND焊盘一定要焊上，不然诱导不出来20V电压。3.充电器一定要用带支持PD20V协议的，一开始诱导不出来后面换了个充电头就可以诱导出来了。4.LM358引脚一定要看好，在找羊原项目里按照原理图画的8脚跟4脚搞反了，8脚是电源正。5.ESP32下载电路有问题，插上type-c后电脑可以识别CH340芯片，用示波器看EN跟BOOT0引脚也有跳变，就是波形可能不符合上电顺序导致无法进入下载程序模式，参考别人设计后发现他们en引脚都上拉电阻并且连了一个电容做延时，所以电路还需要继续完善。6.蜂鸣器封装需要更换。

* 2025/4/14   快开始找工作了，加快项目进度，之前电路修改部分已完成。软件部分把UI、蜂鸣器、编码器、已经移植。目前正在调试ADC测温电路，没有搞清原项目中的测温原理。

    ![image](https://github.com/maxiro-samurai/picx-images-hosting/raw/master/image.86tu7igduf.webp)

* 2025/4/22   想到后面调PID每次都需要重新下载烧录代码很麻烦，所以想到用野火的串口助手，里面有调试PID的功能。但是需要满足野火通信的协议，参考[踏雪Vernon](https://github.com/taxue-vernon/firetool_PID_driver_esp)大佬的代码把功能移植到本项目中。当前面临问题：只有串口0能用，串口0还是下载程序的串口，软件把下载串口占用导致无法下载程序，我在改进后的板子上在EN引脚上添加了一个复位按键，理论上来说可以下载程序。担心是多余的，自动下载电路中有硬件流控，能使ESP32自动复位进入bootloader。软件部分还需要增加保存PID、温度、时间等一些温度的设置功能，这就需要非易失存储器保存数据，比如eeprom。这里使用ESP32中的NVS来存储，详情看[ESP32 非易失性存储器NVS](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.4.1/esp32s3/api-reference/storage/nvs_flash.html#id2)。还忽略了一点，如果使用野火串口助手则无法开启加热功能，因为电脑端USB口无法输出65W。最终调试PID还是没用到串口助手，而是使用WIFI网页控制进行调试，如果想使用串口可以多引出一个串口1用来调试。

* 2025/5/14   使用[thingsboard](https://github.com/thingsboard/thingsboard)开源物联网框架搭建服务器，实现在网页端显示温度，PID参数并且对设备进行控制。

    ![image](https://github.com/maxiro-samurai/picx-images-hosting/raw/master/image.70ak5h8d6q.webp)


