#include <iostream>
#include <omp.h>

using namespace std;


namespace Version
{
	int run()
	{
#ifdef _OPENMP
		cout << "OpenMP supported!\n" << "Version: " << _OPENMP << '\n';
#endif // _OPENMP

		return 0;
	}
}

namespace Parallel
{
	int run()
	{
		cout << "Seqeuential region 1\n";

#pragma omp parallel num_threads(2)
		{
			cout << "Parallel region\n";
		}

		cout << "Sequential region 2\n";
		return 0;
	}
}

namespace Private
{
//	int run()
//	{
//		int n = 1;
//		cout << "In sequential region n = " << n << '\n';
//
//#pragma omp parallel private(n)  // in threads n is uninitalized, so this will not compile
//		{
//			cout << "In thread at enter n = " << n << '\n';
//			n = omp_get_thread_num();
//			cout << "In thread at exit n = " << n << '\n';
//		}
//
//		cout << "In sequetial region (exit) n = " << n << '\n';
//		return 0;
//	}

	int run2()
	{
		int n = 1;
		cout << "In sequential region n = " << n << '\n';

#pragma omp parallel private(n)  // this WILL compile
		{
			n = omp_get_thread_num();
			cout << "In thread at enter n = " << n << '\n';
		}

		cout << "In sequetial region (exit) n = " << n << '\n';
		return 0;
	}
}

namespace Shared
{
	int run()
	{
		int m[10] = { 0 };
		
		cout << "Array at start\n";
		for (int i = 0; i < 10; i++) {
			cout << m[i] << " ";
		}
		cout << '\n';

#pragma omp parallel shared(m)
		{
			m[omp_get_thread_num()] = 1;
		}

		cout << "Array at finish\n";
		for (int i = 0; i < 10; i++) {
			cout << m[i] << " ";
		}
		cout << '\n';
		return 0;
	}
}

namespace Firstprivate
{
	int run() {
		int n = 1;

		cout << "At start n = " << n << '\n';

#pragma omp parallel firstprivate(n)
		{
			cout << "In thread at enter n = " << n << '\n';
			n = omp_get_thread_num();
			cout << "In thread at exit n = " << n << '\n';
		}
		cout << "In sequetial region (exit) n = " << n << '\n';
		
		return 0;
	}
}

namespace Threadprivate
{
	int n;
#pragma omp threadprivate(n)
	int run() {
		int num;
		n = 1;

#pragma omp parallel private(num)
		{
			num = omp_get_thread_num();
			cout << "In thread at enter n = " << n << " num = " << num << '\n';
			n = omp_get_thread_num();
			cout << "In thread at enter n = " << n << " num = " << num << '\n';
		}

		cout << "Middle n = " << n << '\n';

#pragma omp parallel private(num)
		{
			num = omp_get_thread_num();
			cout << "In thread (again) n = " << n << " num = " << num << '\n';
		}

		return 0;
	}
}

namespace SetNumThreads
{
	int run()
	{
		omp_set_num_threads(5);

#pragma omp parallel num_threads(2)
		{
			cout << "Parallel region 1\n";
		}

#pragma omp parallel
		{
			cout << "Parallel region 2\n";
		}
	}
}


int main()
{
	Version::run();
	Parallel::run();
	//Private::run();
	Private::run2();
	Shared::run();
	Firstprivate::run();
	Threadprivate::run();
	SetNumThreads::run();

	return 0;
}