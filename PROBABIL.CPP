_CLASSDEF(Probability)
// setup a class for the different probabilities
class Probability
{
BYTE* nums;   // an array of unsigned integers to fiddle with
int howmany; // how many numbers in array
int first;   // what's the first non-zero value?
public:
	Probability(BYTE* values, int max);   // pass an array of ints to do probs
	void Cauchy();
	void Linear();
	void Uniform();
	void Gaussian();
	void Exponent();
	void Poisson();
};

Probability::Probability(BYTE* values, int max)
{
	nums = values;
	howmany = max;
	srand(rand());  // random seed number

	// get the first non-zero #, used to set various params in prob fn's
	first = 0;
	BOOL hitnum = FALSE;
     int i=0;
	while ( i < howmany  && ! hitnum )  {
		if ( *(nums+i) ) {
			first = *(nums+i);
			hitnum = TRUE;
   	}
		i++;
   }
}

void Probability::Cauchy()
{
	double u;
	for (int i=0; i<howmany; i++)
		if ( *(nums+i) )  { // `first' is our alpha
			u = (abs(first) * tan(  ( (double) (rand() % RAND_MAX) / (double) RAND_MAX ) * 1.570796327  )) + 1.0;
			*(nums+i) = u>99. ? 99 : u;
		}
}

void Probability::Linear()   // take the smaller of two random #'s
{
	int u1, u2, i;
	for (i=0; i<howmany; i++)
		if ( *(nums+i) )  {
			u1 = 1 + (rand() % 99);
			u2 = 1 + (rand() % 99);
			*(nums+i) = (u1<u2) ? u1 : u2;
      }
}

void Probability::Uniform()   // just use regular rand() function -- it's uniform
{
	for (int i=0; i<howmany; i++)
		if ( *(nums+i) )  // give 'em a random number if not 0
			*(nums+i) = 1 + (rand() % 99);
}

void Probability::Gaussian()
{
	double sum;

	for (int i=0; i<howmany; i++)
		if ( *(nums+i) )  {
			sum = 0.0;
			for (int j=0; j<12; j++)
				sum+= (double) rand() / (double) RAND_MAX ; // to get mean of random nums
			sum = abs(first) * ( sum - 6 ) + (99*(sum/12));
			*(nums+i) = (sum>99) ? 99 : ((sum<1) ? 1 : sum);
		}
}

void Probability::Exponent()
{
	double u;
     int sum;
	for (int i=0; i<howmany; i++)
		if ( *(nums+i) )  {
			do u = (double) rand() / (double) RAND_MAX ;
			while (! u);  // make sure u is not zero (taking log)
			sum = 1 + (int)(99 * -log(u)/abs(first));
			*(nums+i) = (sum>99) ? 99 : ((sum<1) ? 1 : sum);
		}
}

void Probability::Poisson()
{
	int n;
	double u, v;

	for (int i=0; i<howmany; i++)
		if ( *(nums+i) )  {
			n = 0;
			u = (double) rand() / (double) RAND_MAX;
			v = exp(-abs(first));
			while (u > v)  {
				n++;
				u *= (double) rand() / (double) RAND_MAX;
			}
			*(nums+i) = n > 99 ? 99 : ((n<1) ? 1 : n);
     	}
}

