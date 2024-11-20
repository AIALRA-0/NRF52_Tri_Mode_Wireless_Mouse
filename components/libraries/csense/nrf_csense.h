/**
 * Copyright (c) 2016 - 2021, Nordic Semiconductor ASA
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
#ifndef NRF_CSENSE_H__
#define NRF_CSENSE_H__

#include <stdint.h>
#include "nrf.h"
#include "sdk_errors.h"
#include "app_timer.h"
#include "nrf_drv_csense.h"
#include "nrf_csense_macros.h"
#include "app_util.h"

/** @file
 *
 * @defgroup nrf_csense 电容传感器库
 * @{
 * @ingroup app_common
 *
 * @brief 该模块用于使用电容传感器库，支持多个滑块、旋转轮和按钮实例。
 */

/**
 * @brief 返回变量地址的宏。
 */
#define NRF_CSENSE_GET_INSTANCE_ID(instance) (&instance)

/**
 * @brief 静态分配电容传感器实例的内存。
 *
 * @param[in,out] name          将要创建的电容传感器实例的名称。
 * @param[in] p1                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 */
#define NRF_CSENSE_BUTTON_DEF(name, p1) NRF_CSENSE_INTERNAL_BUTTON_DEF(name, p1)

/**
 * @brief 用于创建一个2个触点的滑块实例的宏。
 *
 * @param[in,out] name          将要创建的电容传感器实例的名称。
 * @param[in] steps_no          相对触点数量。这意味着在滑块处理程序中，该滑块将给出 (1, steps_no) 之间的值。
 * @param[in] p1                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p2                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 */
#define NRF_CSENSE_SLIDER_2_DEF(name, steps_no, p1, p2) NRF_CSENSE_INTERNAL_SLIDER_2_DEF(name, steps_no, p1, p2)

/**
 * @brief 用于创建一个3个触点的滑块实例的宏。
 *
 * @param[in,out] name          将要创建的电容传感器实例的名称。
 * @param[in] steps_no          相对触点数量。这意味着在滑块处理程序中，该滑块将给出 (1, steps_no) 之间的值。
 * @param[in] p1                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p2                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p3                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 */
#define NRF_CSENSE_SLIDER_3_DEF(name, steps_no, p1, p2, p3) NRF_CSENSE_INTERNAL_SLIDER_3_DEF(name, steps_no, p1, p2, p3)

/**
 * @brief 用于创建一个4个触点的滑块实例的宏。
 *
 * @param[in,out] name          将要创建的电容传感器实例的名称。
 * @param[in] steps_no          相对触点数量。这意味着在滑块处理程序中，该滑块将给出 (1, steps_no) 之间的值。
 * @param[in] p1                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p2                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p3                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p4                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 */
#define NRF_CSENSE_SLIDER_4_DEF(name, steps_no, p1, p2, p3, p4) NRF_CSENSE_INTERNAL_SLIDER_4_DEF(name, steps_no, p1, p2, p3, p4)

/**
 * @brief 用于创建一个5个触点的滑块实例的宏。
 *
 * @param[in,out] name          将要创建的电容传感器实例的名称。
 * @param[in] steps_no          相对触点数量。这意味着在滑块处理程序中，该滑块将给出 (1, steps_no) 之间的值。
 * @param[in] p1                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p2                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p3                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p4                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p5                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 */
#define NRF_CSENSE_SLIDER_5_DEF(name, steps_no, p1, p2, p3, p4, p5) NRF_CSENSE_INTERNAL_SLIDER_5_DEF(name, steps_no, p1, p2, p3, p4, p5)

/**
 * @brief 用于创建一个3个触点的旋转轮实例的宏。
 *
 * @param[in,out] name          将要创建的电容传感器实例的名称。
 * @param[in] steps_no          相对触点数量。这意味着在滑块处理程序中，该轮将给出 (1, steps_no) 之间的值。
 * @param[in] p1                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p2                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p3                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 */
#define NRF_CSENSE_WHEEL_3_DEF(name, steps_no, p1, p2, p3) NRF_CSENSE_INTERNAL_WHEEL_3_DEF(name, steps_no, p1, p2, p3)

/**
 * @brief 用于创建一个4个触点的旋转轮实例的宏。
 *
 * @param[in,out] name          将要创建的电容传感器实例的名称。
 * @param[in] steps_no          相对触点数量。这意味着在滑块处理程序中，该轮将给出 (1, steps_no) 之间的值。
 * @param[in] p1                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p2                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p3                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p4                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 */
