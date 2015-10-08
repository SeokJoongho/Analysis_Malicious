#ifndef __CONVERT_H__
#define __CONVERT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bson.h>
#include <mongoc.h>

typedef unsigned __int64 ULONGLONG;

#define database_convert "database"
#define convert_server_addr "mongodb://59.11.39.160:43572/"

typedef struct __IMAGE_DOS_HEADER // DOS .EXE header
{      
	unsigned short   e_magic;                     // Magic number
	unsigned short   e_cblp;                      // unsigned chars on last page of file
	unsigned short   e_cp;                        // Pages in file
	unsigned short   e_crlc;                      // Relocations
	unsigned short   e_cparhdr;                   // Size of header in paragraphs
	unsigned short   e_minalloc;                  // Minimum extra paragraphs needed
	unsigned short   e_maxalloc;                  // Maximum extra paragraphs needed
	unsigned short   e_ss;                        // Initial (relative) SS value
	unsigned short   e_sp;                        // Initial SP value
	unsigned short   e_csum;                      // Checksum
	unsigned short   e_ip;                        // Initial IP value
	unsigned short   e_cs;                        // Initial (relative) CS value
	unsigned short   e_lfarlc;                    // File address of relocation table
	unsigned short   e_ovno;                      // Overlay number
	unsigned short   e_res[4];                    // Reserved unsigned shorts
	unsigned short   e_oemid;                     // OEM identifier (for e_oeminfo)
	unsigned short   e_oeminfo;                   // OEM information; e_oemid specific
	unsigned short   e_res2[10];                  // Reserved unsigned shorts
	int   e_lfanew;                    // File address of new exe header
} imageDosHeader, *pImageDosHeader;

typedef struct __IMAGE_FILE_HEADER 
{
	unsigned short    Machine;
	unsigned short    NumberOfSections;
	unsigned int   TimeDateStamp;
	unsigned int   PointerToSymbolTable;
	unsigned int   NumberOfSymbols;
	unsigned short    SizeOfOptionalHeader;
	unsigned short    Characteristics;
} imageFileHeader, *pImageFileHeader;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

typedef struct __IMAGE_DATA_DIRECTORY 
{
	unsigned int   VirtualAddress;
	unsigned int   Size;
} imageDataDirectory, *pImageDataDirectory;

typedef struct __IMAGE_OPTIONAL_HEADER 
{
	//
	// Standard fields.
	//

	unsigned short    Magic;
	unsigned char    MajorLinkerVersion;
	unsigned char    MinorLinkerVersion;
	unsigned int   SizeOfCode;
	unsigned int   SizeOfInitializedData;
	unsigned int   SizeOfUninitializedData;
	unsigned int   AddressOfEntryPoint;
	unsigned int   BaseOfCode;
	unsigned int   BaseOfData;

	//
	// NT additional fields.
	//

	unsigned int   ImageBase;
	unsigned int   SectionAlignment;
	unsigned int   FileAlignment;
	unsigned short    MajorOperatingSystemVersion;
	unsigned short    MinorOperatingSystemVersion;
	unsigned short    MajorImageVersion;
	unsigned short    MinorImageVersion;
	unsigned short    MajorSubsystemVersion;
	unsigned short    MinorSubsystemVersion;
	unsigned int   Win32VersionValue;
	unsigned int   SizeOfImage;
	unsigned int   SizeOfHeaders;
	unsigned int   CheckSum;
	unsigned short    Subsystem;
	unsigned short    DllCharacteristics;
	unsigned int   SizeOfStackReserve;
	unsigned int   SizeOfStackCommit;
	unsigned int   SizeOfHeapReserve;
	unsigned int   SizeOfHeapCommit;
	unsigned int   LoaderFlags;
	unsigned int   NumberOfRvaAndSizes;
	imageDataDirectory DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} imageOptionalHeader32, *pImageOptionalHeader32;

