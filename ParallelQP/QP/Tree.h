/*
 * Tree.h
 *
 *  Created on: Jan 15, 2012
 *      Author: lyl
 */

#ifndef TREE_H_
#define TREE_H_

#include <cstdlib>

#define sz(t) (t==NULL?0:t->s)
using namespace std;

struct Node {
	int key, s;
	Node *left, *right;

	Node(int a):s(1), left(NULL), right(NULL), key(a) { }

	~Node() {
		delete left;
		delete right;
	}
};

struct Tree {
	Node* root;

	Tree();
	~Tree();

	int cmp(Node *a, Node *b);
	void update(Node *t);
	void rr(Node *&t, Node *k);
	void lr(Node *&t, Node *k);
	Node findmin(Node *t);
	void insert(Node *&t, Node *k);
	void remove(Node *&t, Node *k);
	int select(Node *t, int k);

	void insert(int val);
	void remove(int val);
	int select(int k);

	void clear();
};

#endif /* TREE_H_ */
