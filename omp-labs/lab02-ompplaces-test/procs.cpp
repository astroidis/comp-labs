#include <iostream>
#include <omp.h>

using namespace std;


int main()
{
    cout << "Places available: " << omp_get_num_places() << '\n';

#pragma omp parallel proc_bind(spread)
    {
        int t = omp_get_thread_num();
        int p = omp_get_place_num();  // returns the place number of the place to which the encountering thread is bound.
        printf("Thread %d at Place %d\n", t, p);
        printf("Processors at this place: %d\n", omp_get_place_num_procs(p));
    }
    return 0;
}
