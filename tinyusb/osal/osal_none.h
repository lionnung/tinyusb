/*
 * osal_none.h
 *
 *  Created on: Jan 19, 2013
 *      Author: hathach
 */

/*
 * Software License Agreement (BSD License)
 * Copyright (c) 2013, hathach (tinyusb.net)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the tiny usb stack.
 */

/** \file
 *  \brief TBD
 *
 *  \note TBD
 */

/** \ingroup TBD
 *  \defgroup TBD
 *  \brief TBD
 *
 *  @{
 */

#ifndef _TUSB_OSAL_NONE_H_
#define _TUSB_OSAL_NONE_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "osal_common.h"

//--------------------------------------------------------------------+
// TICK API
//--------------------------------------------------------------------+
void osal_tick_tock(void);
uint32_t osal_tick_get(void);

//--------------------------------------------------------------------+
// TASK API
// NOTES: Each blocking OSAL_NONE services such as semaphore wait,
// queue receive embedded return statement, therefore local variable
// retain value before/after such services needed to declare as static
// OSAL_TASK_LOOP
// {
//   OSAL_TASK_LOOP_BEGIN
//
//   task body statements
//
//   OSAL_TASK_LOOP_ENG
// }
//--------------------------------------------------------------------+
#define OSAL_TASK_DEF(name, code, stack_depth, prio)
#define osal_task_create(x) TUSB_ERROR_NONE

#define OSAL_TASK_DECLARE(task_name) \
  tusb_error_t task_name(void)

#define TASK_RESTART state = 0

#define OSAL_TASK_LOOP_BEGIN \
  static uint32_t timeout = 0;\
  static uint16_t state = 0;\
  switch(state) {\
    case 0:\

#define OSAL_TASK_LOOP_END \
  default:\
    TASK_RESTART;\
  }\
  return TUSB_ERROR_NONE;

//------------- Sub Task -------------//
#define OSAL_SUBTASK_INVOKED_AND_WAIT(subtask) \
  do {\
    state = __LINE__; case __LINE__:\
    {\
      tusb_error_t status = subtask; /* invoke sub task */\
      if (TUSB_ERROR_OSAL_WAITING == status) /* sub task not finished -> continue waiting */\
        return TUSB_ERROR_OSAL_WAITING;\
      else if (TUSB_ERROR_NONE != status) { /* sub task failed -> restart task*/\
        TASK_RESTART;\
        return status;\
      }/* sub task finished ok --> continue */\
    }\
  }while(1)

#define OSAL_SUBTASK_BEGIN OSAL_TASK_LOOP_BEGIN
#define OSAL_SUBTASK_END OSAL_TASK_LOOP_END

//------------- Task Assert -------------//
#define _TASK_ASSERT_ERROR_HANDLER(error, func_call) \
  func_call; TASK_RESTART; return error

#define TASK_ASSERT_STATUS(sts) \
    ASSERT_DEFINE_WITH_HANDLER(_TASK_ASSERT_ERROR_HANDLER, , tusb_error_t status = (tusb_error_t)(sts),\
                               TUSB_ERROR_NONE == status, status, "%s", TUSB_ErrorStr[status])

#define TASK_ASSERT_STATUS_WITH_HANDLER(sts, func_call) \
    ASSERT_DEFINE_WITH_HANDLER(_TASK_ASSERT_ERROR_HANDLER, func_call, tusb_error_t status = (tusb_error_t)(sts),\
                               TUSB_ERROR_NONE == status, status, "%s", TUSB_ErrorStr[status])

#define TASK_ASSERT(condition)  \
    ASSERT_DEFINE_WITH_HANDLER(_TASK_ASSERT_ERROR_HANDLER, , , \
                               (condition), TUSB_ERROR_OSAL_TASK_FAILED, "%s", "evaluated to false")

#define TASK_ASSERT_WITH_HANDLER(condition, func_call) \
    ASSERT_DEFINE_WITH_HANDLER(_TASK_ASSERT_ERROR_HANDLER, func_call, ,\
                               condition, TUSB_ERROR_OSAL_TASK_FAILED, "%s", "evaluated to false")

//------------- Sub Task Assert -------------//
#define SUBTASK_ASSERT_STATUS(...)               TASK_ASSERT_STATUS(__VA_ARGS__)
#define SUBTASK_ASSERT_STATUS_WITH_HANDLER(...)  TASK_ASSERT_STATUS_WITH_HANDLER(__VA_ARGS__)
#define SUBTASK_ASSERT(...)                      TASK_ASSERT(__VA_ARGS__)
#define SUBTASK_ASSERT_WITH_HANDLER(...)         TASK_ASSERT_WITH_HANDLER(__VA_ARGS__)
//--------------------------------------------------------------------+
// Semaphore API
//--------------------------------------------------------------------+
typedef volatile uint8_t osal_semaphore_t;
typedef osal_semaphore_t * osal_semaphore_handle_t;

