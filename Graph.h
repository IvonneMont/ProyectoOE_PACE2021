#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>
#include <map>
#include<fstream>
using namespace std;

class Graph
{
    private:
        //numero de vertices es a lo mas dos millones
        //numero de vertices + numero de aristas es a lo mas 5 millones
        int num_original_edges;
        vector<unordered_set<int>> adj;//adj.size() es igual al numero de vertices
        vector<vector<int>>cc;
        vector<int>identificador_componente;
        vector<int>sizes;
        vector<unordered_set<int>>cliques;
        vector<long long>descriptores;
        vector<pair<int,int>>m_edges;//lista de aristas modificadas

    public:
        vector<int>solution;
        Graph() = default;
        // Reads a graph in the DIMACS format
	    Graph(istream& is);
        void bfs(const int u);
        void connectedComponents();
        void printGraph() const;
        void printCC() const;
        long long evaluate_solution(const vector<int>&s);
        long long evaluacion_incremental(vector<int>&s_0,long long costo_s0,int v,int clique);
        void random_solution();
        void print_solution();
        void print_cliques();
        long long busqueda_local_ee(vector<int>&s_0);
        void inicializar_descriptores();
        void build_m_edges();
        void save_solution(string file_out);
        void print_edges();

};