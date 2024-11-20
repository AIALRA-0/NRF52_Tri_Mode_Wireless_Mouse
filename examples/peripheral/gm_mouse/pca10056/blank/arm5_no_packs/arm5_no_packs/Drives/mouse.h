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
void mouse_dpi_update(bool increase);
void mouse_connection_mode_update(void);
void mouse_wait_for_ble_connection(void);
void mouse_ble_pairing_handler(void);
void mouse_ble_host_disconnect_handler(void);
void mouse_system_status_update(void);
void mouse_data_update(void);
void mouse_usb_data_send(void);
void mouse_ble_data_send(void);
void mouse_esb_data_send(void);
void mouse_suspend_status_update(void);

// ����ʼ������
void mouse_init(void);
void mouse_wheel_data_init(void);

// �������ѯ����
void mouse_main_task(void);


#endif // MOUSE_H
