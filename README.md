### usbcan设备取消root权限
sudo cp ./lib/usbcan.rules /etc/udev/rules.d/
### 串口给读写权限
sudo chmod 777 /dev/ttyUSB0