#include "xylem.h"
#include "assert.h"

#define RB_RED 0
#define RB_BLACK 1

typedef struct {
    int                 key;
    int                 value;
    xylem_rbtree_node_t node;
} test_node_t;

static int cmp_nn(const xylem_rbtree_node_t* a, const xylem_rbtree_node_t* b) {
    test_node_t* node_a = xylem_rbtree_entry(a, test_node_t, node);
    test_node_t* node_b = xylem_rbtree_entry(b, test_node_t, node);
    if (node_a->key < node_b->key)
        return -1;
    if (node_a->key > node_b->key)
        return 1;
    return 0;
}

static int cmp_kn(const void* key, const xylem_rbtree_node_t* node) {
    int          k = *(const int*)key;
    test_node_t* node_t = xylem_rbtree_entry(node, test_node_t, node);
    if (k < node_t->key)
        return -1;
    if (k > node_t->key)
        return 1;
    return 0;
}

static test_node_t* create_test_node(int key, int value) {
    test_node_t* node = (test_node_t*)malloc(sizeof(test_node_t));
    ASSERT(node != NULL);
    node->key = key;
    node->value = value;
    return node;
}

static void free_test_node(test_node_t* node) {
    free(node);
}

static int count_nodes(xylem_rbtree_node_t* node) {
    if (!node)
        return 0;
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

static void validate_tree_order(xylem_rbtree_t* tree) {
    xylem_rbtree_node_t* node = xylem_rbtree_first(tree);
    xylem_rbtree_node_t* prev_node = NULL;

    while (node) {
        if (prev_node) {
            test_node_t* prev =
                xylem_rbtree_entry(prev_node, test_node_t, node);
            test_node_t* curr = xylem_rbtree_entry(node, test_node_t, node);
            ASSERT(curr->key > prev->key);
        }
        prev_node = node;
        node = xylem_rbtree_next(node);
    }
}

static int
verify_rbtree_node(xylem_rbtree_node_t* node, int* black_height, int is_root) {
    if (!node) {
        *black_height = 1;
        return 1;
    }

    test_node_t* current = xylem_rbtree_entry(node, test_node_t, node);

    if (is_root) {
        ASSERT(node->color == RB_BLACK);
    }

    if (node->color == RB_RED) {
        if (node->left) {
            ASSERT(node->left->color == RB_BLACK);
        }
        if (node->right) {
            ASSERT(node->right->color == RB_BLACK);
        }
    }

    if (node->left) {
        test_node_t* left = xylem_rbtree_entry(node->left, test_node_t, node);
        ASSERT(left->key < current->key);
    }

    if (node->right) {
        test_node_t* right = xylem_rbtree_entry(node->right, test_node_t, node);
        ASSERT(right->key > current->key);
    }

    int left_black_height, right_black_height;
    ASSERT(verify_rbtree_node(node->left, &left_black_height, 0));
    ASSERT(verify_rbtree_node(node->right, &right_black_height, 0));

    ASSERT(left_black_height == right_black_height);

    *black_height = left_black_height + (node->color == RB_BLACK ? 1 : 0);
    return 1;
}

static void verify_rbtree_properties(xylem_rbtree_t* tree) {
    if (!tree || !tree->root) {
        return;
    }

    ASSERT(tree->root->color == RB_BLACK);

    int black_height;
    ASSERT(verify_rbtree_node(tree->root, &black_height, 1));

    validate_tree_order(tree);
}

static void test_insertion_patterns() {
    {
        xylem_rbtree_t tree;
        ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

        const int N = 100;
        for (int i = 0; i < N; i++) {
            test_node_t* node = create_test_node(i, i * 10);
            ASSERT(xylem_rbtree_insert(&tree, &node->node) == XYLEM_RBTREE_OK);
            verify_rbtree_properties(&tree);
        }

        ASSERT(count_nodes(tree.root) == N);

        xylem_rbtree_node_t* node = xylem_rbtree_first(&tree);
        int                  expected = 0;
        while (node) {
            test_node_t* current = xylem_rbtree_entry(node, test_node_t, node);
            ASSERT(current->key == expected);
            expected++;
            node = xylem_rbtree_next(node);
        }
        ASSERT(expected == N);

        node = xylem_rbtree_first(&tree);
        while (node) {
            xylem_rbtree_node_t* next = xylem_rbtree_next(node);
            test_node_t* current = xylem_rbtree_entry(node, test_node_t, node);
            xylem_rbtree_erase(&tree, node);
            free_test_node(current);
            node = next;
        }
    }

    {
        xylem_rbtree_t tree;
        ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

        const int N = 100;
        for (int i = N - 1; i >= 0; i--) {
            test_node_t* node = create_test_node(i, i * 10);
            ASSERT(xylem_rbtree_insert(&tree, &node->node) == XYLEM_RBTREE_OK);
            verify_rbtree_properties(&tree);
        }

        ASSERT(count_nodes(tree.root) == N);

        xylem_rbtree_node_t* node = xylem_rbtree_first(&tree);
        int                  expected = 0;
        while (node) {
            test_node_t* current = xylem_rbtree_entry(node, test_node_t, node);
            ASSERT(current->key == expected);
            expected++;
            node = xylem_rbtree_next(node);
        }
        ASSERT(expected == N);

        node = xylem_rbtree_first(&tree);
        while (node) {
            xylem_rbtree_node_t* next = xylem_rbtree_next(node);
            test_node_t* current = xylem_rbtree_entry(node, test_node_t, node);
            xylem_rbtree_erase(&tree, node);
            free_test_node(current);
            node = next;
        }
    }

    {
        xylem_rbtree_t tree;
        ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

        const int N = 100;
        for (int i = 0; i < N; i++) {
            int          key = (i % 2 == 0) ? i : (N - i);
            test_node_t* node = create_test_node(key, i);
            xylem_rbtree_insert(&tree, &node->node);
            verify_rbtree_properties(&tree);
        }

        xylem_rbtree_node_t* node = xylem_rbtree_first(&tree);
        while (node) {
            xylem_rbtree_node_t* next = xylem_rbtree_next(node);
            test_node_t* current = xylem_rbtree_entry(node, test_node_t, node);
            xylem_rbtree_erase(&tree, node);
            free_test_node(current);
            node = next;
        }
    }
}

static void test_deletion_patterns() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    test_node_t* nodes[100];

    for (int i = 0; i < 100; i++) {
        nodes[i] = create_test_node(i, i * 10);
        ASSERT(xylem_rbtree_insert(&tree, &nodes[i]->node) == XYLEM_RBTREE_OK);
    }

    verify_rbtree_properties(&tree);

    xylem_rbtree_erase(&tree, &nodes[0]->node);
    verify_rbtree_properties(&tree);
    free_test_node(nodes[0]);
    nodes[0] = NULL;

    xylem_rbtree_erase(&tree, &nodes[1]->node);
    verify_rbtree_properties(&tree);
    free_test_node(nodes[1]);
    nodes[1] = NULL;

    xylem_rbtree_erase(&tree, &nodes[50]->node);
    verify_rbtree_properties(&tree);
    free_test_node(nodes[50]);
    nodes[50] = NULL;

    while (tree.root) {
        xylem_rbtree_erase(&tree, tree.root);
        verify_rbtree_properties(&tree);
    }

    for (int i = 0; i < 100; i++) {
        if (nodes[i]) {
            free_test_node(nodes[i]);
        }
    }
}

