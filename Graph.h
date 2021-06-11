#ifndef GRADIENTE_H
#define GRADIENTE_H 1

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
#include <limits>
#include <climits>
#include<algorithm>
#include <time.h>
#include <sys/time.h>
using namespace std;
enum { NS_PER_SECOND = 1000000000 };
using namespace std;

class Graph
{
    private:
        //numero de vertices es a lo mas dos millones
        //numero de vertices + numero de aristas es a lo mas 5 millones
        
        /*Instancia*/
        int num_vertices;
        int num_aristas;

        vector<set<int>> adj;//lista de adyacencias

        /*Solución*/
        vector<set<int>>cliques;//vector de los cluster formados por la solución actual
        vector<int>identificador_componente;//para cada vértice idetifica el cluster al que pertenece en la solución actual

        /*Búsqueda local*/
        vector<long long>descriptores;
        vector<pair<int,vector<int>>>des;
        vector<pair<int,int>>rangos;
        vector<int>sizes;

        /*Data reduction*/
        int reduction=0;
        char data_reduction=1;//bandera para indicar si fue posible hacer reducción
        vector<vector<int>>vertices_comprimidos;//almacena la lista de gemelos para cada vértice único
        vector<int>vertices_unicos;//lista de vertices sin gemelos
        map<int,int>r2v;//llave es un vértice representante y el valor es el número de vertice que se le asigno en la nueva gráfica
        
        
        
        
        