#define OSAL_SEM_DEF(name)\
  osal_semaphore_t name

#define OSAL_SEM_REF(name)\
  &name

static inline osal_semaphore_handle_t osal_semaphore_create(osal_semaphore_t * const p_sem) ATTR_ALWAYS_INLINE;
static inline osal_semaphore_handle_t osal_semaphore_create(osal_semaphore_t * const p_sem)
{
  (*p_sem) = 0;
  return (osal_semaphore_handle_t) p_sem;
}

static inline  tusb_error_t osal_semaphore_post(osal_semaphore_handle_t const sem_hdl) ATTR_ALWAYS_INLINE;
static inline  tusb_error_t osal_semaphore_post(osal_semaphore_handle_t const sem_hdl)
{
  (*sem_hdl)++;

  return TUSB_ERROR_NONE;
}

#define osal_semaphore_wait(sem_hdl, msec, p_error) \
  do {\
    timeout = osal_tick_get();\
    state = __LINE__; case __LINE__:\
    if( *(sem_hdl) == 0 ) {\
      if ( (msec != OSAL_TIMEOUT_WAIT_FOREVER) && (timeout + osal_tick_from_msec(msec) < osal_tick_get()) ) /* time out */ \
        *(p_error) = TUSB_ERROR_OSAL_TIMEOUT;\
      else\
        return TUSB_ERROR_OSAL_WAITING;\
    } else{\
      (*(sem_hdl))--; /*TODO mutex hal_interrupt_disable consideration*/\
      *(p_error) = TUSB_ERROR_NONE;\
    }\
  }while(0)

//--------------------------------------------------------------------+
// QUEUE API
//--------------------------------------------------------------------+
typedef struct{
           uint32_t * const buffer     ; ///< buffer pointer
           uint8_t const depth        ; ///< buffer size
  volatile uint8_t count          ; ///< bytes in fifo
  volatile uint8_t wr_idx       ; ///< write pointer
  volatile uint8_t rd_idx       ; ///< read pointer
} osal_queue_t;

typedef osal_queue_t * osal_queue_handle_t;

// use to declare a queue, within the scope of tinyusb, should only use primitive type only
#define OSAL_QUEUE_DEF(name, queue_depth, type)\
  uint32_t name##_buffer[queue_depth];\
  osal_queue_t name = {\
      .buffer = name##_buffer,\
      .depth   = queue_depth\
  }

static inline osal_queue_handle_t osal_queue_create(osal_queue_t * const p_queue) ATTR_ALWAYS_INLINE;
static inline osal_queue_handle_t osal_queue_create(osal_queue_t * const p_queue)
{
  p_queue->count = p_queue->wr_idx = p_queue->rd_idx = 0;
  return (osal_queue_handle_t) p_queue;
}

// when queue is full, it will overwrite the oldest data in the queue
static inline tusb_error_t osal_queue_send(osal_queue_handle_t const queue_hdl, uint32_t data) ATTR_ALWAYS_INLINE;
static inline tusb_error_t osal_queue_send(osal_queue_handle_t const queue_hdl, uint32_t data)
{
  //TODO mutex lock hal_interrupt_disable

  queue_hdl->buffer[queue_hdl->wr_idx] = data;
  queue_hdl->wr_idx = (queue_hdl->wr_idx + 1) % queue_hdl->depth;

  if (queue_hdl->depth == queue_hdl->count) // queue is full, 1st rd is overwritten
  {
    queue_hdl->rd_idx = queue_hdl->wr_idx; // keep full state
  }else
  {
    queue_hdl->count++;
  }

  //TODO mutex unlock hal_interrupt_enable

  return TUSB_ERROR_NONE;
}

#define osal_queue_receive(queue_hdl, p_data, msec, p_error) \
  do {\
    timeout = osal_tick_get();\
    state = __LINE__; case __LINE__:\
    if( queue_hdl-> count == 0 ) {\
      if ( (msec != OSAL_TIMEOUT_WAIT_FOREVER) && ( timeout + osal_tick_from_msec(msec) < osal_tick_get() )) /* time out */ \
        *(p_error) = TUSB_ERROR_OSAL_TIMEOUT;\
      else\
        return TUSB_ERROR_OSAL_WAITING;\
    } else{\
      /*TODO mutex lock hal_interrupt_disable */\
      *p_data = queue_hdl->buffer[queue_hdl->rd_idx];\
      queue_hdl->rd_idx = (queue_hdl->rd_idx + 1) % queue_hdl->depth;\
      queue_hdl->count--;\
      /*TODO mutex unlock hal_interrupt_enable */\
      *(p_error) = TUSB_ERROR_NONE;\
    }\
  }while(0)


// queue_send, queue_receive

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_OSAL_NONE_H_ */

/** @} */