static void test_stress_insert_delete() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    srand(time(NULL));
    const int N = 10000;
    const int OPS = 50000;

    test_node_t** nodes = (test_node_t**)calloc(N, sizeof(test_node_t*));
    int*          exists = (int*)calloc(N, sizeof(int));

    int inserts = 0, deletes = 0, finds = 0;

    for (int i = 0; i < OPS; i++) {
        int op = rand() % 3;
        int idx = rand() % N;

        switch (op) {
        case 0:
            if (!exists[idx]) {
                nodes[idx] = create_test_node(idx, idx * 10);
                xylem_rbtree_status_t status =
                    xylem_rbtree_insert(&tree, &nodes[idx]->node);
                if (status == XYLEM_RBTREE_OK) {
                    exists[idx] = 1;
                    inserts++;
                } else {
                    free_test_node(nodes[idx]);
                    nodes[idx] = NULL;
                }
            }
            break;

        case 1:
            if (exists[idx] && nodes[idx]) {
                xylem_rbtree_erase(&tree, &nodes[idx]->node);
                free_test_node(nodes[idx]);
                nodes[idx] = NULL;
                exists[idx] = 0;
                deletes++;
            }
            break;

        case 2:
            if (exists[idx]) {
                int                  key = idx;
                xylem_rbtree_node_t* found = xylem_rbtree_find(&tree, &key);
                ASSERT(found != NULL);
                finds++;
            }
            break;
        }

        if (i % 1000 == 0) {
            verify_rbtree_properties(&tree);
        }
    }

    verify_rbtree_properties(&tree);

    for (int i = 0; i < N; i++) {
        if (exists[i] && nodes[i]) {
            xylem_rbtree_erase(&tree, &nodes[i]->node);
            free_test_node(nodes[i]);
        }
    }

    free(nodes);
    free(exists);

    ASSERT(xylem_rbtree_empty(&tree));
}