typedef struct __IMAGE_OPTIONAL_HEADER64 {
	unsigned short        Magic;
	unsigned char        MajorLinkerVersion;
	unsigned char        MinorLinkerVersion;
	unsigned int       SizeOfCode;
	unsigned int       SizeOfInitializedData;
	unsigned int       SizeOfUninitializedData;
	unsigned int       AddressOfEntryPoint;
	unsigned int       BaseOfCode;
	ULONGLONG   ImageBase;
	unsigned int       SectionAlignment;
	unsigned int       FileAlignment;
	unsigned short        MajorOperatingSystemVersion;
	unsigned short        MinorOperatingSystemVersion;
	unsigned short        MajorImageVersion;
	unsigned short        MinorImageVersion;
	unsigned short        MajorSubsystemVersion;
	unsigned short        MinorSubsystemVersion;
	unsigned int       Win32VersionValue;
	unsigned int       SizeOfImage;
	unsigned int       SizeOfHeaders;
	unsigned int       CheckSum;
	unsigned short        Subsystem;
	unsigned short        DllCharacteristics;
	ULONGLONG   SizeOfStackReserve;
	ULONGLONG   SizeOfStackCommit;
	ULONGLONG   SizeOfHeapReserve;
	ULONGLONG   SizeOfHeapCommit;
	unsigned int       LoaderFlags;
	unsigned int       NumberOfRvaAndSizes;
	imageDataDirectory DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} imageOptionalHeader64, *pImageOptionalHeader64;

typedef struct __IMAGE_NT_HEADERS 
{
	unsigned int Signature;
	imageFileHeader FileHeader;
	imageOptionalHeader32 OptionalHeader;
} imageNtHeader32, *pImageNtHeader32;

typedef struct __IMAGE_NT_HEADERS64 {
	unsigned int Signature;
	imageFileHeader FileHeader;
	imageOptionalHeader64 OptionalHeader;
} imageNtHeader64, *pImageNtHeader64;

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct __IMAGE_SECTION_HEADER {
	unsigned char    Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		unsigned int   PhysicalAddress;
		unsigned int   VirtualSize;
	} Misc;
	unsigned int   VirtualAddress;
	unsigned int   SizeOfRawData;
	unsigned int   PointerToRawData;
	unsigned int   PointerToRelocations;
	unsigned int   PointerToLinenumbers;
	unsigned short    NumberOfRelocations;
	unsigned short    NumberOfLinenumbers;
	unsigned int   Characteristics;
} imageSectionHeader, *pImageSectionHeader;

#define IMAGE_SIZEOF_SECTION_HEADER          40

typedef struct __IMAGE_IMPORT_DESCRIPTOR 
{
	union 
	{
		unsigned int   Characteristics;            // 0 for terminating null import descriptor
		unsigned int   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
	} dummyUnionName;
	unsigned int   TimeDateStamp;                  // 0 if not bound,
	// -1 if bound, and real date\time stamp
	//     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
	// O.W. date/time stamp of DLL bound to (Old BIND)

	unsigned int   ForwarderChain;                 // -1 if no forwarders
	unsigned int   Name;
	unsigned int   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} imageImportDescriptor;

typedef imageImportDescriptor UNALIGNED *pImageImportDescriptor;

typedef struct __IMAGE_EXPORT_DIRECTORY 
{
	unsigned int   Characteristics;
	unsigned int   TimeDateStamp;
	unsigned short    MajorVersion;
	unsigned short    MinorVersion;
	unsigned int   Name;
	unsigned int   Base;
	unsigned int   NumberOfFunctions;
	unsigned int   NumberOfNames;
	unsigned int   AddressOfFunctions;     // RVA from base of image
	unsigned int   AddressOfNames;         // RVA from base of image
	unsigned int   AddressOfNameOrdinals;  // RVA from base of image
} imageExportDirectory, *pImageExportDirectory;

typedef struct __IMAGE_IMPORT_BY_NAME {
	unsigned short    Hint;
	unsigned char    Name[1];
} imageImportByName, *pImageImportByName;

#define DIRENTRY "bin\\DataBase_"
#define NAMESIZE 100

typedef struct __IAT_DATA { //New struct!!
	char dllName[NAMESIZE];
	char funcName[NAMESIZE];
	long funcAddr_RVA;
} iatData, *pIatData;

class convert
{
private:
	/***************************************
				private variables
	****************************************/
	FILE* in;
	FILE* allData;

	mongoc_client_t *client_convert;

	int ch;
	int count;
	int arch;
	int iidSize, iibnSize;
	int* temp;
	char* ex_name;
	int convert_connect_cnt;

