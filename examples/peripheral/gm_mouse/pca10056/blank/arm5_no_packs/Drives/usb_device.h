#ifndef USB_DEVICE_H
#define USB_DEVICE_H

/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include <stdint.h>   // ��׼�������Ͷ���
#include <stdbool.h>  // �������Ͷ���
#include <stddef.h>   // ��׼���壨NULL�ȣ�
#include "nrf.h"      // nRF΢�������豸����ͷ�ļ�
#include "nrf_drv_usbd.h" // nRF USB����ͷ�ļ�
#include "nrf_drv_clock.h" // ʱ������ͷ�ļ�
#include "nrf_gpio.h"  // GPIO����ͷ�ļ�
#include "nrf_drv_gpiote.h" // GPIOTE��
#include "nrf_delay.h" // �ӳٺ�����
#include "nrf_drv_power.h" // ��Դ��������ͷ�ļ�
#include "nrfx_log.h"  // ��־ģ��ͷ�ļ�
#include "nrf_log_ctrl.h"  // ��־����ģ��
#include "nrf_log_default_backends.h" // Ĭ����־���
#include "app_timer.h" // ��ʱ��Ӧ�ó���ӿ�
#include "app_error.h" // ������ӿ�
#include "bsp.h"       // �弶֧�ְ�
#include "FreeRTOS.h"	 // FreeRTOS��ĺ���ͷ�ļ�ss
#include "task.h"			 // FreeRTOS��������ͷ�ļ�
#include "semphr.h"			// FreeRTOS�ź����ͻ�����
#include "ws2812b.h"    // RGBָʾ��

 /****************************************
 *            �ַ���������                *
 ****************************************/
#define USBD_STRING_LANG_IX 0x00 // �����ַ�������
#define USBD_STRING_LANG \
    0x04, /* ���������� -> 4�ֽ� */\
    0x03, /* ����������  -> �ַ��������� */\
    0x09, 0x04 /* ֧�ֵ�����ID -> ����Ӣ�� */

#define USBD_STRING_SERIAL_IX 0x00 // �����к��ַ�������

#define USBD_STRING_MANUFACTURER_IX 0x01 // �����ַ�������
#define USBD_STRING_MANUFACTURER \
    0x0E, /* ���������� -> 14�ֽ�  */\
    0x03, /* ���������� -> �ַ���������) */\
    'A', 0x00, \
    'I', 0x00, \
    'A', 0x00, \
    'L', 0x00, \
    'R', 0x00, \
    'A', 0x00
		
#define USBD_STRING_PRODUCT_IX 0x02 // ��Ʒ�ַ�������
#define USBD_STRING_PRODUCT \
    0x14, /* ���������� -> 20�ֽ� */\
    0x03, /* ���������� -> �ַ��������� */\
    'H', 0x00, \
    'I', 0x00, \
    'D', 0x00, \
    ' ', 0x00, \
    'M', 0x00, \
    'O', 0x00, \
    'U', 0x00, \
    'S', 0x00, \
    'E', 0x00

 /****************************************
 *             �豸������                *
 ****************************************/
#define EP0_MAXPACKETSIZE NRF_DRV_USBD_EPSIZE	// EP0��������С
		
/** @brief �豸���������� 
 *  ����ֽ��򣬸��ֽ���ǰ�����ֽ��ں�
 */
#define USBD_DEVICE_DESCRIPTOR \
    0x12, 											/* ���������� -> 18 �ֽ�*/\
    0x01, 											/* ���������� -> �豸������*/\
    0x00, 0x02, 								/* USB�汾 -> 2.0 */\
    0x00, 											/* �豸�� -> �ɽӿ�������������ÿ���ӿڶ�ָ�����Լ�������Ϣ�������ӿڶ������� */\
    0x00, 											/* �豸���� -> ���ض��豸���� */\
    0x00, 											/* �豸Э�� -> ���ض��豸��Э��*/\
    EP0_MAXPACKETSIZE, 					/* ������С */\
    0x15, 0x19, 								/* ����ID -> Nordic Semiconfuctor ASA 6421  */\
    0x0A, 0x52, 								/* ��ƷID -> nRF52 HID���) */\
    0x01, 0x00, 								/* �豸�汾�� -> 0.01 */\
    USBD_STRING_MANUFACTURER_IX,/* �����ַ������� */\
    USBD_STRING_PRODUCT_IX, 		/* ��Ʒ�ַ������� */\
    USBD_STRING_SERIAL_IX, 			/* ���к��ַ������� */\
    0x01 /* �������� */
		
 /****************************************
 *             ����������                *
 ****************************************/

