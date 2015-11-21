#include <iostream>
#include <string>
#include <cassert>
#include <zmqpp/zmqpp.hpp>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>    // std::swap
#include <cmath> 


using namespace std;
using namespace zmqpp;

using Graph = unordered_map<int,vector<int>>;
using NodeSet = unordered_set<int>; 
using AdjMat = vector<vector<int>>;
using Norm = unordered_map<int,int>;

pair<Graph,NodeSet> readGraph(const string& filename) {
	Graph result;
	NodeSet nodes;
	ifstream is(filename);
	string line;
	int edges = 0;
	while(getline(is,line)) {
		if(line[0]=='#') continue;
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
	return {result,nodes};
}

pair<AdjMat,Norm> toMatrix(const Graph& g, const NodeSet& nodes) { // pair
	Norm norm;
	int i = 0;
	for (int n : nodes) {
		norm[n] = i;
		i++;
	}
	
	AdjMat mat(nodes.size(),vector<int>(nodes.size(),0));
	for(const auto& adj : g) {
		int src = adj.first;
		for(const int& tgt : adj.second){
			//cout << src << ".." << tgt << endl;
			int nsrc = norm[src];
			int ntgt = norm[tgt];
			mat[nsrc][ntgt] = 1;
		}
	}
	return {mat,norm};
}


void fixgraph (Graph& g, NodeSet nodes){

	for (int n: nodes)
	{
		if (g.count(n)==0)
		{
			vector <int>adj;
			for(int k:nodes) 
				if (k!=n)
					adj.push_back(k);
			g[n]=adj;

		}
	}
}

int main(){
  pair<Graph,NodeSet> g = readGraph("Wiki-Vote.txt");
  Graph graph = g.first;
  NodeSet nodes = g.second;
  cout << graph.size() << endl;
  cout << nodes.size() << endl;
  fixgraph(graph,nodes);
  cout << graph.size() << endl;
  cout << nodes.size() << endl;
  
  pair<AdjMat,Norm> m = toMatrix(g.first,g.second);
  //int i = m.second[3];
  //int j = m.second[604];
  //cout << m.first[i][j] << endl;

//Para ver la matriz si es pequeña
 /* for(int i = 0; i < nodes.size(); i++) {
 	 for(int j = 0; j < nodes.size(); j++) {
    
    	cout<<m.first[i][j]<<" ";
  		}
  		cout<<endl;
	}*/
//____________________________________-
	cout<<"cantidad de nodos "<<nodes.size()<<endl;
	vector<double> PrI(nodes.size(),0);
	//double PrI[nodes.size()];//Declaracion page rank iniciales
	for(int i=0;i < nodes.size();i++){


		PrI[i]=1.0/((double)nodes.size());
	}

	double z=1.0/((double)nodes.size());
	cout<<"Valores iniciales de PageRank " << z<<endl;

	
	/*for(int i=0;i < nodes.size();i++){
		//cout<<PrI[i];
		cout <<" ";
	}
	cout<<endl;*/
	vector<double> Pl(nodes.size(),0);
	//int Pl[nodes.size()];// declaracion vector de pl
	//Pl.erase;
	vector<double> PrN(nodes.size(),0);
	//double PrN[nodes.size()];//declaracion vector Page rank nuevos
	double delta = 0.00001;
	double d=0.85;
	double parada=0;
	
	for(int i = 0; i < nodes.size(); i++) {
 	 for(int j = 0; j < nodes.size(); j++) {
    	
    	Pl[i]= Pl[i] + m.first[i][j];
  		}
	  		if(Pl[i]==0){
	  			for(int j = 0; j < nodes.size(); j++) {
		  			if(i != j){	
		    			m.first[i][j]=1;
		    		}
	  			}
	  			Pl[i]= nodes.size()-1;
	  		}
  		
  		
  		//cout<<Pl[i]<<" ";
	}

	cout<<endl<<endl;

//Imprime la matriz
	/*for(int i = 0; i < nodes.size(); i++) {
 	 for(int j = 0; j < nodes.size(); j++) {
    
    	cout<<m.first[i][j]<<" ";
  		}
  		cout<<endl;
	}*/
cout<<endl;
//int f=0;
do{
	parada = 0;
	for(int i = 0; i < nodes.size(); i++) {
		double aux=0;
 	 for(int j = 0; j < nodes.size(); j++) {    	

    	aux= aux + m.first[j][i]*(PrI[j]/Pl[j]);
  		}
  		PrN[i]= (1 - d)/nodes.size() + (d *aux);
  		
  		
	}
	
	cout<<endl;

	for(int j = 0; j < nodes.size(); j++) {
		parada= parada + abs(PrI[j]-PrN[j]);
    	PrI[j]=PrN[j];
    	//cout<<PrI[j]<<" ";
  		}
  		cout<<endl;
  		//cout<< parada <<"  ";
//f++;
}while(parada > delta);
	//__________________________________________
	
double suma= 0;		
for(int i=0;i<nodes.size();i++){
	
	cout<<PrI[i]<<endl;
	suma=suma + PrI[i];
} 	 

cout<<"Comprobar PageRank "<<suma<<endl;
  		
  		
  		





  return 0;
}