#include <iostream>
using namespace std;
#include <fstream>
#include <bitset>
#include <string>


struct Sector{//sizeOfSector: 1024 bytes
	unsigned int sectorNr;
	char rawData[1020];
public:
	Sector()/*set the rawData[1020] as /0, ?*/ { sectorNr = 3333; for (int i = 0; i < 1020; i++) rawData[i] = NULL; }
	Sector(unsigned int sectorNr) { Sector(); sectorNr = sectorNr; }
	//~Sector();//delete all data from sector, ?
};


struct Volume_Header{//sizeOfVolume_Header: 1024 bytes
	unsigned int  sectorNr;//--in our case--> 0
	char diskName[12];
	char diskOwner[12];
	char prodDate[10];// produce date: --in our case--> the date of today
	unsigned int ClusQty;//num of clusters: --in our case--> 3200/2
	unsigned int  dataClusQty;//num ofata dclusters: --in our case--> ClusQty-4
	unsigned int  addrDAT;//address of DAT sector:--in our case--> 2nd sector
	unsigned int  addrRootDir;// address of rootdir cluster:--in our case--> 3rd sector?
	unsigned int  addrDATcpy;// arddress of second copy of DAT:--in our case--> NULL
	unsigned int  addrRootDirCpy;//arddress of second copy of rootdir:--in our case--> NULL
	unsigned int  addrDataStart;// first cluster of data:--in our case--> 5th sector
	char  formatDate[10];// format of date:--in our case--> (dd/mm/yyyy)  
	bool isFormated;//
	char  emptyArea[944];//

public:
	Volume_Header()// set all fieldes according to our disk structure, else - null them
	{
		sectorNr = 0;
		for (int i = 0; i < 12; i++)
		{
			diskName[i] = NULL;
			diskOwner[i] = NULL;
		}
		//prodDate =
		ClusQty = 3200 / 2;
		dataClusQty = ClusQty - 4;
		addrDAT = 2;
		addrRootDir = 3;
		addrDATcpy = NULL;
		addrRootDirCpy = NULL;
		addrDataStart = 5;
		strcpy(formatDate, "dd/mm/yyyy");
		//isFormated = false;
		for (int i = 0; i < 944; i++) emptyArea[i] = NULL;
	}
	Volume_Header(string & disk_name, string & disk_owner) { Volume_Header(); strcpy(diskName, disk_name.c_str); strcpy(diskOwner, disk_owner.c_str); }
	~Volume_Header();
};

typedef bitset<1600> DATtype;
struct DAT{//size:
	unsigned intsectorNr;
	DATtype dat_bits;
	char emptyArea[816];

public:
	DAT(){ intsectorNr = 1; dat.set(); }//empty disc, all sectors avalable
	DAT(char only_first)// only the first cluster will be marksd as full
	{
		//
		intsectorNr = 1;
		//
		dat.set();//set all bits to 1
		for (int i = 0; i < 4; i++) dat.reset(i);
	}
	~DAT();
};


struct DirEntry{//size: 72 bytes
	char Filename[12];
	char fileOwner[12];
	unsigned int fileAddr; //first sector address
	char crDate[10];//creation date
	unsigned int fileSize;//no of sectors
	unsigned int eofRecNr;
	unsigned int maxRecSize;
	unsigned int actualRecSize;
	char recFormat[2];//F or V or D
	unsigned int keyOffset;
	unsigned int keySize;
	char keyType[2];
	unsigned char entryStatus;

public:
	DirEntry();// set all fieldes as /0
	//DirEntry(char enter_details);// details enterd by user
	~DirEntry();
};


struct DirSector{//size: 1024 bytes, 72*14 + 4 +12
	unsigned int sectorNr;
	DirEntry  DirEntrys[14];
	char UnUse[12];

public:
	DirSector();//sectorNr = 0/, DirEntrys withe there default constructor
	DirSector(unsigned int sector_num);////sectorNr = sector_num, DirEntrys withe there default constructor
	~DirSector();
};


struct RootDir{//size: 2048 bytes
	DirSector firstRootSector;
	DirSector secondRootSector;

public:

};




class disk {
	Volume_Header vhd;
	DAT dat;
	RootDir rootdir;
	bool mounted;
	fstream dskfl;
	unsigned int currDiskSectorNr;



public:

	//*************
	//
	disk()// vhd, dat and rootdir has there oun constructors. currDiskSectorNr = 0/
	{
		currDiskSectorNr = 3333;
	}

	//*************
	//
	disk(string & host_file_name, string & disk_owner, char c_or_m/*, unsigned int */)
	{
		if (c_or_m == 'c') createdisk(host_file_name, disk_owner);
		mountdisk(host_file_name);
	}

	//*************
	//
	~disk()// unmount, zeroall fieldes.
	{
		unmountdisk();

	}

