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
#include <stdio.h> // ��׼��������⣬���ڵ��Ժ������Ϣ
#include <string.h> // �ַ��������⣬�����ַ�������
#include "nrf_drv_pwm.h" // Nordic PWM�����⣬����PWM����
#include "nrf_delay.h" // �ӳٿ⣬�����ӳٲ���
#include "nrf_gpio.h" // GPIO�⣬����GPIO���ſ���
#include "ws2812b.h" // WS2812B�⣬����ʵ��LED���ƹ���

// Private define *************************************************************
// ÿ�����صı�������
#define PIXEL_BIT_SIZE           ( 24u )
// WS2812B PWM����ʱ�䣬��λΪ16MHzʱ�ӵ�tick
#define WS2812B_PWM_T            ( 20u ) // 20 ticks @ 16 MHz equals 1250 ns
// WS2812B�ߵ�ƽ����ʱ���PWMֵ
#define WS2812B_1                ( 15u | 0x8000 ) // 15 ticks @ 16 MHz equals 937.5 ns
// WS2812B�͵�ƽ����ʱ���PWMֵ
#define WS2812B_0                ( 6u | 0x8000 )  // 6 ticks @ 16 MHz equals 375 ns
// WS2812B����ӳ��
#define WS2812B                  NRF_GPIO_PIN_MAP(WS2812B_PORT,WS2812B_PIN)
// WS2812B��λ�͵�ƽʱ�䳤��
#define WS2812B_RESET_LOW        45

// Private variables **********************************************************
// WS2812B״̬��������ʼΪ��λ״̬
// static WS2812B_StatusTypeDef WS2812B_State = WS2812B_RESET; 
// PWMʵ����ʹ��ʵ��0
static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
// WS2812B���ݻ����������ڴ洢���͸�LED������
static nrf_pwm_values_common_t WS2812B_Buffer[WS2812B_RESET_LOW+PIXEL_COUNT*PIXEL_BIT_SIZE];
// PWM�������ã�����������
nrf_pwm_sequence_t const seq =
{
    .values.p_common = WS2812B_Buffer, // ָ�����ݻ�����
    .length          = NRF_PWM_VALUES_LENGTH(WS2812B_Buffer), // ���ݳ���
    .repeats         = 0, // ���ظ�
    .end_delay       = 0  // �޶����ӳ�
};

static uint8_t pairing_light_toggle_state = 0; // ���ڼ�¼���Ƶĵ�ǰ״̬��0: �ر�, 1: �򿪣�

// Functions ******************************************************************
// ----------------------------------------------------------------------------
/// \brief     PWM�¼��ص��������������ô�����ɱ�־��
///
/// \param     event_type PWM�¼�����
///
/// \return    ��
void WS2812B_pwmhandler( nrf_drv_pwm_evt_type_t event_type )
{
   if( event_type == NRF_DRV_PWM_EVT_FINISHED )
   {
      // WS2812B_State = WS2812B_READY; // ���״̬Ϊ����
   }
}

// ----------------------------------------------------------------------------
/// \brief     ��ʼ��WS2812B LED��������衣
///
/// \param     ��
///
/// \return    WS2812B_StatusTypeDef ״̬��
WS2812B_StatusTypeDef WS2812B_init( void )
{
   // ��ʼ��GPIO����Ϊ���
   nrf_gpio_cfg_output(WS2812B);
   nrf_gpio_pin_clear(WS2812B);
  
   // ���ݻ�������ĩβ��Ҫ����Ϊ�͵�ƽ������ʱ�����50΢��
   for(uint8_t i=0; i<WS2812B_RESET_LOW; i++)
   {
      WS2812B_Buffer[PIXEL_COUNT*PIXEL_BIT_SIZE+i] = 0x8000; // ����Ϊ�͵�ƽ
   }

   // ����PWM
    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            WS2812B, // ͨ��0����
            NRF_DRV_PWM_PIN_NOT_USED,             // ͨ��1δʹ��
            NRF_DRV_PWM_PIN_NOT_USED,             // ͨ��2δʹ��
            NRF_DRV_PWM_PIN_NOT_USED,             // ͨ��3δʹ��
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST, // �ж����ȼ����
        .base_clock   = NRF_PWM_CLK_16MHz,       // ��׼ʱ��16MHz
        .count_mode   = NRF_PWM_MODE_UP,         // ����ģʽΪ���ϼ���
        .top_value    = WS2812B_PWM_T,           // PWM����
        .load_mode    = NRF_PWM_LOAD_COMMON,     // ͨ�ü���ģʽ
        .step_mode    = NRF_PWM_STEP_AUTO        // �Զ�����ģʽ
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, WS2812B_pwmhandler));
    
    // WS2812B_State = WS2812B_READY; // ����״̬Ϊ����
    
    return WS2812B_OK; // ���سɹ�״̬
}

