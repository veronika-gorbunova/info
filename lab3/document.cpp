#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

time_t I = time (nullptr);	//Возвращает количество секунд с 00:00 01.01.1970

int a = 16807;
int c = 4;
int m = pow (2.0, 31) - 1;

int rand ()					//Линейный конгруэнтный метод получения ПСЧ
{
	return I = (a * I + c) % m;
}

void main()
{
	setlocale (LC_ALL, "Russian");

	cout << "Вывод некоторых полученных псевдослучайных чисел на экран" << endl;
	for (int i = 0; i < 10; i ++)
		cout << rand() << endl;	
	cout << endl;

	int N = 10;							//Число интервалов
	vector <int> Frequency (N, 0);		//Вектор, в каждом значении которого содержится число попаданий ПСЧ в каждый интервал
	int M = 1000;						//Число псевдослучайных чисел

	//Рассчитываем число попаданий в каждый интервал
	for (int i = 0; i < M; i ++)		
		Frequency[rand() / (m / N)] ++;	

	//Вывод числа на экран число попаданий ПСЧ в каждый интервал и ее процентное соотношение
	cout << "При M = " << M << endl;
	cout << "Интервал \t Число попаданий" << "\t\t %" << endl;		
	for (int i = 0; i < N; i ++)
		cout << i + 1 << "\t\t\t" << Frequency[i] << "\t\t\t" << 100.0 * Frequency[i] / M << endl;
	cout << endl;

	system ("pause");
}