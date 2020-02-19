# ARM_RTOS_Projects

使用Eclipse IDE([openSTM32 System Workbench](https://www.openstm32.org/System%2BWorkbench%2Bfor%2BSTM32))在STMF411RE板子上引入FreeRTOS函式庫以及設置SEGGER SystemView在Debugger視覺化執行中的Tasks

 
## FreeRTOS

![](https://www.freertos.org/wp-content/uploads/2018/07/logo-1.jpg)

到[FreeRTOS](https://www.freertos.org/)下載kernel



1. Create `Config` and `Third-Party`


2. FreeRTOS可以砍Source>protable中其他部需要的，如下圖

[![](https://i.imgur.com/DDdDRuJ.png)](https://tinyurl.com/y3d4zdhy)
<!-- more -->

2. 不要勾選`Exclude resource from build`，然後按apply再按OK
![](https://i.imgur.com/7eG6GYi.png)

3. Link RTOS source file
![](https://i.imgur.com/xaZH7PN.png)
![](https://i.imgur.com/rCVSYwa.png)
一樣的方法加入ARM_CM4F
![](https://i.imgur.com/EmigjLk.png)

4. FreeRTOS config.h
    * 在`D:\Workspace\RTOS_Workspace\STM32_HelloWorld\`建立`Config`資料夾
    * 到以下路徑`D:\Workspace\Software&Toolchain\FreeRTOSv10.2.1\FreeRTOS\Demo\CORTEX_M4F_STM32F407ZG-SK`複製`FreeRTOSConfig.h`
    * 放進`D:\Workspace\RTOS_Workspace\STM32_HelloWorld\Config`
    * refresh之後同樣需要取消選取
    ![](https://i.imgur.com/vqkYQ2t.png)
    ![](https://i.imgur.com/CuebcCu.png)




---


## SEGGER SystemView

![](https://i.imgur.com/uc7aOLp.png)


視覺化RTOS的時序工具，由德國SEGGER公司開發，有以下功能
* 列出RTOS執行中的Task，以及他們花費的CPU時間
* 顯示ISR進入與離開的時間點
* 分析Task的行為(Blocking、Unblocking、Notifying、Yielding)
* 分析CPU閒置時間 (我們可以在這些時間讓MCU進入睡眠狀態減少功耗)
* 可使用Continuous recording或single-shot recording

> Real time recording 架構圖
> ![](https://i.imgur.com/VWWjO6X.png) 
> RTT: Real time transfer


### 環境設定

#### Step 1: Including SEGGER SystemView in the application

##### Step 1-1

在Thirt-Party下建立一個`SEGGER`資料夾



![](https://i.imgur.com/ncCgPjV.png)

##### Step 1-2

![](https://i.imgur.com/TXElNSv.png)

##### Step 1-3

Include path加入`Config`

![](https://i.imgur.com/UXxwwHJ.png)

同樣方法加入`OS`、`SEGGER`，按Apply > OK

![](https://i.imgur.com/lrYL4cl.png)

#### Step 2: Patching FreeRTOS files

![](https://i.imgur.com/yuT3P0T.png)

![](https://i.imgur.com/oALido0.png)

#### Step 3: FreeRTOSConfig.h Settings

```c
#include "SEGGER_SYSVIEW_FreeRTOS.h"
```
這個標頭檔定義了 trace macros 來產生 SYSTEMVIEW events

![](https://i.imgur.com/MnEQjmX.png)

```c
#define INCLUDE_xTaskGetIdleTaskHandle  1
#define INCLUDE_pxTaskGetStackStart     1
```

![](https://i.imgur.com/gP8GG4Y.png)


#### Step 4: MCU and Project specific settings

##### Step 4-1 

在`SEGGER_SYSVIEW_Conf.h`定義你用的處理器類型

![](https://i.imgur.com/KmfUsg7.png)

##### Step 4-2 

 在`SEGGER_SYSVIEW_Conf.h`設置 SystemView buffer size (SEGGER_SYSVIEW_RTT_BUFFER_SIZE)

![](https://i.imgur.com/6fRwLuZ.png)

![](https://i.imgur.com/rIsNSy6.png)


##### Step 4-3 

在`SEGGER_SYSVIEW_Conf_FreeRTOS.c`設置 application specific 資訊(SRAM base address)，參閱[reference manual 連結](https://www.st.com/content/ccc/resource/technical/document/reference_manual/9b/53/39/1c/f7/01/4a/79/DM00119316.pdf/files/DM00119316.pdf/jcr:content/translations/en.DM00119316.pdf) 

![](https://i.imgur.com/HXk2ZqI.png)



![](https://i.imgur.com/uwhQLiO.png)

#### Step 5: Enable the ARM Cortex Mx Cycle Counter

在`main.c`中`DWT->CTRL`的第一個bit設為1，這樣就能記錄CPU的timestamp

![](https://i.imgur.com/OsFMp2v.png)

![](https://i.imgur.com/GlTuq5j.png)


#### Step 6: Start the recoding of events

如下呼叫SEGGER APIs來開始FreeRTOS的行為

```
SEGGER_SYSVIEW_Conf();
SEGGER_SYSVIEW_Start();
```

![](https://i.imgur.com/PbxkHQv.png)


#### Step 7: Complie, Flash and Debug

1. 編譯並燒錄 FreeRTOS + SystemView application
2.  openSTM32 System Workbench的IDE中進入debugging mode
3. 按一下**run**幾秒後，再按**pause**  

#### Step 8: Collect the recorded data (RTT buffer)

不管**continuous recording** 或**single-shot recording**(這裡使用single-shot recording)都要設置RTT buffer

1. 取得SystemView *RTT buffer address*與*number of bytes*，分別是`Normally_SEGGER_RTT.aUp[1].pBuffer`與`_SEGGER_RTT.aUp[1].WrOff`
![](https://i.imgur.com/I1dfsdt.png)
![](https://i.imgur.com/2xS1pOg.png)
    * 這裡的RTT buffer的start address是 `0x200136f0`
    * number of bytes 是 8189
![](https://i.imgur.com/BKgX6aC.png)

3. 接著將memory dump存成檔案
	* Save the file with `.SVdat` extension 
	* ![](https://i.imgur.com/IebouRj.png)

4. 到SystemView載入此檔案開始分析
![](https://i.imgur.com/ToHYLPe.png)


### J-Link 

如果要**continus recording**，下載J-Link software package 就能將原本Nucleo板子上的ST-LINK改成J-LINK，分析完CPU行為之後，再改回ST-LINK就能繼需開發囉~ 非常方便

> Download: https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/
> ![](https://i.imgur.com/ovXvMrE.png)


