/*
 * struct.cc
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2024
 * Version:   2.0.0
 * License:   The MIT License (MIT)
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
#include <cstdlib>
#include <cstdio>
#include <inttypes.h>
#include "emblob_struct.h"

struct MyStruct
{
    uint32_t magic = 0U;
    uint16_t secret_id = 0;
    uint8_t text[16]{};
};

int main()
{
    // Obtain a typed pointer to the data structure by accessing it via an emblob
    // auto-generated function, and casting it to the desired type.
    auto ptr = static_cast<const MyStruct *>(emblob_get_struct_raw());

    // Print out the values the structure contains.
    auto struct_size = emblob_get_struct_size();
    printf("%" PRIu64 " bytes: magic = 0x%08X, secret_id = 0x%04X, text = '%s'\n",
        struct_size, ptr->magic, ptr->secret_id, ptr->text);

    return EXIT_SUCCESS;
}
