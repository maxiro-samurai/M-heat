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