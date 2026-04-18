#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "$__OUT_NAME__.h"
$C_INCLUDES

#define DEF_DICT_$TT_$VV_REFKEY(a) $REFKEY
#define DEF_DICT_$TT_$VV_FREEKEY(a) $FREEKEY
#define DEF_DICT_$TT_$VV_FREEVALUE(a) $FREEVALUE
#define DEF_DICT_$TT_$VV_SIZEOFKEY(a) $SIZEOFKEY
#define DEF_DICT_$TT_$VV_ZEROKEY(a) $ZEROKEY
#define DEF_DICT_$TT_$VV_ZEROVALUE(a) $ZEROVALUE
#define DEF_DICT_$TT_$VV_EQUAL_KEYS(a, b) ($EQUAL_KEYS)

#ifndef static_assert
#define static_assert(x, msg) \
do { \
    int _array[-1 + 2*(!!x)]; \
    (void)_array; \
} while (0)
#endif

#define DICT_$TT_$VV_DEFAULT_SIZE (8)
#define DICT_$TT_$VV_MIN_CAP (3)
#define DICT_$TT_$VV_LOAD_FACTOR_READY(len, cap) ((len) >= ((cap) * 2 / DICT_$TT_$VV_MIN_CAP))
#define DICT_$TT_$VV_GROWTH_FACTOR (2)

#define EMPTY8  UINT8_MAX
#define EMPTY16 UINT16_MAX
#define EMPTY32 UINT32_MAX
#define EMPTY64 UINT64_MAX

#define TOMBSTONE8  (UINT8_MAX-1)
#define TOMBSTONE16 (UINT16_MAX-1)
#define TOMBSTONE32 (UINT32_MAX-1)
#define TOMBSTONE64 (UINT64_MAX-1)

static void *_dict_$tt_$vv_create_index_table(uint64_t cap)
{
    assert(cap != 0);

    if (cap <= UINT8_MAX)
    {
        uint8_t *ptr = (uint8_t *)$MALLOC(sizeof(uint8_t) * cap);
        for (uint8_t i = 0; i < cap; i++) { ptr[i] = UINT8_MAX; }
        return ptr;
    }
    else if (cap <= UINT16_MAX)
    {
        uint16_t *ptr = (uint16_t *)$MALLOC(sizeof(uint16_t) * cap);
        for (uint16_t i = 0; i < cap; i++) { ptr[i] = UINT16_MAX; }
        return ptr;
    }
    else if (cap <= UINT32_MAX)
    {
        uint32_t *ptr = (uint32_t *)$MALLOC(sizeof(uint32_t) * cap);
        for (uint32_t i = 0; i < cap; i++) { ptr[i] = UINT32_MAX; }
        return ptr;
    }
    else
    {
        uint64_t *ptr = (uint64_t *)$MALLOC(sizeof(uint64_t) * cap);
        for (uint64_t i = 0; i < cap; i++) { ptr[i] = UINT64_MAX; }
        return ptr;
    }
}

static void *_dict_$tt_$vv_reset_index_table(void *index_table, uint64_t cap)
{
    assert(cap != 0);

    if (cap <= UINT8_MAX)
    {
        uint8_t *ptr = (uint8_t *)index_table;
        for (uint8_t i = 0; i < cap; i++) { ptr[i] = UINT8_MAX; }
        return ptr;
    }
    else if (cap <= UINT16_MAX)
    {
        uint16_t *ptr = (uint16_t *)index_table;
        for (uint16_t i = 0; i < cap; i++) { ptr[i] = UINT16_MAX; }
        return ptr;
    }
    else if (cap <= UINT32_MAX)
    {
        uint32_t *ptr = (uint32_t *)index_table;
        for (uint32_t i = 0; i < cap; i++) { ptr[i] = UINT32_MAX; }
        return ptr;
    }
    else
    {
        uint64_t *ptr = (uint64_t *)index_table;
        for (uint64_t i = 0; i < cap; i++) { ptr[i] = UINT64_MAX; }
        return ptr;
    }
}

