#include "method.h"
#define M_PI 3.14159265358979323846
const float R = 6371; //radius of the Earth(km)

void get_dist(map<pair<int, int>, float>& dists, const vector<pair<float, float>>& vertexs)
{
	for (unsigned i = 0; i < vertexs.size(); ++i)
	{
		for (unsigned j = 0; j < vertexs.size(); ++j)
		{
			double lon1 = vertexs[i].first* M_PI/ 180;
			double lon2 = vertexs[j].first* M_PI / 180;
			double lat1 = vertexs[i].second* M_PI / 180;
			double lat2 = vertexs[j].second* M_PI / 180;
			double d = R * acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(lon2-lon1));
			pair<int, int> edge(i, j);
			dists[edge] = d;
		}
	}
}
