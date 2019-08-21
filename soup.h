#pragma once
#include <iostream> 
#include <vector> 

using namespace std; 
typedef unsigned char uchar; 

uint64_t inline rotateleft(const uint64_t& val, int d = 1)
{
	return (val << d) | (val >> (64 - d)); 
}

uint64_t inline rotateright(const uint64_t& val, int d = 1)
{
	return (val >> d) | (val << (64 - d)); 
}

class lifestate
{
public: 
	uint64_t state[64] ; 
	uint64_t bits0[64] ; 
	uint64_t bits1[64] ; 
	uint8_t minidx; 
	uint8_t maxidx; 
	int gen; 
	
	lifestate()
	{
		gen = 0; 
		
		for(int i = 0; i < 64; i++)
		{
			state[i] = 0; 
			bits0[i] = 0;
			bits1[i] = 0;
		}	
	}
	
	lifestate(const vector<uchar>& m)
	{
		for(int i = 0; i < 64; i++)
		{
			state[i] = 0; 
			bits0[i] = 0;
			bits1[i] = 0;
		}
		
		for(int i = 24; i < 40; i++)
		{
			int j = 2 * (i - 24);
			uint64_t val0 = (uint64_t)m[j];
			uint64_t val1 = (uint64_t)m[j+1];
			
			val0 = val0 << 24; 
			val1 = val1 << 32; 
			
			state[i] = val0 | val1; 
		}
		
		minidx = 0; 
		maxidx = 63; 
	
		recalcminmax();
	
	}
	
	bool isempty() const
	{
		for(int i = 0; i < 64; i++)
		{
			if(state[i] != 0)
			{
				return false; 
			}
		}	

		return true; 
	}
	
	bool issame(const lifestate& other) const
	{
		for(int i = 0; i < 64; i++)
			if(other.state[i] != state[i])
				return false; 
			
		return true; 
	}
	
	void set(int x, int y)
	{
		state[y] |= 1ULL << x; 
	}
	
	int get(int x, int y) const
	{
		return (state[y] >> (63 - x)) % 2;
	}
	
	void rotatetorusy()
	{
		uint64_t prev = state[0];
		
		for(int i = 0; i < 63; i++)
			state[i] = state[i + 1];
		
		state[63] = prev;
	}
	
	void rotatetorusx()
	{
		for(int i = 0; i < 64; i++)
			state[i] = rotateleft(state[i]);
	}
	
	void refittorus(int& x, int& y)
	{
		x = 0; 
		y = 0; 
		
		for(int i = 0; i < 64; i++)
		{
			int rect[4];
			getrect(rect);
			
			if(rect[2] == 64)
			{
				rotatetorusx();
				x++; 
			}
			
			if(rect[3] == 64)
			{
				rotatetorusy();
				y++; 
			}
			
			if(rect[2] < 64 && rect[3] < 64)
				break; 
		}
	}
	
	void printrawrange(uint64_t val, int start, int end) const
	{
		for(int i = start; i < end; i++)
			if((val >> (63 - i)) % 2 == 0)
				cout << "_";
			else
				cout << "x";
			
		cout << "\n";
	}
	
	void printraw(uint64_t val) const
	{
		for(int i = 0; i < 64; i++)
			if((val >> i) % 2 == 0)
				cout << "_";
			else
				cout << "x";
			
		cout << "\n";
	}
	
	void printrect() const
	{
		int rect[4];
		getrect(rect);
		
		for(int i = rect[1]; i < rect[1] + rect[3]; i++)
			printrawrange(state[i], rect[0], rect[0] + rect[2]);
	}
	void print() const
	{
		for(int i = 0; i < 64; i++)
			printraw(state[i]);
	}
	
	void printrawxy(uint64_t val, uchar y) const
	{
		for(int i = 0; i < 64; i++)
			if((val >> i) % 2 == 1)
				cout << i << ", " << y << ", ";
	}
	
	void printxy() const
	{
		for(int i = 0; i < 64; i++)
			printrawxy(state[i], i);
	}
	
	void inline addinit(uint64_t& b1, uint64_t& b0, const uint64_t& val)
	{
		b1 = b0 & val;
		b0 ^= val;
	}

	void inline add(uint64_t& b2, uint64_t& b1, uint64_t &b0, const uint64_t& val)
	{
		uint64_t t_b2 = b0 & val;

		b2 |= t_b2 & b1;
		b1 ^= t_b2;
		b0 ^= val;
	}

	void inline addinit(uint64_t& b2, uint64_t& b1, uint64_t &b0, uint64_t& val)
	{
		uint64_t t_b2 = b0 & val;

		b2 = t_b2&b1;
		b1 ^= t_b2;
		b0 ^= val;
	}
	
	void inline add(uint64_t& b1, uint64_t &b0, const uint64_t& val)
	{
		b1 |= b0 & val;
		b0 ^= val;
	}
	
	void calcsum(const uint64_t& val, uint64_t& bit0, uint64_t& bit1)
	{
		uint64_t l = rotateleft(val);
		uint64_t r = rotateright(val);
		bit0 = l ^ r ^ val;
		bit1 = ((l | r) & val) | (l & r);
	}
	
	void recalcminmax()
	{
		while(state[minidx] == 0 && minidx < 64)
		{
			bits0[minidx] = 0;
			bits1[minidx] = 0;
			
			minidx++;
		}
		
		while(state[maxidx] == 0 && maxidx >= 0)
		{
			bits0[maxidx] = 0;
			bits1[maxidx] = 0;
			
			maxidx--;
		}
		
		if(maxidx == 63)
			minidx = 0; 
		
		if(minidx == 0)
			maxidx = 63; 
	}
	
	bool run(int iters)
	{
		for(int i = 0; i < iters; i++)
			if(!evolve())
				return false;				
		
		return true; 
	}
	
	bool evolve()
	{
		int firstidx, lastidx; 
		
		firstidx = minidx; 
		lastidx = maxidx;
		gen++; 
		
		if(firstidx > 0)
			firstidx--; 
		
		if(lastidx < 63)
			lastidx++; 
		
		minidx = firstidx; 
		maxidx = lastidx; 
		
		for(int i = firstidx; i <= lastidx; i++)
			calcsum(state[i], bits0[i], bits1[i]);

		for(int i = firstidx; i <= lastidx; i++)
		{
			uint64_t val = state[i];
			int next = (i + 1) % 64;
			int prev = (i + 63) % 64;
			
			uint64_t bitr0 =  bits0[next];
			uint64_t bitr1 =  bits1[next];
			uint64_t bitl0 =  bits0[prev];
			uint64_t bitl1 =  bits1[prev];
			
			uint64_t sum0, sum1, sum2;
			sum0 = rotateleft(val);
			addinit(sum1, sum0, rotateright(val));
			add(sum1, sum0, bitl0);
			addinit(sum2, sum1, bitl1);
			add(sum2, sum1, sum0, bitr0);
			add(sum2, sum1, bitr1);
			state[i] = ~sum2 & sum1 & (val | sum0);
		}
		
		if(isempty())
			return false; 
		
		recalcminmax();
		
		return true;
	}
	
	//will not work for torus cut - but will work many times. 
	void getrect(int rect[4]) const
	{
		for(int i = 0; i < 64; i++)
		{
			if(state[i] != 0)
			{	
				rect[1] = i; 
				break; 
			}
		}
		
		for(int i = 63; i >= 0; i--)
		{
			if(state[i] != 0)
			{	
				rect[3] = i - rect[1] + 1; 
				break; 
			}
		}
		
		uint64_t orstate = 0; 
		
		for(int i = rect[1]; i < rect[1] + rect[3]; i++)
			orstate |= state[i];
		
		int lasti = -1; 
		int firsti; 
		
		for(int i = 63; i >= 0; i--)
		{
			if(orstate % 2 == 1)
			{
				if(lasti == -1)
					lasti = i; 
				
				firsti = i; 
			}	
			
			orstate = orstate >> 1;
		}
		
		rect[0] = firsti; 
		rect[2] = lasti - firsti + 1; 	
	}
	
	//not optimized used rarelly in soup search
	int getpop() const
	{
		int rect[4];
		getrect(rect);
		int pop = 0; 
		
		for(int i = rect[0]; i < rect[0] + rect[2]; i++)
			for(int j = rect[1]; j < rect[1] + rect[3]; j++)
				pop += get(i, j);
			
		return pop; 
	}
};

class lifeanalyzer
{
public:	
	lifestate captures[15]; 
	lifestate soup; 
	int period; 
	
	lifeanalyzer(const vector<uchar>& m)
	{
		period = 0; 
		soup = lifestate(m);
	}
	
	void evolvetostability()
	{
		const int STEP = 200; 
		
		if(!soup.run(STEP))
			return; 
		
		for(int i = 0; i < 15; i++)
		{
			captures[i] = soup; 
			
			if(!soup.run(56))
				return; 
			
			if(i >= 1)
			{
				if(soup.issame(captures[i - 1]))
				{
					period = 256; 
					break;
				}	
			}	
					
			if(!soup.run(4))
				return; 
			
			if(soup.issame(captures[i]))
			{
				period = 60; 
				break;
			}	
			
			if(!soup.run(STEP - 60))
				return; 
		}
	}
};

struct locatorbit
{ 
	int x; 
	int y; 
	bool negate; 
	
	locatorbit(int inx, int iny, bool innegate)
	{
		x = inx; 
		y = iny;
		negate = innegate; 
	}
};

class locator
{
public: 

	vector<locatorbit> locations; 
	uint64_t stateresult[64]; 

	locator()
	{
		for(int i = 0; i < 64; i++)
			stateresult[i] = 0; 	
	}
	
	locator(const lifestate& state)
	{
		init(state);
	}
	
	void init(const lifestate& state)
	{
		for(int i = 0; i < 64; i++)
			stateresult[i] = state.state[i]; 
	}
	
	bool crosslocate(const lifestate& state, const locatorbit& loc)
	{
		bool all0 = true; 
		
		for(int i = 0; i < 64; i++)
		{
			if(loc.negate)
				stateresult[i] &= ~rotateleft(state.state[(i + loc.y + 64) % 64],((loc.x + 64) % 64));
			else 
				stateresult[i] &= rotateleft(state.state[(i + loc.y + 64) % 64], ((loc.x + 64) % 64));
			
			if(stateresult[i] != 0)
				all0 = false; 
		}
		
		return !all0; 
	}
	
	bool locate(const lifestate& state)
	{
		for(int i = 0; i < locations.size(); i++)
		{
			if(crosslocate(state, locations[i]) == false)
				return false; 
		}
		
		return true; 
	}
	
	void remove(lifestate& state)
	{
		for(int j = 0; j < locations.size(); j++)
		{
			auto loc = locations[j]; 
			
			if(loc.negate)
				continue; 
			
			for(int i = state.minidx; i <= state.maxidx; i++)
				state.state[i] ^= rotateright(stateresult[(i - loc.y + 64) % 64],((loc.x + 64) % 64));
		}
	}
};

class commons
{
public: 
	vector<locator> locators; 
	
