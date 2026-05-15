#include <stdlib.h>
#include "data_structures.h"

#include <unordered_map>


static double now()
{
    struct timespec ts = {0};
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static void insert(DictLongLong *d, long from, long to)
{
    for (long i = from; i < to; i++)
    {
        //printf("Inserting %ld\n", i);
        dict_long_long_set(d, i, i);
    }
}

static void cpp_insert(std::unordered_map<long, long>& cpp, long from, long to)
{
    for (long i = from; i < to; i++)
    {
        cpp[i] = i;
    }
}

static void remove_random(DictLongLong *d, size_t count)
{
    long r;
    for (size_t i = 0; (i < count) && (d->len > 0); i++)
    {
        r = rand();
        r = r % d->len;
        //printf("Removing %ld (%zu / %zu)\n", r, i, count);
        dict_long_long_remove(d, r);
    }
}

static void cpp_remove_random(std::unordered_map<long, long>& cpp, size_t count)
{
    long r;
    for (size_t i = 0; (i < count) && (cpp.size() > 0); i++)
    {
        r = rand();
        r = r % cpp.size();
        cpp.erase(r);
    }
}

static void remove_range(DictLongLong *d, long from, long to)
{
    for (long i = from; i < to; i++)
    {
        dict_long_long_remove(d, i);
    }
}

int main()
{
    double start, end;
    DictLongLong *builder = dict_long_long_new();

    printf("Removing on empty\n");
    dict_long_long_remove(builder, 99);

    printf("Adding/removing range\n");
    insert(builder, 10, 1e5);
    remove_range(builder, 10, 1e5);

    start = now();
    printf("Adding lots\n");
    insert(builder, 10, 1e5);
    srand(0);
    printf("Removing random\n");
    remove_random(builder, 5e4);
    insert(builder, 1e5, 2e5);
    end = now();

    dict_long_long_del(builder);
    printf("c_dict %lf seconds\n", end - start);


    std::unordered_map<long, long> cpp;
    start = now();
    cpp_insert(cpp, 10, 1e5);
    srand(0);
    cpp_remove_random(cpp, 5e4);
    cpp_insert(cpp, 1e5, 2e5);
    end = now();
    printf("cpp_dict %lf seconds\n", end - start);

    return 0;
}
