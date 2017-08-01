#include <cstdio>
#include <cmath>
#include <algorithm>
#include<iostream>
#include "Utility.h"

using namespace std;

bool read_data(
	int n,
	int d,
	float** &data,
	const char* file_name)
{
	FILE* fin = fopen(file_name, "r");
	if (!fin) {
		cout << "file doesn't exist" << endl;
		return false;
	}
	
	int id;
	data = new float*[n];
	for (int i = 0; i < n; i++) {
		data[i] = new float[d+1];//补家最后一位储存id，不在第一位加，以防止出错
		fscanf(fin, "%d", &id);
		int j = 0;
		for (j = 0; j < d; j++) {
			fscanf(fin, "%f", &data[i][j]);
		}
		data[i][j] = id;//补家最后一位储存id，变为了浮点数
	}

	cout << "Finish reading file" << endl;
	fclose(fin);

	return true;
}


