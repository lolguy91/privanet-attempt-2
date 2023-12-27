#ifndef __PACKETS_H_
#define __PACKETS_H_

#include <stdint.h>

enum FieldType {
	PT_BEAM = 0x00, // Beam
	PT_IDENT = 0x01, // Identification
	PT_RESP_IDENT = 0x02, // Identification Response
	PT_DATA = 0x03, // Data
};

struct Field {
    FieldType type;
    uint64_t length;
    union {
        char data_char;
        int8_t data_int8;
        int16_t data_int16;
        int32_t data_int32;
        int64_t data_int64;
        uint8_t data_uint8;
        uint16_t data_uint16;
        uint32_t data_uint32;
        uint64_t data_uint64;
        float data_float;
        double data_double;
    };
};

#endif /* __PACKETS_H_ */