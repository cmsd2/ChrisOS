#ifndef KERNEL_MM_RANGE_H
#define KERNEL_MM_RANGE_H

/*
macros for working with memory ranges
and blocks of memory

general rules:
map has a linked list of regions called regions
region has a pointer to its parent map called map
linked list sibling pointer in region is called next
you must provide the following helper functions:
region_type * name##_alloc(void) // allocates a region struct
void name##_free(region_type * r) // frees a region struct
bool name##_pre_merge(region_type * a, region_type * b) // return true if merge is valid
void name##_post_merge(region_type * a, region_type * b) // move state from b to a after merging
void name##_post_split(region_type * a, region_type * b) // move state from a to b, and remove state in the gap between a and b (one or both may have zero size, and end up empty)

todo: add pre_merge (and pre_split?) to allow testing if merging/splitting is allowed e.g. check adjacent regions are compatible
*/

#include <stdbool.h>
#include <standard.h>
#include <assert.h>

// exclusive end point functions
#define END_POINT_LT_END_POINT(p_start, p_size, r_start, r_size) ((p_size) <= ((r_size) - ((p_start) - (r_start))))
#define END_POINT_GT_START_POINT(p_start, p_size, r_start, r_size) (((p_start) >= (r_start)) || ((p_size) >= ((r_start) - (p_start))))
#define START_POINT_IN_RANGE(p, start, size) ((p) >= (start) && ((p) - (start)) < (size))
#define END_POINT_IN_RANGE(p_start, p_size, r_start, r_size) (END_POINT_GT_START_POINT(p_start, p_size, r_start, r_size) && END_POINT_LT_END_POINT(p_start, p_size, r_start, r_size))
// inclusive endpoints functions
#define POINT_IN_RANGE(p, start, end) ((p) >= (start) && (p) <= (end))

// ranges r1 and r2 must be L-Values
#define MR_SORT_RANGE(region_type, address_property, r1, r2) do { \
    region_type * _sort_ranges_temp_region; \
    if(r1->address_property > r2->address_property) { \
        _sort_ranges_temp_region = r1; \
        r1 = r2; \
        r2 = _sort_ranges_temp_region; \
    } \
} while(0)

#define MR_OVERLAP_PROTO(name, region_type, size_type) \
bool name##_overlap(region_type *a, region_type *b, size_type *result)

#define MR_OVERLAP_IMPL2(name, region_type, size_type, address_property, size_property) \
MR_OVERLAP_PROTO(name, region_type, size_type) { \
    if(b->address_property >= a->address_property) { \
        *result = MIN(b->size_property, a->size_property - (b->address_property - a->address_property)); \
        return (b->address_property - a->address_property) <= a->size_property; \
    } else { \
        *result = MIN(a->size_property, b->size_property - (a->address_property - b->address_property)); \
        return (a->address_property - b->address_property) <= b->size_property; \
    } \
}

#define MR_OVERLAP_IMPL(name, type, size_type) MR_OVERLAP_IMPL2(name, type, size_type, address, size)

#define MRL_INSERT_PROTO(name, map_type, region_type) \
void name##_insert(map_type *map, region_type *elem)

#define MRL_INSERT_IMPL2(name, map_type, region_type, regions_list_property, address_property, size_property) \
MRL_INSERT_PROTO(name, map_type, region_type) { \
    region_type * cur_region; \
\
    LL_FOREACH(map->regions_list_property, cur_region) { \
        if(elem->address_property < cur_region->address_property) { \
            LL_PREPEND_ELEM(map->regions_list_property, cur_region, elem); \
            return; \
        } \
    } \
\
    LL_APPEND(map->regions_list_property, elem); \
}

#define MRL_INSERT_IMPL(name, map_type, region_type) \
MRL_INSERT_IMPL2(name, map_type, region_type, regions, address, size)

#define MR_MERGE_PROTO(name, region_type) \
bool name##_merge(region_type *a, region_type *b)