#define USBD_CONFIG_DESCRIPTOR_FULL_SIZE (9 + (9 + 9 + 7)) // �����������������ܴ�С�����������÷��ص�����������������+�ӿ�+HID+�˿ڣ����ܳ���

/** @brief ���������� */
#define USBD_CONFIG_DESCRIPTOR \
    0x09, /* ���������� -> 9�ֽ� */\
    0x02, /* ���������� -> ���������� */\
    USBD_CONFIG_DESCRIPTOR_FULL_SIZE, 0x00, /* �������ܳ��� */\
    0x01, /* �ӿ����� -> 1*/\
    0x01, /* ѡ������ֵ -> 1*/\
    0x00, /* �����ַ������� -> ���ַ������������������������*/\
    0x80 | (((DEVICE_SELF_POWERED) ? 1U:0U)<<6) | (((REMOTE_WU) ? 1U:0U)<<5), /* �Թ���&Զ�̻������� */\
    100 /* ����� -> 100 * 2 = 200 mA */

 /****************************************
 *             �ӿ�������                *
 ****************************************/
/** @brief �ӿ����������� */
#define USBD_INTERFACE0_DESCRIPTOR \
    0x09, /* ���������� -> 9�ֽ� */\
    0x04, /* ���������� -> �ӿ������� */\
    0x00, /* �ӿڱ�� -> 0*/\
    0x00, /* �������� -> ��*/\
    0x01, /* �˵����� -> 1*/\
    0x03, /* �ӿ��� -> HID */\
    0x00, /* �ӿ����� -> �� TODO:֧��BOOT*/\
    0x02, /* �ӿ�Э�� -> ��� */\
    0x00 /* �ӿ��ַ������� */

 /****************************************
 *             HID������                *
 ****************************************/
/**
 * @brief HID����������
 */
#define USBD_HID0_DESCRIPTOR \
    0x09, /* ���������� -> 9�ֽ� */\
    0x21, /* HID���������� -> HID */\
    0x11, 0x01, /* HID�汾 -> 1.11 */\
    0x00, /* Ŀ����Ҵ��� -> ��Ŀ����� */\
    0x01, /* ������������������ -> 1*/\
    0x22, /* ����������1 ���� -> ���������� */\
    (uint8_t)(USBD_MOUSE_REPORT_DESCRIPTOR_SIZE), /* �������������ȵ��ֽ� */\
    (uint8_t)(USBD_MOUSE_REPORT_DESCRIPTOR_SIZE / 256) /* �������������ȸ��ֽ� */
		
 /****************************************
 *             �˵�������                *
 ****************************************/
#define POLLLING_INTERVAL (1000 / REPORT_RATE) // ��ѯ��� (ms)

#define USBD_ENDPOINT1_DESCRIPTOR \
    0x07, /* ���������� -> 7�ֽ� */\
    0x05, /* ���������� -> �˵������� */\
    0x81, /* �˵��ַ -> ����IN�˵�1 */\
    0x03, /* �˵����� -> �жϴ��� */\
    0x08, 0x00, /* ������С -> 8�ֽ� TODO:����ͬ���˵�΢֡*/\
    POLLLING_INTERVAL /* ���� Report_Rate ������ѯ��� */
		
 /****************************************
 *             ����������                *
 ****************************************/