#define NRF_CSENSE_WHEEL_4_DEF(name, steps_no, p1, p2, p3, p4) NRF_CSENSE_INTERNAL_WHEEL_4_DEF(name, steps_no, p1, p2, p3, p4)

/**
 * @brief 用于创建一个5个触点的旋转轮实例的宏。
 *
 * @param[in,out] name          将要创建的电容传感器实例的名称。
 * @param[in] steps_no          相对触点数量。这意味着在滑块处理程序中，该轮将给出 (1, steps_no) 之间的值。
 * @param[in] p1                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p2                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p3                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p4                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 * @param[in] p5                两个参数的组合：阈值和模拟输入编号。必须以(analog_input_number, threshold)形式传递。
 */
#define NRF_CSENSE_WHEEL_5_DEF(name, steps_no, p1, p2, p3, p4, p5) NRF_CSENSE_INTERNAL_WHEEL_5_DEF(name, steps_no, p1, p2, p3, p4, p5)

/**
 * @cond (NODOX)
 * @defgroup nrf_csense_internal 辅助内部类型声明
 * @brief 内部使用模块定义，仅限库内部使用。
 * @details 这些定义对用户可见，但不建议直接访问它们。请使用API进行访问。
 * @{
 *
 */

    /*
     * @brief 电容传感器实例的前向声明。
     */
    typedef struct nrf_csense_instance_s nrf_csense_instance_t;

    /*
     * @brief 电容传感器触点的前向声明。
     */
    typedef struct nrf_csense_pad_s nrf_csense_pad_t;

    /**
     * @brief 包含测量的触点最小值和最大值的结构体。
     */
    typedef struct
    {
        uint16_t                   max_value;                //!< 触点上测量到的最大值。
        uint16_t                   min_value;                //!< 触点上测量到的最小值。
    }nrf_csense_min_max_t;

    /**
     * @brief 包含单个实例参数的结构体。它可以是一个滑块或按钮。
     */
    struct nrf_csense_instance_s
    {
        nrf_csense_instance_t *     p_next_instance;            //!< 指向下一个实例的指针。
        nrf_csense_pad_t *          p_nrf_csense_pad;           //!< 指向该模块第一个触点的指针。
        nrf_csense_min_max_t *      min_max;                    //!< 包含触点上测量到的最小和最大值的结构体指针。
        uint16_t                    steps;                      //!< 相对触点数量。在滑块处理程序中，滑块将返回 (1, steps_no) 之间的值。
        uint8_t                     number_of_pads;             //!< 实例使用的触点数量。
        bool                        is_active;                  //!< 指示实例是否激活的标志。
        bool                        is_touched;                 //!< 指示实例是否被触摸的标志。
        void *                      p_context;                  //!< 通用指针，用于存储上下文数据。
    };

    /* 初始化时使用的单个触点参数结构体。 */
    struct nrf_csense_pad_s
    {
        nrf_csense_pad_t *          p_next_pad;                 //!< 指向下一个触点的指针。
        uint16_t                    threshold;                  //!< 触点上的阈值电压/充电时间，以确定是否触摸了触点。
        uint8_t                     pad_index;                  //!< 触点的索引号。
        uint8_t                     analog_input_number;        //!< 与触点连接的模拟输入编号。
    };

/** @}
 * @endcond
 */

/**
 * @brief 电容传感器事件类型枚举。
 */
typedef enum
{
    NRF_CSENSE_BTN_EVT_PRESSED,                                 //!< 触发触点按下事件。
    NRF_CSENSE_BTN_EVT_RELEASED,                                //!< 触发触点释放事件。
    NRF_CSENSE_SLIDER_EVT_PRESSED,                              //!< 触发滑块按下事件。
    NRF_CSENSE_SLIDER_EVT_RELEASED,                             //!< 触发滑块释放事件。
    NRF_CSENSE_SLIDER_EVT_DRAGGED,                              //!< 触发滑块拖动事件。
}nrf_csense_evt_type_t;

/**
 * @brief 包含滑块事件数据（包括测量到的步进值）的结构体。
 */
typedef struct
{
    uint16_t step;                                              //!< 测量到的步进值。
} nrf_csense_slider_evt_t;

/**
 * @brief 电容传感器事件的数据联合体。
 */
