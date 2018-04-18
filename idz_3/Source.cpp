#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <stdint.h>
#include <intrin.h>
#include <vector>
#include <conio.h>


const int signLength = 9;

using namespace std;

string bytesToStr(char *str)
{
	stringstream newStr;
	newStr << hex << _byteswap_ulong(*((unsigned int*)str));

	return newStr.str();
}

struct  chunk
{
	int position;
	int length;
	string CRC = "";
};


enum Check { YES, NO, MATCH };



Check check_chunk(string& str)
{
	string IDAT = { "IDAT" };
	if (str == IDAT)
	{
		return MATCH;
	}

	for (int i(0); i < str.length(); i++)
	{
		if (str[i] != IDAT[i])
		{
			str.clear();
			return NO;
		}
	}
	return YES;

}

int main(int argc, char *argv[])
{

	if (argc != 2) { 
		cout << "Usage: idz_3.exe <filename>" << endl;
		return 1;
	}

	ifstream png_stream(argv[1], ios::binary | ios::in);

	////////////////////////////////////////////////////////////////////////////////////////

	if (!png_stream.is_open())
	{
		cout << "Error open" << endl;
		return 1;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	unsigned char sign[signLength];
	memset((char*)sign, 0, signLength);
	png_stream.read((char*)sign, signLength - 1);
	if (strcmp((char*)sign, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A") == 0)
	{
		cout << "Signature is true" << endl;
	}
	else
	{
		cout << "Signature is false" << endl;
		return 2;
	}

	////////////////////////////////////////////////////////////////////////////////////////

	unsigned char IHDR_lenght[5];
	char IHDR_Name[5] = { 0 };
	char IHDR_CRC[5] = { 0 };


	png_stream.read((char*)IHDR_lenght, 4);
	int iHDRLength = _byteswap_ulong(*((unsigned int*)IHDR_lenght));
	png_stream.read((char*)IHDR_Name, 4);


	if (strcmp((char*)IHDR_Name, "IHDR") == 1)
	{
		cout << "IHDR Error!" << endl;
		return 3;
	}

	png_stream.seekg(iHDRLength, ios::cur);
	png_stream.read((char*)IHDR_CRC, 4);

	cout << "Length  IHDR :  " << iHDRLength << "   CRC: 0x" << _byteswap_ulong(*((unsigned int*)IHDR_CRC)) << endl << endl;

	////////////////////////////////////////////////////////////////////////////////////////

	png_stream.seekg(-12, ios::end);

	unsigned char iend_sign[13];
	memset((char*)iend_sign, 0, 13);
	png_stream.read((char*)iend_sign, 12);
	if (strcmp((char*)iend_sign, "\x00\x00\x00\x00\x49\x45\x4E\x44\xAE\x42\x60\x82") == 0)
	{
		cout << "IEND_Signature is true" << endl;
	}
	else
	{
		cout << "IEND_Signature Error" << endl;
		return 4;
	}

	////////////////////////////////////////////////////////////////////////////////////////

	png_stream.seekg(8, ios::beg);

	vector <chunk> chunks_IDAT;
	string str_temp;
	chunk temp_chunk;

	while (!png_stream.eof())
	{
		str_temp += png_stream.get();
		switch (check_chunk(str_temp))
		{
		case NO:
			break;
		case YES:
			break;
		case MATCH:
		{
			png_stream.seekg(-8, ios::cur);
			temp_chunk.position = png_stream.tellg();

			char tempBuff[4] = { 0 };
			png_stream.read((char*)tempBuff, 4);
			temp_chunk.length = _byteswap_ulong(*((unsigned int*)tempBuff));
			png_stream.seekg(temp_chunk.length + 4, ios::cur);
			png_stream.read((char*)tempBuff, 4);
			temp_chunk.CRC = bytesToStr(tempBuff);
			chunks_IDAT.push_back(temp_chunk);
			str_temp.clear();
			break;
		}
		}
	}

	if (chunks_IDAT.size() == 0)
	{
		cout << "PNG File Error - No IDAT!" << endl;
		return 4;
	}

	////////////////////////////////////////////////////////////////////////////////////////

	for (auto it : chunks_IDAT)
	{ 
		cout << "IDAT chunk founded at " << it.position << endl;
		cout << "Length: " << it.length << "   CRC: 0x" << it.CRC << endl << endl;
	}
	cout << "PNG File parsed successfully!" << endl;
	return 0;
	////////////////////////////////////////////////////////////////////////////////////////
}