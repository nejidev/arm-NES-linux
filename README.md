# arm-NES-linux emulate for InfoNES open source
## Planform
	Raspberry Pi or Arm linux or ubuntu
	Direct draw /dev/fb0 Framebuffer

## Build
	cd arm-NES-linux-/linux/
	make

## Build joypad linux kernel module
	cd arm-NES-linux-/joypad/
	make


## USE
	insmod joypad.ko
	cd arm-NES-linux-/linux/
	./InfoNES SuperMario.nes

## Change Log
	2020-07-05 Add Qt project

## Preview
![avatar](https://github.com/nejidev/arm-NES-linux/blob/master/Qt/qt_nes.jpg?raw=true )

## 说明
	为什么会有这么样一个项目，起初发现有人在 STM32F103上实现了，于是找到源码开始研究，最早可以在 ARM 裸板上运行，
	但是 ALSA 的声音一直有问题，后来修复。
	最近又在学习 QT 于是，用 QT 重新适配一版。
	Qt 对中文支持不好，所以请不要选择带有中文路径的 nes 文件。

 