static bool _dict_$tt_$vv_index_table_is_tomb(void *index_table, uint64_t cap, uint64_t index)
{
    assert(index < cap);

    if (cap <= UINT8_MAX)
    {
        uint8_t *ptr = (uint8_t *)index_table;
        return ptr[index] == TOMBSTONE8;
    }
    else if (cap <= UINT16_MAX)
    {
        uint16_t *ptr = (uint16_t *)index_table;
        return ptr[index] == TOMBSTONE16;
    }
    else if (cap <= UINT32_MAX)
    {
        uint32_t *ptr = (uint32_t *)index_table;
        return ptr[index] == TOMBSTONE32;
    }
    else
    {
        uint64_t *ptr = (uint64_t *)index_table;
        return ptr[index] == TOMBSTONE64;
    }
}

// Set the index table at index to the entry's position (index)
#define _dict_$tt_$vv_index_table_set_pos(IndexTable, Cap, Index, Pos) \
do { \
    if ((Cap) <= UINT8_MAX) { \
        uint8_t *ptr = (uint8_t *)(IndexTable); \
        ptr[(Index)] = (Pos); \
    } \
    else if ((Cap) <= UINT16_MAX) { \
        uint16_t *ptr = (uint16_t *)(IndexTable); \
        ptr[(Index)] = (Pos); \
    } \
    else if ((Cap) <= UINT32_MAX) { \
        uint32_t *ptr = (uint32_t *)(IndexTable); \
        ptr[(Index)] = (Pos); \
    } \
    else { \
        uint64_t *ptr = (uint64_t *)(IndexTable); \
        ptr[(Index)] = (Pos); \
    } \
} while (0)

// Set a tombstone at the index
#define _dict_$tt_$vv_index_table_set_tombstone(IndexTable, Cap, Index) \
do { \
    if ((Cap) <= UINT8_MAX) { \
        uint8_t *ptr = (uint8_t *)(IndexTable); \
        ptr[(Index)] = TOMBSTONE8; \
    } \
    else if ((Cap) <= UINT16_MAX) { \
        uint16_t *ptr = (uint16_t *)(IndexTable); \
        ptr[(Index)] = TOMBSTONE16; \
    } \
    else if ((Cap) <= UINT32_MAX) { \
        uint32_t *ptr = (uint32_t *)(IndexTable); \
        ptr[(Index)] = TOMBSTONE32; \
    } \
    else { \
        uint64_t *ptr = (uint64_t *)(IndexTable); \
        ptr[(Index)] = TOMBSTONE64; \
    } \
} while (0)

static uint64_t _dict_$tt_$vv_index_table_get_pos(void *index_table, uint64_t cap, uint64_t index)
{
    if (cap <= UINT8_MAX) {
        uint8_t *ptr = (uint8_t *)index_table;
        return (uint64_t)ptr[index];
    }
    else if (cap <= UINT16_MAX) {
        uint16_t *ptr = (uint16_t *)index_table;
        return (uint64_t)ptr[index];
    }
    else if (cap <= UINT32_MAX) {
        uint32_t *ptr = (uint32_t *)index_table;
        return (uint64_t)ptr[index];
    }
    else {
        uint64_t *ptr = (uint64_t *)index_table;
        return (uint64_t)ptr[index];
    }
}

// Check the Index in the IndexTable to see if it is empty or a tombstone
// If so, break out of some loop which contains this preserving Index to
// remain as an index into IndexTable and entries which can store an entry.
#define _dict_$tt_$vv_index_table_get_break_on_empty_or_tomb(IndexTable, Cap, Index) \
    if ((Cap) <= UINT8_MAX) { \
        uint8_t *ptr = (uint8_t *)(IndexTable); \
        const uint8_t LValue = ptr[(Index)]; \
        if (LValue >= TOMBSTONE8) break; \
    } \
    else if ((Cap) <= UINT16_MAX) { \
        uint16_t *ptr = (uint16_t *)(IndexTable); \
        const uint16_t LValue = ptr[(Index)]; \
        if (LValue >= TOMBSTONE16) break; \
    } \
    else if ((Cap) <= UINT32_MAX) { \
        uint32_t *ptr = (uint32_t *)(IndexTable); \
        const uint32_t LValue = ptr[(Index)]; \
        if (LValue >= TOMBSTONE32) break; \
    } \
    else { \
        uint64_t *ptr = (uint64_t *)(IndexTable); \
        const uint64_t LValue = ptr[(Index)]; \
        if (LValue >= TOMBSTONE64) break; \
    } \

