#include "pch.h"
#include "RBTree.h"



static struct rbnode NullNode = { 1, };

static struct rbnode* find_replacement(struct rbnode* pTarget)
{
	if (pTarget->lft == &NullNode)
		return &NullNode;

	for (pTarget = pTarget->lft; pTarget->rgt != &NullNode; pTarget = pTarget->rgt);

	return pTarget;
}

static void transplant(struct rbtree* pTree, struct rbnode* dst, struct rbnode* src)
{
	if (rb_get_parent(dst) == &NullNode)
		pTree->pRoot = src;
	else if (dst == rb_get_parent(dst)->lft)
		rb_get_parent(dst)->lft = src;
	else
		rb_get_parent(dst)->rgt = src;

	if (src != &NullNode)
		rb_set_parent(src, rb_get_parent(dst));
}

static void right_rotate(struct rbtree* pTree, struct rbnode* pTarget)
{
	struct rbnode* lft = pTarget->lft;
	struct rbnode* parent = rb_get_parent(pTarget);

	pTarget->lft = lft->rgt;
	if (lft->rgt != &NullNode)
		rb_set_parent(lft->rgt, pTarget);

	rb_set_parent(lft, rb_get_parent(pTarget));
	if (parent == &NullNode)
		pTree->pRoot = lft;
	else if (parent->lft == pTarget)
		parent->lft = lft;
	else
		parent->rgt = lft;

	lft->rgt = pTarget;
	rb_set_parent(pTarget, lft);

	if (rb_get_parent(lft) == &NullNode)
		pTree->pRoot = lft;
}

static void left_rotate(struct rbtree* pTree, struct rbnode* pTarget)
{
	struct rbnode* rgt = pTarget->rgt;
	struct rbnode* parent = rb_get_parent(pTarget);

	pTarget->rgt = rgt->lft;
	if (rgt->lft != &NullNode)
		rb_set_parent(rgt->lft, pTarget);

	rb_set_parent(rgt, rb_get_parent(pTarget));
	if (parent == &NullNode)
		pTree->pRoot = rgt;
	else if (parent->lft == pTarget)
		parent->lft = rgt;
	else
		parent->rgt = rgt;

	rgt->lft = pTarget;
	rb_set_parent(pTarget, rgt);

	if (rb_get_parent(rgt) == &NullNode)
		pTree->pRoot = rgt;
}

void insert_fixup(struct rbtree* pTree, struct rbnode* pNewNode)
{
	struct rbnode* uncle;

	while (rb_get_color(rb_get_parent(pNewNode)) == RED)
	{
		if (rb_get_parent(pNewNode) == rb_get_parent(rb_get_parent(pNewNode))->lft)
		{
			uncle = rb_get_parent(rb_get_parent(pNewNode))->rgt;
			if (rb_get_color(uncle) == RED)
			{
				rb_set_color(rb_get_parent(pNewNode), BLACK);
				rb_set_color(uncle, BLACK);
				rb_set_color(rb_get_parent(rb_get_parent(pNewNode)), RED);
				pNewNode = rb_get_parent(rb_get_parent(pNewNode));
			}
			else
			{
				if (pNewNode == rb_get_parent(pNewNode)->rgt)
				{
					pNewNode = rb_get_parent(pNewNode);
					left_rotate(pTree, pNewNode);
				}
				rb_set_color(rb_get_parent(pNewNode), BLACK);
				rb_set_color(rb_get_parent(rb_get_parent(pNewNode)), RED);
				right_rotate(pTree, rb_get_parent(rb_get_parent(pNewNode)));
			}
		}
		else
		{
			uncle = rb_get_parent(rb_get_parent(pNewNode))->lft;
			if (rb_get_color(uncle) == RED)
			{
				rb_set_color(rb_get_parent(pNewNode), BLACK);
				rb_set_color(uncle, BLACK);
				rb_set_color(rb_get_parent(rb_get_parent(pNewNode)), RED);
				pNewNode = rb_get_parent(rb_get_parent(pNewNode));
			}
			else
			{
				if (pNewNode == rb_get_parent(pNewNode)->lft)
				{
					pNewNode = rb_get_parent(pNewNode);
					right_rotate(pTree, pNewNode);
				}
				rb_set_color(rb_get_parent(pNewNode), BLACK);
				rb_set_color(rb_get_parent(rb_get_parent(pNewNode)), RED);
				left_rotate(pTree, rb_get_parent(rb_get_parent(pNewNode)));
			}
		}
	}
	rb_set_color(pTree->pRoot, BLACK);
}

