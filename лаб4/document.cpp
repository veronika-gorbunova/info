#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include <iostream>
#include <sstream>
#include <fstream>
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

//���������� ���������� �������� � ������
int LengthFile (string FileName)
{
	ifstream file (FileName, ios::binary | ios::ate);
	int Length = file.tellg();
	file.close();
	return Length;
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

//�������� ����� � ����
void OutputTextInFile (vector <unsigned char> Text, string FileName)
{
	ofstream File (FileName);

	if (!File.is_open())
	{
		cout << "���� �� ��� ������." << endl;
		system ("pause");
		exit(0);
	}

	//�������, ��������� �� �����, ����������� � ������
	for (int i = 0; i < Text.size(); i ++)
		File << Text[i];

	File.close();
}

//�������� ��������� ��������� �� ����� SecretText (txt) � ���� PictureFileName (bmp)
void PutCryptMessage (string SecretText, LPTSTR PictureFileName)
{
	int Size = LengthFile (SecretText);
	vector <unsigned char> Crypt = GetTextInFile(SecretText, Size);

	//������� ������ ���������
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	unsigned char Palette [1024];
	unsigned long RW;

	//������� ���� ��� ������
	HANDLE InputFile = CreateFile (PictureFileName,  GENERIC_READ,   0, NULL, OPEN_EXISTING, 0, NULL);
	if (InputFile == INVALID_HANDLE_VALUE)
		return;

	//�������� ��������� � �������
	ReadFile (InputFile, &bfh, sizeof(bfh), &RW, NULL);
	ReadFile (InputFile, &bih, sizeof(bih), &RW, NULL);
	ReadFile (InputFile, Palette, sizeof(Palette), &RW, NULL);

	int Height = bih.biHeight;
	int Width  = bih.biWidth;
	
	//������� ����� ��� ���������� ������ �����
	vector <RGBTRIPLE> Points (Height * Width);

	//�������� ����� � ������
	for (int i = 0; i < Height; i ++)
	{
		for (int j = 0; j < Width; j ++)
		{
			ReadFile (InputFile, &Points[Height * i + j], sizeof(Points[Height * i + j]), &RW, NULL);
		}
		//��������� ������������ ������
		ReadFile  (InputFile,  Palette, (sizeof(RGBTRIPLE) * Width) % 4, &RW, NULL);
	}

	//������� ����������
	CloseHandle (InputFile);

	//������� ��������� ���� ������� �������� � ������
	for (int i = 0; i < Crypt.size(); i ++)
	{
		for (int j = 0; j < 4; j ++)
		{
			ChangeBit (Points[4 * i + j].rgbtBlue,  GetBit(Crypt[i], 2 * j), 6);
			ChangeBit (Points[4 * i + j].rgbtBlue,  GetBit(Crypt[i], 2 * j + 1), 7);
		}
	}

	//������� ������ ���� ��� ������
	HANDLE OutputFile = CreateFile (PictureFileName, GENERIC_WRITE,  0, NULL, CREATE_ALWAYS, 0, NULL);
	if (OutputFile == INVALID_HANDLE_VALUE)
		return;

	//������� ��������� � ������� � ����-������
	WriteFile (OutputFile, &bfh, sizeof(bfh), &RW, NULL);
	WriteFile (OutputFile, &bih, sizeof(bih), &RW, NULL);
	WriteFile (OutputFile, Palette, sizeof(Palette), &RW, NULL);

	//�������� ����� � ���� ������
	for (int i = 0; i < Height; i ++)
	{
		for (int j = 0; j < Width; j ++)
		{
			WriteFile (OutputFile, &Points[Height * i + j], sizeof(Points[Height * i + j]), &RW, NULL);
		}
		//��������� ������������ ������
		WriteFile (OutputFile, Palette, (sizeof(RGBTRIPLE) * Width) % 4, &RW, NULL);
	}

	//������� ����������
	CloseHandle (OutputFile);

	cout << "��������� ���������� ���� ������� ��������.\n";
}

//"�������" ��������� ��������� � ���������� ���
void TakeCryptMessage (LPTSTR InputFileName, string Output)
{
	//������� ������ ���������
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	unsigned char Palette [1024];
	unsigned long RW;

	//������� ���� ��� ������
	HANDLE InputFile = CreateFile (InputFileName,  GENERIC_READ,   0, NULL, OPEN_EXISTING, 0, NULL);
	if (InputFile == INVALID_HANDLE_VALUE)
		return;

	//�������� ��������� � �������
	ReadFile (InputFile, &bfh, sizeof(bfh), &RW, NULL);
	ReadFile (InputFile, &bih, sizeof(bih), &RW, NULL);
	ReadFile (InputFile, Palette, sizeof(Palette), &RW, NULL);

	int Height = bih.biHeight;
	int Width  = bih.biWidth;
	
	//������� ����� ��� ���������� ������ �����
	vector <RGBTRIPLE> Points (Height * Width);

	//�������� ����� � ������
	for (int i = 0; i < Height; i ++)
	{
		for (int j = 0; j < Width; j ++)
		{
			ReadFile (InputFile, &Points[Height * i + j], sizeof(Points[Height * i + j]), &RW, NULL);
		}
		// ��������� ������������ ������
		ReadFile  (InputFile,  Palette, (sizeof(RGBTRIPLE) * Width) % 4, &RW, NULL);
	}

	//������� ����������
	CloseHandle (InputFile);

	vector <unsigned char> Text (Height * Width / 4);

	//������� ��������� ���� ������� �������� � ������
	for (int i = 0; i < Height * Width / 4; i ++)
		for (int j = 0; j < 4; j ++)
		{
			ChangeBit (Text[i], GetBit(Points[4 * i + j].rgbtBlue, 6), 2 * j);
			ChangeBit (Text[i], GetBit(Points[4 * i + j].rgbtBlue, 7), 2 * j + 1);
		}

	OutputTextInFile (Text, Output);

		cout << "��������� ���������� ���� ������� ���������.\n";
}

int _tmain (int argc, char * argv[])
{
	setlocale (LC_ALL, "Russian");

	/*
	*	argv[0] - ������������ ���������
	*	argv[1] - ��� �������� (-embed (���������), -retrieve (����������), -check (��������))
	*	argv[2] - ������ ����
	*	argv[3] - ������ ����
	*/

	//argv[2] - "Secret.txt", argv[3] - "Picture.bmp".
	if (string(argv[1]) == "-embed")   
		PutCryptMessage  (argv[2], argv[3]);

	//argv[2] - "Picture.bmp", argv[3] - "Secret.txt".
	if (string(argv[1]) == "-retrieve")
		TakeCryptMessage (argv[2], argv[3]);

	system ("pause");

	return 0;
}
