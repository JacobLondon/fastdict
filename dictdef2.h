/**
 * Keys
 * $TT IDENTIFIER_NAME (UPPER)
 * $Tt IdentifierName (Pascal)
 * $tt identifier_name (lower)
 * $K C type (int, char *, ...)
 * 
 * Values
 * $VV IDENTIFIER_NAME (UPPER)
 * $Vv IdentifierName (Pascal)
 * $vv identifier_name (lower)
 * $U C type (int, char *, ...)
 * 
 * Functions
 * $REFKEY    $K*($K), if T is 'char *' then return $K
 * $FREEKEY   void($K)
 * $FREEVALUE void($U)
 * $SIZEOFKEY size_t($K*)
 * $CMPKEY    int($K, $K)
 * 
 * If a value in the dictionary contains the 'zero value' for
 * its type, then it is considered to not contain anything.
 */
#ifndef DEF_DICTDEF_$TT_$VV_H
#define DEF_DICTDEF_$TT_$VV_H

#define DEF_PROTO
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

$H_INCLUDES

typedef struct DictNode$Tt$Vv {
    uint64_t hash;
    $K key;

    // If key is a pointer, key==NULL is a tombstone marker.
    // Otherwise, we must mark the tombstone in some other way.
#if ! $OPT_KEY_IS_PTR
  #if $OPT_KEY_IS_SMALLER_THAN_SIZE_T
    uint8_t tombstone;
    $U value;
  #else
    $U value;
    uint8_t tombstone;
  #endif
#else
    $U value;
#endif
} DictNode$Tt$Vv;

typedef struct Dict$Tt$Vv {
    DictNode$Tt$Vv *entries;
    void *index_table; // an array of uint8/16/32/64 depending on cap

    uint64_t len; // the number of elements compact in data
    uint64_t cap; // the maximum allocated size of both data and index_table
} Dict$Tt$Vv;

DEF_PROTO Dict$Tt$Vv dict_$tt_$vv_init_reserve(uint64_t cap);
DEF_PROTO Dict$Tt$Vv *dict_$tt_$vv_new_reserve(uint64_t cap);
DEF_PROTO Dict$Tt$Vv dict_$tt_$vv_init(void);
DEF_PROTO Dict$Tt$Vv *dict_$tt_$vv_new(void);
DEF_PROTO void dict_$tt_$vv_deinit(Dict$Tt$Vv *self);
DEF_PROTO void dict_$tt_$vv_del(Dict$Tt$Vv *self);
DEF_PROTO void dict_$tt_$vv_reset(Dict$Tt$Vv *self);

// set returns true if it inserted a brand new element, false if it overrided a
// previously existing element
DEF_PROTO bool dict_$tt_$vv_set(Dict$Tt$Vv *self, $K key, $U value);
DEF_PROTO $U *dict_$tt_$vv_get(Dict$Tt$Vv *self, $K key);
DEF_PROTO bool dict_$tt_$vv_contains(Dict$Tt$Vv *self, $K key);
DEF_PROTO void dict_$tt_$vv_remove(Dict$Tt$Vv *self, $K key);

// Traverse like an array. Note that return value of NULL doesn't necessarily
// mean the traversal is done, the node could be empty, and an empty node will
// have NULL returned instead of junk.
DEF_PROTO DictNode$Tt$Vv *dict_$tt_$vv_at(Dict$Tt$Vv *self, uint64_t i);

DEF_PROTO uint64_t dict_$tt_$vv_size(Dict$Tt$Vv *self);

// Traverse the key/values while skipping the NULL (tombstoned) nodes
#define DICT_$TT_$VV_ITER(DictPtr, Iter, Block) \
do { \
    for (uint64_t _##Iter##_i = 0; _##Iter##_i < dict_$tt_$vv_size(DictPtr); _##Iter##_i++) { \
        DictNode$Tt$Vv *Iter = dict_$tt_$vv_at((DictPtr), _##Iter##_i); \
        if (Iter) \
        { \
            Block \
        } \
    } \
} while (0)

DEF_PROTO DictNode$Tt$Vv *dict_$tt_$vv_iter(Dict$Tt$Vv *self);
DEF_PROTO DictNode$Tt$Vv *dict_$tt_$vv_next(Dict$Tt$Vv *self, DictNode$Tt$Vv *node);

#endif // DEF_DICTDEF_$TT_$VV_H
