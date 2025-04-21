#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "skiplist.h"

int save_database(SkipList *list, const char *filename);
SkipList *load_database(const char *filename);

#endif // PERSISTENCE_H