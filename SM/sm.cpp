#include "sm.h"
#include "graph.h"
#include <fstream>

int main()
{
	ifstream in("data2.txt");

	Graph graph(in,500);

	/*for (unsigned i = 70; i < 110; ++i)
	{
		graph.set_primer(i);
		if (graph.cost()>0)
			cout << i << " : " << graph.cost() << endl;	
	}*/
	float i = 250;
	graph.set_primer(i);
	cout << i << " : " << graph.cost() << endl;
	//graph.print_express();

	system("pause");
}
