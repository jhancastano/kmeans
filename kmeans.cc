//#include "timer.hh"
#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <random>


using namespace std;


using Point = vector<double>;

using DataFrame = vector<Point>;

inline double square(double value){
	return value * value;
}

inline double squared_12_distance(const Point first,const Point second){
	double d = 0.0;
	for(size_t dim = 0; dim < first.size();dim++){
		d += square(first[dim]-second[dim]);
	}
	return d;
}

DataFrame k_means( const DataFrame& data, size_t k, size_t number_of_iterations){
	
	size_t dimensions = data[0].size();		   
	static random_device seed;
	static mt19937 random_number_generator(seed());
	uniform_int_distribution<size_t> indices(0,data.size()-1);		  
	
	// pick centroids as random points from the dataset
	DataFrame means(k);
	
	for (Point& cluster : means){ // cluster -> means
		size_t i = indices(random_number_generator);
		cluster = data[i];		
	}				   
	vector<size_t>assignments(data.size());

	for(size_t iteration = 0; iteration < number_of_iterations; iteration++){
		// find assignements
		for (size_t point = 0; point < data.size() ; point++){
			double best_distance = numeric_limits<double>::max();// variable mejor distacia, inicializada con la maxima
			size_t best_cluster = 0; // variable mejor cluster, inicializada con 0
			for (size_t cluster = 0; cluster < k; ++cluster){
				const double distance = squared_12_distance(data[point], means[cluster]);
				if(distance < best_distance){
				    best_distance = distance;
				    best_cluster = cluster;
				}
			}


		assignments[point] = best_cluster;
		}
		
		DataFrame new_means(k,vector<double>(dimensions,0.0));
		vector<size_t> counts(k, 0);

		for (size_t point = 0; point < data.size(); point++){
		    const size_t cluster = assignments[point];
		    for(size_t d = 0; d < dimensions; d++){
		    	new_means[cluster][d] += data[point][d];
		    }			
			counts[cluster] += 1;
		}
		
		// divide sumas por saltos para obtener centroides
		for (size_t cluster = 0; cluster < k; ++cluster){
			const size_t count = max<size_t>(1, counts[cluster]);
			for(size_t d = 0; d < dimensions;d++){
				means[cluster][d] = new_means[cluster][d] / count;
			}			
		}
	}	
	return means;
}


DataFrame readData(string File){
	DataFrame data;
	ifstream input(File);
	string line;
	while(getline(input,line)){
		istringstream iss(line);
		double x, y, z;
		string a;
		iss >> x >> y >> z;
		Point p;
		p.push_back(x);
		p.push_back(y);
		p.push_back(z);
		data.push_back(p);
		}
		cout << data.size() << endl;
		return data;
}



int main(){
	// main para generar un data set random (mejorar con un dataset externo)
	cout << "k_means"<< endl;
	DataFrame data = readData("iris.data");
	cout << data.size() << endl;
	DataFrame c;
	//vector<size_t> a;
	//cout << data << endl;
	//ScopedTimer t;
	k_means(data,3,100);
	//cout << "tiempo" << t.elapsed() << endl;
	cout << data.size() << "fin" <<endl;

	//cout << c[1] << endl;
	return 0;

}
