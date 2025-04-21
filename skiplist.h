#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "record.h"
#include <stdlib.h> // size_t

// --- Tunable Parameters ---
// Maximum possible levels in the skip list (adjust based on expected N)
// log_p(N). For p=0.5, log2(N). For N=millions (e.g., 2^24), ~24 levels needed.
// Add some buffer. 32 is generally safe for large datasets.
#define MAX_LEVEL 32
// Probability factor for level generation (0.5 is common)
#define SKIPLIST_P 0.5
// -------------------------

// Forward declaration
typedef struct SkipListNode SkipListNode;

// Node structure for the skip list
struct SkipListNode
{
    int key;                // The ID of the record (used for sorting/searching)
    Record *value;          // Pointer to the actual data record
    int level;              // Highest level this node participates in (0-based)
    SkipListNode **forward; // Array of forward pointers
};

// Skip list structure
typedef struct
{
    SkipListNode *header; // Pointer to the header node
    int level;            // Current highest level in the list (0-based)
    size_t size;          // Number of elements in the list
} SkipList;

// --- Function Prototypes ---

// Core Skip List Operations
SkipList *create_skiplist();
Record *search_skiplist(SkipList *list, int search_key);
int insert_skiplist(SkipList *list, int key, Record *value); // Returns 1 on success, 0 on duplicate
int delete_skiplist(SkipList *list, int key);                // Returns 1 on success, 0 if not found
void free_skiplist(SkipList *list);

// Helper for debugging (optional)
void display_skiplist_levels(SkipList *list); // Simple level display

#endif // SKIPLIST_Hs