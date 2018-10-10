/**
 * @file version.h
 * @brief mkverobj API
 *
 * This file and accompanying source code originated from <https://github.com/ryanlederman/mkverobj>.
 * If you obtained it elsewhere, all bets are off.
 *
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @copyright
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ryan M. Lederman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _MKVEROBJ_VERSION_H_INCLUDED
#define _MKVEROBJ_VERSION_H_INCLUDED

#include <stdint.h>
#include <string.h>

/** auto-generated by mkverobj
 * =========================== */
extern uint8_t _binary_VERSION_start;
extern uint8_t _binary_VERSION_end;
/** ========================== */

typedef struct
{
    uint16_t major;  /** major version */
    uint16_t minor;  /** minor version */
    uint16_t build;  /** build number */
    char notes[256]; /** notes */
} version_resource;

static inline void get_version_resource(version_resource* restrict out)
{
    memcpy(out, &_binary_VERSION_start, &_binary_VERSION_end - &_binary_VERSION_start);
}

#endif /* !_MKVEROBJ_VERSION_H_INCLUDED */
