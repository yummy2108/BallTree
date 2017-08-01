#include "BallTree.h"
#include "Utility.h"
#include<iostream>
using namespace std;

BallTree::BallTree()
{
	root = 0;
}

BallTree::~BallTree()
{
	//root = 0;错误
	delete root;
}

bool BallTree::buildTree(int n, int d, float **&data)
{
	MakeBallTree(root, n, d, data);
	return false;
}


bool BallTree::MakeBallTree(BallTreeNode *& ball, int n, int d, float **&data)
{
	
	ball = new BallTreeNode(data, n, d);//在哪里实现的？
	if (n < N0) {
		return true;
	}
	int Lnum = 0;/*数据集left中元素的个数*/
	int Rnum = 0;/*数据集right元素的个数*/
	int Aindex = 0;
	int Bindex = 0;//随机找一个点，然后……两个相离最远的点
	MakeBallTreeSplit(data, n, d, Aindex, Bindex);
//	cout<<"Aindex: "<<Aindex<<" ";//;<<endl;
//	cout<<"Bindex: "<<Bindex<<endl;
	float **left;
	float **right;

	left = new float*[n];
	right = new float*[n];
	for (int i = 0; i < n; ++i) {

		left[i] = new float[d+1];//增加id

		right[i] = new float[d+1];
	}

	for (int i = 0; i < n; ++i) {

		float distanceA = distance(data[i], data[Aindex], d);//在哪里实现的？
		float distanceB = distance(data[i], data[Bindex], d);

		if (distanceA < distanceB) {

			for (int j = 0; j < d+1; j++) {//加一
				left[Lnum][j] = data[i][j];
			}
			Lnum++;
		}
		else {
			for (int j = 0; j < d+1; j++) {
				right[Rnum][j] = data[i][j];
			}
			Rnum++;
		}

	}



	MakeBallTree(ball->left, Lnum, d, left);
	MakeBallTree(ball->right, Rnum, d, right);
}

bool BallTree::MakeBallTreeSplit(float **&data,int n, int d, int & a, int & b)
{

	int x = 0;
	int A = 0;
	int B = 0;
	float max = 0;
	for (int i = 0; i < n; i++) {
		float res = distance(data[i], data[x], d);
		if (res > max) {
			max = res;
			A = i;
		}
	}

	max = 0;
	for (int i = 0; i < n; i++) {
		float res = distance(data[i], data[A], d);
		if (res > max) {
			max = res;
			B = i;
		}
	}
	a = A;
	b = B;
	return true;
}

void preorder_setnum(BallTreeNode* &root,int & num){//
	if (root == 0) {

		return;

	}
	root->node_num = num;                                                                                                                                                         //node_num
	num++;
	preorder_setnum(root->left,num);
	preorder_setnum(root->right,num);
}

void setnode_num(BallTreeNode* &root) {//
	int num=0;
	 preorder_setnum(root,num); // 前序设置node_num 
}

void set_page_slot(BallTreeNode* &root) {//
	if(root->left== 0 && root->right==0) {//若是根 返回 
		root->left_rid.page = -1;
		root->left_rid.slot = -1;
		root->right_rid.page = -1;
		root->right_rid.slot = -1;
		return;
	}
	//cout<<root->node_num<<" "; 
	if(root->left != 0) {//root->D>N0
		root->left_rid.page = ((int)((root->left)->node_num))/(int)(PAGE_SIZE/((root->d+10)*sizeof(float)));//强制，因为要取余数 
		root->left_rid.slot = ((int)((root->left)->node_num))%(int)(PAGE_SIZE/((root->d+10)*sizeof(float)));//强制，因为要取余数 
	}
	if(root->right != 0) {//root->D>N0
		root->right_rid.page = ((int)((root->right)->node_num))/(int)(PAGE_SIZE/((root->d+10)*sizeof(float)));//强制，因为要取余数 
		root->right_rid.slot = ((int)((root->right)->node_num))%(int)(PAGE_SIZE/((root->d+10)*sizeof(float)));//强制，因为要取余数 
	}
	set_page_slot(root->left);
	set_page_slot(root->right);
}

void find_max(BallTreeNode* &root,int &max) {//
	if (root == 0) {
		
		return;
	}
	if(root->node_num>max) {
		max = root->node_num;
	}
	find_max(root->left,max); 
	find_max(root->right,max); 
	
}