	void removeall(lifestate& stable)
	{
		for(int i = 0; i < locators.size(); i++)
		{
			if(stable.isempty())
				return; 
			
			auto lc = locators[i]; 
			lc.init(stable);
			
			if(lc.locate(stable))
				lc.remove(stable);
		}
	}
	commons()
	{
		locators.push_back(locator());
		locators[0].locations.push_back(locatorbit(-1,0,false));
		locators[0].locations.push_back(locatorbit(-1,1,false));
		locators[0].locations.push_back(locatorbit(0,0,false));
		locators[0].locations.push_back(locatorbit(0,1,false));
		locators[0].locations.push_back(locatorbit(-2,-1,true));
		locators[0].locations.push_back(locatorbit(-2,0,true));
		locators[0].locations.push_back(locatorbit(-2,1,true));
		locators[0].locations.push_back(locatorbit(-2,2,true));
		locators[0].locations.push_back(locatorbit(-1,-1,true));
		locators[0].locations.push_back(locatorbit(-1,2,true));
		locators[0].locations.push_back(locatorbit(0,-1,true));
		locators[0].locations.push_back(locatorbit(0,2,true));
		locators[0].locations.push_back(locatorbit(1,-1,true));
		locators[0].locations.push_back(locatorbit(1,0,true));
		locators[0].locations.push_back(locatorbit(1,1,true));
		locators[0].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[1].locations.push_back(locatorbit(-2,0,false));
		locators[1].locations.push_back(locatorbit(-1,0,false));
		locators[1].locations.push_back(locatorbit(0,0,false));
		locators[1].locations.push_back(locatorbit(-3,-1,true));
		locators[1].locations.push_back(locatorbit(-3,0,true));
		locators[1].locations.push_back(locatorbit(-3,1,true));
		locators[1].locations.push_back(locatorbit(-2,-1,true));
		locators[1].locations.push_back(locatorbit(-2,1,true));
		locators[1].locations.push_back(locatorbit(-1,-1,true));
		locators[1].locations.push_back(locatorbit(-1,1,true));
		locators[1].locations.push_back(locatorbit(0,-1,true));
		locators[1].locations.push_back(locatorbit(0,1,true));
		locators[1].locations.push_back(locatorbit(1,-1,true));
		locators[1].locations.push_back(locatorbit(1,0,true));
		locators[1].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[2].locations.push_back(locatorbit(0,0,false));
		locators[2].locations.push_back(locatorbit(0,1,false));
		locators[2].locations.push_back(locatorbit(0,2,false));
		locators[2].locations.push_back(locatorbit(-1,-1,true));
		locators[2].locations.push_back(locatorbit(-1,0,true));
		locators[2].locations.push_back(locatorbit(-1,1,true));
		locators[2].locations.push_back(locatorbit(-1,2,true));
		locators[2].locations.push_back(locatorbit(-1,3,true));
		locators[2].locations.push_back(locatorbit(0,-1,true));
		locators[2].locations.push_back(locatorbit(0,3,true));
		locators[2].locations.push_back(locatorbit(1,-1,true));
		locators[2].locations.push_back(locatorbit(1,0,true));
		locators[2].locations.push_back(locatorbit(1,1,true));
		locators[2].locations.push_back(locatorbit(1,2,true));
		locators[2].locations.push_back(locatorbit(1,3,true));
		locators.push_back(locator());
		locators[3].locations.push_back(locatorbit(-2,0,false));
		locators[3].locations.push_back(locatorbit(-2,1,false));
		locators[3].locations.push_back(locatorbit(-1,-1,false));
		locators[3].locations.push_back(locatorbit(-1,2,false));
		locators[3].locations.push_back(locatorbit(0,0,false));
		locators[3].locations.push_back(locatorbit(0,1,false));
		locators[3].locations.push_back(locatorbit(-3,-1,true));
		locators[3].locations.push_back(locatorbit(-3,0,true));
		locators[3].locations.push_back(locatorbit(-3,1,true));
		locators[3].locations.push_back(locatorbit(-3,2,true));
		locators[3].locations.push_back(locatorbit(-2,-2,true));
		locators[3].locations.push_back(locatorbit(-2,-1,true));
		locators[3].locations.push_back(locatorbit(-2,2,true));
		locators[3].locations.push_back(locatorbit(-2,3,true));
		locators[3].locations.push_back(locatorbit(-1,-2,true));
		locators[3].locations.push_back(locatorbit(-1,0,true));
		locators[3].locations.push_back(locatorbit(-1,1,true));
		locators[3].locations.push_back(locatorbit(-1,3,true));
		locators[3].locations.push_back(locatorbit(0,-2,true));
		locators[3].locations.push_back(locatorbit(0,-1,true));
		locators[3].locations.push_back(locatorbit(0,2,true));
		locators[3].locations.push_back(locatorbit(0,3,true));
		locators[3].locations.push_back(locatorbit(1,-1,true));
		locators[3].locations.push_back(locatorbit(1,0,true));
		locators[3].locations.push_back(locatorbit(1,1,true));
		locators[3].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[4].locations.push_back(locatorbit(-3,0,false));
		locators[4].locations.push_back(locatorbit(-2,-1,false));
		locators[4].locations.push_back(locatorbit(-2,1,false));
		locators[4].locations.push_back(locatorbit(-1,-1,false));
		locators[4].locations.push_back(locatorbit(-1,1,false));
		locators[4].locations.push_back(locatorbit(0,0,false));
		locators[4].locations.push_back(locatorbit(-4,-1,true));
		locators[4].locations.push_back(locatorbit(-4,0,true));
		locators[4].locations.push_back(locatorbit(-4,1,true));
		locators[4].locations.push_back(locatorbit(-3,-2,true));
		locators[4].locations.push_back(locatorbit(-3,-1,true));
		locators[4].locations.push_back(locatorbit(-3,1,true));
		locators[4].locations.push_back(locatorbit(-3,2,true));
		locators[4].locations.push_back(locatorbit(-2,-2,true));
		locators[4].locations.push_back(locatorbit(-2,0,true));
		locators[4].locations.push_back(locatorbit(-2,2,true));
		locators[4].locations.push_back(locatorbit(-1,-2,true));
		locators[4].locations.push_back(locatorbit(-1,0,true));
		locators[4].locations.push_back(locatorbit(-1,2,true));
		locators[4].locations.push_back(locatorbit(0,-2,true));
		locators[4].locations.push_back(locatorbit(0,-1,true));
		locators[4].locations.push_back(locatorbit(0,1,true));
		locators[4].locations.push_back(locatorbit(0,2,true));
		locators[4].locations.push_back(locatorbit(1,-1,true));
		locators[4].locations.push_back(locatorbit(1,0,true));
		locators[4].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[5].locations.push_back(locatorbit(-3,0,false));
		locators[5].locations.push_back(locatorbit(-2,-1,false));
		locators[5].locations.push_back(locatorbit(-2,1,false));
		locators[5].locations.push_back(locatorbit(-1,-1,false));
		locators[5].locations.push_back(locatorbit(-1,2,false));
		locators[5].locations.push_back(locatorbit(0,0,false));
		locators[5].locations.push_back(locatorbit(0,1,false));
		locators[5].locations.push_back(locatorbit(-4,-1,true));
		locators[5].locations.push_back(locatorbit(-4,0,true));
		locators[5].locations.push_back(locatorbit(-4,1,true));
		locators[5].locations.push_back(locatorbit(-3,-2,true));
		locators[5].locations.push_back(locatorbit(-3,-1,true));
		locators[5].locations.push_back(locatorbit(-3,1,true));
		locators[5].locations.push_back(locatorbit(-3,2,true));
		locators[5].locations.push_back(locatorbit(-2,-2,true));
		locators[5].locations.push_back(locatorbit(-2,0,true));
		locators[5].locations.push_back(locatorbit(-2,2,true));
		locators[5].locations.push_back(locatorbit(-2,3,true));
		locators[5].locations.push_back(locatorbit(-1,-2,true));
		locators[5].locations.push_back(locatorbit(-1,0,true));
		locators[5].locations.push_back(locatorbit(-1,1,true));
		locators[5].locations.push_back(locatorbit(-1,3,true));
		locators[5].locations.push_back(locatorbit(0,-2,true));
		locators[5].locations.push_back(locatorbit(0,-1,true));
		locators[5].locations.push_back(locatorbit(0,2,true));
		locators[5].locations.push_back(locatorbit(0,3,true));
		locators[5].locations.push_back(locatorbit(1,-1,true));
		locators[5].locations.push_back(locatorbit(1,0,true));
		locators[5].locations.push_back(locatorbit(1,1,true));
		locators[5].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[6].locations.push_back(locatorbit(-3,-1,false));
		locators[6].locations.push_back(locatorbit(-3,0,false));
		locators[6].locations.push_back(locatorbit(-2,-2,false));
		locators[6].locations.push_back(locatorbit(-2,1,false));
		locators[6].locations.push_back(locatorbit(-1,-1,false));
		locators[6].locations.push_back(locatorbit(-1,1,false));
		locators[6].locations.push_back(locatorbit(0,0,false));
		locators[6].locations.push_back(locatorbit(-4,-2,true));
		locators[6].locations.push_back(locatorbit(-4,-1,true));
		locators[6].locations.push_back(locatorbit(-4,0,true));
		locators[6].locations.push_back(locatorbit(-4,1,true));
		locators[6].locations.push_back(locatorbit(-3,-3,true));
		locators[6].locations.push_back(locatorbit(-3,-2,true));
		locators[6].locations.push_back(locatorbit(-3,1,true));
		locators[6].locations.push_back(locatorbit(-3,2,true));
		locators[6].locations.push_back(locatorbit(-2,-3,true));
		locators[6].locations.push_back(locatorbit(-2,-1,true));
		locators[6].locations.push_back(locatorbit(-2,0,true));
		locators[6].locations.push_back(locatorbit(-2,2,true));
		locators[6].locations.push_back(locatorbit(-1,-3,true));
		locators[6].locations.push_back(locatorbit(-1,-2,true));
		locators[6].locations.push_back(locatorbit(-1,0,true));
		locators[6].locations.push_back(locatorbit(-1,2,true));
		locators[6].locations.push_back(locatorbit(0,-2,true));
		locators[6].locations.push_back(locatorbit(0,-1,true));
		locators[6].locations.push_back(locatorbit(0,1,true));
		locators[6].locations.push_back(locatorbit(0,2,true));
		locators[6].locations.push_back(locatorbit(1,-1,true));
		locators[6].locations.push_back(locatorbit(1,0,true));
		locators[6].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[7].locations.push_back(locatorbit(-3,1,false));
		locators[7].locations.push_back(locatorbit(-2,0,false));
		locators[7].locations.push_back(locatorbit(-2,2,false));
		locators[7].locations.push_back(locatorbit(-1,-1,false));
		locators[7].locations.push_back(locatorbit(-1,2,false));
		locators[7].locations.push_back(locatorbit(0,0,false));
		locators[7].locations.push_back(locatorbit(0,1,false));
		locators[7].locations.push_back(locatorbit(-4,0,true));
		locators[7].locations.push_back(locatorbit(-4,1,true));
		locators[7].locations.push_back(locatorbit(-4,2,true));
		locators[7].locations.push_back(locatorbit(-3,-1,true));
		locators[7].locations.push_back(locatorbit(-3,0,true));
		locators[7].locations.push_back(locatorbit(-3,2,true));
		locators[7].locations.push_back(locatorbit(-3,3,true));
		locators[7].locations.push_back(locatorbit(-2,-2,true));
		locators[7].locations.push_back(locatorbit(-2,-1,true));
		locators[7].locations.push_back(locatorbit(-2,1,true));
		locators[7].locations.push_back(locatorbit(-2,3,true));
		locators[7].locations.push_back(locatorbit(-1,-2,true));
		locators[7].locations.push_back(locatorbit(-1,0,true));
		locators[7].locations.push_back(locatorbit(-1,1,true));
		locators[7].locations.push_back(locatorbit(-1,3,true));
		locators[7].locations.push_back(locatorbit(0,-2,true));
		locators[7].locations.push_back(locatorbit(0,-1,true));
		locators[7].locations.push_back(locatorbit(0,2,true));
		locators[7].locations.push_back(locatorbit(0,3,true));
		locators[7].locations.push_back(locatorbit(1,-1,true));
		locators[7].locations.push_back(locatorbit(1,0,true));
		locators[7].locations.push_back(locatorbit(1,1,true));
		locators[7].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[8].locations.push_back(locatorbit(-3,0,false));
		locators[8].locations.push_back(locatorbit(-3,1,false));
		locators[8].locations.push_back(locatorbit(-2,-1,false));
		locators[8].locations.push_back(locatorbit(-2,2,false));
		locators[8].locations.push_back(locatorbit(-1,-1,false));
		locators[8].locations.push_back(locatorbit(-1,1,false));
		locators[8].locations.push_back(locatorbit(0,0,false));
		locators[8].locations.push_back(locatorbit(-4,-1,true));
		locators[8].locations.push_back(locatorbit(-4,0,true));
		locators[8].locations.push_back(locatorbit(-4,1,true));
		locators[8].locations.push_back(locatorbit(-4,2,true));
		locators[8].locations.push_back(locatorbit(-3,-2,true));
		locators[8].locations.push_back(locatorbit(-3,-1,true));
		locators[8].locations.push_back(locatorbit(-3,2,true));
		locators[8].locations.push_back(locatorbit(-3,3,true));
		locators[8].locations.push_back(locatorbit(-2,-2,true));
		locators[8].locations.push_back(locatorbit(-2,0,true));
		locators[8].locations.push_back(locatorbit(-2,1,true));
		locators[8].locations.push_back(locatorbit(-2,3,true));
		locators[8].locations.push_back(locatorbit(-1,-2,true));
		locators[8].locations.push_back(locatorbit(-1,0,true));
		locators[8].locations.push_back(locatorbit(-1,2,true));
		locators[8].locations.push_back(locatorbit(-1,3,true));
		locators[8].locations.push_back(locatorbit(0,-2,true));
		locators[8].locations.push_back(locatorbit(0,-1,true));
		locators[8].locations.push_back(locatorbit(0,1,true));
		locators[8].locations.push_back(locatorbit(0,2,true));
		locators[8].locations.push_back(locatorbit(1,-1,true));
		locators[8].locations.push_back(locatorbit(1,0,true));
		locators[8].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[9].locations.push_back(locatorbit(-2,1,false));
		locators[9].locations.push_back(locatorbit(-1,0,false));
		locators[9].locations.push_back(locatorbit(-1,2,false));
		locators[9].locations.push_back(locatorbit(0,0,false));
		locators[9].locations.push_back(locatorbit(0,1,false));
		locators[9].locations.push_back(locatorbit(-3,0,true));
		locators[9].locations.push_back(locatorbit(-3,1,true));
		locators[9].locations.push_back(locatorbit(-3,2,true));
		locators[9].locations.push_back(locatorbit(-2,-1,true));
		locators[9].locations.push_back(locatorbit(-2,0,true));
		locators[9].locations.push_back(locatorbit(-2,2,true));
		locators[9].locations.push_back(locatorbit(-2,3,true));
		locators[9].locations.push_back(locatorbit(-1,-1,true));
		locators[9].locations.push_back(locatorbit(-1,1,true));
		locators[9].locations.push_back(locatorbit(-1,3,true));
		locators[9].locations.push_back(locatorbit(0,-1,true));
		locators[9].locations.push_back(locatorbit(0,2,true));
		locators[9].locations.push_back(locatorbit(0,3,true));
		locators[9].locations.push_back(locatorbit(1,-1,true));
		locators[9].locations.push_back(locatorbit(1,0,true));
		locators[9].locations.push_back(locatorbit(1,1,true));
		locators[9].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[10].locations.push_back(locatorbit(-2,0,false));
		locators[10].locations.push_back(locatorbit(-2,1,false));
		locators[10].locations.push_back(locatorbit(-1,-1,false));
		locators[10].locations.push_back(locatorbit(-1,1,false));
		locators[10].locations.push_back(locatorbit(0,0,false));
		locators[10].locations.push_back(locatorbit(-3,-1,true));
		locators[10].locations.push_back(locatorbit(-3,0,true));
		locators[10].locations.push_back(locatorbit(-3,1,true));
		locators[10].locations.push_back(locatorbit(-3,2,true));
		locators[10].locations.push_back(locatorbit(-2,-2,true));
		locators[10].locations.push_back(locatorbit(-2,-1,true));
		locators[10].locations.push_back(locatorbit(-2,2,true));
		locators[10].locations.push_back(locatorbit(-1,-2,true));
		locators[10].locations.push_back(locatorbit(-1,0,true));
		locators[10].locations.push_back(locatorbit(-1,2,true));
		locators[10].locations.push_back(locatorbit(0,-2,true));
		locators[10].locations.push_back(locatorbit(0,-1,true));
		locators[10].locations.push_back(locatorbit(0,1,true));
		locators[10].locations.push_back(locatorbit(0,2,true));
		locators[10].locations.push_back(locatorbit(1,-1,true));
		locators[10].locations.push_back(locatorbit(1,0,true));
		locators[10].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[11].locations.push_back(locatorbit(-2,0,false));
		locators[11].locations.push_back(locatorbit(-1,-1,false));
		locators[11].locations.push_back(locatorbit(-1,1,false));
		locators[11].locations.push_back(locatorbit(0,0,false));
		locators[11].locations.push_back(locatorbit(0,1,false));
		locators[11].locations.push_back(locatorbit(-3,-1,true));
		locators[11].locations.push_back(locatorbit(-3,0,true));
		locators[11].locations.push_back(locatorbit(-3,1,true));
		locators[11].locations.push_back(locatorbit(-2,-2,true));
		locators[11].locations.push_back(locatorbit(-2,-1,true));
		locators[11].locations.push_back(locatorbit(-2,1,true));
		locators[11].locations.push_back(locatorbit(-2,2,true));
		locators[11].locations.push_back(locatorbit(-1,-2,true));
		locators[11].locations.push_back(locatorbit(-1,0,true));
		locators[11].locations.push_back(locatorbit(-1,2,true));
		locators[11].locations.push_back(locatorbit(0,-2,true));
		locators[11].locations.push_back(locatorbit(0,-1,true));
		locators[11].locations.push_back(locatorbit(0,2,true));
		locators[11].locations.push_back(locatorbit(1,-1,true));
		locators[11].locations.push_back(locatorbit(1,0,true));
		locators[11].locations.push_back(locatorbit(1,1,true));
		locators[11].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[12].locations.push_back(locatorbit(-2,-1,false));
		locators[12].locations.push_back(locatorbit(-2,0,false));
		locators[12].locations.push_back(locatorbit(-1,-1,false));
		locators[12].locations.push_back(locatorbit(-1,1,false));
		locators[12].locations.push_back(locatorbit(0,0,false));
		locators[12].locations.push_back(locatorbit(-3,-2,true));
		locators[12].locations.push_back(locatorbit(-3,-1,true));
		locators[12].locations.push_back(locatorbit(-3,0,true));
		locators[12].locations.push_back(locatorbit(-3,1,true));
		locators[12].locations.push_back(locatorbit(-2,-2,true));
		locators[12].locations.push_back(locatorbit(-2,1,true));
		locators[12].locations.push_back(locatorbit(-2,2,true));
		locators[12].locations.push_back(locatorbit(-1,-2,true));
		locators[12].locations.push_back(locatorbit(-1,0,true));
		locators[12].locations.push_back(locatorbit(-1,2,true));
		locators[12].locations.push_back(locatorbit(0,-2,true));
		locators[12].locations.push_back(locatorbit(0,-1,true));
		locators[12].locations.push_back(locatorbit(0,1,true));
		locators[12].locations.push_back(locatorbit(0,2,true));
		locators[12].locations.push_back(locatorbit(1,-1,true));
		locators[12].locations.push_back(locatorbit(1,0,true));
		locators[12].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[13].locations.push_back(locatorbit(-2,0,false));
		locators[13].locations.push_back(locatorbit(-1,-1,false));
		locators[13].locations.push_back(locatorbit(-1,1,false));
		locators[13].locations.push_back(locatorbit(0,0,false));
		locators[13].locations.push_back(locatorbit(-3,-1,true));
		locators[13].locations.push_back(locatorbit(-3,0,true));
		locators[13].locations.push_back(locatorbit(-3,1,true));
		locators[13].locations.push_back(locatorbit(-2,-2,true));
		locators[13].locations.push_back(locatorbit(-2,-1,true));
		locators[13].locations.push_back(locatorbit(-2,1,true));
		locators[13].locations.push_back(locatorbit(-2,2,true));
		locators[13].locations.push_back(locatorbit(-1,-2,true));
		locators[13].locations.push_back(locatorbit(-1,0,true));
		locators[13].locations.push_back(locatorbit(-1,2,true));
		locators[13].locations.push_back(locatorbit(0,-2,true));
		locators[13].locations.push_back(locatorbit(0,-1,true));
		locators[13].locations.push_back(locatorbit(0,1,true));
		locators[13].locations.push_back(locatorbit(0,2,true));
		locators[13].locations.push_back(locatorbit(1,-1,true));
		locators[13].locations.push_back(locatorbit(1,0,true));
		locators[13].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[14].locations.push_back(locatorbit(-3,0,false));
		locators[14].locations.push_back(locatorbit(-3,1,false));
		locators[14].locations.push_back(locatorbit(-2,-1,false));
		locators[14].locations.push_back(locatorbit(-2,2,false));
		locators[14].locations.push_back(locatorbit(-1,-1,false));
		locators[14].locations.push_back(locatorbit(-1,2,false));
		locators[14].locations.push_back(locatorbit(0,0,false));
		locators[14].locations.push_back(locatorbit(0,1,false));
		locators[14].locations.push_back(locatorbit(-4,-1,true));
		locators[14].locations.push_back(locatorbit(-4,0,true));
		locators[14].locations.push_back(locatorbit(-4,1,true));
		locators[14].locations.push_back(locatorbit(-4,2,true));
		locators[14].locations.push_back(locatorbit(-3,-2,true));
		locators[14].locations.push_back(locatorbit(-3,-1,true));
		locators[14].locations.push_back(locatorbit(-3,2,true));
		locators[14].locations.push_back(locatorbit(-3,3,true));
		locators[14].locations.push_back(locatorbit(-2,-2,true));
		locators[14].locations.push_back(locatorbit(-2,0,true));
		locators[14].locations.push_back(locatorbit(-2,1,true));
		locators[14].locations.push_back(locatorbit(-2,3,true));
		locators[14].locations.push_back(locatorbit(-1,-2,true));
		locators[14].locations.push_back(locatorbit(-1,0,true));
		locators[14].locations.push_back(locatorbit(-1,1,true));
		locators[14].locations.push_back(locatorbit(-1,3,true));
		locators[14].locations.push_back(locatorbit(0,-2,true));
		locators[14].locations.push_back(locatorbit(0,-1,true));
		locators[14].locations.push_back(locatorbit(0,2,true));
		locators[14].locations.push_back(locatorbit(0,3,true));
		locators[14].locations.push_back(locatorbit(1,-1,true));
		locators[14].locations.push_back(locatorbit(1,0,true));
		locators[14].locations.push_back(locatorbit(1,1,true));
		locators[14].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[15].locations.push_back(locatorbit(-2,1,false));
		locators[15].locations.push_back(locatorbit(-2,2,false));
		locators[15].locations.push_back(locatorbit(-1,0,false));
		locators[15].locations.push_back(locatorbit(-1,2,false));
		locators[15].locations.push_back(locatorbit(0,0,false));
		locators[15].locations.push_back(locatorbit(0,1,false));
		locators[15].locations.push_back(locatorbit(-3,0,true));
		locators[15].locations.push_back(locatorbit(-3,1,true));
		locators[15].locations.push_back(locatorbit(-3,2,true));
		locators[15].locations.push_back(locatorbit(-3,3,true));
		locators[15].locations.push_back(locatorbit(-2,-1,true));
		locators[15].locations.push_back(locatorbit(-2,0,true));
		locators[15].locations.push_back(locatorbit(-2,3,true));
		locators[15].locations.push_back(locatorbit(-1,-1,true));
		locators[15].locations.push_back(locatorbit(-1,1,true));
		locators[15].locations.push_back(locatorbit(-1,3,true));
		locators[15].locations.push_back(locatorbit(0,-1,true));
		locators[15].locations.push_back(locatorbit(0,2,true));
		locators[15].locations.push_back(locatorbit(0,3,true));
		locators[15].locations.push_back(locatorbit(1,-1,true));
		locators[15].locations.push_back(locatorbit(1,0,true));
		locators[15].locations.push_back(locatorbit(1,1,true));
		locators[15].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[16].locations.push_back(locatorbit(-2,-1,false));
		locators[16].locations.push_back(locatorbit(-2,0,false));
		locators[16].locations.push_back(locatorbit(-1,-1,false));
		locators[16].locations.push_back(locatorbit(-1,1,false));
		locators[16].locations.push_back(locatorbit(0,0,false));
		locators[16].locations.push_back(locatorbit(0,1,false));
		locators[16].locations.push_back(locatorbit(-3,-2,true));
		locators[16].locations.push_back(locatorbit(-3,-1,true));
		locators[16].locations.push_back(locatorbit(-3,0,true));
		locators[16].locations.push_back(locatorbit(-3,1,true));
		locators[16].locations.push_back(locatorbit(-2,-2,true));
		locators[16].locations.push_back(locatorbit(-2,1,true));
		locators[16].locations.push_back(locatorbit(-2,2,true));
		locators[16].locations.push_back(locatorbit(-1,-2,true));
		locators[16].locations.push_back(locatorbit(-1,0,true));
		locators[16].locations.push_back(locatorbit(-1,2,true));
		locators[16].locations.push_back(locatorbit(0,-2,true));
		locators[16].locations.push_back(locatorbit(0,-1,true));
		locators[16].locations.push_back(locatorbit(0,2,true));
		locators[16].locations.push_back(locatorbit(1,-1,true));
		locators[16].locations.push_back(locatorbit(1,0,true));
		locators[16].locations.push_back(locatorbit(1,1,true));
		locators[16].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[17].locations.push_back(locatorbit(-3,-1,false));
		locators[17].locations.push_back(locatorbit(-2,-2,false));
		locators[17].locations.push_back(locatorbit(-2,0,false));
		locators[17].locations.push_back(locatorbit(-1,-1,false));
		locators[17].locations.push_back(locatorbit(-1,1,false));
		locators[17].locations.push_back(locatorbit(0,0,false));
		locators[17].locations.push_back(locatorbit(0,1,false));
		locators[17].locations.push_back(locatorbit(-4,-2,true));
		locators[17].locations.push_back(locatorbit(-4,-1,true));
		locators[17].locations.push_back(locatorbit(-4,0,true));
		locators[17].locations.push_back(locatorbit(-3,-3,true));
		locators[17].locations.push_back(locatorbit(-3,-2,true));
		locators[17].locations.push_back(locatorbit(-3,0,true));
		locators[17].locations.push_back(locatorbit(-3,1,true));
		locators[17].locations.push_back(locatorbit(-2,-3,true));
		locators[17].locations.push_back(locatorbit(-2,-1,true));
		locators[17].locations.push_back(locatorbit(-2,1,true));
		locators[17].locations.push_back(locatorbit(-2,2,true));
		locators[17].locations.push_back(locatorbit(-1,-3,true));
		locators[17].locations.push_back(locatorbit(-1,-2,true));
		locators[17].locations.push_back(locatorbit(-1,0,true));
		locators[17].locations.push_back(locatorbit(-1,2,true));
		locators[17].locations.push_back(locatorbit(0,-2,true));
		locators[17].locations.push_back(locatorbit(0,-1,true));
		locators[17].locations.push_back(locatorbit(0,2,true));
		locators[17].locations.push_back(locatorbit(1,-1,true));
		locators[17].locations.push_back(locatorbit(1,0,true));
		locators[17].locations.push_back(locatorbit(1,1,true));
		locators[17].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[18].locations.push_back(locatorbit(-3,1,false));
		locators[18].locations.push_back(locatorbit(-3,2,false));
		locators[18].locations.push_back(locatorbit(-2,0,false));
		locators[18].locations.push_back(locatorbit(-2,2,false));
		locators[18].locations.push_back(locatorbit(-1,-1,false));
		locators[18].locations.push_back(locatorbit(-1,1,false));
		locators[18].locations.push_back(locatorbit(0,0,false));
		locators[18].locations.push_back(locatorbit(-4,0,true));
		locators[18].locations.push_back(locatorbit(-4,1,true));
		locators[18].locations.push_back(locatorbit(-4,2,true));
		locators[18].locations.push_back(locatorbit(-4,3,true));
		locators[18].locations.push_back(locatorbit(-3,-1,true));
		locators[18].locations.push_back(locatorbit(-3,0,true));
		locators[18].locations.push_back(locatorbit(-3,3,true));
		locators[18].locations.push_back(locatorbit(-2,-2,true));
		locators[18].locations.push_back(locatorbit(-2,-1,true));
		locators[18].locations.push_back(locatorbit(-2,1,true));
		locators[18].locations.push_back(locatorbit(-2,3,true));
		locators[18].locations.push_back(locatorbit(-1,-2,true));
		locators[18].locations.push_back(locatorbit(-1,0,true));
		locators[18].locations.push_back(locatorbit(-1,2,true));
		locators[18].locations.push_back(locatorbit(-1,3,true));
		locators[18].locations.push_back(locatorbit(0,-2,true));
		locators[18].locations.push_back(locatorbit(0,-1,true));
		locators[18].locations.push_back(locatorbit(0,1,true));
		locators[18].locations.push_back(locatorbit(0,2,true));
		locators[18].locations.push_back(locatorbit(1,-1,true));
		locators[18].locations.push_back(locatorbit(1,0,true));
		locators[18].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[19].locations.push_back(locatorbit(-3,2,false));
		locators[19].locations.push_back(locatorbit(-2,1,false));
		locators[19].locations.push_back(locatorbit(-2,3,false));
		locators[19].locations.push_back(locatorbit(-1,0,false));
		locators[19].locations.push_back(locatorbit(-1,2,false));
		locators[19].locations.push_back(locatorbit(0,0,false));
		locators[19].locations.push_back(locatorbit(0,1,false));
		locators[19].locations.push_back(locatorbit(-4,1,true));
		locators[19].locations.push_back(locatorbit(-4,2,true));
		locators[19].locations.push_back(locatorbit(-4,3,true));
		locators[19].locations.push_back(locatorbit(-3,0,true));
		locators[19].locations.push_back(locatorbit(-3,1,true));
		locators[19].locations.push_back(locatorbit(-3,3,true));
		locators[19].locations.push_back(locatorbit(-3,4,true));
		locators[19].locations.push_back(locatorbit(-2,-1,true));
		locators[19].locations.push_back(locatorbit(-2,0,true));
		locators[19].locations.push_back(locatorbit(-2,2,true));
		locators[19].locations.push_back(locatorbit(-2,4,true));
		locators[19].locations.push_back(locatorbit(-1,-1,true));
		locators[19].locations.push_back(locatorbit(-1,1,true));
		locators[19].locations.push_back(locatorbit(-1,3,true));
		locators[19].locations.push_back(locatorbit(-1,4,true));
		locators[19].locations.push_back(locatorbit(0,-1,true));
		locators[19].locations.push_back(locatorbit(0,2,true));
		locators[19].locations.push_back(locatorbit(0,3,true));
		locators[19].locations.push_back(locatorbit(1,-1,true));
		locators[19].locations.push_back(locatorbit(1,0,true));
		locators[19].locations.push_back(locatorbit(1,1,true));
		locators[19].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[20].locations.push_back(locatorbit(-3,-2,false));
		locators[20].locations.push_back(locatorbit(-3,-1,false));
		locators[20].locations.push_back(locatorbit(-2,-2,false));
		locators[20].locations.push_back(locatorbit(-2,0,false));
		locators[20].locations.push_back(locatorbit(-1,-1,false));
		locators[20].locations.push_back(locatorbit(-1,1,false));
		locators[20].locations.push_back(locatorbit(0,0,false));
		locators[20].locations.push_back(locatorbit(-4,-3,true));
		locators[20].locations.push_back(locatorbit(-4,-2,true));
		locators[20].locations.push_back(locatorbit(-4,-1,true));
		locators[20].locations.push_back(locatorbit(-4,0,true));
		locators[20].locations.push_back(locatorbit(-3,-3,true));
		locators[20].locations.push_back(locatorbit(-3,0,true));
		locators[20].locations.push_back(locatorbit(-3,1,true));
		locators[20].locations.push_back(locatorbit(-2,-3,true));
		locators[20].locations.push_back(locatorbit(-2,-1,true));
		locators[20].locations.push_back(locatorbit(-2,1,true));
		locators[20].locations.push_back(locatorbit(-2,2,true));
		locators[20].locations.push_back(locatorbit(-1,-3,true));
		locators[20].locations.push_back(locatorbit(-1,-2,true));
		locators[20].locations.push_back(locatorbit(-1,0,true));
		locators[20].locations.push_back(locatorbit(-1,2,true));
		locators[20].locations.push_back(locatorbit(0,-2,true));
		locators[20].locations.push_back(locatorbit(0,-1,true));
		locators[20].locations.push_back(locatorbit(0,1,true));
		locators[20].locations.push_back(locatorbit(0,2,true));
		locators[20].locations.push_back(locatorbit(1,-1,true));
		locators[20].locations.push_back(locatorbit(1,0,true));
		locators[20].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[21].locations.push_back(locatorbit(-3,0,false));
		locators[21].locations.push_back(locatorbit(-3,1,false));
		locators[21].locations.push_back(locatorbit(-2,2,false));
		locators[21].locations.push_back(locatorbit(-1,-1,false));
		locators[21].locations.push_back(locatorbit(0,0,false));
		locators[21].locations.push_back(locatorbit(0,1,false));
		locators[21].locations.push_back(locatorbit(-4,-1,true));
		locators[21].locations.push_back(locatorbit(-4,0,true));
		locators[21].locations.push_back(locatorbit(-4,1,true));
		locators[21].locations.push_back(locatorbit(-4,2,true));
		locators[21].locations.push_back(locatorbit(-3,-1,true));
		locators[21].locations.push_back(locatorbit(-3,2,true));
		locators[21].locations.push_back(locatorbit(-3,3,true));
		locators[21].locations.push_back(locatorbit(-2,-2,true));
		locators[21].locations.push_back(locatorbit(-2,-1,true));
		locators[21].locations.push_back(locatorbit(-2,0,true));
		locators[21].locations.push_back(locatorbit(-2,1,true));
		locators[21].locations.push_back(locatorbit(-2,3,true));
		locators[21].locations.push_back(locatorbit(-1,-2,true));
		locators[21].locations.push_back(locatorbit(-1,0,true));
		locators[21].locations.push_back(locatorbit(-1,1,true));
		locators[21].locations.push_back(locatorbit(-1,2,true));
		locators[21].locations.push_back(locatorbit(-1,3,true));
		locators[21].locations.push_back(locatorbit(0,-2,true));
		locators[21].locations.push_back(locatorbit(0,-1,true));
		locators[21].locations.push_back(locatorbit(0,2,true));
		locators[21].locations.push_back(locatorbit(1,-1,true));
		locators[21].locations.push_back(locatorbit(1,0,true));
		locators[21].locations.push_back(locatorbit(1,1,true));
		locators[21].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[22].locations.push_back(locatorbit(-3,0,false));
		locators[22].locations.push_back(locatorbit(-3,1,false));
		locators[22].locations.push_back(locatorbit(-2,-1,false));
		locators[22].locations.push_back(locatorbit(-1,2,false));
		locators[22].locations.push_back(locatorbit(0,0,false));
		locators[22].locations.push_back(locatorbit(0,1,false));
		locators[22].locations.push_back(locatorbit(-4,-1,true));
		locators[22].locations.push_back(locatorbit(-4,0,true));
		locators[22].locations.push_back(locatorbit(-4,1,true));
		locators[22].locations.push_back(locatorbit(-4,2,true));
		locators[22].locations.push_back(locatorbit(-3,-2,true));
		locators[22].locations.push_back(locatorbit(-3,-1,true));
		locators[22].locations.push_back(locatorbit(-3,2,true));
		locators[22].locations.push_back(locatorbit(-2,-2,true));
		locators[22].locations.push_back(locatorbit(-2,0,true));
		locators[22].locations.push_back(locatorbit(-2,1,true));
		locators[22].locations.push_back(locatorbit(-2,2,true));
		locators[22].locations.push_back(locatorbit(-2,3,true));
		locators[22].locations.push_back(locatorbit(-1,-2,true));
		locators[22].locations.push_back(locatorbit(-1,-1,true));
		locators[22].locations.push_back(locatorbit(-1,0,true));
		locators[22].locations.push_back(locatorbit(-1,1,true));
		locators[22].locations.push_back(locatorbit(-1,3,true));
		locators[22].locations.push_back(locatorbit(0,-1,true));
		locators[22].locations.push_back(locatorbit(0,2,true));
		locators[22].locations.push_back(locatorbit(0,3,true));
		locators[22].locations.push_back(locatorbit(1,-1,true));
		locators[22].locations.push_back(locatorbit(1,0,true));
		locators[22].locations.push_back(locatorbit(1,1,true));
		locators[22].locations.push_back(locatorbit(1,2,true));
		locators[22].locations.push_back(locatorbit(5,-1,true));
		locators[22].locations.push_back(locatorbit(5,0,true));
		locators[22].locations.push_back(locatorbit(5,1,true));
		locators.push_back(locator());
		locators[23].locations.push_back(locatorbit(-3,-1,false));
		locators[23].locations.push_back(locatorbit(-2,-2,false));
		locators[23].locations.push_back(locatorbit(-2,1,false));
		locators[23].locations.push_back(locatorbit(-1,-2,false));
		locators[23].locations.push_back(locatorbit(-1,1,false));
		locators[23].locations.push_back(locatorbit(0,0,false));
		locators[23].locations.push_back(locatorbit(-3,-3,true));
		locators[23].locations.push_back(locatorbit(-3,-2,true));
		locators[23].locations.push_back(locatorbit(-3,0,true));
		locators[23].locations.push_back(locatorbit(-3,1,true));
		locators[23].locations.push_back(locatorbit(-3,2,true));
		locators[23].locations.push_back(locatorbit(-2,-3,true));
		locators[23].locations.push_back(locatorbit(-2,-1,true));
		locators[23].locations.push_back(locatorbit(-2,0,true));
		locators[23].locations.push_back(locatorbit(-2,2,true));
		locators[23].locations.push_back(locatorbit(-1,-3,true));
		locators[23].locations.push_back(locatorbit(-1,-1,true));
		locators[23].locations.push_back(locatorbit(-1,0,true));
		locators[23].locations.push_back(locatorbit(-1,2,true));
		locators[23].locations.push_back(locatorbit(0,-3,true));
		locators[23].locations.push_back(locatorbit(0,-2,true));
		locators[23].locations.push_back(locatorbit(0,-1,true));
		locators[23].locations.push_back(locatorbit(0,1,true));
		locators[23].locations.push_back(locatorbit(0,2,true));
		locators[23].locations.push_back(locatorbit(1,-1,true));
		locators[23].locations.push_back(locatorbit(1,0,true));
		locators[23].locations.push_back(locatorbit(1,1,true));
		locators[23].locations.push_back(locatorbit(6,-1,true));
		locators[23].locations.push_back(locatorbit(6,0,true));
		locators[23].locations.push_back(locatorbit(6,1,true));
		locators.push_back(locator());
		locators[24].locations.push_back(locatorbit(-3,1,false));
		locators[24].locations.push_back(locatorbit(-2,-1,false));
		locators[24].locations.push_back(locatorbit(-2,2,false));
		locators[24].locations.push_back(locatorbit(-1,-1,false));
		locators[24].locations.push_back(locatorbit(-1,2,false));
		locators[24].locations.push_back(locatorbit(0,0,false));
		locators[24].locations.push_back(locatorbit(-3,-2,true));
		locators[24].locations.push_back(locatorbit(-3,-1,true));
		locators[24].locations.push_back(locatorbit(-3,0,true));
		locators[24].locations.push_back(locatorbit(-3,2,true));
		locators[24].locations.push_back(locatorbit(-3,3,true));
		locators[24].locations.push_back(locatorbit(-2,-2,true));
		locators[24].locations.push_back(locatorbit(-2,0,true));
		locators[24].locations.push_back(locatorbit(-2,1,true));
		locators[24].locations.push_back(locatorbit(-2,3,true));
		locators[24].locations.push_back(locatorbit(-1,-2,true));
		locators[24].locations.push_back(locatorbit(-1,0,true));
		locators[24].locations.push_back(locatorbit(-1,1,true));
		locators[24].locations.push_back(locatorbit(-1,3,true));
		locators[24].locations.push_back(locatorbit(0,-2,true));
		locators[24].locations.push_back(locatorbit(0,-1,true));
		locators[24].locations.push_back(locatorbit(0,1,true));
		locators[24].locations.push_back(locatorbit(0,2,true));
		locators[24].locations.push_back(locatorbit(0,3,true));
		locators[24].locations.push_back(locatorbit(1,-1,true));
		locators[24].locations.push_back(locatorbit(1,0,true));
		locators[24].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[25].locations.push_back(locatorbit(-3,1,false));
		locators[25].locations.push_back(locatorbit(-2,0,false));
		locators[25].locations.push_back(locatorbit(-2,1,false));
		locators[25].locations.push_back(locatorbit(-1,0,false));
		locators[25].locations.push_back(locatorbit(-1,1,false));
		locators[25].locations.push_back(locatorbit(0,0,false));
		locators[25].locations.push_back(locatorbit(-4,-1,true));
		locators[25].locations.push_back(locatorbit(-4,0,true));
		locators[25].locations.push_back(locatorbit(-4,1,true));
		locators[25].locations.push_back(locatorbit(-4,2,true));
		locators[25].locations.push_back(locatorbit(-3,-1,true));
		locators[25].locations.push_back(locatorbit(-3,0,true));
		locators[25].locations.push_back(locatorbit(-3,2,true));
		locators[25].locations.push_back(locatorbit(-3,3,true));
		locators[25].locations.push_back(locatorbit(-2,-2,true));
		locators[25].locations.push_back(locatorbit(-2,-1,true));
		locators[25].locations.push_back(locatorbit(-2,2,true));
		locators[25].locations.push_back(locatorbit(-2,3,true));
		locators[25].locations.push_back(locatorbit(-1,-2,true));
		locators[25].locations.push_back(locatorbit(-1,-1,true));
		locators[25].locations.push_back(locatorbit(-1,2,true));
		locators[25].locations.push_back(locatorbit(-1,3,true));
		locators[25].locations.push_back(locatorbit(0,-2,true));
		locators[25].locations.push_back(locatorbit(0,-1,true));
		locators[25].locations.push_back(locatorbit(0,1,true));
		locators[25].locations.push_back(locatorbit(0,2,true));
		locators[25].locations.push_back(locatorbit(1,-1,true));
		locators[25].locations.push_back(locatorbit(1,0,true));
		locators[25].locations.push_back(locatorbit(1,1,true));
		locators[25].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[26].locations.push_back(locatorbit(-3,-1,false));
		locators[26].locations.push_back(locatorbit(-2,-1,false));
		locators[26].locations.push_back(locatorbit(-2,0,false));
		locators[26].locations.push_back(locatorbit(-1,-1,false));
		locators[26].locations.push_back(locatorbit(-1,0,false));
		locators[26].locations.push_back(locatorbit(0,0,false));
		locators[26].locations.push_back(locatorbit(-4,-2,true));
		locators[26].locations.push_back(locatorbit(-4,-1,true));
		locators[26].locations.push_back(locatorbit(-4,0,true));
		locators[26].locations.push_back(locatorbit(-4,1,true));
		locators[26].locations.push_back(locatorbit(-3,-3,true));
		locators[26].locations.push_back(locatorbit(-3,-2,true));
		locators[26].locations.push_back(locatorbit(-3,0,true));
		locators[26].locations.push_back(locatorbit(-3,1,true));
		locators[26].locations.push_back(locatorbit(-2,-3,true));
		locators[26].locations.push_back(locatorbit(-2,-2,true));
		locators[26].locations.push_back(locatorbit(-2,1,true));
		locators[26].locations.push_back(locatorbit(-2,2,true));
		locators[26].locations.push_back(locatorbit(-1,-3,true));
		locators[26].locations.push_back(locatorbit(-1,-2,true));
		locators[26].locations.push_back(locatorbit(-1,1,true));
		locators[26].locations.push_back(locatorbit(-1,2,true));
		locators[26].locations.push_back(locatorbit(0,-2,true));
		locators[26].locations.push_back(locatorbit(0,-1,true));
		locators[26].locations.push_back(locatorbit(0,1,true));
		locators[26].locations.push_back(locatorbit(0,2,true));
		locators[26].locations.push_back(locatorbit(1,-2,true));
		locators[26].locations.push_back(locatorbit(1,-1,true));
		locators[26].locations.push_back(locatorbit(1,0,true));
		locators[26].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[27].locations.push_back(locatorbit(-1,-1,false));
		locators[27].locations.push_back(locatorbit(-1,0,false));
		locators[27].locations.push_back(locatorbit(-1,1,false));
		locators[27].locations.push_back(locatorbit(0,0,false));
		locators[27].locations.push_back(locatorbit(0,1,false));
		locators[27].locations.push_back(locatorbit(0,2,false));
		locators[27].locations.push_back(locatorbit(-3,-1,true));
		locators[27].locations.push_back(locatorbit(-3,0,true));
		locators[27].locations.push_back(locatorbit(-3,1,true));
		locators[27].locations.push_back(locatorbit(-2,-2,true));
		locators[27].locations.push_back(locatorbit(-2,-1,true));
		locators[27].locations.push_back(locatorbit(-2,0,true));
		locators[27].locations.push_back(locatorbit(-2,1,true));
		locators[27].locations.push_back(locatorbit(-2,2,true));
		locators[27].locations.push_back(locatorbit(-2,3,true));
		locators[27].locations.push_back(locatorbit(-1,-2,true));
		locators[27].locations.push_back(locatorbit(-1,2,true));
		locators[27].locations.push_back(locatorbit(-1,3,true));
		locators[27].locations.push_back(locatorbit(0,-2,true));
		locators[27].locations.push_back(locatorbit(0,-1,true));
		locators[27].locations.push_back(locatorbit(0,3,true));
		locators[27].locations.push_back(locatorbit(1,-2,true));
		locators[27].locations.push_back(locatorbit(1,-1,true));
		locators[27].locations.push_back(locatorbit(1,0,true));
		locators[27].locations.push_back(locatorbit(1,1,true));
		locators[27].locations.push_back(locatorbit(1,2,true));
		locators[27].locations.push_back(locatorbit(1,3,true));
		locators[27].locations.push_back(locatorbit(2,0,true));
		locators[27].locations.push_back(locatorbit(2,1,true));
		locators[27].locations.push_back(locatorbit(2,2,true));
		locators.push_back(locator());
		locators[28].locations.push_back(locatorbit(-1,1,false));
		locators[28].locations.push_back(locatorbit(-1,2,false));
		locators[28].locations.push_back(locatorbit(-1,3,false));
		locators[28].locations.push_back(locatorbit(0,0,false));
		locators[28].locations.push_back(locatorbit(0,1,false));
		locators[28].locations.push_back(locatorbit(0,2,false));
		locators[28].locations.push_back(locatorbit(-3,1,true));
		locators[28].locations.push_back(locatorbit(-3,2,true));
		locators[28].locations.push_back(locatorbit(-3,3,true));
		locators[28].locations.push_back(locatorbit(-2,-1,true));
		locators[28].locations.push_back(locatorbit(-2,0,true));
		locators[28].locations.push_back(locatorbit(-2,1,true));
		locators[28].locations.push_back(locatorbit(-2,2,true));
		locators[28].locations.push_back(locatorbit(-2,3,true));
		locators[28].locations.push_back(locatorbit(-2,4,true));
		locators[28].locations.push_back(locatorbit(-1,-1,true));
		locators[28].locations.push_back(locatorbit(-1,0,true));
		locators[28].locations.push_back(locatorbit(-1,4,true));
		locators[28].locations.push_back(locatorbit(0,-1,true));
		locators[28].locations.push_back(locatorbit(0,3,true));
		locators[28].locations.push_back(locatorbit(0,4,true));
		locators[28].locations.push_back(locatorbit(1,-1,true));
		locators[28].locations.push_back(locatorbit(1,0,true));
		locators[28].locations.push_back(locatorbit(1,1,true));
		locators[28].locations.push_back(locatorbit(1,2,true));
		locators[28].locations.push_back(locatorbit(1,3,true));
		locators[28].locations.push_back(locatorbit(1,4,true));
		locators[28].locations.push_back(locatorbit(2,0,true));
		locators[28].locations.push_back(locatorbit(2,1,true));
		locators[28].locations.push_back(locatorbit(2,2,true));
		locators.push_back(locator());
		locators[29].locations.push_back(locatorbit(-5,4,false));
		locators[29].locations.push_back(locatorbit(-5,5,false));
		locators[29].locations.push_back(locatorbit(-4,3,false));
		locators[29].locations.push_back(locatorbit(-4,5,false));
		locators[29].locations.push_back(locatorbit(-3,3,false));
		locators[29].locations.push_back(locatorbit(-3,4,false));
		locators[29].locations.push_back(locatorbit(-2,1,false));
		locators[29].locations.push_back(locatorbit(-2,2,false));
		locators[29].locations.push_back(locatorbit(-1,0,false));
		locators[29].locations.push_back(locatorbit(-1,2,false));
		locators[29].locations.push_back(locatorbit(0,0,false));
		locators[29].locations.push_back(locatorbit(0,1,false));
		locators[29].locations.push_back(locatorbit(-6,3,true));
		locators[29].locations.push_back(locatorbit(-6,4,true));
		locators[29].locations.push_back(locatorbit(-6,5,true));
		locators[29].locations.push_back(locatorbit(-6,6,true));
		locators[29].locations.push_back(locatorbit(-5,2,true));
		locators[29].locations.push_back(locatorbit(-5,3,true));
		locators[29].locations.push_back(locatorbit(-5,6,true));
		locators[29].locations.push_back(locatorbit(-4,2,true));
		locators[29].locations.push_back(locatorbit(-4,4,true));
		locators[29].locations.push_back(locatorbit(-4,6,true));
		locators[29].locations.push_back(locatorbit(-3,0,true));
		locators[29].locations.push_back(locatorbit(-3,1,true));
		locators[29].locations.push_back(locatorbit(-3,2,true));
		locators[29].locations.push_back(locatorbit(-3,5,true));
		locators[29].locations.push_back(locatorbit(-3,6,true));
		locators[29].locations.push_back(locatorbit(-2,-1,true));
		locators[29].locations.push_back(locatorbit(-2,0,true));
		locators[29].locations.push_back(locatorbit(-2,3,true));
		locators[29].locations.push_back(locatorbit(-2,4,true));
		locators[29].locations.push_back(locatorbit(-2,5,true));
		locators[29].locations.push_back(locatorbit(-1,-1,true));
		locators[29].locations.push_back(locatorbit(-1,1,true));
		locators[29].locations.push_back(locatorbit(-1,3,true));
		locators[29].locations.push_back(locatorbit(0,-1,true));
		locators[29].locations.push_back(locatorbit(0,2,true));
		locators[29].locations.push_back(locatorbit(0,3,true));
		locators[29].locations.push_back(locatorbit(1,-1,true));
		locators[29].locations.push_back(locatorbit(1,0,true));
		locators[29].locations.push_back(locatorbit(1,1,true));
		locators[29].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[30].locations.push_back(locatorbit(-5,-4,false));
		locators[30].locations.push_back(locatorbit(-5,-3,false));
		locators[30].locations.push_back(locatorbit(-4,-4,false));
		locators[30].locations.push_back(locatorbit(-4,-2,false));
		locators[30].locations.push_back(locatorbit(-3,-3,false));
		locators[30].locations.push_back(locatorbit(-3,-2,false));
		locators[30].locations.push_back(locatorbit(-2,-1,false));
		locators[30].locations.push_back(locatorbit(-2,0,false));
		locators[30].locations.push_back(locatorbit(-1,-1,false));
		locators[30].locations.push_back(locatorbit(-1,1,false));
		locators[30].locations.push_back(locatorbit(0,0,false));
		locators[30].locations.push_back(locatorbit(0,1,false));
		locators[30].locations.push_back(locatorbit(-6,-5,true));
		locators[30].locations.push_back(locatorbit(-6,-4,true));
		locators[30].locations.push_back(locatorbit(-6,-3,true));
		locators[30].locations.push_back(locatorbit(-6,-2,true));
		locators[30].locations.push_back(locatorbit(-5,-5,true));
		locators[30].locations.push_back(locatorbit(-5,-2,true));
		locators[30].locations.push_back(locatorbit(-5,-1,true));
		locators[30].locations.push_back(locatorbit(-4,-5,true));
		locators[30].locations.push_back(locatorbit(-4,-3,true));
		locators[30].locations.push_back(locatorbit(-4,-1,true));
		locators[30].locations.push_back(locatorbit(-3,-5,true));
		locators[30].locations.push_back(locatorbit(-3,-4,true));
		locators[30].locations.push_back(locatorbit(-3,-1,true));
		locators[30].locations.push_back(locatorbit(-3,0,true));
		locators[30].locations.push_back(locatorbit(-3,1,true));
		locators[30].locations.push_back(locatorbit(-2,-4,true));
		locators[30].locations.push_back(locatorbit(-2,-3,true));
		locators[30].locations.push_back(locatorbit(-2,-2,true));
		locators[30].locations.push_back(locatorbit(-2,1,true));
		locators[30].locations.push_back(locatorbit(-2,2,true));
		locators[30].locations.push_back(locatorbit(-1,-2,true));
		locators[30].locations.push_back(locatorbit(-1,0,true));
		locators[30].locations.push_back(locatorbit(-1,2,true));
		locators[30].locations.push_back(locatorbit(0,-2,true));
		locators[30].locations.push_back(locatorbit(0,-1,true));
		locators[30].locations.push_back(locatorbit(0,2,true));
		locators[30].locations.push_back(locatorbit(1,-1,true));
		locators[30].locations.push_back(locatorbit(1,0,true));
		locators[30].locations.push_back(locatorbit(1,1,true));
		locators[30].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[31].locations.push_back(locatorbit(-3,-2,false));
		locators[31].locations.push_back(locatorbit(-3,-1,false));
		locators[31].locations.push_back(locatorbit(-2,-2,false));
		locators[31].locations.push_back(locatorbit(-1,1,false));
		locators[31].locations.push_back(locatorbit(0,0,false));
		locators[31].locations.push_back(locatorbit(0,1,false));
		locators[31].locations.push_back(locatorbit(-4,-3,true));
		locators[31].locations.push_back(locatorbit(-4,-2,true));
		locators[31].locations.push_back(locatorbit(-4,-1,true));
		locators[31].locations.push_back(locatorbit(-4,0,true));
		locators[31].locations.push_back(locatorbit(-3,-3,true));
		locators[31].locations.push_back(locatorbit(-3,0,true));
		locators[31].locations.push_back(locatorbit(-2,-3,true));
		locators[31].locations.push_back(locatorbit(-2,-1,true));
		locators[31].locations.push_back(locatorbit(-2,0,true));
		locators[31].locations.push_back(locatorbit(-2,1,true));
		locators[31].locations.push_back(locatorbit(-2,2,true));
		locators[31].locations.push_back(locatorbit(-1,-3,true));
		locators[31].locations.push_back(locatorbit(-1,-2,true));
		locators[31].locations.push_back(locatorbit(-1,-1,true));
		locators[31].locations.push_back(locatorbit(-1,0,true));
		locators[31].locations.push_back(locatorbit(-1,2,true));
		locators[31].locations.push_back(locatorbit(0,-1,true));
		locators[31].locations.push_back(locatorbit(0,2,true));
		locators[31].locations.push_back(locatorbit(1,-1,true));
		locators[31].locations.push_back(locatorbit(1,0,true));
		locators[31].locations.push_back(locatorbit(1,1,true));
		locators[31].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[32].locations.push_back(locatorbit(-3,-2,false));
		locators[32].locations.push_back(locatorbit(-3,-1,false));
		locators[32].locations.push_back(locatorbit(-2,-2,false));
		locators[32].locations.push_back(locatorbit(-2,-1,false));
		locators[32].locations.push_back(locatorbit(-1,0,false));
		locators[32].locations.push_back(locatorbit(-1,1,false));
		locators[32].locations.push_back(locatorbit(0,0,false));
		locators[32].locations.push_back(locatorbit(0,1,false));
		locators[32].locations.push_back(locatorbit(-4,-3,true));
		locators[32].locations.push_back(locatorbit(-4,-2,true));
		locators[32].locations.push_back(locatorbit(-4,-1,true));
		locators[32].locations.push_back(locatorbit(-4,0,true));
		locators[32].locations.push_back(locatorbit(-3,-3,true));
		locators[32].locations.push_back(locatorbit(-3,0,true));
		locators[32].locations.push_back(locatorbit(-2,-3,true));
		locators[32].locations.push_back(locatorbit(-2,0,true));
		locators[32].locations.push_back(locatorbit(-2,1,true));
		locators[32].locations.push_back(locatorbit(-2,2,true));
		locators[32].locations.push_back(locatorbit(-1,-3,true));
		locators[32].locations.push_back(locatorbit(-1,-2,true));
		locators[32].locations.push_back(locatorbit(-1,-1,true));
		locators[32].locations.push_back(locatorbit(-1,2,true));
		locators[32].locations.push_back(locatorbit(0,-1,true));
		locators[32].locations.push_back(locatorbit(0,2,true));
		locators[32].locations.push_back(locatorbit(1,-1,true));
		locators[32].locations.push_back(locatorbit(1,0,true));
		locators[32].locations.push_back(locatorbit(1,1,true));
		locators[32].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[33].locations.push_back(locatorbit(-3,2,false));
		locators[33].locations.push_back(locatorbit(-3,3,false));
		locators[33].locations.push_back(locatorbit(-2,2,false));
		locators[33].locations.push_back(locatorbit(-2,3,false));
		locators[33].locations.push_back(locatorbit(-1,0,false));
		locators[33].locations.push_back(locatorbit(-1,1,false));
		locators[33].locations.push_back(locatorbit(0,0,false));
		locators[33].locations.push_back(locatorbit(0,1,false));
		locators[33].locations.push_back(locatorbit(-4,1,true));
		locators[33].locations.push_back(locatorbit(-4,2,true));
		locators[33].locations.push_back(locatorbit(-4,3,true));
		locators[33].locations.push_back(locatorbit(-4,4,true));
		locators[33].locations.push_back(locatorbit(-3,1,true));
		locators[33].locations.push_back(locatorbit(-3,4,true));
		locators[33].locations.push_back(locatorbit(-2,-1,true));
		locators[33].locations.push_back(locatorbit(-2,0,true));
		locators[33].locations.push_back(locatorbit(-2,1,true));
		locators[33].locations.push_back(locatorbit(-2,4,true));
		locators[33].locations.push_back(locatorbit(-1,-1,true));
		locators[33].locations.push_back(locatorbit(-1,2,true));
		locators[33].locations.push_back(locatorbit(-1,3,true));
		locators[33].locations.push_back(locatorbit(-1,4,true));
		locators[33].locations.push_back(locatorbit(0,-1,true));
		locators[33].locations.push_back(locatorbit(0,2,true));
		locators[33].locations.push_back(locatorbit(1,-1,true));
		locators[33].locations.push_back(locatorbit(1,0,true));
		locators[33].locations.push_back(locatorbit(1,1,true));
		locators[33].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[34].locations.push_back(locatorbit(-3,2,false));
		locators[34].locations.push_back(locatorbit(-3,3,false));
		locators[34].locations.push_back(locatorbit(-2,3,false));
		locators[34].locations.push_back(locatorbit(-1,0,false));
		locators[34].locations.push_back(locatorbit(0,0,false));
		locators[34].locations.push_back(locatorbit(0,1,false));
		locators[34].locations.push_back(locatorbit(-4,1,true));
		locators[34].locations.push_back(locatorbit(-4,2,true));
		locators[34].locations.push_back(locatorbit(-4,3,true));
		locators[34].locations.push_back(locatorbit(-4,4,true));
		locators[34].locations.push_back(locatorbit(-3,1,true));
		locators[34].locations.push_back(locatorbit(-3,4,true));
		locators[34].locations.push_back(locatorbit(-2,-1,true));
		locators[34].locations.push_back(locatorbit(-2,0,true));
		locators[34].locations.push_back(locatorbit(-2,1,true));
		locators[34].locations.push_back(locatorbit(-2,2,true));
		locators[34].locations.push_back(locatorbit(-2,4,true));
		locators[34].locations.push_back(locatorbit(-1,-1,true));
		locators[34].locations.push_back(locatorbit(-1,1,true));
		locators[34].locations.push_back(locatorbit(-1,2,true));
		locators[34].locations.push_back(locatorbit(-1,3,true));
		locators[34].locations.push_back(locatorbit(-1,4,true));
		locators[34].locations.push_back(locatorbit(0,-1,true));
		locators[34].locations.push_back(locatorbit(0,2,true));
		locators[34].locations.push_back(locatorbit(1,-1,true));
		locators[34].locations.push_back(locatorbit(1,0,true));
		locators[34].locations.push_back(locatorbit(1,1,true));
		locators[34].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[35].locations.push_back(locatorbit(-3,1,false));
		locators[35].locations.push_back(locatorbit(-2,0,false));
		locators[35].locations.push_back(locatorbit(-2,2,false));
		locators[35].locations.push_back(locatorbit(-1,-1,false));
		locators[35].locations.push_back(locatorbit(-1,1,false));
		locators[35].locations.push_back(locatorbit(0,0,false));
		locators[35].locations.push_back(locatorbit(-4,0,true));
		locators[35].locations.push_back(locatorbit(-4,1,true));
		locators[35].locations.push_back(locatorbit(-4,2,true));
		locators[35].locations.push_back(locatorbit(-3,-1,true));
		locators[35].locations.push_back(locatorbit(-3,0,true));
		locators[35].locations.push_back(locatorbit(-3,2,true));
		locators[35].locations.push_back(locatorbit(-3,3,true));
		locators[35].locations.push_back(locatorbit(-2,-2,true));
		locators[35].locations.push_back(locatorbit(-2,-1,true));
		locators[35].locations.push_back(locatorbit(-2,1,true));
		locators[35].locations.push_back(locatorbit(-2,3,true));
		locators[35].locations.push_back(locatorbit(-1,-2,true));
		locators[35].locations.push_back(locatorbit(-1,0,true));
		locators[35].locations.push_back(locatorbit(-1,2,true));
		locators[35].locations.push_back(locatorbit(-1,3,true));
		locators[35].locations.push_back(locatorbit(0,-2,true));
		locators[35].locations.push_back(locatorbit(0,-1,true));
		locators[35].locations.push_back(locatorbit(0,1,true));
		locators[35].locations.push_back(locatorbit(0,2,true));
		locators[35].locations.push_back(locatorbit(1,-1,true));
		locators[35].locations.push_back(locatorbit(1,0,true));
		locators[35].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[36].locations.push_back(locatorbit(-3,-1,false));
		locators[36].locations.push_back(locatorbit(-2,-2,false));
		locators[36].locations.push_back(locatorbit(-2,0,false));
		locators[36].locations.push_back(locatorbit(-1,-1,false));
		locators[36].locations.push_back(locatorbit(-1,1,false));
		locators[36].locations.push_back(locatorbit(0,0,false));
		locators[36].locations.push_back(locatorbit(-4,-2,true));
		locators[36].locations.push_back(locatorbit(-4,-1,true));
		locators[36].locations.push_back(locatorbit(-4,0,true));
		locators[36].locations.push_back(locatorbit(-3,-3,true));
		locators[36].locations.push_back(locatorbit(-3,-2,true));
		locators[36].locations.push_back(locatorbit(-3,0,true));
		locators[36].locations.push_back(locatorbit(-3,1,true));
		locators[36].locations.push_back(locatorbit(-2,-3,true));
		locators[36].locations.push_back(locatorbit(-2,-1,true));
		locators[36].locations.push_back(locatorbit(-2,1,true));
		locators[36].locations.push_back(locatorbit(-2,2,true));
		locators[36].locations.push_back(locatorbit(-1,-3,true));
		locators[36].locations.push_back(locatorbit(-1,-2,true));
		locators[36].locations.push_back(locatorbit(-1,0,true));
		locators[36].locations.push_back(locatorbit(-1,2,true));
		locators[36].locations.push_back(locatorbit(0,-2,true));
		locators[36].locations.push_back(locatorbit(0,-1,true));
		locators[36].locations.push_back(locatorbit(0,1,true));
		locators[36].locations.push_back(locatorbit(0,2,true));
		locators[36].locations.push_back(locatorbit(1,-1,true));
		locators[36].locations.push_back(locatorbit(1,0,true));
		locators[36].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[37].locations.push_back(locatorbit(-2,0,false));
		locators[37].locations.push_back(locatorbit(-1,1,false));
		locators[37].locations.push_back(locatorbit(-1,2,false));
		locators[37].locations.push_back(locatorbit(0,0,false));
		locators[37].locations.push_back(locatorbit(0,1,false));
		locators[37].locations.push_back(locatorbit(-3,-1,true));
		locators[37].locations.push_back(locatorbit(-3,0,true));
		locators[37].locations.push_back(locatorbit(-3,1,true));
		locators[37].locations.push_back(locatorbit(-2,-1,true));
		locators[37].locations.push_back(locatorbit(-2,1,true));
		locators[37].locations.push_back(locatorbit(-2,2,true));
		locators[37].locations.push_back(locatorbit(-2,3,true));
		locators[37].locations.push_back(locatorbit(-1,-1,true));
		locators[37].locations.push_back(locatorbit(-1,0,true));
		locators[37].locations.push_back(locatorbit(-1,3,true));
		locators[37].locations.push_back(locatorbit(0,-1,true));
		locators[37].locations.push_back(locatorbit(0,2,true));
		locators[37].locations.push_back(locatorbit(0,3,true));
		locators[37].locations.push_back(locatorbit(1,-1,true));
		locators[37].locations.push_back(locatorbit(1,0,true));
		locators[37].locations.push_back(locatorbit(1,1,true));
		locators[37].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[38].locations.push_back(locatorbit(-2,0,false));
		locators[38].locations.push_back(locatorbit(-2,1,false));
		locators[38].locations.push_back(locatorbit(-1,1,false));
		locators[38].locations.push_back(locatorbit(-1,2,false));
		locators[38].locations.push_back(locatorbit(0,0,false));
		locators[38].locations.push_back(locatorbit(-3,-1,true));
		locators[38].locations.push_back(locatorbit(-3,0,true));
		locators[38].locations.push_back(locatorbit(-3,1,true));
		locators[38].locations.push_back(locatorbit(-3,2,true));
		locators[38].locations.push_back(locatorbit(-2,-1,true));
		locators[38].locations.push_back(locatorbit(-2,2,true));
		locators[38].locations.push_back(locatorbit(-2,3,true));
		locators[38].locations.push_back(locatorbit(-1,-1,true));
		locators[38].locations.push_back(locatorbit(-1,0,true));
		locators[38].locations.push_back(locatorbit(-1,3,true));
		locators[38].locations.push_back(locatorbit(0,-1,true));
		locators[38].locations.push_back(locatorbit(0,1,true));
		locators[38].locations.push_back(locatorbit(0,2,true));
		locators[38].locations.push_back(locatorbit(0,3,true));
		locators[38].locations.push_back(locatorbit(1,-1,true));
		locators[38].locations.push_back(locatorbit(1,0,true));
		locators[38].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[39].locations.push_back(locatorbit(-2,1,false));
		locators[39].locations.push_back(locatorbit(-1,-1,false));
		locators[39].locations.push_back(locatorbit(-1,0,false));
		locators[39].locations.push_back(locatorbit(0,0,false));
		locators[39].locations.push_back(locatorbit(0,1,false));
		locators[39].locations.push_back(locatorbit(-3,0,true));
		locators[39].locations.push_back(locatorbit(-3,1,true));
		locators[39].locations.push_back(locatorbit(-3,2,true));
		locators[39].locations.push_back(locatorbit(-2,-2,true));
		locators[39].locations.push_back(locatorbit(-2,-1,true));
		locators[39].locations.push_back(locatorbit(-2,0,true));
		locators[39].locations.push_back(locatorbit(-2,2,true));
		locators[39].locations.push_back(locatorbit(-1,-2,true));
		locators[39].locations.push_back(locatorbit(-1,1,true));
		locators[39].locations.push_back(locatorbit(-1,2,true));
		locators[39].locations.push_back(locatorbit(0,-2,true));
		locators[39].locations.push_back(locatorbit(0,-1,true));
		locators[39].locations.push_back(locatorbit(0,2,true));
		locators[39].locations.push_back(locatorbit(1,-1,true));
		locators[39].locations.push_back(locatorbit(1,0,true));
		locators[39].locations.push_back(locatorbit(1,1,true));
		locators[39].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[40].locations.push_back(locatorbit(-2,-1,false));
		locators[40].locations.push_back(locatorbit(-2,0,false));
		locators[40].locations.push_back(locatorbit(-1,-2,false));
		locators[40].locations.push_back(locatorbit(-1,-1,false));
		locators[40].locations.push_back(locatorbit(0,0,false));
		locators[40].locations.push_back(locatorbit(-3,-2,true));
		locators[40].locations.push_back(locatorbit(-3,-1,true));
		locators[40].locations.push_back(locatorbit(-3,0,true));
		locators[40].locations.push_back(locatorbit(-3,1,true));
		locators[40].locations.push_back(locatorbit(-2,-3,true));
		locators[40].locations.push_back(locatorbit(-2,-2,true));
		locators[40].locations.push_back(locatorbit(-2,1,true));
		locators[40].locations.push_back(locatorbit(-1,-3,true));
		locators[40].locations.push_back(locatorbit(-1,0,true));
		locators[40].locations.push_back(locatorbit(-1,1,true));
		locators[40].locations.push_back(locatorbit(0,-3,true));
		locators[40].locations.push_back(locatorbit(0,-2,true));
		locators[40].locations.push_back(locatorbit(0,-1,true));
		locators[40].locations.push_back(locatorbit(0,1,true));
		locators[40].locations.push_back(locatorbit(1,-1,true));
		locators[40].locations.push_back(locatorbit(1,0,true));
		locators[40].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[41].locations.push_back(locatorbit(-2,1,false));
		locators[41].locations.push_back(locatorbit(-1,-1,false));
		locators[41].locations.push_back(locatorbit(-1,1,false));
		locators[41].locations.push_back(locatorbit(0,0,false));
		locators[41].locations.push_back(locatorbit(0,1,false));
		locators[41].locations.push_back(locatorbit(-3,0,true));
		locators[41].locations.push_back(locatorbit(-3,1,true));
		locators[41].locations.push_back(locatorbit(-3,2,true));
		locators[41].locations.push_back(locatorbit(-2,-2,true));
		locators[41].locations.push_back(locatorbit(-2,-1,true));
		locators[41].locations.push_back(locatorbit(-2,0,true));
		locators[41].locations.push_back(locatorbit(-2,2,true));
		locators[41].locations.push_back(locatorbit(-1,-2,true));
		locators[41].locations.push_back(locatorbit(-1,0,true));
		locators[41].locations.push_back(locatorbit(-1,2,true));
		locators[41].locations.push_back(locatorbit(0,-2,true));
		locators[41].locations.push_back(locatorbit(0,-1,true));
		locators[41].locations.push_back(locatorbit(0,2,true));
		locators[41].locations.push_back(locatorbit(1,-1,true));
		locators[41].locations.push_back(locatorbit(1,0,true));
		locators[41].locations.push_back(locatorbit(1,1,true));
		locators[41].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[42].locations.push_back(locatorbit(-2,-1,false));
		locators[42].locations.push_back(locatorbit(-2,0,false));
		locators[42].locations.push_back(locatorbit(-1,-2,false));
		locators[42].locations.push_back(locatorbit(-1,0,false));
		locators[42].locations.push_back(locatorbit(0,0,false));
		locators[42].locations.push_back(locatorbit(-3,-2,true));
		locators[42].locations.push_back(locatorbit(-3,-1,true));
		locators[42].locations.push_back(locatorbit(-3,0,true));
		locators[42].locations.push_back(locatorbit(-3,1,true));
		locators[42].locations.push_back(locatorbit(-2,-3,true));
		locators[42].locations.push_back(locatorbit(-2,-2,true));
		locators[42].locations.push_back(locatorbit(-2,1,true));
		locators[42].locations.push_back(locatorbit(-1,-3,true));
		locators[42].locations.push_back(locatorbit(-1,-1,true));
		locators[42].locations.push_back(locatorbit(-1,1,true));
		locators[42].locations.push_back(locatorbit(0,-3,true));
		locators[42].locations.push_back(locatorbit(0,-2,true));
		locators[42].locations.push_back(locatorbit(0,-1,true));
		locators[42].locations.push_back(locatorbit(0,1,true));
		locators[42].locations.push_back(locatorbit(1,-1,true));
		locators[42].locations.push_back(locatorbit(1,0,true));
		locators[42].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[43].locations.push_back(locatorbit(-2,0,false));
		locators[43].locations.push_back(locatorbit(-1,0,false));
		locators[43].locations.push_back(locatorbit(-1,2,false));
		locators[43].locations.push_back(locatorbit(0,0,false));
		locators[43].locations.push_back(locatorbit(0,1,false));
		locators[43].locations.push_back(locatorbit(-3,-1,true));
		locators[43].locations.push_back(locatorbit(-3,0,true));
		locators[43].locations.push_back(locatorbit(-3,1,true));
		locators[43].locations.push_back(locatorbit(-2,-1,true));
		locators[43].locations.push_back(locatorbit(-2,1,true));
		locators[43].locations.push_back(locatorbit(-2,2,true));
		locators[43].locations.push_back(locatorbit(-2,3,true));
		locators[43].locations.push_back(locatorbit(-1,-1,true));
		locators[43].locations.push_back(locatorbit(-1,1,true));
		locators[43].locations.push_back(locatorbit(-1,3,true));
		locators[43].locations.push_back(locatorbit(0,-1,true));
		locators[43].locations.push_back(locatorbit(0,2,true));
		locators[43].locations.push_back(locatorbit(0,3,true));
		locators[43].locations.push_back(locatorbit(1,-1,true));
		locators[43].locations.push_back(locatorbit(1,0,true));
		locators[43].locations.push_back(locatorbit(1,1,true));
		locators[43].locations.push_back(locatorbit(1,2,true));
		locators.push_back(locator());
		locators[44].locations.push_back(locatorbit(-2,0,false));
		locators[44].locations.push_back(locatorbit(-2,1,false));
		locators[44].locations.push_back(locatorbit(-1,0,false));
		locators[44].locations.push_back(locatorbit(-1,2,false));
		locators[44].locations.push_back(locatorbit(0,0,false));
		locators[44].locations.push_back(locatorbit(-3,-1,true));
		locators[44].locations.push_back(locatorbit(-3,0,true));
		locators[44].locations.push_back(locatorbit(-3,1,true));
		locators[44].locations.push_back(locatorbit(-3,2,true));
		locators[44].locations.push_back(locatorbit(-2,-1,true));
		locators[44].locations.push_back(locatorbit(-2,2,true));
		locators[44].locations.push_back(locatorbit(-2,3,true));
		locators[44].locations.push_back(locatorbit(-1,-1,true));
		locators[44].locations.push_back(locatorbit(-1,1,true));
		locators[44].locations.push_back(locatorbit(-1,3,true));
		locators[44].locations.push_back(locatorbit(0,-1,true));
		locators[44].locations.push_back(locatorbit(0,1,true));
		locators[44].locations.push_back(locatorbit(0,2,true));
		locators[44].locations.push_back(locatorbit(0,3,true));
		locators[44].locations.push_back(locatorbit(1,-1,true));
		locators[44].locations.push_back(locatorbit(1,0,true));
		locators[44].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[45].locations.push_back(locatorbit(-2,-1,false));
		locators[45].locations.push_back(locatorbit(-2,1,false));
		locators[45].locations.push_back(locatorbit(-1,0,false));
		locators[45].locations.push_back(locatorbit(-1,1,false));
		locators[45].locations.push_back(locatorbit(0,0,false));
		locators[45].locations.push_back(locatorbit(-3,-2,true));
		locators[45].locations.push_back(locatorbit(-3,-1,true));
		locators[45].locations.push_back(locatorbit(-3,0,true));
		locators[45].locations.push_back(locatorbit(-3,1,true));
		locators[45].locations.push_back(locatorbit(-3,2,true));
		locators[45].locations.push_back(locatorbit(-2,-2,true));
		locators[45].locations.push_back(locatorbit(-2,0,true));
		locators[45].locations.push_back(locatorbit(-2,2,true));
		locators[45].locations.push_back(locatorbit(-1,-2,true));
		locators[45].locations.push_back(locatorbit(-1,-1,true));
		locators[45].locations.push_back(locatorbit(-1,2,true));
		locators[45].locations.push_back(locatorbit(0,-1,true));
		locators[45].locations.push_back(locatorbit(0,1,true));
		locators[45].locations.push_back(locatorbit(0,2,true));
		locators[45].locations.push_back(locatorbit(1,-1,true));
		locators[45].locations.push_back(locatorbit(1,0,true));
		locators[45].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[46].locations.push_back(locatorbit(-2,1,false));
		locators[46].locations.push_back(locatorbit(-1,1,false));
		locators[46].locations.push_back(locatorbit(-1,2,false));
		locators[46].locations.push_back(locatorbit(0,0,false));
		locators[46].locations.push_back(locatorbit(0,2,false));
		locators[46].locations.push_back(locatorbit(-3,0,true));
		locators[46].locations.push_back(locatorbit(-3,1,true));
		locators[46].locations.push_back(locatorbit(-3,2,true));
		locators[46].locations.push_back(locatorbit(-2,0,true));
		locators[46].locations.push_back(locatorbit(-2,2,true));
		locators[46].locations.push_back(locatorbit(-2,3,true));
		locators[46].locations.push_back(locatorbit(-1,-1,true));
		locators[46].locations.push_back(locatorbit(-1,0,true));
		locators[46].locations.push_back(locatorbit(-1,3,true));
		locators[46].locations.push_back(locatorbit(0,-1,true));
		locators[46].locations.push_back(locatorbit(0,1,true));
		locators[46].locations.push_back(locatorbit(0,3,true));
		locators[46].locations.push_back(locatorbit(1,-1,true));
		locators[46].locations.push_back(locatorbit(1,0,true));
		locators[46].locations.push_back(locatorbit(1,1,true));
		locators[46].locations.push_back(locatorbit(1,2,true));
		locators[46].locations.push_back(locatorbit(1,3,true));
		locators.push_back(locator());
		locators[47].locations.push_back(locatorbit(-2,-1,false));
		locators[47].locations.push_back(locatorbit(-2,1,false));
		locators[47].locations.push_back(locatorbit(-1,-1,false));
		locators[47].locations.push_back(locatorbit(-1,0,false));
		locators[47].locations.push_back(locatorbit(0,0,false));
		locators[47].locations.push_back(locatorbit(-3,-2,true));
		locators[47].locations.push_back(locatorbit(-3,-1,true));
		locators[47].locations.push_back(locatorbit(-3,0,true));
		locators[47].locations.push_back(locatorbit(-3,1,true));
		locators[47].locations.push_back(locatorbit(-3,2,true));
		locators[47].locations.push_back(locatorbit(-2,-2,true));
		locators[47].locations.push_back(locatorbit(-2,0,true));
		locators[47].locations.push_back(locatorbit(-2,2,true));
		locators[47].locations.push_back(locatorbit(-1,-2,true));
		locators[47].locations.push_back(locatorbit(-1,1,true));
		locators[47].locations.push_back(locatorbit(-1,2,true));
		locators[47].locations.push_back(locatorbit(0,-2,true));
		locators[47].locations.push_back(locatorbit(0,-1,true));
		locators[47].locations.push_back(locatorbit(0,1,true));
		locators[47].locations.push_back(locatorbit(1,-1,true));
		locators[47].locations.push_back(locatorbit(1,0,true));
		locators[47].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[48].locations.push_back(locatorbit(-2,1,false));
		locators[48].locations.push_back(locatorbit(-1,0,false));
		locators[48].locations.push_back(locatorbit(-1,1,false));
		locators[48].locations.push_back(locatorbit(0,0,false));
		locators[48].locations.push_back(locatorbit(0,2,false));
		locators[48].locations.push_back(locatorbit(-3,0,true));
		locators[48].locations.push_back(locatorbit(-3,1,true));
		locators[48].locations.push_back(locatorbit(-3,2,true));
		locators[48].locations.push_back(locatorbit(-2,-1,true));
		locators[48].locations.push_back(locatorbit(-2,0,true));
		locators[48].locations.push_back(locatorbit(-2,2,true));
		locators[48].locations.push_back(locatorbit(-1,-1,true));
		locators[48].locations.push_back(locatorbit(-1,2,true));
		locators[48].locations.push_back(locatorbit(-1,3,true));
		locators[48].locations.push_back(locatorbit(0,-1,true));
		locators[48].locations.push_back(locatorbit(0,1,true));
		locators[48].locations.push_back(locatorbit(0,3,true));
		locators[48].locations.push_back(locatorbit(1,-1,true));
		locators[48].locations.push_back(locatorbit(1,0,true));
		locators[48].locations.push_back(locatorbit(1,1,true));
		locators[48].locations.push_back(locatorbit(1,2,true));
		locators[48].locations.push_back(locatorbit(1,3,true));
		locators.push_back(locator());
		locators[49].locations.push_back(locatorbit(-2,1,false));
		locators[49].locations.push_back(locatorbit(-1,2,false));
		locators[49].locations.push_back(locatorbit(0,0,false));
		locators[49].locations.push_back(locatorbit(0,1,false));
		locators[49].locations.push_back(locatorbit(0,2,false));
		locators[49].locations.push_back(locatorbit(-3,0,true));
		locators[49].locations.push_back(locatorbit(-3,1,true));
		locators[49].locations.push_back(locatorbit(-3,2,true));
		locators[49].locations.push_back(locatorbit(-2,0,true));
		locators[49].locations.push_back(locatorbit(-2,2,true));
		locators[49].locations.push_back(locatorbit(-2,3,true));
		locators[49].locations.push_back(locatorbit(-1,-1,true));
		locators[49].locations.push_back(locatorbit(-1,0,true));
		locators[49].locations.push_back(locatorbit(-1,1,true));
		locators[49].locations.push_back(locatorbit(-1,3,true));
		locators[49].locations.push_back(locatorbit(0,-1,true));
		locators[49].locations.push_back(locatorbit(0,3,true));
		locators[49].locations.push_back(locatorbit(1,-1,true));
		locators[49].locations.push_back(locatorbit(1,0,true));
		locators[49].locations.push_back(locatorbit(1,1,true));
		locators[49].locations.push_back(locatorbit(1,2,true));
		locators[49].locations.push_back(locatorbit(1,3,true));
		locators.push_back(locator());
		locators[50].locations.push_back(locatorbit(-2,-1,false));
		locators[50].locations.push_back(locatorbit(-2,0,false));
		locators[50].locations.push_back(locatorbit(-2,1,false));
		locators[50].locations.push_back(locatorbit(-1,1,false));
		locators[50].locations.push_back(locatorbit(0,0,false));
		locators[50].locations.push_back(locatorbit(-3,-2,true));
		locators[50].locations.push_back(locatorbit(-3,-1,true));
		locators[50].locations.push_back(locatorbit(-3,0,true));
		locators[50].locations.push_back(locatorbit(-3,1,true));
		locators[50].locations.push_back(locatorbit(-3,2,true));
		locators[50].locations.push_back(locatorbit(-2,-2,true));
		locators[50].locations.push_back(locatorbit(-2,2,true));
		locators[50].locations.push_back(locatorbit(-1,-2,true));
		locators[50].locations.push_back(locatorbit(-1,-1,true));
		locators[50].locations.push_back(locatorbit(-1,0,true));
		locators[50].locations.push_back(locatorbit(-1,2,true));
		locators[50].locations.push_back(locatorbit(0,-1,true));
		locators[50].locations.push_back(locatorbit(0,1,true));
		locators[50].locations.push_back(locatorbit(0,2,true));
		locators[50].locations.push_back(locatorbit(1,-1,true));
		locators[50].locations.push_back(locatorbit(1,0,true));
		locators[50].locations.push_back(locatorbit(1,1,true));
		locators.push_back(locator());
		locators[51].locations.push_back(locatorbit(-2,1,false));
		locators[51].locations.push_back(locatorbit(-1,0,false));
		locators[51].locations.push_back(locatorbit(0,0,false));
		locators[51].locations.push_back(locatorbit(0,1,false));
		locators[51].locations.push_back(locatorbit(0,2,false));
		locators[51].locations.push_back(locatorbit(-3,0,true));
		locators[51].locations.push_back(locatorbit(-3,1,true));
		locators[51].locations.push_back(locatorbit(-3,2,true));
		locators[51].locations.push_back(locatorbit(-2,-1,true));
		locators[51].locations.push_back(locatorbit(-2,0,true));
		locators[51].locations.push_back(locatorbit(-2,2,true));
		locators[51].locations.push_back(locatorbit(-1,-1,true));
		locators[51].locations.push_back(locatorbit(-1,1,true));
		locators[51].locations.push_back(locatorbit(-1,2,true));
		locators[51].locations.push_back(locatorbit(-1,3,true));
		locators[51].locations.push_back(locatorbit(0,-1,true));
		locators[51].locations.push_back(locatorbit(0,3,true));
		locators[51].locations.push_back(locatorbit(1,-1,true));
		locators[51].locations.push_back(locatorbit(1,0,true));
		locators[51].locations.push_back(locatorbit(1,1,true));
		locators[51].locations.push_back(locatorbit(1,2,true));
		locators[51].locations.push_back(locatorbit(1,3,true));
		locators.push_back(locator());
		locators[52].locations.push_back(locatorbit(-2,-1,false));
		locators[52].locations.push_back(locatorbit(-2,0,false));
		locators[52].locations.push_back(locatorbit(-2,1,false));
		locators[52].locations.push_back(locatorbit(-1,-1,false));
		locators[52].locations.push_back(locatorbit(0,0,false));
		locators[52].locations.push_back(locatorbit(-3,-2,true));
		locators[52].locations.push_back(locatorbit(-3,-1,true));
		locators[52].locations.push_back(locatorbit(-3,0,true));
		locators[52].locations.push_back(locatorbit(-3,1,true));
		locators[52].locations.push_back(locatorbit(-3,2,true));
		locators[52].locations.push_back(locatorbit(-2,-2,true));
		locators[52].locations.push_back(locatorbit(-2,2,true));
		locators[52].locations.push_back(locatorbit(-1,-2,true));
		locators[52].locations.push_back(locatorbit(-1,0,true));
		locators[52].locations.push_back(locatorbit(-1,1,true));
		locators[52].locations.push_back(locatorbit(-1,2,true));
		locators[52].locations.push_back(locatorbit(0,-2,true));
		locators[52].locations.push_back(locatorbit(0,-1,true));
		locators[52].locations.push_back(locatorbit(0,1,true));
		locators[52].locations.push_back(locatorbit(1,-1,true));
		locators[52].locations.push_back(locatorbit(1,0,true));
		locators[52].locations.push_back(locatorbit(1,1,true));
	}
};


