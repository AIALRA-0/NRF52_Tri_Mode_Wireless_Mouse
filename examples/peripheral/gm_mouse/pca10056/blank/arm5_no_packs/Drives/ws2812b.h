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
// ����NordicоƬ��ض���
#include "nrf.h"

// Exported types *************************************************************
/// \brief WS2812B��״̬ö������
typedef enum
{
   WS2812B_OK       = 0x00U, ///< �����ɹ�
   WS2812B_ERROR    = 0x01U, ///< ����ʧ��
   WS2812B_BUSY     = 0x02U, ///< �豸æµ��
   WS2812B_TIMEOUT  = 0x03U, ///< ������ʱ
   WS2812B_READY    = 0x04U, ///< �豸��׼����
   WS2812B_RESET    = 0x05U  ///< �豸���ڸ�λ״̬
} WS2812B_StatusTypeDef;

// Exported functions *********************************************************
/// \brief ��ʼ��WS2812B�豸
///
/// \return WS2812B_StatusTypeDef ״̬��
WS2812B_StatusTypeDef   WS2812B_init            ( void );

/// \brief �����������ݷ��͵�WS2812B LEDs
///
/// \return ��
void                    WS2812B_sendBuffer      ( void );

/// \brief ���WS2812B�����������ͺ��������ؽ���Ϊ��ɫ��
///
/// \return ��
void                    WS2812B_clearBuffer     ( void );

/// \brief ���õ������ص���ɫ��
///
/// \param [in] pixel_pos ����λ��
/// \param [in] red ��ɫͨ��ֵ��0-255��
/// \param [in] green ��ɫͨ��ֵ��0-255��
/// \param [in] blue ��ɫͨ��ֵ��0-255��
///
/// \return ��
void                    WS2812B_setPixel        ( uint16_t pixel_pos, uint8_t red, uint8_t green, uint8_t blue );

/// \brief      ��תָ�����ص���ɫ״̬����Եƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_togglePairingLight(void);

/// \brief      ���õ�������Ϊ��ɫ������ƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_setSuspendLight(void);

/// \brief      ���õ�������Ϊ��ɫ�����ӵƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_setConnectedLight(void);

/// \brief      ���õ�������Ϊ��ɫ����������ƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_setDisconnectedLight(void);

/// \brief      ���õ�������Ϊ��ɫ�����������ƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_setHostDeleteLight(void);

/// \brief      ������еƹ⣬�ر��������ء�
///
/// \param      ��
///
/// \return     ��
void WS2812B_clearLight(void);

/// \brief      ������ָʾ�Ƹߣ���ɫ����
///
/// \param      ��
///
/// \return     ��
void WS2812B_setBatteryHighLight(void);

/// \brief      ������ָʾ���У���ɫ����
///
/// \param      ��
///
/// \return     ��
void WS2812B_setBatteryMiddleLight(void);

/// \brief      ������ָʾ�Ƶͣ���ɫ����
///
/// \param      ��
///
/// \return     ��
void WS2812B_setBatteryLowLight(void);

#endif // __WS2812B_H
