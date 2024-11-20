// ****************************************************************************
/// \file      ws2812b.c
///
/// \brief     WS2812B C Source File
///
/// \details   The ws2812b leds are controlled by the pwm peripheral. Depending 
///            on the dutycycle the ws2812b leds 
///            interprets a 1 or 0 bit and holds a 24 bit shiftregister. The 24 
///            bits are splittet into red (8b), green (8) and blue (8b). 
///            Bits are being shiftet through all leds connected in serial. 
///            After finishing the shifting, the line needs
///            to be set low for at least 50 us, so the leds accept their
///            shift registers thus emitting their colours.
///
/// \author    Nico Korn
///
/// \version   1.0.0.0
///
/// \date      24032021
/// 
/// \copyright Copyright (c) 2021 Nico Korn
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///
/// \pre       
///
/// \bug       
///
/// \warning   
///
/// \todo      
///
// ****************************************************************************

// Include ********************************************************************
#include <stdio.h> // 标准输入输出库，用于调试和输出信息
#include <string.h> // 字符串操作库，用于字符串处理
#include "nrf_drv_pwm.h" // Nordic PWM驱动库，用于PWM控制
#include "nrf_delay.h" // 延迟库，用于延迟操作
#include "nrf_gpio.h" // GPIO库，用于GPIO引脚控制
#include "ws2812b.h" // WS2812B库，具体实现LED控制功能

// Private define *************************************************************
// 每个像素的比特数量
#define PIXEL_BIT_SIZE           ( 24u )
// WS2812B PWM周期时间，单位为16MHz时钟的tick
#define WS2812B_PWM_T            ( 20u ) // 20 ticks @ 16 MHz equals 1250 ns
// WS2812B高电平持续时间的PWM值
#define WS2812B_1                ( 15u | 0x8000 ) // 15 ticks @ 16 MHz equals 937.5 ns
// WS2812B低电平持续时间的PWM值
#define WS2812B_0                ( 6u | 0x8000 )  // 6 ticks @ 16 MHz equals 375 ns
// WS2812B引脚映射
#define WS2812B                  NRF_GPIO_PIN_MAP(WS2812B_PORT,WS2812B_PIN)
// WS2812B复位低电平时间长度
#define WS2812B_RESET_LOW        45

// Private variables **********************************************************
// WS2812B状态变量，初始为复位状态
// static WS2812B_StatusTypeDef WS2812B_State = WS2812B_RESET; 
// PWM实例，使用实例0
static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
// WS2812B数据缓冲区，用于存储发送给LED的数据
static nrf_pwm_values_common_t WS2812B_Buffer[WS2812B_RESET_LOW+PIXEL_COUNT*PIXEL_BIT_SIZE];
// PWM序列配置，关联缓冲区
nrf_pwm_sequence_t const seq =
{
    .values.p_common = WS2812B_Buffer, // 指向数据缓冲区
    .length          = NRF_PWM_VALUES_LENGTH(WS2812B_Buffer), // 数据长度
    .repeats         = 0, // 不重复
    .end_delay       = 0  // 无额外延迟
};

static uint8_t pairing_light_toggle_state = 0; // 用于记录蓝灯的当前状态（0: 关闭, 1: 打开）

// Functions ******************************************************************
// ----------------------------------------------------------------------------
/// \brief     PWM事件回调函数，用于重置传输完成标志。
///
/// \param     event_type PWM事件类型
///
/// \return    无
void WS2812B_pwmhandler( nrf_drv_pwm_evt_type_t event_type )
{
   if( event_type == NRF_DRV_PWM_EVT_FINISHED )
   {
      // WS2812B_State = WS2812B_READY; // 标记状态为就绪
   }
}

// ----------------------------------------------------------------------------
/// \brief     初始化WS2812B LED所需的外设。
///
/// \param     无
///
/// \return    WS2812B_StatusTypeDef 状态码
WS2812B_StatusTypeDef WS2812B_init( void )
{
   // 初始化GPIO引脚为输出
   nrf_gpio_cfg_output(WS2812B);
   nrf_gpio_pin_clear(WS2812B);
  
   // 数据缓冲区的末尾需要设置为低电平，持续时间大于50微秒
   for(uint8_t i=0; i<WS2812B_RESET_LOW; i++)
   {
      WS2812B_Buffer[PIXEL_COUNT*PIXEL_BIT_SIZE+i] = 0x8000; // 设置为低电平
   }

   // 配置PWM
    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            WS2812B, // 通道0引脚
            NRF_DRV_PWM_PIN_NOT_USED,             // 通道1未使用
            NRF_DRV_PWM_PIN_NOT_USED,             // 通道2未使用
            NRF_DRV_PWM_PIN_NOT_USED,             // 通道3未使用
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST, // 中断优先级最低
        .base_clock   = NRF_PWM_CLK_16MHz,       // 基准时钟16MHz
        .count_mode   = NRF_PWM_MODE_UP,         // 计数模式为向上计数
        .top_value    = WS2812B_PWM_T,           // PWM周期
        .load_mode    = NRF_PWM_LOAD_COMMON,     // 通用加载模式
        .step_mode    = NRF_PWM_STEP_AUTO        // 自动步进模式
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, WS2812B_pwmhandler));
    
    // WS2812B_State = WS2812B_READY; // 设置状态为就绪
    
    return WS2812B_OK; // 返回成功状态
}