static void test_iterator_edge_cases() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    ASSERT(xylem_rbtree_first(&tree) == NULL);
    ASSERT(xylem_rbtree_last(&tree) == NULL);

    test_node_t* single = create_test_node(50, 500);
    ASSERT(xylem_rbtree_insert(&tree, &single->node) == XYLEM_RBTREE_OK);

    ASSERT(xylem_rbtree_prev(&single->node) == NULL);
    ASSERT(xylem_rbtree_next(&single->node) == NULL);
    ASSERT(xylem_rbtree_first(&tree) == &single->node);
    ASSERT(xylem_rbtree_last(&tree) == &single->node);

    test_node_t* left = create_test_node(25, 250);
    test_node_t* right = create_test_node(75, 750);
    ASSERT(xylem_rbtree_insert(&tree, &left->node) == XYLEM_RBTREE_OK);
    ASSERT(xylem_rbtree_insert(&tree, &right->node) == XYLEM_RBTREE_OK);

    verify_rbtree_properties(&tree);

    test_node_t* left_entry =
        xylem_rbtree_entry(xylem_rbtree_prev(&single->node), test_node_t, node);
    ASSERT(left_entry == left);

    test_node_t* right_entry =
        xylem_rbtree_entry(xylem_rbtree_next(&single->node), test_node_t, node);
    ASSERT(right_entry == right);

    ASSERT(xylem_rbtree_prev(&left->node) == NULL);

    ASSERT(xylem_rbtree_next(&right->node) == NULL);

    xylem_rbtree_erase(&tree, &single->node);
    xylem_rbtree_erase(&tree, &left->node);
    xylem_rbtree_erase(&tree, &right->node);

    free_test_node(single);
    free_test_node(left);
    free_test_node(right);
}

static void test_duplicate_operations() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    const int N = 1000;

    test_node_t* node = create_test_node(42, 420);
    ASSERT(xylem_rbtree_insert(&tree, &node->node) == XYLEM_RBTREE_OK);

    for (int i = 0; i < 10; i++) {
        test_node_t* dup = create_test_node(42, i);
        ASSERT(xylem_rbtree_insert(&tree, &dup->node) == XYLEM_RBTREE_DUP);
        free_test_node(dup);
    }

    verify_rbtree_properties(&tree);
    ASSERT(count_nodes(tree.root) == 1);

    xylem_rbtree_erase(&tree, &node->node);
    xylem_rbtree_erase(&tree, &node->node);
    xylem_rbtree_erase(&tree, &node->node);

    ASSERT(xylem_rbtree_empty(&tree));
    free_test_node(node);

    test_node_t** nodes = (test_node_t**)malloc(N * sizeof(test_node_t*));
    for (int i = 0; i < N; i++) {
        nodes[i] = create_test_node(i, i * 10);
        ASSERT(xylem_rbtree_insert(&tree, &nodes[i]->node) == XYLEM_RBTREE_OK);
    }

    verify_rbtree_properties(&tree);
    ASSERT(count_nodes(tree.root) == N);

    srand(time(NULL));
    int remaining = N;
    while (remaining > 0) {
        int idx = rand() % N;
        if (nodes[idx]) {
            xylem_rbtree_erase(&tree, &nodes[idx]->node);
            free_test_node(nodes[idx]);
            nodes[idx] = NULL;
            remaining--;

            if (remaining % 100 == 0) {
                verify_rbtree_properties(&tree);
                ASSERT(count_nodes(tree.root) == remaining);
            }
        }
    }

    free(nodes);
    ASSERT(xylem_rbtree_empty(&tree));
}

