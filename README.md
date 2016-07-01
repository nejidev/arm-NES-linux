# arm-NES-linux-
测试环境：
linux kernel 4.1 s3c2440 开发板
基于 linux 的 绘制 /dev/fb0 实现的 NES 模拟器，移值于 InfoNES ，手柄控制


编译方式：
cd arm-NES-linux-/linux/
make


使用方式：
应该先加载手柄驱动后，在把
得到 InfoNES 复制到 开发板上 运行即可。

joypad 中是 红白机 linux 下驱动源码，及测试程序。


编译方式：
cd arm-NES-linux-/joypad/
make 


使用方式：
复制 joypad.ko 到开发板上 ， insmod joypad.ko

或者使用 joypad 里面的 test.c 进行测试。

注意： 并不是所有 nes 格式游戏都能支持。 

后期，可能会加入，全屏显示，声音。 

其它，请在 博客上  http://www.cnblogs.com/ningci/ 上交流。
