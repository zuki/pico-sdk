/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_UTIL_PHEAP_H
#define _PICO_UTIL_PHEAP_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_PHEAP, Enable/disable assertions in the pheap module, type=bool, default=0, group=pico_util
#ifndef PARAM_ASSERTIONS_ENABLED_PHEAP
#define PARAM_ASSERTIONS_ENABLED_PHEAP 0
#endif

/**
 * \file pheap.h
 * \defgroup util_pheap pheap
 * \ingroup pico_util
 * \brief ペアリングヒープの実装.
 *
 * pheapはシンプルなペアリングヒープを定義します。この実装は単に配列の
 * インデックスを追跡するだけであり、ヒープエントリのためのストレージと
 * 比較関数を提供するのはユーザにまかされています。
 *
 * **注意:** このクラスはコンカレントな使用では安全でありません。外部で
 * 保護する必要があります。さらに、コンカレントに使用する場合、呼び出し元は
 * 返されたIDの使用について保護する必要があります。たとえば、ph_remove_and_free_head は
 * もはやヒープにない要素のidを返します。ユーザはこのIDをコンパニオン配列から
 * データを探すのに使用できますが、そのIDはその後の操作で再利用されている可能性が
 * あるので、ヒープに対してなにか操作を行うとそのデータを上書きしてしまう可能性が
 * あります。
 *
 */
// PICO_CONFIG: PICO_PHEAP_MAX_ENTRIES, Maximum number of entries in the pheap, min=1, max=65534, default=255, group=pico_util
#ifndef PICO_PHEAP_MAX_ENTRIES
#define PICO_PHEAP_MAX_ENTRIES 255
#endif

// public heap_node ids are numbered from 1 (0 means none)
#if PICO_PHEAP_MAX_ENTRIES < 256
typedef uint8_t pheap_node_id_t;
#elif PICO_PHEAP_MAX_ENTRIES < 65535
typedef uint16_t pheap_node_id_t;
#else
#error invalid PICO_PHEAP_MAX_ENTRIES
#endif

typedef struct pheap_node {
    pheap_node_id_t child, sibling, parent;
} pheap_node_t;

/**
 * \ingroup util_pheap
 *
 * \brief ペアリングヒープに必要なユーザ提供の比較関数.
 *
 * \return 自然な順序で a < b の場合、true. この相対順序はすべての
 * 呼び出しで同じでなければならない。
 */
typedef bool (*pheap_comparator)(void *user_data, pheap_node_id_t a, pheap_node_id_t b);

typedef struct pheap {
    pheap_node_t *nodes;
    pheap_comparator comparator;
    void *user_data;
    pheap_node_id_t max_nodes;
    pheap_node_id_t root_id;
    // we remove from head and add to tail to stop reusing the same ids
    pheap_node_id_t free_head_id;
    pheap_node_id_t free_tail_id;
} pheap_t;

/**
 * \ingroup util_pheap
 *
 * \brief ノードの効率的なソート順序を維持するペアリングヒープを作成する.
 *
 * ヒープ自身はユーザのノードごとの状態を保存しないので、コンパニオン配列は
 * ユーザが保持することが期待されています。ヒープの実装がノードの相対順序を
 * 決定できるように比較関数を提供する必要があります。
 *
 * \param max_nodes ヒープに存在するノードの最大数（これはPICO_PHEAP_MAX_ENTRIESに
 *  より制限されており、1バイトでインデックスを格納できるようにそのデフォルトは255です）。
 * \param comparator ノード比較関数
 * \param user_data コールバック関数で提供されているヒープに関連するユーザデータへのポインタ
 * \return 新規に割り当てられ初期化されたヒープ
 */
pheap_t *ph_create(uint max_nodes, pheap_comparator comparator, void *user_data);

/**
 * \ingroup util_pheap
 *
 * \brief ペアリングヒープからすべてのノードを削除する.
 * \param heap ヒープ
 */
void ph_clear(pheap_t *heap);

/**
 * \ingroup util_pheap
 *
 * \brief ペアリングヒープの割り当てを破棄する.
 *
 * この関数はph_create()で作成したヒープ以外には使用できないことに注意してください。
 *
 * \param heap ヒープ
 */
void ph_destroy(pheap_t *heap);

// internal method
static inline pheap_node_t *ph_get_node(pheap_t *heap, pheap_node_id_t id) {
    assert(id && id <= heap->max_nodes);
    return heap->nodes + id - 1;
}

// internal method
static void ph_add_child_node(pheap_t *heap, pheap_node_id_t parent_id, pheap_node_id_t child_id) {
    pheap_node_t *n = ph_get_node(heap, parent_id);
    assert(parent_id);
    assert(child_id);
    assert(parent_id != child_id);
    pheap_node_t *c = ph_get_node(heap, child_id);
    c->parent = parent_id;
    if (!n->child) {
        n->child = child_id;
    } else {
        c->sibling = n->child;
        n->child = child_id;
    }
}

// internal method
static pheap_node_id_t ph_merge_nodes(pheap_t *heap, pheap_node_id_t a, pheap_node_id_t b) {
    if (!a) return b;
    if (!b) return a;
    if (heap->comparator(heap->user_data, a, b)) {
        ph_add_child_node(heap, a, b);
        return a;
    } else {
        ph_add_child_node(heap, b, a);
        return b;
    }
}

/**
 * \ingroup util_pheap
 *
 * \brief ヒープの未使用空間から新規ノードを割り当てる.
 *
 * \param heap ヒープ
 * \return ノードの識別子、ヒープに空きがない場合は0
 */
