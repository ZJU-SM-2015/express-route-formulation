#pragma once
#include "sm.h"

const double M_PI = 3.141592653589;
const float R = 6371; //radius of the Earth(km)
const float INF = 0xffff;

struct Box{
	set<int> subs;
	float contain;

	Box() { contain = 0; }
};

struct Route{
	int begin;
	int desti;
	int truck;
	float arrivetime;
	Box box;
	Route(int _begin, int _desti, int _truck, float _arrivetime, Box _box)
		:begin(_begin), desti(_desti), truck(_truck), arrivetime(_arrivetime), box(_box) {}
	friend bool operator< (const Route& r1, const Route&r2);
};

struct TP{
	float time;
	string place;

	TP(float _time, string _place)
		:time(_time), place(_place){}

	friend bool operator !=(const TP&, const TP&);
};

class City{
	friend bool less_lat(const City&, const City&);
	friend class Graph;

public:
	City(int _id, string _name, pair<float, float> _coor, float _weight,bool _prime)
		:id(_id), name(_name), coor(_coor), weight(_weight),prime(_prime) {}
	~City() {}
	int get_id();
	float get_weight();

private:
	int id;
	string name;
	pair<float, float> coor;
	float weight;
	bool prime;
};


class Graph{
public:
	Graph(ifstream&,float prime_weight);
	~Graph() {}
	float dist(const pair<float, float>& v1, const pair<float, float>& v2);
	int closest_primer(int target);
	
	void set_primer_between(int id, int primer_id, float prime_radius);
	void set_primer(float prime_radius);

	void print_primers();
	void print_express();
	void get_limit(map<pair<int, int>, float>& limit);
	void get_pri2sub(map<int, set<int>>& pri2sub, const vector<int>& sub2pri);
	void get_sub2pri(vector<int>& sub2pri);
	void route(int, int);
	float cost(int &error);
	void print_timetable(ostream& os);

private:
	vector<City> city;
	map<pair<int, int>, float> dists; //distance
	map<pair<int, int>, float> express; //express from one to another
	map<pair<int, int>, vector<TP>> timetable;
};