typedef union
{
    nrf_csense_slider_evt_t slider;                             //!< 包含滑块事件数据（包括测量到的步进值）的结构体。
} nrf_csense_evt_param_t;

/**
 * @brief 包含事件参数的结构体。
 */
typedef struct
{
    nrf_csense_evt_type_t       nrf_csense_evt_type;            //!< 事件类型。
    nrf_csense_instance_t *     p_instance;                     //!< 指向实例的指针。
    nrf_csense_evt_param_t      params;                         //!< 电容传感器事件的数据联合体。
}nrf_csense_evt_t;

/**
 * @brief 电容传感器事件处理函数类型。
 */
typedef void (* nrf_csense_event_handler_t)(nrf_csense_evt_t * p_evt);

/**
 * @brief 设置实例处理函数的函数。
 *
 * @param [in] p_instance                           指向要修改步进值的实例的指针。
 * @param [in] p_context                            通用指针。它将在回调函数中传递。
 */
__STATIC_INLINE void nrf_csense_instance_context_set(nrf_csense_instance_t * const p_instance, void * p_context)
{
    p_instance->p_context = p_context;
}

/**
 * @brief 初始化模块的函数。初始化后，没有任何实例被启用。
 *
 * @param [in] event_handler                          电容传感器模块的事件处理函数。
 * @param [in] ticks                                  在下一次转换之间的应用程序定时器的滴答数。
 *
 * @retval NRF_ERROR_INVALID_PARAM                    如果提供了无效参数。
 * @retval NRF_ERROR_INVALID_STATE                    如果使用的模块处于无效状态。
 * @retval NRF_ERROR_INTERNAL                         如果在初始化电容传感器库使用的模块之一时发生了错误。
 * @retval NRF_SUCCESS                                如果模块初始化成功。
 */
ret_code_t nrf_csense_init(nrf_csense_event_handler_t event_handler, uint32_t ticks);

/**
 * @brief 反初始化模块的函数。
 *
 * @return @ref nrf_drv_csense_uninit 和 @ref app_timer_stop 返回的值。
 */
ret_code_t nrf_csense_uninit(void);

/**
 * @brief 将电容传感器实例添加到链表的函数。
 *
 * 该函数调用 @ref nrf_csense_enable 启用添加到链表中的实例。
 *
 * @param [in] p_instance                            指向电容传感器实例的指针。它将由模块保存，并在引用该实例时使用。
 *
 * @return @ref nrf_csense_enable 返回的值。
 */
ret_code_t nrf_csense_add(nrf_csense_instance_t * const p_instance);

/**
 * @brief 启用单个实例的函数。
 *
 * @param [in,out] p_instance                        指向电容传感器实例的指针。它将由模块保存，并在引用该实例时使用。
 *
 * @return @ref app_timer_start 返回的值。
 */
ret_code_t nrf_csense_enable(nrf_csense_instance_t * const p_instance);

/**
 * @brief 禁用实例的函数。
 *
 * @param [in] p_instance                              指向要禁用的实例的指针。
 *
 * @retval NRF_ERROR_INVALID_PARAM                     如果实例已经被禁用。
 * @retval NRF_SUCCESS                                 如果实例成功禁用。
 */
ret_code_t nrf_csense_disable(nrf_csense_instance_t * const p_instance);

/**
 * @brief 设置下一次测量之间的滴答数的函数。
 *
 * @param [in] ticks                                 应用程序定时器的滴答数。
 *
 * @retval NRF_ERROR_BUSY                             如果电容传感器正忙。
 * @retval NRF_ERROR_INVALID_PARAM                    如果提供了无效的参数。
 * @retval NRF_ERROR_INVALID_STATE                    如果 app_timer 处于无效状态。
 * @retval NRF_SUCCESS                                如果滴答数成功设置。
 */
ret_code_t nrf_csense_ticks_set(uint32_t ticks);

/**
 * @brief 设置实例步进值的函数。
 *
 * 注意：在更改实例的步进值之前，必须禁用该实例。
 *
 * @param [in] p_instance                           指向要修改步进值的实例的指针。
 * @param [in] steps                                新的步进值。
 *
 * @retval NRF_ERROR_BUSY                           如果电容传感器正忙。
 * @retval NRF_SUCCESS                              如果步进值设置成功。
 */
ret_code_t nrf_csense_steps_set(nrf_csense_instance_t  * const p_instance, uint16_t steps);


/** @} */

#endif //NRF_CSENSE_H__
