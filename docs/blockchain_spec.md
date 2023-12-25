# Blockchain Specification

## Overview

This specification outlines the structure of a basic blockchain system consisting of pages, blocks,edits and fields.The blockchain is made of pages, each page is made of 256 blocks. Each block is designed to store a series of edits, each containing arbitrary fields and a corresponding signature. Each field can be of various data types such as strings, integers, and floating-point numbers.

## Data Structures

### Field

```c
struct Field {
    FieldType type;
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
```

- `type`: Enum representing the field type.
- `length`: Length of the data.
- `data_str`: the start of the string.
- `data_int8`, `data_int16`, ..., `data_double`: Data for respective numeric types.

### Edit
An edit is a collection of fields, and has the following header:
```c
struct Edit_header {
    int num_fields;
    char signature[256];
};
```

- `num_fields`: number of fields following the header.
- `signature`: Signature for the edit.

### Block
A block is a collection of edits, and has the following header:
```c
struct Block_header {
    unsigned long long timestamp;
    unsigned in num_edits;
    unsigned char prev_block_hash[SHA256_DIGEST_LENGTH];
};
```

- `timestamp`: Time when the block was created.
- `prev_block_hash`: SHA256 hash of the previous block, only including the original(0th edit).
- `num_edits`: Number of edits following the header.

### Page
 A page is a collection of 256 blocks, and has the following header:
```c
struct Page_header {
    unsigned int sha256_checksum[8];
};
```
- `sha256_checksum`: SHA256 checksum of the whole page.

## Notes

- Adjust the size of arrays and fields as needed based on specific requirements.
- Implement additional functionalities based on the desired features of the blockchain system.

