#include "graph.h"

bool operator != (const TP& tp1, const TP& tp2)
{
	return tp1.place != tp2.place || tp1.time != tp2.time;
}

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
	
	express[{hz, nj}] = 1; //the amount of express from hz to nj is one car
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

bool less_dist(pair<int, float> p1,pair<int, float> p2)
{
	return (p1.second < p2.second);
}




bool operator< (const Route& r1, const Route&r2){
	return r1.arrivetime < r2.arrivetime;
}

void Graph::get_limit(map<pair<int, int>, float>& limit)
{
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
}

void Graph::get_pri2sub(map<int, set<int>>& pri2sub, const vector<int>& sub2pri)
{
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
}
void Graph::get_sub2pri(vector<int>& sub2pri)
{
	for (unsigned i = 0; i < city.size(); ++i)
	{
		sub2pri.push_back(closest_primer(i));
	}
}

float Graph::cost(int &error)
{
	map<pair<int, int>, float> limit;		//time limit
	get_limit(limit);

	vector<int> sub2pri;				//subprime city id to its closest prime city id
	get_sub2pri(sub2pri);

	map<int,set<int>> pri2sub;			//a prime city to its subprime cities
	get_pri2sub(pri2sub, sub2pri);

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

	map<int, vector<Route>> routes_of_pri;
	map<pair<int, int>, float> time; // time for express from subprimer to primer
	map<pair<int, int>, vector<float>> time_vec;
	map<pair<int, int>, int> truck_between;
	for (auto iter:pri2sub)
	{
		int pri = iter.first;
		set<int> subs = iter.second;
		vector<int> sub_arrive;

		map<float,int> sub_dist;
		vector<float> dists_of_sub;
		map<pair<int, int>, float> exp_sub2pri;
		for (auto iter1 : subs)
		{
			sub_dist[dists[{iter1, pri}]] = iter1;
			dists_of_sub.push_back(dists[{iter1, pri}]);
		}
		//sort the distances, start from the closest subprimer
		sort(dists_of_sub.begin(),dists_of_sub.end());
		//initialize time
		for (auto iter : subs)
		{
			for (unsigned i = 0; i < city.size(); ++i)
			{
				if (city[i].prime == true)
					time.insert({ { iter, i }, 0 });
			}
		}
		//calculate sum of express from a subprime to a primer
		for (auto iter : dists_of_sub)
		{
			int sub = sub_dist[iter];
			for (unsigned i = 0; i < city.size(); ++i)
			{
				if (city[i].prime == true)
					exp_sub2pri.insert({ { sub, i }, 0 });
			}
			int pri_of_to;
			for (unsigned i = 0; i < city.size(); ++i)
			{
				pri_of_to = sub2pri[i];
				exp_sub2pri[{sub, pri_of_to}] += express[{sub, i}];    //express from a subprime to a far away primer
			}
		}

		map<int,Box> boxs;
		vector<Route> routes;
		
		map<int, float> exp_2pri;
		for (unsigned i = 0; i < city.size() && city[i].prime == true; ++i)
			exp_2pri.insert({i,0});
		for (auto iter : exp_sub2pri)
		{
			int sub = iter.first.first;
			int pri = iter.first.second;
			float exp = iter.second;
			exp_2pri[pri] += exp;

			//calculate cost
		}
		//start
		float t = 0; //record time
		
		for (unsigned i = 0; i < city.size() && city[i].prime == true; ++i)
		{
			truck_between.insert({ { pri, i }, 0 });
		}

		for (auto iter : dists_of_sub)
		{
			t = (iter / (70 * 1.0));
			int sub = sub_dist[iter];
			truck_between[{sub, pri}] = int(express[{sub,pri}]+1);

			for (unsigned i = 0; i < city.size(); ++i)
			{
				if (t >= 0.0001)
				{
					TP tp(t, city[pri].name);
					timetable[{sub, i}].push_back(tp);
				}
			}
			for (unsigned i = 0; i < city.size(); ++i)//an iteration through the boxs
			{
				if (city[i].prime == true)
				{
					for (auto j : boxs[i].subs)
					{
						time[{j, i}] = t;
					}
					
					boxs[i].contain += exp_sub2pri[{sub, i}];
					boxs[i].subs.insert(sub);
					if (boxs[i].contain >= 1 || exp_2pri[i] <= 1)	//       //if the express to another primer is sum up to over 1 truck
					{
						int truck = int(boxs[i].contain + 1);
						for (auto j : boxs[i].subs)
						{
							if (j == sub2pri[j])
							{
								time[{j, i}] += dists[{sub2pri[j], i}] / (70 * 1.0); //!!!consider pri2pri
								if (truck_between.find({ sub2pri[j], i })==truck_between.end())
									truck_between[{sub2pri[j], i}] = truck;

								for (auto iter : pri2sub[i])
								{
									TP tp(time[{j, i}], city[i].name);
									timetable[{j, iter}].push_back(tp);
								}

								time_vec[{j, i}].push_back(dists[{j, i}] / (70 * 1.0));
							}
							else
							{
								time[{j, i}] += dists[{sub2pri[j], i}] / (70 * 1.0) + 2; //!!!consider pri2pri
								if (truck_between.find({ sub2pri[j], i }) == truck_between.end())
									truck_between[{sub2pri[j], i}] = truck;

								for (auto iter : pri2sub[i])
								{
									TP tp(time[{j, i}], city[i].name);
									timetable[{j, iter}].push_back(tp);
								}

								time_vec[{j, i}].push_back(dists[{j, i}] / (70 * 1.0));
								time_vec[{j, i}].push_back(2);
							}
							
							if ((int(time[{j, i}] + 19) % 24 > 7) && (int(time[{j, i}] + 19) % 24 < 19))
							{
								float t = int(time[{j, i}] + 19) / 24 * 24 + 19;
								Route route(j, i, truck, time[{j, i}], boxs[i]);
								routes.push_back(route);
							}
							else
							{
								Route route(j, i, truck, time[{j, i}], boxs[i]);
								routes.push_back(route);
							}
						}
						exp_2pri[i] -= boxs[i].contain;
						boxs[i].contain = 0;               // the trucks can go off now
						boxs[i].subs.clear();
					}
				}
			}
		}
		//clear the boxs
		//for (unsigned i = 0; i < boxs.size(); ++i)
		for (auto iter:boxs)
		{
			int truck = int(iter.second.contain + 1);
			unsigned i = iter.first; //pri_of_to
			for (auto j : iter.second.subs)
			{
				time[{j, i}] += dists[{j, i}] / (70 * 1.0) + 2;
				if (truck_between.find({ sub2pri[j], i }) == truck_between.end())
					truck_between[{sub2pri[j], i}] = truck;

				for (auto iter : pri2sub[i])
				{
					TP tp(time[{j, i}], city[i].name);
					timetable[{j, iter}].push_back(tp);
				}

				time_vec[{j, i}].push_back(dists[{j, i}] / (70 * 1.0));
				time_vec[{j, i}].push_back(2);
				Route route(j, i, truck, time[{j, i}], boxs[i]);
				routes.push_back(route);
			}
			boxs[i].contain = 0;               // the trucks can go off now
			boxs[i].subs.clear();
		}

		routes_of_pri[pri] = routes;

	}// an iteration ends

	map<int, vector<Route>> routes_to_pri;
	for (auto iter : routes_of_pri)
	{
		vector<Route> routes = iter.second;
		for (unsigned i = 0; i < routes.size(); ++i)
		{
			int desti = routes[i].desti;
			routes_to_pri[desti].push_back(routes[i]);
		}
	}

	map<pair<int, int>, float> finish_time;
	for (auto iter : routes_to_pri)
	{
		map<int, Box> boxs;//boxs for its subprimer
		int pri_of_to = iter.first;
		vector<Route> routes = iter.second;
		set<int> dest = pri2sub[pri_of_to];
		sort(routes.begin(),routes.end()); //sort the routes
		float cleartime = 0;

		for (auto to:dest)
		{
			truck_between[{pri_of_to, to}] = 0;
		}
		for (unsigned i = 0; i < routes.size(); ++i)
		{
			Route r = routes[i];
			Box box = r.box;
			set<int> begins = box.subs;
			for (auto to : dest)
			{
				for (auto from : begins)
				{
					boxs[to].contain += express[{from, to}];
					boxs[to].subs.insert(from);
					if (to == pri_of_to)
					{
						if (finish_time.find({from,to})==finish_time.end())
							finish_time[{from, to}] = r.arrivetime;

						//truck_between[{pri_of_to, to}] += truck; //
						time_vec[{from, to}].push_back(0);
						boxs[to].contain = 0;
						boxs[to].subs.clear();
					}
					
				}
				if (boxs[to].contain >= 1)
				{
					set<int> set_of_from = boxs[to].subs;
					for (auto from : set_of_from)
					{
						if ( to == sub2pri[to])
						{
							if (finish_time.find({ from, to }) == finish_time.end())
								finish_time[{from, to}] = r.arrivetime + dists[{pri_of_to, to}] / 70;
							
							time_vec[{from, to}].push_back(dists[{pri_of_to, to}] / 70);
						}
						else if (sub2pri[from] == sub2pri[to])
						{
							if (finish_time.find({ from, to }) == finish_time.end())
								finish_time[{from, to}] = r.arrivetime + dists[{pri_of_to, to}] / 70;

							time_vec[{from, to}].push_back(dists[{pri_of_to, to}] / 70);

							TP tp(finish_time[{from, to}], city[to].name);
							if (timetable[{from,to}].back() != tp)
							timetable[{from, to}].push_back(tp);

							int truck = int(boxs[to].contain + 1);
							if (truck_between.find({ pri_of_to, to }) == truck_between.end())
								truck_between[{pri_of_to, to}] = truck;
						}
						else
						{
							if (finish_time.find({ from, to }) == finish_time.end())
								finish_time[{from, to}] = r.arrivetime + 2 + dists[{pri_of_to, to}] / 70;
							time_vec[{from, to}].push_back(2);
							time_vec[{from, to}].push_back(dists[{pri_of_to, to}] / 70);

							TP tp(finish_time[{from, to}], city[to].name);
							if (timetable[{from, to}].back() != tp)
							timetable[{from, to}].push_back(tp);

							int truck = int(boxs[to].contain + 1);
							if (truck_between.find({ pri_of_to, to }) == truck_between.end())
								truck_between[{pri_of_to, to}] = truck;
						}
					}
					
					boxs[to].contain = 0;
					boxs[to].subs.clear();
				}
				else
				{
					//if the next arrive time will make some express from somewhere stored in the box already break the limit, it should be send out right now
					for (auto from : boxs[to].subs)
					{
						if ((i < routes.size() - 1) && (limit[{from, to}] < (dists[{pri_of_to, to}] / 70 + routes[i + 1].arrivetime + 2)))
						{
							//go now
							for (auto from : boxs[to].subs)
							{
								if (finish_time.find({ from, to }) == finish_time.end())
									finish_time[{from, to}] = r.arrivetime + dists[{pri_of_to, to}] / 70 + 2;
								time_vec[{from, to}].push_back(-1);
								time_vec[{from, to}].push_back(r.arrivetime + dists[{pri_of_to, to}] / 70);
								time_vec[{from, to}].push_back(2);

								int truck = int(boxs[to].contain + 1);
								if (truck_between.find({ pri_of_to, to }) == truck_between.end())
									truck_between[{pri_of_to, to}] = truck;

								TP tp(finish_time[{from, to}], city[to].name);
								if (timetable[{from, to}].back() != tp)
									timetable[{from, to}].push_back(tp);
							}
							boxs[to].contain = 0;
							boxs[to].subs.clear();
							break;
						}
					}
				}
			}
			cleartime = r.arrivetime;
		}
		//clear the rest
		for (auto iter : boxs)
		{
			int to = iter.first;
			set<int> set_of_from = boxs[to].subs;
			for (auto from : set_of_from)
			{
				if (finish_time.find({ from, to }) == finish_time.end())
					finish_time[{from, to}] = cleartime + 2 + dists[{pri_of_to, to}] / 70;

				int truck = int(boxs[to].contain + 1);
				if (truck_between.find({ pri_of_to, to }) == truck_between.end())
					truck_between[{pri_of_to, to}] = truck;

				TP tp(finish_time[{from, to}], city[to].name);
				if (timetable[{from, to}].back() != tp)
					timetable[{from, to}].push_back(tp);
			}
		}

	}
	int cnt = 0;
	ofstream out("error.txt");
	for (auto iter : finish_time)
	{
		int from = iter.first.first;
		int to = iter.first.second;
		if (from == to) continue;
		float time = iter.second;

		if (time > limit[{from, to}])
		{
			out << "Out of time limit : from " << city[from].name << " to " << city[to].name << endl;
			out << "Time : " << time << " Limit : " << limit[{from, to}] << endl;
			out << "Route : " << city[from].name << "->" << city[sub2pri[from]].name << "->" << city[sub2pri[to]].name << "->" << city[to].name << endl;
			for (auto t : time_vec[{from, sub2pri[from]}])
				out << t << " | ";
			out << endl;
			out << endl;
			cnt++;
		}
	}
	error = cnt;

	float sum_cost = 0;
	int count = 0;
	for (auto iter : truck_between)
	{
		int from = iter.first.first;
		int to = iter.first.second;
		int truck = iter.second;
		if (truck != 0)
			count++;
		sum_cost += dists[{from, to}] * truck;
	}

	return sum_cost;
}

void Graph::print_timetable(ostream& os)
{
	if (timetable.size() == 0)
		cout << "no table" << endl;

	else
	{
		for (auto iter : timetable)
		{
			int from = iter.first.first;
			int to = iter.first.second;
			if (from == to) continue;
			os << city[from].name << " " << city[to].name << endl;
			vector<TP> tps = iter.second;
			for (auto iter2 : tps)
			{
				float time = iter2.time;
				int hour = int(time);
				int min = int((time - hour) * 60);
				int sec = int(time * 3600 - hour * 3600 - min * 60);
				int day = (19 + hour) / 24;
				hour = (19 + hour) % 24;
				os << "day" << day << " " << hour << ":" << min << ":" << sec << " " << iter2.place << endl;
			}
			os << endl;
		}
		
	}
}
