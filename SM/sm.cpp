#include "sm.h"
#include "city.h"
#include <fstream>

int main()
{
	ifstream in("data2.txt");

	Graph graph(in,500);

	graph.set_primer(200);

	system("pause");
}
