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
#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <limits>
#include <climits>

 
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
        vector<pair<int,vector<int>>>des;
        vector<pair<int,int>>m_edges;//lista de aristas modificadas
        vector<pair<int,int>>rangos;
        long long costo;
        long long best_costo=LLONG_MAX;

    public:
        vector<int>solution;
        vector<int>best_solution;
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
        void print_des();
        long long busqueda_local_ee(vector<int>&s_0);
        long long busqueda_local_ee2(vector<int>&s_0);
        void inicializar_descriptores();
        void inicializar_descriptores2();
        void build_m_edges();
        void save_solution(string file_out);
        void print_edges();

};
Graph::Graph(istream& is){
			string line;
			while (getline(is, line) && (line.front() == 'c'))
				;
			if (line.empty() || (line.front() != 'p'))
				throw invalid_argument("First non-comment line is not the problem line");
			auto iss = istringstream(line);
			string s;
			int n, m;
			if ((!(iss >> line >> line >> n >> m)) || (n < 0) || (m < 0))
				throw invalid_argument("Incomplete or invalid problem line");
			adj = vector<unordered_set<int>>(n);
            num_original_edges=m;
			for (int i = 0; i < m; ++i) {
				while (getline(is, line) && (line.front() == 'c'))
					;
				iss.clear();
				iss.str(line);
				int u, v;
				if (!(iss >> u >> v))
					throw invalid_argument("An invalid line or not enough lines in the input. Offending line:\n" + line + "\n");
				--u; --v;
				adj[u].insert(v); adj[v].insert(u);
			}
}
void Graph::bfs(const int u){
    queue<int> q;
	q.push(u);
	while (!q.empty()) {
		auto cur = q.front();
		q.pop();
		for (auto i : adj[cur]) {
            if (identificador_componente[i] >= 0)
				continue;
			identificador_componente[i] = identificador_componente[u];
			++sizes[identificador_componente[u]];
			q.push(i);
		}
   }
}

void Graph::connectedComponents()
{
  identificador_componente = vector<int>(adj.size(), -1);
	for (int i = 0; i < adj.size(); ++i) {
			if (identificador_componente[i] >= 0)
				continue;
            int id=sizes.size();
			identificador_componente[i] = id;
			sizes.push_back(1);
			bfs(i);
	}
  cc.resize(sizes.size());
  
  for(int i=0; i<identificador_componente.size(); i++){
        cc[identificador_componente[i]].push_back(i);    
  }
  long long rango=0;
  rangos.resize(cc.size(),{-1,-1});
  for(int j=0; j<cc.size(); j++){
      rangos[j]={rango,rango+sizes[j]-1};
      rango+=sizes[j];
  }
}
void Graph::printGraph() const
		{
			for (int i = 0; i < adj.size(); ++i) {
				std::cout << i << ": ";
				for (auto j : adj[i])
					std::cout << j << ", ";
				std::cout << '\n';
			}
		}
void Graph::printCC() const
{
    for (int i = 0; i <cc.size(); ++i) {
       cout <<i<<" "<<sizes[i]<<" R "<<rangos[i].first<<" "<<rangos[i].second<<": ";
		  for(int j=0; j<cc[i].size(); j++)
            cout<<cc[i][j]<<" ";
		  cout << '\n';
	}
}

long long Graph::evaluate_solution(const vector<int>&s){
    long long costo=0;
    for (int i = 0; i <adj.size(); ++i){
        for(int j=i+1; j<adj.size(); ++j){
            if(solution[i]==solution[j]&&(adj[i].find(j)==adj[i].end()))
                costo++;
            if(solution[i]!=solution[j]&&(adj[i].find(j)!=adj[i].end()))
                costo++;
       }
	}
    return costo;
}


void Graph::random_solution(){
    cliques=vector<unordered_set<int>>(adj.size());
    solution.clear();
    for(int i=0; i<adj.size(); i++){
        int min=rangos[identificador_componente[i]].first;
        int max=rangos[identificador_componente[i]].second;
        int cq=rand()%(max-min+1)+min;
        cliques[cq].insert(i);
        solution.push_back(cq);
    }
}
void Graph::print_solution(){
    for(int i=0; i<solution.size(); i++)
        cout<<solution[i]<<" ";
    cout<<"\n";
}

void Graph::print_cliques(){
    for(int i=0; i<cliques.size(); i++){
        cout << i << ": ";
        for (auto j : cliques[i])
            std::cout << j << ", ";
		std::cout << '\n';
    }
}

long long Graph::evaluacion_incremental(vector<int>&s_0,long long costo_s0,int v,int c){
  if(c==s_0[v])//Es la misma solucion 
    return costo_s0;
  for (auto j : cliques[s_0[v]]){
      if(j!=v){
          if(adj[v].find(j)==adj[v].end())//la arista se agregó previamente y ahora de elimina
            costo_s0--;
          if(adj[v].find(j)!=adj[v].end())//la arista es una arista original  y ahora se elimina
            costo_s0++;
      }
  }
  for (auto j : cliques[c]){
      if(j!=v){
          if(adj[v].find(j)==adj[v].end())//no es una arista original y hay que agregar la arista
            costo_s0++;
          if(adj[v].find(j)!=adj[v].end())//la arista es una arista original que se elimino antes  y ahora se reincorpora
            costo_s0--;
      }
  }
  return costo_s0;
}

