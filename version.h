/*
 * @file version.h
 * @brief mkverobj API
 *
 * This file and accompanying source code originated from <https://github.com/aremmell/mkverobj/>.
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

/** auto-generated by mkverobj */
#ifdef __cplusplus
#   define MKVEROBJ_EXTERNAL extern "C"
#else
#   define MKVEROBJ_EXTERNAL extern
#endif

MKVEROBJ_EXTERNAL uint8_t version_data;

/** maximum length of notes field. */
#define MAX_VER_NOTES 256

/** complete version information structure. */
typedef struct
{
    uint16_t major;  /** major version */
    uint16_t minor;  /** minor version */
    uint16_t build;  /** build number */
    char notes[MAX_VER_NOTES]; /** notes (e.g. commit hash, date) */
} version_resource;

/** @brief retrieves address of version information.
 *
 * @return const version_resource*
 */
static inline const version_resource* get_version_resource(void)
{
    return (version_resource*)&version_data;
}

/** @brief retrieves major version number.
 *
 * @return uint16_t
 */
static inline uint16_t get_version_major(void)
{
    return get_version_resource()->major;
}

/** @brief retrieves minor version number.
 *
 * @return uint16_t
 */
static inline uint16_t get_version_minor(void)
{
    return get_version_resource()->minor;
}

/** @brief retrieves build version number.
 *
 * @return uint16_t
 */
static inline uint16_t get_version_build(void)
{
    return get_version_resource()->build;
}

/** @brief retrieves version notes string.
 *
 * @return const char*
 */
static inline const char* get_version_notes(void)
{
    return get_version_resource()->notes;
}

#endif /* !_MKVEROBJ_VERSION_H_INCLUDED */
