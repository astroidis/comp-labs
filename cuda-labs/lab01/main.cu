#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <locale.h>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;


void device_info()
{
	cudaDeviceProp prop;
	setlocale(LC_ALL, "RUS");

	int count;
	cudaGetDeviceCount(&count);

	for (int i = 0; i < count; i++)
	{
		cudaGetDeviceProperties(&prop, i);

		cout << "--- Общая информация об устройстве " << i << '\n';
		cout << "Имя: " << prop.name << '\n';
		cout << "Вычислительные возможности: " << prop.major << '.' << prop.minor << '\n';
		cout << "Тактовая частота: " << prop.clockRate << '\n';
		cout << "Перекрытие копирования: ";
		if (prop.deviceOverlap)
			cout << "разрешено\n";
		else
			cout << "запрещено\n";

		cout << "Тайм-аут выполнения ядра: ";
		if (prop.kernelExecTimeoutEnabled)
			cout << "включен\n";
		else
			cout << "выключен\n";

		cout << "--- Информация о памяти для устройства " << i << '\n';
		cout << "Всего глобальной памяти: " << prop.totalGlobalMem << '\n';
		cout << "Всего константной памяти: " << prop.totalConstMem << '\n';
		cout << "Максимальный шаг: " << prop.memPitch << '\n';
		cout << "Выравнивание текстур: " << prop.textureAlignment << '\n';

		cout << "--- Инфо о мультипроцессорах для уст-ва " << i << '\n';
		cout << "Кол-во мультипроцессоров: " << prop.multiProcessorCount << '\n';
		cout << "Разделяемая память на один МП: " << prop.sharedMemPerBlock << '\n';
		cout << "Количество регистров на один МП: " << prop.regsPerBlock << '\n';
		cout << "Количество нитей в варпе: " << prop.warpSize << '\n';
		cout << "Макс.кол-во нитей в блоке: " << prop.maxThreadsPerBlock << '\n';
		cout << "Макс.кол-во нитей по измерениям: (" << prop.maxThreadsDim[0] << ", " << prop.maxThreadsDim[1] << ", " << prop.maxThreadsDim[2] << ")\n";
		cout << "Максимальные размеры сетки: (" << prop.maxGridSize[0] << ", " << prop.maxGridSize[1] << ", " << prop.maxGridSize[2] << ")\n";
	}
}

namespace AddNumbers
{
	__global__ void add(int a, int b, int *c)
	{
		*c = a + b;
	}

	void run()
	{
		int c;
		int *dev_c;
		cudaMalloc((void**)&dev_c, sizeof(int));
		add<<< 1, 1 >>>(2, 7, dev_c);
		cudaMemcpy(&c, dev_c, sizeof(int), cudaMemcpyDeviceToHost);
		cout << "2 + 7 = " << c << '\n';
		cudaFree(dev_c);
	}
}

namespace AddVectors
{
	const int N = 10;

	__global__ void add_block(int *a, int *b, int *c)
	{
		int tid = blockIdx.x;
		c[tid] = a[tid] + b[tid];
		printf("Block number %d write %d in array on %d position\n", tid, c[tid], tid);
	}

	void run_block()
	{
		int a[N], b[N], c[N];
		int *dev_a, *dev_b, *dev_c;

		cudaMalloc((void**)&dev_a, N * sizeof(int));
		cudaMalloc((void**)&dev_b, N * sizeof(int));
		cudaMalloc((void**)&dev_c, N * sizeof(int));

		for (int i = 0; i < N; i++)
		{
			a[i] = i;
			b[i] = i * i;
		}

		cudaMemcpy(dev_a, a, N * sizeof(int), cudaMemcpyHostToDevice);
		cudaMemcpy(dev_b, b, N * sizeof(int), cudaMemcpyHostToDevice);

		add_block<<< N, 1 >>> (dev_a, dev_b, dev_c);

		cudaMemcpy(c, dev_c, N * sizeof(int), cudaMemcpyDeviceToHost);

		for (int i = 0; i < N; i++)
			cout << a[i] << " + " << b[i] << " = " << c[i] << '\n';

		cudaFree(dev_a);
		cudaFree(dev_b);
		cudaFree(dev_c);
	}


