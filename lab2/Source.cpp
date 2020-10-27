#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

//�������� 16-�� ������� ���������
char Hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

//���������� ����� ����� �� ���� FileName; ��������� ������ ���������� � ������, ���� ���� � ��������� ������ �� ������
ifstream OpenFile(string FileName)
{
	ifstream File(FileName, ios::binary);
	if (!File.is_open())
	{
		cout << "���� �� ��� ������." << endl;
		system("pause");
		exit(0);
	}
	return File;
}

//���������� ������ ������� A ������� Size, �������� �������� ����� Value (������������ ��� ������������)
template <class Type>
int FindIndex(Type A[], int Size, Type Value)
{
	for (int i = 0; i < Size; i++)
		if (A[i] == Value)
			return i;
	return -1;
}

//���������� ���������� ������� ���� � ����� ������� (������������ ��� ������������)
int NZeroBytes(vector <bool> Bits)
{
	int Size = Bits.size() / 4;
	int ZeroBytes = 0;
	for (int i = Size - 1; i >= 0; i--)
	{
		int ZeroBits = 0;
		for (int j = 0; j < 4; j++)
			if (Bits[4 * i + j] == 0)
				ZeroBits++;
		if (ZeroBits == 4) ZeroBytes++;
		else break;
	}
	return ZeroBytes;
}

//���������� ���������� �������� � ����� FileName
int LengthFile(string FileName)
{
	ifstream file(FileName, ios::binary | ios::ate);
	int Length = file.tellg();
	file.close();
	return Length;
}

//���������� ����� �� ����� FileName ������� Size
vector <char> GetTextInFile(string FileName, int Size)
{
	ifstream File = OpenFile(FileName);
	vector <char> Text(Size);

	//�������, ��������� �� �����, ����������� � ������
	for (int i = 0; i < Size; i++)
		File.get(Text[i]);

	File.close();
	return Text;
}

//���������� ������ ����� ������ Text. ��������� ZeroBytes ������� �����
vector <bool> TextToBitsArray(vector <char> Text, int ZeroBits)
{
	int Size = Text.size();						//������� ���������� �������� � �������� ������
	vector <bool> Bits(Size * 4 + ZeroBits);	//�������������� ������ �����

	for (int i = 0; i < Size; i++)				//�������� ���� ��� ������� ������� ������ � ������� �� � ������
	{
		int C = FindIndex(Hex, 16, Text[i]);	//��������� ����� �� 16-�� � 10-�� ������� ���������
		for (int j = 4; j > 0; j--)			//��������� ����� � 2-�� ��, �������� ������� �� ������� �� 2
		{
			Bits[4 * i + j - 1] = C % 2;
			C /= 2;
		}
	}

	//������� ����, ����������� � ������ ��� ������������ �����:
	for (int i = 4 * Size; i < 4 * Size + ZeroBits; i++)
		Bits[i] = 0;

	return Bits;
}

//������� �������� ������� Vector ���������� ���� Type � ����� ������ Stream (������������ ��� ������ ������� ����� � ����� � ���� ��� �� �����)
template <class Type>
ostream & operator << (ostream & Stream, vector <Type> Vector)
{
	int Size = Vector.size();
	for (int i = 0; i < Size; i++)
		Stream << Vector[i];
	return Stream;
}

//��������� ������ ����� Bits ������� Size ���� ������� � �����, �������� ������ 4 ���� � ��������� ������
vector <char> BitsArrayToText(vector <bool> Bits, int Size)
{
	vector <char> Text(Size, 0);

	for (int i = 0; i < Size; i++)
	{
		for (int j = 0; j < 4; j++)	//��������� ����� �� 2-�� ������� ��������� � 10-��
			Text[i] += Bits[4 * i + j] * pow(2.0, 3 - j);
		Text[i] = Hex[Text[i]];			//��������� ����� �� 10-�� �� � 16 ��
	}

	return Text;
}

