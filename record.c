#include "record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Record *create_record(int id, const char *name, double value)
{
    Record *rec = (Record *)malloc(sizeof(Record));
    if (!rec)
    {
        perror("Failed to allocate memory for record");
        return NULL;
    }
    rec->id = id;
    strncpy(rec->name, name, MAX_NAME_LEN - 1);
    rec->name[MAX_NAME_LEN - 1] = '\0'; // Ensure null termination
    rec->value = value;
    return rec;
}

void print_record(const Record *record)
{
    if (record)
    {
        printf("  ID   : %d\n", record->id);
        printf("  Name : %s\n", record->name);
        printf("  Value: %.2f\n", record->value);
    }
    else
    {
        printf("  (Record not found or NULL)\n");
    }
}

// No free_record needed here as it's handled by skiplist delete/free