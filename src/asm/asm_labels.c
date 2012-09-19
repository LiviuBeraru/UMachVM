#include <stdlib.h>
#include <string.h> // strncpy
#include <stdio.h>


struct label {
    char name[65];
    int  offset;
};

struct label_item {
    struct label      *label;
    struct label_item *next;
};

/** Stack of labels which have not been
 *  associated with an offset yet. When given an offset,
 *  the elements of this list go into the labels list.
 *  These are pending items, waiting for an offset. */
static struct label_item *pending = NULL;

/** Linked list of labels */
static struct label_item *labels = NULL;

static
struct label_item* new_label_item(void);
static void        delete_label_item(struct label_item* item);
static void        delete_list(struct label_item* head);

void label_insert_name(const char* name)
{
    struct label_item *item = new_label_item();
    strncpy(item->label->name, name, 64);
    item->next = pending;
    pending = item;
}

void label_insert_offset(int offset)
{
    if (pending == NULL) {
        return;
    }
    struct label_item *next = NULL;

    while (pending) {
        next = pending->next;        
        pending->label->offset = offset;
        pending->next = labels;
        labels = pending;

        pending = next;
    }
}

int label_get_offset(const char* labelname, int* offset)
{
    struct label_item *item = labels;
    char *name = NULL;
    while (item) {
        name = item->label->name;
        if (strcmp(name, labelname) == 0) {
            *offset = item->label->offset;
            return 0;
        }
        item = item -> next;
    }
    return -1;
}


void labels_delete(void)
{
    delete_list(pending);
    pending = NULL;
    delete_list(labels);
    labels = NULL;
}

struct label_item* new_label_item(void) {
    struct label_item *item  = malloc(sizeof(*item));
    struct label      *label = calloc(1, sizeof(*label));
    item -> label = label;
    item -> next = NULL;
    return item;
}

void delete_label_item(struct label_item* item )
{
    free(item->label);
    free(item);
}

/** Delete all elements of the list beginning with the specified list head. */
void delete_list(struct label_item* head)
{
    if (head == NULL) {
        return;
    }
    
    struct label_item *next = NULL;
    while (head) {
        next = head -> next;
        delete_label_item(head);
        head = next;
    }
}

void print_labels(void)
{
    if (labels == NULL) {
        printf("No labels here...\n");
        return;
    }
    struct label_item *item = labels;
    while (item) {
        printf("label: %s [%d]\n", item->label->name, item->label->offset);
        item = item -> next;
    }
}
