#include "persistence.h"
#include "record.h" // Need MAX_NAME_LEN
#include <stdio.h>
#include <stdlib.h>

// Saves the skip list data to a binary file
int save_database(SkipList *list, const char *filename)
{
    if (!list || !filename)
        return 0;

    FILE *fp = fopen(filename, "wb"); // Open in binary write mode
    if (!fp)
    {
        perror("Error opening file for saving");
        return 0;
    }

    SkipListNode *current = list->header->forward[0];
    size_t records_written = 0;

    // Write the number of records first (optional but helpful for loading)
    if (fwrite(&list->size, sizeof(size_t), 1, fp) != 1)
    {
        perror("Error writing record count");
        fclose(fp);
        return 0;
    }

    // Iterate through level 0 (contains all nodes)
    while (current)
    {
        if (current->value)
        {
            // Write the entire Record struct directly
            if (fwrite(current->value, sizeof(Record), 1, fp) != 1)
            {
                perror("Error writing record data");
                // Attempt to close file, but indicate failure
                fclose(fp);
                return 0; // Indicate failure, data might be corrupt
            }
            records_written++;
        }
        current = current->forward[0];
    }

    fclose(fp);

    if (records_written != list->size)
    {
        fprintf(stderr, "Warning: Mismatch between list size (%lu) and records written (%lu).\n",
                (unsigned long)list->size, (unsigned long)records_written);

        // Decide if this is critical enough to return failure
    }

    printf("Database saved successfully to %s (%lu records).\n", filename, (unsigned long)records_written);
    return 1; // Success
}

// Loads data from a binary file into a new skip list
SkipList *load_database(const char *filename)
{
    FILE *fp = fopen(filename, "rb"); // Open in binary read mode
    if (!fp)
    {
        // It's okay if the file doesn't exist on first run
        // perror("Error opening file for loading");
        return create_skiplist(); // Return a new empty list
    }

    SkipList *list = create_skiplist();
    if (!list)
    {
        fclose(fp);
        return NULL; // Failed to create list structure
    }

    size_t record_count = 0;
    size_t records_read = 0;

    // Read the number of records (if saved)
    if (fread(&record_count, sizeof(size_t), 1, fp) != 1)
    {
        if (!feof(fp))
        { // Don't report error if file was just empty
            perror("Error reading record count");
        }
        // Continue trying to read records anyway, might be older format
        // Or return list here if count is critical
    }

    Record temp_record;
    // Read records one by one
    while (fread(&temp_record, sizeof(Record), 1, fp) == 1)
    {
        // Create a new Record on the heap to store in the list
        Record *new_rec = (Record *)malloc(sizeof(Record));
        if (!new_rec)
        {
            perror("Memory allocation failed during load");
            // Cleanup partially loaded list? Difficult. Best effort: return what we have.
            fclose(fp);
            return list; // Return partially loaded list
        }
        *new_rec = temp_record; // Copy data

        // Insert into the skip list
        if (!insert_skiplist(list, new_rec->id, new_rec))
        {
            fprintf(stderr, "Error inserting record ID %d during load (duplicate? memory?)\n", new_rec->id);
            free(new_rec); // Free the record we couldn't insert
            // Continue loading others?
        }
        else
        {
            records_read++;
        }
    }

    if (ferror(fp))
    {
        perror("Error reading from database file");
        // Keep partially loaded list? Or free and return NULL?
        // Let's return what we managed to load.
    }

    fclose(fp);
    printf("Database loaded successfully from %s (%lu records read).\n", filename, (unsigned long)records_read);
    return list;
}