static void test_color_properties() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    for (int i = 0; i < 100; i++) {
        test_node_t* node = create_test_node(i, i * 10);
        ASSERT(xylem_rbtree_insert(&tree, &node->node) == XYLEM_RBTREE_OK);
        ASSERT(tree.root->color == RB_BLACK);
        verify_rbtree_properties(&tree);
    }

    for (int i = 0; i < 100; i += 2) {
        int                  key = i;
        xylem_rbtree_node_t* found = xylem_rbtree_find(&tree, &key);
        if (found) {
            test_node_t* node = xylem_rbtree_entry(found, test_node_t, node);
            xylem_rbtree_erase(&tree, found);
            free_test_node(node);
            ASSERT(tree.root ? tree.root->color == RB_BLACK : 1);
            verify_rbtree_properties(&tree);
        }
    }

    xylem_rbtree_node_t* curr = xylem_rbtree_first(&tree);
    while (curr) {
        xylem_rbtree_node_t* next = xylem_rbtree_next(curr);
        test_node_t*         node = xylem_rbtree_entry(curr, test_node_t, node);
        xylem_rbtree_erase(&tree, curr);
        free_test_node(node);
        curr = next;
    }

    ASSERT(xylem_rbtree_empty(&tree));
}

static void test_tree_balance() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    const int N = 10000;

    for (int i = 0; i < N; i++) {
        test_node_t* node = create_test_node(i, i * 10);
        ASSERT(xylem_rbtree_insert(&tree, &node->node) == XYLEM_RBTREE_OK);
    }

    verify_rbtree_properties(&tree);

    int max_height = 0;
    int min_height = INT_MAX;

    xylem_rbtree_node_t* curr = xylem_rbtree_first(&tree);
    while (curr) {
        xylem_rbtree_node_t* next = xylem_rbtree_next(curr);
        test_node_t*         node = xylem_rbtree_entry(curr, test_node_t, node);
        xylem_rbtree_erase(&tree, curr);
        free_test_node(node);
        curr = next;
    }

    ASSERT(xylem_rbtree_empty(&tree));
}

static void test_basic_insert_and_find() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);
    ASSERT(xylem_rbtree_empty(&tree));

    test_node_t* node1 = create_test_node(10, 100);
    ASSERT(xylem_rbtree_insert(&tree, &node1->node) == XYLEM_RBTREE_OK);
    ASSERT(!xylem_rbtree_empty(&tree));
    ASSERT(tree.root == &node1->node);

    test_node_t* node2 = create_test_node(5, 50);
    ASSERT(xylem_rbtree_insert(&tree, &node2->node) == XYLEM_RBTREE_OK);

    test_node_t* node3 = create_test_node(15, 150);
    ASSERT(xylem_rbtree_insert(&tree, &node3->node) == XYLEM_RBTREE_OK);

    test_node_t* dup = create_test_node(10, 200);
    ASSERT(xylem_rbtree_insert(&tree, &dup->node) == XYLEM_RBTREE_DUP);
    free_test_node(dup);

    int                  key = 10;
    xylem_rbtree_node_t* found = xylem_rbtree_find(&tree, &key);
    ASSERT(found == &node1->node);

    int not_found_key = 20;
    ASSERT(xylem_rbtree_find(&tree, &not_found_key) == NULL);

    xylem_rbtree_erase(&tree, &node1->node);
    xylem_rbtree_erase(&tree, &node2->node);
    xylem_rbtree_erase(&tree, &node3->node);

    free_test_node(node1);
    free_test_node(node2);
    free_test_node(node3);
}

