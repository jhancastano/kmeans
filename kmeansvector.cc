#include "timer.hh"
#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <random>
#include <omp.h>

using namespace std;

using DataFrame = vector<double>;

inline int esmultiplo(int numero,int nVariables){
	if(nVariables%numero == 0){
		return 1;
	}
	else{
		return 0;
	}

}

inline double square(double value){
	return value * value;
}

inline double squared_12_distance(const DataFrame first,const int firstpoint, const DataFrame second,const int secondpoint ,const int nVariables){
	double d = 0.0;
	for(size_t dim = 0; dim < nVariables;dim++){
		d += square(first[firstpoint*nVariables + dim] - second[secondpoint*nVariables + dim]);
	}
	return d;
}

// returna Dataframe,vector kmeans(data,nCluster,nIteraciones,nVariables,epsilon) 
pair< DataFrame,vector<size_t> > k_means( const DataFrame& data, size_t k, size_t number_of_iterations,size_t nVariables, double ep,const int empty, const DataFrame Imeans){
	size_t dimensions = nVariables;
	// en proximo codigo colocar data.size/nvariables		   
	static random_device seed;
	static mt19937 random_number_generator(seed());
	uniform_int_distribution<size_t> indices(0,data.size()/nVariables -1);/// change		  
	// pick centroids as random points from the dataset
	DataFrame means(k*nVariables,0.0);// K*nvariables
	double distanciaepsilon;
	int contador = 0;
	size_t epsilon = numeric_limits<size_t>::max();
	//----------primeros means------------------------
	if(empty == 0){
	for(int y=0; y < k; y++ ){
		size_t i = indices(random_number_generator);
		for(int x=0;x < nVariables ;++x){
			means[y*nVariables+x] = data[i*nVariables+x];
		}
	}}
	if(empty == 1)
		means = Imeans;
	//---------------------------------------------
	vector<size_t> assignments(data.size()/nVariables);


	size_t iteration = 0;
	while( (iteration<number_of_iterations) && (contador != k)){
		// find assignements
		#pragma omp parallel for
		for (size_t point = 0; point < data.size()/nVariables ; ++point){
			double best_distance = numeric_limits<double>::max();// variable mejor distacia, inicializada con la maxima
			size_t best_cluster = 0; // variable mejor cluster, inicializada con 0
			for (size_t cluster = 0; cluster < k; ++cluster){
				const double distance = squared_12_distance( data ,point, means,cluster, nVariables);
				if(distance < best_distance){
				    best_distance = distance;
				    best_cluster = cluster;
				}
			}
		assignments[point] = best_cluster;
		}		
		DataFrame new_means(k*dimensions,0.0);
		DataFrame new_meansaux(k*dimensions,0.0);
		vector<size_t> counts(k, 0);
		for (size_t point = 0; point < data.size()/nVariables; ++point){
		    const size_t cluster = assignments[point];
		    for(size_t d = 0; d < dimensions; d++){
		    	new_means[cluster*nVariables + d] += data[point*nVariables + d];
		    }			
			counts[cluster] += 1;
		}
		// divide sumas por saltos para obtener centroides
		contador = 0;

		for (size_t cluster = 0; cluster < k; ++cluster){
			const size_t count = max<size_t>(1, counts[cluster]);
			for(size_t d = 0; d < dimensions;d++){
				new_meansaux[cluster * nVariables + d] = means[cluster * nVariables + d];
				means[cluster*nVariables + d] = new_means[cluster * nVariables + d] / count;
			}
			distanciaepsilon = squared_12_distance(new_meansaux,cluster,means,cluster,nVariables);
			if(distanciaepsilon < ep)
				contador++;
			}
			++iteration;
		
	}
	return {means, assignments};
}

DataFrame readData(string File,int nVariables ){
	DataFrame data;
	ifstream input(File);
	string line;
	while(getline(input,line)){
		istringstream iss(line);
		double v;
		for(int i = 0;i < nVariables; i++){
			iss >> v;
			data.push_back(v);
			}
		}
		cout << data.size() << endl;
		return data;
}


void imprimirkameans(vector<size_t> m,int k){
	vector<int> v(k);
	for(int i = 0; i < m.size(); i++) {
  	v[m[i]]++;
  	}
  	for(int x = 0; x<v.size(); x++){
  		cout << "k_means" << x << " -> "<<v[x] <<endl;
  	}
}

void printmens(DataFrame means,int nVariables){
	for(int i=0; i<means.size();i++){
		if (esmultiplo(nVariables,i)==1)
		{
			cout<<'|'<<endl;
		}
		cout << means[i]<<',';
	}
	cout<<'|'<<endl;
}


int main(){
	// main
	string dataset;
	int numeroVariables;
	int numeroCluster;
	int numeroIT;
	double epsilon;

	//cout << "ingrese nombre dataset"<<endl;
	//cin >>dataset;
	//cout << "ingrese numero variables dataset"<<endl;
	//cin >> numeroVariables;
	//cout << "ingrese numero de cluster o k"<<endl;
	//cin >> numeroCluster;
	//cout << "ingrese numero de iteraciones"<<endl;
	//cin >> numeroIT;
	//cout << "ingrese epsilon de convergencia ej(0.1)"<<endl;
	//cin >> epsilon;
	


	dataset= "DATASETS/arrhythmia.dat";
	numeroVariables = 279;
	numeroCluster = 13;
	numeroIT = 10;
	epsilon = 0.001;

	DataFrame data = readData(dataset,numeroVariables);
	DataFrame means = readData("DATASETS/arrhythmiaMeans",numeroVariables);
	DataFrame c;
	vector<size_t> a;
	
		ofstream archivo;
		archivo.open("tiemposkmeansvectorpiris.csv",ios::out);
		if(archivo.fail()){
			cout<<"error"<<endl;
			exit(1);
		}
		for(int i=0;i<1;i++){
			ScopedTimer t;
			tie(c,a) = k_means(data,numeroCluster,numeroIT,numeroVariables,epsilon,1,means);
			archivo<<t.elapsed()<<endl;
			//cout <<  " tiempo : " << t.elapsed()<< "ms" << endl;
		}
	




	//ScopedTimer t;
	//kmeans(data,nCluster,nIteraciones,nVariables,epsilon)
	//tie(c,a) = k_means(data,numeroCluster,numeroIT,numeroVariables,epsilon,1,means);
	//cout <<  " tiempo : " << t.elapsed()<< "ms" << endl;

	//imprimirkameans(a,numeroCluster);
	//printmens(c,numeroVariables);
	
	return 0;

}
