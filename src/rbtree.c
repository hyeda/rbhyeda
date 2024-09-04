#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); 
  // calloc 함수는 메모리를 할당하고 0으로 초기화함. 1이 의미하는건 구조체 하나 메모리 할당, 뒤에건 구조체 메모리 블록의 크기.
  if (p != NULL){
    p->nil = (node_t*)calloc(1, sizeof(node_t));
    if (p->nil != NULL){ // 메모리 할당이 실패하면 NULL을 반환함 그래서 성공했을 시에만 시행하게 한거임.
      p->nil->color = RBTREE_BLACK;
      p->nil->left = p->nil->right = p->nil->parent = p->nil; 
      // 가지 왼쪽 오른쪽 부모 모두 자기 자신을 가리키고 있음. 가지의 끝을 표현함.
      // 부모 까지 자기 자신으로 설정하는 이유는 트리 구조 구현할 때 흔히 사용되는 관례임. 코드의 일관성을 유지할 수 있음.
      // 예외 처리를 단순화 할수 있음. 사실 nil이라는 노드는 부모, 자식 관계를 가지는 노드가 아니기 때문
      p->root = p->nil; // 처음엔 나무에 아무것도 없는 상태라고 보면 됨.
    } 
  }
  return p;
}
void delete_node(rbtree *t, node_t *n){
  if (n == t->nil){
    return;
  }
  delete_node(t, n->left);
  delete_node(t, n->right); 
  free(n); // 루트를 젤 마지막에 해제
}
void delete_rbtree(rbtree *t) {
  delete_node(t, t->root); // 
  free(t->nil); //닐노드까지 해제
  free(t); // 전체 해제
}

void left_rotate(rbtree *t, node_t *x){
  node_t *y = x->right;
  x->right = y->left;
  if (y->left != t->nil) {// y의 왼쪽자식이 있으면 
    y->left->parent = x; //그 자식의 부모를 x로 설정 
  }
  y->parent = x->parent; // y의 부모가 x의 부모였던애가 됨.
  if (x->parent == t->nil) { // 만약 x의 부모가 없다면 즉 x가 루트였다는 거
    t->root = y; // 루트노드가 y가 됨.
  } else if (x == x->parent->left) { // x가 왼쪽 자식이라면 
    x->parent->left = y; // 그 자리에 y가 감.
  } else { // x가 오른쪽 자식이었다면 
    x->parent->right = y; //y 가 그자리에 감.
  }
  y->left = x; // y의 왼쪽 자식이 x
  x->parent = y; // x의 부모 y
}

