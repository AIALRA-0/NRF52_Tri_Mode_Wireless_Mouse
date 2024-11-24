// ****************************************************************************
/// \file      ws2812b.h
///
/// \brief     WS2812B C HeaderFile
///
/// \details   Driver Module for WS2812B leds.
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

// Define to prevent recursive inclusion **************************************
#ifndef __WS2812B_H
#define __WS2812B_H

// Include ********************************************************************
// 引入Nordic芯片相关定义
#include "nrf.h"

// Exported types *************************************************************
/// \brief WS2812B的状态枚举类型
typedef enum
{
   WS2812B_OK       = 0x00U, ///< 操作成功
   WS2812B_ERROR    = 0x01U, ///< 操作失败
   WS2812B_BUSY     = 0x02U, ///< 设备忙碌中
   WS2812B_TIMEOUT  = 0x03U, ///< 操作超时
   WS2812B_READY    = 0x04U, ///< 设备已准备好
   WS2812B_RESET    = 0x05U  ///< 设备处于复位状态
} WS2812B_StatusTypeDef;

// Exported functions *********************************************************
/// \brief 初始化WS2812B设备
///
/// \return WS2812B_StatusTypeDef 状态码
WS2812B_StatusTypeDef   WS2812B_init            ( void );

/// \brief 将缓冲区数据发送到WS2812B LEDs
///
/// \return 无
void                    WS2812B_sendBuffer      ( void );

/// \brief 清空WS2812B缓冲区。发送后所有像素将变为黑色。
///
/// \return 无
void                    WS2812B_clearBuffer     ( void );

/// \brief 设置单个像素的颜色。
///
/// \param [in] pixel_pos 像素位置
/// \param [in] red 红色通道值（0-255）
/// \param [in] green 绿色通道值（0-255）
/// \param [in] blue 蓝色通道值（0-255）
///
/// \return 无
void                    WS2812B_setPixel        ( uint16_t pixel_pos, uint8_t red, uint8_t green, uint8_t blue );

/// \brief      翻转指定像素的蓝色状态（配对灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_togglePairingLight(void);

/// \brief      设置单个像素为黄色（挂起灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_setSuspendLight(void);

/// \brief      设置单个像素为绿色（连接灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_setConnectedLight(void);

/// \brief      设置单个像素为橘色（主机清除灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_setDisconnectedLight(void);

/// \brief      设置单个像素为红色（主机断连灯）。
///
/// \param      无
///
/// \return     无
void WS2812B_setHostDeleteLight(void);

/// \brief      清除所有灯光，关闭所有像素。
///
/// \param      无
///
/// \return     无
void WS2812B_clearLight(void);

/// \brief      清除电池指示灯高（绿色）。
///
/// \param      无
///
/// \return     无
void WS2812B_setBatteryHighLight(void);

/// \brief      清除电池指示灯中（黄色）。
///
/// \param      无
///
/// \return     无
void WS2812B_setBatteryMiddleLight(void);

/// \brief      清除电池指示灯低（红色）。
///
/// \param      无
///
/// \return     无
void WS2812B_setBatteryLowLight(void);

#endif // __WS2812B_H
