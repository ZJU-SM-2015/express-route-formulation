#include "city.h"

#define M_PI 3.14159265358979323846
const float R = 6371; //radius of the Earth(km)

int City::get_id()
{
	return id;
}

int City::get_weight()
{
	return weight;
}

void City::set_prime()
{
	prime = true;
}

bool City::is_prime()
{
	return prime;
}

pair<float, float> City::get_coor()
{
	return coor;
}

Graph::Graph(ifstream file)
{
	vector<pair<float, float>> vertexs;
	float lat, lon;
	int i = 0;
	float weight;
	while (file >> lat >> lon >> weight)
	{
		pair<float, float> v(lat, lon);
		vertexs.push_back(v);
		City city(i, v, weight);
		citys.push_back(city);
	}

	for (unsigned i = 0; i < vertexs.size(); ++i)
	{
		for (unsigned j = 0; j < vertexs.size(); ++j)
		{
			pair<int, int> edge(i, j);
			dists[edge] = dist(vertexs[i], vertexs[j]);
		}
	}
}

double Graph::dist(const pair<float, float>& v1, const pair<float, float>& v2)
{
	double lon1 = v1.second* M_PI / 180;
	double lon2 = v2.second* M_PI / 180;
	double lat1 = v1.first* M_PI / 180;
	double lat2 = v2.first* M_PI / 180;
	double d = R * acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(lon2 - lon1));
	return d;
}

//choose the city with less latitude
bool less_lat(const City& c1, const City& c2)
{
	return c1.coor.first < c2.coor.first;
}

float cross_product(pair<float, float> v1, pair<float, float> v2)
{
	return v1.first*v2.second - v1.second*v2.first;
}

City Graph::closest_primer(const City& c)
{
	float min_dist = 0xffff;
	City closest_primer;
	for (auto iter : citys)
	{
		if (iter.is_prime() && dist(iter.get_coor(), c.get_coor()) < min_dist )
		{
			closest_primer = iter;
			min_dist = dist(iter.get_coor(), c.get_coor());
		}
	}
	return closest_primer;
}

void Graph::set_primer_between(const City& c, const City& primer, float prime_radius)
{
	float min_dist_to_prime = 0xffff;
	int new_primer_id;
	for (auto iter : citys)
	{
		if (dist(iter.get_coor(), c.get_coor()) < prime_radius)
		{
			if (dist(iter.get_coor(), primer.get_coor()) < min_dist_to_prime)
			{
				min_dist_to_prime = dist(iter.get_coor(), primer.get_coor());
				new_primer_id = iter.get_id();
			}
		}
	}

	citys[new_primer_id].set_prime();

}

void Graph::set_primer(float prime_weight,float prime_radius)
{
	for (auto iter : citys)
	{
		if (iter.get_weight() > prime_weight)
			iter.set_prime();
	}

	auto start = *min_element(citys.begin(), citys.end(), less_lat); // a method from <algorithm>

	vector<bool> selected;
	for (unsigned i = 0; i < citys.size(); ++i)
		selected[i] = false;

	auto select = start;
	selected[start.get_id()] = true;

	while (1)
	{
		City closest_p = closest_primer(select);
		//if the distance between it and the cloest primer is greater out of prime_radius
		if (dist(closest_p.get_coor(), select.get_coor()) > prime_radius) 
		{
			set_primer_between(closest_p, select, prime_radius);
		}

		//closure iteration
		float leftest = 0xffff;
		City leftest_city;
		
		for (unsigned i = 0; i < citys.size() && (selected[i]==false); ++i)
		{
			float ret = cross_product(select.get_coor(), citys[i].get_coor());
			if (ret < leftest)
			{
				leftest = ret;
				leftest_city = citys[i];
			}
		}
		if (leftest == 0xffff)
			break; //break if no leftest city any more
		selected[leftest_city.get_id()] = true;

		select = leftest_city;

	}


}