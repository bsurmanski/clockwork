/**
 * voctree.c
 * obj
 * October 03, 2012
 * Brandon Surmanski
 *
 * Sparse Voxel Octree
 */

#include <stdint.h>

#define FAR_PTR 0x8000


struct svo_discriptor //child discriptor
{
    uint16_t child_ptr;
    uint8_t valid_mask;
    uint8_t leaf_mask;

    uint8_t contour_ptr[3];
    uint8_t contour_mask;
};

struct svo_info
{

};

struct contour
{

};

struct svo_block
{
    struct svo_info info;
    struct svo_discriptor nodes[]; 
    struct contour contour;
    //TODO: optional attachments
};

void voctree_init(Voctree *v, int x, int y, int z)
{

}

void voctree_finalize(Voctree *v)
{

}
