### Overview 

  - 交叉编译工具链

------ 

### 文件说明

  - cross-2.95.3.tar.bz2 
    - 编译AT91RM9200-Boot/AT91RM9200-Loader
  - arm-linux-gcc-4.3.2.tgz
    - 可编译除AT91RM9200-Boot/AT91RM9200-Loader外的所有组件
    - 应用层程序编译器
  - arm-2009q1-203-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
    - 备用
    - 可编译linux-2.6.33.5内核
  - arm-linux-gcc-3.3.2.tar.bz2
    - 备用
    - 编译内核和AT91RM9200-Boot/AT91RM9200-Loader外的其他组件

------

### 交叉编译环境搭建
  
  - 交叉编译工具链安装
    - 解压cross-2.95.3.tar.bz2到编译系统/usr/local/arm/目录下，生成/usr/local/arm/2.95.3/目录
    - 解压arm-linux-gcc-4.3.2.tgz到编译系统/usr/local/arm/目录下，生成/usr/local/arm/4.3.2/目录
  


