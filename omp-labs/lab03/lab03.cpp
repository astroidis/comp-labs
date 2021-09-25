#include <omp.h>
#include <iostream>
#include <windows.h>

using namespace std;


namespace Schedule
{
	int run_static()
	{
		int A[10], B[10], C[10], n;

		for (int i = 0; i < 10; i++)
		{
			A[i] = i;
			B[i] = 2 * i;
			C[i] = 0;
		}

#pragma omp parallel shared(A, B, C) private(n)
		{
			n = omp_get_thread_num();

#pragma omp for schedule(static)
			for (int i = 0; i < 10; i++) {
				C[i] = A[i] + B[i];
#pragma omp critical
				cout << "Thread " << n << " had added elements with index " << i << "\n";
			}
		}

		return 0;
	}


	int run_guided()
	{
#pragma omp parallel for schedule(guided, 3)
		for (int i = 0; i < 20; i++) {
#pragma omp critical
			{
				cout << "Thread " << omp_get_thread_num() << " executed iteration " << i << "\n";
			}
			Sleep(1);
		}
		
		return 0;
	}
}


namespace Sections
{
	int run()
	{
		int n;

#pragma omp parallel private(n)
		{
			n = omp_get_thread_num();

#pragma omp sections
			{
#pragma omp section
				{
					cout << "First section, thread " << omp_get_thread_num() << '\n';
				}
#pragma omp section
				{
					cout << "Second section, thread " << omp_get_thread_num() << '\n';
				}
			}

			cout << "Parallel region\n";
		}

		return 0;
	}

	/* this code compiles but crashes with runtime error
	 * it says that 'n' used but initialized
	 * but i dont understand how can this be
	 */
	int with_lastprivate()
	{
		int n = 0;

#pragma omp parallel
		{
#pragma omp sections lastprivate(n)
			{
#pragma omp section
				{
					n = 1;
				}
#pragma omp section
				{
					n = 2;
				}
#pragma omp section
				{
					n = 3;
				}
			}

#pragma omp critical
			cout << "In thread " << omp_get_thread_num() << " n = " << n << '\n';
		}

		cout << "In sequiential region n = " << n << '\n';
		return 0;
	}
}


int main()
{
	//Schedule::run_static();
	//Schedule::run_guided();
	Sections::run();

	return 0;
}