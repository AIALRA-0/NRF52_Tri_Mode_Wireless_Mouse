[中文说明](#中文说明) | [English Description](#english-description)

### 中文说明

---

# **NRF52_Tri_Mode_Wireless_Mouse**  
基于 NRF52 平台的游戏鼠标，支持蓝牙、USB 和 ESB (RF) 模式。

---

### **免责声明**
1. 本人代码水平很烂 :|  
2. 本人只会 Copy&Paste，有问题请问 GPT :)  
3. 蓝牙配对逻辑一坨，但能用，有空就优化 :(  
4. 本项目纯属图一乐，要商用或者 DIY 请自行修改，本人概不负责 :)  
5. BUG 是肯定有的，请自己修，或者 Commit :)  
6. 延迟和低功耗是没有优化的，能跑能用就够了 :)  
7. 我是傻逼，RTOS 是没有的，有空再说 :(  
8. 8K 是没有的，外挂 USB 高速 PHY 有空再搞，早知道换个片了 *8K 屁用没有 :(  
9. 注释我就用中文，乱码自己转换，可能后面让 GPT 翻译个英文的 :)  
10. 维护是不存在的，啥时候更新看心情 :)  

---

### **文件说明**
#### **项目文件**
- `examples\peripheral\gm_mouse`  
  鼠标本体，支持蓝牙，USB 有线，ESB(RF) 三种模式，开发使用 52840 平台。
- `examples\peripheral\gm_mouse_rx`  
  ESB (RF) 接收器，插上就配对，地址固定。

#### **驱动**
- **nrf 驱动**：部分驱动遭到了我的摧残和修改，请自行查阅。
- `examples\peripheral\gm_mouse\pca10056\blank\arm5_no_packs\Drives`  
  用户驱动文件夹，下面的驱动都放在这。
- **驱动模块**：
  - **ble_device**：ble 设备驱动，负责蓝牙交互，写的一坨，图一乐，能用，回报率最高 133Hz。
  - **esb_device**：esb 设备驱动，RF 逻辑就是按键通过 RF 发送给 RX 然后转为 USB HID 协议发送给上位机，回报率最高 1000Hz。
  - **usb_device**：usb 设备驱动，usb 全速，上不了高速，故而不支持 8K 微帧，回报率最高 1000Hz。
  - **mouse**：鼠标逻辑本体，一团乱麻。
  - **paw3399**：鼠标传感器驱动，改改寄存器值啥的就可以平替为其他传感器了，前提有 Datasheet，但其实没啥区别，除了几个新特性。
  - **ws2812b**：RGB 指示灯驱动，参考 [https://github.com/nicokorn/NRF52840_WS2812B](https://github.com/nicokorn/NRF52840_WS2812B)。

#### **工具**
- **PAW3399DM-T4QU_DS_V1.00-07122020_Confidential**：鼠标传感器 Datasheet，本项目使用 PAW3399，对于 PAW3395/3950 移植应该很简单，只需要修改对应的寄存器配置即可，不过个人感觉用不太到。
- **waratah**：用于生成 HID 描述符的脚本，详情参考 [https://github.com/microsoft/hidtools](https://github.com/microsoft/hidtools)。

---

### **功能**
- **鼠标**：默认 2 主按键，1 中键按键，1 滚轮，1 传感器，扫描方式为轮询。
- **USB**：基于 USB HID，最高 1000Hz。
- **BLE**：基于 BLE HID，最高 133Hz。
- **ESB**：基于 ESB RF -> USB HID，最高 1000Hz。
- **回报率**：这玩意其实就是 USB 的回报率，没区别。
- **配对按键**：BLE 模式下长按重新开始配对。
- **模式切换逻辑**：
  - **USB**：USB 连接。
  - **蓝牙**：USB 断连，模式选择按钮高电平。
  - **ESB RF**：USB 断连，模式选择按钮低电平。
- **DPI 更改**：预设 5 组 DPI，两个按键用于前向/后向循环。
- **状态指示灯**：
  - **绿色**：连接。
  - **红色**：BLE 断连，需要重新开始配对。
  - **黄色**：无操作，设备自行挂起。
  - **蓝色**：闪烁，BLE 配对。
  - **紫色**：需要主机删除已有的设备信息后才能运行。
- **电量指示灯**：
  - **绿色**：电量高于 40%。
  - **黄色**：电量高于 20%。
  - **红色**：电量低于 20%。
- **RGB**：没人用 RGB，有空就写。

---

### **引脚**
DK 能用的 IO 太乱，等评估板画好再说吧。

---

### **评估版**
在画了.jpg  

---

### English Description

---

# **NRF52_Tri_Mode_Wireless_Mouse**  
A gaming mouse based on the NRF52 platform, supporting Bluetooth, USB, and ESB (RF) modes.

---

### **Disclaimer**
1. My coding skills are terrible :|  
2. I only know how to Copy&Paste, ask GPT if you have questions :)  
3. The Bluetooth pairing logic is a mess, but it works; I'll optimize when I have time :(  
4. This project is just for fun. If you want to use it commercially or for DIY, please modify it yourself. I'm not responsible :)  
5. BUGS are definitely present. Fix them yourself or submit a commit :)  
6. Latency and low power consumption are not optimized. As long as it runs, it's good enough :)  
7. I'm an idiot; there's no RTOS. Maybe I'll add it later :(  
8. No 8K support; an external USB high-speed PHY might help. Should have used a better chip *8K is pointless anyway :(  
9. Comments are in Chinese. Convert them yourself if they look garbled. I might ask GPT to translate them into English later :)  
10. Maintenance doesn't exist. Updates depend on my mood :)  