//Compares strings first by length, then by lexicographical ordering.
//A hash character is worse than anything else.
string compare_representations(const string& a, const string& b)
{
    if (a == "#")
        return b;
    else if (b == "#")
        return a;
    else if (a.length() < b.length())
        return a;
    else if (b.length() < a.length())
        return b;
    else if (a < b)
        return a;
    else
        return b;
}

//A subroutine used by canonise:
string canonise_orientation(const lifestate& ls, int length, int breadth, int ox, int oy, int a, int b, int c, int d)
{
    string representation; 

    string chars("0123456789abcdefghijklmnopqrstuvwxyz");
	
	int maxval = int((breadth - 1) / 5) + 1; 
	
    for(int v = 0; v < maxval; v++)
	{
        int zeroes = 0;
		
        if (v != 0)
            representation += "z";
		
        for(int u = 0; u < length; u++)
		{
            int baudot = 0;
			
            for(int w = 0; w < 5; w++)
			{
                int x = ox + a * u + b * (5 * v + w);
                int y = oy + c * u + d * (5 * v + w);
                baudot = (baudot >> 1) + 16 * ls.get(x, y);
			}
			
            if (baudot == 0)
                zeroes++;
            else
			{
                if (zeroes > 0)
				{   
					if (zeroes == 1)
                        representation += "0";
                    else if (zeroes == 2)
                        representation += "w";
                    else if (zeroes == 3)
                        representation += "x";
                    else
                    {    
						representation += "y";
                        representation += chars[zeroes - 4];
					}
				}
				
                zeroes = 0;
                representation += chars[baudot];
			}
		}
	}
    return representation;
}

