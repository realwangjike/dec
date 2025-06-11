#! /bin/sh

OLD_PATH="${PATH}"

test -d output && rm -rf output
mkdir output

# 编译loader.bin
echo "compile loader.bin"
cd src/AT91RM9200-Loader/
make clean
make 
test -f loader.bin && cp -rf loader.bin ../../output/ \
                   || { echo "compile loader.bin failed."; exit 1; }
cd ../..

# 编译boot.bin
echo "compile boot.bin"
cd src/AT91RM9200-Boot/
make clean
make 
test -f boot.bin && cp -rf boot.bin ../../output/ \
                   || { echo "compile boot.bin failed."; exit 1; }
cd ../..


export ARCH=arm
export CROSS_COMPILE=arm-linux-
export PATH=/usr/local/arm/4.3.2/bin/:${OLD_PATH}

# 编译u-boot
echo "compile u-boot"
cd src/u-boot-1.3.4
test -f u-boot.bin.gz && rm -rf u-boot.bin.gz
make clean distclean
make at91rm9200dk_config
make -j $(($(nproc) * 4))
test -f u-boot.bin && cp -rf u-boot.bin ../../output/ \
                   || { echo "compile u-boot.bin failed."; exit 1; }
gzip u-boot.bin
test -f u-boot.bin.gz && cp -rf u-boot.bin.gz ../../output/ \
                   || { echo "compile u-boot.bin.gz failed."; exit 1; }
cd ../..

# 编译linux kernel
echo "compile kernel"
cd src/linux-2.6.33.5/
test -f arch/arm/boot/uImage && rm -rf arch/arm/boot/uImage
make clean distclean
make at91rm9200dk_defconfig
make all -j $(($(nproc) * 16))
mkimage -A arm -O linux -T kernel -C none \
    -a 0x20008000 -e 0x20008000 -n "Linux-2.6.33.5" \
    -d arch/arm/boot/zImage arch/arm/boot/uImage
test -f arch/arm/boot/uImage && cp -rf arch/arm/boot/uImage ../../output/ \
                   || { echo "compile uImage failed."; exit 1; }
cd ../..

# 编译busybox
echo "compile busybox"
cd src/busybox-1.13.2/
make clean distclean
make defconfig
make install -j $(($(nproc) * 4))
test -f _install/bin/busybox || { echo "compile busybox failed."; exit 1; }
cd ../..

#编译ip命令
echo "compile ip"
cd ./src/utils/iproute2-2.6.33/
make clean distclean
export KERNEL_INCLUDE=../../linux-2.6.33.5/include/
make -j $(($(nproc) * 4))
test -f ip/ip || { echo "compile ip failed."; exit 1; }
cd ../.. 
cd ../

#编译canutils
echo "compile cantils"
cd ./src/utils/canutils-3.0.2/
export KERNEL_INCLUDE=../../linux-2.6.33.5/include/
export CC=/usr/local/arm/4.3.2/bin/arm-linux-gcc
autoreconf -vif
./configure --host=arm-linux-gnueabi --prefix=$(pwd)/output CFLAGS="-I${KERNEL_INCLUDE}"
make install -j $(($(nproc) * 4))
test -f src/candump || { echo "compile candump failed."; exit 1; }
test -f src/canecho || { echo "compile canecho failed."; exit 1; }
test -f src/cansend || { echo "compile cansend failed."; exit 1; }
test -f src/cansequence || { echo "compile cansequence failed."; exit 1; }
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
