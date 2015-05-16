#include "city.h"

int City::get_id()
{
	return id;
}

float City::get_weight()
{
	return weight;
}

Graph::Graph(ifstream& file,float prime_weight)
{
	vector<pair<float, float>> vertexs;
	int i = 0;						//id
	string name;					//city name
	float lat, lon;					//latitude & longitude
	float weight;					//weight = GDP * population
	bool prime;						//whether it is a prime city
	int flag = file.is_open();
	string line;
	while (getline(file,line))
	{
		stringstream ss;
		ss << line;
		ss >> name >> lat >> lon >> weight;
		pair<float, float> v(lat, lon);
		vertexs.push_back(v);
		if (weight > prime_weight)
			prime = true;
		else
			prime = false;
		City c(i,name, v, weight,prime);
		city.push_back(c);

		i++;
	}
	//construct distances set
	//dists[{i,j}] means distance between city with id i and city with id j
	for (unsigned i = 0; i < vertexs.size(); ++i)
	{
		for (unsigned j = 0; j < vertexs.size(); ++j)
		{
			pair<int, int> edge(i, j);
			dists[edge] = dist(vertexs[i], vertexs[j]);
		}
	}
}

float Graph::dist(const pair<float, float>& v1, const pair<float, float>& v2)
{
	double lon1 = v1.second* M_PI / 180;
	double lon2 = v2.second* M_PI / 180;
	double lat1 = v1.first* M_PI / 180;
	double lat2 = v2.first* M_PI / 180;
	float d = R * acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(lon2 - lon1));
	return d;
}

//choose the city with less latitude
bool less_lat(const City& c1, const City& c2)
{
	return c1.coor.first <= c2.coor.first;
}

float cross_product(pair<float, float> v1, pair<float, float> v2)
{
	return v1.first*v2.second - v1.second*v2.first;
}

int Graph::closest_primer(int target)
{
	float min_dist = INF;
	int cp_id;
	for (unsigned i = 0; i < city.size();++i)
	{
		if (city[i].prime==true && dists[{i,target}] < min_dist )
		{
			cp_id = i;
			min_dist = dists[{i, target}];
		}
	}
	return cp_id;
}

void Graph::set_primer_between(int id, int primer_id, float prime_radius)
{
	float min_dist_to_prime = INF;
	int new_primer_id;
	for (unsigned i = 0; i < city.size(); ++i)
	{
		if (dists[{i,id}] < prime_radius && city[i].prime == false)
		{
			if (dists[{i,primer_id}] < min_dist_to_prime)
			{
				min_dist_to_prime = dists[{i, primer_id}];
				new_primer_id = i;
			}
		}
	}
	if (min_dist_to_prime == INF) //test
	{
		cout << "no appropriate city is found for" << city[id].name << "and" << city[primer_id].name << endl;
		return;
	}

	city[new_primer_id].prime = true;
	cout << "new primer" << city[new_primer_id].name << endl; //test

}

void Graph::set_primer(float prime_radius)
{
	for (auto iter : city)
	{
		if (iter.prime == true)
		{
			cout << iter.name << endl; //test
		}
		else
			iter.prime = false;
	}
	cout << "******The cities above are first group of primers******" << endl; //test

	auto start = min_element(city.begin(), city.end(), less_lat);
	
	//record that whether the city has been selected
	vector<bool> selected;
	for (unsigned i = 0; i < city.size(); ++i)
	{
		selected.push_back(false);
	}

	unsigned iter = start->id;
	selected[iter] = true;

	while (1)
	{
		if (count(selected.begin(), selected.end(), true) == 57)
		{
			break;
		}

		int cp_id = closest_primer(iter);
		//if the distance between it and the cloest primer is greater than prime_radius, set a primer between them
		if (dists[{iter, cp_id }] > prime_radius)
		{
			set_primer_between(iter, cp_id, prime_radius);
		}

		//closure iteration
		int leftest_id;
		for (unsigned i = 0; i < selected.size(); ++i)
		{
			if (selected[i] == false)
			{
				leftest_id = i;
				break;
			}
		}

		for (unsigned i = 0; i < city.size(); ++i)
		{
			if (city[i].prime == true)
				continue;
			if (selected[i] == false)
			{
				pair<float, float> leftest_coor = city[leftest_id].coor;
				pair<float, float> this_coor = city[i].coor;
				pair<float, float> iter_coor = city[iter].coor;
				pair<float, float> leftest2iter = { leftest_coor.second - iter_coor.second, leftest_coor.first - iter_coor.first };
				pair<float, float> this2iter = { this_coor.second - iter_coor.second, this_coor.first - iter_coor.first };
				float ret = cross_product(this2iter, leftest2iter);
				if (ret < 0)
				{
					leftest_id = i;
					//cout << city[i].name << endl;
				}
			}
		}

		selected[leftest_id] = true;
		iter = leftest_id;
		//cout << "next one :"<< city[iter].name << endl; //test
	}


}

void Graph::print_primers()
{
	for (auto iter : city)
	{
		if (iter.prime == true)
			cout << iter.name << endl;
	}
}