void write_apage(BallTreeNode* &root, float cache[PAGE_SIZE/sizeof(float)],int &slot_num,int &page_num) {
	if (root == 0) {
		
		return;
	}//((int)((root->right)->node_num))/(int)(PAGE_SIZE/((root->d+10)*sizeof(float)))
	if(((int)((root->node_num))/(int)(PAGE_SIZE/((root->d+10)*sizeof(float)))) == page_num) { // 所有节点，满足遍历存储在第a页的点 
		int k;
		k=0;
		slot_num = (int)(((root->node_num))%(int)(PAGE_SIZE/((root->d+10)*sizeof(float))));
		for(k=0;k<root->d;k++) // 写进去的数据的顺序 
		cache[slot_num*node_slot_size+k] = root->center[k]; 
		cache[slot_num*node_slot_size+0+k] = root->radius; 
		cache[slot_num*node_slot_size+1+k] = root->D; 
		cache[slot_num*node_slot_size+2+k] = root->d; 
		cache[slot_num*node_slot_size+3+k] = root->node_num; 
		cache[slot_num*node_slot_size+4+k] = root->left_rid.page; 
		cache[slot_num*node_slot_size+5+k] = root->left_rid.slot; 
		cache[slot_num*node_slot_size+6+k] = root->right_rid.page; 
		cache[slot_num*node_slot_size+7+k] = root->right_rid.slot; 
		cache[slot_num*node_slot_size+8+k] = root->list_rid.page; // 只有在叶子有效，指向20点大小的槽，list 
		cache[slot_num*node_slot_size+9+k] = root->list_rid.slot; 	
		//temcount++;		
//	} 
	 }
	 write_apage(root->left,  cache , slot_num, page_num);
	 write_apage(root->right,  cache , slot_num, page_num);
}


void w_n_to_index(BallTreeNode* &root, float cache[PAGE_SIZE/sizeof(float)],FILE* & index_bin) {
	
	char ch;//测试用 
	int max;//最大的节点编号
	find_max(root, max); // 找出最大的编号 
	int slot_num=0;//用于记录当前的节点的cache地址，结合 node_slot_size 使用 
	int a;
	for(a =0; a <= max/(int)(PAGE_SIZE/((root->d+10)*sizeof(float)));a++) {//a 为页号 
			slot_num=0;
		write_apage(root,cache,slot_num,a); // 写第a页到cache 
		
		fwrite(cache, sizeof(float), PAGE_SIZE/sizeof(float), index_bin); // 把第a页写进文件 
	} 
}

void write_index(BallTreeNode* &root,FILE* & index_bin) {
	char ch;
	setnode_num(root);//给每一个节点编号1.2.3.4……………………用于确认页号和slot 
//	cin>>ch;
	set_page_slot(root);//确定每和一个节点的左右子树要存的位置，页号 和 slot 
//	cin>>ch;
	float cache[PAGE_SIZE/sizeof(float)];
	for(int a=0;a<PAGE_SIZE/sizeof(float); a++) {
		cache[a]=0;
	}
	//cin>>ch;
	w_n_to_index(root,cache,index_bin); // 写进index.bin 。。。。。存的是每个节点的全部数据 
	
	return;
}

 
void write_data(FILE* & data_bin, BallTreeNode *&root, int & page_num, int &slot_num, float cache[PAGE_SIZE/sizeof(float)]){
	if(root==0) {
		return;
	}
	
	root->list_rid.page= -1;
	root->list_rid.slot = -1;
	
	//判断是否为叶子 
	if((root->left==0 && root->right==0)||root->D<=N0) {
		if((int)((root->d+1)*N0*(slot_num+1))<PAGE_SIZE/sizeof(float)) { // 判断当前缓存页是都足够存储当前叶子节点 
			int a,b;   
			a=0;
			b=0;
			for(a=0;a<root->D;a++){
				for(b=0;b<root->d+1;b++) {
					cache[(root->d+1)*N0*(slot_num)+a*(root->d+1)+b] = root->data[a][b];
				}
			}
			root->list_rid.page = page_num; // 将page slot  存进叶子的索引 
			root->list_rid.slot = slot_num;
			slot_num++;
		}
		else {
			fwrite(cache, sizeof(float), PAGE_SIZE/sizeof(float), data_bin); // 缓存满了，先将当前叶子写进内存，将cache写进文件，文件指针向下 
			slot_num=0; // slot重置 
			page_num++;
			if((int)((root->d+1)*N0*(slot_num+1))<PAGE_SIZE/sizeof(float)) {
				int a,b;
				a=0;
				b=0;
				for(a=0;a<root->D;a++){
					for(b=0;b<root->d+1;b++) {
						cache[(root->d+1)*N0*(slot_num)+a*(root->d+1)+b] = root->data[a][b];
					}
				}
				root->list_rid.page= page_num;
				root->list_rid.slot = slot_num;
				slot_num++;
			}
		}
	}
	write_data(data_bin, root->left, page_num, slot_num,cache); // 前序递归 
	write_data(data_bin, root->right, page_num, slot_num,cache);
}