	__global__ void add_thread(int *a, int *b, int *c)
	{
		int tid = threadIdx.x;
		c[tid] = a[tid] + b[tid];
		printf("Thread number %d write %d in array on %d position\n", tid, c[tid], tid);
	}

	void run_thread()
	{
		int a[N], b[N], c[N];
		int *dev_a, *dev_b, *dev_c;

		cudaMalloc((void**)&dev_a, N * sizeof(int));
		cudaMalloc((void**)&dev_b, N * sizeof(int));
		cudaMalloc((void**)&dev_c, N * sizeof(int));

		for (int i = 0; i < N; i++)
		{
			a[i] = i;
			b[i] = i * i;
		}

		cudaMemcpy(dev_a, a, N * sizeof(int), cudaMemcpyHostToDevice);
		cudaMemcpy(dev_b, b, N * sizeof(int), cudaMemcpyHostToDevice);
		add_thread<<< 1, N >>> (dev_a, dev_b, dev_c);

		cudaMemcpy(c, dev_c, N * sizeof(int), cudaMemcpyDeviceToHost);

		for (int i = 0; i < N; i++)
			cout << a[i] << " + " << b[i] << " = " << c[i] << '\n';

		cudaFree(dev_a);
		cudaFree(dev_b);
		cudaFree(dev_c);
	}
}


namespace CudaPi
{
	//вычислить число pi, методом интегрирование четверти окружности радиуса 1
	using CUDA_FLOAT = float;
	const int GRID_SIZE = 256;
	const int BLOCK_SIZE = 256;

	__global__ void pi_kern(CUDA_FLOAT *res)
	{
		int n = threadIdx.x + blockIdx.x * BLOCK_SIZE;
		CUDA_FLOAT x0 = n * 1.f / (BLOCK_SIZE * GRID_SIZE);
		CUDA_FLOAT y0 = sqrtf(1 - x0 * x0);
		CUDA_FLOAT dx = 1.f / (1.f * BLOCK_SIZE * GRID_SIZE);
		CUDA_FLOAT s = 0;
		CUDA_FLOAT x1, y1;
		
		x1 = x0 + dx;
		y1 = sqrtf(1 - x1 * x1);
		s = (y0 + y1) * dx / 2.f;
		res[n] = s;
	}

	int run() 
	{
		CUDA_FLOAT pi = 0;
		CUDA_FLOAT *res_d;
		CUDA_FLOAT res[GRID_SIZE * BLOCK_SIZE];

		cudaMalloc((void**)&res_d, sizeof(CUDA_FLOAT)*GRID_SIZE * BLOCK_SIZE);

		dim3 grid(GRID_SIZE);
		dim3 block(BLOCK_SIZE);

		pi_kern<<< grid, block >>>(res_d);

		cudaThreadSynchronize();

		cudaMemcpy(&res, res_d, sizeof(CUDA_FLOAT) * GRID_SIZE * BLOCK_SIZE, cudaMemcpyDeviceToHost); 
		cudaFree(res_d);

		for (int i = 0; i < GRID_SIZE * BLOCK_SIZE; i++)
			pi += res[i];
		
		pi *= 4;
		cout << "PI = " << pi <<'\n';

		return 0;
	}
}

namespace PowerOfK
{
	const int K = 3;
	const int SIZE = 32;

	__global__ void pow(long long powers[SIZE], int base)
	{
		int id = threadIdx.x;
		for (int i = 0; i < id; i++)
			powers[id] *= base;
	}

