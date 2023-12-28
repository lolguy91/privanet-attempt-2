#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

//Structs
struct Field {
    char type;
    unsigned long long length;
    union {
        char data_str;
        char data_int8;
        short data_int16;
        int data_int32;
        long long data_int64;
        unsigned char data_uint8;
        unsigned short data_uint16;
        unsigned int data_uint32;
        unsigned long long data_uint64;
        float data_float;
        double data_double;
    };
};

struct Edit_header {
    int num_fields;
    char signature[256];
};

struct Block_header {
    unsigned long long timestamp;
    unsigned int num_edits;
    unsigned int prev_block_hash[8];
};

struct Page_header {
    unsigned int sha256_checksum[8];
};

struct Block_header*    get_block(void*                  page ,int num);
char                    checksum (void*                  page         );
struct Edit_header*     get_edit (struct Block_header*   block,int num);
struct Field*           get_field(struct Edit_header*    edit ,int num);

#endif