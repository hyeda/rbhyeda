#include <assert.h>
#include <rbtree.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// new_rbtree should return rbtree struct with null root node
void test_init(void) {
  rbtree *t = new_rbtree(); 
  // 새로운 rb트리 생성, 구조체에 대한 포인터를 반환
  assert(t != NULL);
  // NULL이 아닌 유효한 트리 포인터를 반환했는지 확인. NULL이면 프로그램이 멈춤 
#ifdef SENTINEL 
  // sentinel이라는 메크로가 정의됐는지 확인.
  assert(t->nil != NULL); 
  //nil 포인터가 NULL이 아닌지 확인
  assert(t->root == t->nil); 
  // 루트 노드가 nil을 가리키는지, 이건 비어있다는 뜻
#else 
  assert(t->root == NULL); // 루트노드가 NULL상태인지.
#endif 
  delete_rbtree(t); // 테스트가 끝나고 트리 삭제하고, 메모리 해제
}

// root node should have proper values and pointers
void test_insert_single(const key_t key) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);
  assert(p != NULL);
  assert(t->root == p);
  assert(p->key == key);
  // assert(p->color == RBTREE_BLACK);  // color of root node should be black
#ifdef SENTINEL
  assert(p->left == t->nil);
  assert(p->right == t->nil);
  assert(p->parent == t->nil);
#else
  assert(p->left == NULL);
  assert(p->right == NULL);
  assert(p->parent == NULL);
#endif
  delete_rbtree(t);
}

// find should return the node with the key or NULL if no such node exists
void test_find_single(const key_t key, const key_t wrong_key) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);

  node_t *q = rbtree_find(t, key);
  assert(q != NULL);
  assert(q->key == key);
  assert(q == p);

  q = rbtree_find(t, wrong_key);
  assert(q == NULL);

  delete_rbtree(t);
}

// erase should delete root node
void test_erase_root(const key_t key) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);
  assert(p != NULL);
  assert(t->root == p);
  assert(p->key == key);

  rbtree_erase(t, p);
#ifdef SENTINEL
  assert(t->root == t->nil);
#else
  assert(t->root == NULL);
#endif

  delete_rbtree(t);
}

static void insert_arr(rbtree *t, const key_t *arr, const size_t n) {
  for (size_t i = 0; i < n; i++) {
    rbtree_insert(t, arr[i]);
  }
}

static int comp(const void *p1, const void *p2) {
  const key_t *e1 = (const key_t *)p1;
  const key_t *e2 = (const key_t *)p2;
  if (*e1 < *e2) {
    return -1;
  } else if (*e1 > *e2) {
    return 1;
  } else {
    return 0;
  }
};

// min/max should return the min/max value of the tree
void test_minmax(key_t *arr, const size_t n) {
  // null array is not allowed
  assert(n > 0 && arr != NULL);

  rbtree *t = new_rbtree();
  assert(t != NULL);

  insert_arr(t, arr, n);
  assert(t->root != NULL);
#ifdef SENTINEL
  assert(t->root != t->nil);
#endif

  qsort((void *)arr, n, sizeof(key_t), comp);
  node_t *p = rbtree_min(t);
  assert(p != NULL);
  assert(p->key == arr[0]);

  node_t *q = rbtree_max(t);
  assert(q != NULL);
  assert(q->key == arr[n - 1]);

  rbtree_erase(t, p);
  p = rbtree_min(t);
  assert(p != NULL);
  assert(p->key == arr[1]);

  if (n >= 2) {
    rbtree_erase(t, q);
    q = rbtree_max(t);
    assert(q != NULL);
    assert(q->key == arr[n - 2]);
  }

  delete_rbtree(t);
}

void test_to_array(rbtree *t, const key_t *arr, const size_t n) {
  assert(t != NULL);
  
  insert_arr(t, arr, n);
  qsort((void *)arr, n, sizeof(key_t), comp);

  key_t *res = calloc(n, sizeof(key_t));
  rbtree_to_array(t, res, n);
  for (int i = 0; i < n; i++) {
    assert(arr[i] == res[i]);
  }
  free(res);
}

