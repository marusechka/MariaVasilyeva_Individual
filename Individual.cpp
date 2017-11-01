#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <conio.h>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <chrono>
#include <ctime>

using namespace std::chrono;
using namespace std;

// интегрируемая функция
double func(double x)
{
	return (4 / (sqrt(4 - pow(x, 2))));
}

// функция параллельного вычисления интеграла методом прямоугольника
// a, b - пределы интегрирования
// m - число разбиений
// Fx - интегрируемая функция
double Parallel_Rect_Solve(double(*Fx)(double), double a, double b, int m)
{
	double h = (b - a) / m; // шаг разбиения
	cilk::reducer_opadd<double>Sum(0.0);
	cilk_for(int i = 0; i < m - 1; ++i)
	{
		Sum += Fx(a + i * h);
	}
	float result = Sum.get_value() * h;
	return result;
}

// функция последовательного вычисления интеграла методом прямоугольника
// a, b - пределы интегрирования
// m - число разбиений
// Fx - интегрируемая функция
double Serial_Rect_Solve(double(*Fx)(double), double a, double b, int m)
{
	double h = (b - a) / m; // шаг разбиения
	double Sum = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		Sum += Fx(a + i * h);
	}
	return Sum*h;
}


int main(int argc, char argv[])
{
	setlocale(LC_ALL, "Russian");
	float PI = 3.14159265358979323846;
	double I; // результат вычисление интеграла
	double a = 0, b = 1; // пределы интегрирования
	int m = 1000000; // число разбиений
	float chisl = (2 * PI) / 3;

	printf("\nЧисло разбиений  = %i\n", m);

	high_resolution_clock::time_point t1, t2;
	t1 = high_resolution_clock::now();

	I = Serial_Rect_Solve(&func, a, b, m);

	t2 = high_resolution_clock::now();
	duration<double> duration_serial = (t2 - t1);

	std::cout <<"\nВремя вычисления интеграла последовательно: " << duration_serial.count() << " секунд\n" << std::endl;
	printf("Значение интеграла, вычисленного последовательно = %.22f\n", I);

	t1 = high_resolution_clock::now();

	I = Parallel_Rect_Solve(&func, a, b, m);

	t2 = high_resolution_clock::now();
	duration<double> duration_parallel = (t2 - t1);

	std::cout <<"\nВремя вычисления интеграла параллельно: " << duration_parallel.count() << " се-кунд\n" << std::endl;
	printf("Значение интеграла, вычисленного     параллельно = %.22f\n", I);
	printf("\nОшибка вычисления = %.22f\n", chisl-I);
	printf("\nПроизводительность = %.4f\n", duration_serial.count() / duration_parallel.count());

	system("pause");
}
