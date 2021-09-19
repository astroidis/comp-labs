#include <iostream>
#include <omp.h>
#include <bitset>
#include <limits>
#include <format>

using namespace std;


namespace Reduction
{
	int plus()
	{
		int count = 0;

#pragma omp parallel reduction(+ : count)
		/* count initialized with 0 in threads
		 * combiner: omp_out += omp_in
		 */
		{
			count++;
			cout << "In thread " << count << '\n';
		}

		cout << "Result: " << count << '\n';

		return 0;
	}


	int minus()
	{
		int count = 0;

#pragma omp parallel reduction(- : count)
		/* count initialized with 0 in threads
		 * combiner: omp_out += omp_in (looks like a typo but actually it is not, check specs)
		 */
		{
			count++;
			cout << "In thread " << count << '\n';
		}

		cout << "Result: " << count << '\n';

		return 0;
	}


	int multiply()
	{
		int count = 1;

#pragma omp parallel reduction(* : count)
		/* count initialized with 1 in threads
		 * combiner: omp_out *= omp_in
		 */
		{
			count++;
			cout << "In thread " << count << '\n';
		}

		cout << "Result: " << count << '\n';

		return 0;
	}


	int bit_and()
	{
		unsigned char count = 1;

#pragma omp parallel reduction(& : count)
		/* count initialized with ~0 in threads
		 * combiner: omp_out &= omp_in
		 */
		{
			cout << bitset<(sizeof(count) * 8)>(count) << '\n';
		}

		cout << "Result: " << bitset<(sizeof(count) * 8)>(count) << '\n';

		return 0;
	}


	int bit_or()
	{
		unsigned char count = 0b000111;

#pragma omp parallel reduction(| : count)
		/* count initialized with 0 in threads
		 * combiner: omp_out |= omp_in
		 */
		{
			count = 0b100110;
			cout << bitset<(sizeof(count) * 8)>(count) << '\n';
		}

		cout << "Result: " << bitset<(sizeof(count) * 8)>(count) << '\n';

		return 0;
	}


	int bit_xor()
	{
		unsigned char count = 0;

#pragma omp parallel reduction(^ : count)
		/* count initialized with 0 in threads
		 * combiner: omp_out ^= omp_in
		 */
		{
			count = 0b1100;
			cout << bitset<(sizeof(count) * 8)>(count) << '\n';
		}

		cout << "Result: " << bitset<(sizeof(count) * 8)>(count) << '\n';

		return 0;
	}


	int bool_and()
	{
		int count = 2;

#pragma omp parallel reduction(&& : count)
		/* count initialized with 1 in threads
		 * combiner: omp_in && omp_out
		 */
		{
			count = 1;
			cerr << "In thread " << count << '\n';
		}

		cout << "Result: " << count << '\n';

		return 0;
	}

	int bool_or()
	{
		int count = 3;
#pragma omp parallel reduction(|| : count)
		/* count initialized with 0 in threads
		 * combiner: omp_in || omp_out
		 */
		{
			count = 0;
			cerr << "In thread " << count << '\n';
		}

		cout << "Result: " << count << '\n';

		return 0;
	}
}


namespace Copyin
{
	int n;
#pragma omp threadprivate(n)

	int run()
	{
		n = 1;
#pragma omp parallel copyin(n)  // all threads access the master thread's value
		{
			n = 2;
			cout << "n = " << n << '\n';
		}
		cout << "Master n = " << n << '\n';

		return 0;
	}
}


namespace Dynamic
{
	int run()
	{
		cout << "OMP_DYNAMIC = " << omp_get_dynamic() << '\n';
		
		/* enables dynamic adjustment of the
		 * number of threads available for the execution
		 * to optimize resource usage
		 * only 4 threads will be created later
		 */
		omp_set_dynamic(1);
		cout << "OMP_DYNAMIC = " << omp_get_dynamic() << '\n';

#pragma omp parallel num_threads(28)
		{
#pragma omp master
			{
				cout << "Parallel region, number of threads - " << omp_get_num_threads() << '\n';
			}
		}
		return 0;
	}
}


namespace Nested
{
	int run()
	{
		int n;
		omp_set_nested(1);

#pragma omp parallel private(n)
		{
			n = omp_get_thread_num();

#pragma omp parallel
			{
				cout << format("Part 1, thread {} - {}\n", n, omp_get_thread_num());
			}
		}

		omp_set_nested(0);

#pragma omp parallel private(n)
		{
			n = omp_get_thread_num();

#pragma omp parallel
			{
				cout << format("Part 2, thread {} - {}\n", n, omp_get_thread_num());
			}
		}
	}

	int nest3()
	{
		int l1, l2;
		omp_set_nested(1);
		omp_set_num_threads(2);

#pragma omp parallel private(l1)
		{
			l1 = omp_get_thread_num();

#pragma omp parallel private(l2)
			{
				l2 = omp_get_thread_num();

#pragma omp parallel
				{
					cout << format("Level 1: {} Level 2: {} Level 3: {}\n", l1, l2, omp_get_thread_num());
				}
			}
		}
		return 0;
	}
}


namespace InParallel
{
	void mode(void)
	{
		if (omp_in_parallel())
		{
			cout << "Parallel region\n";
		}
		else
		{
			cout << "Serial region\n";
		}
	}

	int run()
	{
		mode();

#pragma omp parallel
		{
#pragma omp master
			{
				mode();
			}
		}
		return 0;
	}
}


namespace Single
{
	int run()
	{
#pragma omp parallel
		{
			cout << "Message 1\n";

			/* following code will be executed by only one of the threads
			 * other threads will wait at implicit barrier at the end of this block
			 * (so all "Message 1" will be printed before "Message 2")
			 * to disable this syncronisation used `nowait`
			 * (so output will be mixed up)
			 */
#pragma omp single nowait
			{
				cout << "Single thread\n";
			}
			cout << "Message 2\n";
		}
		return 0;
	}
}


namespace Copyprivate
{
	int run()
	{
		int n;

#pragma omp parallel private(n)
		{
			n = omp_get_thread_num();
			cout << format("Start n = {}\n", n);

#pragma omp single copyprivate(n)  // n will be shared among all threads
			{
				n = 100;
			}

			cout << format("End n = {}\n", n);
		}
		return 0;
	}
}


namespace Master
{
	int run()
	{
		int n;

#pragma omp parallel private(n)
		{
			n = 1;

#pragma omp master
			{
				n = 2;
			}

			cout << "First n = " << n << '\n';

#pragma omp barrier
#pragma omp master
			{
				n = 3;
			}
			cout << "Second n = " << n << '\n';
		}
		return 0;
	}
}


namespace MaxThreads
{
	int run()
	{
		cout << "Max number of threads " << omp_get_max_threads() << '\n';

		return 0;
	}
}


int main()
{
	//Reduction::plus();
	//Reduction::minus();
	//Reduction::multiply();
	//Reduction::bit_and();
	//Reduction::bit_or();
	//Reduction::bit_xor();
	//Reduction::bool_and();
	//Reduction::bool_or();
	//Copyin::run();
	//Dynamic::run();
	//Nested::run();
	Nested::nest3();
	//InParallel::run();
	//Single::run();
	//Copyprivate::run();
	//Master::run();
	//MaxThreads::run();
	
	return 0;
}