# 设备名固定
sudo cp ./lib/usbcan.rules /etc/udev/rules.d/usbcan.rules/
# 重新加载udev规则
sudo udevadm control --reload && sudo udevadm trigger
