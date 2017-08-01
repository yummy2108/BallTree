#include"math.h"
#include<iostream>
using namespace std;
struct rid {
//	private:
public:
		int page;
		int slot;
		public:
		rid(int _page = -1,int _slot = -1) {
			page = _page;
			slot = _slot;
			}
		void set_rid(int _page,int _slot) {
			page = _page;
			slot = _slot;
			}

};

struct Query{
	float index;
	float maxproduct;
	Query(float in = -1, float max = -1){
		index = in;
		maxproduct = max;
	}
	void set(float in, float max){
		index = in;
		maxproduct = max;
	}
};

 
struct BallTreeNode
{
	//建树过程中每个节点存的值
	float **data;/*当前Ball里面的数据*/
	float *center;/*Ball的中心*/
	float radius;/*半径*/
	int D;/*当前Ball总共有多少条数据 必须要小于N0*/ //用来判断是否为叶子节点 
	int d;/*每条数据有多长*/
	int node_num;//节点编号，用于球节点该存的页号，每个叶存的数据个数：slot占用字节：sizeof(float)* 14; PAGE_SIZE/sizeof(float)/(sizeof(float)* (14-3))
	BallTreeNode*left;
	BallTreeNode*right;
	BallTreeNode();
	~BallTreeNode();
	BallTreeNode(float **&_data, int _D/*n*/, int _d);
	void getcenter();
	void getradius();

	rid left_rid,right_rid, list_rid;//左右孩子的索引rid和索引子树的才有效的list_rid(指向球里面的所有点的对应的rid的的slot（假定为20个rid))

};

BallTreeNode::BallTreeNode() {
	data = 0;
	center = 0;
	radius = 0;
	D = 0;
	d = 0;
	left = 0;
	right = 0;
	list_rid.page=-1;
	list_rid.slot=-1;
}
inline BallTreeNode::~BallTreeNode()//释放错误
{
//	cout<<"delete"<<endl;//查看用
	delete[]data;
}

BallTreeNode::BallTreeNode(float **&_data, int _D, int _d) {
		data = new float*[_D];//n
	for (int i = 0; i < _D; i++) {
		data[i] = new float[_d+1];//增加了1
		for (int j = 0; j < _d+1; j++) {
			data[i][j] = _data[i][j];
		}
	}
	D = _D;
	d = _d;
	//增加一个至零
	left = 0;
	right = 0;
	getcenter();
	getradius();
}

void BallTreeNode::getcenter() {
	center = new float[d];
	for (int i = 0; i < d; i++) {
		center[i] = 0;
	}
	for (int i = 0; i < d; i++) {
		for (int j = 0; j < D; j++) {
			center[i] += data[j][i];
			//center[i] += data[i][j];错误 
		}
	}
	for (int i = 0; i < d; i++) {
		center[i] /= D;
	}
}

void BallTreeNode::getradius() {
	float max = 0;
	float temp = 0;
	for (int i = 0; i < D; i++) {
		for (int j = 0; j < d; j++) {
			temp += (data[i][j] - center[j])*(data[i][j] - center[j]);
		}
		if (temp > max) {
			max = temp;
		}
		temp = 0;//回0；啊啊啊啊啊抓狂
	}
	radius = sqrt(max);

}


float distance(float *a, float *b, int _d) {
	float res = 0.0;
	for (int i = 0; i < _d; i++) {
		res += (a[i] - b[i])*(a[i] - b[i]);
	}
	return sqrt(res);
}



