#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

time_t I = time (nullptr);	//���������� ���������� ������ � 00:00 01.01.1970

int a = 16807;
int c = 4;
int m = pow (2.0, 31) - 1;

int rand ()					//�������� ������������ ����� ��������� ���
{
	return I = (a * I + c) % m;
}

void main()
{
	setlocale (LC_ALL, "Russian");

	cout << "����� ��������� ���������� ��������������� ����� �� �����" << endl;
	for (int i = 0; i < 10; i ++)
		cout << rand() << endl;	
	cout << endl;

	int N = 10;							//����� ����������
	vector <int> Frequency (N, 0);		//������, � ������ �������� �������� ���������� ����� ��������� ��� � ������ ��������
	int M = 1000;						//����� ��������������� �����

	//������������ ����� ��������� � ������ ��������
	for (int i = 0; i < M; i ++)		
		Frequency[rand() / (m / N)] ++;	

	//����� ����� �� ����� ����� ��������� ��� � ������ �������� � �� ���������� �����������
	cout << "��� M = " << M << endl;
	cout << "�������� \t ����� ���������" << "\t\t %" << endl;		
	for (int i = 0; i < N; i ++)
		cout << i + 1 << "\t\t\t" << Frequency[i] << "\t\t\t" << 100.0 * Frequency[i] / M << endl;
	cout << endl;

	system ("pause");
}