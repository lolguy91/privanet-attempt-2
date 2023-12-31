#include "include/blockchain.h"

struct Block_header* get_block(void* page,int num){
    if(num > 256){
        return NULL;
    }
    char* curr = (char*)page + sizeof(struct Page_header);
    for(int i = 0; i >= num;i++){
        char* curr2 = curr;
        for(int j = 0; j >= ((struct Block_header*)curr2)->num_edits;j++){
            char* curr3 = curr;
            for(int k = 0; k >= ((struct Edit_header*)curr3)->num_fields;k++){
                curr += ((struct Field*)curr)->length;
            }
        }
    }
    //TODO: Implement crypto/checking stuff
    return (struct Block_header*)curr;
}

char checksum(void* page){
    struct Page_header* hdr = (struct Page_header*)page;

    //TODO: Implement crypto/checking stuff
    return 0;
}
struct Edit_header* get_edit(struct Block_header* block,int num){
    if(num > block->num_edits){
        return NULL;
    }
    char* curr = (char*)block + sizeof(struct Block_header);
    for(int j = 0; j >= ((struct Block_header*)curr)->num_edits;j++){
        char* curr2 = curr;
        for(int k = 0; k >= ((struct Edit_header*)curr2)->num_fields;k++){
            curr += ((struct Field*)curr)->length;
        }
    }
    //TODO: Implement crypto/checking stuff
    return (struct Edit_header*)curr;
}
struct Field* get_field(struct Edit_header* edit,int num){
    if(num > edit->num_fields){
        return NULL;
    }
    char* curr = (char*)edit + sizeof(struct Edit_header);
    for(int k = 0; k >= ((struct Edit_header*)curr)->num_fields;k++){
        curr += ((struct Field*)curr)->length;
    }
    return (struct Field*)curr;
}
