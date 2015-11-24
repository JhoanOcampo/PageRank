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




int main(int argc, char **argv) {

  
  string ip, puerto1,puerto2;
  ip = argv[1];
  puerto1 = argv[2];
  puerto2 = argv[3];

  cout << "CLIENT\n";
  context ctx;

  socket s(ctx, socket_type::xreq);
  s.connect("tcp://localhost:"+puerto1);

  socket t(ctx, socket_type::xreq);
  t.connect("tcp://localhost:"+puerto2);

  //socket e(ctx, socket_type::push);
  //e.bind("tcp://*:" + puerto1);
pair<Graph, NodeSet> g = readGraph("a.txt");
  Graph graph = g.first;
  NodeSet nodes = g.second;
  cout << graph.size() << endl;
  cout << nodes.size() << endl;
  fixgraph(graph, nodes);
  //printGraph(graph);
  cout << graph.size() << endl;
  cout << nodes.size() << endl;

  pair<AdjMat, Norm> n = toMatrix(graph, g.second);
  vector<double> PrI = Pageiniciales(nodes);
  
  vector<int>rango1(2,0);
  int r;
  r=nodes.size()/2;

  int c=0;
    for(int i=0;i<2;i++){
      rango1[i]= r*c;
      c++;
      cout<<rango1[i]<<endl;
    }


  vector<int>rango2(2,0);
  int d=1;
    for(int i=0;i<2;i++){
      rango2[i]= r*d;
      d++;
      cout<<rango2[i]<<endl;
    }

    if(nodes.size()%2 != 0){
      rango2[1]=rango2[1]+1;
    }
    
    message o;

    for(int i=0;i<nodes.size();i++){
      o << rango1[i];
    }
    s.send(o);
    

    message p;

    for(int i=0;i<nodes.size();i++){
      p << rango2[i];
    }
    t.send(p);
    
//________________________
    double suma=0;
    bool bo=true;
    double delta=0.0000001;
//_____________________________
  while (bo==true) {
    suma=0;
    message PiS;
    for(int i=0;i<nodes.size();i++){
      PiS << PrI[i];
    }
    s.send(PiS);

    message PiS2;
    for(int i=0;i<nodes.size();i++){
      PiS2 << PrI[i];
    }
    t.send(PiS2);

    int cs1,cs2;
     message in1;
     s.receive(in1);
     in1 >> cs1;

     message in2;
     s.receive(in2);
     in2 >> cs2;
     //PrI.erase(first,last);

     

    
     vector<int>PrA1(in1.parts()-1,0);

     for(int i=0;i<in1.parts()-1;i++){
      in1 >>PrA1[i];
      //cout<<rango[i];
    }

    
     vector<int>PrA2(in2.parts()-1,0);

     for(int i=0;i<in2.parts()-1;i++){
      in2 >>PrA2[i];
      //cout<<rango[i];
    } 

    if(cs1 < cs2){
      for(int i=0;i<in2.parts()-1;i++){
        PrA1.push_back(PrA2[i]);
        }
      }else{
        for(int i=0;i<in1.parts()-1;i++){
          PrA2.push_back(PrA1[i]);
            }
        }
      cout<<"tamaño "<<PrA1.size()<<endl;
      cout<<"tamaño "<<PrA2.size()<<endl;

    for(int i=0;i<nodes.size();i++){
      if(PrA1.size() < PrA2.size()){
        suma=suma+ abs(PrI[i]-PrA2[i]);
        PrI[i]=PrA2[i];
      }else{
        suma=suma+ abs(PrI[i]-PrA1[i]);
        PrI[i]=PrA1[i];
        
      }
    }

    if(suma < delta){
      bo=false;
      for(int i=0;i<PrI.size();i++){
        cout<<PrI[i]<<endl;
      }
      cout<<"El valor de la parada es: "<<suma<<endl;
    }


  
  

  }
  return 0;
}