	void run()
	{
		long long h_powers[SIZE];
		long long *d_powers;

		cudaMalloc(reinterpret_cast<void **>(&d_powers), SIZE * sizeof(long long));
		
		for (int i = 0; i < SIZE; i++) {
			h_powers[i] = 1;
		}

		cudaMemcpy(d_powers, h_powers, SIZE * sizeof(long long), cudaMemcpyHostToDevice);

		pow<<<1, SIZE>>>(d_powers, K);

		cudaMemcpy(h_powers, d_powers, SIZE * sizeof(long long), cudaMemcpyDeviceToHost);
		
		for (int i = 0; i < SIZE; i++) {
			cout << "Power " << i << ": " << h_powers[i] << '\n';
		}

		cudaFree(d_powers);
	}
}


namespace RowSum
{
	const int SIZE = 5;

	__global__ void row_sum(int *matrix, size_t pitch, int result[SIZE])
	{
		int *row = (int *)((char *)matrix + threadIdx.x * pitch);
		for (int i = 0; i < SIZE; i++) {
			result[threadIdx.x] += row[i];
		}
	}

	void run()
	{
		int h_matrix[SIZE][SIZE] =
		{
			{ 1, 2, 3, 4, 5 },
			{ 6, 7, 8, 9, 0 },
			{ 1, 2, 3, 4, 5 },
			{ 6, 7, 8, 9, 0 },
			{ 1, 2, 3, 4, 5 }
		},
			h_result[SIZE] = { 0 }; 
		int *d_result, *d_matrix;
		size_t pitch;

		cudaMalloc(&d_result, SIZE * sizeof(int));
		cudaMemcpy(d_result, h_result, SIZE * sizeof(int), cudaMemcpyHostToDevice);

		cudaMallocPitch(&d_matrix, &pitch, SIZE * sizeof(int), SIZE);
		cudaMemcpy2D(d_matrix, pitch, h_matrix, SIZE * sizeof(int), SIZE * sizeof(int), SIZE, cudaMemcpyHostToDevice);

		row_sum <<<1, SIZE>>> (d_matrix, pitch, d_result);

		cudaMemcpy(h_result, d_result, SIZE * sizeof(int), cudaMemcpyDeviceToHost);

		for (int i = 0; i < SIZE; i++) {
			cout << "Sum of " << i << " row: " << h_result[i] << '\n';
		}

		cudaFree(d_result);
		cudaFree(d_matrix);
	}
}


namespace MultiplyMatrix
{
	const int SIZE = 5;

	__global__ void row_sum(int *matrix, size_t pitch, int factor)
	{
		int *item = (int *)((char *)matrix + threadIdx.x * pitch) + threadIdx.y;
		*item *= factor;
	}

	void run()
	{
		int factor = 5;
		int h_matrix[SIZE][SIZE] =
		{
			{ 1, 2, 3, 4, 5 },
			{ 6, 7, 8, 9, 0 },
			{ 1, 2, 3, 4, 5 },
			{ 6, 7, 8, 9, 0 },
			{ 1, 2, 3, 4, 5 }
		};
		int *d_matrix;
		size_t pitch;

		cudaMallocPitch(&d_matrix, &pitch, SIZE * sizeof(int), SIZE);
		cudaMemcpy2D(d_matrix, pitch, h_matrix, SIZE * sizeof(int), SIZE * sizeof(int), SIZE, cudaMemcpyHostToDevice);

		dim3 threads(SIZE, SIZE);
		row_sum <<<1, threads>>> (d_matrix, pitch, factor);

		cudaMemcpy2D(h_matrix, SIZE * sizeof(int), d_matrix, pitch, SIZE * sizeof(int), SIZE, cudaMemcpyDeviceToHost);

		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++)
			{
				cout << setw(5) << h_matrix[i][j];
			}
			cout << '\n';
		}

		cudaFree(d_matrix);
	}
}


int main()
{
	//device_info();
	//AddNumbers::run();
	//AddVectors::run_block();
	//AddVectors::run_thread();
	//CudaPi::run();
	//PowerOfK::run();
	//RowSum::run();
	MultiplyMatrix::run();

	return 0;
}