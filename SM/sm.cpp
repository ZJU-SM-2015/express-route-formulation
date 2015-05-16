#include "sm.h"
#include "graph.h"
#include <fstream>

int main()
{
	ifstream in("data2.txt");

	Graph graph(in,500);

	graph.set_primer(220);

	system("pause");
}
