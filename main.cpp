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

	if (argc < 5) {
		std::cerr << "usage: verifer [graph file] [semilla] [file out] [metodo]" << std::endl;
		return -1;
	}
    semilla=atoi(argv[2]);
    int tipo_bl=atoi(argv[4]);
    string file_out=argv[3];
    srand(semilla);
	Graph G;
    ifstream canal_entrada;
    canal_entrada.open(argv[1]);
    if (canal_entrada) {
		try {
				G = Graph(canal_entrada);
				//G.printGraph();
                //cout<<"\n";
                G.connectedComponents();
                //G.printCC();
                G.inicializar_descriptores2();
                G.random_solution();
                //G.print_solution();
                //cout<<G.evaluate_solution(G.solution)<<"\n";
                //G.print_cliques();
                if(tipo_bl==1)
                    G.busqueda_local_ee(G.solution);
                else
                    G.busqueda_local_ee2(G.solution);
                G.build_m_edges();
                G.save_solution(file_out);
                //G.print_edges();
                //G.print_solution();
			}
			catch (const std::invalid_argument& ia) {
				return -1;
		}
	}
	else { return -1; }
	return 0;
}
