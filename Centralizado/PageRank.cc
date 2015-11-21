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
      vector<int> adj;
      for (int k : nodes)
        if (k != n)
          adj.push_back(k);
      g[n] = adj;
    }
  }
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

vector<int> Pagelinks(Graph &g, NodeSet &nodes, pair<AdjMat, Norm> &m) {

  vector<int> Pl(nodes.size(), 0);//Declaracion vector de links que salen de un nodo
  

  for (int i = 0; i < nodes.size(); i++) {

    for (int j = 0; j < nodes.size(); j++) {

      Pl[i] = Pl[i] + m.first[i][j];
    }
    if (Pl[i] == 0) {
      for (int j = 0; j < nodes.size(); j++) {
        if (i != j) {
          m.first[i][j] = 1;
        }
      }
      Pl[i] = nodes.size() - 1;
    }

    
  }
  
  return Pl;
}

vector<double> PageRank(double &delta, vector<double> &PrI, vector<int> &Pl,
                        Graph &g, NodeSet &nodes, pair<AdjMat, Norm> &m) {

  int iteraciones = 0;
  double parada;
  double d = 0.85;
  vector<double> PrN(nodes.size(), 0);//Declaracion del vector que va a tener los nuevos PageRank
  do {
    iteraciones++;
    parada = 0;
    for (int i = 0; i < nodes.size(); i++) {
      double aux = 0;
      for (int j = 0; j < nodes.size(); j++) {

        aux = aux + m.first[j][i] * (PrI[j] / Pl[j]);
      }
      PrN[i] = (1 - d) / nodes.size() + (d * aux);
    }

    

    for (int j = 0; j < nodes.size(); j++) {
      parada = parada + abs(PrI[j] - PrN[j]);
      PrI[j] = PrN[j];
      
    }
    
    
    
  } while (parada > delta);
  cout<<"Numero de iteraciones: "<<iteraciones<<endl;
  cout<<"Valor de la parada: "<< parada <<endl;
  return PrN;
}
int main() {
  pair<Graph, NodeSet> g = readGraph("Wiki-Vote.txt");
  Graph graph = g.first;
  NodeSet nodes = g.second;
  cout << graph.size() << endl;
  cout << nodes.size() << endl;
  fixgraph(graph, nodes);
  cout << graph.size() << endl;
  cout << nodes.size() << endl;

  pair<AdjMat, Norm> m = toMatrix(g.first, g.second);

  
  vector<double> PrI;
  PrI = Pageiniciales(g.second);

  vector<int> Pl;
  Pl = Pagelinks(g.first, g.second, m);

  
  double delta = 0.0000001;
  vector<double> PrN = PageRank(delta, PrI, Pl, g.first, g.second, m);

  double suma = 0;
  for (int i = 0; i < nodes.size(); i++) {

    cout << PrI[i] << endl;
    suma = suma + PrI[i];
  }

  cout << "Comprobar PageRank " << suma << endl;

  return 0;
}