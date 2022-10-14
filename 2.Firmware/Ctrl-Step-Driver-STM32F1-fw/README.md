使用STM32CubeMX重新生成工程后，需要取消`stm32f1xx_it.c`的更改。

Note 2022/09/26  by Denn 
1.tb67h450恒流芯片 http://news.eeworld.com.cn/mp/TOSHIBA/a98472.jspx
2.https://xie.infoq.cn/article/4fdf6c390ab06c4c522e4a1e1  cstdint标准库  类型定义 考虑多平台兼容性,比如 int32_t是平台无关的，不要直接使用int;
3.https://blog.csdn.net/imxlw00/article/details/104279438 stm32F103官方外设标准库及启动文件
https://blog.csdn.net/weixin_44563314/article/details/119896308 
4.https://blog.csdn.net/qq153471503/article/details/100972009 stm32全系列HAL库和外设标准库下载
5.ADC模数转换器 https://blog.csdn.net/dongxiaodongvip/article/details/114498855
https://blog.csdn.net/qq_43743762/article/details/100067558 
6.STM32CubeMX配置F103芯片以控制步进电机 https://zhuanlan.zhihu.com/p/84308538
6.1 STMCubeMX配置STM32F103CBTx的引脚和时钟https://blog.csdn.net/Brendon_Tan/article/details/107869664
6.2 配置2个通道channel的ADC1采集 https://blog.csdn.net/apple_2333/article/details/96962574
    通过DMA控制器进行采集ADC1 https://blog.csdn.net/qq_40993639/article/details/121801920 
    DMA的理解：不占用cpu，数据传输速度快，可以直接从外设和内存间相互读取 https://blog.csdn.net/zy19981110/article/details/123005092 
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&whole_adc_data[0][0], 2);
    计算采样周期为239.5cycles = 1/14Mhz*239 = 17us
6.3 配置CAN https://blog.csdn.net/lxzdsg/article/details/113218517
    https://blog.csdn.net/prolop87/article/details/122671441
    配置完后需要配置过滤器HAL_CAN_ConfigFilter;启动HAL_CAN_Start();HAL_CAN_ActivateNotification
    在HAL_CAN_MspInit()中需要对GPIO的引脚复用做映射 __HAL_AFIO_REMAP_CAN1_2()就是PB8-9复用;
    重写CAN_Send(CAN_TxHeaderTypeDef* pHeader, uint8_t* data)和HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* CanHandle)
6.4 配置Tim 
6.5 配置GPIO: 配置PA2-5作为电机控制的引脚;PC13-14控制LED;
6.6 配置SPI：https://my.oschina.net/u/4288213/blog/3603671
6.7 配置USART
7.利用STM32F103控制步进电机 https://blog.csdn.net/weixin_42232871/article/details/89740422
  CubeMX-电机控制相关配置https://blog.csdn.net/niker_co/article/details/105899875
8._it.c和_conf.h等文件作用 https://www.cnblogs.com/Chuck-Yue/p/10478448.html
9.BSP与HAL库驱动文件.c的区别 https://blog.csdn.net/tq384998430/article/details/53484512
无操作系统时，直接使用HAL库，不需要BSP程序;
有操作系统时，在HAL库基础上，还需要提供BSP驱动程序以供OS使用;
BSP是硬件相关和操作系统相关;HAL只是硬件相关;
10.MT6816 磁编芯片 http://www.magntek.com.cn/list/174/507.htm
11.system_stm32f1xx.c 文件作用:系统初始化+系统时钟设置和更新
  https://blog.csdn.net/qq_46359697/article/details/107207409
当系统启动文件.s执行__main()之前，调用了system_stm32f1xx.c中的SystemInit()和时钟设置；
12.stm32f1xx_hal_msp.c 文件作用：对MCU硬件上的初始化设置（一些协议、数据格式等等上层的内容一般不涉及），把具体的硬件配置抽象，形成符合HAL库要求的、具有统一格式的和属性种类的结构体。此文件由用户进行编程初始化和配置。该文件内的初始化过程强调的是外设最底层硬件上的初始化。
 https://shequ.stmicroelectronics.cn/thread-620752-1-1.html
为了使HAL库对具有相同内核但资源不同的MCU有较强兼容性，特增加了此文件，让用户根据每款MCU在硬件上具体区别，初始化和配置外设的IO引脚、外设的工作时钟以及外设的中断与MCU内核寄存器的对应关系。
MSP初始化执行流:HAL库中\Drivers\STM32F1xx_HAL_Driver\Src\stm32f1xx_hal_adc.c对外设MSP设置用的是weak HAL_ADC_MspInit()，在用户代码层\Core\src\adc.c中对外设可以根据配置重写HAL_ADC_MspInit();
13.整个流程：
13.1 启动文件.s->system_stm32f1xx.c系统初始化SystemInit()
13.2 \Core\Src\main.c入口主函数main()
-HAL_Init():配置Flash prefetch;设置中断优先级;HAL_InitTick;底层硬件初始化HAL_MspInit()[在stm32f1xx_hal.c中定义成weak，在stm32f1xx_hal_msp.c中重写]
-SystemClock_Config():\Core\Src\main.c系统时钟配置
-MX_xxx_Init():外设初始化，GPIO、SPI、DMA、ADC、USART、CAN、TIM
     e.g. MX_ADC1_Init()->HAL_ADC_Init(hadc1) + HAL_ADC_ConfigChannel(&hadc1, &sConfig)+HAL_ADCEx_Calibration_Start+HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&whole_adc_data[0][0], 2);
     应用层外设文件adc.c初始化HAL_ADC_Init()调用HAL_ADC_MspInit(),调用的是用户层重写的HAL_ADC_MspInit();
-Main(): \UserApp\main.cpp中用户业务入口主函数Main()
14.


本工程STM32CubMX自动生成文件目录：
-Core:(CubeMX根据配置引脚和外设自动生成的基于HAL库的应用层接口，\
        在main.c入口main()中调用UserApp层代码Main())
    -adc.h/.c
    -can.h/.c
    -gpio.h/.c
    -spi.h/.c
    -dma.h/.c
    -tim.h/.c
    -usart.h/.c

    -stm32f1xx_it.h/.c   //是存放STM32工程中所有中断函数的模板文件;中断驱动.c和中断头文件
    -stm32f1_hal_conf.h  //按照模块宏定义引用对应的HAL库头文件
    -stm32f1xx_hal_msp.c //MSP初始化 
    -system_stm32f1xx.c  //__main()之前的系统初始化和时钟设置
    -syscalls.c          //系统调用 printf
-Ctrl
    -Driver:
        -driver_base.h
        -sin_map.h
        -tb67h450_base.h
        -tb67h450_base.cpp
-Drivers:（底层驱动和HAL库）
    -CMSIS: 
        -Device:\STM32Cube_FW_F1_V1.8.0\Drivers\CMSIS\Device\ST\STM32F1xx\Include
        -Include:\STM32Cube_FW_F1_V1.8.0\Drivers\CMSIS\Core\Include
    -STM32F1XX_HAL_Driver:\STM32Cube_FW_F1_V1.8.0\Drivers\STM32F1xx_HAL_Driver