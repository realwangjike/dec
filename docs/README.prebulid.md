
### 文件说明

  + boot.bin: 内部启动时第一阶段引导程序
  + loader.bin: 外部启动(从Flash启动)时第一阶段引导程序
  + u-boot.bin：u-boot程序，第二阶段引导程序，负责启动Linux内核
  + u-boot.bin.gz：压缩的u-boot程序
  + uImage: Linux内核镜像文件，可通过u-boot引导启动
  + ramdisk.img：内存磁盘映象文件，包含基本的文件系统，用作Linux内核启动后的根文件系统
