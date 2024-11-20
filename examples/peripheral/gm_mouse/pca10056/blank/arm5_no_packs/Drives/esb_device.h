#ifndef ESB_DEVICE_H
#define ESB_DEVICE_H

/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include <stdint.h>   // ��׼�������Ͷ���
#include <stdbool.h>  // �������Ͷ���
#include <stddef.h>   // ��׼���壨NULL�ȣ�

// Nordic��
#include "nordic_common.h"  // ����NordicоƬ��ͨ�ö���͹��ܣ��繤�ߺ�ȡ�
#include "nrf.h"            // �ṩNordicоƬ�ļĴ������ڴ�ӳ�䶨�塣
#include "app_error.h"      // �ṩ������ʹ������ܣ����ڵ��Ժʹ�����־��¼��
#include "nrf_delay.h"			// nrf�ӳٿ�

// ESB��
#include "nrf_esb.h"        // NRF��ǿ��ShockBurst(ESB)Э���
#include "nrf_esb_error_codes.h" // ESBЭ����صĴ������

// ʱ�ӿ�
#include "nrf_drv_clock.h"	// ʱ�ӿ�

// ϵͳ��
#include "app_util.h"       // Ӧ�ó���ʵ�ù��߿�

// ��־����
#include "nrf_log.h"             // ��־ģ�飬���ڼ�¼���Ժ�������Ϣ��
#include "nrf_log_ctrl.h"        // ��־����ģ�飬���ڿ�����־��¼��������ֹͣ��
#include "nrf_log_default_backends.h" // ��־��˹���������־����ľ���λ�ã��紮�ڻ�洢��


 /****************************************
 *                 ����  			           *
 ****************************************/
 // ESB����
void esb_device_event_handler(nrf_esb_evt_t const * p_event);
uint32_t esb_module_init(void);
void esb_device_init(void);

#endif // ESB_DEVICE_H
