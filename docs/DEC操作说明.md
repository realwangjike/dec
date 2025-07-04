# DEC操作说明

### 程序文件与服务准备
  - 从代码仓库下载prebuild目录到某一个windows目录  
    ![](attachments/Clipboard_2025-06-16-17-13-33.png)
  - 启动tftp服务软件  
    ![](attachments/Clipboard_2025-06-16-17-15-03.png)
  - 配置目录并保存（Settings-->TFTP）  
    ![](attachments/Clipboard_2025-06-16-17-16-42.png)
  - 就绪  
    ![](attachments/Clipboard_2025-06-16-17-18-40.png)

------

### 程序下载与系统启动
  - PC使用串口线连接到DEC的DEBUG_COM口
  - PC机中打开超级终端，新建连接，选则对应的com口  
    ![](attachments/Clipboard_2025-06-16-16-22-38.png)
  - 端口属性设置为速率115200，数据位8，奇偶校验无，停止位1，数据流控制无;  
    ![](attachments/Clipboard_2025-06-16-16-23-48.png)
  - 启动跳线J1(具体见附件板子上的红色方框所在位置)至internal，即跳线帽进行对应的跳线;(由于2.0DEC主板丝印印错，裸板调试时跳线需接至external侧);
  - 通过按钮重置(reset)DEC电路板
  - 观察超级终端，界面连续输出CCC(串口线连接正常，DEC电路板正常)  
    ![](attachments/Clipboard_2025-06-16-16-31-27.png)
  - 在超级终端界面菜单栏，选择 传送 ——> 发送文件，选择loader.bin文件路径，协议选择 Xmodem, 点击发送  
    ![](attachments/Clipboard_2025-06-16-16-35-21.png)
  - 发送成功后，超级终端输出 loader版本号，与下载U-Boot的提示，并重复持续输出C  
    ![](attachments/Clipboard_2025-06-16-16-37-16.png)
  - 再次超级终端界面菜单栏，选择 传送 ——> 发送文件，文件选择u-boot.bin，协议选择 Xmodem, 点击发送  
    ![](attachments/Clipboard_2025-06-16-16-38-37.png)
  - 文件发送成功后，终端输出u-boot信息，如下  
    ![](attachments/Clipboard_2025-06-16-16-40-52.png)
  - 设置u-boot的环境变量，如ipaddr, serverip, ethaddr，参考值如下：  
    ```
    baudrate=115200
    ethaddr=00:17:17:ff:00:00
    bootcmd=tftp 21200000 ramdisk.img; tftp 20000000 uImage; bootm 20000000
    bootdelay=3
    bootargs=mem=32M console=ttyS0,115200 initrd=0x21200000,8388608 root=/dev/ram0 r
    w init=/sbin/init LD_LIBRARY_PATH=/lib/
    ipaddr=192.168.90.65
    serverip=192.168.90.60
    stdin=serial
    stdout=serial
    stderr=serial
    ```  
    ![](attachments/Clipboard_2025-06-16-16-48-07.png)
  - 按照实际情况添加或修改某些变量后保存，例如，设置bootdelay为5, ```setenv bootdelay 5```  
    ![](attachments/Clipboard_2025-06-16-16-50-37.png)
  - 解除保护并擦除第一个flash块  
    ```
    protect off 10000000 1001ffff
    erase 10000000 1001ffff
    ```  
    ![](attachments/Clipboard_2025-06-16-16-57-21.png)
  - 从tftp服务器下载boot.bin  
    ```
    tftp 20000000 boot.bin
    cp.b 20000000 10000000 2908
    tftp 20000000 u-boot.bin.gz
    cp.b 20000000 10010000 ecb9
    ```  
    ![](attachments/Clipboard_2025-06-16-17-00-59.png)
  - 设置flash保护  
    ```
    protect on 10000000 1001ffff
    ```  
    ![](attachments/Clipboard_2025-06-16-17-03-06.png)
  - 启动跳线J1至external,并重置DEC电路板，等等系统重启  
    ![](attachments/Clipboard_2025-06-16-17-05-43.png)
  - 系统重启后自动进入linux系统  
    ![](attachments/Clipboard_2025-06-16-17-07-46.png)

------

### CAN卡配置与数据收发    

  - 查看所有链路层节点  
    ```
    ip link
    ip -d link show can0
    ```  
    ![](attachments/Clipboard_2025-06-17-09-57-41.png)
  - can0/can1设置波特率，队列长度，并启动  
    ``` 
    ip link set can1 type can bitrate 1000000
    ip link set can0 type can bitrate 1000000
    ip link set can1 txqueuelen 1500
    ip link set can0 txqueuelen 1500
    ifconfig can1 up
    ifconfig can0 up
    ```  
    ![](attachments/Clipboard_2025-06-17-10-02-21.png)  
  - 开始监听can0卡    
    ```candump can0```     
    ![](attachments/Clipboard_2025-06-17-10-03-26.png)
  - CAN分析仪器配置对应参数  
    ![](attachments/Clipboard_2025-06-17-10-05-02.png)
  - CAN分析仪启动后发送消息  
    ![](attachments/Clipboard_2025-06-17-10-06-29.png)
  - 查看Linux系统中接收到的CAN消息  
    ![](attachments/Clipboard_2025-06-17-10-07-33.png)  
  - Linux系统中CAN发送消息  
    ![](attachments/Clipboard_2025-06-17-10-10-00.png)  
  - 查看CAN分析仪器接收到的消息  
    ![](attachments/Clipboard_2025-06-17-10-10-50.png)




    
    
