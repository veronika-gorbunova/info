#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

//���������� N-�� ��� 1-��������� ����� A, ������� � ������
bool GetBit (unsigned char A, int N)
{
	return (A >> 8 - N - 1) & 1u;
}

//��������� � �������� A ��������� ��� Bit � ������� N (� ������)
void ChangeBit (unsigned char & A, bool Bit, int N)
{
	if (Bit == 0) A &= ~(1 << 8 - N - 1);
	if (Bit == 1) A |=  (1 << 8 - N - 1);
}

//���������� ����� ����� �� ���� FileName; ��������� ������ ���������� � ������, ���� ���� � ��������� ������ �� ������
ifstream OpenFile (string FileName)
{
	ifstream File (FileName, ios::binary);
	if (!File.is_open())
	{
		cout << "���� �� ��� ������." << endl;
		system ("pause");
		exit(0);
	}
	return File;
}

//���������� ������ ������� A ������� Size, �������� �������� ����� Value (������������ ��� ������������)
int FindIndex (int A [], int Size, int Value)
{
	for (int i = 0; i < Size; i ++)
		if (A[i] == Value)
			return i;
	return -1;
}

//���������� ���������� ������� ���� � ����� ������� � ������� �� (������������ ��� ������������)
void DelZeroBytes (vector <unsigned char> & Text)
{
	int Size = Text.size();
	int ZeroBytes = 0;
	for (int i = Size - 1; i >= 0; i --)
		if (Text[i] == 0) 
			ZeroBytes ++;
	Text.erase(Text.begin() + Size - ZeroBytes, Text.begin() + Size);
}

//���������� ���������� �������� � ������
int LengthFile (string FileName)
{
	ifstream file (FileName, ios::binary | ios::ate);
	int Length = file.tellg();
	file.close();
	return Length;
}

//���������� ����� �� ����� FileName � ��� ������ Size
vector <unsigned char> GetTextInFile (string FileName, int & Size)
{
	ifstream File = OpenFile (FileName);
	vector <unsigned char> Text (Size, 0);
	
	//�������, ��������� �� �����, ����������� � ������
	char C;
	for (int i = 0; File.get(C); i ++)
		Text[i] = unsigned char (C);

	File.close();
	return Text;
}

//������� �������� ������� Vector ���������� ���� Type � ����� ������ Stream 
//(������������ ��� ������ ������� ����� � ����� � ���� ��� �� �����)
template <class Type>
ostream & operator << (ostream & Stream, vector <Type> Vector)
{
	int Size = Vector.size();
	for (int i = 0; i < Size; i ++)
		if (Vector[i] != 13 || Vector[i + 1] != 10)
			Stream << Vector [i];
	return Stream;
}

//����������/������������ ������ ����� InputFile ������ Key ������� Size = 4. ��������� ���������� �������� � ���� OutputFile.
//���� Mod = Coding - ����������� ����������, 
//���� Mod = Decode - ����������� ������������
void CodingFile (string InputFile, string OutputFile, int Key [], string Mode)
{
	int Size = 4;
	ofstream Output (OutputFile, ios::trunc);					//��������� ���� ��� ������ � ���� �������������� ������, ios::trunc ������� ���������� �����, ���� �� �� ����

	int Length = LengthFile (InputFile);						//������� ������ ��������� ����� � ������
	int NZeros = Size - Length % Size;							//���������� ����� ������� ����, ����������� � ������ ����� ��� ������������ �����
	if (Length % Size == 0) NZeros = 0;
	Length += NZeros;
	vector <unsigned char> Text = GetTextInFile (InputFile, Length);	//��������� ����� �� �������� �����

	//cout << Text << endl;										//������� �������� ����� �� �����

	/*for (int i = 0; i < Length; i ++)							//������� ���� �� �����
		for (int j = 0; j < 8; j ++)
			cout << bool(GetBit(Text[i], j));
	cout << endl;*/

	vector <unsigned char> OutputText (Length, 0);

	if (Mode == "Coding")										//���������� �����
		for (int i = 0; i < Length / Size; i ++)				//���������� �����
			for (int j = 0; j < Size * 8; j ++)
				ChangeBit(OutputText[Size * i + j / 8], GetBit(Text[Size * i + Key[j] / 8], Key[j] % 8), j % 8);
	
	if (Mode == "Decode")										//������������ �����
	{
		for (int i = 0; i < Length / Size; i ++)				//������������ �����
			for (int j = 0; j < Size * 8; j ++)
				ChangeBit(OutputText[Size * i + j / 8], GetBit(Text[Size * i + FindIndex(Key, 32, j) / 8], FindIndex(Key, 32, j) % 8), j % 8);
		DelZeroBytes(OutputText);
	}

	//����� ����������� ������ ����� ���������� / ������������ ����� � ����
	Output << OutputText;	
	Output.close();
}


//��������� ��������� ������ �� �����������
bool CompareFiles (string FileName1, string FileName2)
{
	ifstream File1 = OpenFile (FileName1);
	ifstream File2 = OpenFile (FileName2);

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
	int Key [N] = {12, 13, 30, 5, 27, 6, 11, 25, 3, 21, 22, 2, 23, 0, 8, 4, 18, 19, 10, 1, 14, 29, 9, 28, 20, 17, 26, 31, 7, 16, 15, 24};	//��������������� ����

	cout << "�������� ��������� ����:" << endl;
	CodingFile ("Input.txt", "Encrypt.txt", Key, "Coding");				//���������� ������

	cout << endl << "������������� ��������� ����:" << endl;
	CodingFile ("Encrypt.txt", "Decrypt.txt", Key, "Decode");			//������������� ������

	bool Compare = CompareFiles("Input.txt", "Decrypt.txt");			//�������� ��������� �������� � ��������������� ������
	if (Compare) cout << endl << "�������� � ������������� ����� ���������." << endl;
	else		 cout << endl << "�������� � ������������� ����� ��������."  << endl;

	system ("pause");
}