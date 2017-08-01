#include"BallTreeNode.h"
#ifndef __BALL_TREE_H
#define __BALL_TREE_H

class BallTree {

private:
	BallTreeNode * root;//head
public:
	BallTree();
	~BallTree();
	BallTreeNode* getnode() {
		return root;
	}
	bool buildTree(
		int n,
		int d,
		float**&data);
	bool MakeBallTree(BallTreeNode*&ball, int n, int d, float **&data);
	bool MakeBallTreeSplit(float **&data, int n, int d, int &a, int &b);
	void print();

	bool storeTree(
		const char* index_path);

	bool restoreTree(
		const char* index_path);

	int mipSearch(
		int d,
		float* query);

	// optional
	bool insertData(
		int d,
		float* data);

	// optional
	bool deleteData(
		int d,
		float* data);

	// optional
	bool buildQuadTree(
		int n,
		int d,
		float** data);
};

#endif

