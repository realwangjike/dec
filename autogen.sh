#! /bin/sh

OLD_PATH="${PATH}"

test -d output && rm -rf output
mkdir output

# 编译loader.bin
cd src/AT91RM9200-Loader/
make clean
make 
cd ../..

# 编译boot.bin
# 编译loader.bin
cd src/AT91RM9200-Boot/
make clean
make 
cd ../..


export ARCH=arm
export CROSS_COMPILE=arm-linux-
export PATH=/usr/local/arm/4.3.2/bin/:${OLD_PATH}

cd src/linux-2.6.33.5/
make clean distclean
make at91rm9200dk_defconfig
make all -j $(($(nproc) * 16))
mkimage -A arm -O linux -T kernel -C none \
    -a 0x20008000 -e 0x20008000 -n "Linux-2.6.33.5" \
    -d arch/arm/boot/zImage arch/arm/boot/uImage
cd ../..

# 编译busybox
cd src/busybox-1.13.2/
make clean distclean
make defconfig
make install -j $(($(nproc) * 4))
cd ../..

#编译ip命令
cd ./src/utils/iproute2-2.6.33/
make clean distclean
export KERNEL_INCLUDE=../../linux-2.6.33.5/include/
make -j $(($(nproc) * 4))
cd ../.. 
cd ../

#编译canutils
cd ./src/utils/canutils-3.0.2/
export KERNEL_INCLUDE=../../linux-2.6.33.5/include/
export CC=/usr/local/arm/4.3.2/bin/arm-linux-gcc
autoreconf -vif
./configure --host=arm-linux-gnueabi --prefix=$(pwd)/output CFLAGS="-I${KERNEL_INCLUDE}"
make install -j $(($(nproc) * 4))
cd ../..
cd ..

# 生成文件系统
test -d output/fs && rm -rf output/fs
mkdir output/fs 
cp -rf filesystem/* output/fs/
find output/fs -name .gitkeep | xargs rm -rf

# 加入busybox文件
cp -rf src/busybox-1.13.2/_install/bin/* output/fs/bin/
cp -rf src/busybox-1.13.2/_install/sbin/* output/fs/sbin/
cp -rf src/busybox-1.13.2/_install/usr/bin/* output/fs/usr/bin/
cp -rf src/busybox-1.13.2/_install/usr/sbin/* output/fs/usr/sbin/
ln -s sbin/init output/fs/init

# 替换ip命令
rm -rf output/fs/bin/ip
cp -rf src/utils/iproute2-2.6.33/ip/ip output/fs/bin/ip

# 加入canutils
cp src/utils/canutils-3.0.2/output/bin/* output/fs/bin/