#define NOMINMAX  // disable covering std::numeric_limits<>::min()

#include <omp.h>
#include <iostream>
#include <windows.h>
#include <cmath>
#include <iomanip>
#include <limits>
#include <random>

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


namespace LeibnizPi
{
	/* calculating pi with leibniz formula and openmp
	 */
	int run()
	{
		double pi = 0;
		int count = 100000;

#pragma omp parallel for reduction(+:pi)
		for (int i = 0; i < count; i++) {
			pi += pow(-1, i) / (2.0 * i + 1);
		}
		
		cout << "Pi = " << setprecision(10) << 4 * pi << '\n';
		return 0;
	}
}

namespace VectorDotProduct
{
	/* calculate dot product of vectors a(a1, a2, a3) and b(b1, b2, b3)
	 * a * b = a1*b1 + a2*b2 + a3*b3
	 */
	int run()
	{
		const int size = 10;
		double prod = 0,
			A[size] = { 1, 5, 7, 5, 8, 4, 6, 1, 10, 0},
			B[size] = { 3, 5, 1, 6, 8, 2, 6, 9, 0, 2};

#pragma omp parallel for shared(A, B) reduction(+:prod)
		for (int i = 0; i < size; i++) {
			prod += A[i] * B[i];
			cout << omp_get_thread_num() << '\n';
		}

		cout << "Dot product: " << prod << '\n';
		return 0;
	}
}


namespace MaxValue
{
	int run()
	{
		const int size = 15;
		int V[size];
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution dist(-1000, 1000);
		for (int i = 0; i < size; i++) {
			V[i] = dist(gen);
			cout << V[i] << " ";
		}
		cout << '\n';

		int max = numeric_limits<int>::min();

#pragma omp parallel for
		for (int i = 0; i < size; i++) {
			if (V[i] > max)
#pragma omp critical
			{
				max = V[i];
			}
		}

		cout << "Max value: " << max << '\n';
		return 0;
	}
}


int main()
{
	//Schedule::run_static();
	//Schedule::run_guided();
	//Sections::run();
	//LeibnizPi::run();
	//VectorDotProduct::run();
	MaxValue::run();

	return 0;
}