    public:
        long long costo;
        long long best_costo=LLONG_MAX;
        vector<int>solution;
        vector<int>best_solution;
        Graph() = default;
        // Reads a graph in the DIMACS format
	      Graph(istream& is);
        char get_flag_data_reduction(){return data_reduction;};
        int get_num_reductions(){return reduction;};
        int get_num_vertex(){return adj.size();};
        int get_num_aristas(){return num_aristas;};
        int bfs(const int u,int rango,double tiempo_restante);
        void connectedComponents(double tiempo_restante);
        void printGraph() const;
        void printCC() const;
        long long evaluate_solution(const vector<int>&s);
        long long evaluate_solution_V(vector<int>&V,const vector<int>&s);
        long long evaluacion_incremental(vector<int>&s_0,long long costo_s0,int v,int clique);
        void random_solution();
        void print_solution();
        void print_cliques();
        void print_des();
        long long busqueda_local_ee(vector<int>&s_0,double tiempo_r);
        long long busqueda_local_ee_cc(vector<int>&V,vector<int>&s_0,double tiempo_r,int ri);
        long long busqueda_local_ee2(vector<int>&s_0,double tiempo_r);
        void inicializar_descriptores();
        void inicializar_descriptores_cc(vector<int>&V);
        void inicializar_descriptores2();
        void build_m_edges();
        void build_m_edges_data_reduction();
        void save_solution(string file_out);
        void random_solution1();
        void optimizacion(vector<int>&V,int ri,double tiempo_restante);
        void mutar_solucion();
        void find_twins_vertex();
        long long get_costo(){return costo;};
        void save_solution_data_reduction(string file_out);
        int verificar_conexidad_fuerte();
};
int Graph::verificar_conexidad_fuerte(){
  if ((double)num_aristas>0.8*(num_vertices*(num_vertices-1.0)/2)){
    best_solution.resize(num_vertices,0);
    best_costo=(num_vertices*(num_vertices-1))/2-num_aristas;
    return 1;
  }
  return 0;
}
void Graph::find_twins_vertex(){
  vector<set<int>>adj_reducida;
  vector<int>estados(adj.size(),0);
  vector<int>representante(adj.size());
  for(int i=0; i<adj.size(); i++){
    if (estados[i]>0)
      continue;
    estados[i]=1;
    representante[i]=i;
    r2v.insert({i,vertices_unicos.size()});
    vertices_unicos.push_back(i);
    vertices_comprimidos.push_back({});
    for(auto v:adj[i]){
      set<int>aux=adj[i];
      aux.erase(v);
      adj[v].erase(i);
      if (aux==adj[v]){
        estados[v]=2;
        representante[v]=i;
        vertices_comprimidos[vertices_comprimidos.size()-1].push_back(v);
      }
      adj[v].insert(i);
    }
  }
  // for(int i=0; i<vertices_comprimidos.size(); i++){
  //   cout<<vertices_unicos[i]<<": ";
  //   for(int j=0; j<vertices_comprimidos[i].size(); j++){
  //     cout<<vertices_comprimidos[i][j]<<" ";
  //   }
  //   cout<<"\n";
  // }
  if(vertices_unicos.size()==adj.size()){//no hubo reduccion
      data_reduction=0;
      vertices_comprimidos.clear();
      r2v.clear();
      vertices_unicos.clear();
      return;
  }
  adj_reducida.resize(vertices_unicos.size());
  for (int i=0; i<vertices_unicos.size(); i++){
    for(auto v:adj[vertices_unicos[i]]){
      if(representante[vertices_unicos[i]]!=representante[v]){
        adj_reducida[i].insert(r2v[representante[v]]);
      }
    }
  }
  reduction=representante.size()-vertices_unicos.size();
  adj=adj_reducida;
}
void Graph::mutar_solucion(){
  int p=-1;
  double max=-1;
  vector<double>coeficiente(cliques.size(),-1);//numero de inserciones entre el numero de vertices del clique
  vector<int>cliques_vacios;
  for (int i=0; i<adj.size(); ++i){
        for(int j=i+1; j<adj.size(); ++j){
            if(solution[i]==solution[j]&&(adj[i].find(j)==adj[i].end()))//se agrego una arista
                coeficiente[solution[i]]+=1.0/cliques[solution[i]].size();
       }
	}
  for(int i=0; i<coeficiente.size(); i++){
    if(coeficiente[i]>max){
      max=coeficiente[i];
      p=i;
    }
    if(cliques[i].size()==0){
      cliques_vacios.push_back(i);
    }
  }
  if (p==-1)
    return;
  int clan =cliques_vacios[rand()%cliques_vacios.size()];
  int pv =rand()%cliques[p].size();
  auto it=cliques[p].begin();
  for(int i=0; i<pv; i++){
    it++;
  }
  int vertice=*it;
  cliques[solution[vertice]].erase(vertice);
  cliques[clan].insert(vertice);
  solution[vertice]=clan;
  for(auto i:adj[vertice]){
    if(solution[i]==solution[vertice]){
      cliques[solution[i]].erase(i);
      cliques[clan].insert(i);
      solution[i]=clan;
    }
  }
}
void Graph::optimizacion(vector<int>&V,int ri,double tiempo_restante){
  // struct timeval start, end;
  // gettimeofday(&start, NULL);
  // for(int i=0; i<V.size(); i++){
  //    cliques[solution[V[i]]].erase(V[i]);
  //    int cq=rand()%V.size()+ri;
  //    cliques[cq].insert(V[i]);
  //    solution[V[i]]=cq;
  //  }
  // gettimeofday(&end, NULL);
  // double tiempo=((end.tv_sec * 1000000 + end.tv_usec)-(start.tv_sec * 1000000 + start.tv_usec))/1e+6;
  if(V.size()==1){
    solution[V[0]]=ri;
    return;
  }
  busqueda_local_ee_cc(V,solution,tiempo_restante,ri);
}
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
			adj.resize(n); 
      num_vertices=n;
      num_aristas=m;
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
int Graph::bfs(const int u, int rango,double tiempo_restante){
  long long num_edges=0;
  struct timeval start, end;
  gettimeofday(&start, NULL);
  int numv=1;
  vector<int>vertices;
  vertices.push_back(u);
  queue<int> q;
	q.push(u);
	while (!q.empty()) {
		auto cur = q.front();
    num_edges+=adj[cur].size();
		q.pop();
		for (auto i : adj[cur]) {
      if (identificador_componente[i] >= 0)
				continue;
			identificador_componente[i] = identificador_componente[u];
      vertices.push_back(i);
			numv++;
			q.push(i);
		}
   }
   gettimeofday(&end, NULL);
   double tiempo=((end.tv_sec * 1000000 + end.tv_usec)-(start.tv_sec * 1000000 + start.tv_usec))/1e+6;
   num_edges=num_edges/2;
  //  if(num_edges==(vertices.size()*(vertices.size()-1))/2){
  //    cout<<"grafica completa "<<vertices.size()<<endl;
  //  }
   optimizacion(vertices,rango,tiempo_restante-tiempo);
   return numv;
}

