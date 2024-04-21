#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdint> // uint8_t
#include<math.h>

#include <sstream>
#include <iomanip>
#include <Windows.h>


using namespace std;

ofstream fileoutput;

namespace ReadFiles {

	template<typename T> // dinh nghia mot khuan mau ham 

	void Print_text(string propStr, int level, T value, bool isEndLine = false, bool isHeader = false)
	{

		for (int i = 0; i < level; i++)
		{
			cout << "\t";
			fileoutput << "\t"; // file output
		}

		cout << propStr << ": ";
		fileoutput << propStr << ": ";

		if (isEndLine) {
			cout << "\n";
			fileoutput << "\n";
		}

		if (isHeader == false) {
			cout << value << "\n";
			fileoutput << value << "\n";
		}
		
	}

	// doc n byte ky tu va tra ve 1 chuoi trong file input
	string Read_string(ifstream& inputfile, int nbyte)
	{
		string resString = "";

		vector<char> buff = vector<char>(nbyte);
		// doc du lieu tu tap tin vao chuoi buff
		if (inputfile.read(buff.data(), buff.size())) // buff.data() : tra ve mot con tro dang tro o vi tri dau tien cua vector buff
		{
			// Chuyển đổi các giá trị byte trong vector buff thành các giá trị unsigned 8-bit integers và lưu trữ chúng vào vector arrbytes.
			vector<uint8_t>	arrbytes(buff.begin(), buff.begin() + nbyte); // uint8_t: kieu du lieu so nguyen k dau 1 byte, co gia tri tu 0 den 255
			for (int i = 0; i < nbyte; i++)
			{
				resString += char(arrbytes[i]);
				//res += char(buff[i]);
			}
		}
		return resString;
	}

	long long int Read_int(ifstream& inputfile, int nbyte)
	{
		long long int sizeBox = 0;

		vector<char> buff = vector<char>(nbyte);
		if (inputfile.read(buff.data(), buff.size()))
		{
			vector<uint8_t>	arrbytes(buff.begin(), buff.begin() + nbyte);
			for (int i = 0; i < arrbytes.size(); i++)
			{
				sizeBox += uint64_t(arrbytes[i] * pow(16, 2 * i));
			}
		}
		return sizeBox;
	}

	void Read_nbyte(ifstream& fileinput, long long int nbyte)
	{
		vector<char> buff = vector<char>(nbyte);

		fileinput.read(buff.data(), buff.size());
	}
}

namespace aviFile
{
	using namespace ReadFiles;

	// read data of chunk box
	void ReadData_avih_box(ifstream& fileinput, int level)
	{
		Print_text("dwMicroSecPerFrame", level, Read_int(fileinput, 4));
		Print_text("dwMaxbytesPerSec", level, Read_int(fileinput, 4));
		Print_text("dwPaddingGranularity", level, Read_int(fileinput, 4));
		Print_text("dwFlags", level, Read_int(fileinput, 4));
		Print_text("dwTotalFrames", level, Read_int(fileinput, 4));
		Print_text("dwInitialFrames", level, Read_int(fileinput, 4));
		Print_text("dwStreams", level, Read_int(fileinput, 4));
		Print_text("dwSuggestedBufferSize", level, Read_int(fileinput, 4));
		Print_text("dwWight", level, Read_int(fileinput, 4));
		Print_text("dwHeight", level, Read_int(fileinput, 4));

		for (int i = 0; i < 4; i++)
		{
			Print_text("dwReserved", level, Read_int(fileinput, 4));
		}
	}

