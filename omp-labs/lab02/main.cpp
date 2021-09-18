#include <iostream>
#include <omp.h>
#include <bitset>
#include <limits>

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

int main()
{
	//Reduction::bit_and();
	//Reduction::bit_or();
	//Reduction::bit_xor();
	//Reduction::bool_and();
	Reduction::bool_or();

	return 0;
}