static void test_edge_insertions() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    test_node_t* min_node = create_test_node(INT_MIN, 1);
    ASSERT(xylem_rbtree_insert(&tree, &min_node->node) == XYLEM_RBTREE_OK);

    test_node_t* max_node = create_test_node(INT_MAX, 2);
    ASSERT(xylem_rbtree_insert(&tree, &max_node->node) == XYLEM_RBTREE_OK);

    test_node_t* zero_node = create_test_node(0, 3);
    ASSERT(xylem_rbtree_insert(&tree, &zero_node->node) == XYLEM_RBTREE_OK);

    int key = INT_MIN;
    ASSERT(xylem_rbtree_find(&tree, &key) == &min_node->node);

    key = INT_MAX;
    ASSERT(xylem_rbtree_find(&tree, &key) == &max_node->node);

    key = 0;
    ASSERT(xylem_rbtree_find(&tree, &key) == &zero_node->node);

    verify_rbtree_properties(&tree);

    xylem_rbtree_erase(&tree, &min_node->node);
    xylem_rbtree_erase(&tree, &max_node->node);
    xylem_rbtree_erase(&tree, &zero_node->node);

    free_test_node(min_node);
    free_test_node(max_node);
    free_test_node(zero_node);
}

static void test_massive_insert_delete() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    const int     N = 10000;
    test_node_t** nodes = (test_node_t**)malloc(N * sizeof(test_node_t*));
    ASSERT(nodes != NULL);

    for (int i = 0; i < N; i++) {
        nodes[i] = create_test_node(i, i * 10);
        ASSERT(xylem_rbtree_insert(&tree, &nodes[i]->node) == XYLEM_RBTREE_OK);
    }
    ASSERT(count_nodes(tree.root) == N);
    verify_rbtree_properties(&tree);

    for (int i = 0; i < N; i += 2) {
        xylem_rbtree_erase(&tree, &nodes[i]->node);
    }
    ASSERT(count_nodes(tree.root) == N / 2);
    verify_rbtree_properties(&tree);

    for (int i = 1; i < N; i += 2) {
        int                  key = i;
        xylem_rbtree_node_t* found = xylem_rbtree_find(&tree, &key);
        ASSERT(found == &nodes[i]->node);
    }
    for (int i = 1; i < N; i += 2) {
        xylem_rbtree_erase(&tree, &nodes[i]->node);
    }
    ASSERT(xylem_rbtree_empty(&tree));

    for (int i = 0; i < N; i++) {
        free_test_node(nodes[i]);
    }
    free(nodes);
}

static void test_reverse_order_insert() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    const int N = 1000;
    for (int i = N - 1; i >= 0; i--) {
        test_node_t* node = create_test_node(i, i);
        ASSERT(xylem_rbtree_insert(&tree, &node->node) == XYLEM_RBTREE_OK);
        xylem_rbtree_erase(&tree, &node->node);
        free_test_node(node);
    }
    ASSERT(xylem_rbtree_empty(&tree));
}

static void test_random_insert_delete() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    srand(time(NULL));
    const int     N = 5000;
    test_node_t** nodes = (test_node_t**)malloc(N * sizeof(test_node_t*));
    int*          inserted = (int*)calloc(N, sizeof(int));

    for (int i = 0; i < N * 2; i++) {
        int idx = rand() % N;
        if (inserted[idx]) {
            if (nodes[idx]) {
                xylem_rbtree_erase(&tree, &nodes[idx]->node);
                free_test_node(nodes[idx]);
                nodes[idx] = NULL;
                inserted[idx] = 0;
            }
        } else {
            nodes[idx] = create_test_node(idx, idx * 10);
            xylem_rbtree_status_t status =
                xylem_rbtree_insert(&tree, &nodes[idx]->node);
            if (status == XYLEM_RBTREE_OK) {
                inserted[idx] = 1;
            } else {
                free_test_node(nodes[idx]);
                nodes[idx] = NULL;
            }
        }
        if (i % 1000 == 0) {
            verify_rbtree_properties(&tree);
        }
    }
    for (int i = 0; i < N; i++) {
        if (inserted[i] && nodes[i]) {
            xylem_rbtree_erase(&tree, &nodes[i]->node);
            free_test_node(nodes[i]);
        }
    }
    free(nodes);
    free(inserted);
    ASSERT(xylem_rbtree_empty(&tree));
}

