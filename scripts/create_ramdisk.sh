#! /bin/sh
test -f ramdisk.img && rm ramdisk.img
test -d ramdisk && rm -rf ramdisk
dd if=/dev/zero of=ramdisk.img bs=1k count=8k
/sbin/mkfs.ext2 -F ramdisk.img
mkdir ramdisk
sudo mount -o loop ramdisk.img ramdisk
sudo cp -rf output/fs/* ramdisk/
sudo mknod -m 622 ramdisk/dev/console c 5 1
sudo mknod -m 622 ramdisk/dev/null c 1 3
sudo umount ramdisk