	//*************
	//
	void createdisk(string & disk_name, string & disk_owner)//simulates disk produsing, or ...
	{

		//create a file, write empty sectors, and number them
		dskfl.open(disk_name, ios::binary | ios::out);

		//
		Sector tmp_sector;
		for (int i = 0; i < 3200; i++)
		{
			tmp_sector.sectorNr = i;
			dskfl.write((char*)&tmp_sector, sizeof(Sector));
		}

		//enter details to vhd, then write vhd to the first sectorcf
		strcpy(vhd.diskName, disk_name.c_str);
		strcpy(vhd.diskOwner, disk_owner.c_str);
		vhd.isFormated = false;//?

		//
		dskfl.seekp(0);
		dskfl.write((char*)&vhd, sizeof(Volume_Header));

		//
		dskfl.close();

	}


	//*************
	//
	void mountdisk(string & disk_name)//simulates if we are accesing the disk with ure computer
	{
		//copy first 2 clusters from file to the vhd, dat and rootdir fieldes

		//read vhd from sector 0
		dskfl.open(disk_name, ios::binary | ios::in | ios::out);
		//dskfl.seekg(0);
		dskfl.read((char*)&vhd, sizeof(Volume_Header));

		//read dat from sector 1
		//dskfl.seekg(1);
		dskfl.read((char*)&dat, sizeof(DAT));

		//read rootdir from sectors 2 3
		//dskfl.seekg(2);
		dskfl.read((char*)&rootdir, sizeof(RootDir));

		//turn mount field to true
		mounted = true;
	}


	//*************
	//
	void unmountdisk()//simulates as if we are unaccesing the disk with ure computer
	{
		//copy the vhd, dat and rootdir fieldes to the first 2 clusters of the file

		//write vhd to sector 0
		dskfl.write((char*)&vhd, sizeof(Volume_Header));

		//write vhd to sector 1
		dskfl.write((char*)&dat, sizeof(DAT));

		//write vhd to sectors 2 3
		dskfl.write((char*)&rootdir, sizeof(RootDir));

		//
		dskfl.close();

		//turn mount field to true
		mounted = false;
	}


	//*************
	//
	/*void recreatedisk(string & disk_owner)
	{

	if(dskfl && !mounted && disk_owner.compare(vhd.diskOwner))
	createdisk(string(vhd.diskName), string(vhd.diskOwner));
	}*/

	void recreatedisk(string & disk_owner, string & disk_name)
	{

		if (dskfl && !mounted && disk_owner.compare(vhd.diskOwner))

			dskfl.i();
		createdisk(string(vhd.diskName), string(vhd.diskOwner));
	}


	//*************
	//
	fstream* getdskfl()
	{
		if (dskfl.is_open())
			return &dskfl;
		//else
		return NULL;
	}


	//*************
	//
	void seekToSector(unsigned int required_sector)
	{
		currDiskSectorNr = required_sector;
	}


	//*************
	//write from buffer to sertain? sector
	void writeSector(unsigned int required_sector, Sector* buffer)
	{
		//if mounted...

		//check if required_sector is allready full
		if (!dat.dat_bits.at(required_sector))
			exception;

		//update currDiskSectorNr
		currDiskSectorNr = required_sector;

		//write each byte from buffer to sector
		//dskfl.open;
		dskfl.seekp(currDiskSectorNr*sizeof(Sector) + 4);
		dskfl.write((char*)&(buffer->rawData), sizeof(Sector::rawData));

		//update DAT -- ????
		//temporary version: we need to now if we want to write more to te cluster
		dat.dat_bits.reset(currDiskSectorNr);

		//update currDiskSectorNr
		currDiskSectorNr++;
	}


	//*************
	//write from buffer to sector
	void writeSector(Sector* buffer)
	{
		//check if required_sector is allready full
		if (!dat.dat_bits.at(currDiskSectorNr))
			exception;

		//write each byte from buffer to sector
		dskfl.open;
		dskfl.seekp(currDiskSectorNr*sizeof(Sector) + 4);
		dskfl.write((char*)&(buffer->rawData), sizeof(Sector::rawData));

		//update DAT -- ????
		//temporary version: we need to now if we want to write more to te cluster
		dat.dat_bits.reset(currDiskSectorNr);

		//update currDiskSectorNr
		currDiskSectorNr++;
	}


	//*************
	//
	void readSector(int required_sector, Sector* buffer)
	{
		//check if required_sector is empty
		if (dat.dat_bits.at(required_sector))
			exception;

		//update currDiskSectorNr
		currDiskSectorNr = required_sector;

		//read each byte from sector to buffer
		//dskfl.open;
		dskfl.seekg(currDiskSectorNr*sizeof(Sector), ios_base::beg);
		dskfl.read((char*)buffer, sizeof(Sector));

		//update currDiskSectorNr
		currDiskSectorNr++;
	}


	//*************
	//
	void readSector(Sector* buffer)
	{
		//check if required_sector is empty
		if (dat.dat_bits.at(currDiskSectorNr))
			exception;

		//read each byte from sector to buffer
		//dskfl.open;
		dskfl.seekg(currDiskSectorNr*sizeof(Sector), ios_base::beg);
		dskfl.read((char*)buffer, sizeof(Sector));

		//update currDiskSectorNr
		currDiskSectorNr++;
	}

};