static void delete_fixup(struct rbtree* pTree, struct rbnode* pTarget)
{
	struct rbnode* sibling;
	while (pTarget != pTree->pRoot && rb_get_color(pTarget) == BLACK)
	{
		if (pTarget == rb_get_parent(pTarget)->lft)
		{
			sibling = rb_get_parent(pTarget)->rgt;
			if (rb_get_color(sibling) == RED)
			{
				rb_set_color(sibling, BLACK);
				rb_set_color(rb_get_parent(pTarget), RED);
				left_rotate(pTree, rb_get_parent(pTarget));
				sibling = rb_get_parent(pTarget)->rgt;
			}
			if (rb_get_color(sibling->lft) == BLACK && rb_get_color(sibling->rgt) == BLACK)
			{
				rb_set_color(sibling, RED);
				pTarget = rb_get_parent(pTarget);
			}
			else
			{
				if (rb_get_color(sibling->rgt) == BLACK)
				{
					rb_set_color(sibling->lft, BLACK);
					rb_set_color(sibling, RED);
					right_rotate(pTree, sibling);
					sibling = rb_get_parent(pTarget)->rgt;
				}
				rb_set_color(sibling, rb_get_color(rb_get_parent(pTarget)));
				rb_set_color(rb_get_parent(pTarget), BLACK);
				rb_set_color(sibling->rgt, BLACK);
				left_rotate(pTree, rb_get_parent(pTarget));
				pTarget = pTree->pRoot;
			}
		}
		else
		{
			sibling = rb_get_parent(pTarget)->lft;
			if (rb_get_color(sibling) == RED)
			{
				rb_set_color(sibling, BLACK);
				rb_set_color(rb_get_parent(pTarget), RED);
				right_rotate(pTree, rb_get_parent(pTarget));
				sibling = rb_get_parent(pTarget)->lft;
			}
			if (rb_get_color(sibling->rgt) == BLACK && rb_get_color(sibling->lft) == BLACK)
			{
				rb_set_color(sibling, RED);
				pTarget = rb_get_parent(pTarget);
			}
			else
			{
				if (rb_get_color(sibling->lft) == BLACK)
				{
					rb_set_color(sibling->rgt, BLACK);
					rb_set_color(sibling, RED);
					left_rotate(pTree, sibling);
					sibling = rb_get_parent(pTarget)->lft;
				}
				rb_set_color(sibling, rb_get_color(rb_get_parent(pTarget)));
				rb_set_color(rb_get_parent(pTarget), BLACK);
				rb_set_color(sibling->lft, BLACK);
				right_rotate(pTree, rb_get_parent(pTarget));
				pTarget = pTree->pRoot;
			}
		}
	}
	rb_set_color(pTarget, BLACK);
}

struct rbnode* rb_get_nil()
{
	return &NullNode;
}

struct rbnode* rb_delete(struct rbtree* pTree, struct rbnode* target, void (*copy)(void* target, void* replacement))
{
	struct rbnode* replacement, * subtree;

	if (!target)
		return target;

	if (target->lft == &NullNode)
		replacement = target;
	else
		replacement = find_replacement(target);

	if (replacement->lft != &NullNode)
		subtree = replacement->lft;
	else
		subtree = replacement->rgt;

	rb_set_parent(subtree, rb_get_parent(replacement));

	if (rb_get_parent(replacement) == &NullNode)
		pTree->pRoot = subtree;
	else
	{
		if (replacement == rb_get_parent(replacement)->lft)
			rb_get_parent(replacement)->lft = subtree;
		else
			rb_get_parent(replacement)->rgt = subtree;
	}

	if (replacement != target)
		copy(target, replacement);

	if (rb_get_color(replacement) == BLACK)
		delete_fixup(pTree, subtree);

	pTree->cnt--;
	return replacement;
}
