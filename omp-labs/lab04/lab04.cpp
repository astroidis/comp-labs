#include <iostream>
#include <omp.h>
#include <format>
#include <Windows.h>

using namespace std;


namespace CalculatePi
{
	int run_atomic()
	{
		double pi = 0;
		int count = 100000;

#pragma omp parallel for
		for (int i = 0; i < count; i++) {
#pragma omp atomic
			pi += pow(-1, i) / (2.0 * i + 1);
		}

		cout << "Pi = " << 4 * pi << '\n';
		return 0;
	}

	int run_lock()
	{
		double pi = 0;
		int count = 100000;
		omp_lock_t lock;
		omp_init_lock(&lock);

#pragma omp parallel for
		for (int i = 0; i < count; i++) {
			omp_set_lock(&lock);
			pi += pow(-1, i) / (2.0 * i + 1);
			omp_unset_lock(&lock);
		}

		cout << "Pi = " << 4 * pi << '\n';
		return 0;
	}
}


namespace Ordered
{
	int run()
	{
		int i, n;
#pragma omp parallel private (i, n)
		{
			n = omp_get_thread_num();
#pragma omp for ordered
			for (i = 0; i < 5; i++) {
				cout << format("Thread {} iteration {}\n", n, i);
#pragma omp ordered
				{
					cout << format("Ordered: Thread {} iteration {}\n", n, i);
				}
			}
		}
		return 0;
	}
}


namespace Locks
{
	omp_lock_t lock;

	int run_set()
	{
		omp_init_lock(&lock);

#pragma omp parallel
		{
			int n = omp_get_thread_num();
			omp_set_lock(&lock);
			cout << "Start locked region, thread " << n << '\n';
			Sleep(1000);
			cout << "End locked region, thread " << n << '\n';
			omp_unset_lock(&lock);
		}

		omp_destroy_lock(&lock);
		return 0;
	}

	int run_test_set()
	{
		omp_init_lock(&lock);

#pragma omp parallel
		{
			int n = omp_get_thread_num();
			while (!omp_test_lock(&lock))
			{
				cout << "Lock was set, thread " << n << '\n';
				Sleep(500);
			}
			cout << "Start locked region, thread " << n << '\n';
			Sleep(1000);
			cout << "End locked region, thread " << n << '\n';
			omp_unset_lock(&lock);
		}

		omp_destroy_lock(&lock);
		return 0;
	}
}


int main()
{
	//CalculatePi::run_atomic();
	//CalculatePi::run_lock();
	//Ordered::run();
	//Locks::run_set();
	Locks::run_test_set();

	return 0;
}