	imageDosHeader idh;
	imageNtHeader32 inh;
	imageNtHeader64 inh64;
	imageOptionalHeader64 ioh64;
	imageSectionHeader* ish;
	imageImportDescriptor* iid;
	imageExportDirectory ied;
	imageImportByName* iibn;

	iatData* idata;
	
	/***************************************
		private functions for binary data
	****************************************/
	void saveWholeBinaryFile();
	
	/***********************************************
		private functions for PE file data parsing
	************************************************/
	void parseImageDosHeader(FILE* file, imageDosHeader* idh);
	int parseImageNTHeader(FILE* file, imageNtHeader32* inh, imageNtHeader64* inh64);
	int parseImageFileHeader(FILE* file, imageFileHeader* ifh);
	void parseImageOptionalHeader32(FILE* file, imageOptionalHeader32* ioh);
	void parseImageOptionalHeader64(FILE* file, imageOptionalHeader64* ioh64);
	void parseImageDataDirectory(FILE* file, imageDataDirectory* idd);
	int parseImageSectionHeader(FILE* file, imageSectionHeader* ish);
	void parseImageImportDescriptor(FILE* file, imageImportDescriptor* iid);
	void parseImageImportByName(FILE* file, imageImportByName* iibn, iatData* idata);
	short countImageImportByName(FILE* file, imageNtHeader32 *inh, imageSectionHeader *ish, imageImportDescriptor* iid);
	void parseIATData(FILE *file, imageNtHeader32 *inh, imageSectionHeader *ish, long dllAddr, iatData* idata);
	short countImageImportDescriptor(FILE *file, imageNtHeader32 *inh, imageSectionHeader *ish);
	/*******************************************************************
		private functions for convert binary file to assembly language
	*********************************************************************/
	long convertRVAToRAW(imageNtHeader32 *inh, imageSectionHeader *ish, unsigned int addr_RVA);
	int convertHexToAsm(FILE* file, imageSectionHeader codeIsh, imageSectionHeader rdataIsh, imageSectionHeader dataIsh, iatData* idata, int iibnSize);
	
	/*****************************************************************************
		private functions for support convertin binary file to assembly language
	******************************************************************************/
	int findCodeSection(unsigned char* str);
	void fseekAddrToValue(FILE *file, imageNtHeader32 *inh, imageSectionHeader *ish, long addr);
	int pointToTextSection(char* str);
	int findIAT(iatData* idata, iatData* iatData, int iibnSize, int addr);

	/************************************
		private functions for string
	*************************************/
	int stringContain(char* str1, char* str2);
	int stringIsNull(char* str);
	void subStr(char* input, char* output, int nStart, int nLen);
	
	/*******************************************
		private functions for using database
	********************************************/
	int start_db();

	char* find_hex_contents (char* hexcode);
	int find_hex (char* hexcode);

	int insert_header_info(char *target_machine, char *comp_timestamp, int entry_pt, int num_of_section);
	int insert_section(char *name, int vir_addr, int vir_size, int raw_size);
	int insert_import(char *dll_name,char *func_name);

	void non_exist(char *func_name);
	void convert_value_strtok(char *str, char *query_func_name, char *print_str);	
	
public:
	/*****************
		constructor
	******************/
	convert();
	convert(char* dirPath);

	/****************************************************
		public function for initializing class member
	*****************************************************/
	void clear();
	void reOpenFile(char* dirPath);

	/*********************************************************
		public function for calling private member function
	**********************************************************/
	void doParseImageDosHeader();
	void doSkip();	
	int doParseImageNTHeader();
	int findIATData();
	int doConvertHexToAsm();
	void separateSection();

	int doConvert();

	/***********************************************************
		public function for sending PE file data to database
	************************************************************/
	int sendPEHeaderInfo();
	void sendPESectionInfo();
	void sendPEImportInfo();

	/******************************************
		public function for get PE file data
	*******************************************/
	int getMachine();
	tm* getTimeStamp();
	int getAddressOfEntryPoint();
	int getNumberOfSections();
	char* getSectionName(int i);
	int getVirtualAddress(int i);
	int getVirtualSize(int i);
	int getSizeOfRawData(int i);
	int getIibnSize();
	char* getDllName(int i);
	char* getFuncName(int i);
	char* getPath();

	/****************
		destructor
	*****************/
	~convert();
};

#endif