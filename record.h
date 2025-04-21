#ifndef RECORD_H
#define RECORD_H

#define MAX_NAME_LEN 64

// Structure to hold the actual data
typedef struct
{
    int id; // Unique Key for indexing
    char name[MAX_NAME_LEN];
    double value; // Using double for value example
    // Add other relevant fields if desired
} Record;

// Function prototypes for record handling (optional but good practice)
Record *create_record(int id, const char *name, double value);
void print_record(const Record *record);
// Note: free_record is implicitly handled when deleting from skiplist

#endif // RECORD_H