static void test_traversal_functions(void) {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    int keys[] = {
        50, 30, 70, 20, 40, 60, 80, 10, 90, 25, 35, 45, 55, 65, 75, 85};
    const size_t N = sizeof(keys) / sizeof(keys[0]);

    test_node_t** nodes = (test_node_t**)malloc(N * sizeof(test_node_t*));
    ASSERT(nodes != NULL);

    for (size_t i = 0; i < N; i++) {
        nodes[i] = create_test_node(keys[i], (int)i);
        ASSERT(xylem_rbtree_insert(&tree, &nodes[i]->node) == XYLEM_RBTREE_OK);
    }
    verify_rbtree_properties(&tree);

    xylem_rbtree_node_t* first = xylem_rbtree_first(&tree);
    ASSERT(first != NULL);
    test_node_t* first_node = xylem_rbtree_entry(first, test_node_t, node);
    ASSERT(first_node->key == 10);

    xylem_rbtree_node_t* last = xylem_rbtree_last(&tree);
    ASSERT(last != NULL);
    test_node_t* last_node = xylem_rbtree_entry(last, test_node_t, node);
    ASSERT(last_node->key == 90);

    xylem_rbtree_node_t* curr = first;
    int                  prev_key = INT_MIN;
    size_t               count = 0;
    while (curr) {
        test_node_t* curr_node = xylem_rbtree_entry(curr, test_node_t, node);
        ASSERT(curr_node->key > prev_key);
        prev_key = curr_node->key;
        curr = xylem_rbtree_next(curr);
        count++;
    }
    ASSERT(count == N);

    curr = last;
    prev_key = INT_MAX;
    count = 0;
    while (curr) {
        test_node_t* curr_node = xylem_rbtree_entry(curr, test_node_t, node);
        ASSERT(curr_node->key < prev_key);
        prev_key = curr_node->key;
        curr = xylem_rbtree_prev(curr);
        count++;
    }
    ASSERT(count == N);

    for (size_t i = 0; i < N; i++) {
        xylem_rbtree_erase(&tree, &nodes[i]->node);
        free_test_node(nodes[i]);
    }
    free(nodes);
}

static void test_null_parameters() {
    ASSERT(xylem_rbtree_init(NULL, cmp_nn, cmp_kn) == XYLEM_RBTREE_ERR);

    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, NULL, cmp_kn) == XYLEM_RBTREE_ERR);
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, NULL) == XYLEM_RBTREE_ERR);

    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    test_node_t* node = create_test_node(1, 1);
    ASSERT(xylem_rbtree_insert(NULL, &node->node) == XYLEM_RBTREE_ERR);
    ASSERT(xylem_rbtree_insert(&tree, NULL) == XYLEM_RBTREE_ERR);

    int key = 1;
    ASSERT(xylem_rbtree_find(NULL, &key) == NULL);
    ASSERT(xylem_rbtree_find(&tree, NULL) == NULL);

    ASSERT(xylem_rbtree_first(NULL) == NULL);
    ASSERT(xylem_rbtree_last(NULL) == NULL);

    xylem_rbtree_erase(NULL, &node->node);
    xylem_rbtree_erase(&tree, NULL);

    bool empty_result = xylem_rbtree_empty(NULL);

    free_test_node(node);
}

static void test_single_node_operations() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    test_node_t* single = create_test_node(42, 420);
    ASSERT(xylem_rbtree_insert(&tree, &single->node) == XYLEM_RBTREE_OK);
    verify_rbtree_properties(&tree);

    ASSERT(xylem_rbtree_first(&tree) == &single->node);
    ASSERT(xylem_rbtree_last(&tree) == &single->node);
    ASSERT(xylem_rbtree_next(&single->node) == NULL);
    ASSERT(xylem_rbtree_prev(&single->node) == NULL);

    int key = 42;
    ASSERT(xylem_rbtree_find(&tree, &key) == &single->node);

    xylem_rbtree_erase(&tree, &single->node);
    ASSERT(xylem_rbtree_empty(&tree));

    free_test_node(single);
}