#define _dict_$tt_$vv_index_table_get_break_on_empty_continue_on_tomb(IndexTable, Cap, Index) \
    if ((Cap) <= UINT8_MAX) { \
        uint8_t *ptr = (uint8_t *)(IndexTable); \
        const uint8_t LValue = ptr[(Index)]; \
        if (LValue == EMPTY8) break; \
        if (LValue == TOMBSTONE8) continue; \
    } \
    else if ((Cap) <= UINT16_MAX) { \
        uint16_t *ptr = (uint16_t *)(IndexTable); \
        const uint16_t LValue = ptr[(Index)]; \
        if (LValue == EMPTY16) break; \
        if (LValue == TOMBSTONE16) continue; \
    } \
    else if ((Cap) <= UINT32_MAX) { \
        uint32_t *ptr = (uint32_t *)(IndexTable); \
        const uint32_t LValue = ptr[(Index)]; \
        if (LValue == EMPTY32) break; \
        if (LValue == TOMBSTONE32) continue; \
    } \
    else { \
        uint64_t *ptr = (uint64_t *)(IndexTable); \
        const uint64_t LValue = ptr[(Index)]; \
        if (LValue == EMPTY64) break; \
        if (LValue == TOMBSTONE64) continue; \
    } \

static DictNode$Tt$Vv *_dict_$tt_$vv_table_lookup(Dict$Tt$Vv *self, uint64_t index)
{
    assert(self);
    assert(index < self->cap);

    if (self->cap <= UINT8_MAX)
    {
        uint8_t *ptr = (uint8_t *)self->index_table;
        const uint8_t entry_index = ptr[index];
        if (entry_index >= TOMBSTONE8) return NULL;
        return &self->entries[entry_index];
    }
    else if (self->cap <= UINT16_MAX)
    {
        uint16_t *ptr = (uint16_t *)self->index_table;
        const uint16_t entry_index = ptr[index];
        if (entry_index >= TOMBSTONE16) return NULL;
        return &self->entries[entry_index];
    }
    else if (self->cap <= UINT32_MAX)
    {
        uint32_t *ptr = (uint32_t *)self->index_table;
        const uint32_t entry_index = ptr[index];
        if (entry_index >= TOMBSTONE32) return NULL;
        return &self->entries[entry_index];
    }
    else
    {
        uint64_t *ptr = (uint64_t *)self->index_table;
        const uint64_t entry_index = ptr[index];
        if (entry_index >= TOMBSTONE64) return NULL;
        return &self->entries[entry_index];
    }
}

Dict$Tt$Vv dict_$tt_$vv_init_reserve(uint64_t cap)
{
    Dict$Tt$Vv self;
    assert(cap >= DICT_$TT_$VV_MIN_CAP);

    self.entries = (DictNode$Tt$Vv *)$MALLOC(sizeof(DictNode$Tt$Vv) * cap);
    assert(self.entries);

    self.index_table = _dict_$tt_$vv_create_index_table(cap);
    assert(self.index_table);

    self.len = 0;
    self.cap = cap;

    return self;
}

Dict$Tt$Vv *dict_$tt_$vv_new_reserve(uint64_t cap)
{
    Dict$Tt$Vv *self = (Dict$Tt$Vv *)$MALLOC(sizeof(*self));
    assert(self);

    *self = dict_$tt_$vv_init_reserve(cap);

    return self;
}

