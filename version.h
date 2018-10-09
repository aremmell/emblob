#ifndef _VERSION_H_INCLUDED
#define _VERSION_H_INCLUDED

#include <stdint.h>
#include <string.h>

#define MAX_VERSION_NOTES 256

struct version_resource
{
    uint16_t major;
    uint16_t minor;
    uint16_t build;
    char notes[MAX_VERSION_NOTES];
};

extern uint8_t _binary_VERSION_start;
extern uint8_t _binary_VERSION_end;

static inline void get_version_resource(version_resource* out)
{
    memcpy(out, &_binary_VERSION_start, &_binary_VERSION_end - &_binary_VERSION_start);
}

#endif /* !_VERSION_H_INCLUDED */
