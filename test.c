#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // Included for potential future use, not strictly needed now

#include "skiplist.h" // Needs access to skiplist operations
#include "record.h"   // Needs access to Record definition and create_record

// --- Timer Structure ---
typedef struct {
    clock_t start;
    clock_t end;
} Timer;

void start_timer(Timer* t) {
    t->start = clock();
}

double stop_timer(Timer* t) {
    t->end = clock();
    return ((double)(t->end - t->start)) / CLOCKS_PER_SEC;
}
// --- End Timer ---

// --- Helper for Test Modes ---
// Simple Fisher-Yates shuffle for randomizing IDs
void shuffle_ids(int* array, size_t n) {
    if (n > 1) {
        for (size_t i = n - 1; i > 0; i--) {
            // Ensure rand() is seeded before calling this
            size_t j = (size_t)rand() % (i + 1);
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }
}

// --- Test Mode Functions (Copied from previous main.c modification) ---

// Performs N insertions (IDs 0 to N-1) and prints timing
void run_test_insert(long n) {
    if (n <= 0) {
        fprintf(stderr, "Error: Number of insertions (N) must be positive.\n");
        return;
    }
    SkipList* list = create_skiplist();
    if (!list) {
        fprintf(stderr, "Fatal: Failed to create skiplist for test.\n");
        return;
    }

    Timer timer;
    char name_buf[MAX_NAME_LEN];
    double value;
    Record* rec;
    int success_count = 0;

    printf("test_type,N,M,total_time_s,avg_time_per_op_s\n"); // CSV Header

    start_timer(&timer);
    for (long i = 0; i < n; ++i) {
        snprintf(name_buf, MAX_NAME_LEN, "Record_%ld", i);
        value = (double)(i % 1000); // Simple value
        rec = create_record((int)i, name_buf, value);
        if (rec && insert_skiplist(list, (int)i, rec)) {
             success_count++;
        } else if (rec) {
             fprintf(stderr, "Warning: Failed to insert test record ID %ld\n", i);
             free(rec); // Free if insert failed
        } else {
             fprintf(stderr, "Warning: Failed to create test record ID %ld\n", i);
        }
    }
    double elapsed = stop_timer(&timer);

    if (success_count > 0) {
         printf("insert,%ld,%ld,%.6f,%.9f\n",
               n, n, elapsed, elapsed / success_count);
    } else {
         printf("insert,%ld,%ld,%.6f,NaN\n", n, n, elapsed); // Avoid division by zero
    }


    free_skiplist(list);
}

// Performs M searches on a list pre-filled with N records (IDs 0 to N-1)
void run_test_search(long n, long m) {
     if (n <= 0 || m <= 0) {
        fprintf(stderr, "Error: N and M must be positive for search test.\n");
        return;
    }
     if (m > n) {
         fprintf(stderr, "Warning: M (%ld) > N (%ld) for search, may search duplicates/non-existent.\n", m, n);
     }

    SkipList* list = create_skiplist();
     if (!list) { fprintf(stderr, "Fatal: Failed to create skiplist for test.\n"); return; }

    // 1. Pre-fill the list with N records
    char name_buf[MAX_NAME_LEN];
    double value;
    Record* rec;
    long prefill_success = 0;
    for (long i = 0; i < n; ++i) {
        snprintf(name_buf, MAX_NAME_LEN, "Record_%ld", i);
        value = (double)(i % 1000);
        rec = create_record((int)i, name_buf, value);
        if (rec && insert_skiplist(list, (int)i, rec)) {
             prefill_success++;
        } else if (rec) {
             free(rec);
        }
    }
    if (prefill_success != n) { // Check if prefill worked as expected
         fprintf(stderr, "Error: Pre-fill failed. Expected %ld records, inserted %ld.\n", n, prefill_success);
         free_skiplist(list);
         return;
    }


    // 2. Prepare M random IDs to search for (from 0 to N-1)
    int* search_ids = (int*)malloc(sizeof(int) * m);
    if (!search_ids) { fprintf(stderr, "Fatal: Failed to allocate memory for search IDs.\n"); free_skiplist(list); return; }
    for(long i = 0; i < m; ++i) {
        search_ids[i] = rand() % n; // Simple random selection
    }

    // 3. Perform M searches and time them
    Timer timer;
    int found_count = 0;
    start_timer(&timer);
    for (long i = 0; i < m; ++i) {
        if (search_skiplist(list, search_ids[i]) != NULL) {
            found_count++;
        }
    }
    double elapsed = stop_timer(&timer);

    // 4. Print results
     printf("test_type,N,M,total_time_s,avg_time_per_op_s\n"); // CSV Header
    if (m > 0) {
        printf("search,%ld,%ld,%.6f,%.9f\n",
               n, m, elapsed, elapsed / m);
    } else {
         printf("search,%ld,%ld,%.6f,NaN\n", n, m, elapsed);
    }

    // 5. Cleanup
    free(search_ids);
    free_skiplist(list);
}

// Performs M deletions on a list pre-filled with N records (IDs 0 to N-1)
void run_test_delete(long n, long m) {
    if (n <= 0 || m <= 0) { fprintf(stderr, "Error: N and M must be positive for delete test.\n"); return; }
    if (m > n) { fprintf(stderr, "Error: Cannot delete M (%ld) > N (%ld) distinct items.\n", m, n); return; }

    SkipList* list = create_skiplist();
    if (!list) { fprintf(stderr, "Fatal: Failed to create skiplist for test.\n"); return; }

    // 1. Pre-fill the list with N records (IDs 0 to N-1)
    char name_buf[MAX_NAME_LEN];
    double value;
    Record* rec;
    long prefill_success = 0;
    for (long i = 0; i < n; ++i) {
        snprintf(name_buf, MAX_NAME_LEN, "Record_%ld", i);
        value = (double)(i % 1000);
        rec = create_record((int)i, name_buf, value);
        if (rec && insert_skiplist(list, (int)i, rec)) {
             prefill_success++;
        } else if (rec) {
             free(rec);
        }
    }
     if (prefill_success != n) {
         fprintf(stderr, "Error: Pre-fill failed. Expected %ld records, inserted %ld.\n", n, prefill_success);
         free_skiplist(list);
         return;
     }


    // 2. Prepare M *distinct* IDs to delete
    int* all_ids = (int*)malloc(sizeof(int) * n);
    int* ids_to_delete = (int*)malloc(sizeof(int) * m);
    if (!all_ids || !ids_to_delete) {
        fprintf(stderr, "Fatal: Failed to allocate memory for delete IDs.\n");
        free(all_ids); free(ids_to_delete); free_skiplist(list); return;
    }

    for(long i = 0; i < n; ++i) all_ids[i] = (int)i;
    shuffle_ids(all_ids, n); // Shuffle them
    for(long i = 0; i < m; ++i) ids_to_delete[i] = all_ids[i]; // Take the first M

    free(all_ids);

    // 3. Perform M deletions and time them
    Timer timer;
    int delete_success_count = 0;
    start_timer(&timer);
    for (long i = 0; i < m; ++i) {
        if (delete_skiplist(list, ids_to_delete[i])) {
            delete_success_count++;
        } else {
             // This might happen if shuffling somehow produced duplicates, though unlikely for M<=N
             fprintf(stderr, "Warning: Failed to delete ID %d during test (already deleted?).\n", ids_to_delete[i]);
        }
    }
    double elapsed = stop_timer(&timer);

    // 4. Print results
    printf("test_type,N,M,total_time_s,avg_time_per_op_s\n"); // CSV Header
    if (delete_success_count > 0) {
        printf("delete,%ld,%ld,%.6f,%.9f\n",
               n, m, elapsed, elapsed / delete_success_count);
    } else {
         printf("delete,%ld,%ld,%.6f,NaN\n", n, m, elapsed);
    }

    // 5. Cleanup
    free(ids_to_delete);
    free_skiplist(list);
}


// --- Main Function for Testing ---
int main(int argc, char *argv[]) {
    // Seed random number generator (important for shuffle and skiplist level)
    srand((unsigned int)time(NULL));

    // Argument Parsing for Test Modes
    if (argc < 3) { // Need at least program name and test type
        usage: // Label for goto
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s --test-insert <N>\n", argv[0]);
        fprintf(stderr, "  %s --test-search <N> <M>\n", argv[0]);
        fprintf(stderr, "  %s --test-delete <N> <M>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--test-insert") == 0) {
        if (argc != 3) goto usage;
        long n = atol(argv[2]);
        run_test_insert(n);
    } else if (strcmp(argv[1], "--test-search") == 0) {
        if (argc != 4) goto usage;
        long n = atol(argv[2]);
        long m = atol(argv[3]);
        run_test_search(n, m);
    } else if (strcmp(argv[1], "--test-delete") == 0) {
         if (argc != 4) goto usage;
        long n = atol(argv[2]);
        long m = atol(argv[3]);
        run_test_delete(n, m);
    } else {
        fprintf(stderr, "Error: Unknown test type '%s'\n", argv[1]);
        goto usage;
    }

    return 0; // Exit after test
}