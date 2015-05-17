#pragma once
#include "sm.h"


const double M_PI = 3.141592653589;
const float R = 6371; //radius of the Earth(km)
const float INF = 0xffff;

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

	void route(int, int);
	float cost();

private:
	vector<City> city;
	map<pair<int, int>, float> dists; //distance
	map<pair<int, int>, float> express; //express from one to another
};