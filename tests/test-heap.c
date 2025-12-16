#include "xylem.h"

typedef struct {
    int               key;
    int               value;
    xylem_heap_node_t heap_node;
} MyStruct;

static int my_cmp(const xylem_heap_node_t* a, const xylem_heap_node_t* b) {
    MyStruct* x = xylem_heap_entry(a, MyStruct, heap_node);
    MyStruct* y = xylem_heap_entry(b, MyStruct, heap_node);
    return (x->key > y->key) - (x->key < y->key);
}

static void test_heap_init() {
    xylem_heap_t heap;
    assert(xylem_heap_init(NULL, my_cmp) == XYLEM_HEAP_ERR);
    assert(xylem_heap_init(&heap, NULL) == XYLEM_HEAP_ERR);
    assert(xylem_heap_init(&heap, my_cmp) == XYLEM_HEAP_OK);
    assert(heap.root == NULL);
    assert(heap.nelts == 0);
    assert(heap.cmp == my_cmp);
}

static void test_heap_empty() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);
    assert(xylem_heap_empty(&heap) == true);
    assert(xylem_heap_empty(NULL) == true);

    MyStruct node = {5, 10, {0}};
    xylem_heap_insert(&heap, &node.heap_node);
    assert(xylem_heap_empty(&heap) == false);

    xylem_heap_remove(&heap, &node.heap_node);
    assert(xylem_heap_empty(&heap) == true);
}

static void test_heap_insert() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    assert(xylem_heap_insert(NULL, NULL) == XYLEM_HEAP_ERR);
    assert(xylem_heap_insert(&heap, NULL) == XYLEM_HEAP_ERR);
    assert(xylem_heap_insert(NULL, (xylem_heap_node_t*)1) == XYLEM_HEAP_ERR);

    MyStruct nodes[10];
    for (int i = 0; i < 10; i++) {
        nodes[i].key = i * 2;
        nodes[i].value = i;
        assert(xylem_heap_insert(&heap, &nodes[i].heap_node) == XYLEM_HEAP_OK);
        assert(heap.nelts == (size_t)(i + 1));
    }
    MyStruct* root_entry = xylem_heap_entry(heap.root, MyStruct, heap_node);
    assert(root_entry->key == 0);

    xylem_heap_t heap2;
    xylem_heap_init(&heap2, my_cmp);
    MyStruct nodes2[10];
    for (int i = 9; i >= 0; i--) {
        nodes2[i].key = i;
        nodes2[i].value = i * 10;
        xylem_heap_insert(&heap2, &nodes2[i].heap_node);
    }
    root_entry = xylem_heap_entry(heap2.root, MyStruct, heap_node);
    assert(root_entry->key == 0);
}

static void test_heap_root() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    assert(xylem_heap_root(NULL) == NULL);
    assert(xylem_heap_root(&heap) == NULL);

    MyStruct node1 = {5, 10, {0}};
    xylem_heap_insert(&heap, &node1.heap_node);
    assert(xylem_heap_root(&heap) == &node1.heap_node);

    MyStruct node2 = {3, 20, {0}};
    xylem_heap_insert(&heap, &node2.heap_node);
    assert(xylem_heap_root(&heap) == &node2.heap_node);

    MyStruct node3 = {1, 30, {0}};
    xylem_heap_insert(&heap, &node3.heap_node);
    assert(xylem_heap_root(&heap) == &node3.heap_node);
}

static void test_heap_remove() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    xylem_heap_remove(NULL, NULL);
    xylem_heap_remove(&heap, NULL);
    xylem_heap_remove(NULL, (xylem_heap_node_t*)1);

    MyStruct nodes[5];
    for (int i = 0; i < 5; i++) {
        nodes[i].key = i;
        nodes[i].value = i * 10;
        xylem_heap_insert(&heap, &nodes[i].heap_node);
    }
    assert(heap.nelts == 5);
    xylem_heap_remove(&heap, &nodes[2].heap_node);
    assert(heap.nelts == 4);

    xylem_heap_remove(&heap, &nodes[0].heap_node);
    assert(heap.nelts == 3);
    MyStruct* root_entry = xylem_heap_entry(heap.root, MyStruct, heap_node);
    assert(root_entry->key == 1);
}

