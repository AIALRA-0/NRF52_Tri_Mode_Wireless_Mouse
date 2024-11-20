#ifndef MOUSE_H
#define MOUSE_H

/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include <stdint.h>   // ��׼�������Ͷ���
#include <stdbool.h>  // �������Ͷ���
#include <stddef.h>   // ��׼���壨NULL�ȣ�
#include "nrf_gpio.h"  // GPIO����ͷ�ļ�
#include "paw3399.h"	// paw3399 ��������
#include "usb_device.h" // USB �豸ʵ��
#include "ble_device.h" // BLE �豸ʵ��
#include "esb_device.h" // BLE �豸ʵ��

/***************************************
 *             ��������            	 *
 ***************************************/
 
// ��긨������
void mouse_button_debounce(bool button_state, uint8_t *button_count, uint8_t *button_bit);
void mouse_update_dpi(bool increase);

// �������ѯ����
void mouse_main_task(void);

// ����ʼ������
void mouse_init(void);

#endif // MOUSE_H
