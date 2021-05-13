#include "Graph.h"

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
        cout << i << ": ";
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
        int cq=rand()%adj.size();
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
  return costo_s0;
}
void Graph::inicializar_descriptores(){
  descriptores.clear();
  for(long long i=0; i<adj.size()*adj.size(); i++)
    descriptores.push_back(i);
}

void Graph::build_m_edges(){
    for (int i = 0; i <adj.size(); ++i){
        for(int j=i+1; j<adj.size(); ++j){
            if(solution[i]==solution[j]&&(adj[i].find(j)==adj[i].end()))
                m_edges.push_back({i,j});
            if(solution[i]!=solution[j]&&(adj[i].find(j)!=adj[i].end()))
                m_edges.push_back({i,j});
       }
	}
}

void Graph::save_solution(string file_out){
    ofstream canal_salida;
    cout<<file_out[0];
    canal_salida.open(file_out);
    if(canal_salida){
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