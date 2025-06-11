#! /bin/sh

test -d output && rm -rf output

# loader.bin
cd src/AT91RM9200-Loader/
make clean
cd ../..

# boot.bin
cd src/AT91RM9200-Boot/
make clean
cd ../..

# linux kernel
cd src/linux-2.6.33.5/
make clean distclean
cd ../..

# busybox
cd src/busybox-1.13.2/
make clean distclean
cd ../..

# ip
cd ./src/utils/iproute2-2.6.33/
make clean distclean
cd ../.. 
cd ../

# canutils
cd ./src/utils/canutils-3.0.2/
rm -rf output
make clean distclean
cd ../..
cd ..

test -f ramdisk.img && rm ramdisk.img
test -d ramdisk && rm -rf ramdisk