void test_multi_instance() {
  rbtree *t1 = new_rbtree();
  assert(t1 != NULL);
  rbtree *t2 = new_rbtree();
  assert(t2 != NULL);

  key_t arr1[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n1 = sizeof(arr1) / sizeof(arr1[0]);
  insert_arr(t1, arr1, n1);
  qsort((void *)arr1, n1, sizeof(key_t), comp);

  key_t arr2[] = {4, 8, 10, 5, 3};
  const size_t n2 = sizeof(arr2) / sizeof(arr2[0]);
  insert_arr(t2, arr2, n2);
  qsort((void *)arr2, n2, sizeof(key_t), comp);

  key_t *res1 = calloc(n1, sizeof(key_t));
  rbtree_to_array(t1, res1, n1);
  for (int i = 0; i < n1; i++) {
    assert(arr1[i] == res1[i]);
  }

  key_t *res2 = calloc(n2, sizeof(key_t));
  rbtree_to_array(t2, res2, n2);
  for (int i = 0; i < n2; i++) {
    assert(arr2[i] == res2[i]);
  }

  free(res2);
  free(res1);
  delete_rbtree(t2);
  delete_rbtree(t1);
}

// Search tree constraint
// The values of left subtree should be less than or equal to the current node
// The values of right subtree should be greater than or equal to the current
// node

static bool search_traverse(const node_t *p, key_t *min, key_t *max,
                            node_t *nil) {
  if (p == nil) {
    return true;
  }

  *min = *max = p->key;

  key_t l_min, l_max, r_min, r_max;
  l_min = l_max = r_min = r_max = p->key;

  const bool lr = search_traverse(p->left, &l_min, &l_max, nil);
  if (!lr || l_max > p->key) {
    return false;
  }
  const bool rr = search_traverse(p->right, &r_min, &r_max, nil);
  if (!rr || r_min < p->key) {
    return false;
  }

  *min = l_min;
  *max = r_max;
  return true;
}

void test_search_constraint(const rbtree *t) {
  assert(t != NULL);
  node_t *p = t->root;
  key_t min, max;
#ifdef SENTINEL
  node_t *nil = t->nil;
#else
  node_t *nil = NULL;
#endif
  assert(search_traverse(p, &min, &max, nil));
}

// Color constraint
// 1. Each node is either red or black. (by definition)
// 2. All NIL nodes are considered black.
// 3. A red node does not have a red child.
// 4. Every path from a given node to any of its descendant NIL nodes goes
// through the same number of black nodes.

bool touch_nil = false;
int max_black_depth = 0;

static void init_color_traverse(void) {
  touch_nil = false;
  max_black_depth = 0;
}

static bool color_traverse(const node_t *p, const color_t parent_color,
                           const int black_depth, node_t *nil) {
  if (p == nil) {
    if (!touch_nil) {
      touch_nil = true;
      max_black_depth = black_depth;
    } else if (black_depth != max_black_depth) {
      return false;
    }
    return true;
  }
  if (parent_color == RBTREE_RED && p->color == RBTREE_RED) {
    return false;
  }
  int next_depth = ((p->color == RBTREE_BLACK) ? 1 : 0) + black_depth;
  return color_traverse(p->left, p->color, next_depth, nil) &&
         color_traverse(p->right, p->color, next_depth, nil);
}

void test_color_constraint(const rbtree *t) {
  assert(t != NULL);
#ifdef SENTINEL
  node_t *nil = t->nil;
#else
  node_t *nil = NULL;
#endif
  node_t *p = t->root;
  assert(p == nil || p->color == RBTREE_BLACK);

  init_color_traverse();
  assert(color_traverse(p, RBTREE_BLACK, 0, nil));
}

// rbtree should keep search tree and color constraints
void test_rb_constraints(const key_t arr[], const size_t n) {
  rbtree *t = new_rbtree();
  assert(t != NULL);

  insert_arr(t, arr, n);
  assert(t->root != NULL);

  test_color_constraint(t);
  test_search_constraint(t);

  delete_rbtree(t);
}

// rbtree should manage distinct values
void test_distinct_values() {
  const key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_rb_constraints(entries, n);
}

// rbtree should manage values with duplicate
void test_duplicate_values() {
  const key_t entries[] = {10, 5, 5, 34, 6, 23, 12, 12, 6, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_rb_constraints(entries, n);
}

void test_minmax_suite() {
  key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_minmax(entries, n);
}

void test_to_array_suite() {
  rbtree *t = new_rbtree();
  assert(t != NULL);

  key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_to_array(t, entries, n);

  delete_rbtree(t);
}

void test_find_erase(rbtree *t, const key_t *arr, const size_t n) {
  for (int i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    assert(p != NULL);
  }

  for (int i = 0; i < n; i++) {
    node_t *p = rbtree_find(t, arr[i]);
    // printf("arr[%d] = %d\n", i, arr[i]);
    assert(p != NULL);
    assert(p->key == arr[i]);
    rbtree_erase(t, p);
  }

  for (int i = 0; i < n; i++) {
    node_t *p = rbtree_find(t, arr[i]);
    assert(p == NULL);
  }

  for (int i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    assert(p != NULL);
    node_t *q = rbtree_find(t, arr[i]);
    assert(q != NULL);
    assert(q->key == arr[i]);
    assert(p == q);
    rbtree_erase(t, p);
    q = rbtree_find(t, arr[i]);
    assert(q == NULL);
  }
}

void test_find_erase_fixed() {
  const key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n = sizeof(arr) / sizeof(arr[0]);
  rbtree *t = new_rbtree();
  assert(t != NULL);

  test_find_erase(t, arr, n);

  delete_rbtree(t);
}

void test_find_erase_rand(const size_t n, const unsigned int seed) {
  srand(seed);
  rbtree *t = new_rbtree();
  key_t *arr = calloc(n, sizeof(key_t));
  for (int i = 0; i < n; i++) {
    arr[i] = rand();
  }

  test_find_erase(t, arr, n);

  free(arr);
  delete_rbtree(t);
}

int main(void) {
  test_init(); // 트리 처음 만들 때 트리의 루트가 nil로 설정되는지, 비어있는 상태에서 시작되는지 확인.
  test_insert_single(1024); // 단일 값이 정확히 삽입되는지.
  test_find_single(512, 1024); // 특정 값을 찾는 과정을 검사, 없는 값은 NULL
  test_erase_root(128); // 루트노드를 삭제하는 과정 검사, 루트는 트리 전체의 구조에 영향을 줄 수 있기 때문에 매우 중요함.
  test_find_erase_fixed(); // 정해진 여러 값을 트리에 삽입한 후, 특정 값을 찾고 삭제하는 과정 확인.
  test_minmax_suite(); // 트리에서 최솟값, 최댓값 찾는 테스트
  test_to_array_suite(); // 트리에 저장된 값들을 배열로 변환하는 기능. 일반적으로 중위 순회를 사용하여 정렬된 배열을 얻는다.
  test_distinct_values(); // 여러값을 삽입해도 올바르게 작동하는지, 균형을 유지하는지 확인.
  test_duplicate_values(); // 중복된 값을 삽입했을 때 제대로 유지되는지 체크
  test_multi_instance(); // 여러 트리 인스턴스가 동시에 작동할 때, 각 트리가 독립적으로 서로 간섭하지 않고 잘 동작하는지 확인. 
  test_find_erase_rand(10000, 17); // 랜덤하게 생성된 값들을 트리에 삽입하고 값을 찾고 삭제하는 과정을 테스트함.
  printf("Passed all tests!\n");
}
