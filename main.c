#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For timing

#include "skiplist.h"
#include "record.h"
#include "persistence.h"

#define INPUT_BUFFER_SIZE 256
#define DB_FILENAME "crud_database.bin"

void print_help()
{
    printf("\n--- C-CRUD SkipList DB ---\n");
    printf("Commands:\n");
    printf("  add <id> <name> <value>  - Add a new record\n");
    printf("  get <id>               - Retrieve a record by ID\n");
    printf("  del <id>               - Delete a record by ID\n");
    printf("  update <id> <name> <val>- Update record (name/value)\n");
    printf("  save [filename]        - Save DB (default: %s)\n", DB_FILENAME);
    printf("  load [filename]        - Load DB (default: %s)\n", DB_FILENAME);
    printf("  list                   - Display skip list levels (debug)\n");
    printf("  stats                  - Show list size and height\n");
    printf("  bulkadd <count>        - Add N random records for testing\n");
    printf("  help                   - Show this help message\n");
    printf("  quit                   - Exit the application\n");
    printf("--------------------------\n");
}

// Helper to measure time
typedef struct
{
    clock_t start;
    clock_t end;
} Timer;

void start_timer(Timer *t)
{
    t->start = clock();
}

double stop_timer(Timer *t)
{
    t->end = clock();
    return ((double)(t->end - t->start)) / CLOCKS_PER_SEC;
}