string slcanonise(const lifestate& ls)
{
	int rect[4];
	ls.getrect(rect);
	
	string representation = canonise_orientation(ls, rect[2], rect[3], rect[0], rect[1], 1, 0, 0, 1);
	
	representation = compare_representations(representation, canonise_orientation(ls, rect[2], rect[3], rect[0]+rect[2]-1, rect[1], -1, 0, 0, 1));
	
	representation = compare_representations(representation, canonise_orientation(ls, rect[2], rect[3], rect[0], rect[1]+rect[3]-1, 1, 0, 0, -1));
	
	representation = compare_representations(representation, canonise_orientation(ls, rect[2], rect[3], rect[0]+rect[2]-1, rect[1]+rect[3]-1, -1, 0, 0, -1));
	
	representation = compare_representations(representation, canonise_orientation(ls, rect[3], rect[2], rect[0], rect[1], 0, 1, 1, 0));
	
	representation = compare_representations(representation, canonise_orientation(ls, rect[3], rect[2], rect[0]+rect[2]-1, rect[1], 0, -1, 1, 0));
	
	representation = compare_representations(representation, canonise_orientation(ls, rect[3], rect[2], rect[0], rect[1]+rect[3]-1, 0, 1, -1, 0));
	
	representation = compare_representations(representation, canonise_orientation(ls, rect[3], rect[2], rect[0]+rect[2]-1, rect[1]+rect[3]-1, 0, -1, -1, 0));
	
	string prefix = "s" + to_string(ls.getpop());
	
	return ("x" + prefix + "_" + representation);
}

