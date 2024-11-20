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
#include "ws2812b.h"  // LED����

#include "app_scheduler.h" // app������

#include "nrf_drv_saadc.h"      // SAADC������

/***************************************
 *             �궨��            	 	*
 ***************************************/
//SAADC
#define FILTER_WINDOW_SIZE 5     // �������ڴ�С

// ������
#define SCHED_MAX_EVENT_DATA_SIZE       APP_TIMER_SCHED_EVENT_DATA_SIZE             /**< @brief �������¼�������С�� */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                80                                          /**< @brief �����������е�����¼������������л�Ӧ����Ҫ���ࡣ */
#else
#define SCHED_QUEUE_SIZE                40                                          /**< @brief �����������е�����¼����� */
#endif

// ���
# define BATTERY_HIGH_LIM 40
# define BATTERY_LOW_LIM 20

typedef enum {
    BATTERY_LOW,
    BATTERY_MIDDLE,
    BATTERY_HIGH
} battery_state_t; // ���״̬
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
void mouse_scheduler_init(void);

// ��궨ʱ������
void ble_pairing_timeout_handler(void *p_context);
void mouse_timer_init(void);

// ��ص�������
void mouse_saadc_init(void);
void mouse_saadc_callback(nrf_drv_saadc_evt_t const * p_event);
void mouse_battery_update_timer_handler(void *p_context);
uint8_t saadc_value_to_battery_percent(int16_t adc_value);
void mouse_battery_update_timer_start(void);

// �������ѯ����
void mouse_main_task(void);


#endif // MOUSE_H
