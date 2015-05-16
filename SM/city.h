#pragma once
#include "sm.h"
#include <fstream>

class City{
	friend bool less_lat(const City&, const City&);

public:
	City() {}
	City(int _id, pair<float, float> _coor, float _weight)
		:id(_id), coor(_coor), weight(_weight) {
		prime = false;
	}
	~City();
	pair<float, float> get_coor();
	const pair<float, float> get_coor() const;
	int get_id();
	int get_weight();
	bool is_prime();
	void set_prime();

private:
	int id;
	pair<float, float> coor;
	float weight;
	bool prime;
};


class Graph{
public:
	Graph(ifstream);
	~Graph();
	double dist(const pair<float, float>& v1, const pair<float, float>& v2);
	City closest_primer(const City&);
	void set_primer_between(const City&, const City&, float prime_radius);
	void set_primer(float prime_weight, float prime_radius);

private:

	vector<City> citys;
	map<pair<int, int>, double> dists; //distance

};