	void ReadData_strh_box(ifstream& fileinput, int level)
	{
		Print_text("fccType", level, Read_int(fileinput, 4));
		Print_text("fccHandler", level, Read_string(fileinput, 4));
		Print_text("dwFlags", level, Read_int(fileinput, 4));

		Print_text("wPrority", level, Read_int(fileinput, 2));
		Print_text("wLanguage", level, Read_int(fileinput, 2));

		Print_text("dwInitialFrames", level, Read_int(fileinput, 4));
		Print_text("dwScale", level, Read_int(fileinput, 4));
		Print_text("dwRate", level, Read_int(fileinput, 4));
		Print_text("dwStart", level, Read_int(fileinput, 4));
		Print_text("dwLength", level, Read_int(fileinput, 4));
		Print_text("dwSuggestedBufferSize", level, Read_int(fileinput, 4));
		Print_text("dwQuality", level, Read_int(fileinput, 4));
		Print_text("dwSampleSize", level, Read_int(fileinput, 4));

		Print_text("rcFrame", level, Read_int(fileinput, 8));
	}

	void ReadData_dmlh_box(ifstream& fileinput, int level)
	{
		Print_text("dwTotalFrames", level, Read_int(fileinput, 4));
	}

	// ----------------------------------------------------------

	// read chunk box
	long long int Read_chuckBox(ifstream& fileinput, string nameBox, int level)
	{
		long long int sizeBox = Read_int(fileinput, 4);

		if (sizeBox <= 0) return 8; // 8 byte cua name va size

		Print_text("CHUNK", level - 1, "", true, true);
		//--
		Print_text("dwFourCC", level, nameBox);
		Print_text("dwSize", level, sizeBox);

		// data[]
		if (nameBox == "avih")
		{
			ReadData_avih_box(fileinput, level);
		}
		else if (nameBox == "strh")
		{
			ReadData_strh_box(fileinput, level);
		}
		else if (nameBox == "dmlh")
		{
			//ReadData_dmlh_box(fileinput, level);
			Read_nbyte(fileinput, sizeBox);
		}
		else { // strf, strn, indx, ...
			Read_nbyte(fileinput, sizeBox);
		}

		return sizeBox + 8;
	}


	// read 1 box bat ky cua file avi 
	long long int Read_AviBox(ifstream& fileinput, int level)
	{
		string nameBox = Read_string(fileinput, 4);
		long long int currentSize = 0;
		long long int originalSize = 0;

		if (nameBox == "LIST")
		{
			Print_text("LIST", level, "", true, true);
			//--
			Print_text("dwList", level, nameBox);

			originalSize = Read_int(fileinput, 4);
			currentSize = originalSize;

			Print_text("dwSize", level, originalSize);

			string dwFourCC_Str = Read_string(fileinput, 4);
			Print_text("dwFourCC", level, dwFourCC_Str);

			currentSize -= 4;

			// data[]
			while (currentSize > 4)
			{
				currentSize -= Read_AviBox(fileinput, level + 1);
			}
		}
		else
		{
			// read ChunkBox
			originalSize = Read_chuckBox(fileinput, nameBox, level + 1);
		}

		return originalSize + 8;
	}

	void Read_aviFile(ifstream& fileinput, int level)
	{
		string nameBox = Read_string(fileinput, 4);
		long long int sizeBox = Read_int(fileinput, 4);

		Print_text("RIFF", level, "", true, true);

		//--
		Print_text("dwList", level, nameBox);
		Print_text("dwSize", level, sizeBox);

		string dwFourCC_Str = Read_string(fileinput, 4);
		Print_text("dwFourCC", level, dwFourCC_Str);

		sizeBox -= 4;

		// data[]
		while (sizeBox > 4)
		{
			sizeBox -= Read_AviBox(fileinput, level + 1);
		}
	}

}


int main() {

	using namespace aviFile;

	ifstream fileinput;
	fileinput.open("file_example_AVI_480_750kB.avi", std::ios::binary);

	fileoutput.open("output_read_aviFile.txt", ios::out);

	try
	{
		Read_aviFile(fileinput, 0); // level = 0
	}
	catch (const std::exception& e)
	{
		cout << "Error: can't open file.avi!";
		fileoutput << "Error: can't open file.avi!";
	}

	
	fileinput.close();
	fileoutput.close();
	return 0;
}