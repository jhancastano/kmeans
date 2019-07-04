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

inline double square(double value){
	return value * value;
}

inline double squared_12_distance(const DataFrame first,int firstpoint, const DataFrame second,int secondpoint , int nVariables){
	double d = 0.0;
	for(size_t dim = 0; dim < nVariables;dim++){
		d += square(first[firstpoint + dim] - second[secondpoint + dim]);
	}
	return d;
}

pair< DataFrame,vector<size_t> > k_means( const DataFrame& data, size_t k, size_t number_of_iterations,size_t nVariables, double ep){
	size_t dimensions = nVariables;
	// en proximo codigo colocar data.size/nvariables		   
	static random_device seed;
	static mt19937 random_number_generator(seed());
	
	uniform_int_distribution<size_t> indices(0,data.size()/nVariables -1);/// change		  
	// pick centroids as random points from the dataset
	DataFrame means(k*nVariables,0.0);// K*nvariables
	
	double distanciaepsilon;
	size_t contador;
	size_t epsilon = numeric_limits<size_t>::max();


	for(int y=0; y < k; y++ ){
		size_t i = indices(random_number_generator);
		for(int x=0;x<nVariables ;x++){
			means[y*nVariables+x] = data[x+i*nVariables];
		}
	}


	vector<size_t> assignments(data.size()/nVariables);

	
	#pragma omp parallel for
	for(size_t iteration = 0; iteration < number_of_iterations; ++iteration){

		if(ep > epsilon ){
			iteration = number_of_iterations + 1;
		}
		// find assignements
		for (size_t point = 0; point < data.size()/nVariables ; ++point){
			double best_distance = numeric_limits<double>::max();// variable mejor distacia, inicializada con la maxima
			size_t best_cluster = 0; // variable mejor cluster, inicializada con 0
			for (size_t cluster = 0; cluster < k; cluster++){
				const double distance = squared_12_distance(data,point,means,cluster,nVariables);
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
		for (size_t cluster = 0; cluster < k; ++cluster){
			const size_t count = max<size_t>(1, counts[cluster]);
			for(size_t d = 0; d < dimensions;d++){
				new_meansaux[cluster * nVariables + d] = means[cluster * nVariables + d];
				means[cluster*nVariables + d] = new_means[cluster * nVariables + d] / count;
			}
			distanciaepsilon = squared_12_distance(new_meansaux,cluster,means,cluster,nVariables);
			//cout << new_meansaux[cluster][0] <<'|'<< new_meansaux[cluster][1] <<'|'<< new_meansaux[cluster][2]<< endl;
			//cout << means[cluster][0] <<'|'<< means[cluster][1] <<'|'<< means[cluster][2]<< endl;
			if(distanciaepsilon < ep){
				//cout << "exito" << endl;
				contador++;
			}
			else{//cout << "menor 1" << endl;
				contador = 0;}
			if(contador > k){
				cout << iteration <<endl;
				iteration = number_of_iterations + 1;
			}			
		}
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
	cout << "prueba" << endl; 
	for(int i = 0; i < m.size(); i++) {
  	//cout << "point " << i << " -> " << a[i] << endl;
	//cout << m[i] <<endl;
	//cout << data[i][0]<<'|'<<data[i][1]<< '|'<< data[i][2]<<'|'<<data[i][3]<< " -> "<< m[i] <<endl;
  	v[m[i]]++;
  	}

  	cout << v.size() << endl;
  	for(int x = 0; x<v.size(); x++){
  		cout << "k_means" << x << " -> "<<v[x] <<endl;
  	}
}


int main(){
	// main
	cout << "k_means"<< endl;
	DataFrame data = readData("iris.data",4);
	cout << data.size() << endl;
	DataFrame c;
	vector<size_t> a;
	//cout << data << endl;
	ScopedTimer t;

	tie(c,a) = k_means(data,3,10000,4,0.0);
	cout <<  " tiempo : " << t.elapsed()<< "ms" << endl;

	imprimirkameans(a,3);

	
	return 0;

}