long long Graph::busqueda_local_ee(vector<int>&s_0){
  long long costo_s0,costo2;
  long long pos;
  int v,des,c;
  int flag=0;
  costo_s0=evaluate_solution(s_0);
  costo2=costo_s0+1;
  int num_clanes=adj.size();
  while(flag<1){
    inicializar_descriptores();
    int fin=adj.size()*adj.size();
    while(fin>0&&costo_s0<=costo2){
      pos=rand()%fin;//toma el valor de una posicion de descriptor no comprobado
      des=descriptores[pos]; //Descriptor no comprobado
      v=des/num_clanes;//vertice
      c=des-v*num_clanes;//clique
      swap(descriptores[pos],descriptores[fin-1]);//manda hacia atras al descriptor actual
      fin--;//reduce en uno el conjunto de descriptores no comprobados
      costo2=evaluacion_incremental(s_0,costo_s0,v,c);
    }
    if(costo2<costo_s0)//se encontro un vecino mejor
      {
        costo_s0=costo2;
        costo2=costo_s0+1;
        cliques[s_0[v]].erase(v);
        cliques[c].insert(v);
        s_0[v]=c;
       
      }
    else{//el actual es el mejor de su vecindad
        flag=1;
    }
    

  }
  costo=costo_s0;
  return costo_s0;
}
long long Graph::busqueda_local_ee2(vector<int>&s_0){
  long long costo_s0,costo2;
  long long pos;
  int pv,pc,v,c;
  int flag=0;
  costo_s0=evaluate_solution(s_0);
  costo2=costo_s0+1;
  int num_clanes=adj.size();
  while(flag<1){
    inicializar_descriptores2();
    while(des.size()>0&&costo_s0<=costo2){
      pv=rand()%des.size();//elige vertice
      pc=rand()%des[pv].second.size(); //elige clan
      v=des[pv].first;//vertice
      c=des[pv].second[pc];//clique
      swap(des[pv].second[pc],des[pv].second[des[pv].second.size()-1]);
      des[pv].second.pop_back();//elimina el descriptor comprobado
      if(des[pv].second.size()==0){
        swap(des[pv],des[des.size()-1]);//elimina el vertice sin conjuntos disponible
        des.pop_back();
      }
      costo2=evaluacion_incremental(s_0,costo_s0,v,c);
    }
    if(costo2<costo_s0)//se encontro un vecino mejor
      {
        costo_s0=costo2;
        costo2=costo_s0+1;
        cliques[s_0[v]].erase(v);
        cliques[c].insert(v);
        s_0[v]=c;
       
      }
    else{//el actual es el mejor de su vecindad
        flag=1;
    }
    

  }
  costo=costo_s0;
  return costo_s0;
}
void Graph::inicializar_descriptores(){
  descriptores.clear();
  for(long long i=0; i<adj.size()*adj.size(); i++)
    descriptores.push_back(i);
}
void Graph::inicializar_descriptores2(){
  des.clear();
  for(long long i=0; i<adj.size(); i++){
    long long tam=rangos[identificador_componente[i]].second-rangos[identificador_componente[i]].first;
    if (tam>0){//evitar puntos aislados
      vector<int>conjuntos;
      for (long long j=rangos[identificador_componente[i]].first; j<=rangos[identificador_componente[i]].second; j++)
        conjuntos.push_back(j);
      des.push_back({i,conjuntos});
    }
  }
}

void Graph::build_m_edges(){
    for (int i = 0; i <adj.size(); ++i){
        for(int j=i+1; j<adj.size(); ++j){
            if(best_solution[i]==best_solution[j]&&(adj[i].find(j)==adj[i].end()))
                m_edges.push_back({i,j});
            if(best_solution[i]!=best_solution[j]&&(adj[i].find(j)!=adj[i].end()))
                m_edges.push_back({i,j});
       }
	}
}

void Graph::save_solution(string file_out){
    ofstream canal_salida;
    canal_salida.open(file_out);
    if(canal_salida){
      canal_salida<<costo<<"\n";
      for(long long i=0; i<m_edges.size(); i++)
        canal_salida<<m_edges[i].first+1<<" "<<m_edges[i].second+1<<"\n";

      canal_salida.close();
     }
     else{
       cout<<"Error creating file!\n";
     }
}
void Graph::print_edges(){
      for(long long i=0; i<m_edges.size(); i++)
        cout<<m_edges[i].first+1<<" "<<m_edges[i].second+1<<"\n";

}

void Graph::print_des(){
  for (int i=0; i<des.size(); i++){
    cout<<des[i].first<<": ";
    for(int j=0; j<des[i].second.size(); j++){
      cout<<des[i].second[j]<<" ";
    }
    cout<<"\n";
  }
}

volatile sig_atomic_t tle = 0;
int n;
float worker;
void term(int signum)
{
    tle = 1;
}

int main(int argc, char** argv)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    long long c_cost;
    long long b_cost=LLONG_MAX;

    Graph G;
	G = Graph(cin);
    G.connectedComponents();
    ios_base::sync_with_stdio(false);
    while(!tle){
        G.inicializar_descriptores2();
        G.random_solution();
        c_cost=G.busqueda_local_ee2(G.solution);
        if(c_cost<b_cost){
            b_cost=c_cost;
            G.best_solution=G.solution;
        } 
    }
    G.build_m_edges();
    G.print_edges();
	return 0;
}

