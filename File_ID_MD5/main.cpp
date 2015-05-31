/*
	Program: File_ID_MD5

	使用檔案名稱計算MD5值(128-bit)，接著把該值分成4段(32-bit)，全部做XOR後當作File ID
	此輸出格式可匯入Excel使用
 */

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <windows.h>
#include <dirent.h>

#include <md5.h>
#include <hex.h>

#pragma comment(lib, "cryptlib.lib")

using namespace std;
using namespace CryptoPP;

int main()
{
	int ID;
	int p1, p2, p3, p4;
	string file_name;

	fstream comp_file, dup_log;
	string comp_path = "./Comparison_Table.txt", dup_path = "./Duplicated_List.txt";

	DIR *dp;
	struct dirent *ep;
	string doc_path = "./Doc/";

	CryptoPP::MD5 hash;
	byte digest[CryptoPP::MD5::DIGESTSIZE];

	HexEncoder encoder;
	string output;

	vector<int> v;
	vector<int>::iterator iter;

	LARGE_INTEGER startTime, endTime, fre;
	double times;

	QueryPerformanceFrequency(&fre); // 取得CPU頻率
	QueryPerformanceCounter(&startTime); // 取得開機到現在經過幾個CPU Cycle
	/* Program to Timing */
	dp = opendir(doc_path.c_str()); // for each file f, create a list f_bar of unique keyword
	if (dp != NULL)
	{
		readdir(dp); // .
		readdir(dp); // ..
		comp_file.open(comp_path, ios::out);
		if (!comp_file)
			cerr << "Error: open" << comp_path << " failed..." << endl;
		else
		{
			dup_log.open(dup_path, ios::out);
			if (!dup_log)
				cerr << "Error: can not create " << dup_path << endl;

			comp_file << "File name                      ";
			comp_file << left << ", " << "ID        " << ", MD5 " << endl;;
			
			while (ep = readdir(dp))
			{
				output.clear();
				//printf("Processing file: %s\n", ep->d_name);
				file_name.assign(ep->d_name);
				hash.CalculateDigest(digest, (byte*)file_name.c_str(), file_name.length());

				encoder.Attach(new StringSink(output));
				encoder.Put(digest, sizeof(digest));
				encoder.MessageEnd();

				//cout << output << std::endl;

				strncpy((char*)&p1, (char*)&digest[0], 4);
				strncpy((char*)&p2, (char*)&digest[4], 4);
				strncpy((char*)&p3, (char*)&digest[8], 4);
				strncpy((char*)&p4, (char*)&digest[12], 4);

				ID = p1 ^ p2 ^ p3 ^ p4;
				iter = find(v.begin(), v.end(), ID);
				if (iter == v.end())
				{
					v.push_back(ID);
					//cout << ID << endl;

					comp_file.width(32);
					comp_file << left << file_name << ", ";
					comp_file.width(11);
					comp_file << right << ID << ", " << output << endl;
				}
				else
				{
					cout << "Duplicated ID: " << ID << endl;
					cout << "    File name: " << file_name << endl << endl;
					dup_log << "Duplicated ID: " << ID << endl;
					dup_log << "    File name: " << file_name << endl << endl;
				}

			}
			comp_file.close();
		}
	}

	QueryPerformanceCounter(&endTime); // 取得開機到程式執行完成經過幾個CPU Cycle
	times = ((double)endTime.QuadPart - (double)startTime.QuadPart) / fre.QuadPart;

	cout << fixed << setprecision(3) << "Processing time: " << times << 's' << endl << endl;
	dup_log << fixed << setprecision(3) << "Processing time: " << times << 's' << endl << endl;
	dup_log.close();

	system("PAUSE");
	return 0;
}