#!/bin/bash
# create_flash_img.sh
OUT_FILE="output/flash.bin"
FLASH_SIZE=16777216  # 16MB

test -f $OUT_FILE && $OUT_FILE

# 创建空白镜像
dd if=/dev/zero bs=$FLASH_SIZE count=1 | tr "\000" "\377" > $OUT_FILE

# 生成uboot_env
mkenvimage -s 0x20000 -o output/uboot_env.bin boot/uboot_env.txt

# 生成rootfs.jffs2
test -f output/rootfs.jffs2 && output/rootfs.jffs2
mkfs.jffs2 -d output/fs/ -o output/rootfs.jffs2 -e 0x20000 -n --pad=0x00dc0000

# 填充组件
dd if=output/boot.bin bs=1024 conv=notrunc of=$OUT_FILE                  # boot.bin
dd if=output/u-boot.bin.gz bs=1024 conv=notrunc of=$OUT_FILE seek=64          # u-boot.bin.gz
dd if=output/uboot_env.bin bs=1024 conv=notrunc of=$OUT_FILE seek=128         # uboot_env.bin
dd if=output/uImage bs=1024 conv=notrunc of=$OUT_FILE seek=256         # uImage
dd if=output/rootfs.jffs2 bs=1024 conv=notrunc of=$OUT_FILE seek=2304        # rootfs.jffs2

echo "Generated $OUT_FILE with SHA1: $(sha1sum $OUT_FILE)"