void Graph::connectedComponents(double tiempo_restante)
{
  double tiempo=0;
  identificador_componente.clear();
  identificador_componente.resize(adj.size(),-1);
  long long rango=0;
  int num_cc=0;
	for (int i=0; i<adj.size(); ++i) {
      struct timeval start, end;
      gettimeofday(&start, NULL);
			if (identificador_componente[i]>=0)
				continue;
			identificador_componente[i]=num_cc;
			rango+=bfs(i,rango,tiempo_restante);
      num_cc++;
      gettimeofday(&end, NULL);
      tiempo=tiempo+((end.tv_sec * 1000000 + end.tv_usec)-(start.tv_sec * 1000000 + start.tv_usec))/1e+6;
      if(tiempo>=tiempo_restante)
         return;
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
// void Graph::printCC() const
// {
//     for (int i = 0; i <cc.size(); ++i) {
//        cout <<i<<" "<<sizes[i]<<" R "<<rangos[i].first<<" "<<rangos[i].second<<": ";
// 		  for(int j=0; j<cc[i].size(); j++)
//             cout<<cc[i][j]<<" ";
// 		  cout << '\n';
// 	}
// }

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

long long Graph::evaluate_solution_V(vector<int>&V,const vector<int>&s){
    long long costo=0;
    for(int i=0; i<V.size(); i++){
      for(int j=i+1; j<V.size(); j++){
        if(solution[V[i]]==solution[V[j]]&&(adj[V[i]].find(V[j])==adj[V[i]].end()))
                costo++;
        if(solution[V[i]]!=solution[V[j]]&&(adj[V[i]].find(j)!=adj[V[i]].end()))
                costo++;
      }
    }
    return costo;
}


void Graph::random_solution(){
    cliques.resize(adj.size());
    solution.clear();
    for(int i=0; i<adj.size(); i++){
        int min=rangos[identificador_componente[i]].first;
        int max=rangos[identificador_componente[i]].second;
        int cq=rand()%(max-min+1)+min;
        cliques[cq].insert(i);
        solution.push_back(cq);
    }
}
void Graph::random_solution1(){
    cliques.resize(adj.size());
    solution.clear();
    for(int i=0; i<adj.size(); i++){
        int cq=rand()%adj.size();
        cliques[cq].insert(i);
        solution.push_back(cq);
    }
    costo=evaluate_solution(solution);
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

long long Graph::busqueda_local_ee(vector<int>&s_0,double tiempo_r){
  double tiempo=0;
  long long costo_s0,costo2;
  long long pos;
  int v,des,c;
  int flag=0;
  costo_s0=costo;
  costo2=costo_s0+1;
  int num_clanes=adj.size();
  while(flag<1){
    clock_t inicio=clock();
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
    clock_t fint=clock();
    tiempo=tiempo+double(fint-inicio)/CLOCKS_PER_SEC;
    // if(tiempo>=tiempo_r)
    //   flag=1;

  }
  costo=costo_s0;
  return costo_s0;
}
long long Graph::busqueda_local_ee_cc(vector<int>&V,vector<int>&s_0,double tiempo_r,int ri){
  struct timeval start, end;
  double tiempo=0;
  long long costo_s0,costo2;
  long long pos;
  int v,des,c,pv;
  int flag=0;
  costo_s0=evaluate_solution(solution);
  costo2=costo_s0+1;
  int num_clanes=V.size();
  while(flag<1){
    gettimeofday(&start, NULL);
    inicializar_descriptores_cc(V);
    int fin=V.size()*V.size();
    while(fin>0&&costo_s0<=costo2){
      pos=rand()%fin;//toma el valor de una posicion de descriptor no comprobado
      des=descriptores[pos]; //Descriptor no comprobado
      pv=des/num_clanes;//vertice
      c=des-pv*num_clanes+ri;//clique
      v=V[pv];
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
    gettimeofday(&end, NULL);
    tiempo=tiempo+((end.tv_sec * 1000000 + end.tv_usec)-(start.tv_sec * 1000000 + start.tv_usec))/1e+6;
    if(tiempo>=tiempo_r)
       flag=1;
  }
  costo=costo_s0;
  return costo_s0;
}
long long Graph::busqueda_local_ee2(vector<int>&s_0,double tiempo_r){
  double tiempo=0;
  long long costo_s0,costo2;
  long long pos;
  int pv,pc,v,c;
  int flag=0;
  costo_s0=evaluate_solution(s_0);
  costo2=costo_s0+1;
  int num_clanes=adj.size();
  while(flag<1){
    clock_t inicio=clock();
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
    clock_t fin=clock();
    tiempo=tiempo+double(fin-inicio)/CLOCKS_PER_SEC;
    if(tiempo>=tiempo_r)
      flag=1;
  }
  costo=costo_s0;
  return costo_s0;
}
void Graph::inicializar_descriptores(){
  descriptores.clear();
  for(long long i=0; i<adj.size()*adj.size(); i++)
    descriptores.push_back(i);
}
void Graph::inicializar_descriptores_cc(vector<int>&V){
  descriptores.clear();
  for(long long i=0; i<V.size()*V.size(); i++)
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
                printf("%d %d\n",i+1,j+1);
            if(best_solution[i]!=best_solution[j]&&(adj[i].find(j)!=adj[i].end()))
                printf("%d %d\n",i+1,j+1);
       }
	}
}
void Graph::build_m_edges_data_reduction(){
  /*
    i y j son vértices únicos, es necesario trasformarlos a vértices de la gráfica original para cada gemelo
  */
    for (int i=0; i<adj.size(); ++i){
        for(int j=i+1; j<adj.size(); ++j){
            if(best_solution[i]==best_solution[j]&&(adj[i].find(j)==adj[i].end()))//arista agregada
                {
                  printf("%d %d\n",vertices_unicos[i]+1,vertices_unicos[j]+1);
                  for(int k=0; k<vertices_comprimidos[i].size(); k++)
                      printf("%d %d\n",vertices_comprimidos[i][k]+1,vertices_unicos[j]+1);
                  
                  for(int l=0; l<vertices_comprimidos[j].size(); l++)
                      printf("%d %d\n",vertices_unicos[i]+1,vertices_comprimidos[j][l]+1);
                  
                  for(int k=0; k<vertices_comprimidos[i].size(); k++){
                    for(int l=0; l<vertices_comprimidos[j].size(); l++)
                      printf("%d %d\n",vertices_comprimidos[i][k]+1,vertices_comprimidos[j][l]+1);
                  }
                }
            if(best_solution[i]!=best_solution[j]&&(adj[i].find(j)!=adj[i].end()))//arista eliminada
                {
                  printf("%d %d\n",vertices_unicos[i]+1,vertices_unicos[j]+1);
                  for(int k=0; k<vertices_comprimidos[i].size(); k++)
                      printf("%d %d\n",vertices_comprimidos[i][k]+1,vertices_unicos[j]+1);
                  
                  for(int l=0; l<vertices_comprimidos[j].size(); l++)
                      printf("%d %d\n",vertices_unicos[i]+1,vertices_comprimidos[j][l]+1);
                  
                  for(int k=0; k<vertices_comprimidos[i].size(); k++){
                    for(int l=0; l<vertices_comprimidos[j].size(); l++)
                      printf("%d %d\n",vertices_comprimidos[i][k]+1,vertices_comprimidos[j][l]+1);
                  }
                }
       }
	}
}
void Graph::save_solution_data_reduction(string file_out){
    long long inser=0;
    long long del=0;
    ofstream canal_salida;
    canal_salida.open(file_out);
    if(canal_salida){
      for (int i=0; i<adj.size(); ++i){
        for(int j=i+1; j<adj.size(); ++j){
            if(best_solution[i]==best_solution[j]&&(adj[i].find(j)==adj[i].end()))//arista agregada
                {
                  inser+=(vertices_comprimidos[j].size()+1)*(vertices_comprimidos[i].size()+1);
                }
            if(best_solution[i]!=best_solution[j]&&(adj[i].find(j)!=adj[i].end()))//arista eliminada
                {
                  del+=(vertices_comprimidos[j].size()+1)*(vertices_comprimidos[i].size()+1);
                }
          }
	    }
      canal_salida<<inser<<" "<<del<<"\n";
      canal_salida<<inser+del<<"\n";
      canal_salida<<reduction;
      canal_salida.close();
     }
     else{
       cout<<"Error creating file!\n";
     }
}
void Graph::save_solution(string file_out){
    long long inser=0;
    long long del=0;
    ofstream canal_salida;
    canal_salida.open(file_out);
    if(canal_salida){
      for (int i = 0; i <adj.size(); ++i){
        for(int j=i+1; j<adj.size(); ++j){
            if(best_solution[i]==best_solution[j]&&(adj[i].find(j)==adj[i].end()))
              {
                del++;
                //canal_salida<<i+1<<" "<<j+1<<"\n";
              }
            if(best_solution[i]!=best_solution[j]&&(adj[i].find(j)!=adj[i].end()))
              {
                inser++;
                //canal_salida<<i+1<<" "<<j+1<<"\n";
              }
       }
	   }
      canal_salida<<inser<<" "<<del<<"\n";
      canal_salida<<best_costo<<"\n";
      canal_salida.close();
     }
     else{
       cout<<"Error creating file!\n";
     }
}
// void Graph::print_edges(){
//       for(long long i=0; i<m_edges.size(); i++)
//         cout<<m_edges[i].first+1<<" "<<m_edges[i].second+1<<"\n";

// }

void Graph::print_des(){
  for (int i=0; i<des.size(); i++){
    cout<<des[i].first<<": ";
    for(int j=0; j<des[i].second.size(); j++){
      cout<<des[i].second[j]<<" ";
    }
    cout<<"\n";
  }
}

#endif