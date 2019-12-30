#pragma once

#define RED		0
#define BLACK	1

#define RBCAST(name)	((struct rbnode*)name)

#define rb_entry(ptr, type, member)	((type*)((char*)ptr - (unsigned long) & (((type*)0)->member)))

#define rb_get_parent(node)			((struct rbnode*)(((struct rbnode*)node)->pParentColor & ~3))
#define rb_set_parent(node, parent)	(((struct rbnode*)node)->pParentColor = (((struct rbnode*)node)->pParentColor & 3) | (unsigned long)(parent))

#define rb_get_color(node)			(((struct rbnode*)node)->pParentColor & 1)
#define rb_set_color(node, color)	(((struct rbnode*)node)->pParentColor = (((struct rbnode*)node)->pParentColor & ~1) | (color))
#define rb_flip_color(node)			(((struct rbnode*)node)->pParentColor = (((struct rbnode*)node)->pParentColor ^ 1))

#pragma pack(push, 4)
struct rbnode {
	unsigned long pParentColor;
	struct rbnode* lft;
	struct rbnode* rgt;
};
#pragma pack(pop)

struct rbtree {
	struct rbnode* pRoot;
	int cnt;
};

extern "C"
{
	struct rbnode* rb_get_nil();
	void rb_insert(struct rbtree* pTree, struct rbnode* pNewNode);
	void insert_fixup(struct rbtree* pTree, struct rbnode* pTarget);
	struct rbnode* rb_delete(struct rbtree* pTree, struct rbnode* target, void (*copy)(void* target, void* replacement));
	void* rb_search(struct rbtree* pTree, int data);
}
//
// static function list
//
//static struct rbnode* find_replacement(struct rbnode* pTree);
//static int transplant(struct rbtree* pTree, struct rbnode* dst, struct rbnode* src);
//static int right_rotate(struct rbtree* pTree, struct rbnode* pTarget);
//static int left_rotate(struct rbtree* pTree, struct rbnode* pTarget);
//static int delete_fixup(struct rbtree* pTree, struct rbnode* pTarget);
