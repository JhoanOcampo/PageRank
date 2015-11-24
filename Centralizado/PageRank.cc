#include <iostream>
#include <string>
#include <cassert>
#include <zmqpp/zmqpp.hpp>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;
using namespace zmqpp;

using Graph = unordered_map<int, vector<int>>;
using NodeSet = unordered_set<int>;
using AdjMat = vector<vector<int>>;
using Norm = unordered_map<int, int>;

void printGraph(const Graph& g) {
  for (const auto& adj : g) {
    cout << adj.first << ": ";
    for(auto t : adj.second)
      cout << " " << t;
    cout << endl;
  }
}

pair<Graph, NodeSet> readGraph(const string &filename) {
  Graph result;
  NodeSet nodes;
  ifstream is(filename);
  string line;
  int edges = 0;
  while (getline(is, line)) {
    if (line[0] == '#')
      continue;
    stringstream ss(line);
    int source;
    int target;
    ss >> source;
    ss >> target;
    result[source].push_back(target);
    nodes.insert(source);
    nodes.insert(target);
    edges++;
  }
  cout << "Edges: " << edges << endl;
  return {result, nodes};
}

pair<AdjMat, Norm> toMatrix(const Graph &g, const NodeSet &nodes) { 
  //cout << "------------------" << endl;
  //printGraph(g);
  //cout << "------------------" << endl;

  Norm norm;
  int i = 0;
  for (int n : nodes) {
    norm[n] = i;
    i++;
  }

  AdjMat mat(nodes.size(), vector<int>(nodes.size(), 0));
  for (const auto &adj : g) {
    int src = adj.first;
    for (const int &tgt : adj.second) {
      // cout << src << ".." << tgt << endl;
      int nsrc = norm[src];
      int ntgt = norm[tgt];
      mat[nsrc][ntgt] = 1;
    }
  }
  return {mat, norm};
}

void fixgraph(Graph &g, NodeSet nodes) {
  for (int n : nodes) {
    if (g.count(n) == 0) {
      //cout << "Node " << n << endl;
      NodeSet adj(nodes);
      adj.erase(n);
      vector<int> adjN(adj.begin(), adj.end());
      g[n] = adjN;
    }
  }
  //printGraph(g);
}

vector<double> Pageiniciales(const NodeSet &nodes) {

  vector<double> PrI; // Declaracion PageRank iniciales

  for (int i = 0; i < nodes.size(); i++) {

    PrI.push_back(1.0 / ((double)nodes.size()));
    
  }
  double z=1.0/((double)nodes.size());
  cout << "Tamaño de PrI " << z << endl;

  return PrI;
}

vector<int> Pagelinks(const AdjMat &m) {

  int tamaño =m.size();
  vector<int> Pl(tamaño, 0);//Declaracion vector de links que salen de un nodo
  
  
  for (int i = 0; i < tamaño; i++) {

    for (int j = 0; j < tamaño; j++) {

      Pl[i] = Pl[i] + m[i][j];
    }
    /*
    if (Pl[i] == 0) {
      for (int l = 0; l < nodes.size(); l++) {
        if (i != l) {
          m.first[i][l] = 1;
        }
      }
      Pl[i] = nodes.size() - 1;
    }
  */
    
  }
  
  return Pl;
}

vector<double> PageRank(double &delta, vector<double> &PrI, vector<int> &Pl,
                        const AdjMat &m) {

  int iteraciones = 0;
  double parada;
  double d = 0.85;
  int numNodes = m.size();
  vector<double> PrN(numNodes, 0);//Declaracion del vector que va a tener los nuevos PageRank
  do {
    iteraciones++;
    parada = 0;
    for (int i = 0; i < numNodes; i++) {
      double aux = 0;
      for (int j = 0; j < numNodes; j++) {

        aux = aux + m[j][i] * (PrI[j] / Pl[j]);
      }
      PrN[i] = (1 - d) / numNodes + (d * aux);
    }

    

    for (int j = 0; j < numNodes; j++) {
      parada = parada + abs(PrI[j] - PrN[j]);
      PrI[j] = PrN[j];
      
    }
    
    //cout << "Iteracion " << iteraciones << endl;
    
  } while (parada > delta);
  cout<<"Numero de iteraciones: "<<iteraciones<<endl;
  cout<<"Valor de la parada: "<< parada <<endl;
  return PrN;
}


int main() {
  pair<Graph, NodeSet> g = readGraph("WikiTalk.txt");
  Graph graph = g.first;
  NodeSet nodes = g.second;
  cout << graph.size() << endl;
  cout << nodes.size() << endl;
  //pair<AdjMat, Norm> m = toMatrix(g.first, g.second);
  /*
   for(int i = 0; i < nodes.size(); i++) {
   for(int j = 0; j < nodes.size(); j++) {
    
      cout<<m.first[i][j]<<" ";
      }
      cout<<endl;
  }*/
  fixgraph(graph, nodes);
  //printGraph(graph);
  cout << graph.size() << endl;
  cout << nodes.size() << endl;

  pair<AdjMat, Norm> n = toMatrix(graph, g.second);

  /*for(int i = 0; i < nodes.size(); i++) {
   for(int j = 0; j < nodes.size(); j++) {
    
      cout<<n.first[i][j]<<" ";
      }
      cout<<endl;
  }*/
  
  vector<double> PrI = Pageiniciales(g.second);
  vector<int> Pl = Pagelinks(n.first);

  /*for(int i = 0; i < nodes.size(); i++) {
   for(int j = 0; j < nodes.size(); j++) {
    
      cout<<n.first[i][j]<<" ";
      }
      cout<<endl;
  }*/
  
  double delta = 0.0000001;
  vector<double> PrN = PageRank(delta, PrI, Pl, n.first);

  double suma = 0;
  for (int k = 0; k < nodes.size(); k++) {

    //cout << PrI[k] << endl;
    suma = suma + PrI[k];
  }

  cout << "Comprobar PageRank " << suma << endl;
 
  return 0;
}