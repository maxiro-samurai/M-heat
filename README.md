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