Dict$Tt$Vv dict_$tt_$vv_init(void)
{
    return dict_$tt_$vv_init_reserve(DICT_$TT_$VV_DEFAULT_SIZE);
}

Dict$Tt$Vv *dict_$tt_$vv_new(void)
{
    return dict_$tt_$vv_new_reserve(DICT_$TT_$VV_DEFAULT_SIZE);
}

void dict_$tt_$vv_deinit(Dict$Tt$Vv *self)
{
    assert(self);

    if (self->entries)
    {
        for (uint64_t i = 0; i < dict_$tt_$vv_size(self); i++) {
            DictNode$Tt$Vv *node = dict_$tt_$vv_at(self, i);
            if (!node) continue;

            if (!DEF_DICT_$TT_$VV_ZEROKEY(node->key)) {
                DEF_DICT_$TT_$VV_FREEKEY(node->key);
            }

            if (DEF_DICT_$TT_$VV_ZEROVALUE(node->value)) {
                DEF_DICT_$TT_$VV_FREEVALUE(node->value);
            }
        }
        $FREE(self->entries);
    }

    if (self->index_table)
    {
        $FREE(self->index_table);
    }
}

void dict_$tt_$vv_del(Dict$Tt$Vv *self)
{
    if (!self) return;

    dict_$tt_$vv_deinit(self);

    $FREE(self);
}

void dict_$tt_$vv_reset(Dict$Tt$Vv *self)
{
    assert(self);
    assert(self->entries);

    for (uint64_t i = 0; i < dict_$tt_$vv_size(self); i++) {
        DictNode$Tt$Vv *node = dict_$tt_$vv_at(self, i);
        if (!node) continue;

        if (!DEF_DICT_$TT_$VV_ZEROKEY(node->key)) {
            DEF_DICT_$TT_$VV_FREEKEY(node->key);
        }

        if (DEF_DICT_$TT_$VV_ZEROVALUE(node->value)) {
            DEF_DICT_$TT_$VV_FREEVALUE(node->value);
        }

    }

    _dict_$tt_$vv_reset_index_table(self->index_table, self->cap);

    self->len = 0;
}

static uint64_t _dict_$tt_$vv_hash($K key)
{
#if $OPT_KEY_IS_TRIVIALLY_HASHABLE
    static_assert(sizeof(key) <= sizeof(uint64_t), "DICT_$TT_$VV is not trivially hashable.");
    uint64_t hash[1] = {0};
    memcpy(hash, &key, sizeof(key));
    return hash[0];
#elif ! $OPT_HASH_USE_DEFAULT
    return $OPT_HASH_NON_DEFAULT_FUNC(key);
#else
    // FNV1A
    uint8_t *buf = (uint8_t *)DEF_DICT_$TT_$VV_REFKEY(key);
    const uint64_t size = DEF_DICT_$TT_$VV_SIZEOFKEY(buf);
    uint64_t hash = 0x811C9DC5;
    for (uint64_t i = 0; i < size; i++) {
        hash = (buf[i] ^ hash) * 0x01000193;
    }
    return hash;
#endif
}