/*
_merge(a, b)
unions a and b if a and b are adjacent or overlap
returns true if a and b are adjacent or overlap, and a will be updated to equal the union of a and b
returns false otherwise, and neither a nor b are modified
*/
#define MR_MERGE_IMPL2(name, region_type, address_property, size_property) \
MR_MERGE_PROTO(name, region_type) { \
    bool ret; \
    region_type output; \
    region_type *out_param = a; \
    if(!name##_pre_merge(a, b)) { \
        return false; \
    } \
    MR_SORT_RANGE(region_type, address_property, a, b); \
    /* a's start address is now <= b's start address */ \
    if(START_POINT_IN_RANGE(b->address_property, a->address_property, a->size_property) && END_POINT_IN_RANGE(b->address_property, b->size_property, a->address_property, a->size_property)) { \
        /* b lies completely within a */ \
        output.address_property = a->address_property; \
        output.size_property = a->size_property; \
        ret = true; \
    } else if(START_POINT_IN_RANGE(b->address_property, a->address_property, a->size_property) && END_POINT_IN_RANGE(a->address_property, a->size_property, b->address_property, b->size_property)) { \
        /* a and b overlap */ \
        output.address_property = a->address_property; \
        output.size_property = b->size_property + (b->address_property - a->address_property); \
        ret = true; \
    } else if((b->address_property - a->address_property) == a->size_property) { \
        /* a and b are adjacent */ \
        output.address_property = a->address_property; \
        output.size_property = a->size_property + b->size_property; \
        ret = true; \
    } else { \
        /* no overlap. do nothing */ \
        ret = false; \
    } \
    if(ret) { \
        out_param->address_property = output.address_property; \
        out_param->size_property = output.size_property; \
    } \
    return ret; \
}

#define MR_MERGE_IMPL(name, region_type) \
MR_MERGE_IMPL2(name, region_type, address, size)

/*
_cut(a, b)
subtracts b from a
if b overlaps with a, returns true, and both a and b modified to show the 0, 1 or 2 pieces remaining after subtracting b from a
otherwise returns false, and neither a nor b are modified
*/
#define MR_CUT_PROTO(name, region_type) \
bool name##_cut(region_type *a, region_type *b)

#define MR_CUT_IMPL2(name, region_type, address_property, size_property) \
MR_CUT_PROTO(name, region_type) { \
    bool ret; \
    if(START_POINT_IN_RANGE(b->address_property, a->address_property, a->size_property)) { \
        if(END_POINT_IN_RANGE(b->address_property, b->size_property, a->address_property, a->size_property)) { \
            /* b is completely within a */ \
            region_type first, second; \
            first.address_property = a->address_property; \
            first.size_property = (b->address_property - a->address_property); \
            second.address_property = b->address_property + b->size_property; \
            second.size_property = a->size_property - (second.address_property - a->address_property); \
            a->address_property = first.address_property; \
            a->size_property = first.size_property; \
            b->address_property = second.address_property; \
            b->size_property = second.size_property; \
            ret = true; \
        } else { \
            /* b overlaps on the right hand side */ \
            region_type temp; \
            temp.address_property = a->address_property; \
            temp.size_property = b->address_property - a->address_property; \
            a->address_property = temp.address_property; \
            a->size_property = temp.size_property; \
            b->size_property = 0; \
            ret = true; \
        } \
    } else if(b->address_property < a->address_property) { \
        if(END_POINT_IN_RANGE(b->address_property, b->size_property, a->address_property, a->size_property)) { \
            /* b overlaps on the left hand side */ \
            region_type temp; \
            temp.address_property = b->address_property + b->size_property; \
            temp.size_property = a->size_property - (b->size_property - (a->address_property - b->address_property)); \
            a->address_property = temp.address_property; \
            a->size_property = temp.size_property; \
            b->size_property = 0; \
            ret = true; \
        } else if(START_POINT_IN_RANGE(a->address_property, b->address_property, b->size_property)) { \
            /* a lies entirely within b */ \
            a->size_property = 0; \
            b->size_property = 0; \
            ret = true; \
        } else { \
            /* no overlap */ \
            ret = false; \
        } \
    } else { \
        /* no overlap */ \
        ret = false; \
    } \
    return ret; \
}

#define MR_CUT_IMPL(name, region_type) \
MR_CUT_IMPL2(name, region_type, address, size)

#define MR_CUT_AND_STITCH_PROTO(name, map_type, region_type) \
void name##_cut_and_stitch(map_type * map, region_type * region, region_type * hole)