static void test_heap_dequeue() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    xylem_heap_dequeue(NULL);
    xylem_heap_dequeue(&heap);

    MyStruct nodes[5];
    int      keys[] = {10, 5, 20, 3, 8};
    for (int i = 0; i < 5; i++) {
        nodes[i].key = keys[i];
        nodes[i].value = i;
        xylem_heap_insert(&heap, &nodes[i].heap_node);
    }
    assert(heap.nelts == 5);
    MyStruct* root_entry = xylem_heap_entry(heap.root, MyStruct, heap_node);
    assert(root_entry->key == 3);

    xylem_heap_dequeue(&heap);
    assert(heap.nelts == 4);
    root_entry = xylem_heap_entry(heap.root, MyStruct, heap_node);
    assert(root_entry->key == 5);
}

static void test_heap_single_node() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    MyStruct node = {42, 100, {0}};
    xylem_heap_insert(&heap, &node.heap_node);

    assert(heap.nelts == 1);
    assert(heap.root == &node.heap_node);
    assert(xylem_heap_root(&heap) == &node.heap_node);
    assert(xylem_heap_empty(&heap) == false);

    xylem_heap_remove(&heap, &node.heap_node);
    assert(heap.nelts == 0);
    assert(heap.root == NULL);
    assert(xylem_heap_empty(&heap) == true);
}

static void test_heap_random_operations() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    MyStruct nodes[100];
    for (int i = 0; i < 100; i++) {
        nodes[i].key = rand() % 1000;
        nodes[i].value = i;
        xylem_heap_insert(&heap, &nodes[i].heap_node);
    }
    assert(heap.nelts == 100);

    int last_key = -1;
    while (!xylem_heap_empty(&heap)) {
        MyStruct* min = xylem_heap_entry(heap.root, MyStruct, heap_node);
        assert(min->key >= last_key);
        last_key = min->key;
        xylem_heap_dequeue(&heap);
    }
    assert(heap.nelts == 0);
    assert(heap.root == NULL);
}

static void test_heap_duplicate_keys() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    MyStruct nodes[10];
    int      keys[] = {5, 5, 5, 5, 5, 3, 3, 3, 8, 8};
    for (int i = 0; i < 10; i++) {
        nodes[i].key = keys[i];
        nodes[i].value = i;
        xylem_heap_insert(&heap, &nodes[i].heap_node);
    }
    assert(heap.nelts == 10);

    MyStruct* first = xylem_heap_entry(heap.root, MyStruct, heap_node);
    assert(first->key == 3);

    int count = 0;
    while (!xylem_heap_empty(&heap)) {
        MyStruct* min = xylem_heap_entry(heap.root, MyStruct, heap_node);
        assert(min->key >= 3 && min->key <= 8);
        count++;
        xylem_heap_dequeue(&heap);
    }
    assert(count == 10);
}

static void test_heap_remove_last_node() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    MyStruct node = {5, 10, {0}};
    xylem_heap_insert(&heap, &node.heap_node);
    assert(heap.nelts == 1);
    xylem_heap_remove(&heap, &node.heap_node);
    assert(heap.nelts == 0);
    assert(heap.root == NULL);

    xylem_heap_t heap2;
    xylem_heap_init(&heap2, my_cmp);
    MyStruct nodes[3] = {
        {3, 1, {0}},
        {2, 2, {0}},
        {1, 3, {0}},
    };
    for (int i = 0; i < 3; i++) {
        xylem_heap_insert(&heap2, &nodes[i].heap_node);
    }
    assert(heap2.nelts == 3);
    xylem_heap_remove(&heap2, &nodes[2].heap_node);
    assert(heap2.nelts == 2);
}

static void test_heap_edge_cases() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    xylem_heap_dequeue(NULL);
    assert(xylem_heap_root(NULL) == NULL);
    assert(xylem_heap_empty(NULL) == true);
    xylem_heap_remove(NULL, NULL);
    xylem_heap_remove(&heap, NULL);

    MyStruct nodes[4] = {
        {10, 1, {0}},
        {20, 2, {0}},
        {30, 3, {0}},
        {40, 4, {0}},
    };
    for (int i = 0; i < 4; i++) {
        xylem_heap_insert(&heap, &nodes[i].heap_node);
    }
    assert(heap.nelts == 4);

    for (int i = 2; i >= 0; i--) {
        xylem_heap_remove(&heap, &nodes[i].heap_node);
        assert(heap.nelts == (size_t)(i + 1));
    }
    assert(heap.nelts == 1);
    xylem_heap_remove(&heap, &nodes[3].heap_node);
    assert(heap.nelts == 0);
}

