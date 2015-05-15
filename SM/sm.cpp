#include "sm.h"
#include "method.h"
#include <fstream>

int main()
{
	ifstream in("lon&lat.txt");
	float lat, lon;
	vector<pair<float, float>> vertexs;
	while (in >> lat >> lon)
	{
		pair<float, float> v(lon, lat);
		vertexs.push_back(v);
	}
	map<pair<int, int>, float> dists;
	get_dist(dists, vertexs);

	for (auto iter : dists)
	{
		if (iter.first.first < 2)
		cout << "(" << iter.first.first << "," << iter.first.second << ")" << " " << iter.second << endl;
	}

	system("pause");
}