// ----------------------------------------------------------------------------
/// \brief     ���ͻ��������ݵ�WS2812B LEDs��
///
/// \param     ��
///
/// \return    ��
void WS2812B_sendBuffer( void )
{
   // WS2812B_State = WS2812B_BUSY; // ���״̬Ϊæµ
  
   nrf_drv_pwm_simple_playback(&m_pwm0, &seq, 1, NRF_DRV_PWM_FLAG_STOP); // ��������
    
   // while( WS2812B_State != WS2812B_READY ); 
}

// ----------------------------------------------------------------------------
/// \brief      ���WS2812B�����������������ڷ��ͺ󽫱�Ϊ��ɫ��
///
/// \param     ��
///
/// \return    ��
void WS2812B_clearBuffer( void )
{
   for( uint16_t i=0; i<PIXEL_COUNT*PIXEL_BIT_SIZE; i++ )
   {
      WS2812B_Buffer[i] = WS2812B_0; // ����Ϊ�͵�ƽ
   }
}

// ----------------------------------------------------------------------------
/// \brief      ���õ������ص���ɫ��
///
/// \param      [in] pixel_pos ����λ��
/// \param      [in] red ��ɫͨ��ֵ
/// \param      [in] green ��ɫͨ��ֵ
/// \param      [in] blue ��ɫͨ��ֵ
///
/// \return     ��
void WS2812B_setPixel( uint16_t pixel_pos, uint8_t red, uint8_t green, uint8_t blue )
{
  for( uint8_t i=0; i<8; i++)
  {
    // ���ú�ɫͨ��
    if( (0x80 & (red<<i)) == 0x80 )
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+8+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+8+i] = WS2812B_0;
    }
    // ������ɫͨ��
    if( (0x80 & (green<<i)) == 0x80 )
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+i] = WS2812B_0;
    }
    // ������ɫͨ��
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

/// \brief      ����ָ������Ϊ��ɫ������ƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_setSuspendLight(void)
{
    WS2812B_setPixel(0, 255, 255, 0); // ��ɫ��RGB = (255, 255, 0)
    WS2812B_sendBuffer();             // ���ͻ���������
}

/// \brief      ��תָ�����ص���ɫ״̬����Եƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_togglePairingLight(void)
{
    if (pairing_light_toggle_state == 0)
    {
        WS2812B_setPixel(0, 0, 0, 255); // ������ɫ��RGB = (0, 0, 255)
        pairing_light_toggle_state = 1;               // ����״̬Ϊ��
    }
    else
    {
        WS2812B_setPixel(0, 0, 0, 0);   // ������أ��رյƹ⣩
        pairing_light_toggle_state = 0;               // ����״̬Ϊ�ر�
    }

    WS2812B_sendBuffer(); // ���ͻ���������
}


/// \brief      ����ָ������Ϊ��ɫ�����ӵƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_setConnectedLight(void)
{
    WS2812B_setPixel(0, 0, 255, 0);  // ��ɫ��RGB = (0, 255, 0)
    WS2812B_sendBuffer();            // ���ͻ���������
}

/// \brief      ����ָ������Ϊ��ɫ����������ƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_setHostDeleteLight(void)
{
    WS2812B_setPixel(0, 255, 0, 255); // ��ɫ��RGB = (255, 0, 255)
    WS2812B_sendBuffer();             // ���ͻ���������
}

/// \brief      ����ָ������Ϊ��ɫ�������ƣ���
///
/// \param      ��
///
/// \return     ��
void WS2812B_setDisconnectedLight(void)
{
    WS2812B_setPixel(0, 255, 0, 0);  // ��ɫ��RGB = (255, 0, 0)
    WS2812B_sendBuffer();            // ���ͻ���������
}

/// \brief     ���ָʾ�ơ�
///
/// \param      ��
///
/// \return     ��
void WS2812B_clearLight(void)
{
    WS2812B_clearBuffer(); 						 // ���������
    WS2812B_sendBuffer();            // ���ͻ���������
}

/// \brief      ���õ��ָʾ�Ƹߣ���ɫ����
///
/// \param      ��
///
/// \return     ��
void WS2812B_setBatteryHighLight(void)
{
    WS2812B_setPixel(1, 0, 255, 0);  // ��ɫ��RGB = (0, 255, 0)
    WS2812B_sendBuffer();            // ���ͻ���������
}

/// \brief      ���õ��ָʾ���У���ɫ����
///
/// \param      ��
///
/// \return     ��
void WS2812B_setBatteryMiddleLight(void)
{
    WS2812B_setPixel(1, 255, 255, 0); // ��ɫ��RGB = (255, 255, 0)
    WS2812B_sendBuffer();             // ���ͻ���������
}

/// \brief      ���õ��ָʾ�Ƶͣ���ɫ����
///
/// \param      ��
///
/// \return     ��
void WS2812B_setBatteryLowLight(void)
{
    WS2812B_setPixel(1, 255, 0, 0);  // ��ɫ��RGB = (255, 0, 0)
    WS2812B_sendBuffer();            // ���ͻ���������
}