void right_rotate(rbtree *t, node_t *y){
  node_t *x = y->left;
  y->left = x->right;
  if (x->right != t->nil) {
    x->right->parent = y;
  }
  x->parent = y->parent;
  if (y->parent == t->nil) {
    t->root = x;
  } else if (y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }
  x->right = y;
  y->parent = x;    
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while ( z->parent->color == RBTREE_RED ) { // 부모가 빨간색이면 계속 진행.
    if (z->parent == z->parent->parent->left){ //새 노드의 부모가 왼쪽 자식인지 -> 왼쪽으로 꺽이고 뻗음.
      node_t *uncle = z->parent->parent->right; 
      if (uncle->color == RBTREE_RED) { // 삼촌노드도 빨간색이면 (case1)
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; // 빨간색 된 조부모를 기준으로 다시 수정.
      } else {// 삼촌노드는 검은색일때
        if (z == z->parent->right) { // 내가 부모 오른쪽 자식이면 triangle (case2)
          z = z->parent; // rotate함수가 부모가 기준이라서 z를 부모로 바꾼거임. -> 그럼 회전하면 z가 아래로 내려옴.
          left_rotate(t, z); // case2 를 case3으로 만듬.
        } // 여기부터는 case3해결!! // 여기서 지금 z가 맨 아래로 가있는 상황임.
        z->parent->color = RBTREE_BLACK; // 따라서 z의 부모는 중간 노드
        z->parent->parent->color = RBTREE_RED; // z의 조부모는 꼭대기 노드
        right_rotate(t, z->parent->parent); // 조부모를 기준으로 오른쪽으로 돌리기.
      }
    } else { // 부모가 오른쪽 자식인지. 그럼 오른쪽으로 꺾이고 오른쪽으로 뻗음.
      node_t *uncle = z->parent->parent->left;
      if (uncle->color == RBTREE_RED) { // 삼촌노드도 빨간색이면 (case1)
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          right_rotate(t,z);
        } 
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *new_node = (node_t*)calloc(1, sizeof(node_t));
  new_node->key = key;
  new_node->color = RBTREE_RED;
  new_node->left = new_node->right = t->nil;

  node_t *new_parent = t->nil; //처음엔 아무것도 가리킬게 없음.
  node_t *now = t->root; // 처음 시작은 루트를 가리킴.

  while (now != t->nil) {  // 현재가 닐노드가 될때까지 계속 진행
    new_parent = now; // 진행한다는건 부모가 현재노드가 되고 현재노드가 크기에 따라 왼쪽자식,오른쪽자식으로 감.
    if (new_node->key < now->key) {
      now = now->left;
    } else {
      now = now->right;
    }
  }
  // 윗부분은 진행하기만 하고 아랫부분은 이제 진행을 마치고 마지막 부모노드 왼쪽자식으로 갈지 오른쪽자식으로 갈지 갱신하는 거임.
  new_node->parent = new_parent; // now가 닐 노드까지 갔으면 그전 노드인 new_parent가 새노드의 부모라고 갱신해주면 됨. 
  if (new_parent == t->nil) { //만약 계속 아무것도 가리키는게 없다면
    t->root = new_node; // 아무것도 없는거니까 새로운 노드가 뉴노드가 됨.
  } else if (new_node->key < new_parent->key) { // 뉴노드의 키가 부모키보다 작으면 왼쪽으로 감.
    new_parent->left = new_node; //부모 정보도 갱신 해줌
  } else {
    new_parent->right = new_node;
  }
  rbtree_insert_fixup(t, new_node); // 이제 수정해줘야함.

  return new_node;
}


node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *find_node = (node_t*)calloc(1, sizeof(node_t));
  find_node->key = key;
  find_node->left = find_node->right = t->nil;

  node_t *now = t->root; // 처음 시작은 루트를 가리킴.

  while (now != t->nil) {  // 현재가 닐노드가 될때까지 계속 진행
    if (find_node->key < now->key) {
      now = now->left;
    } else if (find_node->key > now->key){
      now = now->right;
    } else {
      find_node = now;
      return find_node;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *min_node = t->root; // 처음 시작은 루트를 가리킴.

  while (min_node->left != t->nil) {  // 현재가 닐노드가 될때까지 계속 진행
    min_node = min_node->left;
  }
  return min_node;
}

node_t *tree_minimum(rbtree *t, node_t *p) {
  node_t *min_node = p; 
  
  while (min_node->left != t->nil) {  // 현재가 닐노드가 될때까지 계속 진행
      min_node = min_node->left;
  }
  
  return min_node;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *max_node = t->root; // 처음 시작은 루트를 가리킴.

  while (max_node->right != t->nil) {  // 현재가 닐노드가 될때까지 계속 진행
    max_node = max_node->right;
  }
  return max_node;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v) { // 노드 u를 v로 대체하는 함수, 부모의 자식 정보도 갱신 해줘야해서 u = v로 끝나지 않음.
  if (u->parent == t->nil) { // u가 루트노드면
    t->root = v; // v가 루트노드가 됨.
  } else if (u == u->parent->left) { // 
    u->parent->left = v; 
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}
void rbtree_erase_fixup(rbtree *t, node_t *p) {
  node_t *brother; // 형제 노드
  while (p != t->root && p->color == RBTREE_BLACK) {
    if (p == p->parent->left) { // 왼쪽 자식인지
      brother = p->parent->right;
      if (brother->color == RBTREE_RED) { // case 1 
        brother->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        left_rotate(t, p->parent);
        brother = p->parent->right; // 새로운 case로 변경
      } 
      if (brother->left->color == RBTREE_BLACK && brother->right->color == RBTREE_BLACK) { // case 2
        brother->color = RBTREE_RED;
        p = p->parent;
      } else {
        if (brother->right->color == RBTREE_BLACK){ // case 3
          brother->left->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;
          right_rotate(t, brother);
          brother = p->parent->right;
        } 
        brother->color = p->parent->color; // case 4
        p->parent->color = RBTREE_BLACK;
        brother->right->color = RBTREE_BLACK;
        left_rotate(t, p->parent);
        p = t->root;
      }

    } else {
      brother = p->parent->left;
      if (brother->color == RBTREE_RED) {
        p->parent->color = RBTREE_RED;
        brother->color = RBTREE_BLACK;
        right_rotate(t, p->parent);
        brother = p->parent->left;
      }
      if (brother->left->color == RBTREE_BLACK && brother->right->color == RBTREE_BLACK ) {
        brother->color = RBTREE_RED;
        p = p->parent;
      } else { 
        if (brother->left->color == RBTREE_BLACK) { //case 2
          brother->right->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;
          left_rotate(t, brother);
          brother = p->parent->left;
        }
        brother->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        brother->left->color = RBTREE_BLACK;
        right_rotate(t, p->parent);
        p = t->root;
      }
    } 
  }
  p->color = RBTREE_BLACK;
}
int rbtree_erase(rbtree *t, node_t *p) {
  node_t* deleted_node = p; // 없어질 노드 , 처음에는 p로 설정
  node_t* replace; // 삭제 되고 그자리를 대신할 노드
  color_t deleted_node_color = deleted_node->color; // 삭제될 노드의 색
  // 자식이 하나이하일때 노드 이동중
  if (p->left == t->nil) { // p의 왼쪽 자식이 없으면 , 오른쪽 자식만 있거나, 아예없거나
    replace = p->right; // 만약 자식이 없었으면 nil을 가리킬듯
    rbtree_transplant(t, p, replace); // p를 오른쪽 자식으로 대체
  } else if (p->right == t->nil) { // p 의 오른쪽 자식이 없으면
    replace = p->left;
    rbtree_transplant(t, p, replace); // p를 왼쪽 자식으로 대체
  } else { // 자식 노드가 둘인 경우  
    deleted_node = tree_minimum(t, p->right); //없어질 노드는 오른쪽에서 가장 작은수
    deleted_node_color = deleted_node->color; // 오른쪽에서 가장 작은수의 색으로 갱신.
    replace = deleted_node->right; // 오가수의 왼쪽 자식은 없을 테니까 오른쪽 자식으로 대체
    if (deleted_node != p->right) { // deleted_node가 p의 바로 오른쪽 자식이 아닌지
      rbtree_transplant(t, deleted_node, replace); // 삭제된 노드 오른쪽 자식이 대체해주고
      deleted_node->right = p->right; // 삭제된 노드가 이제 p로 대체 됐으니까
      deleted_node->right->parent = deleted_node; // 정보 갱신한 거임. 
    } else { // 삭제 노드가 바로 오른쪽 자식이라면
      replace->parent = deleted_node;
    }  
    rbtree_transplant(t, p, deleted_node); // 색깔이나 자실노드를 설정하지 않아줘서 아래 3줄이 필요하다.
    deleted_node->left = p->left; // p의 왼쪽 자식을 del에 부여한다.
    deleted_node->left->parent = deleted_node; // p->left->parent = deleted_node와 같다.
    deleted_node->color = p->color;
    }
  if (deleted_node_color == RBTREE_BLACK) {
    rbtree_erase_fixup(t, replace);
  }
  free(p);
  return 0;
}
int rbtree_LMR_array(const rbtree *t, node_t *node, key_t *arr, int i, const size_t n){ // 중위 순회로 하면 오름차순으로 정렬되어 배열에 저장됨.
  if (node != t->nil && i < n){ // 왼쪽이 없을때까지 계속 갔다가 
    i = rbtree_LMR_array(t, node->left, arr, i, n); // 왼쪽 갈수있을 때까지 감. 더이상 없으면 아랫문장으로 감.
    arr[i] = node->key;
    i += 1;
    i = rbtree_LMR_array(t, node->right, arr, i, n); // 오른쪽 있으면 가고 없으면 인덱스 배출
  }
  return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) { // 저장된 노드의 개수를 반환 하는 함수.
  if (t->root == t->nil){
    return 0;
  }
  int count = rbtree_LMR_array(t, t->root, arr, 0, n);
  return count;
}