//����������/������������ ������ ����� InputFile ������ Key ������� Size. ��������� ���������� �������� � ���� OutputFile.
//���� Mod = Coding - ����������� ����������, 
//���� Mod = Decode - ����������� ������������
void CodingFile(string InputFile, string OutputFile, int Key[], int Size, string Mode)
{
	ofstream Output(OutputFile, ios::trunc);					//��������� ���� ��� ������ � ���� �������������� ������, ios::trunc ������� ���������� �����, ���� �� �� ����

	int Length = LengthFile(InputFile);						//���������� ���������� �������� � �������� �����
	int NZeros = 4 * (Size - Length % Size);					//���������� ����� ������� ���, ����������� � ������ ����� ��� ������������ �����
	if (Length % Size == 0) NZeros = 0;
	vector <char> Text = GetTextInFile(InputFile, Length);		//��������� ����� �� �������� �����
	vector <bool> Bits = TextToBitsArray(Text, NZeros);		//�������� ������ ����� ��������� �����

	cout << Text << endl;										//������� �������� ����� �� �����
	cout << Bits << endl;										//������� ������ ����� �� �����

	if (Mode == "Coding")										//���������� �����
	{
		vector <bool> Encrypt(4 * Length + NZeros);					//������ ����� �������������� �����
		for (int i = 0; i < (Length * 4 + NZeros) / (8 * Size); i++)	//���������� �����
			for (int j = 0; j < Size * 8; j++)
				Encrypt[Size * 8 * i + j] = Bits[Size * 8 * i + Key[j]];
		Output << BitsArrayToText(Encrypt, (Length * 4 + NZeros) / 4);	//����� ����������� ������ ����� ���������� ����� � ����
	}

	if (Mode == "Decode")										//������������ �����
	{
		vector <bool> Decrypt(4 * Length);						//������ ����� �������������� �����
		for (int i = 0; i < Length / (2 * Size); i++)			//������������ �����
			for (int j = 0; j < Size * 8; j++)
				Decrypt[Size * 8 * i + j] = Bits[Size * 8 * i + FindIndex(Key, 8 * Size, j)];

		//����� ����������� ������ ����� ������������ ����� � ����, ���������� ������� ����� � ����� �������
		Output << BitsArrayToText(Decrypt, Length - 1 * NZeroBytes(Decrypt));
	}

	Output.close();
}

//��������� ��������� ������ �� �����������
bool CompareFiles(string FileName1, string FileName2)
{
	ifstream File1 = OpenFile(FileName1);
	ifstream File2 = OpenFile(FileName2);

	char ch1, ch2;
	bool Result = true;
	while (File1.get(ch1) && File2.get(ch2))
	{
		if (ch1 != ch2)
		{
			Result = false;
			break;
		}
	}

	File1.close();
	File2.close();
	return Result;
}

/*
	  Input.txt - �������� ��������� ����
	Encrypt.txt - ����������� ��������� ����
	Decrypt.txt - ������������� ��������� ����
*/

void main()
{
	setlocale(LC_ALL, "Russian");

	const int N = 32;													//����� �������� � ��������������� �����
	int Key[N] = { 12, 13, 30, 5, 27, 6, 11, 25, 3, 21, 22, 2, 23, 0, 8, 4, 18, 19, 10, 1, 14, 29, 9, 28, 20, 17, 26, 31, 7, 16, 15, 24 };//��������������� ����

	cout << "�������� ��������� ����:" << endl;
	CodingFile("Input.txt", "Encrypt.txt", Key, N / 8, "Coding");		//���������� ������

	cout << endl << "������������� ��������� ����:" << endl;
	CodingFile("Encrypt.txt", "Decrypt.txt", Key, N / 8, "Decode");	//������������� ������

	bool Compare = CompareFiles("Input.txt", "Decrypt.txt");			//�������� ��������� �������� � ��������������� ������
	if (Compare) cout << endl << "�������� � ������������� ����� ���������." << endl;
	else		 cout << endl << "�������� � ������������� ����� ��������." << endl;

	system("pause");
}