// https://www.youtube.com/watch?v=p33CVV29OG8
bool dict_$tt_$vv_set(Dict$Tt$Vv *self, $K key, $U value)
{
    assert(self);

#if $OPT_KEY_IS_PTR
    assert(key);
#endif

    if (DICT_$TT_$VV_LOAD_FACTOR_READY(self->len+1, self->cap))
    {
        self->entries = (DictNode$Tt$Vv *)$REALLOC(self->entries, sizeof(DictNode$Tt$Vv) * self->cap * DICT_$TT_$VV_GROWTH_FACTOR);
        const uint64_t newcap = self->cap * DICT_$TT_$VV_GROWTH_FACTOR;

        void *new_index_table = _dict_$tt_$vv_create_index_table(newcap);
        assert(new_index_table);

        for (uint64_t pos = 0; pos < dict_$tt_$vv_size(self); pos++) {
            DictNode$Tt$Vv *entry = dict_$tt_$vv_at(self, pos);
            if (!entry) continue;

            uint64_t h, perturb;
            h = perturb = entry->hash;
            uint64_t i = h % newcap;

            while (1)
            {
                _dict_$tt_$vv_index_table_get_break_on_empty_or_tomb(new_index_table, newcap, i);
                i = (5 * i + perturb + 1) % newcap;
                perturb >>= 5;
            }
            _dict_$tt_$vv_index_table_set_pos(new_index_table, newcap, i, pos);
        }

        $FREE(self->index_table);
        self->index_table = new_index_table;
        self->cap = newcap;
    }

    const DictNode$Tt$Vv node = {
        .hash = _dict_$tt_$vv_hash(key),
        .key = key,
        .value = value,
    #if ! $OPT_KEY_IS_PTR
        .tombstone = false,
    #endif
    };

    bool earlier_i_found = false;
    uint64_t tombstone_i;

    uint64_t h, perturb;
    h = perturb = node.hash;
    uint64_t i = h % self->cap;
    while (1)
    {
        // In the index_table, a tombstoned spot can be reused, unlike the entry
        // array. So if we find a tombstone, save it so next lookup doesn't have
        // to pass tombstones to find this entry we are setting in this function.
        if (!earlier_i_found && _dict_$tt_$vv_index_table_is_tomb(self->index_table, self->cap, i))
        {
            earlier_i_found = true;
            tombstone_i = i;
            continue;
        }

        // Empty means we have a spot, tomb means we found a first tomb above but found
        // a second one here. Skip if second+ tomb.
        _dict_$tt_$vv_index_table_get_break_on_empty_continue_on_tomb(self->index_table, self->cap, i);

        // If we didn't break, then we are have an index of an entry which is alive.
        // If this alive entry has the same hash, we must override it with this new
        // value instead of finding the next spot.
        DictNode$Tt$Vv *tmpnode = _dict_$tt_$vv_table_lookup(self, i);
        if ((tmpnode->hash == node.hash) && (
            #if $OPT_KEY_IS_PTR
                tmpnode->key == node.key || // identity
            #endif
                DEF_DICT_$TT_$VV_EQUAL_KEYS(tmpnode->key, node.key)
            ))
        {
            if (DEF_DICT_$TT_$VV_ZEROVALUE(tmpnode->value)) {
                DEF_DICT_$TT_$VV_FREEVALUE(tmpnode->value);
            }

            tmpnode->value = value;

            // If we have an earlier i, then we can move this index up in the
            // index_table so it will hit sooner than going through tombed indices.
            // Note this doesn't change the entry order and we are replacing the
            // current index_table i by a tomb in case there's things after.
            if (earlier_i_found)
            {
                const uint64_t curpos = _dict_$tt_$vv_index_table_get_pos(self->index_table, self->cap, i);
                _dict_$tt_$vv_index_table_set_tombstone(self->index_table, self->cap, i);
                _dict_$tt_$vv_index_table_set_pos(self->index_table, self->cap, tombstone_i, curpos);
            }

            return false; // override existing
        }

        // calc next index to check
        i = (5 * i + perturb + 1) % self->cap;
        perturb >>= 5;
    }

    // Got out means that i is an index_table spot for this entry.
    const uint64_t pos = self->len;
    self->entries[pos] = node;
    self->len++;

    // Use the earlier index if one was found.
    if (earlier_i_found)
    {
        i = tombstone_i;
    }
    _dict_$tt_$vv_index_table_set_pos(self->index_table, self->cap, i, pos);
    return true; // brand new element
}

static DictNode$Tt$Vv *_dict_$tt_$vv_get_node(Dict$Tt$Vv *self, $K key)
{
    assert(self);
    uint64_t h, perturb;
    h = perturb = _dict_$tt_$vv_hash(key);
    uint64_t i = h % self->cap;

    while (1)
    {
        _dict_$tt_$vv_index_table_get_break_on_empty_continue_on_tomb(self->index_table, self->cap, i);

        DictNode$Tt$Vv *node = _dict_$tt_$vv_table_lookup(self, i);
        if (node && (node->hash == h))
        {
            return node;
        }

        i = (5 * i + perturb + 1) % self->cap;
        perturb >>= 5;
    }
    return NULL;
}

