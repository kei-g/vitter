#ifndef __include_vitter_h__
#define __include_vitter_h__

#include "bitstream.h"

#include <stdbool.h>

typedef struct _vitter_node vitter_node_t;
typedef struct _vitter_tree vitter_tree_t;
typedef struct _vitter vitter_t;

bool vitter_alloc(vitter_t *v, uint16_t size);
void vitter_dealloc(vitter_t *v);
bool vitter_decode(uint16_t *symbol, vitter_t *v);
bool vitter_encode(vitter_t *v, uint16_t symbol);

#ifdef USE_VITTER_SCALE
void vitter_scale(vitter_tree_t *tree, uint16_t bits);
#endif /* USE_VITTER_SCALE */

struct _vitter_node {
	uint16_t child;
	uint16_t parent;
	uint16_t symbol;
	uint16_t weight;
};

struct _vitter_tree {
	uint16_t escape;
	uint16_t *map;
	vitter_node_t *nodes;
	uint16_t root;
	uint16_t size;
};

struct _vitter {
	bitstream_t stream;
	vitter_tree_t tree;
};

#endif /* __include_vitter_h__ */
