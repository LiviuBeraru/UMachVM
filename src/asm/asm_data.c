#include <stdlib.h>
#include <string.h>

#include "asm_data.h"
#include "asm_labels.h"

unsigned char begin_data[4] = {0xFF, 'D', 'A', 'T'};

struct data_item {
   char *label;
   char *data;
   int   datasize;
   struct data_item *next;
};

static struct data_item *head = NULL;
static struct data_item *last = NULL;

void insert_data(const char *label, void *data, size_t n)
{
    /* we allocate space for 4 byte aligned data */
    int datasize = (4 - (n % 4)) % 4; // difference to the next multiple of 4
    datasize += n;
    
    /* create new list element */
    struct data_item *item = calloc(1, sizeof(struct data_item));
    
    /* fill in the element's data */
    item->label = calloc(strlen(label) + 1, sizeof(char)); // +1 for '\0'
    strcpy(item->label, label);
    
    item->data = calloc(datasize, 1);
    memcpy(item->data, data, n);
    
    item->datasize = datasize;
    
    /* insert into list */
    if (head == NULL) {
        /* first element inserted */
        head = item;
        last = item;
    } else {
        last->next = item;
        last = item;
    }
}

void delete_data(void)
{
    struct data_item *item = head;
    struct data_item *next = head;
    
    while (item) {
        next = item -> next;
        
        free(item->label);
        free(item->data);
        free(item);
        
        item = next;
    }
    
    head = NULL;
    last = NULL;
}

void translate_data_labels(int offset)
{
    /* offset is the number of instructions which were assembled.
     * this is also the offset of the first data chunk.
     */
    struct data_item *item = head;
    while (item) {
        label_insert_name(item->label);
        label_insert_offset(offset);
        
        /* offset of the next data is the current data size 
         * divided by 4 because we count offsets in 4 bytes each
         * data size is always a multiple of 4 so it's safe to divide by 4
         */
        offset += (item->datasize) / 4;
        item = item->next;
    }
}


void write_data(FILE *output)
{
    struct data_item *item = head; 
    
    /* write the data mark */
    fwrite(begin_data, 1, 4, output);
    
    while (item) {
        fwrite(item->data, 1, item->datasize, output);
        item = item -> next;
    }
}
