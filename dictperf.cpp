
#include "data_structures.h"
#include <unordered_map>
#include <string>
#include <time.h>
#include <string.h>
#include <stdlib.h>

static double now()
{
    struct timespec ts = {0};
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static char *str(const char *s)
{
    return strdup(s);
}

int main()
{
    double start, end;
    size_t elements;

    DictIntInt c_dict = dict_int_int_init();
    std::unordered_map<int, int> cpp_dict;

    elements = 100000000;
    start = now();
    for (volatile size_t i = 0; i < elements; i++)
    {
        dict_int_int_set(&c_dict, (int)i, (int)i);
    }
    end = now();
    printf("c_dict elements (%zu) %lf seconds (%lf elements/s)\n",
        elements, end - start, (double)elements/(double)(end-start));

    start = now();
    for (volatile size_t i = 0; i < elements; i++)
    {
        cpp_dict[(int)i] = (int)i;
    }
    end = now();
    printf("cpp_dict elements (%zu) %lf seconds (%lf elements/s)\n",
        elements, end - start, (double)elements/(double)(end-start));



    volatile size_t count = 0;
    start = now();
    DICT_INT_INT_ITER(&c_dict, it, {
        count++;
    });
    end = now();
    printf("c_dict traverse (%zu) %lf seconds (%lf elements/s)\n",
        elements, end - start, (double)elements/(double)(end-start));

    count = 0;
    start = now();
    for (DictNodeIntInt *it = dict_int_int_iter(&c_dict);
         it;
         it = dict_int_int_next(&c_dict, it))
    {
        count++;
    }
    end = now();
    printf("c_dict fast traverse (%zu) %lf seconds (%lf elements/s)\n",
        elements, end - start, (double)elements/(double)(end-start));

    count = 0;
    start = now();
    for (auto& kv : cpp_dict)
    {
        count++;
    }
    end = now();
    printf("cpp_dict traverse (%zu) %lf seconds (%lf elements/s)\n",
        elements, end - start, (double)elements/(double)(end-start));


    //////////////////////////////////////////////////////////////////

    DictStringInt cs_dict = dict_string_int_init();
    std::unordered_map<std::string, int> cpps_dict;
    char buf[1024];

    elements = 100000;
    start = now();
    for (volatile size_t i = 0; i < elements; i++)
    {
        snprintf(buf, sizeof(buf), "%zu ------------------------------------------ %zu", i, i);
        dict_string_int_set(&cs_dict, str(buf), (int)i);
    }
    end = now();
    printf("cs_dict traverse (%zu) %lf seconds (%lf elements/s)\n",
        elements, end - start, (double)elements/(double)(end-start));

    start = now();
    for (volatile size_t i = 0; i < elements; i++)
    {
        snprintf(buf, sizeof(buf), "%zu ------------------------------------------ %zu", i, i);
        cpps_dict[std::string(buf)] = (int)i;
    }
    end = now();
    printf("cpps_dict traverse (%zu) %lf seconds (%lf elements/s)\n",
        elements, end - start, (double)elements/(double)(end-start));

    return 0;
}
