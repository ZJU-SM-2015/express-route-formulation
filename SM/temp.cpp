//start
float t = 0; //record time

for (auto iter : dists_of_sub)
{
	t = (iter / (70 * 1.0));
	int sub = sub_dist[iter];

	for (unsigned i = 0; i < city.size(); ++i)//an iteration through the boxs
	{
		if (city[i].prime == true)
		{
			for (unsigned j = 0; j < boxs[i].subs.size(); ++j)
			{
				time[{j, i}] = t;
				time_vec[{j, i}].push_back(t);
			}

			boxs[i].contain += exp_sub2pri[{sub, i}];
			boxs[i].subs.insert(sub);
			if (boxs[i].contain >= 1 || exp_2pri[i] <= 1)	//       //if the express to another primer is sum up to over 1 truck
			{
				int truck = int(boxs[i].contain + 1);
				for (auto j : boxs[i].subs)
				{
					time[{j, i}] += dists[{j, i}] / (70 * 1.0) + 2; //!!!consider pri2pri???
					time_vec[{j, i}].push_back(dists[{j, i}] / (70 * 1.0));
					time_vec[{j, i}].push_back(2);
					Route route(j, i, truck, time[{j, i}], boxs[i]);
					routes.push_back(route);
				}
				exp_2pri[i] -= boxs[i].contain;
				boxs[i].contain = 0;               // the trucks can go off now
				boxs[i].subs.clear();
			}
		}
	}
}
//clear the boxs
for (unsigned i = 0; i < boxs.size(); ++i)
{
	int truck = int(boxs[i].contain + 1);

	for (auto j : boxs[i].subs)
	{
		time[{j, i}] += dists[{j, i}] / (70 * 1.0) + 2;
		time_vec[{j, i}].push_back(dists[{j, i}] / (70 * 1.0));
		time_vec[{j, i}].push_back(2);
		Route route(j, i, truck, time[{j, i}], boxs[i]);
		routes.push_back(route);
	}
	boxs[i].contain = 0;               // the trucks can go off now
	boxs[i].subs.clear();
}

routes_of_pri[pri] = routes;
