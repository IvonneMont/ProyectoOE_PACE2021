#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include "Graph.h"

int main(int argc, char** argv)
{
    int semilla;

	if (argc < 4) {
		std::cerr << "usage: verifer [graph file] [semilla] [file out]" << std::endl;
		return -1;
	}
    semilla=atoi(argv[2]);
    string file_out=argv[3];
    srand(semilla);
	Graph G;
    if (ifstream ifs{ argv[1] }; ifs) {
		try {
				G = Graph(ifs);
				//G.printGraph();
               //cout<<"\n";
                G.connectedComponents();
                //G.printCC();
                G.random_solution();
                //G.print_solution();
                cout<<"\n"<<G.evaluate_solution(G.solution)<<"\n";
                //G.print_cliques();
                cout<<"\n"<<G.busqueda_local_ee(G.solution)<<"\n";
                cout<<"\n"<<G.evaluate_solution(G.solution)<<"\n";
                G.build_m_edges();
                G.save_solution(file_out);
                G.print_edges();
                //G.print_solution();
			}
			catch (const std::invalid_argument& ia) {
				return -1;
		}
	}
	else { return -1; }
	return 0;
}