static void test_heap_upward_adjustment() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    MyStruct nodes[10];
    int      keys[] = {50, 30, 70, 20, 40, 60, 80, 10, 35, 5};

    for (int i = 0; i < 10; i++) {
        nodes[i].key = keys[i];
        nodes[i].value = i;
        xylem_heap_insert(&heap, &nodes[i].heap_node);
    }
    assert(heap.nelts == 10);
    MyStruct* root = xylem_heap_entry(heap.root, MyStruct, heap_node);
    assert(root->key == 5);

    xylem_heap_node_t* right_child = heap.root->right;
    if (right_child != NULL) {
        MyStruct* right_entry =
            xylem_heap_entry(right_child, MyStruct, heap_node);
        if (right_entry->key == 70) {
            xylem_heap_remove(&heap, right_child);
            assert(heap.nelts == 9);
        }
    }
    MyStruct small_node = {1, 100, {0}};
    xylem_heap_insert(&heap, &small_node.heap_node);
    root = xylem_heap_entry(heap.root, MyStruct, heap_node);
    assert(root->key == 1);

    if (heap.root->left != NULL) {
        xylem_heap_remove(&heap, heap.root->left);
    }
    while (!xylem_heap_empty(&heap)) {
        xylem_heap_dequeue(&heap);
    }
}

static void test_heap_complex_removal() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    MyStruct nodes[15];
    for (int i = 0; i < 15; i++) {
        nodes[i].key = (i * 7) % 20;
        nodes[i].value = i;
        xylem_heap_insert(&heap, &nodes[i].heap_node);
    }
    assert(heap.nelts == 15);

    for (int i = 0; i < 7; i++) {
        if (heap.root != NULL) {
            xylem_heap_dequeue(&heap);
        }
    }
    assert(heap.nelts == 8);

    for (int i = 7; i < 15; i++) {
        xylem_heap_remove(&heap, &nodes[i].heap_node);
    }
    assert(heap.nelts == 0);
}

static void test_heap_remove_last_root_node() {
    xylem_heap_t heap;
    xylem_heap_init(&heap, my_cmp);

    MyStruct node1 = {5, 10, {0}};
    xylem_heap_insert(&heap, &node1.heap_node);
    assert(heap.nelts == 1);
    assert(heap.root == &node1.heap_node);

    xylem_heap_remove(&heap, &node1.heap_node);
    assert(heap.nelts == 0);
    assert(heap.root == NULL);
    assert(xylem_heap_empty(&heap) == true);

    MyStruct nodes[3] = {
        {3, 1, {0}},
        {1, 2, {0}},
        {2, 3, {0}},
    };
    for (int i = 0; i < 3; i++) {
        xylem_heap_insert(&heap, &nodes[i].heap_node);
    }
    assert(heap.nelts == 3);

    xylem_heap_remove(&heap, &nodes[0].heap_node);
    assert(heap.nelts == 2);

    xylem_heap_remove(&heap, &nodes[2].heap_node);
    assert(heap.nelts == 1);

    xylem_heap_remove(&heap, &nodes[1].heap_node);
    assert(heap.nelts == 0);
    assert(heap.root == NULL);

    MyStruct node2 = {10, 20, {0}};
    xylem_heap_insert(&heap, &node2.heap_node);
    assert(heap.nelts == 1);

    xylem_heap_dequeue(&heap);
    assert(heap.nelts == 0);
    assert(heap.root == NULL);

    MyStruct node3 = {7, 30, {0}};
    xylem_heap_insert(&heap, &node3.heap_node);
    assert(heap.nelts == 1);
    assert(heap.root == &node3.heap_node);

    xylem_heap_remove(&heap, &node3.heap_node);
    assert(heap.nelts == 0);
    assert(heap.root == NULL);
}

int main() {
    test_heap_init();
    test_heap_empty();
    test_heap_insert();
    test_heap_root();
    test_heap_remove();
    test_heap_dequeue();
    test_heap_single_node();
    test_heap_random_operations();
    test_heap_duplicate_keys();
    test_heap_remove_last_node();
    test_heap_edge_cases();
    test_heap_upward_adjustment();
    test_heap_complex_removal();
    test_heap_remove_last_root_node();
    return 0;
}