#include "sm.h"
#include "graph.h"
#include <fstream>

int main()
{
	
	ofstream out("output.txt");

	for (float i = 100; i < 150; i = i + 1.0)
	{
		ifstream in("data2.txt");
		Graph graph(in, 500);
		graph.set_primer(i);
		int error;
		float cost = graph.cost(error);
		out << i << "  " << cost << " " << error << endl;
	}

	//system("pause");
}