int main()
{
    char input[INPUT_BUFFER_SIZE];
    char command[32];
    char filename_buf[INPUT_BUFFER_SIZE]; // Buffer for optional filenames
    Timer timer;                          // For timing operations

    // --- Initialization ---
    printf("Loading database...\n");
    SkipList *db_list = load_database(DB_FILENAME);
    if (!db_list)
    {
        fprintf(stderr, "Fatal: Could not initialize database.\n");
        return 1;
    }
    printf("Database ready. Type 'help' for commands.\n");
    // ---------------------

    while (1)
    {
        printf("> ");
        if (!fgets(input, INPUT_BUFFER_SIZE, stdin))
        {
            printf("Error reading input or EOF reached. Exiting.\n");
            break; // Exit on read error or EOF
        }

        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;

        // Basic command parsing
        int id;
        char name[MAX_NAME_LEN];
        double value;
        int items_scanned = sscanf(input, "%s", command);

        if (items_scanned < 1)
        {
            continue; // Empty input
        }

        // --- Command Handling ---
        if (strcmp(command, "quit") == 0)
        {
            break; // Exit loop
        }
        else if (strcmp(command, "help") == 0)
        {
            print_help();
        }
        else if (strcmp(command, "add") == 0)
        {
            items_scanned = sscanf(input, "%*s %d %63s %lf", &id, name, &value);
            if (items_scanned == 3)
            {
                if (id < 0)
                {
                    printf("Error: ID must be non-negative.\n");
                    continue;
                }
                Record *new_rec = create_record(id, name, value);
                if (new_rec)
                {
                    start_timer(&timer);
                    int success = insert_skiplist(db_list, id, new_rec);
                    double elapsed = stop_timer(&timer);
                    if (success)
                    {
                        printf("Record ID %d added successfully. (%.6f s)\n", id, elapsed);
                    }
                    else
                    {
                        printf("Error: Failed to add record ID %d (duplicate or memory error?).\n", id);
                        free(new_rec); // Important: free the record if insertion failed
                    }
                }
            }
            else
            {
                printf("Usage: add <id> <name> <value>\n");
            }
        }
        else if (strcmp(command, "get") == 0)
        {
            items_scanned = sscanf(input, "%*s %d", &id);
            if (items_scanned == 1)
            {
                start_timer(&timer);
                Record *rec = search_skiplist(db_list, id);
                double elapsed = stop_timer(&timer);
                if (rec)
                {
                    printf("Record found (%.6f s):\n", elapsed);
                    print_record(rec);
                }
                else
                {
                    printf("Record ID %d not found. (%.6f s)\n", id, elapsed);
                }
            }
            else
            {
                printf("Usage: get <id>\n");
            }
        }
        else if (strcmp(command, "del") == 0)
        {
            items_scanned = sscanf(input, "%*s %d", &id);
            if (items_scanned == 1)
            {
                start_timer(&timer);
                int success = delete_skiplist(db_list, id);
                double elapsed = stop_timer(&timer);
                if (success)
                {
                    printf("Record ID %d deleted successfully. (%.6f s)\n", id, elapsed);
                }
                else
                {
                    printf("Error: Record ID %d not found. (%.6f s)\n", id, elapsed);
                }
            }
            else
            {
                printf("Usage: del <id>\n");
            }
        }
        else if (strcmp(command, "update") == 0)
        {
            items_scanned = sscanf(input, "%*s %d %63s %lf", &id, name, &value);
            if (items_scanned == 3)
            {
                start_timer(&timer);
                Record *rec = search_skiplist(db_list, id);
                double elapsed = stop_timer(&timer);
                if (rec)
                {
                    // Update in-place (key doesn't change)
                    strncpy(rec->name, name, MAX_NAME_LEN - 1);
                    rec->name[MAX_NAME_LEN - 1] = '\0';
                    rec->value = value;
                    printf("Record ID %d updated successfully. (Search took %.6f s)\n", id, elapsed);
                }
                else
                {
                    printf("Error: Record ID %d not found for update. (Search took %.6f s)\n", id, elapsed);
                }
            }
            else
            {
                printf("Usage: update <id> <new_name> <new_value>\n");
            }
        }
        else if (strcmp(command, "save") == 0)
        {
            const char *filename_to_save = DB_FILENAME;
            items_scanned = sscanf(input, "%*s %255s", filename_buf);
            if (items_scanned == 1)
            {
                filename_to_save = filename_buf;
            }
            start_timer(&timer);
            save_database(db_list, filename_to_save);
            double elapsed = stop_timer(&timer);
            printf("Save operation took %.6f s.\n", elapsed);
        }
        else if (strcmp(command, "load") == 0)
        {
            const char *filename_to_load = DB_FILENAME;
            items_scanned = sscanf(input, "%*s %255s", filename_buf);
            if (items_scanned == 1)
            {
                filename_to_load = filename_buf;
            }
            printf("Warning: Loading will replace the current in-memory database.\n");
            printf("Saving current data first is recommended (use 'save').\n");
            printf("Proceed with load? (y/N): ");
            if (fgets(input, INPUT_BUFFER_SIZE, stdin) && (input[0] == 'y' || input[0] == 'Y'))
            {
                printf("Loading from %s...\n", filename_to_load);
                start_timer(&timer);
                free_skiplist(db_list); // Free the old list first
                db_list = load_database(filename_to_load);
                double elapsed = stop_timer(&timer);
                if (!db_list)
                {
                    fprintf(stderr, "Fatal: Load failed, could not re-initialize database.\n");
                    return 1; // Critical error
                }
                printf("Load operation took %.6f s.\n", elapsed);
            }
            else
            {
                printf("Load cancelled.\n");
            }
        }
        else if (strcmp(command, "list") == 0)
        {
            display_skiplist_levels(db_list);
        }
        else if (strcmp(command, "stats") == 0)
        {
            printf("Database Stats:\n");
            printf("  Record Count: %lu\n", (unsigned long)db_list->size);
            printf("  Current Max Level: %d (0-based)\n", db_list->level);
        }
        else if (strcmp(command, "bulkadd") == 0)
        {
            int count = 0;
            items_scanned = sscanf(input, "%*s %d", &count);
            if (items_scanned == 1 && count > 0)
            {
                printf("Adding %d random records...\n", count);
                start_timer(&timer);
                int added_count = 0;
                int attempted_id = (db_list->size > 0) ? (rand() % (db_list->size * 5)) : 0; // Start somewhere random
                for (int i = 0; i < count;)
                {
                    // Try to find a unique ID quickly - not perfectly robust for dense IDs
                    while (search_skiplist(db_list, attempted_id) != NULL)
                    {
                        attempted_id++;
                    }

                    sprintf(name, "RandomName_%d", attempted_id);
                    value = (double)(rand() % 100000) / 100.0;
                    Record *rec = create_record(attempted_id, name, value);
                    if (rec && insert_skiplist(db_list, rec->id, rec))
                    {
                        added_count++;
                        i++; // Only increment loop counter on successful add
                    }
                    else if (rec)
                    {
                        free(rec); // Free if insert failed (shouldn't happen with check above)
                    }
                    attempted_id++; // Move to next potential ID
                    if (i > 0 && i % 10000 == 0)
                    { // Progress update
                        printf("... added %d records ...\n", added_count);
                    }
                }
                double elapsed = stop_timer(&timer);
                printf("Finished adding %d records in %.6f s (%.2f records/sec).\n",
                       added_count, elapsed, added_count / elapsed);
            }
            else
            {
                printf("Usage: bulkadd <count> (count > 0)\n");
            }
        }
        else
        {
            printf("Unknown command: '%s'. Type 'help' for commands.\n", command);
        }
    }

    // --- Cleanup ---
    printf("Exiting. Saving database to %s...\n", DB_FILENAME);
    save_database(db_list, DB_FILENAME); // Auto-save on exit
    free_skiplist(db_list);
    printf("Cleanup complete. Goodbye!\n");
    // ---------------

    return 0;
}