---

### **File Description**
#### **Project Files**
- `examples\peripheral\gm_mouse`  
  The main mouse, supporting Bluetooth, USB wired, and ESB (RF) modes. Developed using the 52840 platform.  
- `examples\peripheral\gm_mouse_rx`  
  The ESB (RF) receiver. Just plug it in to pair, with a fixed address.

#### **Drivers**
- **nrf drivers**: Some drivers have been modified. Please check them yourself.
- **User driver folder**:  
  `examples\peripheral\gm_mouse\pca10056\blank\arm5_no_packs\Drives`  
  All user drivers are stored here.
- **Driver Modules**:
  - **ble_device**: BLE device driver, responsible for Bluetooth interaction. Maximum polling rate: 133Hz.
  - **esb_device**: ESB device driver. RF logic sends keypresses to the RX, which converts them to USB HID protocol for the host. Maximum polling rate: 1000Hz.
  - **usb_device**: USB device driver. Limited to USB full-speed; does not support 8K micro-frames. Maximum polling rate: 1000Hz.
  - **mouse**: Main mouse logic; it's a tangled mess.
  - **paw3399**: Mouse sensor driver. Can replace with other sensors by modifying register values (if you have the datasheet).
  - **ws2812b**: RGB indicator driver (reference: [https://github.com/nicokorn/NRF52840_WS2812B](https://github.com/nicokorn/NRF52840_WS2812B)).

#### **Tools**
- **PAW3399DM-T4QU_DS_V1.00-07122020_Confidential**: Datasheet for the PAW3399 mouse sensor. Porting to PAW3395/3950 should be simple by modifying register configurations, though it might not be necessary.
- **waratah**: Script for generating HID descriptors (reference: [https://github.com/microsoft/hidtools](https://github.com/microsoft/hidtools)).

---

### **Features**
- **Mouse**: Default 2 main buttons, 1 middle button, 1 scroll wheel, 1 sensor, with polling scanning.  
- **USB**: USB HID-based, max polling rate 1000Hz.  
- **BLE**: BLE HID-based, max polling rate 133Hz.  
- **ESB**: ESB RF -> USB HID-based, max polling rate 1000Hz.  
- **Polling rate**: Essentially USB polling rate; no difference.  
- **Pairing button**: Long press in BLE mode to restart pairing.  
- **Mode switching logic**:
  - **USB**: When USB is connected.  
  - **Bluetooth**: When USB is disconnected and mode select button is high.  
  - **ESB RF**: When USB is disconnected and mode select button is low.  
- **DPI adjustment

**: 5 preset DPI levels, with buttons for cycling forward/backward.  
- **Status indicator**:
  - **Green**: Connected.  
  - **Red**: BLE disconnected; restart pairing required.  
  - **Yellow**: No operation; device in suspend.  
  - **Blue (flashing)**: BLE pairing.  
  - **Purple**: Requires the host to delete existing device info to proceed.  
- **Battery indicator**:
  - **Green**: Battery >40%.  
  - **Yellow**: Battery >20%.  
  - **Red**: Battery <20%.  
- **RGB**: Not implemented yet. Might add later.

---

### **Pins**
DK's available IOs are too messy. Will finalize after evaluation board design.

---

### **Evaluation Board**
In progress.jpg
