#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "cache.h"

/**
 * Allocate a cache entry
 */
struct cache_entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    // calloc space entry
    struct cache_entry *new_entry = malloc(sizeof(struct cache_entry));

    new_entry->path = malloc(strlen(path) + 1);
    strcpy(new_entry->path, path);

    new_entry->content_type = malloc(strlen(content_type)+1);
    strcpy(new_entry->content_type, content_type);

    new_entry->content = malloc(sizeof(content));

    new_entry->content_length = content_length;
    
    new_entry->next = NULL;
    new_entry->prev = NULL;

    return new_entry;
}

/**
 * Deallocate a cache entry
 */
void free_entry(struct cache_entry *entry)
{
    free(entry->path);
    free(entry->content_type);
    free(entry->content);
    free(entry);
}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(struct cache *cache, struct cache_entry *ce)
{
    // Insert at the head of the list
    if (cache->head == NULL) {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    } else {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(struct cache *cache, struct cache_entry *ce)
{
    if (ce != cache->head) {
        if (ce == cache->tail) {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;

        } else {
            // We're neither the head nor the tail
            ce->prev->next = ce->next;
            ce->next->prev = ce->prev;
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}


/**
 * Removes the tail from the list and returns it
 * 
 * NOTE: does not deallocate the tail
 */
struct cache_entry *dllist_remove_tail(struct cache *cache)
{
    struct cache_entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 * 
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
struct cache *cache_create(int max_size, int hashsize)
{
    struct hashtable *ht = hashtable_create(hashsize, NULL);
    struct cache *new_cache = calloc(max_size, sizeof(struct cache));
    new_cache->cur_size = 0;
    new_cache->head = NULL;
    new_cache->tail = NULL;
    new_cache->index = ht;
    new_cache->max_size = max_size;

    return new_cache;
}

void cache_free(struct cache *cache)
{
    struct cache_entry *cur_entry = cache->head;

    hashtable_destroy(cache->index);

    while (cur_entry != NULL) {
        struct cache_entry *next_entry = cur_entry->next;

        free_entry(cur_entry);

        cur_entry = next_entry;
    }

    free(cache);
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 * 
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(struct cache *cache, char *path, char *content_type, void *content, int content_length)
{
//    * Allocate a new cache entry with the passed parameters.
    printf("3.0\n");
    struct cache_entry *new_entry = alloc_entry(path, content_type, content, content_length);

//    * Insert the entry at the head of the doubly-linked list.
    printf("3.1\n");
    dllist_insert_head(cache, new_entry);

//    * Store the entry in the hashtable as well, indexed by the entry's `path`.
    printf("3.2\n");
    hashtable_put(cache->index, path, new_entry);

//    * Increment the current size of the cache.
    printf("3.3\n");
    cache->cur_size += 1;

//    * If the cache size is greater than the max size:
    printf("3.4\n");
    if(cache->cur_size > cache->max_size){
//      * Remove the cache entry at the tail of the linked list.
        printf("3.5\n");
        struct cache_entry *remove_entry = dllist_remove_tail(cache);

//      * Remove that same entry from the hashtable, using the entry's `path` and the `hashtable_delete` function.
        printf("3.6\n");
        hashtable_delete(cache->index, remove_entry->path);

//      * Free the cache entry.
        printf("3.7\n");
        free_entry(remove_entry);

//      * Ensure the size counter for the number of entries in the cache is correct. This happens in remove_tail.
    }
}

/**
 * Retrieve an entry from the cache
 */
struct cache_entry *cache_get(struct cache *cache, char *path)
{
//    * Attempt to find the cache entry pointer by `path` in the hash table.
    printf("2.0\n");
    struct cache_entry *search_entry = hashtable_get(cache->index, path);

//    * If not found, return `NULL`.
    printf("2.1\n");
    if(search_entry == NULL){
        return NULL;
    }

//    * Move the cache entry to the head of the doubly-linked list.
    printf("2.2\n");

    dllist_move_to_head(cache, search_entry);
    
//    * Return the cache entry pointer.
    printf("2.3\n");

    return search_entry;

}
