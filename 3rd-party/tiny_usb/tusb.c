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

#include "tusb.h"

#include "common/tusb_error.h"
#include "device/usbd_pvt.h"
#include "tusb_option.h"

bool tusb_init(void)
{
  TU_ASSERT ( tud_init() ); // init device stack
  return true;
}

bool tusb_inited(void)
{
  bool ret = false;

  ret = ret || tud_inited();

  return ret;
}

/*------------------------------------------------------------------*/
/* Debug
 *------------------------------------------------------------------*/
#if CFG_TUSB_DEBUG
#include <ctype.h>

char const* const tusb_strerr[TUSB_ERROR_COUNT] = { ERROR_TABLE(ERROR_STRING) };

static void dump_str_line(unsigned char const* buf, unsigned short count)
{
  tu_printf("  |");

  // each line is 16 bytes
  for(unsigned short i=0; i<count; i++)
  {
    const char ch = buf[i];
    tu_printf("%c", isprint(ch) ? ch : '.');
  }

  tu_printf("|\r\n");
}

/* Print out memory contents
 *  - buf   : buffer
 *  - count : number of item
 *  - indent: prefix spaces on every line
 */
void tu_print_mem(void const *buf, unsigned int count, unsigned char indent)
{
  unsigned char const size = 1; // fixed 1 byte for now

  if ( !buf || !count )
  {
    tu_printf("NULL\r\n");
    return;
  }

  unsigned char const *buf8 = (unsigned char const *) buf;

  char format[] = "%00X";
  format[2] += 2*size;

  const unsigned char item_per_line  = 16 / size;

  for(unsigned int i=0; i<count; i++)
  {
    unsigned int value=0;

    if ( i%item_per_line == 0 )
    {
      // Print Ascii
      if ( i != 0 )
      {
        dump_str_line(buf8-16, 16);
      }

      for(unsigned char s=0; s < indent; s++) tu_printf(" ");

      // print offset or absolute address
      tu_printf("%04X: ", 16*i/item_per_line);
    }

    memcpy(&value, buf8, size);
    buf8 += size;

    tu_printf(" ");
    tu_printf(format, value);
  }

  // fill up last row to 16 for printing ascii
  const unsigned int remain = count%16;
  unsigned char nback = (remain ? remain : 16);

  if ( remain )
  {
    for(unsigned int i=0; i< 16-remain; i++)
    {
      tu_printf(" ");
      for(int j=0; j<2*size; j++) tu_printf(" ");
    }
  }

  dump_str_line(buf8-nback, nback);
}

#endif