// ----------------------------------------------------------------------------
/// \brief     发送缓冲区数据到WS2812B LEDs。
///
/// \param     无
///
/// \return    无
void WS2812B_sendBuffer( void )
{
   // WS2812B_State = WS2812B_BUSY; // 标记状态为忙碌
  
   nrf_drv_pwm_simple_playback(&m_pwm0, &seq, 1, NRF_DRV_PWM_FLAG_STOP); // 发送数据
    
   // while( WS2812B_State != WS2812B_READY ); 
}

// ----------------------------------------------------------------------------
/// \brief      清空WS2812B缓冲区。所有像素在发送后将变为黑色。
///
/// \param     无
///
/// \return    无
void WS2812B_clearBuffer( void )
{
   for( uint16_t i=0; i<PIXEL_COUNT*PIXEL_BIT_SIZE; i++ )
   {
      WS2812B_Buffer[i] = WS2812B_0; // 设置为低电平
   }
}

// ----------------------------------------------------------------------------
/// \brief      设置单个像素的颜色。
///
/// \param      [in] pixel_pos 像素位置
/// \param      [in] red 红色通道值
/// \param      [in] green 绿色通道值
/// \param      [in] blue 蓝色通道值
///
/// \return     无
void WS2812B_setPixel( uint16_t pixel_pos, uint8_t red, uint8_t green, uint8_t blue )
{
  for( uint8_t i=0; i<8; i++)
  {
    // 设置红色通道
    if( (0x80 & (red<<i)) == 0x80 )
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+8+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+8+i] = WS2812B_0;
    }
    // 设置绿色通道
    if( (0x80 & (green<<i)) == 0x80 )
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+i] = WS2812B_0;
    }
    // 设置蓝色通道
    if( (0x80 & (blue<<i)) == 0x80 )
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+16+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+16+i] = WS2812B_0;
    }
  }
}

/// \brief      设置指定像素为黄色（挂起灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_setSuspendLight(void)
{
    WS2812B_setPixel(0, 255, 255, 0); // 黄色，RGB = (255, 255, 0)
    WS2812B_sendBuffer();             // 发送缓冲区数据
}

/// \brief      翻转指定像素的蓝色状态（配对灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_togglePairingLight(void)
{
    if (pairing_light_toggle_state == 0)
    {
        WS2812B_setPixel(0, 0, 0, 255); // 设置蓝色，RGB = (0, 0, 255)
        pairing_light_toggle_state = 1;               // 更新状态为打开
    }
    else
    {
        WS2812B_setPixel(0, 0, 0, 0);   // 清空像素（关闭灯光）
        pairing_light_toggle_state = 0;               // 更新状态为关闭
    }

    WS2812B_sendBuffer(); // 发送缓冲区数据
}


/// \brief      设置指定像素为绿色（连接灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_setConnectedLight(void)
{
    WS2812B_setPixel(0, 0, 255, 0);  // 绿色，RGB = (0, 255, 0)
    WS2812B_sendBuffer();            // 发送缓冲区数据
}

/// \brief      设置指定像素为紫色（主机清除灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_setHostDeleteLight(void)
{
    WS2812B_setPixel(0, 255, 0, 255); // 紫色，RGB = (255, 0, 255)
    WS2812B_sendBuffer();             // 发送缓冲区数据
}

/// \brief      设置指定像素为红色（断连灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_setDisconnectedLight(void)
{
    WS2812B_setPixel(0, 255, 0, 0);  // 红色，RGB = (255, 0, 0)
    WS2812B_sendBuffer();            // 发送缓冲区数据
}

/// \brief     清除指示灯。
///
/// \param      无
///
/// \return     无
void WS2812B_clearLight(void)
{
    WS2812B_clearBuffer(); 						 // 清除缓冲区
    WS2812B_sendBuffer();            // 发送缓冲区数据
}

/// \brief      设置电池指示灯高（绿色）。
///
/// \param      无
///
/// \return     无
void WS2812B_setBatteryHighLight(void)
{
    WS2812B_setPixel(1, 0, 255, 0);  // 绿色，RGB = (0, 255, 0)
    WS2812B_sendBuffer();            // 发送缓冲区数据
}

/// \brief      设置电池指示灯中（黄色）。
///
/// \param      无
///
/// \return     无
void WS2812B_setBatteryMiddleLight(void)
{
    WS2812B_setPixel(1, 255, 255, 0); // 黄色，RGB = (255, 255, 0)
    WS2812B_sendBuffer();             // 发送缓冲区数据
}

/// \brief      设置电池指示灯低（红色）。
///
/// \param      无
///
/// \return     无
void WS2812B_setBatteryLowLight(void)
{
    WS2812B_setPixel(1, 255, 0, 0);  // 红色，RGB = (255, 0, 0)
    WS2812B_sendBuffer();            // 发送缓冲区数据
}

