#ifndef ASM_LABELS_H
#define ASM_LABELS_H

/** Insert a label name. 
 *  The label is first inserted into a pending list 
 *  until you associate it with an offset. */
void label_insert_name   (const char *name);

void label_insert_offset (int offset);
int  label_get_offset    (const char *labelname, int *offset);

/** Delete all labels. */
void labels_delete(void);

/** Prints all labels. Debugging function. */
void printlabels(void);

#endif
