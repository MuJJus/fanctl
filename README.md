# fanctl

树莓派温控风扇启停，43 度启动，35 度停止。

## 硬件
### 材料
* 风扇 x1
* TO-92 封装 S8550 三极管 x1
* 杜邦线 x2

### 组装
* 三极管平面朝上，从左往右依次为 E B C。
* 风扇负(黑)接 E 极。
* C 极连接树莓派 GND。
* B 极连接树莓派 GPIO22。
* 风扇正(红)连接树莓派 5V。

## 依赖
需要 wiringPi，raspbian 可使用以下命令安装。  
`sudo apt-get install wiringpi`

## 编译安装
```
make && sudo make install
```

## 使用
执行 `sudo /usr/bin/fanctl` 守护进程。强制风扇运行 `sudo killall -SIGUSR1 fanctl`，取消强制状态 `sudo killall -SIGUSR2 fanctl`。

### 注意
如果三极管类型和接法有所不同，程序中的高低电平可能需要对调。
