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

