/* 
 * based on code from wikipedia
 * Creative Commons Attribution-ShareAlike License
 */
#include <utils/sort.h>

int comp_int(void *a, void *b) {
    int ai = (int)a;
    int bi = (int)b;

    if(ai < bi) {
        return -1;
    } else if(ai > bi) {
        return 1;
    } else {
        return 0;
    }
}

int comp_uint(void *a, void *b) {
    unsigned int ai = (unsigned int)a;
    unsigned int bi = (unsigned int)b;

    if(ai < bi) {
        return -1;
    } else if(ai > bi) {
        return 1;
    } else {
        return 0;
    }
}

int comp_ulong(void *a, void *b) {
    unsigned long ai = (unsigned long)a;
    unsigned long bi = (unsigned long)b;

    if(ai < bi) {
        return -1;
    } else if(ai > bi) {
        return 1;
    } else {
        return 0;
    }
}

void swap(void ** items, size_t n1, size_t n2) {
    void * temp = items[n1];
    items[n1] = items[n2];
    items[n2] = temp;
}

void bubble_sort(void ** items, size_t length, comp_fn comp) {
    size_t n = length;
    do {
        size_t newn = 0;
        for(int i = 1; i < n; i++) {
            int comp_n = comp(items[i-1], items[i]);
            if (comp_n > 0) {
                swap(items, i-1, i);
                newn = i;
            }
        }
        n = newn;
    } while (n);
}

void insertion_sort(void ** items, size_t length, comp_fn comp) {
    for(size_t i = 1; i < length; i++) {
        void * x = items[i];
        size_t j = i;
        int comp_n = comp(items[j-1], x);
        while (j && comp_n > 0) {
            items[j] = items[j-1];
            j = j - 1;
            items[j] = x;
        }
    }
}