#define USBD_MOUSE_REPORT_DESCRIPTOR_SIZE  60
#define USBD_MOUSE_REPORT_DESCRIPTOR /* ʹ��Waratah�Զ����� */\
    0x05, 0x01,     /* usage page (generic desktop). Global item, applies to all subsequent items   */\
    0x09, 0x02,     /* usage (mouse). Local item                                                    */\
    0xA1, 0x01,     /* collection (application)                                                     */\
    0x09, 0x01,     		/* usage (pointer)                                                              */\
    0xA1, 0x00,     		/* collection (physical)                                                        */\
		0x05, 0x09,     				/*   usage page (buttons). Global item, applies to all subsequent items         */\
    0x19, 0x01,     						/*   usage minimum (1)                                                          */\
    0x29, 0x08,     						/*   usage maximum (8)                                                          */\
    0x15, 0x00,     						/*   logical minimum (0)                                                        */\
    0x25, 0x01,     						/*   logical maximum (1)                                                        */\
    0x95, 0x08,     						/*   report count (8)                                                           */\
    0x75, 0x01,     						/*   report size (1)                                                            */\
    0x81, 0x02,     						/*   input (data, var, abs)                                                     */\
    0x05, 0x01,     				/*   usage page (generic desktop). Global item, applies to all subsequent items */\
		0x16, 0x00, 0x80,    				/*   LogicalMinimum(-32,768)										*/\
		0x26, 0xFF, 0x7F,    				/*   LogicalMaximum(32,767)										*/\
    0x75, 0x10,     						/*   report size (16)                                                            */\
    0x09, 0x30,     						/*   usage (X)                                                                  */\
    0x09, 0x31,     						/*   usage (Y)                                                                  */\
    0x95, 0x02,     						/*   report count (2)                                                           */\
    0x81, 0x06,     						/*   input 	(Data, Variable, Relative)																          */\
		0x05, 0x01,     				/*   usage page (generic desktop). Global item, applies to all subsequent items */\
		0x15, 0x81,     						/*   logical minimum (-127)                                                     */\
    0x25, 0x7F,     						/*   logical maximum (127)                                                      */\
    0x75, 0x08,     						/*   report size (8)                                                            */\
		0x09, 0x38,     						/*   usage wheel                                                                */\
    0x95, 0x01,     						/*   report count (1)                                                           */\
    0x81, 0x06,     						/*   input 	(Data, Variable, Relative)																          */\
    0xC0,           		/* end collection                                                               */\
    0xC0            /* End Collection 																																	*/

 /****************************************
 *                 ����  			           *
 ****************************************/
// USB��־��������
bool usb_device_get_mark_usbd_configured(void); 
void usb_device_set_mark_usbd_configured(bool value); 
bool usb_device_get_mark_usbd_suspended(void); 
void usb_device_set_mark_usbd_suspended(bool value); 
bool usb_device_get_mark_usbd_remote_wakeup_enabled(void); 
void usb_device_set_mark_usbd_remote_wakeup_enabled(bool value); 
bool usb_device_get_mark_send_device_data(void); 
void usb_device_set_mark_send_device_data(bool value); 
bool usb_device_get_mark_usbd_suspend_state_request(void); 
void usb_device_set_mark_usbd_suspend_state_request(bool value); 
bool usb_device_get_mark_device_has_motion(void);
void usb_device_set_mark_device_has_motion(bool value);
	
// USBD ���ú���
ret_code_t usb_device_configure_endpoint(uint8_t index, nrf_drv_usbd_ep_t ep);

// USBD ������Ӧ����
void usb_device_respond_setup_request(nrf_drv_usbd_setup_t const * const p_setup, void const * p_data, size_t size);
void usb_device_respond_get_status_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_clear_feature_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_feature_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_get_descriptor_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_get_config_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_config_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_idle_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_interface_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_protocol_request(nrf_drv_usbd_setup_t const * const p_setup);

// USBD �¼�������
void usb_device_event_handler(nrf_drv_usbd_evt_t const * const p_event);

// USBD ��Դ����
void usb_device_power_event_handler(nrf_drv_power_usb_evt_t event);

// GPIO ����
void usb_device_gpio_init(void);
void usb_device_gpiote_init(void);

// ��λ����
void usb_device_log_reset_reason(void);

// ������
void usb_device_force_suspend(void);
void usb_device_force_leaving_suspend(void);

// USBD ��ʼ������
void usb_device_init(void);

#endif // USB_DEVICE_H
