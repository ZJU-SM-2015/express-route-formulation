#include "graph.h"

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
	int hz, nj;
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

		//find hangzhou and nanjing
		if (name == "杭州市")
			hz = i;
		if (name == "南京市")
			nj = i;

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
	
	express[{hz, nj}] == 1; //the amount of express from hz to nj is one car
	for (unsigned i = 0; i < vertexs.size(); ++i)
	{
		for (unsigned j = 0; j < vertexs.size(); ++j)
		{
			if (i == j) continue;
			pair<int, int> edge1(i, j);
			pair<int, int> edge2(j, i);
			express[edge1] = city[i].weight * city[j].weight / (city[hz].weight * city[nj].weight);
			express[edge2] = city[i].weight * city[j].weight / (city[hz].weight * city[nj].weight);
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
	//cout << "new primer" << city[new_primer_id].name << endl; //test

}

void Graph::set_primer(float prime_radius)
{
	for (auto iter : city)
	{
		if (iter.prime == true)
		{
			//cout << iter.name << endl; //test
		}
		else
			iter.prime = false;
	}
	//cout << "******The cities above are first group of primers******" << endl; //test

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

void Graph::print_express()
{
	for (auto iter : express)
	{
		string c1 = city[iter.first.first].name;
		string c2 = city[iter.first.second].name;
		if (c1 == "杭州市") //test hangzhou
			cout << c1 << "--" << c2 << ":" << iter.second << endl;
	}
}

void Graph::route(int c1, int c2)
{
	
}

float Graph::cost()
{
	map<pair<int, int>, float> limit;		//time limit
	for (auto iter : dists)
	{
		int from = iter.first.first;
		int to = iter.first.second;
		float dist = iter.second;
		if (dist <= 400)
			limit[{from, to}] = 12;
		else if (dist <= 1200)
			limit[{from, to}] = 36;
		else if (dist <= 3000)
			limit[{from, to}] = 60;
		else
			limit[{from, to}] = 84;
	}

	vector<int> sub2pri;				//subprime city id to its closest prime city id
	for (unsigned i = 0; i < city.size(); ++i)
	{
		sub2pri.push_back(closest_primer(i));
	}

	map<int,set<int>> pri2sub;			//a prime city to its subprime cities
	for (unsigned i = 0; i < city.size(); ++i)
	{
		if (city[i].prime == true)
		{
			set<int> subs;
			for (unsigned j = 0; j < sub2pri.size(); ++j)
			{
				if (sub2pri[j] == i)
					subs.insert(j);
			}
			pri2sub.insert({ i, subs });
		}
	}

	map<int, float> assem_time;		//time when finish assembling express from its 

	for (auto iter : pri2sub)
	{
		int pri = iter.first;
		set<int> subs = iter.second;
		float max_dist = 0;
		for (auto sub : subs)
		{
			if (dists[{sub, pri}] > max_dist)
				max_dist = dists[{sub, pri}];
		}

		assem_time.insert({ pri, max_dist / (70*1.0) });
	}
	map<pair<int, int>,float> ass_sub2pri;
	for (unsigned i = 0; i < sub2pri.size(); ++i)
	{
		float sum = 0;
		for (unsigned j = 0; j < city.size(); ++j)
		{
			sum += express[{i, j}];
		}
		ass_sub2pri[{i, sub2pri[i]}] = sum ;
	}

	map<pair<int, int>, float> truck_sub2pri;
	for (unsigned i = 0; i < sub2pri.size(); ++i)
	{
		float sum = 0;
		for (unsigned j = 0; j < city.size(); ++j)
		{
			sum += express[{i, j}];
		}
		truck_sub2pri[{i, sub2pri[i]}] = sum;
	}

	map<pair<int, int>, float> time_cost;
	int cnt = 0;
	for (auto iter : express)
	{
		int from = iter.first.first;
		int to = iter.first.second;
		float exp = iter.second;

		int pri_of_from = sub2pri[from];
		int pri_of_to = sub2pri[to];
		float time = 0;
		float t1, t2, t3, t4;
		if (assem_time[pri_of_from] == 0)
			t1 = 0;
		else
			t1 = assem_time[pri_of_from] + 2; //if not in 19:00~24:00 or 0:00~7:00 ?
		t2 = dists[{pri_of_from, pri_of_to}] / 70;
		time += t1 + t2;
		int arrive_pri_of_to = (int)(19 + time + 0.5) % 24;
		if (arrive_pri_of_to >= 19 && arrive_pri_of_to <= 24 || arrive_pri_of_to >= 0 && arrive_pri_of_to <= 5)
			t3 = 2;
		else
			t3 = 2 + 12;

		t4 = dists[{pri_of_to,to}] / 70;
		time += t3 + t4;
		time_cost[{from, to}] = time;
		ofstream out("out.txt");
		if (time > limit[{from, to}])
		{
			std::cout << "Out of time limit : from " << city[from].name << " to " << city[to].name << endl;
			std::cout << "Time : " << time << " Limit : " << limit[{from, to}] << endl;
			std::cout << "Route : " << city[from].name << "->" << city[sub2pri[from]].name << "->" << city[sub2pri[to]].name << "->" << city[to].name << endl;
			std::cout << t1 << " " << t2 << " " << t3 << " " << t4 << endl;
			std::cout << endl;
			cnt++;
		}
		
	}
	cout << cnt << endl;
	map<pair<int, int>, float> exp_pri2pri; //express from one primer to another
	vector<int> sum_of_send;
	for (auto iter : express)
		exp_pri2pri.insert({ iter.first, 0 });
	for (auto iter : express)
	{
		int from = iter.first.first;
		int to = iter.first.second;
		float exp = iter.second;

		int pri_of_from = sub2pri[from];
		int pri_of_to = sub2pri[to];
		pair<int, int> pri2pri(pri_of_from, pri_of_to);
		exp_pri2pri[pri2pri] += exp;
	}

	map<pair<int, int>, int> truck_pri2pri; //truck from one primer to another
	for (auto iter : exp_pri2pri)
	{
		truck_pri2pri.insert({ iter.first, int(iter.second + 1) });
	}

	double cost = 0;
	for (unsigned i = 0; i < sub2pri.size();++i)
	{
		float sum = 0;
		for (unsigned j = 0; j < city.size(); ++j)
		{
			sum += express[{i, j}];
		}
		int truck2pri = int(sum + 1);
		cost += truck2pri * dists[{i, sub2pri[i]}];
	}

	for (auto iter : truck_pri2pri)
	{
		int from = iter.first.first;
		int to = iter.first.second;
		int truck = iter.second;
		cost += truck * dists[{from, to}];
	}

	for (unsigned i = 0; i < sub2pri.size(); ++i)
	{
		float exp_pri2sub = 0;
		for (unsigned j = 0; j < city.size(); ++j)
		{
			exp_pri2sub += express[{j, i}];
		}
		int truck_pri2sub = int(exp_pri2sub + 1);
		cost += truck_pri2sub * dists[{i, sub2pri[i]}];
	}

	return cost;
}