#define MR_CUT_AND_STITCH_IMPL(name, map_type, region_type) \
MR_CUT_AND_STITCH_IMPL2(name, map_type, region_type, regions, address, size)

#define MR_CUT_AND_STITCH_IMPL2(name, map_type, region_type, regions_list_property, address_property, size_property) \
MR_CUT_AND_STITCH_PROTO(name, map_type, region_type) { \
    bool overlap; \
    region_type * new_piece; \
    region_type tmp = *hole; \
    overlap = name##_cut(region, &tmp); \
    if(overlap) { \
        if(tmp.size_property) { \
            new_piece = name##_alloc(); \
            *new_piece = *region; \
            new_piece->address_property = tmp.address_property; \
            new_piece->size_property = tmp.size_property; \
            name##_insert(map, new_piece); \
        } \
        \
        if(!region->size_property) { \
            LL_DELETE(map->regions_list_property, region); \
            name##_free(region); \
        } \
    } \
}

/*
removes a region from the list, cutting existing regions into pieces if necessary.
if any regions are removed, they will be freed using name##_free
*/
#define MRL_SUBTRACT_PROTO(name, map_type, region_type) \
void name##_subtract(map_type *map, region_type *region)

#define MRL_SUBTRACT_IMPL2(name, map_type, region_type, regions_list_property, map_property, address_property, size_property) \
MRL_SUBTRACT_PROTO(name, map_type, region_type) { \
    region_type *current_region, *tmp; \
\
    LL_FOREACH_SAFE(map->regions_list_property, current_region, tmp) { \
        name##_cut_and_stitch(map, current_region, region); \
    } \
}

#define MRL_SUBTRACT_IMPL(name, map_type, region_type) \
MRL_SUBTRACT_IMPL2(name, map_type, region_type, regions, map, address, size)

/*
adds an element to the list, merging with existing elements if necessary.
if items are removed as a result of merging, they will be freed using
name##_free
the region passed in as a parameter is also liable to be freed if it's merged in
*/
#define MRL_ADD_PROTO(name, map_type, region_type) \
void name##_add(map_type *map, region_type *region)

#define MRL_ADD_IMPL2(name, map_type, region_type, regions_list_property, map_property, address_property, size_property) \
MRL_ADD_PROTO(name, map_type, region_type) { \
    region_type *cur_region, *tmp; \
    bool merged; \
    bool added = false; \
\
    LL_FOREACH_SAFE(map->regions_list_property, cur_region, tmp) { \
        merged = name##_merge(cur_region, region); \
\
        if(merged) { \
            name##_free(region); \
            added = true; \
            region = cur_region; \
            if(cur_region->next) { \
                merged = name##_merge(cur_region->next, cur_region); \
                if(merged) { \
                    LL_DELETE(map->regions_list_property, cur_region); \
                    name##_free(cur_region); \
                } \
            } \
            break; \
        } else if(region->address_property < cur_region->address_property) { \
            added = true; \
            LL_PREPEND_ELEM(map->regions_list_property, cur_region, region); \
            break; \
        } \
    } \
\
    if(!added) { \
        LL_APPEND(map->regions_list_property, region); \
    } \
}

#define MRL_ADD_IMPL(name, map_type, region_type) \
MRL_ADD_IMPL2(name, map_type, region_type, regions, map, address, size)

#define MRL_PROTOS(name, map_type, region_type) \
MRL_INSERT_PROTO(name, map_type, region_type); \
MR_MERGE_PROTO(name, region_type); \
MRL_ADD_PROTO(name, map_type, region_type); \
MR_CUT_PROTO(name, region_type); \
MR_CUT_AND_STITCH_PROTO(name, map_type, region_type); \
MRL_SUBTRACT_PROTO(name, map_type, region_type)

#define MRL_IMPLS(name, map_type, region_type) \
MRL_INSERT_IMPL(name, map_type, region_type) \
MR_MERGE_IMPL(name, region_type) \
MRL_ADD_IMPL(name, map_type, region_type) \
MR_CUT_IMPL(name, region_type) \
MR_CUT_AND_STITCH_IMPL(name, map_type, region_type) \
MRL_SUBTRACT_IMPL(name, map_type, region_type)

#endif
