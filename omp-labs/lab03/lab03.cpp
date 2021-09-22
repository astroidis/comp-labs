#include <omp.h>
#include <iostream>
#include <windows.h>

//int main() {
//	system("chcp 1251 > nul");
//
//	int A[10], B[10], C[10], i, n;
//	for (i = 0; i < 10; i++) {
//		A[i] = i; 
//		B[i] = 2 * i; 
//		C[i] = 0;
//	}
//
//#pragma omp parallel shared(A, B, C) private(i, n)
//	{
//		n = omp_get_thread_num();
//#pragma omp for schedule(static)
//		for (i = 0; i < 10; i++) {
//			C[i] = A[i] + B[i];
//			printf("Нить %d сложила элементы с номером %d\n", n, i);
//		}
//	}
//	return 0;
//}

//int main()
//{
//	system("chcp 1251>nul");
//
//#pragma omp parallel
//	{
//#pragma omp for schedule (guided, 3)
//		for (int i = 0; i < 20; i++)
//		{
//			printf("Нить %d выполнила итерацию %d\n", omp_get_thread_num(), i);
//			Sleep(1);
//		}
//	}
//	return 0;
//}

//#include <fstream>

//int main()
//{
//	system("chcp 1251>nul");
//	std::ofstream file;
//	file.open("guided_6.txt");
//
//#pragma omp parallel
//	{
//#pragma omp for schedule (guided, 6)
//		for (int i = 0; i < 100; i++)
//		{
//#pragma omp critical
//			{
//				file << omp_get_thread_num() << "," << i << '\n';
//			}
//			Sleep(1);
//		}
//	}
//	return 0;
//}

int main() 
{
	system("chcp 1251>nul");

	int n;
#pragma omp parallel private(n)
	{
		n = omp_get_thread_num();
#pragma omp sections
	{
#pragma omp section
		{
			printf("Первая секция, процесс %d\n", n);
		}
#pragma omp section
		{
			printf("Вторая секция, процесс %d\n", n);
		}
#pragma omp section
		{
			printf("Третья секция, процесс %d\n", n);
		}
	}
	printf("Параллельная область, процесс %d\n", n);
	}
	return 0;
}