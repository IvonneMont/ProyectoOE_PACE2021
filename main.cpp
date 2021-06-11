#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include "Graph.h"
#include <algorithm>
#include<ctime>
#include<cstdio>
#include <chrono>
#include <unistd.h>
using namespace std;
void ejecuciones_cluster(int argc, char** argv){
    double tiempo_max=600;
    double tiempo=0;
    int semilla;
    if (argc < 4) {
		std::cerr << "usage: verifer [graph file] [semilla] [file out]" << std::endl;
		return;
	}
    semilla=atoi(argv[2]);
    string file_out=argv[3];
    srand(semilla);
	Graph G;
    ifstream canal_entrada;
    canal_entrada.open(argv[1]);
    if (canal_entrada) {
		try {
                struct timespec begin1, end1; 
                clock_gettime(CLOCK_REALTIME, &begin1);
				G = Graph(canal_entrada);
                if(G.verificar_conexidad_fuerte()){
                    G.save_solution(file_out);
                    return;
                }

                G.find_twins_vertex();
                G.random_solution1();
                G.best_solution=G.solution;
                G.best_costo=G.costo;

                clock_gettime(CLOCK_REALTIME,&end1);
                long seconds1 = end1.tv_sec - begin1.tv_sec;
                long nanoseconds1 = end1.tv_nsec - begin1.tv_nsec;
                double elapsed1 = seconds1 + (double)nanoseconds1*1e-9;
                tiempo+=elapsed1;
                while(tiempo<tiempo_max){
                    struct timespec begin, end; 
                    clock_gettime(CLOCK_REALTIME, &begin);
                    G.connectedComponents(tiempo_max-tiempo);
                    if(G.costo<G.best_costo){
                         G.best_costo=G.costo;
                         G.best_solution=G.solution;
                     } 
                    clock_gettime(CLOCK_REALTIME,&end);
                    long seconds = end.tv_sec - begin.tv_sec;
                    long nanoseconds = end.tv_nsec - begin.tv_nsec;
                    double elapsed = seconds + (double)nanoseconds*1e-9;
                    tiempo+=elapsed;

                    clock_gettime(CLOCK_REALTIME, &begin);
                    G.mutar_solucion();
                    G.connectedComponents(tiempo_max-tiempo);
                    clock_gettime(CLOCK_REALTIME,&end);
                    seconds = end.tv_sec - begin.tv_sec;
                    nanoseconds = end.tv_nsec - begin.tv_nsec;
                    elapsed = seconds + (double)nanoseconds*1e-9;
                    tiempo+=elapsed;
                }
                if(G.get_flag_data_reduction()==0){
                    G.save_solution(file_out);
                }
                else{
                    G.save_solution_data_reduction(file_out);
                }
			}
			catch (const std::invalid_argument& ia) {
				return;
		}
	}
	else {
    cout<<"No existe el archivo";
    return;
    }
}
void pruebas(int argc, char** argv){
    int semilla;

	if (argc < 4) {
		std::cerr << "usage: verifer [graph file] [semilla] [file out]" << std::endl;
		return;
	}
    semilla=atoi(argv[2]);
    string file_out=argv[3];
    srand(semilla);
	Graph G;
    ifstream canal_entrada;
    canal_entrada.open(argv[1]);
    if (canal_entrada) {
		try {
				G=Graph(canal_entrada);
                G.random_solution1();
                G.connectedComponents(60);
               
                G.best_solution=G.solution;
                G.best_costo=G.costo;
                G.save_solution(file_out);

			}
			catch (const std::invalid_argument& ia) {
				return;
		}
	}
	else { return; }
}
void data_reduction(int argc, char** argv){
    int semilla;

	if (argc < 4) {
		std::cerr << "usage: verifer [graph file] [semilla] [file out]" << std::endl;
		return;
	}
    semilla=atoi(argv[2]);
    string file_out=argv[3];
    srand(semilla);
	Graph G;
    ifstream canal_entrada;
    canal_entrada.open(argv[1]);
    if (canal_entrada) {
		try {
				G=Graph(canal_entrada);
                if(G.verificar_conexidad_fuerte()){
                    G.save_solution(file_out);
                    return;
                }
                G.find_twins_vertex();
                G.random_solution1();
                G.best_solution=G.solution;
                G.best_costo=G.costo;
                G.print_solution();

                G.connectedComponents(60);
                G.best_solution=G.solution;
                G.best_costo=G.costo;
                G.print_solution();

                if(G.get_flag_data_reduction()==0){
                    G.save_solution(file_out);
                }
                else{
                    G.save_solution_data_reduction(file_out);
                }


			}
			catch (const std::invalid_argument& ia) {
				return;
		}
	}
	else { return; }
}
void vector_data_reduction(string ruta){
    vector<int>vector_nv;
    vector<int>vector_nr;
    string archivo;
    for (int i=195; i<=199; i++){
        if (i%2==0)
            continue;
        if(i<10)
            archivo=ruta+"00"+to_string(i)+".gr";
        if(i<100&&i>=10)
            archivo=ruta+"0"+to_string(i)+".gr";
        if(i>=100)
            archivo=ruta+to_string(i)+".gr";
        ifstream canal_entrada;
        canal_entrada.open(archivo);
        if (canal_entrada){
            cout<<i<<" ";
            Graph G = Graph(canal_entrada);
            if(G.verificar_conexidad_fuerte()){
                vector_nv.push_back(G.get_num_vertex());
                vector_nr.push_back(0);
                cout<<G.get_num_aristas()<<" "<<G.get_num_vertex()*(G.get_num_vertex()-1)/2<<" "<<G.get_num_vertex()*(G.get_num_vertex()-1)/2-G.get_num_aristas();
                cout<<" vc \n";
                continue;
            }
            G.find_twins_vertex();
            vector_nv.push_back(G.get_num_vertex()+G.get_num_reductions());
            vector_nr.push_back(G.get_num_reductions());
            cout<<G.get_num_reductions()+G.get_num_vertex()<<" ";
            cout<<G.get_num_reductions()<<"\n";
        }
        canal_entrada.close();
    }
    ofstream canal_salida;
    canal_salida.open("/home/est_posgrado_ivonne.monter/Optimizacion_Estocastica/proyecto/v_dr.txt");
    if (canal_salida){
        for(int i=0; i<vector_nv.size(); i++){
            canal_salida<<vector_nv[i]<<", ";
        }
        canal_salida<<"\n";
        for(int i=0; i<vector_nr.size(); i++){
            canal_salida<<vector_nr[i]<<", ";
        }
    }

}
int main(int argc, char** argv)
{
    ejecuciones_cluster(argc,argv);
    //pruebas(argc,argv);
    //data_reduction(argc,argv);
    //vector_data_reduction(argv[1]);
	return 0;
}
