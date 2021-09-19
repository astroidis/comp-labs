#include <iostream>
#include <omp.h>

using namespace std;

int main()
{
    #ifdef _OPENMP
    cout << "OpenMP version " << _OPENMP << '\n';
    #endif

    return 0;
}
