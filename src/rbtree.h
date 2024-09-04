/*이 헤더파일의 역할은 rbtree의 구현을 위한 청사진 같은 역할을 함 
  트리와 관련된 여러가지 기능과 구조를 미리 정의 해두고 다른 코드에서 사용할 수 있게 해줌.
  노드가, 트리전체가 어떻게 생겼는지 정의 해줌.
  어떤 함수들이 쓰이게 될지 선언 해둠. 이렇게 하면 코드를 이해하기도 쉽고 유지 보수하기도 쉬움
*/ 
#ifndef _RBTREE_H_ // 헤더파일 중복 정의를 방지하기 위해 사용됨. 시작점
#define _RBTREE_H_ // 

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t; 
// 노드의 색을 나타내는 열거형 타입 정의

typedef int key_t; //트리에서 키 값을 나타내는 타입

typedef struct node_t { // 레드 블랙트리의 노드를 나타내는 구조체
  color_t color; // 노드 색
  key_t key; // 노드 키
  struct node_t *parent, *left, *right; // 부모노드, 왼쪽자식, 오른쪽 자식 가리키는 포인터
} node_t; // 이렇게 하면 struct 생략해도 됨.

typedef struct { // 레드 블랙트리를 나타내는 구조체
  node_t *root; // 트리 루트 노드 가리키는 포인터
  node_t *nil;  // for sentinel // NULL 역할, 트리의 끝을 나타내는 데 사용됨.
} rbtree;

rbtree *new_rbtree(void); // 새로운 트리 생성하는 함수
void delete_rbtree(rbtree *); // 트리 학제하고 메모리 해제하는 함수.

node_t *rbtree_insert(rbtree *, const key_t); // 트리에 주어진 키 값 삽입
node_t *rbtree_find(const rbtree *, const key_t); // 주어진 키 값 검색
node_t *rbtree_min(const rbtree *); // 가장 작은 키 값 반환
node_t *rbtree_max(const rbtree *); // 가장 큰 키 값 반환
int rbtree_erase(rbtree *, node_t *); // 특정 노드를 삭제하는 함수

int rbtree_to_array(const rbtree *, key_t *, const size_t); 
// 트리의 모든 키 값을 배열에 담아 반환

#endif  // _RBTREE_H_ //얘가 끝점
