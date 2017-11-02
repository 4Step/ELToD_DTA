//*********************************************************
//	Random.hpp - random number class
//*********************************************************

#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Random Class definition
//---------------------------------------------------------

class Random
{
public:
	Random (int seed = 0);

	int Seed (int seed);
	int Seed (void)             { return ((int) _next_random_number); }

	int Percent (void)          { return ((int) (Probability () * 100.0 + 0.5)); }

	double Probability (void);
	double Probability (int id) { _next_random_number = id; return (Probability ()); }

	void Randomize (Integers &list);

private:
	unsigned int _next_random_number;
};
#endif
