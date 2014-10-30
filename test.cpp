#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

struct Sector{//sizeOfSector: 1024 bytes
	unsigned int sectorNr;
	char rawData[1020];
public:
	Sector()/*set the rawData[1020] as /0, ?*/ { sectorNr = 3333; for (int i = 0; i < 1020; i++) rawData[i] = NULL; }
	Sector(unsigned int sectorNr) { Sector(); sectorNr = sectorNr; }
	//~Sector();//delete all data from sector, ?
};


int main()
{
	fstream dskfl;
	dskfl.open("hello.drive", ios::binary | ios::out);

	//
	Sector tmp_sector;
	for (int i = 0; i < 3200; i++)
	{
		tmp_sector.sectorNr = i;
		dskfl.write((char*)&tmp_sector, sizeof(Sector));
	}
	dskfl.close();

};
