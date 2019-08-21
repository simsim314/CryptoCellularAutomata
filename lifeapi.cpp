#include <iostream> 
#include "soup.h"
#include "picosha2.h"
#include <chrono> 

using namespace picosha2;
typedef unsigned long long uvlong; 

int main()
{

	const string wallet = "0x18c4479a8Fca5D701575fd4647Ed7bC5198D4CE4";
	int total = 0; 
	int id; 
	commons	cm; 
	//stats st; 
	
	// Record start time
  auto start = std::chrono::high_resolution_clock::now();

	for(uvlong i = 1;; i++)
	{
		string src_str = wallet + to_string(i);
		
		vector<uchar> hash(k_digest_size);
		hash256(src_str.begin(), src_str.end(), hash.begin(), hash.end());

		lifeanalyzer ls(hash);
		ls.evolvetostability();
		
		if(ls.period == 0)
			continue; 
		
		cm.removeall(ls.soup);
		
		if(!ls.soup.isempty())
		{	
			lifestate ls1 = ls.soup; 
			ls.soup.evolve();
			
			if(!ls1.issame(ls.soup))
				continue; 
			
			int dx, dy; 
			ls.soup.refittorus(dx, dy);
			//cout << dx << "," << dy; 
			string name = slcanonise(ls.soup);
			
			//if(st.census.find(name) != st.census.end())
			//	cout << i << "," << name << ":" << st.census[name] << "\n";
			
			//cout << i << ":" <<  << "\n";
			
			
			//ls.soup.printrect();
			
		}

		if(i % 1000 == 0)
		{
			auto finish = std::chrono::high_resolution_clock::now();

			std::chrono::duration<double> elapsed = finish - start;
			double duration = elapsed.count();
			
			cout << i << ":" << i / duration << " Soup in sec\n";
		}
		
	}
}