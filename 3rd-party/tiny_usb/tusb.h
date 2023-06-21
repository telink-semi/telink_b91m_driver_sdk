/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#pragma once

//--------------------------------------------------------------------+
// INCLUDE
//--------------------------------------------------------------------+
#include "class/msc/msc_device.h"
#include "common/tusb_common.h"
#include "common/tusb_fifo.h"
#include "common/tusb_verify.h"
#include "device/usbd.h"
#include "osal/osal_none.h"
//--------------------------------------------------------------------+
// APPLICATION API
//--------------------------------------------------------------------+
/** \ingroup group_application_api
 *  @{ */

// Initialize device/host stack
// Note: when using with RTOS, this should be called after scheduler/kernel is started.
// Otherwise it could cause kernel issue since USB IRQ handler does use RTOS queue API.
bool tusb_init(void);

// Check if stack is initialized
bool tusb_inited(void);

// TODO
// bool tusb_teardown(void);