bool BallTree::storeTree(const char * index_path)//文件路径
{	
//将树写进外存 
	char ch;
	int page_num=0;
	int slot_num=0;
	float cache[PAGE_SIZE/sizeof(float)]; // 缓存，一页有多少 float，写文件按照float格式，读出来也是 
	for(int a=0;a < PAGE_SIZE/sizeof(float); a++) { // 初始化 
		cache[a]=0;
	}
	
	FILE * data_bin = fopen("data.bin","wb" ); // 打开二进制文件 
	FILE * index_bin = fopen(index_path,"wb" ); 
	
	write_data(data_bin, root, page_num, slot_num,cache); // 按照前序遍历，从左到右，将叶子节点内数据写进data.bin 
	
	fwrite(cache, sizeof(float), PAGE_SIZE/sizeof(float), data_bin);//写最后一页 
	cout<<"write data success"<<endl;
	
	write_index(root,index_bin);//写索引 
	cout<<"write index success"<<endl;
	
	fclose(data_bin);
	fclose(index_bin);
	return false;
}

// 叶子节点 ，根据叶子节点的页号槽号在data.bin找出对应的数据读进内存 
void cin_data(BallTreeNode* &root){
	int a=0;
	int b=0;
	root->data = new float* [root->D]; // 申请指针 
		int pageoffeset = 0;
		int slotoffeset = 0;
		pageoffeset = root->list_rid.page*PAGE_SIZE;
		slotoffeset = root->list_rid.slot*dot_num*N0;//*sizeof(float);                                                                                    这N0 
		
		float cache[PAGE_SIZE/sizeof(float)];
		for(int k=0;k<PAGE_SIZE/sizeof(float); k++) {
			cache[k]=0;
		}
		FILE *data= fopen("data.bin","rb");
		
		fseek(data, pageoffeset, SEEK_SET);
		
		fread(cache, sizeof(float),PAGE_SIZE/sizeof(float),data);

		
		for( a=0; a<root->D; a++){
			root->data[a]=new float[root->d+1];

			for(b=0; b<root->d+1; b++) {
				root->data[a][b] = cache[slotoffeset+a*dot_num+b];
			}

		}
		fclose(data);
}


void prebulidtree_index(BallTreeNode* &root,FILE * &index_bin, float cache[PAGE_SIZE/sizeof(float)]) {
	if(root->D > N0) { // 非叶子节点 
		root->left = new BallTreeNode;
		int pageoffset;
		pageoffset = root->left_rid.page*PAGE_SIZE;// 
		int slot;
		slot=0;
		slot= root->left_rid.slot*ball_num_f; // 左孩子叶节点 * 60 
		
		fseek(index_bin, pageoffset, SEEK_SET); // 把文件指针指向页偏移地点 
		fread(cache,sizeof(float),PAGE_SIZE/sizeof(float),index_bin); // 读出对应的一页 
		
		int k;
			root->left->center= new float[root->d];  
	for(k=0;k<root->d;k++) {
		root->left->center[k]=cache[k+slot];
	}
		root->left->radius=cache[k+0+slot];
		root->left->D=cache[k+1+slot];
		root->left->d=cache[k+2+slot];
		root->left->node_num=cache[k+3+slot];
		root->left->left_rid.page=cache[k+4+slot];
		root->left->left_rid.slot=cache[k+5+slot];
		root->left->right_rid.page=cache[k+6+slot];
		root->left->right_rid.slot=cache[k+7+slot];
		root->left->list_rid.page =cache[k+8+slot];
		root->left->list_rid.slot=cache[k+9+slot];
		
		//right node
		root->right = new BallTreeNode;
		pageoffset = root->right_rid.page*PAGE_SIZE;//
		slot = 0;
		slot= root->right_rid.slot*ball_num_f;
		
		fseek(index_bin,pageoffset, SEEK_SET);
		
		fread(cache,sizeof(float),PAGE_SIZE/sizeof(float),index_bin);
			root->right->center= new float[root->d];
			
	for(k=0;k<root->d;k++) {
		root->right->center[k]=cache[k+slot];
	}
		root->right->radius=cache[k+0+slot];
		root->right->D=cache[k+1+slot];
		root->right->d=cache[k+2+slot];
		root->right->node_num=cache[k+3+slot];
		root->right->left_rid.page=cache[k+4+slot];
		root->right->left_rid.slot=cache[k+5+slot];
		root->right->right_rid.page=cache[k+6+slot];
		root->right->right_rid.slot=cache[k+7+slot];
		root->right->list_rid.page =cache[k+8+slot];
		root->right->list_rid.slot=cache[k+9+slot];
		
	//	prebulidtree_index(root->left,index_bin,cache);
	//	prebulidtree_index(root->right,index_bin,cache);
	}
	else {// if(root->D <= N0){ //&& root->list_rid.page!=-1  && root->list_rid.slot!= -1
		cin_data(root);
		return;
	}
	return;
}

