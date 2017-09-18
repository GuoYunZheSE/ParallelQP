/*
 * Tree.cpp
 *
 *  Created on: Jan 15, 2012
 *      Author: lyl
 */

#include "Tree.h"

Tree::Tree() {
	root = NULL;
}

Tree::~Tree() {
	delete root;
}

int Tree::cmp(Node *a, Node *b) {
	return a->key - b->key;
}

void Tree::update(Node *t) {
	if (t)
		t->s = sz(t->left) + sz(t->right) + 1;
}

void Tree::rr(Node *&t, Node *k) {
	t->left = k->right;
	k->right = t;
	update(t);
	update(k);
	t = k;
}

void Tree::lr(Node *&t, Node *k) {
	t->right = k->left;
	k->left = t;
	update(t);
	update(k);
	t = k;
}

void Tree::insert(Node *&t, Node *k) {
	if (t == NULL)
		t = k;
	else if (cmp(k, t) < 0) {
		insert(t->left, k);
		if (sz(t->left->left) > sz(t->right))
			rr(t, t->left);
	} else {
		insert(t->right, k);
		if (sz(t->right->right) > sz(t->left))
			lr(t, t->right);
	}
	update(t);
}

Node Tree::findmin(Node *t) {
	while (t->left != NULL)
		t = t->left;
	return *t;
}

void Tree::remove(Node *&t, Node *k) {
	if (t == NULL)
		return;
	if (cmp(k, t) == 0)
		if (t->right == NULL) {
			k = t->left;
			t->left = NULL;
			delete t;
			t = k;
		} else {
			Node tmp = findmin(t->right);
			remove(t->right, &tmp);
			tmp.left = t->left;
			tmp.right = t->right;
			*t = tmp;
			tmp.left = tmp.right = NULL;
		}
	else if (cmp(k, t) < 0)
		remove(t->left, k);
	else
		remove(t->right, k);
	update(t);
}

int Tree::select(Node *t, int k) {
	if (t == NULL)
		return -1;
	if (sz(t) == 1)
		return t->key;
	if (sz(t->left) >= k)
		return select(t->left, k);
	else
		return select(t->right, k - sz(t->left));
}

void Tree::insert(int val) {
	insert(root, new Node(val));
}

void Tree::remove(int val) {
	Node tmp = Node(val);
	remove(root, &tmp);
}

int Tree::select(int k) {
	return select(root, k);
}

void Tree::clear() {
	delete root;
	root = NULL;
}
