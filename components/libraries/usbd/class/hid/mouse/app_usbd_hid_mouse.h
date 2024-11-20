/**
 * Copyright (c) 2017 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 * @file
 * @defgroup app_usbd_hid_mouse USB HID�����
 * @{
 * @ingroup app_usbd_hid
 * @brief �ṩUSB HID���������͡������API��
 *
 * @details ��ģ��ʵ����HID�����ĳ�ʼ�������ơ��Լ���USBͨ�ŵĻ����ӿڡ�
 */

#ifndef APP_USBD_HID_MOUSE_H__
#define APP_USBD_HID_MOUSE_H__

#include <stdint.h>   // ��׼��������
#include <stdbool.h>  // ��׼��������

#include "nrf_drv_usbd.h"             // USB������
#include "app_usbd_class_base.h"      // ������USBD�ඨ��
#include "app_usbd_hid_types.h"       // HID���Ͷ���
#include "app_usbd_hid.h"             // HID�����ӿ�
#include "app_usbd.h"                 // USB�豸���Ľӿ�
#include "app_usbd_core.h"            // USB����ʵ��
#include "app_usbd_descriptor.h"      // USB������
#include "app_usbd_hid_mouse_desc.h"  // HID���������
#include "app_usbd_hid_mouse_internal.h"  // HID����ڲ�����

#ifdef DOXYGEN
/**
 * @brief HID�����ʵ�����͡�
 *
 * @ref APP_USBD_CLASS_TYPEDEF �궨���˴����͡�
 */
typedef struct { } app_usbd_hid_mouse_t;
#else
/*lint -save -e10 -e26 -e123 -e505 */
APP_USBD_CLASS_TYPEDEF(app_usbd_hid_mouse,                      \
            APP_USBD_HID_MOUSE_CONFIG(0, NRF_DRV_USBD_EPIN1),   \
            APP_USBD_HID_MOUSE_INSTANCE_SPECIFIC_DEC,           \
            APP_USBD_HID_MOUSE_DATA_SPECIFIC_DEC                \
);
/*lint -restore*/
#endif

/**
 * @brief HID�����ʵ����ȫ�ֶ����
 *
 * @param instance_name     ʵ������
 * @param interface_number  �ӿڱ��
 * @param endpoint          ����˵㣨@ref nrf_drv_usbd_ep_t���ͣ�
 * @param bcnt              ��갴������1��8��
 * @param user_ev_handler   �û��¼�����������ѡ��
 * @param subclass_boot     ��������ģʽ��@ref app_usbd_hid_subclass_t���ͣ�
 *
 * @code
 * APP_USBD_HID_MOUSE_GLOBAL_DEF(my_awesome_mouse, 0, NRF_DRV_USBD_EPIN1, 3, NULL);
 * @endcode
 */
#define APP_USBD_HID_MOUSE_GLOBAL_DEF(instance_name,                                                    \
                                      interface_number,                                                 \
                                      endpoint,                                                         \
                                      bcnt,                                                             \
                                      user_ev_handler,                                                  \
                                      subclass_boot)                                                    \
    APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(mouse_desc, APP_USBD_HID_MOUSE_REPORT_DSC_BUTTON(bcnt));  \
    static const app_usbd_hid_subclass_desc_t * mouse_descs[] = {&mouse_desc};                          \
    APP_USBD_HID_MOUSE_GLOBAL_DEF_INTERNAL(instance_name,                                               \
                                           interface_number,                                            \
                                           endpoint,                                                    \
                                           bcnt,                                                        \
                                           user_ev_handler,                                             \
                                           subclass_boot)

/**
 * @brief ��HID����ڲ���ȡ��ʵ���İ���������
 *
 * @param[in] p_mouse HID���ʵ������@ref APP_USBD_HID_MOUSE_GLOBAL_DEF���壩��
 *
 * @return ����ʵ����
 */
static inline app_usbd_class_inst_t const *
app_usbd_hid_mouse_class_inst_get(app_usbd_hid_mouse_t const * p_mouse)
{
    return &p_mouse->base;
}

/**
 * @brief �ӻ���ʵ����ȡHID���ʵ���İ���������
 *
 * @param[in] p_inst ����ʵ����
 *
 * @return HID�����ʵ�������
 */
static inline app_usbd_hid_mouse_t const *
app_usbd_hid_mouse_class_get(app_usbd_class_inst_t const * p_inst)
{
    return (app_usbd_hid_mouse_t const *)p_inst;
}

/**
 * @brief �ƶ������X���ϵ�λ�á�
 *
 * @param[in] p_mouse HID���ʵ������@ref APP_USBD_HID_MOUSE_GLOBAL_DEF���壩��
 * @param[in] offset  ���X�������ƶ�λ�ã�֧��int8_t��������Χ����
 *
 * @return ��׼�����롣
 */
ret_code_t app_usbd_hid_mouse_x_move(app_usbd_hid_mouse_t const * p_mouse, int8_t offset);

/**
 * @brief �ƶ������Y���ϵ�λ�á�
 *
 * @param[in] p_mouse HID���ʵ������@ref APP_USBD_HID_MOUSE_GLOBAL_DEF���壩��
 * @param[in] offset  ���Y�������ƶ�λ�ã�֧��int8_t��������Χ����
 *
 * @return ��׼�����롣
 */
ret_code_t app_usbd_hid_mouse_y_move(app_usbd_hid_mouse_t const * p_mouse, int8_t offset);

/**
 * @brief �������ƶ���
 *
 * @param[in] p_mouse HID���ʵ������@ref APP_USBD_HID_MOUSE_GLOBAL_DEF���壩��
 * @param[in] offset  ���ֵ�����ƶ�λ�ã�֧��int8_t��������Χ����
 *
 * @return ��׼�����롣
 */
ret_code_t app_usbd_hid_mouse_scroll_move(app_usbd_hid_mouse_t const * p_mouse, int8_t offset);

/**
 * @brief ������갴��״̬��
 *
 * @param[in] p_mouse    HID���ʵ������@ref APP_USBD_HID_MOUSE_GLOBAL_DEF���壩��
 * @param[in] button_id  ������ţ�0��7����
 * @param[in] state      ����״̬��true ��ʾ���£�false ��ʾ�ͷš�
 *
 * @return ��׼�����롣
 */
ret_code_t app_usbd_hid_mouse_button_state(app_usbd_hid_mouse_t const * p_mouse,
                                           uint8_t button_id,
                                           bool state);

/**
 * @brief ����SET_PROTOCOL����ĺ�����
 *
 * @param[in] p_mouse HID���ʵ����
 * @param[in] ev      �û��¼���
 *
 * @return ��׼�����롣
 */
ret_code_t hid_mouse_on_set_protocol(app_usbd_hid_mouse_t const * p_mouse,
                                     app_usbd_hid_user_event_t ev);

/**
 * @brief ���HID��껺���������Ϳձ��档
 *
 * @param[in] p_inst ����ʵ����
 *
 * @return ��׼�����롣
 */
ret_code_t hid_mouse_clear_buffer(app_usbd_class_inst_t const * p_inst);

/** @} */

#endif /* APP_USBD_HID_MOUSE_H__ */