// 将整棵树读出来 
bool BallTree::restoreTree(const char * index_path)
{
	FILE * index_bin;
	
	index_bin = fopen(index_path,"rb" );

	float cache[PAGE_SIZE/sizeof(float)];
	
	for(int a=0;a<PAGE_SIZE/sizeof(float); a++) {
		cache[a]=0;
	}
	
	fread(cache,sizeof(float),PAGE_SIZE/sizeof(float),index_bin); // 读出一页处 
	// 第一页第一个为根节点 
	BallTreeNode *head;
	head = new BallTreeNode;
	head->center= new float[demension];
	
	int k;
	for(k=0;k<demension;k++) {
		head->center[k] = cache[k];
	}
		head->radius=cache[k+0];
		head->D=cache[k+1];
		head->d=cache[k+2];
		head->node_num=cache[k+3];
		head->left_rid.page=cache[k+4];
		head->left_rid.slot=cache[k+5];
		head->right_rid.page=cache[k+6];
		head->right_rid.slot=cache[k+7];
		head->list_rid.page =cache[k+8];
		head->list_rid.slot=cache[k+9];
	root=head;
	//prebulidtree_index(root, index_bin, cache); // 递归把所有的孩子写进去 
	
	fclose(index_bin);
	return false;
}

float InnerProduct(float*a, float*b, int d){
	float result = 0;
	for(int i = 0; i < d; i++){
		result += a[i]*b[i];
	}
	return result;
}

float MaxMip(float * query, BallTreeNode*node, int d){
	float result = 0;
	result+= InnerProduct(query,node->center,d);
	float q = 0;
	for(int i = 0; i < d; i++){
		q += query[i]*query[i];
	}
	result += sqrt(q)*(node->radius);
	return result;
} 

void LinearSearch(float* query, BallTreeNode*node,int d,Query&QueryResult){
	for(int i = 0; i < node->D && node->data != 0; i++){
		float temp = InnerProduct(node->data[i],query,d);
		if(temp > QueryResult.maxproduct){
			float index = node->data[i][d];
			QueryResult.set(index,temp);
		}
	}
	return;
}
//prebulidtree_index(root->left,index_bin,cache);
void TreeSearch(float* &query, BallTreeNode* &node, int d, Query& QueryResult, float cache[PAGE_SIZE/sizeof(float)], FILE * &index_bin){
	char ch;
	prebulidtree_index(node,index_bin,cache);
	if(QueryResult.maxproduct < MaxMip(query,node,d)){
			if(node->D <= 20){
				LinearSearch(query,node,d,QueryResult);
				return;
			}
			float leftbound = MaxMip(query,node->left,d);
			float rightbound = MaxMip(query,node->right,d);
			if(leftbound < rightbound){
				TreeSearch(query,node->right,d,QueryResult,cache,index_bin);
				TreeSearch(query,node->left,d,QueryResult,cache,index_bin);
			}else{
				TreeSearch(query,node->left,d,QueryResult,cache,index_bin);
				TreeSearch(query,node->right,d,QueryResult,cache,index_bin);
			}
	}else{
		return;
	}
}

int BallTree::mipSearch(int d, float * query)
{
	FILE * index_bin;
	index_bin = fopen("Netflix\\index\\index.bin","rb" );
	float cache[PAGE_SIZE/sizeof(float)];
									
	for(int a=0;a<PAGE_SIZE/sizeof(float); a++) {
		cache[a]=0;
		}
		
	 Query QueryResult; 
	 TreeSearch(query,root,d,QueryResult,cache,index_bin);
	 fclose(index_bin);
	 return (int)QueryResult.index;
}

bool BallTree::insertData(int d, float * data)
{
	return false;
}

bool BallTree::deleteData(int d, float * data)
{
	return false;
}

bool BallTree::buildQuadTree(int n, int d, float ** data)
{
	return false;
}

void inorder(BallTreeNode *root,FILE* & fout){
	if(root==0) return;
	if(root->left==0 && root->right==0 &&root->data!=0) {
		for(int a=0;a<root->D;a++)  {
			fprintf(fout, "%f ", root->data[a][root->d]);
		for(int b=0;b<root->d;b++)
		fprintf(fout, "%f ", root->data[a][b]);
		fprintf(fout, "\n", "");
	}
	}
	inorder(root->left,fout);
	inorder(root->right,fout);
	
}