static void test_duplicate_handling() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    test_node_t* node1 = create_test_node(100, 1);
    test_node_t* node2 = create_test_node(100, 2);
    test_node_t* node3 = create_test_node(100, 3);

    ASSERT(xylem_rbtree_insert(&tree, &node1->node) == XYLEM_RBTREE_OK);
    ASSERT(xylem_rbtree_insert(&tree, &node2->node) == XYLEM_RBTREE_DUP);
    ASSERT(xylem_rbtree_insert(&tree, &node3->node) == XYLEM_RBTREE_DUP);

    int                  key = 100;
    xylem_rbtree_node_t* found = xylem_rbtree_find(&tree, &key);
    ASSERT(found == &node1->node);
    test_node_t* found_node = xylem_rbtree_entry(found, test_node_t, node);
    ASSERT(found_node->value == 1);

    xylem_rbtree_erase(&tree, &node1->node);
    ASSERT(xylem_rbtree_find(&tree, &key) == NULL);

    free_test_node(node1);
    free_test_node(node2);
    free_test_node(node3);
}

static void test_tree_properties_after_operations() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    test_node_t* nodes[100];

    for (int i = 0; i < 100; i++) {
        nodes[i] = create_test_node(i * 2, i);
        ASSERT(xylem_rbtree_insert(&tree, &nodes[i]->node) == XYLEM_RBTREE_OK);
        verify_rbtree_properties(&tree);
    }
    for (int i = 0; i < 100; i += 3) {
        xylem_rbtree_erase(&tree, &nodes[i]->node);
        verify_rbtree_properties(&tree);
    }
    for (int i = 1; i < 100; i += 3) {
        xylem_rbtree_erase(&tree, &nodes[i]->node);
        verify_rbtree_properties(&tree);
    }
    for (int i = 2; i < 100; i += 3) {
        xylem_rbtree_erase(&tree, &nodes[i]->node);
        verify_rbtree_properties(&tree);
    }
    ASSERT(xylem_rbtree_empty(&tree));

    for (int i = 0; i < 100; i++) {
        free_test_node(nodes[i]);
    }
}

static void test_large_scale_random() {
    xylem_rbtree_t tree;
    ASSERT(xylem_rbtree_init(&tree, cmp_nn, cmp_kn) == XYLEM_RBTREE_OK);

    srand(time(NULL));
    const int N = 100000;

    clock_t start = clock();
    for (int i = 0; i < N; i++) {
        int          key = rand() % 1000000;
        test_node_t* node = create_test_node(key, i);
        xylem_rbtree_status_t status = xylem_rbtree_insert(&tree, &node->node);
        if (status != XYLEM_RBTREE_OK) {
            free_test_node(node);
        }
    }
    clock_t end = clock();
    double  insert_time = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < 10000; i++) {
        int key = rand() % 1000000;
        xylem_rbtree_find(&tree, &key);
    }
    end = clock();
    double find_time = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    xylem_rbtree_node_t* curr = xylem_rbtree_first(&tree);
    int                  count = 0;
    int                  prev = INT_MIN;
    while (curr) {
        test_node_t* curr_node = xylem_rbtree_entry(curr, test_node_t, node);
        ASSERT(curr_node->key >= prev);
        prev = curr_node->key;
        curr = xylem_rbtree_next(curr);
        count++;
    }
    end = clock();
    double traverse_time = (double)(end - start) / CLOCKS_PER_SEC;

    verify_rbtree_properties(&tree);

    xylem_rbtree_node_t* node = xylem_rbtree_first(&tree);
    while (node) {
        xylem_rbtree_node_t* next = xylem_rbtree_next(node);
        test_node_t* current = xylem_rbtree_entry(node, test_node_t, node);
        xylem_rbtree_erase(&tree, node);
        free_test_node(current);
        node = next;
    }
    ASSERT(xylem_rbtree_empty(&tree));
}

int main() {
    test_basic_insert_and_find();
    test_edge_insertions();
    test_massive_insert_delete();
    test_reverse_order_insert();
    test_random_insert_delete();
    test_traversal_functions();
    test_null_parameters();
    test_single_node_operations();
    test_duplicate_handling();
    test_tree_properties_after_operations();
    test_large_scale_random();
    test_insertion_patterns();
    test_deletion_patterns();
    test_stress_insert_delete();
    test_iterator_edge_cases();
    test_duplicate_operations();
    test_color_properties();
    test_tree_balance();
    return 0;
}