$U *dict_$tt_$vv_get(Dict$Tt$Vv *self, $K key)
{
    DictNode$Tt$Vv *p = _dict_$tt_$vv_get_node(self, key);
    if (!p) return NULL;

    return &p->value;
}

bool dict_$tt_$vv_contains(Dict$Tt$Vv *self, $K key)
{
    return !!dict_$tt_$vv_get(self, key);
}

// Place a tombstone in the index table to indicate this entry is dead but may
// be reused. Eliminate needing to re-hash / re-size.
// NOTE: Because the set function always puts the next node on the end (at self->len)
// we preserve insertion order traversal! However, we end up with dead spots until
// a resize occurs, at which points all dead spots are removed and all alive
// entries are re-hashed / inserted.
//
// You could theoretically break insertion traversal order by making tombed entries
// into a linked list, where the hash points to the next tombed spot, or 0/sentinel
// in which case you would append a new entry at the very end instead of reusing
// tombed spots. Oh well, let's preserve insertion order with the idea that resizing
// will result in a compacted array, and this will be sufficiently often enough.
void dict_$tt_$vv_remove(Dict$Tt$Vv *self, $K key)
{
    assert(self);
    uint64_t h, perturb;
    h = perturb = _dict_$tt_$vv_hash(key);
    uint64_t i = h % self->cap;

    while (1)
    {
        _dict_$tt_$vv_index_table_get_break_on_empty_continue_on_tomb(self->index_table, self->cap, i);

        DictNode$Tt$Vv *node = _dict_$tt_$vv_table_lookup(self, i);
        if (node && (node->hash == h))
        {
            if (!DEF_DICT_$TT_$VV_ZEROKEY(node->key)) {
                DEF_DICT_$TT_$VV_FREEKEY(node->key);
            }

            if (DEF_DICT_$TT_$VV_ZEROVALUE(node->value)) {
                DEF_DICT_$TT_$VV_FREEVALUE(node->value);
            }

        #if $OPT_KEY_IS_PTR
            node->key = NULL;
        #else
            node->tombstone = true;
        #endif

            _dict_$tt_$vv_index_table_set_tombstone(self->index_table, self->cap, i);
            break;
        }

        i = (5 * i + perturb + 1) % self->cap;
        perturb >>= 5;
    }
}

DictNode$Tt$Vv *dict_$tt_$vv_at(Dict$Tt$Vv *self, uint64_t i)
{
    assert(self);
    assert(i < self->len);

#if $OPT_KEY_IS_PTR
    if (!self->entries[i].key) return NULL;
#else
    if (self->entries[i].tombstone) return NULL;
#endif
    return &self->entries[i];
}

uint64_t dict_$tt_$vv_size(Dict$Tt$Vv *self)
{
    assert(self);
    return self->len;
}

DictNode$Tt$Vv *dict_$tt_$vv_iter(Dict$Tt$Vv *self)
{
    assert(self);

    const DictNode$Tt$Vv *const entries = self->entries;
    DictNode$Tt$Vv *node = self->entries;
    for ( ; (size_t)(node - entries) < self->len; node++)
    {
#if $OPT_KEY_IS_PTR
        if (node->key) return node;
#else
        if (!node->tombstone) return node;
#endif
    }
    return NULL;
}

DictNode$Tt$Vv *dict_$tt_$vv_next(Dict$Tt$Vv *self, DictNode$Tt$Vv *node)
{
    assert(self);
    assert(node);
    assert(node >= self->entries);
    assert(node < &self->entries[self->len]);

    node += 1;

    const DictNode$Tt$Vv *const entries = self->entries;
    for ( ; ((size_t)(node - entries) < self->len); node++)
    {
#if $OPT_KEY_IS_PTR
        if (node->key) return node;
#else
        if (!node->tombstone) return node;
#endif
    }
    return NULL;
}
