// C++ program to generate random double
// uniform_real_distribution and
// default_random_engine
#include <iostream>
#include <random>
using namespace std;
 
// Driver Code
int main()
{
    // Declaring the upper and lower
    // bounds
    double lower_bound = 0;
    double upper_bound = 100;
 
    uniform_real_distribution<double> unif(lower_bound,
                                           upper_bound);
 
    default_random_engine re;
 
    // Getting a random double value
    double random_double = unif(re);
 
    cout << random_double << endl;
 
    return 0;
}