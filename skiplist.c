#include "skiplist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Malloc, free
#include <time.h>   // For seeding random number generator

// --- Helper Functions ---

// Creates a new skip list node
static SkipListNode *create_node(int level, int key, Record *value)
{
    SkipListNode *node = (SkipListNode *)malloc(sizeof(SkipListNode));
    if (!node)
        return NULL;

    // Allocate space for the forward pointers array (level + 1 pointers)
    node->forward = (SkipListNode **)malloc(sizeof(SkipListNode *) * (level + 1));
    if (!node->forward)
    {
        free(node);
        return NULL;
    }

    // Initialize forward pointers to NULL
    for (int i = 0; i <= level; i++)
    {
        node->forward[i] = NULL;
    }

    node->key = key;
    node->value = value; // Stores the pointer to the actual record
    node->level = level;

    return node;
}

// Generates a random level for a new node
// Levels are 0-based
static int random_level()
{
    int level = 0;
    // RAND_MAX is defined in stdlib.h
    // Keep increasing level with probability SKIPLIST_P
    // Ensure level does not exceed MAX_LEVEL - 1
    while ((rand() < SKIPLIST_P * RAND_MAX) && level < (MAX_LEVEL - 1))
    {
        level++;
    }
    return level;
}

// --- Core Skip List Operations ---

SkipList *create_skiplist()
{
    SkipList *list = (SkipList *)malloc(sizeof(SkipList));
    if (!list)
        return NULL;

    // Create header node with minimum key value (or sentinel) and max level
    // Key = -1 assumes IDs are non-negative. Adjust if necessary.
    list->header = create_node(MAX_LEVEL - 1, -1, NULL); // Max level index
    if (!list->header)
    {
        free(list);
        return NULL;
    }

    list->level = 0; // Initially, list level is 0
    list->size = 0;

    // Seed random number generator once during initialization
    srand((unsigned int)time(NULL));

    return list;
}

Record *search_skiplist(SkipList *list, int search_key)
{
    if (!list)
        return NULL;
    SkipListNode *current = list->header;

    // Start from the highest level of the list
    for (int i = list->level; i >= 0; i--)
    {
        // Traverse right while next node's key is less than search_key
        while (current->forward[i] && current->forward[i]->key < search_key)
        {
            current = current->forward[i];
        }
    }

    // Moved down to level 0. The next node (if it exists) is the candidate.
    current = current->forward[0];

    // Check if the candidate node exists and its key matches
    if (current && current->key == search_key)
    {
        return current->value; // Return pointer to the Record
    }
    else
    {
        return NULL; // Not found
    }
}

int insert_skiplist(SkipList *list, int key, Record *value)
{
    if (!list || !value || key < 0)
        return 0; // Basic validation

    SkipListNode *update[MAX_LEVEL]; // Array to store pointers to nodes that need updating
    SkipListNode *current = list->header;

    // Find insertion points at each level and store predecessors in update[]
    for (int i = list->level; i >= 0; i--)
    {
        while (current->forward[i] && current->forward[i]->key < key)
        {
            current = current->forward[i];
        }
        update[i] = current; // Store the node where we moved down
    }

    // Move to the potential insertion point at level 0
    current = current->forward[0];

    // Check if key already exists
    if (current && current->key == key)
    {
        // Optionally update the existing value? For now, treat as duplicate error.
        // free(value); // Free the passed-in record if we don't insert it
        return 0; // Duplicate key found
    }

    // Key doesn't exist, proceed with insertion
    int new_level = random_level();

    // If the new node's level is higher than the current list level,
    // update the list level and initialize update pointers for new levels.
    if (new_level > list->level)
    {
        for (int i = list->level + 1; i <= new_level; i++)
        {
            update[i] = list->header; // New levels point back to header
        }
        list->level = new_level; // Update the list's max level
    }

    // Create the new node
    SkipListNode *new_node = create_node(new_level, key, value);
    if (!new_node)
        return 0; // Allocation failed

    // Insert the new node by updating forward pointers
    for (int i = 0; i <= new_level; i++)
    {
        new_node->forward[i] = update[i]->forward[i]; // New node points to what update[i] was pointing to
        update[i]->forward[i] = new_node;             // update[i] now points to the new node
    }

    list->size++;
    return 1; // Insertion successful
}

int delete_skiplist(SkipList *list, int key)
{
    if (!list || key < 0)
        return 0;

    SkipListNode *update[MAX_LEVEL];
    SkipListNode *current = list->header;

    // Find the node to delete and store predecessors in update[]
    for (int i = list->level; i >= 0; i--)
    {
        while (current->forward[i] && current->forward[i]->key < key)
        {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // Move to the potential node to delete at level 0
    current = current->forward[0];

    // Check if the node exists and key matches
    if (current && current->key == key)
    {
        // Update forward pointers to bypass the node to be deleted
        for (int i = 0; i <= current->level; i++)
        {
            // Only update if the predecessor at this level points to the node
            if (update[i]->forward[i] == current)
            {
                update[i]->forward[i] = current->forward[i];
            }
        }

        // Free the associated Record data first!
        if (current->value)
        {
            free(current->value);
        }
        // Free the forward pointers array of the node
        free(current->forward);
        // Free the node itself
        free(current);

        // Update the list level if the deleted node was the tallest
        // Check from top down if levels are now empty
        while (list->level > 0 && list->header->forward[list->level] == NULL)
        {
            list->level--;
        }

        list->size--;
        return 1; // Deletion successful
    }

    return 0; // Key not found
}

void free_skiplist(SkipList *list)
{
    if (!list)
        return;

    SkipListNode *current = list->header->forward[0]; // Start at the first actual node
    SkipListNode *next;

    // Traverse level 0 and free all nodes
    while (current)
    {
        next = current->forward[0];
        // Free the associated Record data
        if (current->value)
        {
            free(current->value);
        }
        // Free the node's forward array
        free(current->forward);
        // Free the node itself
        free(current);
        current = next;
    }

    // Free the header node's forward array and the header node itself
    free(list->header->forward);
    free(list->header);
    // Free the list structure
    free(list);
}

// Optional: Simple display for debugging
void display_skiplist_levels(SkipList *list)
{
    if (!list)
    {
        printf("Skip list is NULL.\n");
        return;
    }
    printf("\n--- Skip List Structure (Size: %lu) ---\n", (unsigned long)list->size);
    for (int i = list->level; i >= 0; i--)
    {
        SkipListNode *node = list->header->forward[i];
        printf("Level %d: Header -> ", i);
        while (node)
        {
            printf("[%d] -> ", node->key);
            node = node->forward[i];
        }
        printf("NULL\n");
    }
    printf("-------------------------------------\n");
}