static inline pheap_node_id_t ph_new_node(pheap_t *heap) {
    if (!heap->free_head_id) return 0;
    pheap_node_id_t id = heap->free_head_id;
    pheap_node_t *hn = ph_get_node(heap, id);
    heap->free_head_id = hn->sibling;
    if (!heap->free_head_id) heap->free_tail_id = 0;
    hn->child = hn->sibling = hn->parent = 0;
    return id;
}

/**
 * \ingroup util_pheap
 *
 * \brief ヒープにノードを挿入する.
 *
 * この関数はヒープに（事前にph_new_node()で割り当てた）ノードを挿入します。
 * 挿入位置はヒープの比較関数を呼び出して決定します。
 *
 * \param heap ヒープ
 * \param id 挿入するノードのID
 * \return ペアリングヒープの新たなヘッド（すなわち、最初に比較するノード）のID
 */
static inline pheap_node_id_t ph_insert_node(pheap_t *heap, pheap_node_id_t id) {
    assert(id);
    pheap_node_t *hn = ph_get_node(heap, id);
    hn->child = hn->sibling = hn->parent = 0;
    heap->root_id = ph_merge_nodes(heap, heap->root_id, id);
    return heap->root_id;
}

/**
 * \ingroup util_pheap
 *
 * \brief ペアリングヒープのヘッドノード（すなわち、最初に比較するノード）を返す.
 * ただし、そのノードはヒープから削除しない。
 *
 * \param heap ヒープ
 * \return 現在のヘッドノードのID
 */
static inline pheap_node_id_t ph_peek_head(pheap_t *heap) {
    return heap->root_id;
}

/**
 * \ingroup util_pheap
 *
 * Remove the head node from the pairing heap. This head node is
 * the node which compares first in the logical ordering provided
 * by the comparator.
 *
 * Note that in the case of free == true, the returned id is no longer
 * allocated and may be re-used by future node allocations, so the caller
 * should retrieve any per node state from the companion array before modifying
 * the heap further.
 *
 * @param heap the heap
 * @param free true if the id is also to be freed; false if not - useful if the caller
 *        may wish to re-insert an item with the same id)
 * @return the old head node id.
 */
pheap_node_id_t ph_remove_head(pheap_t *heap, bool free);

/**
 * \ingroup util_pheap
 * Remove the head node from the pairing heap. This head node is
 * the node which compares first in the logical ordering provided
 * by the comparator.
 *
 * Note that the returned id will be freed, and thus may be re-used by future node allocations,
 * so the caller should retrieve any per node state from the companion array before modifying
 * the heap further.
 *
 * @param heap the heap
 * @return the old head node id.
 */
static inline pheap_node_id_t ph_remove_and_free_head(pheap_t *heap) {
    return ph_remove_head(heap, true);
}

/**
 * \ingroup util_pheap
 * Remove and free an arbitrary node from the pairing heap. This is a more
 * costly operation than removing the head via ph_remove_and_free_head()
 *
 * @param heap the heap
 * @param id the id of the node to free
 * @return true if the the node was in the heap, false otherwise
 */
bool ph_remove_and_free_node(pheap_t *heap, pheap_node_id_t id);

/**
 * \ingroup util_pheap
 * Determine if the heap contains a given node. Note containment refers
 * to whether the node is inserted (ph_insert_node()) vs allocated (ph_new_node())
 *
 * @param heap the heap
 * @param id the id of the node
 * @return true if the heap contains a node with the given id, false otherwise.
 */
static inline bool ph_contains_node(pheap_t *heap, pheap_node_id_t id) {
    return id == heap->root_id || ph_get_node(heap, id)->parent;
}


/**
 * \ingroup util_pheap
 * Free a node that is not currently in the heap, but has been allocated
 *
 * @param heap the heap
 * @param id the id of the node
 */
static inline void ph_free_node(pheap_t *heap, pheap_node_id_t id) {
    assert(id && !ph_contains_node(heap, id));
    if (heap->free_tail_id) {
        ph_get_node(heap, heap->free_tail_id)->sibling = id;
    }
    if (!heap->free_head_id) {
        assert(!heap->free_tail_id);
        heap->free_head_id = id;
    }
    heap->free_tail_id = id;
}

/**
 * \ingroup util_pheap
 * Print a representation of the heap for debugging
 *
 * @param heap the heap
 * @param dump_key a method to print a node value
 * @param user_data the user data to pass to the dump_key method
 */
void ph_dump(pheap_t *heap, void (*dump_key)(pheap_node_id_t id, void *user_data), void *user_data);

/**
 * \ingroup util_pheap
 * Initialize a statically allocated heap (ph_create() using the C heap).
 * The heap member `nodes` must be allocated of size max_nodes.
 *
 * @param heap the heap
 * @param max_nodes the max number of nodes in the heap (matching the size of the heap's nodes array)
 * @param comparator the comparator for the heap
 * @param user_data the user data for the heap.
 */
void ph_post_alloc_init(pheap_t *heap, uint max_nodes, pheap_comparator comparator, void *user_data);

/**
 * \ingroup util_pheap
 * Define a statically allocated pairing heap. This must be initialized
 * by ph_post_alloc_init
 */
#define PHEAP_DEFINE_STATIC(name, _max_nodes) \
    static_assert(_max_nodes && _max_nodes < (1u << (8 * sizeof(pheap_node_id_t))), ""); \
    static pheap_node_t name ## _nodes[_max_nodes]; \
    static pheap_t name = { \
            .nodes = name ## _nodes, \
            .max_nodes = _max_nodes \
    };


#ifdef __cplusplus
}
#endif

#endif
