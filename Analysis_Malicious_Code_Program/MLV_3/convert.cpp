#define _CRT_SECURE_NO_WARNINGS
#include "convert.h"

/*
convert class constructor
*/
convert::convert()
{
	start_db();

	ish = NULL;
	iid = NULL;
	iibn = NULL;
	idata = NULL;

	count = 0;
	iibnSize = 0;
}
convert::convert(char* dirPath)
{
	char* ptr;
	char* path;
	int j = 0;

	path = (char *)malloc(sizeof(char) * strlen(dirPath) + 1);
	strcpy(path, dirPath);

	if( (in = fopen(dirPath, "rb")) == NULL)
	{
		fputs("File open failed\n", stderr);
		exit(1);
	}

	start_db();

	ptr = strtok(path, "\\");

	//사용자가 입력한 경로에서 실행파일의 이름을 가져옴
	while(ptr != NULL)
	{
		if(strstr(ptr, ".exe") != NULL)
		{
			ex_name = (char *)malloc(sizeof(char) * strlen(ptr) + 1);
			strcpy(ex_name, strtok(ptr, ".exe"));
			break;
		}

		ptr = strtok(NULL, "\\");
	}

	allData = fopen("wholeEXEData.txt", "w");

	ish = NULL;
	iid = NULL;
	iibn = NULL;
	idata = NULL;

	free(path);
}

/*
Database와 connection을 생성하기 위한 함수
5번의 연결 시도

success : return 0
failed	: return -1
*/
int convert::start_db()
{
	convert_connect_cnt = 5;
	client_convert = NULL;
	bson_t *query = NULL;
	const bson_t *doc;
	mongoc_cursor_t *cursor = NULL;
	mongoc_collection_t *collection = NULL;
	bool connect_success = false;

	while(connect_success == false && convert_connect_cnt > 0){

		if(client_convert)      mongoc_client_destroy(client_convert);
		if(cursor)      mongoc_cursor_destroy(cursor);
		if(query)      bson_destroy(query);
		if(collection)   mongoc_collection_destroy(collection);

		mongoc_init ();
		query = bson_new ();
		client_convert = mongoc_client_new (convert_server_addr);         // connect ip address
		collection = mongoc_client_get_collection (client_convert, database_convert, "Total function");
		cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

		if(mongoc_cursor_next(cursor,&doc))   //Check connection
			connect_success = true;
		else{
			if(convert_connect_cnt != 0)         //reconnection 5 times
				convert_connect_cnt--;
		}
	}

	if(cursor)
	{
		mongoc_cursor_destroy(cursor);
	}

	if(query)
	{
		bson_destroy (query);
	}

	if(collection)
	{
		mongoc_collection_destroy (collection);
	}
	if(connect_success == false) 
	{
		if(client_convert)
			mongoc_client_destroy(client_convert);
		return -1;
	}

	return 0;
}

/*
private function을 실행시키는 함수
*/
void convert::doParseImageDosHeader()
{
	parseImageDosHeader(in, &idh);
}
void convert::doSkip()
{
	unsigned char buf[17];
	char offset[16];
	int i = 0;
	int len;

	FILE* dosStub = fopen("dosStubData.txt", "w");

	while(1)
	{		
		sprintf(offset, "%08X	", ftell(in));
		len = fread(buf, 1, 16, in);

		if(ftell(in) >= idh.e_lfanew)
			break;

		fputs(offset, dosStub);
		for(i = 0; i < 16; i++)
		{
			fprintf(dosStub, "%02X ", buf[i]);
		}

		fprintf(dosStub, "	");

		for(i = 0; i < 16; i++)
		{
			if(buf[i] >= 0x20 && buf[i] <= 0x7E)
				fprintf(dosStub, "%c ", buf[i]);
			else
				fprintf(dosStub, ". ");
		}
		fprintf(dosStub, "\n");
	}

	fclose(dosStub);

	if( fseek(in, idh.e_lfanew, SEEK_SET) == -1 )
	{
		printf("file offset skip failed\n");
	}
}
int convert::doParseImageNTHeader()
{
	int returnValue = 0;

	if(parseImageNTHeader(in, &inh, &inh64) == 64)
	{
		return -99;

		/* 64bit architecture supports not yet
		arch = 64;
		ish = (imageSectionHeader*)malloc(sizeof(imageSectionHeader) * inh64.FileHeader.NumberOfSections);

		for(int i=0; i < inh64.FileHeader.NumberOfSections; i++)
		{
		returnValue = parseImageSectionHeader(in, &ish[i]);

		if(returnValue == -1)
		{
		return -1;
		}

		else if(returnValue == -2)
		{
		return -2;
		}
		}

		if(ish[0].PointerToRawData == 0x00 || ish[0].SizeOfRawData == 0x00)
		{
		return -3;
		}

		return 0;
		*/
	}
	else
	{
		arch = 32;
		ish = (imageSectionHeader*)malloc(sizeof(imageSectionHeader) * inh.FileHeader.NumberOfSections);

		for(int i=0; i < inh.FileHeader.NumberOfSections; i++)
		{
			returnValue = parseImageSectionHeader(in, &ish[i]);

			if(returnValue == -1)
			{
				return -1;
			}

			else if(returnValue == -2)
			{
				return -2;
			}
		}

		if(ish[0].PointerToRawData == 0x00 || ish[0].SizeOfRawData == 0x00)
		{
			return -3;
		}

		return returnValue;
	}
}
int convert::findIATData()
{
	iidSize = countImageImportDescriptor(in, &inh, ish); //size 구하는 방법 수정 -why? 구조체들의 전체 크기가 일정치 않음

	if(iidSize >= 0)
	{
		iid = (imageImportDescriptor*)malloc(sizeof(imageImportDescriptor) * iidSize); 
		temp = (int*)malloc(sizeof(int) * iidSize+2);
		temp[0] = 0; temp[1] = 0;

		if(fseek(in, convertRVAToRAW(&inh, ish, inh.OptionalHeader.DataDirectory[1].VirtualAddress), SEEK_SET) == -1)
		{
			printf("Offset setting failed\n");
			return -7;
		}

		for(int i = 0; i < iidSize; i++) {
			parseImageImportDescriptor(in, &iid[i]);
		}

		for(int i = 0; i < iidSize; i++) {
			temp[i+2] = countImageImportByName(in, &inh, ish, &iid[i]);
			iibnSize += temp[i+2];
		}

		if(iibnSize >= 0)
		{

			iibn = (imageImportByName*)malloc(sizeof(imageImportByName) * iibnSize);
			idata = (iatData*)malloc(sizeof(iatData) * iibnSize);

			for(int i = 2; i < iidSize+2; i++) {
				for(int j = 0; j < temp[i]; j++) {
					fseekAddrToValue(in, &inh, ish, iid[i-2].FirstThunk+(j*4)); //INT정보를 보고 찾는게 맞으나 정보가 없는 경우가 있음
					parseImageImportByName(in, &iibn[temp[i-2]+temp[i-1]+j], &idata[temp[i-2]+temp[i-1]+j]);
					parseIATData(in, &inh, ish, iid[i-2].Name, &idata[temp[i-2]+temp[i-1]+j]);
					idata[temp[i-2]+temp[i-1]+j].funcAddr_RVA = iid[i-2].FirstThunk+(j*4);
				}
			}

			return 0;
		}

		else
		{
			return -7;
		}
	}

	else
	{
		return -7;
	}
}
int convert::doConvertHexToAsm()
{
	int returnValue = 0;

	if(arch == 64)
	{
	}

	else
	{
		returnValue = convertHexToAsm(in, ish[0], ish[1], ish[2], idata, iibnSize);
	}

	fclose(in);
	in = NULL;

	if(returnValue == -4)
	{
		return -4;
	}

	else if(returnValue == -5)
	{
		return -5;
	}

	return returnValue;
}

/*
convert class가 동작하면서 사용한 메모리 및 변수를 초기화
*/
void convert::clear()
{
	FILE * fp;
	char buf[1024];

	count = 0;
	iibnSize = 0;

	fp = fopen("wholeEXEData.txt", "r");

	if(fp != NULL)
	{
		fclose(fp);
		remove("wholeEXEData.txt");
	}

	fp = fopen("idhData.txt", "r");

	if(fp != NULL)
	{
		fclose(fp);
		remove("idhData.txt");
	}

	fp = fopen("dosStubData.txt", "r");

	if(fp != NULL)
	{
		fclose(fp);
		remove("dosStubData.txt");
	}

	fp = fopen("inhData.txt", "r");

	if(fp != NULL)
	{
		fclose(fp);
		remove("inhData.txt");
	}

	fp = fopen("inhOverView.txt", "r");

	if(fp != NULL)
	{
		fclose(fp);
		remove("inhOverView.txt");
	}

	fp = fopen("inhSignature.txt", "r");

	if(fp != NULL)
	{
		fclose(fp);
		remove("inhSignature.txt");
	}

	fp = fopen("ifhData.txt", "r");

	if(fp != NULL)
	{
		fclose(fp);
		remove("ifhData.txt");
	}

	fp = fopen("iohData.txt", "r");

	if(fp != NULL)
	{
		fclose(fp);
		remove("iohData.txt");
	}

	fp = fopen("iddData.txt", "r");

	if(fp != NULL)
	{
		fclose(fp);
		remove("iddData.txt");
	}

	if(ish != NULL)
	{
		for(int i = 0; i < inh.FileHeader.NumberOfSections; i++)
		{
			strcpy(buf, getSectionName(i));
			strcat(buf, "Section.txt");

			remove(buf);

			strcpy(buf, getSectionName(i));
			strcat(buf, "SectionInfo.txt");
			remove(buf);
		}
		free(ish);
		ish = NULL;
	}

	if(iid != NULL)
	{
		free(iid);
		iid = NULL;
	}

	if(iibn != NULL)
	{
		free(iibn);
		iibn = NULL;
	}

	if(idata != NULL)
	{
		free(idata);
		idata = NULL;
	}

	if(in != NULL)
	{
		fclose(in);
		in = NULL;
	}
}

/*
class에서 가지고 있는 file의 경로 및 file 이름을 다시 설정하는 함수
*/
void convert::reOpenFile(char* dirPath)
{
	char* ptr;
	char* path;
	int j = 0;

	path = (char *)malloc(sizeof(char) * strlen(dirPath) + 1);
	strcpy(path, dirPath);

	if(in != NULL)
		fclose(in);

	if( (in = fopen(dirPath, "rb")) == NULL)
	{
		fputs("File open failed\n", stderr);
		exit(1);
	}

	ptr = strtok(path, "\\");

	//사용자가 입력한 경로에서 실행파일의 이름을 가져옴
	while(ptr != NULL)
	{
		if(strstr(ptr, ".exe") != NULL)
		{
			ex_name = (char *)malloc(sizeof(char) * strlen(ptr) + 1);
			strcpy(ex_name, strtok(ptr, ".exe"));
			break;
		}

		ptr = strtok(NULL, "\\");
	}

	allData = fopen("wholeEXEData.txt", "w");
	saveWholeBinaryFile();
	free(path);
}

/*
binary file을 assembly language로 변환하기 위해 필요한 함수들을 한 번에 실행하는 함수
*/
int convert::doConvert()
{
	int returnValue = 0;

	saveWholeBinaryFile();
	doParseImageDosHeader();
	doSkip();

	returnValue = doParseImageNTHeader();

	//doParseImageNTHeader의 결과 값이 0이 아닐 경우 결과 값을 return
	if(returnValue == -1)
	{
		return -1;
	}

	else if(returnValue == -2)
	{
		return -2;
	}

	else if(returnValue == -3)
	{
		return -3;
	}

	else if(returnValue == -99)
	{
		return -99;
	}

	findIATData();
	separateSection();

	returnValue = doConvertHexToAsm();

	//doConvertHexToAsm()의 결과 값이 0이 아닐 경우 결과 값을 return
	if(returnValue == -3)
	{
		return -3;
	}
	else if(returnValue == -4)
	{
		return -4;
	}

	return returnValue;
}

/*
convert class의 소멸자
*/
convert::~convert()
{
	clear();

	free(ex_name);
	mongoc_client_destroy(client_convert);
}

/*
Bianry file의 처음부터 끝까지 binary file의 offset과
aw data, raw data에 해당하는 값을 파일로 저장하는 함수
*/
void convert::saveWholeBinaryFile()
{
	unsigned char buf[17];
	char offset[16];
	int i = 0;
	int len;

	while(1)
	{
		sprintf(offset, "%08X	", ftell(in));
		len = fread(buf, 1, 16, in);

		if(len <= 0)
			break;

		if(fputs(offset, allData) != EOF)
		{
			for(i = 0; i < 16; i++)
			{
				fprintf(allData, "%02X ", buf[i]);
			}

			fprintf(allData, "	");

			for(i = 0; i < 16; i++)
			{
				if(buf[i] >= 0x20 && buf[i] <= 0x7E)
					fprintf(allData, "%c ", buf[i]);
				else
					fprintf(allData, ". ");
			}
			fprintf(allData, "\n");
		}
	}

	fseek(in, 0, SEEK_SET);
	fclose(allData);
}

/*
parseImageDosHedaer함수
binary file에서 IMAGE_DOS_HEADER를 parsing 하는 함수	
*/
void convert::parseImageDosHeader(FILE* file, imageDosHeader* idh)
{
	/*
	@Parameter
	idh	: IMAGE_DOS_HAEDER의 정보를 갖는 class member 변수

	@Local Variable
	IMAGE_DOS_HEADER의 정보를 file로 저장하기 위해 사용
	*/
	FILE* idhData = fopen("idhData.txt", "w");
	char buf[1024];
	int startOffset;
	int long0, long1, long2, long3;

	startOffset = ftell(file);
	idh->e_magic = fgetc(file) << 8;
	idh->e_magic = idh->e_magic | fgetc(file);
	sprintf(buf, "%08X	%04X		Signature		WORD	e_magic		Magic DOS signature MZ\n", startOffset, idh->e_magic);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_cblp = fgetc(file);
	idh->e_cblp = idh->e_cblp | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Extra bytes		WORD	e_cblp		Bytes on last page of file\n", startOffset, idh->e_cblp);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_cp = fgetc(file);
	idh->e_cp = idh->e_cp | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Pages			WORD	e_cp		Page in file\n", startOffset, idh->e_cp);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_crlc = fgetc(file);
	idh->e_crlc = idh->e_crlc | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Relocation items	WORD	e_crlc		Relocations\n", startOffset, idh->e_crlc);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_cparhdr = fgetc(file);
	idh->e_cparhdr = idh->e_cparhdr | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Header Size		WORD	e_cparhdr	Size of header in paragraphs\n", startOffset, idh->e_cparhdr);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_minalloc = fgetc(file);
	idh->e_minalloc = idh->e_minalloc | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Minimum allocation	WORD	e_minalloc	Minimum extra paragraphs needed\n", startOffset, idh->e_minalloc);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_maxalloc = fgetc(file);
	idh->e_maxalloc = idh->e_maxalloc | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Maximum allocation	WORD	e_maxalloc	Minimum extra paragraphs needed\n", startOffset, idh->e_maxalloc);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_ss = fgetc(file);
	idh->e_ss = idh->e_ss | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Initial SS		WORD	e_ss		Initial (relative) SS Value\n", startOffset, idh->e_ss);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_sp = fgetc(file);
	idh->e_sp = idh->e_sp | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Initial SP		WORD	e_sp		Initial SP Value\n", startOffset, idh->e_sp);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_csum = fgetc(file);
	idh->e_csum = idh->e_csum | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Checksum		WORD	e_csum		Checksum\n", startOffset, idh->e_csum);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_ip = fgetc(file);
	idh->e_ip = idh->e_ip | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Initial IP		WORD	e_ip		Initial IP Value\n", startOffset, idh->e_ip);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_cs = fgetc(file);
	idh->e_cs = idh->e_cs | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Initial CS		WORD	e_cs		Initial (relative) CS Value\n", startOffset, idh->e_cs);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_lfarlc = fgetc(file);
	idh->e_lfarlc = idh->e_lfarlc | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Relocation table	WORD	e_lfarlc	File address of relocation table\n", startOffset, idh->e_lfarlc);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_ovno = fgetc(file);
	idh->e_ovno = idh->e_ovno | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		Overlay number		WORD	e_ovno		Overlay number\n", startOffset, idh->e_ovno);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res[0] = fgetc(file) << 8;
	idh->e_res[0] = idh->e_res[0] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res[0]	Reserved words\n", startOffset, idh->e_res[0]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res[1] = fgetc(file) << 8;
	idh->e_res[1] = idh->e_res[1] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res[1]	Reserved words\n", startOffset, idh->e_res[1]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res[2] = fgetc(file) << 8;
	idh->e_res[2] = idh->e_res[2] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res[2]	Reserved words\n", startOffset, idh->e_res[2]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res[3] = fgetc(file) << 8;
	idh->e_res[3] = idh->e_res[3] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res[3]	Reserved words\n", startOffset, idh->e_res[3]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_oemid = fgetc(file);
	idh->e_oemid = idh->e_oemid | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		OEM Indtifier		WORD	e_oemid		OEN identifier (for e_oeminfo)\n", startOffset, idh->e_oemid);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_oeminfo = fgetc(file);
	idh->e_oeminfo = idh->e_oeminfo | fgetc(file) << 8;
	sprintf(buf, "%08X	%04X		OEM Information		WORD	e_oeminfo	OEN information\n", startOffset, idh->e_oeminfo);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[0] = fgetc(file) << 8;
	idh->e_res2[0] = idh->e_res2[0] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[0]	Reserved words\n", startOffset, idh->e_res2[0]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[1] = fgetc(file) << 8;
	idh->e_res2[1] = idh->e_res2[1] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[1]	Reserved words\n", startOffset, idh->e_res2[1]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[2] = fgetc(file) << 8;
	idh->e_res2[2] = idh->e_res2[2] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[2]	Reserved words\n", startOffset, idh->e_res2[2]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[3] = fgetc(file) << 8;
	idh->e_res2[3] = idh->e_res2[3] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[3]	Reserved words\n", startOffset, idh->e_res2[3]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[4] = fgetc(file) << 8;
	idh->e_res2[4] = idh->e_res2[4] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[4]	Reserved words\n", startOffset, idh->e_res2[4]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[5] = fgetc(file) << 8;
	idh->e_res2[5] = idh->e_res2[5] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[5]	Reserved words\n", startOffset, idh->e_res2[5]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[6] = fgetc(file) << 8;
	idh->e_res2[6] = idh->e_res2[6] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[6]	Reserved words\n", startOffset, idh->e_res2[6]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[7] = fgetc(file) << 8;
	idh->e_res2[7] = idh->e_res2[7] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[7]	Reserved words\n", startOffset, idh->e_res2[7]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[8] = fgetc(file) << 8;
	idh->e_res2[8] = idh->e_res2[8] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[8]	Reserved words\n", startOffset, idh->e_res2[8]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	idh->e_res2[9] = fgetc(file) << 8;
	idh->e_res2[9] = idh->e_res2[9] | fgetc(file);
	sprintf(buf, "%08X	%04X		Reserved		WORD	e_res2[9]	Reserved words\n", startOffset, idh->e_res2[9]);
	fputs(buf, idhData);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idh->e_lfanew = long3 << 24;
	idh->e_lfanew = idh->e_lfanew | long2 << 16;
	idh->e_lfanew = idh->e_lfanew | long1 << 8;
	idh->e_lfanew = idh->e_lfanew | long0;

	sprintf(buf, "%08X	%08X	Offset PE Header	DWORD	e_lfanew	Offset to Start of PE header", startOffset, idh->e_lfanew);
	fputs(buf, idhData);

	fclose(idhData);
}
/*
parseImageNTHeader함수
현재 64비트 imageNTHeader는 지원되지 않는다.
ImageNTHeader를 parse하는 함수

return 
1	: parseImageNTHeader 함수 내부에서 file open에 실패 한 경우
0	: binary file에서 IMAGE_NT_HEADER 정보를 읽어오는데 성공할 경우
-1	: binary file이 잘 못된 section name을 가질 경우
-2	: binary file이 packing된 경우
*/
int convert::parseImageNTHeader(FILE* file, imageNtHeader32* inh, imageNtHeader64* inh64)
{
	/*
	@Parameter
	inh		:	32비트 IMAGE_NT_HEADER정보를 저장하는 class memeber 변수
	inh64	:	64비트 IMAGE_NT_HEADER정보를 저장하는 class memeber 변수 (현재 사용되지 않음)

	@Local Variable

	*/
	unsigned char buf[17];
	char offset[16];
	int i = 0;
	int len;
	int lastNTHeaderOffset;
	char dataBuf[512];
	int startOffset;

	FILE* entry;
	FILE* inhOverView = fopen("inhOverView.txt", "w");

	FILE* inhSign = fopen("inhSignature.txt", "w");
	startOffset = ftell(file);
	inh->Signature = fgetc(file) << 24;
	inh->Signature = inh->Signature | fgetc(file) << 16;
	inh->Signature = inh->Signature | fgetc(file) << 8;
	inh->Signature = inh->Signature | fgetc(file);
	sprintf(dataBuf, "%08X	%08X	Signature	DWORD	PE Signature\n", startOffset, inh->Signature);
	fputs(dataBuf, inhSign);
	fclose(inhSign);

	//parseImageFileHeader의 결과값으로 binary file의 architecture 판단
	if( parseImageFileHeader(file, &(inh->FileHeader)) == 64)
	{
		inh64->Signature = inh->Signature;
		inh64->FileHeader = inh->FileHeader;
		parseImageOptionalHeader64(file, &(inh64->OptionalHeader));
		return -99;
	}

	else
	{
		parseImageOptionalHeader32(file, &(inh->OptionalHeader));

		if( (entry = fopen("mainAddress.txt", "w")) == NULL)
		{
			printf("main address file open failed\n");
			return 1;
		}

		fprintf(entry, "%08X\n", inh->OptionalHeader.AddressOfEntryPoint);
		fclose(entry);

		lastNTHeaderOffset = ftell(file);

		fseek(file, idh.e_lfanew, SEEK_SET);

		while(1)
		{
			sprintf(offset, "%08X	", ftell(file));
			len = fread(buf, 1, 16, file);

			if(ftell(file) >= lastNTHeaderOffset)
			{
				if(ftell(file) - lastNTHeaderOffset > 0)
				{
					fputs(offset, inhOverView);

					for(i = 0; i < ftell(file) - lastNTHeaderOffset; i++)
					{
						fprintf(inhOverView, "%02X ", buf[i]);
					}

					len = ((16 - i)/2) - 1;
					for(i = 0; i < len; i++)
					{
						fprintf(inhOverView, "	");
					}
				}
				fprintf(inhOverView, "	");

				for(i = 0; i < ftell(file) - lastNTHeaderOffset; i++)
				{
					if(buf[i] >= 0x20 && buf[i] <= 0x7E)
						fprintf(inhOverView, "%c ", buf[i]);
					else
						fprintf(inhOverView, ". ");
				}
				fprintf(inhOverView, "\n");
				break;
			}

			fputs(offset, inhOverView);
			for(i = 0; i < 16; i++)
			{
				fprintf(inhOverView, "%02X ", buf[i]);
			}

			fprintf(inhOverView, "	");

			for(i = 0; i < 16; i++)
			{
				if(buf[i] >= 0x20 && buf[i] <= 0x7E)
					fprintf(inhOverView, "%c ", buf[i]);
				else
					fprintf(inhOverView, ". ");
			}
			fprintf(inhOverView, "\n");
		}

		fclose(inhOverView);
		fseek(file, lastNTHeaderOffset, SEEK_SET);
		return 32;
	}

}
/*
parseImageFileHeader함수
ImageFileHeader를 parse하는 함수.

return
64	: binary file의 architecture가 64비트
32	: binary file의 architecture가 32비트
*/
int convert::parseImageFileHeader(FILE* file, imageFileHeader* ifh)
{
	/*
	@Parameter 
	ifh	: IMAGE_FILE_HEADER의 정보를 갖는 class member 변수

	@Local Variable
	chkMachine	: binary file의 architecture 정보를 저장하는 변수
	*/
	FILE* ifhData = fopen("ifhData.txt", "w");
	char buf[1024];
	int startOffset;

	int temp, chkMachine;
	int long0, long1, long2, long3;

	startOffset = ftell(file);
	temp = fgetc(file);
	chkMachine = fgetc(file) << 8;
	chkMachine = chkMachine | temp;

	ifh->Machine = chkMachine;
	sprintf(buf, "%08X	%04X		WORD	Machine				Machine\n", startOffset, ifh->Machine);
	fputs(buf, ifhData);

	startOffset = ftell(file);
	temp = fgetc(file);
	ifh->NumberOfSections = fgetc(file) << 8;
	ifh->NumberOfSections = ifh->NumberOfSections | temp;
	sprintf(buf, "%08X	%04X		WORD	NumberOfSections		NumberOfSections\n", startOffset, ifh->NumberOfSections);
	fputs(buf, ifhData);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ifh->TimeDateStamp = long3 << 24;
	ifh->TimeDateStamp = ifh->TimeDateStamp | long2 << 16;
	ifh->TimeDateStamp = ifh->TimeDateStamp | long1 << 8;
	ifh->TimeDateStamp = ifh->TimeDateStamp | long0;

	sprintf(buf, "%08X	%08X	DWORD	TimeDateStamp			Date & time image was create by the linker\n", startOffset, ifh->TimeDateStamp);
	fputs(buf, ifhData);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ifh->PointerToSymbolTable = long3 << 24;
	ifh->PointerToSymbolTable = ifh->PointerToSymbolTable | long2 << 16;
	ifh->PointerToSymbolTable = ifh->PointerToSymbolTable | long1 << 8;
	ifh->PointerToSymbolTable = ifh->PointerToSymbolTable | long0;

	sprintf(buf, "%08X	%08X	DWORD	PointerToSymbolTable		Zero or offset of COFF symbol table in older files\n", startOffset, ifh->PointerToSymbolTable);
	fputs(buf, ifhData);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ifh->NumberOfSymbols = long3 << 24;
	ifh->NumberOfSymbols = ifh->NumberOfSymbols | long2 << 16;
	ifh->NumberOfSymbols = ifh->NumberOfSymbols | long1 << 8;
	ifh->NumberOfSymbols = ifh->NumberOfSymbols | long0;

	sprintf(buf, "%08X	%08X	DWORD	NumberOfSymbols			Number of symbols in COFF symbol table\n", startOffset, ifh->NumberOfSymbols);
	fputs(buf, ifhData);

	startOffset = ftell(file);
	temp = fgetc(file);
	ifh->SizeOfOptionalHeader = fgetc(file) << 8;
	ifh->SizeOfOptionalHeader = ifh->SizeOfOptionalHeader | temp;

	sprintf(buf, "%08X	%04X		WORD	SizeOfOptionalHeader		Size of optional header in bytes\n", startOffset, ifh->SizeOfOptionalHeader);
	fputs(buf, ifhData);

	startOffset = ftell(file);
	temp = fgetc(file);
	ifh->Characteristics = fgetc(file) << 8;
	ifh->Characteristics = ifh->Characteristics | temp;

	sprintf(buf, "%08X	%04X		WORD	Characteristics			Characteristics\n", startOffset, ifh->Characteristics);
	fputs(buf, ifhData);

	fclose(ifhData);

	if(chkMachine == 0x8664 || chkMachine == 0x0200)
		return 64;
	else
		return 32;
}
/*
parseImageOptionalHeader32함수
32비트 IMAGE_OPTIONAL_HEADER 구조체를 parse하는 함수
*/
void convert::parseImageOptionalHeader32(FILE* file, imageOptionalHeader32* ioh)
{
	/*
	@Parameter
	ioh	: IMAGE_OPTIONAL_HEADER의 정보를 갖는 class memeber 변수
	*/
	int temp;
	int long0, long1, long2, long3;

	FILE* iohFile = fopen("iohData.txt", "w");

	char buf[1024];
	int startOffset;

	startOffset = ftell(file);
	temp = fgetc(file);
	ioh->Magic = fgetc(file) << 8;
	ioh->Magic = ioh->Magic | temp;
	sprintf(buf, "%08X	%04X		WORD	Magic				Excutable file's architecture\n", startOffset, ioh->Magic);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	ioh->MajorLinkerVersion = fgetc(file);
	sprintf(buf, "%08X	%02X		BYTE	MajorLinkerVersion		Major version number of the linker\n", startOffset, ioh->MajorLinkerVersion);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	ioh->MinorLinkerVersion = fgetc(file);
	sprintf(buf, "%08X	%02X		BYTE	MinorLinkerVersion		Minor version number of the linker\n", startOffset, ioh->MinorLinkerVersion);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SizeOfCode = long3 << 24;
	ioh->SizeOfCode = ioh->SizeOfCode | long2 << 16;
	ioh->SizeOfCode = ioh->SizeOfCode | long1 << 8;
	ioh->SizeOfCode = ioh->SizeOfCode | long0;
	sprintf(buf, "%08X	%08X	DWORD	SizeOfCode			Size of code section or sum if multiple code sections\n", startOffset, ioh->SizeOfCode);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SizeOfInitializedData = long3 << 24;
	ioh->SizeOfInitializedData = ioh->SizeOfInitializedData | long2 << 16;
	ioh->SizeOfInitializedData = ioh->SizeOfInitializedData | long1 << 8;
	ioh->SizeOfInitializedData = ioh->SizeOfInitializedData | long0;
	sprintf(buf, "%08X	%08X	DWORD	SizeOfInitializedData		Size of initialized data section\n", startOffset, ioh->SizeOfInitializedData);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SizeOfUninitializedData = long3 << 24;
	ioh->SizeOfUninitializedData = ioh->SizeOfUninitializedData | long2 << 16;
	ioh->SizeOfUninitializedData = ioh->SizeOfUninitializedData | long1 << 8;
	ioh->SizeOfUninitializedData = ioh->SizeOfUninitializedData | long0;
	sprintf(buf, "%08X	%08X	DWORD	SizeOfUninitializedData		Size of uninitialized data section\n", startOffset, ioh->SizeOfUninitializedData);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->AddressOfEntryPoint = long3 << 24;
	ioh->AddressOfEntryPoint = ioh->AddressOfEntryPoint | long2 << 16;
	ioh->AddressOfEntryPoint = ioh->AddressOfEntryPoint | long1 << 8;
	ioh->AddressOfEntryPoint = ioh->AddressOfEntryPoint | long0;
	sprintf(buf, "%08X	%08X	DWORD	AddressOfEntryPoint		Start of code execution, optional for DLLs, zero when non present\n", startOffset, ioh->AddressOfEntryPoint);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->BaseOfCode = long3 << 24;
	ioh->BaseOfCode = ioh->BaseOfCode | long2 << 16;
	ioh->BaseOfCode = ioh->BaseOfCode | long1 << 8;
	ioh->BaseOfCode = ioh->BaseOfCode | long0;
	sprintf(buf, "%08X	%08X	DWORD	BaseOfCode			RVA of first byte of code when loaded into RAM\n", startOffset, ioh->BaseOfCode);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->BaseOfData = long3 << 24;
	ioh->BaseOfData = ioh->BaseOfData | long2 << 16;
	ioh->BaseOfData = ioh->BaseOfData | long1 << 8;
	ioh->BaseOfData = ioh->BaseOfData | long0;
	sprintf(buf, "%08X	%08X	DWORD	BaseOfData			RVA of first byte of data when loaded into RAM\n", startOffset, ioh->BaseOfData);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->ImageBase = long3 << 24;
	ioh->ImageBase = ioh->ImageBase | long2 << 16;
	ioh->ImageBase = ioh->ImageBase | long1 << 8;
	ioh->ImageBase = ioh->ImageBase | long0;
	sprintf(buf, "%08X	%08X	DWORD	ImageBase			Prefferd load address\n", startOffset, ioh->ImageBase);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SectionAlignment = long3 << 24;
	ioh->SectionAlignment = ioh->SectionAlignment | long2 << 16;
	ioh->SectionAlignment = ioh->SectionAlignment | long1 << 8;
	ioh->SectionAlignment = ioh->SectionAlignment | long0;
	sprintf(buf, "%08X	%08X	DWORD	SectionAlignment		Alignment of sections when loaded in RAM\n", startOffset, ioh->SectionAlignment);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->FileAlignment = long3 << 24;
	ioh->FileAlignment = ioh->FileAlignment | long2 << 16;
	ioh->FileAlignment = ioh->FileAlignment | long1 << 8;
	ioh->FileAlignment = ioh->FileAlignment | long0;
	sprintf(buf, "%08X	%08X	DWORD	FileAlignment			Alignment of sections in file on disk\n", startOffset, ioh->FileAlignment);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	temp = fgetc(file);
	ioh->MajorOperatingSystemVersion = fgetc(file);
	ioh->MajorOperatingSystemVersion = ioh->MajorOperatingSystemVersion | temp;
	sprintf(buf, "%08X	%04X		WORD	MajorOperatingSystemVersion	Major version no. of required oepration\n", startOffset, ioh->MajorOperatingSystemVersion);
	fputs(buf, iohFile);


	startOffset = ftell(file);
	temp = fgetc(file);
	ioh->MinorOperatingSystemVersion = fgetc(file);
	ioh->MinorOperatingSystemVersion = ioh->MinorOperatingSystemVersion | temp;
	sprintf(buf, "%08X	%04X		WORD	MinorOperatingSystemVersion	Minor version no. of required oepration\n", startOffset, ioh->MinorOperatingSystemVersion);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	temp = fgetc(file);
	ioh->MajorImageVersion = fgetc(file);
	ioh->MajorImageVersion = ioh->MajorImageVersion | temp;
	sprintf(buf, "%08X	%04X		WORD	MajorImageVersion		Major version number of the image\n", startOffset, ioh->MajorImageVersion);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	temp = fgetc(file);
	ioh->MinorImageVersion = fgetc(file);
	ioh->MinorImageVersion = ioh->MinorImageVersion | temp;
	sprintf(buf, "%08X	%04X		WORD	MinorImageVersion		Minor version number of the image\n", startOffset, ioh->MajorImageVersion);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	temp = fgetc(file);
	ioh->MajorSubsystemVersion = fgetc(file);
	ioh->MajorSubsystemVersion = ioh->MajorSubsystemVersion | temp;
	sprintf(buf, "%08X	%04X		WORD	MajorSubsystemVersion		Major version number of the subsystem\n", startOffset, ioh->MajorSubsystemVersion);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	temp = fgetc(file);
	ioh->MinorSubsystemVersion = fgetc(file);
	ioh->MinorSubsystemVersion = ioh->MinorSubsystemVersion | temp;
	sprintf(buf, "%08X	%04X		WORD	MinorSubsystemVersion		Minor version number of the subsystem", startOffset, ioh->MinorSubsystemVersion);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->Win32VersionValue = long3 << 24;
	ioh->Win32VersionValue = ioh->Win32VersionValue | long2 << 16;
	ioh->Win32VersionValue = ioh->Win32VersionValue | long1 << 8;
	ioh->Win32VersionValue = ioh->Win32VersionValue | long0;
	sprintf(buf, "%08X	%08X	DWORD	Win32VersionValue		32bit Windows version value\n", startOffset, ioh->Win32VersionValue);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SizeOfImage = long3 << 24;
	ioh->SizeOfImage = ioh->SizeOfImage | long2 << 16;
	ioh->SizeOfImage = ioh->SizeOfImage | long1 << 8;
	ioh->SizeOfImage = ioh->SizeOfImage | long0;
	sprintf(buf, "%08X	%08X	DWORD	SizeOfImage			Amount of memory allocated by loader for image. Must be a multiple of SectionAlignment\n", startOffset, ioh->SizeOfImage);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SizeOfHeaders = long3 << 24;
	ioh->SizeOfHeaders = ioh->SizeOfHeaders | long2 << 16;
	ioh->SizeOfHeaders = ioh->SizeOfHeaders | long1 << 8;
	ioh->SizeOfHeaders = ioh->SizeOfHeaders | long0;
	sprintf(buf, "%08X	%08X	DWORD	SizeOfHeaders			Offset of first section, multiple of FileAlignment\n", startOffset, ioh->SizeOfHeaders);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->CheckSum = long3 << 24;
	ioh->CheckSum = ioh->CheckSum | long2 << 16;
	ioh->CheckSum = ioh->CheckSum | long1 << 8;
	ioh->CheckSum = ioh->CheckSum | long0;
	sprintf(buf, "%08X	%08X	DWORD	CheckSum			Image checksum (only required for kernel-mode drivers and some system DLLs)\n", startOffset, ioh->CheckSum);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	temp = fgetc(file);
	ioh->Subsystem = fgetc(file) << 8;
	ioh->Subsystem = ioh->Subsystem | temp;
	sprintf(buf, "%08X	%04X		WORD	Subsystem			0002H == Windows GUI, 0003H == console\n", startOffset, ioh->Subsystem);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	temp = fgetc(file);
	ioh->DllCharacteristics = fgetc(file) << 8;
	ioh->DllCharacteristics = ioh->DllCharacteristics | temp;
	sprintf(buf, "%08X	%04X		WORD	DllCharacteristics		DllCharacteristics\n", startOffset, ioh->DllCharacteristics);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SizeOfStackReserve = long3 << 24;
	ioh->SizeOfStackReserve = ioh->SizeOfStackReserve | long2 << 16;
	ioh->SizeOfStackReserve = ioh->SizeOfStackReserve | long1 << 8;
	ioh->SizeOfStackReserve = ioh->SizeOfStackReserve | long0;
	sprintf(buf, "%08X	%08X	DWORD	SizeOfStackReserve		Number of bytes reserved for the stack\n", startOffset, ioh->SizeOfStackReserve);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SizeOfStackCommit = long3 << 24;
	ioh->SizeOfStackCommit = ioh->SizeOfStackCommit | long2 << 16;
	ioh->SizeOfStackCommit = ioh->SizeOfStackCommit | long1 << 8;
	ioh->SizeOfStackCommit = ioh->SizeOfStackCommit | long0;
	sprintf(buf, "%08X	%08X	DWORD	SizeOfStackCommit		Number of bytes actually used for the stack\n", startOffset, ioh->SizeOfStackCommit);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SizeOfHeapReserve = long3 << 24;
	ioh->SizeOfHeapReserve = ioh->SizeOfHeapReserve | long2 << 16;
	ioh->SizeOfHeapReserve = ioh->SizeOfHeapReserve | long1 << 8;
	ioh->SizeOfHeapReserve = ioh->SizeOfHeapReserve | long0;
	sprintf(buf, "%08X	%08X	DWORD	SizeOfHeapReserve		Number of bytes to reserve for the local heap\n", startOffset, ioh->SizeOfHeapReserve);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->SizeOfHeapCommit = long3 << 24;
	ioh->SizeOfHeapCommit = ioh->SizeOfHeapCommit | long2 << 16;
	ioh->SizeOfHeapCommit = ioh->SizeOfHeapCommit | long1 << 8;
	ioh->SizeOfHeapCommit = ioh->SizeOfHeapCommit | long0;
	sprintf(buf, "%08X	%08X	DWORD	SizeOfHeapCommit		Number of bytes actually used for local heap\n", startOffset, ioh->SizeOfHeapCommit);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->LoaderFlags = long3 << 24;
	ioh->LoaderFlags = ioh->LoaderFlags | long2 << 16;
	ioh->LoaderFlags = ioh->LoaderFlags | long1 << 8;
	ioh->LoaderFlags = ioh->LoaderFlags | long0;
	sprintf(buf, "%08X	%08X	DWORD	LoaderFlags			This member is obsolete\n", startOffset, ioh->LoaderFlags);
	fputs(buf, iohFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh->NumberOfRvaAndSizes = long3 << 24;
	ioh->NumberOfRvaAndSizes = ioh->NumberOfRvaAndSizes | long2 << 16;
	ioh->NumberOfRvaAndSizes = ioh->NumberOfRvaAndSizes | long1 << 8;
	ioh->NumberOfRvaAndSizes = ioh->NumberOfRvaAndSizes | long0;
	sprintf(buf, "%08X	%08X	DWORD	NumberOfRvaAndSizes		Number of directory entries\n", startOffset, ioh->NumberOfRvaAndSizes);
	fputs(buf, iohFile);

	fclose(iohFile);
	parseImageDataDirectory(file, ioh->DataDirectory);
}
/*
parseImageOptionalHeader64함수
64비트 IMAGE_OPTIONAL_HEADER를 parse하는 함수
*/
void convert::parseImageOptionalHeader64(FILE* file, imageOptionalHeader64* ioh64)
{
	int temp;
	int long0, long1, long2, long3, long4, long5, long6, long7;

	temp = fgetc(file);
	ioh64->Magic = fgetc(file) << 8;
	ioh64->Magic = ioh64->Magic | temp;	

	ioh64->MajorLinkerVersion = fgetc(file);
	ioh64->MinorLinkerVersion = fgetc(file);

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->SizeOfCode = long3 << 24;
	ioh64->SizeOfCode = ioh64->SizeOfCode | long2 << 16;
	ioh64->SizeOfCode = ioh64->SizeOfCode | long1 << 8;
	ioh64->SizeOfCode = ioh64->SizeOfCode | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->SizeOfInitializedData = long3 << 24;
	ioh64->SizeOfInitializedData = ioh64->SizeOfInitializedData | long2 << 16;
	ioh64->SizeOfInitializedData = ioh64->SizeOfInitializedData | long1 << 8;
	ioh64->SizeOfInitializedData = ioh64->SizeOfInitializedData | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->SizeOfUninitializedData = long3 << 24;
	ioh64->SizeOfUninitializedData = ioh64->SizeOfUninitializedData | long2 << 16;
	ioh64->SizeOfUninitializedData = ioh64->SizeOfUninitializedData | long1 << 8;
	ioh64->SizeOfUninitializedData = ioh64->SizeOfUninitializedData | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->AddressOfEntryPoint = long3 << 24;
	ioh64->AddressOfEntryPoint = ioh64->AddressOfEntryPoint | long2 << 16;
	ioh64->AddressOfEntryPoint = ioh64->AddressOfEntryPoint | long1 << 8;
	ioh64->AddressOfEntryPoint = ioh64->AddressOfEntryPoint | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->BaseOfCode = long3 << 24;
	ioh64->BaseOfCode = ioh64->BaseOfCode | long2 << 16;
	ioh64->BaseOfCode = ioh64->BaseOfCode | long1 << 8;
	ioh64->BaseOfCode = ioh64->BaseOfCode | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);
	long4 = fgetc(file);
	long5 = fgetc(file);
	long6 = fgetc(file);
	long7 = fgetc(file);

	ioh64->ImageBase = long7 << 56;
	ioh64->ImageBase = ioh64->ImageBase | long6 << 48;
	ioh64->ImageBase = ioh64->ImageBase | long5 << 40;
	ioh64->ImageBase = ioh64->ImageBase | long4 << 32;
	ioh64->ImageBase = ioh64->ImageBase | long3 << 24;
	ioh64->ImageBase = ioh64->ImageBase | long2 << 16;
	ioh64->ImageBase = ioh64->ImageBase | long1 << 8;
	ioh64->ImageBase = ioh64->ImageBase | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->SectionAlignment = long3 << 24;
	ioh64->SectionAlignment = ioh64->SectionAlignment | long2 << 16;
	ioh64->SectionAlignment = ioh64->SectionAlignment | long1 << 8;
	ioh64->SectionAlignment = ioh64->SectionAlignment | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->FileAlignment = long3 << 24;
	ioh64->FileAlignment = ioh64->FileAlignment | long2 << 16;
	ioh64->FileAlignment = ioh64->FileAlignment | long1 << 8;
	ioh64->FileAlignment = ioh64->FileAlignment | long0;

	temp = fgetc(file);
	ioh64->MajorOperatingSystemVersion = fgetc(file);
	ioh64->MajorOperatingSystemVersion = ioh64->MajorOperatingSystemVersion | temp;

	temp = fgetc(file);
	ioh64->MinorOperatingSystemVersion = fgetc(file);
	ioh64->MinorOperatingSystemVersion = ioh64->MinorOperatingSystemVersion | temp;

	temp = fgetc(file);
	ioh64->MajorImageVersion = fgetc(file);
	ioh64->MajorImageVersion = ioh64->MajorImageVersion | temp;

	temp = fgetc(file);
	ioh64->MinorImageVersion = fgetc(file);
	ioh64->MinorImageVersion = ioh64->MinorImageVersion | temp;

	temp = fgetc(file);
	ioh64->MajorSubsystemVersion = fgetc(file);
	ioh64->MajorSubsystemVersion = ioh64->MajorSubsystemVersion | temp;

	temp = fgetc(file);
	ioh64->MinorSubsystemVersion = fgetc(file);
	ioh64->MinorSubsystemVersion = ioh64->MinorSubsystemVersion | temp;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->Win32VersionValue = long3 << 24;
	ioh64->Win32VersionValue = ioh64->Win32VersionValue | long2 << 16;
	ioh64->Win32VersionValue = ioh64->Win32VersionValue | long1 << 8;
	ioh64->Win32VersionValue = ioh64->Win32VersionValue | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->SizeOfImage = long3 << 24;
	ioh64->SizeOfImage = ioh64->SizeOfImage | long2 << 16;
	ioh64->SizeOfImage = ioh64->SizeOfImage | long1 << 8;
	ioh64->SizeOfImage = ioh64->SizeOfImage | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->SizeOfHeaders = long3 << 24;
	ioh64->SizeOfHeaders = ioh64->SizeOfHeaders | long2 << 16;
	ioh64->SizeOfHeaders = ioh64->SizeOfHeaders | long1 << 8;
	ioh64->SizeOfHeaders = ioh64->SizeOfHeaders | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->CheckSum = long3 << 24;
	ioh64->CheckSum = ioh64->CheckSum | long2 << 16;
	ioh64->CheckSum = ioh64->CheckSum | long1 << 8;
	ioh64->CheckSum = ioh64->CheckSum | long0;

	temp = fgetc(file);
	ioh64->Subsystem = fgetc(file) << 8;
	ioh64->Subsystem = ioh64->Subsystem | temp;

	temp = fgetc(file);
	ioh64->DllCharacteristics = fgetc(file) << 8;
	ioh64->DllCharacteristics = ioh64->DllCharacteristics | temp;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);
	long4 = fgetc(file);
	long5 = fgetc(file);
	long6 = fgetc(file);
	long7 = fgetc(file);

	ioh64->SizeOfStackReserve = long7 << 56;
	ioh64->SizeOfStackReserve = ioh64->SizeOfStackReserve | long6 << 48;
	ioh64->SizeOfStackReserve = ioh64->SizeOfStackReserve | long5 << 40;
	ioh64->SizeOfStackReserve = ioh64->SizeOfStackReserve | long4 << 32;
	ioh64->SizeOfStackReserve = ioh64->SizeOfStackReserve | long3 << 24;
	ioh64->SizeOfStackReserve = ioh64->SizeOfStackReserve | long2 << 16;
	ioh64->SizeOfStackReserve = ioh64->SizeOfStackReserve | long1 << 8;
	ioh64->SizeOfStackReserve = ioh64->SizeOfStackReserve | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);
	long4 = fgetc(file);
	long5 = fgetc(file);
	long6 = fgetc(file);
	long7 = fgetc(file);

	ioh64->SizeOfStackCommit = long7 << 56;
	ioh64->SizeOfStackCommit = ioh64->SizeOfStackCommit | long6 << 48;
	ioh64->SizeOfStackCommit = ioh64->SizeOfStackCommit | long5 << 40;
	ioh64->SizeOfStackCommit = ioh64->SizeOfStackCommit | long4 << 32;
	ioh64->SizeOfStackCommit = ioh64->SizeOfStackCommit | long3 << 24;
	ioh64->SizeOfStackCommit = ioh64->SizeOfStackCommit | long2 << 16;
	ioh64->SizeOfStackCommit = ioh64->SizeOfStackCommit | long1 << 8;
	ioh64->SizeOfStackCommit = ioh64->SizeOfStackCommit | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);
	long4 = fgetc(file);
	long5 = fgetc(file);
	long6 = fgetc(file);
	long7 = fgetc(file);

	ioh64->SizeOfHeapReserve = long7 << 56;
	ioh64->SizeOfHeapReserve = ioh64->SizeOfHeapReserve | long6 << 48;
	ioh64->SizeOfHeapReserve = ioh64->SizeOfHeapReserve | long5 << 40;
	ioh64->SizeOfHeapReserve = ioh64->SizeOfHeapReserve | long4 << 32;
	ioh64->SizeOfHeapReserve = ioh64->SizeOfHeapReserve | long3 << 24;
	ioh64->SizeOfHeapReserve = ioh64->SizeOfHeapReserve | long2 << 16;
	ioh64->SizeOfHeapReserve = ioh64->SizeOfHeapReserve | long1 << 8;
	ioh64->SizeOfHeapReserve = ioh64->SizeOfHeapReserve | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);
	long4 = fgetc(file);
	long5 = fgetc(file);
	long6 = fgetc(file);
	long7 = fgetc(file);

	ioh64->SizeOfHeapCommit = long7 << 56;
	ioh64->SizeOfHeapCommit = ioh64->SizeOfHeapCommit | long6 << 48;
	ioh64->SizeOfHeapCommit = ioh64->SizeOfHeapCommit | long5 << 40;
	ioh64->SizeOfHeapCommit = ioh64->SizeOfHeapCommit | long4 << 32;
	ioh64->SizeOfHeapCommit = ioh64->SizeOfHeapCommit | long3 << 24;
	ioh64->SizeOfHeapCommit = ioh64->SizeOfHeapCommit | long2 << 16;
	ioh64->SizeOfHeapCommit = ioh64->SizeOfHeapCommit | long1 << 8;
	ioh64->SizeOfHeapCommit = ioh64->SizeOfHeapCommit | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->LoaderFlags = long3 << 24;
	ioh64->LoaderFlags = ioh64->LoaderFlags | long2 << 16;
	ioh64->LoaderFlags = ioh64->LoaderFlags | long1 << 8;
	ioh64->LoaderFlags = ioh64->LoaderFlags | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ioh64->NumberOfRvaAndSizes = long3 << 24;
	ioh64->NumberOfRvaAndSizes = ioh64->NumberOfRvaAndSizes | long2 << 16;
	ioh64->NumberOfRvaAndSizes = ioh64->NumberOfRvaAndSizes | long1 << 8;
	ioh64->NumberOfRvaAndSizes = ioh64->NumberOfRvaAndSizes | long0;

	parseImageDataDirectory(file, ioh64->DataDirectory);
}
/*
parseImageDataDirectory함수
IMAGE_DATA_DIRECTORY 구조체를 parse하는 함수
*/
void convert::parseImageDataDirectory(FILE* file, imageDataDirectory* idd)
{
	/*
	@Parameter
	idd	:	IMAGE_DATA_DIRECTORY의 값을 저장하는 class memeber 변수
	*/
	int long0, long1, long2, long3;

	FILE* iddFile = fopen("iddData.txt", "w");

	char buf[1024];
	int startOffset;

	//Export Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[0].VirtualAddress = long3 << 24;
	idd[0].VirtualAddress = idd[0].VirtualAddress | long2 << 16;
	idd[0].VirtualAddress = idd[0].VirtualAddress | long1 << 8;
	idd[0].VirtualAddress = idd[0].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_0	RVA of Export Directory\n", startOffset, idd[0].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[0].Size = long3 << 24;
	idd[0].Size = idd[0].Size | long2 << 16;
	idd[0].Size = idd[0].Size | long1 << 8;
	idd[0].Size = idd[0].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_0	size of Export Directory\n", startOffset, idd[0].Size);
	fputs(buf, iddFile);

	//Import Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[1].VirtualAddress = long3 << 24;
	idd[1].VirtualAddress = idd[1].VirtualAddress | long2 << 16;
	idd[1].VirtualAddress = idd[1].VirtualAddress | long1 << 8;
	idd[1].VirtualAddress = idd[1].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_1	RVA of Import Directory\n", startOffset, idd[1].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[1].Size = long3 << 24;
	idd[1].Size = idd[1].Size | long2 << 16;
	idd[1].Size = idd[1].Size | long1 << 8;
	idd[1].Size = idd[1].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_1	size of Import Directory\n", startOffset, idd[1].Size);
	fputs(buf, iddFile);

	//Resource Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[2].VirtualAddress = long3 << 24;
	idd[2].VirtualAddress = idd[2].VirtualAddress | long2 << 16;
	idd[2].VirtualAddress = idd[2].VirtualAddress | long1 << 8;
	idd[2].VirtualAddress = idd[2].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_2	RVA of Resource Directory\n", startOffset, idd[2].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[2].Size = long3 << 24;
	idd[2].Size = idd[2].Size | long2 << 16;
	idd[2].Size = idd[2].Size | long1 << 8;
	idd[2].Size = idd[2].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_2	size of Resource Directory\n", startOffset, idd[2].Size);
	fputs(buf, iddFile);

	//Exception Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[3].VirtualAddress = long3 << 24;
	idd[3].VirtualAddress = idd[3].VirtualAddress | long2 << 16;
	idd[3].VirtualAddress = idd[3].VirtualAddress | long1 << 8;
	idd[3].VirtualAddress = idd[3].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_3	RVA of Exception Directory\n", startOffset, idd[3].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[3].Size = long3 << 24;
	idd[3].Size = idd[3].Size | long2 << 16;
	idd[3].Size = idd[3].Size | long1 << 8;
	idd[3].Size = idd[3].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_3	size of Exception Directory\n", startOffset, idd[3].Size);
	fputs(buf, iddFile);

	//Security Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[4].VirtualAddress = long3 << 24;
	idd[4].VirtualAddress = idd[4].VirtualAddress | long2 << 16;
	idd[4].VirtualAddress = idd[4].VirtualAddress | long1 << 8;
	idd[4].VirtualAddress = idd[4].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_4	Raw Offset of Security Directory\n", startOffset, idd[4].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[4].Size = long3 << 24;
	idd[4].Size = idd[4].Size | long2 << 16;
	idd[4].Size = idd[4].Size | long1 << 8;
	idd[4].Size = idd[4].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_4	size of Security Directory\n", startOffset, idd[4].Size);
	fputs(buf, iddFile);

	//Basereloc Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[5].VirtualAddress = long3 << 24;
	idd[5].VirtualAddress = idd[5].VirtualAddress | long2 << 16;
	idd[5].VirtualAddress = idd[5].VirtualAddress | long1 << 8;
	idd[5].VirtualAddress = idd[5].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_5	RVA of Base Relocation Directory\n", startOffset, idd[5].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[5].Size = long3 << 24;
	idd[5].Size = idd[5].Size | long2 << 16;
	idd[5].Size = idd[5].Size | long1 << 8;
	idd[5].Size = idd[5].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_5	size of Base Relocation Directory\n", startOffset, idd[5].Size);
	fputs(buf, iddFile);

	//Debug Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[6].VirtualAddress = long3 << 24;
	idd[6].VirtualAddress = idd[6].VirtualAddress | long2 << 16;
	idd[6].VirtualAddress = idd[6].VirtualAddress | long1 << 8;
	idd[6].VirtualAddress = idd[6].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_6	RVA of Debug Directory\n", startOffset, idd[6].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[6].Size = long3 << 24;
	idd[6].Size = idd[6].Size | long2 << 16;
	idd[6].Size = idd[6].Size | long1 << 8;
	idd[6].Size = idd[6].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_6	size of Debug Directory\n", startOffset, idd[6].Size);
	fputs(buf, iddFile);

	//Copyright Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[7].VirtualAddress = long3 << 24;
	idd[7].VirtualAddress = idd[7].VirtualAddress | long2 << 16;
	idd[7].VirtualAddress = idd[7].VirtualAddress | long1 << 8;
	idd[7].VirtualAddress = idd[7].VirtualAddress | long0;

	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_7	RVA of Copyright Note\n", startOffset, idd[7].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[7].Size = long3 << 24;
	idd[7].Size = idd[7].Size | long2 << 16;
	idd[7].Size = idd[7].Size | long1 << 8;
	idd[7].Size = idd[7].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_7	size of Copyright Note\n", startOffset, idd[7].Size);
	fputs(buf, iddFile);

	//Globalptr Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[8].VirtualAddress = long3 << 24;
	idd[8].VirtualAddress = idd[8].VirtualAddress | long2 << 16;
	idd[8].VirtualAddress = idd[8].VirtualAddress | long1 << 8;
	idd[8].VirtualAddress = idd[8].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_8	RVA to be used as Global Pointer\n", startOffset, idd[8].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[8].Size = long3 << 24;
	idd[8].Size = idd[8].Size | long2 << 16;
	idd[8].Size = idd[8].Size | long1 << 8;
	idd[8].Size = idd[8].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_8	not used\n", startOffset, idd[8].Size);
	fputs(buf, iddFile);

	//TLS Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[9].VirtualAddress = long3 << 24;
	idd[9].VirtualAddress = idd[9].VirtualAddress | long2 << 16;
	idd[9].VirtualAddress = idd[9].VirtualAddress | long1 << 8;
	idd[9].VirtualAddress = idd[9].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_9	RVA of Thread Local Storage Directory\n", startOffset, idd[9].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[9].Size = long3 << 24;
	idd[9].Size = idd[9].Size | long2 << 16;
	idd[9].Size = idd[9].Size | long1 << 8;
	idd[9].Size = idd[9].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_9	size of Thread Local Storage Directory\n", startOffset, idd[9].Size);
	fputs(buf, iddFile);

	//Load_config Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[10].VirtualAddress = long3 << 24;
	idd[10].VirtualAddress = idd[10].VirtualAddress | long2 << 16;
	idd[10].VirtualAddress = idd[10].VirtualAddress | long1 << 8;
	idd[10].VirtualAddress = idd[10].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_10	RVA of Load Configuration Directory\n", startOffset, idd[10].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[10].Size = long3 << 24;
	idd[10].Size = idd[10].Size | long2 << 16;
	idd[10].Size = idd[10].Size | long1 << 8;
	idd[10].Size = idd[10].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_10	size of Load Configuration Directory\n", startOffset, idd[10].Size);
	fputs(buf, iddFile);

	//Bound_import Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[11].VirtualAddress = long3 << 24;
	idd[11].VirtualAddress = idd[11].VirtualAddress | long2 << 16;
	idd[11].VirtualAddress = idd[11].VirtualAddress | long1 << 8;
	idd[11].VirtualAddress = idd[11].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_11	RVA of Bound Import Directory\n", startOffset, idd[11].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[11].Size = long3 << 24;
	idd[11].Size = idd[11].Size | long2 << 16;
	idd[11].Size = idd[11].Size | long1 << 8;
	idd[11].Size = idd[11].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_11	size of Bound Import Directory\n", startOffset, idd[11].Size);
	fputs(buf, iddFile);

	//IAT Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[12].VirtualAddress = long3 << 24;
	idd[12].VirtualAddress = idd[12].VirtualAddress | long2 << 16;
	idd[12].VirtualAddress = idd[12].VirtualAddress | long1 << 8;
	idd[12].VirtualAddress = idd[12].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_12	RVA of Import Address Table\n", startOffset, idd[12].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[12].Size = long3 << 24;
	idd[12].Size = idd[12].Size | long2 << 16;
	idd[12].Size = idd[12].Size | long1 << 8;
	idd[12].Size = idd[12].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_12	size of Import Address Table\n", startOffset, idd[12].Size);
	fputs(buf, iddFile);

	//Delay_import Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[13].VirtualAddress = long3 << 24;
	idd[13].VirtualAddress = idd[13].VirtualAddress | long2 << 16;
	idd[13].VirtualAddress = idd[13].VirtualAddress | long1 << 8;
	idd[13].VirtualAddress = idd[13].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_13	RVA of Delay Import Directory\n", startOffset, idd[13].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[13].Size = long3 << 24;
	idd[13].Size = idd[13].Size | long2 << 16;
	idd[13].Size = idd[13].Size | long1 << 8;
	idd[13].Size = idd[13].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_13	size of Delay Import Directory\n", startOffset, idd[13].Size);
	fputs(buf, iddFile);

	//Com_descriptor Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[14].VirtualAddress = long3 << 24;
	idd[14].VirtualAddress = idd[14].VirtualAddress | long2 << 16;
	idd[14].VirtualAddress = idd[14].VirtualAddress | long1 << 8;
	idd[14].VirtualAddress = idd[14].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_14	RVA of COM descriptor Directory\n", startOffset, idd[14].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[14].Size = long3 << 24;
	idd[14].Size = idd[14].Size | long2 << 16;
	idd[14].Size = idd[14].Size | long1 << 8;
	idd[14].Size = idd[14].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_14	size of COM descriptor Directory\n", startOffset, idd[14].Size);
	fputs(buf, iddFile);

	//Reserved Directory
	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[15].VirtualAddress = long3 << 24;
	idd[15].VirtualAddress = idd[15].VirtualAddress | long2 << 16;
	idd[15].VirtualAddress = idd[15].VirtualAddress | long1 << 8;
	idd[15].VirtualAddress = idd[15].VirtualAddress | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTORY_15	Reserved\n", startOffset, idd[15].VirtualAddress);
	fputs(buf, iddFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	idd[15].Size = long3 << 24;
	idd[15].Size = idd[15].Size | long2 << 16;
	idd[15].Size = idd[15].Size | long1 << 8;
	idd[15].Size = idd[15].Size | long0;
	sprintf(buf, "%08X	%08X	DWORD	IMAGE_DATA_DIRECTROY_15	Reserved\n", startOffset, idd[15].Size);
	fputs(buf, iddFile);
	fclose(iddFile);
}
/*
parseImageSectionHeader함수
IMAGE_SECTION_HEADER 구조체를 parse하는 함수

return
0	: IMAGE_SECTION_HEADER의 parsing이 성공한 경우
-1	: binray file이 잘못된 section 이름을 가질 경우
-2	: binray file이 packing된 경우
*/
int convert::parseImageSectionHeader(FILE* file, imageSectionHeader* ish)
{
	/*
	@Parameter
	ish	: IMAGE_SECTION_HEADER의 정보를 갖는 class member 변수
	*/
	unsigned char temp;
	unsigned char long0, long1, long2, long3;
	FILE* ishFile;
	char buf[1024];
	char fileName[128];
	int startOffset;

	startOffset = ftell(file);
	ish->Name[0] = fgetc(file);
	ish->Name[1] = fgetc(file);
	ish->Name[2] = fgetc(file);
	ish->Name[3] = fgetc(file);
	ish->Name[4] = fgetc(file);
	ish->Name[5] = fgetc(file);
	ish->Name[6] = fgetc(file);
	ish->Name[7] = fgetc(file);

	//section 이름이 잘 못된 경우 -1 return
	if(ish->Name[7] != 0x00)
	{
		return -1;
	}

	strcpy(fileName, (char*)ish->Name);
	strcat(fileName, "SectionInfo.txt");

	ishFile = fopen(fileName, "w");
	sprintf(buf, "%08X	8BYTE	%02X %02X %02X %02X	NAME				%s\n", startOffset, ish->Name[0], ish->Name[1], ish->Name[2], ish->Name[3], ish->Name);
	fputs(buf, ishFile);
	sprintf(buf, "%08X		%02X %02X %02X %02X	\n", startOffset + 4, ish->Name[4], ish->Name[5], ish->Name[6], ish->Name[7]);
	fputs(buf, ishFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ish->Misc.PhysicalAddress = long3 << 24;
	ish->Misc.PhysicalAddress = ish->Misc.PhysicalAddress | long2 << 16;
	ish->Misc.PhysicalAddress = ish->Misc.PhysicalAddress | long1 << 8;
	ish->Misc.PhysicalAddress = ish->Misc.PhysicalAddress | long0;
	sprintf(buf, "%08X	DWORD	%08X	VirtualSize			Actual size of data in section\n", startOffset, ish->Misc.PhysicalAddress);
	fputs(buf, ishFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ish->VirtualAddress = long3 << 24;
	ish->VirtualAddress = ish->VirtualAddress | long2 << 16;
	ish->VirtualAddress = ish->VirtualAddress | long1 << 8;
	ish->VirtualAddress = ish->VirtualAddress | long0;
	sprintf(buf, "%08X	DWORD	%08X	VirtualAddress			RVA where section begins in memory\n", startOffset, ish->Misc.PhysicalAddress);
	fputs(buf, ishFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ish->SizeOfRawData = long3 << 24;
	ish->SizeOfRawData = ish->SizeOfRawData | long2 << 16;
	ish->SizeOfRawData = ish->SizeOfRawData | long1 << 8;
	ish->SizeOfRawData = ish->SizeOfRawData | long0;
	sprintf(buf, "%08X	DWORD	%08X	SizeOfRawData			Size of data on disk\n", startOffset, ish->SizeOfRawData);
	fputs(buf, ishFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ish->PointerToRawData = long3 << 24;
	ish->PointerToRawData = ish->PointerToRawData | long2 << 16;
	ish->PointerToRawData = ish->PointerToRawData | long1 << 8;
	ish->PointerToRawData = ish->PointerToRawData | long0;
	sprintf(buf, "%08X	DWORD	%08X	PointerToRawData		Raw offset of section on disk\n", startOffset, ish->PointerToRawData);
	fputs(buf, ishFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ish->PointerToRelocations = long3 << 24;
	ish->PointerToRelocations = ish->PointerToRelocations | long2 << 16;
	ish->PointerToRelocations = ish->PointerToRelocations | long1 << 8;
	ish->PointerToRelocations = ish->PointerToRelocations | long0;
	sprintf(buf, "%08X	DWORD	%08X	PointerToRelocations		Start of relocation entries for section, zero if none\n", startOffset, ish->PointerToRelocations);
	fputs(buf, ishFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ish->PointerToLinenumbers = long3 << 24;
	ish->PointerToLinenumbers = ish->PointerToLinenumbers | long2 << 16;
	ish->PointerToLinenumbers = ish->PointerToLinenumbers | long1 << 8;
	ish->PointerToLinenumbers = ish->PointerToLinenumbers | long0;
	sprintf(buf, "%08X	DWORD	%08X	PointerToLinenumbers		Start of lin-no. entries for section, zero if none\n", startOffset, ish->PointerToLinenumbers);
	fputs(buf, ishFile);

	startOffset = ftell(file);
	temp = fgetc(file);
	ish->NumberOfRelocations = fgetc(file) << 8;
	ish->NumberOfRelocations = ish->NumberOfRelocations | temp;
	sprintf(buf, "%08X	WORD	%04X		NumberOfRelocations		This value is zero for excutable images\n", startOffset, ish->NumberOfRelocations);
	fputs(buf, ishFile);

	startOffset = ftell(file);
	temp = fgetc(file);
	ish->NumberOfLinenumbers = fgetc(file) << 8;
	ish->NumberOfLinenumbers = ish->NumberOfLinenumbers | temp;
	sprintf(buf, "%08X	WORD	%04X		NumberOfLinenumbers		Number of line-number entries for section\n", startOffset, ish->NumberOfLinenumbers);
	fputs(buf, ishFile);

	startOffset = ftell(file);
	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	ish->Characteristics = long3 << 24;
	ish->Characteristics = ish->Characteristics | long2 << 16;
	ish->Characteristics = ish->Characteristics | long1 << 8;
	ish->Characteristics = ish->Characteristics | long0;
	sprintf(buf, "%08X	DWORD	%08X	Characteristics			Section Characteristics\n", startOffset, ish->Characteristics);
	fputs(buf, ishFile);

	fclose(ishFile);

	//section 이름으로 packing 여부를 판단하여 packing된 경우 -2 return
	if(strstr((char*)ish->Name, "UPX") != NULL || strstr((char*)ish->Name, "aspack") != NULL || strstr((char*)ish->Name, "pec") != NULL)
	{
		return -2;
	}

	else
	{
		return 0;
	}
}
/*
parseImageImportDescriptor함수
IMAGE_IMPORT_DESCRIPTOR 구조체를 parse하는 함수
*/
void convert::parseImageImportDescriptor(FILE* file, imageImportDescriptor* iid)
{
	/*
	@Parameter
	iid	: IMAGE_IMPORT_DESCRIPTOR 의 정보를 갖는 class member 변수
	*/
	unsigned char long0, long1, long2, long3;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	iid->dummyUnionName.OriginalFirstThunk = long3 << 24;
	iid->dummyUnionName.OriginalFirstThunk = iid->dummyUnionName.OriginalFirstThunk | long2 << 16;
	iid->dummyUnionName.OriginalFirstThunk = iid->dummyUnionName.OriginalFirstThunk | long1 << 8;
	iid->dummyUnionName.OriginalFirstThunk = iid->dummyUnionName.OriginalFirstThunk | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	iid->TimeDateStamp = long3 << 24;
	iid->TimeDateStamp = iid->TimeDateStamp | long2 << 16;
	iid->TimeDateStamp = iid->TimeDateStamp | long1 << 8;
	iid->TimeDateStamp = iid->TimeDateStamp | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	iid->ForwarderChain = long3 << 24;
	iid->ForwarderChain = iid->ForwarderChain | long2 << 16;
	iid->ForwarderChain = iid->ForwarderChain | long1 << 8;
	iid->ForwarderChain = iid->ForwarderChain | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	iid->Name = long3 << 24;
	iid->Name = iid->Name | long2 << 16;
	iid->Name = iid->Name | long1 << 8;
	iid->Name = iid->Name | long0;

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	iid->FirstThunk = long3 << 24;
	iid->FirstThunk = iid->FirstThunk | long2 << 16;
	iid->FirstThunk = iid->FirstThunk | long1 << 8;
	iid->FirstThunk = iid->FirstThunk | long0;
}
/*
parseImageImportByName함수
IMAGE_IMPORT_BY_NAME 구조체를 parse하는 함수.
*/
void convert::parseImageImportByName(FILE* file, imageImportByName* iibn, iatData* idata)
{
	/*
	@Parameter
	iibn	: IMAGE_IMPORT_BY_NAME의 정보를 갖는 class member 변수
	idata	: IMAGE_IMPORT_BY_NAME의 함수 이름을 갖는 class member 변수
	*/
	char temp;
	unsigned char long0, long1;
	int index = 1;
	int tmp;

	tmp = ftell(file);

	long0 = fgetc(file);
	long1 = fgetc(file);

	tmp = ftell(file);

	iibn->Hint = long1 << 8;
	iibn->Hint = iibn->Hint | long0;

	temp = fgetc(file);

	tmp = ftell(file);

	iibn->Name[0] = temp;
	idata->funcName[0] = temp;

	while((temp = fgetc(file)) != 0) {
		idata->funcName[index++] = temp;
	}
	idata->funcName[index] = '\0';
}
/*
parseIATData함수
IATData를 parse하는 함수.
IMAGE_NT_HEADER, IMAGE_SECTION_HEADER 를 사용하여 parse한다.
*/
void convert::parseIATData(FILE *file, imageNtHeader32 *inh, imageSectionHeader *ish, long dllAddr, iatData* idata)
{
	/*
	@Parameter
	inh		: IAT DATA를 읽어오기 위해 필요한 IMAGE_NT_HEADER 정보를 갖는 class member 변수
	ish		: IAT DATA를 읽어오기 위해 필요한 IMAGE_SECTION_HEADER 정보를 갖는 class memeber 변수
	dllAddr	: dll의 주소값
	idata	: dll의 이름을 갖는 class memeber 변수
	*/
	char ch;
	int index = 0;

	//RVA 주소를 RAW 주소로 변환하여 file offset의 위치를 바꾸고 해당 위치에서 데이터를 읽어온다.
	if(fseek(file, convertRVAToRAW(inh, ish, dllAddr), SEEK_SET) == -1)
	{
		printf("Offset setting failed\n");
		return;
	}

	while((ch = fgetc(file)) != 0) {
		idata->dllName[index++] = ch;
	}
	idata->dllName[index] = '\0';
}
/*
countImageImportByName함수
IMAGE_NT_HEADER, IMAGE_SECTION_HEADER, IMAGE_IMPORT_DESCRIPTOR 구조체를 이용하여 count하는 함수

return
count	: IMAGE_IMPORT_BY_NAME의 개수
*/
short convert::countImageImportByName(FILE* file, imageNtHeader32 *inh, imageSectionHeader *ish, imageImportDescriptor* iid)
{
	/*
	@Parameter
	inh		: IMAGE_IMPORT_BY_NAME의 개수를 알기 위해 필요한 IMAGE_NT_HEADER 정보를 갖는 class member 변수
	ish		: IMAGE_IMPORT_BY_NAME의 개수를 알기 위해 필요한 IMAGE_SECTION_HEADER 정보를 갖는 class memeber 변수
	iid		: IMAGE_IMPORT_BY_NAME의 개수를 알기 위해 필요한 IMAGE_IMPOTR_DESCRIPTOR 정보를 갖는 class memeber 변수
	*/
	long temp;
	unsigned char long0, long1, long2, long3;
	short count = 0;

	if(fseek(file, convertRVAToRAW(inh, ish, iid->dummyUnionName.OriginalFirstThunk), SEEK_SET) == -1)
	{
		printf("Offset setting failed\n");
		return -1;
	}

	while(1) {
		long0 = fgetc(file);
		long1 = fgetc(file);
		long2 = fgetc(file);
		long3 = fgetc(file);

		temp = long3 << 24;
		temp = temp | long2 << 16;
		temp = temp | long1 << 8;
		temp = temp | long0;

		if(temp == 0)
			break;

		count++;
	}

	return count;
}

/*
addr에 저장된 value로 file offset을 변환하는 함수
*/
void convert::fseekAddrToValue(FILE *file, imageNtHeader32 *inh, imageSectionHeader *ish, long addr) 
{
	long temp;
	unsigned char long0, long1, long2, long3;

	if(fseek(file, convertRVAToRAW(inh, ish, addr), SEEK_SET) == -1)
	{	
		return;
	}

	long0 = fgetc(file);
	long1 = fgetc(file);
	long2 = fgetc(file);
	long3 = fgetc(file);

	temp = long3 << 24;
	temp = temp | long2 << 16;
	temp = temp | long1 << 8;
	temp = temp | long0;

	if(fseek(file, convertRVAToRAW(inh, ish, temp), SEEK_SET) == -1)
	{
		return;
	}
}

/*
convertRVAToRAW함수
RVA형식을 RAW형식으로 바꿔주는 함수
*/
long convert::convertRVAToRAW(imageNtHeader32 *inh, imageSectionHeader *ish, unsigned int addr_RVA)
{
	unsigned int addr_RAW;
	int numOfSection = inh->FileHeader.NumberOfSections;

	for(int i = 1; i <= numOfSection; i++) {
		if(addr_RVA >= ish[i-1].VirtualAddress && addr_RVA < ish[i].VirtualAddress)
		{
			if(addr_RVA >= ish[i-1].VirtualAddress + ish[i-1].Misc.VirtualSize) //null area error
			{				
				return -1;
			}

			addr_RAW = addr_RVA - ish[i-1].VirtualAddress + ish[i-1].PointerToRawData;

			if(addr_RAW >= ish[i-1].PointerToRawData && addr_RAW < ish[i].PointerToRawData)
			{
				if(addr_RAW >= ish[i-1].PointerToRawData + ish[i-1].SizeOfRawData)
				{					
					return -1;
				}

				return addr_RAW;
			}
			break;
		}
	}

	return -1;
}

/*
convertHexToAsm함수
Hex를 Assembly로 바꿔주는 함수

return
0
-4
-5
*/
int convert::convertHexToAsm(FILE* file, imageSectionHeader codeIsh, imageSectionHeader rdataIsh, imageSectionHeader dataIsh, iatData* idata, int iibnSize)
{
	/*
	@Parameter
	codeIsh		: code(text) section 정보를 갖고 있는 class member 변수
	rdataIsh	: rdata section 정보를 갖고 있는 class member 변수
	dataIsh		: data section 정보를 갖고 있는 class member 변수
	idata		: dll, function 정보를 갖고 있는 class membe 변수
	iibnSzie	: import하여 사용되는 function의 개수
	*/
	unsigned char opData;
	unsigned char opTemp1, opTemp2, opTemp3;
	char orData;
	char temp0, temp1, temp2, temp3;
	int temp, rawAddr;

	int numOfOpr1, numOfOpr2;

	int fileOffset = 0;
	int offsetFlag = 0;

	int nullCount = 0;

	int newOperation = 0;

	char dir[128];
	char inputString[1024];
	char bufTemp[1024];
	char iatString[512];
	char dsString[512];

	int index = 0;

	FILE* output = fopen("convert.txt", "w");

	iatData iatData;
	int iatFound = 0;

	int offsetTemp;
	unsigned int i;

	int switchTableFlag = 0;

	//binary file의 offset을 code(text)영역의 시작주소로 설정
	if(fseek(file, codeIsh.PointerToRawData, SEEK_SET) == -1)
	{
		return -4;
	}

	//code(text) 영역의 시작주소부터 code 영역의 크기까지 루프를 반복
	while((unsigned int)ftell(file) < codeIsh.PointerToRawData + codeIsh.SizeOfRawData)
	{
		memset(inputString, 0, 1024);
		memset(bufTemp, 0, 1024);
		memset(dsString, 0, 100);

		if(newOperation == 0)
		{
			opData = fgetc(file);

			if(opData == 0x00)
			{
				nullCount++;

				//code영역이 끝나기 전에 code가 끝날 경우 file close & return
				if(nullCount >= 8)
				{
					fclose(output);
					output = NULL;
					return 0;
				}

				continue;
			}

			else
			{
				nullCount = 0;
			}			

			opTemp1 = fgetc(file);
			opTemp2 = fgetc(file);
			opTemp3 = fgetc(file);

			//operation을 비교하기 전에 swtich table의 값인지 판단
			for(i = (ish[0].PointerToRawData >> 12); i <= (ish[0].SizeOfRawData) >> 12; i++)
			{
				if(opTemp3 != 0x00)
					break;

				else if((((opTemp3 << 16 | opTemp2 << 8 | opTemp1) & 0x0000FFF0) >> 4) == ( ( (inh.OptionalHeader.ImageBase & 0x00FFF000 ) >> 12) + i) )
				{
					fprintf(output, "%08X Switch Table %08X\n", ftell(file) - 4, (opTemp3 << 24)| (0x00 << 16) | (opTemp1 << 8) | opData);
					switchTableFlag = 1;
					break;
				}				
			}			

			if(switchTableFlag == 1)
			{
				switchTableFlag = 0;
				continue;
			}

			//swtich table이 아니면 file offset을 되돌리고 opData로 operation 비교
			fseek(file, ftell(file) - 3, SEEK_SET);

			sprintf(dir, "0x%08X",  opData);

			//Database에서 operation이 있는지 검사 후 operation이 있으면 operation의 내용을 복사
			if( (find_hex(dir) == -1) )
			{
				newOperation = 1;
				continue;
			}

			strcpy(inputString, find_hex_contents(dir));

			//operation의 내용이 MORE라면 위의 과정을 반복
			if(strcmp(inputString, "MORE") == 0 || strcmp(inputString, "MORE\n") == 0) //2 or upper bytes operation
			{
				opTemp1 = opData;
				opData = fgetc(file);

				if(opTemp1 == 0x64 || opTemp1 == 0x66 || opTemp1 == 0xF2 || opTemp1 == 0xF3)
				{
					sprintf(dir, "%08X",  (opTemp1 << 24) | opData);
				}

				else
				{
					sprintf(dir, "0x%08X",  (opTemp1 << 8) | opData);
				}

				if( (find_hex(dir) == -1) )
				{
					newOperation = 2;
					continue;
				}

				strcpy(inputString, find_hex_contents(dir));

				if(strcmp(inputString, "MORE") == 0 || strcmp(inputString, "MORE\n") == 0) //3 or upper bytes operation
				{
					opTemp2 = opTemp1;
					opTemp1 = opData;
					opData = fgetc(file);

					if(opTemp2 == 0x64 || opTemp2 == 0x66 || opTemp2 == 0xF2 || opTemp2 == 0xF3)
					{
						sprintf(dir, "%08X",  (opTemp2 << 24) | (opTemp1 << 8) | opData);
					}
					else
					{
						sprintf(dir, "0x%08X",  (opTemp2 << 16) | (opTemp1 << 8) | opData);
					}

					if( (find_hex(dir) == -1) )
					{
						newOperation = 3;
						continue;
					}

					strcpy(inputString, find_hex_contents(dir));

					if(strcmp(inputString, "MORE") == 0 || strcmp(inputString, "MORE\n") == 0) //4bytes operation
					{
						opTemp3 = opTemp2;
						opTemp2 = opTemp1;
						opTemp1 = opData;
						opData = fgetc(file);

						if(opTemp3 == 0x64 || opTemp3 == 0x66 || opTemp3 == 0xF2 || opTemp3 == 0xF3)
						{
							sprintf(dir, "%08X",  (opTemp3 << 24) | (opTemp2 << 16) | (opTemp1 << 8) | opData);
						}

						else
						{
							sprintf(dir, "0x%08X",  (opTemp3 << 24) | (opTemp2 << 16) | (opTemp1 << 8) | opData);
						}

						if( (find_hex(dir) == -1) )
						{
							newOperation = 4;
							continue;
						}

						strcpy(inputString, find_hex_contents(dir));

						if( strcmp(inputString, "MORE") == 0 || strcmp(inputString, "MORE\n") == 0)
						{
							newOperation = 5;
							continue;
						}

						else	//4byte operation 처리 루틴
						{
							if(inputString[0] == '0')
							{
								subStr(inputString, inputString, 2, strlen(inputString) -2);

								sprintf(bufTemp, "%08X %s", ftell(file) - 4, inputString);
								fprintf(output, bufTemp);
							}

							else if(inputString[0] == '1')
							{
								if(inputString[2] == '1')
								{
									orData = fgetc(file);

									subStr(inputString, inputString, 4, strlen(inputString) - 4);

									sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
									fprintf(output, bufTemp, orData);
								}
								else if(inputString[2] == '2')
								{
									temp = fgetc(file);
									orData = fgetc(file);

									subStr(inputString, inputString, 4, strlen(inputString) - 4);

									sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
									fprintf(output, bufTemp, (orData << 8) | temp);
								}
								else if(inputString[2] == '4')
								{
									temp0 = fgetc(file);
									temp1 = fgetc(file);
									temp2 = fgetc(file);
									temp3 = fgetc(file);

									subStr(inputString, inputString, 4, strlen(inputString) - 4);

									sprintf(bufTemp, "%08X %s", ftell(file) - 8, inputString);
									fprintf(output, bufTemp, (temp3 << 24) | (temp2 << 16) | (temp1 << 8) | orData);
								}
							}

							else if(inputString[0] == '2')
							{
								if(inputString[2] == '1')
								{
									orData = fgetc(file);
									temp = orData;
									numOfOpr1 = 1;
								}

								else if(inputString[2] == '2')
								{
									temp0 = fgetc(file);
									orData = fgetc(file);

									temp = (orData << 8) | temp0;

									numOfOpr1 = 2;
								}

								else if(inputString[2] == '4')
								{
									temp0 = fgetc(file);
									temp1 = fgetc(file);
									temp2 = fgetc(file);
									temp3 = fgetc(file);

									temp = (temp3 << 24) | (temp2 << 16) | (temp1 << 8) | temp0;

									numOfOpr1 = 4;
								}

								if(inputString[4] == '1')
								{
									orData = fgetc(file);

									numOfOpr2 = 1;
								}

								else if(inputString[4] == '2')
								{
									temp0 = fgetc(file);
									temp1 = fgetc(file);

									orData = (temp1 << 8) | temp0;

									numOfOpr2 = 2;
								}

								else if(inputString[4] == '4')
								{
									temp0 = fgetc(file);
									temp1 = fgetc(file);
									temp2 = fgetc(file);
									temp3 = fgetc(file);

									orData = (temp3 << 24) | (temp2 << 16) | (temp1 << 8) | temp0;

									numOfOpr2 = 4;
								}

								subStr(inputString, inputString, 6, strlen(inputString) - 6);

								sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
								fprintf(output, bufTemp, temp, orData);
							}
						}
					}

					else	//3bytes operation 처리 루틴
					{
						if(inputString[0] == '0')
						{
							subStr(inputString, inputString, 2, strlen(inputString) - 2);

							sprintf(bufTemp, "%08X %s", ftell(file) - 3, inputString);
							fprintf(output, bufTemp);
						}

						else if(inputString[0] == '1')
						{
							if(inputString[2] == '1')
							{
								orData = fgetc(file);

								subStr(inputString, inputString, 4, strlen(inputString) - 4);

								sprintf(bufTemp, "%08X %s", ftell(file) - 4, inputString);
								fprintf(output, bufTemp, orData);

							}
							else if(inputString[2] == '2')
							{
								temp = fgetc(file);
								orData = fgetc(file);

								subStr(inputString, inputString, 4, strlen(inputString) - 4);

								sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
								fprintf(output, bufTemp, (orData << 8) | temp);
							}
							else if(inputString[2] == '4')
							{
								temp0 = fgetc(file);
								temp1 = fgetc(file);
								temp2 = fgetc(file);
								temp3 = fgetc(file);

								subStr(inputString, inputString, 4, strlen(inputString) - 4);

								temp = 0;
								temp = ((temp3 << 24) & 0xFF000000) | ((temp2 << 16) & 0x00FF0000) | ((temp1 << 8) & 0x0000FF00) | (temp0 & 0x000000FF);

								if( stringContain(inputString, "DWORD PTR DS:") == 1)
								{
									if( (rdataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
									{
										rawAddr = temp & 0x0000FFFF;

										iatFound = findIAT(idata, &iatData, iibnSize, rawAddr);

										if(iatFound == 1)
										{
											strcpy(iatString, iatData.dllName);
											strcat(iatString, " : ");
											strcat(iatString, iatData.funcName);

											sprintf(bufTemp, "%08X %s", ftell(file) - 7, inputString);
											fprintf(output, bufTemp, temp, iatString);
										}
									}

									else if( (dataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
									{
										offsetTemp = ftell(file);

										if((temp & 0x00FFFF) >= (unsigned)(ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
										{
											return -5;
										}

										fseek(file, temp & 0x00FFFF, SEEK_SET);

										for(i=0; i < 4; i++)
										{
											dsString[i] = fgetc(file);
										}
										dsString[i] = '\0';

										sprintf(bufTemp, "%08X %s", offsetTemp - 7, inputString);
										fprintf(output, bufTemp, temp, dsString);

										fseek(file, offsetTemp, SEEK_SET);
									}

									else
									{
										sprintf(bufTemp, "%08X %s", ftell(file) - 7, inputString);
										fprintf(output, bufTemp, temp, "");
									}
								}

								else if( stringContain(inputString, "WORD PTR DS:") == 1 )
								{
									if( (rdataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
									{
										rawAddr = temp & 0x0000FFFF;

										iatFound = findIAT(idata, &iatData, iibnSize, rawAddr);

										if(iatFound == 1)
										{
											strcpy(iatString, iatData.dllName);
											strcat(iatString, " : ");
											strcat(iatString, iatData.funcName);

											sprintf(bufTemp, "%08X %s", ftell(file) - 7, inputString);
											fprintf(output, bufTemp, temp, iatString);
										}
									}

									else if( (dataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
									{
										offsetTemp = ftell(file);

										if((temp & 0x00FFFF) >= (unsigned)(ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
										{
											return -5;
										}

										fseek(file, temp & 0x00FFFF, SEEK_SET);

										for(i=0; i < 2; i++)
										{
											dsString[i] = fgetc(file);
										}
										dsString[i] = '\0';

										sprintf(bufTemp, "%08X %s", offsetTemp - 7, inputString);
										fprintf(output, bufTemp, temp, dsString);

										fseek(file, offsetTemp, SEEK_SET);
									}

									else
									{
										sprintf(bufTemp, "%08X %s", ftell(file) - 7, inputString);
										fprintf(output, bufTemp, temp, "");
									}
								}

								else if( stringContain(inputString, "BYTE PTR DS:") == 1 )
								{
									if( (rdataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
									{
										rawAddr = temp & 0x0000FFFF;

										iatFound = findIAT(idata, &iatData, iibnSize, rawAddr);

										if(iatFound == 1)
										{
											strcpy(iatString, iatData.dllName);
											strcat(iatString, " : ");
											strcat(iatString, iatData.funcName);

											sprintf(bufTemp, "%08X %s", ftell(file) - 7, inputString);
											fprintf(output, bufTemp, temp, iatString);
										}
									}

									else if( (dataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
									{
										offsetTemp = ftell(file);

										if((temp & 0x00FFFF) >= (unsigned)(ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
										{
											return -5;
										}

										fseek(file, temp & 0x00FFFF, SEEK_SET);

										for(i=0; i < 1; i++)
										{
											dsString[i] = fgetc(file);
										}
										dsString[i] = '\0';

										sprintf(bufTemp, "%08X %s", offsetTemp - 7, inputString);
										fprintf(output, bufTemp, temp, dsString);

										fseek(file, offsetTemp, SEEK_SET);
									}

									else
									{
										sprintf(bufTemp, "%08X %s", ftell(file) - 7, inputString);
										fprintf(output, bufTemp, temp, "");
									}
								}

								else
								{
									temp = ((temp3 << 24) & 0xFF000000) | ((temp2 << 16) & 0x00FF0000) | ((temp1 << 8) & 0x0000FF00) | ((temp0) & 0x000000FF);

									sprintf(bufTemp, "%08X %s", ftell(file) - 7, inputString);
									fprintf(output, bufTemp, temp);
								}
							}
						}

						else if(inputString[0] == '2')
						{
							if(inputString[2] == '1')
							{
								orData = fgetc(file);
								temp = orData;
								numOfOpr1 = 1;
							}

							else if(inputString[2] == '2')
							{
								temp0 = fgetc(file);
								orData = fgetc(file);

								temp = (orData << 8) | temp0;

								numOfOpr1 = 2;
							}

							else if(inputString[2] == '4')
							{
								temp0 = fgetc(file);
								temp1 = fgetc(file);
								temp2 = fgetc(file);
								temp3 = fgetc(file);

								temp = (temp3 << 24) | (temp2 << 16) | (temp1 << 8) | temp0;

								numOfOpr1 = 4;
							}

							if(inputString[4] == '1')
							{
								orData = fgetc(file);

								numOfOpr2 = 1;
							}

							else if(inputString[4] == '2')
							{
								temp0 = fgetc(file);
								temp1 = fgetc(file);

								orData = (temp1 << 8) | temp0;

								numOfOpr2 = 2;
							}

							else if(inputString[4] == '4')
							{
								temp0 = fgetc(file);
								temp1 = fgetc(file);
								temp2 = fgetc(file);
								temp3 = fgetc(file);

								orData = (temp3 << 24) | (temp2 << 16) | (temp1 << 8) | temp0;

								numOfOpr2 = 4;
							}

							subStr(inputString, inputString, 6, strlen(inputString) - 6);

							sprintf(bufTemp, "%08X %s", ftell(file) - 3 - numOfOpr1 - numOfOpr2, inputString);
							fprintf(output, bufTemp, temp, orData);
						}
					}
				}

				else //2bytes operation 처리 루틴
				{
					if(inputString[0] == '0')
					{
						subStr(inputString, inputString, 2, strlen(inputString) - 2);

						sprintf(bufTemp, "%08X %s", ftell(file) - 2, inputString);
						fprintf(output, bufTemp);
					}

					else if(inputString[0] == '1')
					{
						if(inputString[2] == '1')
						{
							orData = fgetc(file);

							subStr(inputString, inputString, 4, strlen(inputString) - 4);

							sprintf(bufTemp, "%08X %s", ftell(file) - 3, inputString);
							fprintf(output, bufTemp, orData);
						}
						else if(inputString[2] == '2')
						{
							temp0 = fgetc(file);
							orData = fgetc(file);

							subStr(inputString, inputString, 4, strlen(inputString) - 4);

							temp = ((orData) & 0x0000FF00) | ((temp0) & 0x000000FF);

							sprintf(bufTemp, "%08X %s", ftell(file) - 4, inputString);
							fprintf(output, bufTemp, temp);
						}
						else if(inputString[2] == '4')
						{
							temp0 = fgetc(file);
							temp1 = fgetc(file);
							temp2 = fgetc(file);
							temp3 = fgetc(file);

							subStr(inputString, inputString, 4, strlen(inputString) - 4);

							temp = 0;
							temp = ((temp3 << 24) & 0xFF000000) | ((temp2 << 16) & 0x00FF0000) | ((temp1 << 8) & 0x0000FF00) | (temp0 & 0x000000FF);

							if( stringContain(inputString, "DWORD PTR DS:") == 1)
							{
								if( ((inh.OptionalHeader.ImageBase | rdataIsh.PointerToRawData) & 0x00FFF000) == (temp & 0x00FFF000) )
								{
									rawAddr = temp & 0x0000FFFF;

									iatFound = findIAT(idata, &iatData, iibnSize, rawAddr);

									if(iatFound == 1)
									{
										strcpy(iatString, iatData.dllName);
										strcat(iatString, " : ");
										strcat(iatString, iatData.funcName);

										sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
										fprintf(output, bufTemp, temp, iatString);
									}
								}

								else if( (((inh.OptionalHeader.ImageBase | dataIsh.PointerToRawData) & 0x00FFF000) <= (temp & 0x00FFF000)) && (((inh.OptionalHeader.ImageBase | (dataIsh.PointerToRawData + dataIsh.SizeOfRawData)) & 0x00FFF000) >= (temp & 0x00FFF000)))
								{
									char chTemp;
									unsigned int forOffset = 0;
									offsetTemp = ftell(file);

									if((temp & 0x00FFFF) >= (ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
									{
										return -5;
									}

									fseek(file, temp & 0x00FFFF, SEEK_SET);

									for(i=0; i < 4 + forOffset; i++)
									{
										chTemp = fgetc(file);

										if(chTemp == 0x0A)
										{
											strcat(dsString, "\\n");
											forOffset = 1;
											i++;
										}
										else if(chTemp > 0x7F)
											dsString[i] = '\0';
										else
											dsString[i] = chTemp;
									}
									dsString[i] = '\0';

									if(stringIsNull(dsString) && strstr(inputString, "("))
									{
										inputString[strlen(inputString) - 6] = '\0';
										strcat(inputString, "\n");

										sprintf(bufTemp, "%08X %s", offsetTemp - 6, inputString);
										fprintf(output, bufTemp, temp);
									}
									else
									{
										if(pointToTextSection(dsString))
										{
											inputString[strlen(inputString) - 6] = '\0';
											strcat(inputString, " (%X)\n");

											sprintf(bufTemp, "%08X %s", offsetTemp - 6, inputString);
											fprintf(output, bufTemp, temp, (((dsString[3] << 24) & 0xFF000000) | ((dsString[2] << 16) & 0x00FF0000) | ((dsString[1] << 8) & 0x0000FF00) | (dsString[0] & 0x000000FF)));
										}

										else
										{
											sprintf(bufTemp, "%08X %s", offsetTemp - 6, inputString);
											fprintf(output, bufTemp, temp, dsString);
										}
									}

									fseek(file, offsetTemp, SEEK_SET);
								}

								else
								{
									sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
									fprintf(output, bufTemp, temp, "");
								}
							}

							else if(stringContain(inputString, "WORD PTR DS:") == 1)
							{
								if( (rdataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
								{
									rawAddr = temp & 0x0000FFFF;

									iatFound = findIAT(idata, &iatData, iibnSize, rawAddr);

									if(iatFound == 1)
									{
										strcpy(iatString, iatData.dllName);
										strcat(iatString, " : ");
										strcat(iatString, iatData.funcName);

										sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
										fprintf(output, bufTemp, temp, iatString);
									}
								}

								else if( (dataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
								{
									offsetTemp = ftell(file);

									if((temp & 0x00FFFF) >= (ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
									{
										return -5;
									}

									fseek(file, temp & 0x00FFFF, SEEK_SET);

									for(i=0; i < 2; i++)
									{
										dsString[i] = fgetc(file);
									}
									dsString[i] = '\0';

									sprintf(bufTemp, "%08X %s", offsetTemp - 6, inputString);
									fprintf(output, bufTemp, temp, dsString);

									fseek(file, offsetTemp, SEEK_SET);
								}

								else
								{
									sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
									fprintf(output, bufTemp, temp, "");
								}
							}

							else if(stringContain(inputString, "BYTE PTR DS:") == 1)
							{
								if( (rdataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
								{
									rawAddr = temp & 0x0000FFFF;

									iatFound = findIAT(idata, &iatData, iibnSize, rawAddr);

									if(iatFound == 1)
									{
										strcpy(iatString, iatData.dllName);
										strcat(iatString, " : ");
										strcat(iatString, iatData.funcName);

										sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
										fprintf(output, bufTemp, temp, iatString);
									}
								}

								else if( (dataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
								{
									offsetTemp = ftell(file);

									if((temp & 0x00FFFF) >= (ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
									{
										return -5;
									}

									fseek(file, temp & 0x00FFFF, SEEK_SET);

									for(i=0; i < 1; i++)
									{
										dsString[i] = fgetc(file);
									}
									dsString[i] = '\0';

									sprintf(bufTemp, "%08X %s", offsetTemp - 6, inputString);
									fprintf(output, bufTemp, temp, dsString);

									fseek(file, offsetTemp, SEEK_SET);
								}

								else
								{
									sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
									fprintf(output, bufTemp, temp, "");
								}
							}

							else if(inputString[0] == 'J')
							{
								sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
								fprintf(output, bufTemp, ftell(file) + temp);
							}

							else
							{
								temp = ((temp3 << 24) & 0xFF000000) | ((temp2 << 16) & 0x00FF0000) | ((temp1 << 8) & 0x0000FF00) | ((temp0) & 0x000000FF);

								sprintf(bufTemp, "%08X %s", ftell(file) - 6, inputString);
								fprintf(output, bufTemp, temp);
							}
						}
					}

					else if(inputString[0] == '2')
					{
						if(inputString[2] == '1')
						{
							orData = fgetc(file);
							temp = orData;

							numOfOpr1 = 1;
						}

						else if(inputString[2] == '2')
						{
							temp0 = fgetc(file);
							orData = fgetc(file);

							temp = (orData << 8) | temp0;

							numOfOpr1 = 2;
						}

						else if(inputString[2] == '4')
						{
							temp0 = fgetc(file);
							temp1 = fgetc(file);
							temp2 = fgetc(file);
							temp3 = fgetc(file);

							temp = ((temp3 << 24) &0xFF000000) | ((temp2 << 16) & 0x00FF0000) | ((temp1 << 8) & 0x0000FF00) | (temp0 & 0x000000FF);

							numOfOpr1 = 4;
						}

						if(inputString[4] == '1')
						{
							orData = fgetc(file);

							numOfOpr2 = 1;
						}

						else if(inputString[4] == '2')
						{
							temp0 = fgetc(file);
							temp1 = fgetc(file);

							orData = (temp1 << 8) | temp0;

							numOfOpr2 = 2;
						}

						else if(inputString[4] == '4')
						{
							temp0 = fgetc(file);
							temp1 = fgetc(file);
							temp2 = fgetc(file);
							temp3 = fgetc(file);

							orData = ((temp3 << 24) &0xFF000000) | ((temp2 << 16) & 0x00FF0000) | ((temp1 << 8) & 0x0000FF00) | (temp0 & 0x000000FF);

							numOfOpr2 = 4;
						}

						subStr(inputString, inputString, 6, strlen(inputString) - 6);

						sprintf(bufTemp, "%08X %s", ftell(file) - 2 - numOfOpr1 - numOfOpr2, inputString);
						fprintf(output, bufTemp, temp, orData);
					}
				}
			}

			else //1byte operation 처리 루틴
			{
				if(inputString[0] == '0')
				{
					subStr(inputString, inputString, 2, strlen(inputString) - 2); 

					sprintf(bufTemp, "%08X %s", ftell(file) - 1, inputString);
					fprintf(output, bufTemp);
				}

				else if(inputString[0] == '1')
				{
					if(inputString[2] == '1')
					{
						orData = fgetc(file);

						subStr(inputString, inputString, 4, strlen(inputString) - 4);

						if(inputString[0] == 'J')
						{
							sprintf(bufTemp, "%08X %s", ftell(file) - 2, inputString);
							fprintf(output, bufTemp, ftell(file) + orData);
						}

						else
						{
							sprintf(bufTemp, "%08X %s", ftell(file) - 2, inputString);
							fprintf(output, bufTemp, orData);
						}
					}
					else if(inputString[2] == '2')
					{
						temp = fgetc(file);
						orData = fgetc(file);

						subStr(inputString, inputString, 4, strlen(inputString) - 4);

						if(inputString[0] == 'J')
						{
							sprintf(bufTemp, "%08X %s", ftell(file) - 3, inputString);
							fprintf(output, bufTemp, ftell(file) + ( (orData << 8) | temp));
						}

						else
						{
							sprintf(bufTemp, "%08X %s", ftell(file) - 3, inputString);
							fprintf(output, bufTemp, (orData << 8) | temp);
						}
					}
					else if(inputString[2] == '4')
					{
						temp0 = fgetc(file);
						temp1 = fgetc(file);
						temp2 = fgetc(file);
						temp3 = fgetc(file);

						subStr(inputString, inputString, 4, strlen(inputString) - 4);

						temp = 0;
						temp = ((temp3 << 24) & 0xFF000000) | ((temp2 << 16) & 0x00FF0000) | ((temp1 << 8) & 0x0000FF00) | (temp0 & 0x000000FF);

						if( stringContain(inputString, "DWORD PTR DS:") == 1)
						{
							if( ((inh.OptionalHeader.ImageBase | rdataIsh.PointerToRawData) & 0x00FFF000) == (temp & 0x00FFF000) )
							{
								rawAddr = temp & 0x0000FFFF;

								iatFound = findIAT(idata, &iatData, iibnSize, rawAddr);

								if(iatFound == 1)
								{
									strcpy(iatString, iatData.dllName);
									strcat(iatString, " : ");
									strcat(iatString, iatData.funcName);

									sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
									fprintf(output, bufTemp, temp, iatString);
								}
							}

							else if( (((inh.OptionalHeader.ImageBase | dataIsh.PointerToRawData) & 0x00FFF000) <= (temp & 0x00FFF000)) && (((inh.OptionalHeader.ImageBase | (dataIsh.PointerToRawData + dataIsh.SizeOfRawData)) & 0x00FFF000) >= (temp & 0x00FFF000)))
							{
								char chTemp;
								unsigned int forOffset = 0;
								offsetTemp = ftell(file);

								if((temp & 0x00FFFF) >= (ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
								{
									return -5;
								}

								fseek(file, temp & 0x00FFFF, SEEK_SET);

								for(i=0; i < 4 + forOffset; i++)
								{
									chTemp = fgetc(file);

									if(chTemp == 0x0A)
									{
										strcat(dsString, "\\n");
										forOffset = 1;
										i++;
									}

									else if(chTemp > 0x7F)
										dsString[i] = '\0';
									else
										dsString[i] = chTemp;
								}
								dsString[i] = '\0';

								if(stringIsNull(dsString) && strstr(inputString, "("))
								{									
									inputString[strlen(inputString) - 6] = '\0';
									strcat(inputString, "\n");

									sprintf(bufTemp, "%08X %s", offsetTemp - 5, inputString);
									fprintf(output, bufTemp, temp);
								}

								else
								{
									if(pointToTextSection(dsString))
									{
										inputString[strlen(inputString) - 6] = '\0';
										strcat(inputString, "(%X)\n");

										sprintf(bufTemp, "%08X %s", offsetTemp - 5, inputString);
										fprintf(output, bufTemp, temp, (((dsString[3] << 24) & 0xFF000000) | ((dsString[2] << 16) & 0x00FF0000) | ((dsString[1] << 8) & 0x0000FF00) | (dsString[0] & 0x000000FF)));
									}

									else
									{
										sprintf(bufTemp, "%08X %s", offsetTemp - 5, inputString);
										fprintf(output, bufTemp, temp, dsString);
									}
								}
								fseek(file, offsetTemp, SEEK_SET);
							}

							else
							{
								sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
								fprintf(output, bufTemp, temp, "");
							}
						}

						else if( stringContain(inputString, "WORD PTR DS:") == 1)
						{
							if( (rdataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
							{
								rawAddr = temp & 0x0000FFFF;

								iatFound = findIAT(idata, &iatData, iibnSize, rawAddr);

								if(iatFound == 1)
								{
									strcpy(iatString, iatData.dllName);
									strcat(iatString, " : ");
									strcat(iatString, iatData.funcName);

									sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
									fprintf(output, bufTemp, temp, iatString);
								}
							}

							else if( (dataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
							{
								offsetTemp = ftell(file);

								if((temp & 0x00FFFF) >= (ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
								{
									return -5;
								}

								fseek(file, temp & 0x00FFFF, SEEK_SET);

								for(i=0; i < 2; i++)
								{
									dsString[i] = fgetc(file);
								}
								dsString[i] = '\0';

								sprintf(bufTemp, "%08X %s", offsetTemp - 5, inputString);
								fprintf(output, bufTemp, temp, dsString);

								fseek(file, offsetTemp, SEEK_SET);
							}

							else
							{
								sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
								fprintf(output, bufTemp, temp, "");
							}
						}

						else if( stringContain(inputString, "BYTE PTR DS:") == 1)
						{							
							if( (rdataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
							{
								rawAddr = temp & 0x0000FFFF;

								iatFound = findIAT(idata, &iatData, iibnSize, rawAddr);

								if(iatFound == 1)
								{
									strcpy(iatString, iatData.dllName);
									strcat(iatString, " : ");
									strcat(iatString, iatData.funcName);

									sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
									fprintf(output, bufTemp, temp, iatString);
								}
							}

							else if( (dataIsh.PointerToRawData & 0x0000F000) == (temp & 0x0000F000) )
							{
								offsetTemp = ftell(file);

								if((temp & 0x00FFFF) >= (ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
								{
									return -5;
								}

								fseek(file, temp & 0x00FFFF, SEEK_SET);

								for(i=0; i < 1; i++)
								{
									dsString[i] = fgetc(file);
								}
								dsString[i] = '\0';

								sprintf(bufTemp, "%08X %s", offsetTemp - 5, inputString);
								fprintf(output, bufTemp, temp, dsString);

								fseek(file, offsetTemp, SEEK_SET);
							}

							else
							{
								sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
								fprintf(output, bufTemp, temp, "");
							}
						}

						else if( stringContain(inputString, "CALL") == 1)
						{
							sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
							fprintf(output, bufTemp, ftell(file) + temp);
						}

						else if( inputString[0] == 'J')
						{
							sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
							fprintf(output, bufTemp, ftell(file) + temp);
						}

						else if( stringContain(inputString, "PUSH") == 1 && ( (temp & 0x00FFF000) == ((inh.OptionalHeader.ImageBase | dataIsh.PointerToRawData) & 0x00FFF000)) )
						{
							char chTemp;
							unsigned int forOffset = 0;
							offsetTemp = ftell(file);

							if((temp & 0x00FFFF) >= (ish[inh.FileHeader.NumberOfSections-1].PointerToRawData + ish[inh.FileHeader.NumberOfSections-1].SizeOfRawData))
							{
								return -5;
							}

							fseek(file, temp & 0x00FFFF, SEEK_SET);

							i = 0;

							while( (chTemp = fgetc(file)) != NULL)
							{
								if(chTemp == '%')
								{
									strcat(dsString, "%%");
									i = i + 2;
								}

								else if(chTemp == 0x0A)
								{
									strcat(dsString, "\\n");
									i++;
								}

								else if(chTemp > 0x7F)
									dsString[i++] = '\0';

								else
									dsString[i++] = chTemp;
							}

							dsString[i+1] = '\0';

							inputString[strlen(inputString) - 1] = '\0';

							if(stringIsNull(dsString) || (i > 510))
							{
								strcat(inputString, "\n");

								sprintf(bufTemp, "%08X %s", offsetTemp - 5, inputString);
								fprintf(output, bufTemp, (temp & 0x0000FFFF));
							}

							else
							{
								strcat(inputString, " (");
								strcat(inputString, dsString);
								strcat(inputString, ")\n");

								sprintf(bufTemp, "%08X %s", offsetTemp - 5, inputString);
								fprintf(output, bufTemp, (temp & 0x0000FFFF));
							}
							fseek(file, offsetTemp, SEEK_SET);
						}

						else
						{
							sprintf(bufTemp, "%08X %s", ftell(file) - 5, inputString);
							fprintf(output, bufTemp, temp);
						}
					}
				}

				else if(inputString[0] == '2')
				{
					if(inputString[2] == '1')
					{
						orData = fgetc(file);
						temp = orData;

						numOfOpr1 = 1;
					}

					else if(inputString[2] == '2')
					{
						temp0 = fgetc(file);
						orData = fgetc(file);

						temp = (orData << 8) | temp0;

						numOfOpr1 = 2;
					}

					else if(inputString[2] == '4')
					{
						temp0 = fgetc(file);
						temp1 = fgetc(file);
						temp2 = fgetc(file);
						temp3 = fgetc(file);

						temp = (temp3 << 24) | (temp2 << 16) | (temp1 << 8) | temp0;

						numOfOpr1 = 4;
					}

					if(inputString[4] == '1')
					{
						orData = fgetc(file);

						numOfOpr2 = 1;
					}

					else if(inputString[4] == '2')
					{
						temp0 = fgetc(file);
						temp1 = fgetc(file);

						orData = (temp1 << 8) | temp0;

						numOfOpr2 = 2;
					}

					else if(inputString[4] == '4')
					{
						temp0 = fgetc(file);
						temp1 = fgetc(file);
						temp2 = fgetc(file);
						temp3 = fgetc(file);

						orData = (temp3 << 24) | (temp2 << 16) | (temp1 << 8) | temp0;

						numOfOpr2 = 4;
					}

					subStr(inputString, inputString, 6, strlen(inputString) - 6);

					sprintf(bufTemp, "%08X %s", ftell(file) - 1 - numOfOpr1 - numOfOpr2, inputString);
					fprintf(output, bufTemp, temp, orData);
				}
			}
		}

		//operation이 없을 경우 처리 루틴
		else if(newOperation == 1)
		{
			sprintf(bufTemp, "%08X UNKNOWN\n", ftell(file) - 1);
			fprintf(output, bufTemp);
			newOperation = 0;
		}

		else if(newOperation == 2)
		{
			sprintf(bufTemp, "%08X UNKNOWN\n", ftell(file) - 2);
			fprintf(output, bufTemp);
			newOperation = 0;
		}

		else if(newOperation == 3)
		{
			sprintf(bufTemp, "%08X UNKNOWN\n", ftell(file) - 3);
			fprintf(output, bufTemp);
			newOperation = 0;
		}

		else if(newOperation == 4)
		{
			sprintf(bufTemp, "%08X UNKNOWN\n", ftell(file) - 4);
			fprintf(output, bufTemp);
			newOperation = 0;
		}

		else if(newOperation == 5)
		{
			sprintf(bufTemp, "%08X UNKNOWN\n", ftell(file) - 5);
			fprintf(output, bufTemp);
			newOperation = 0;
		}
	}

	if(output != NULL)
		fclose(output);

	return 0;
}

/*
Import Address Table을 찾는 함수

return
1	: Import Address table을 찾은 경우
0	: Import Address table을 못 찾은 경우
*/
int convert::findIAT(iatData* idata, iatData* iatData, int iibnSize, int addr)
{
	for(int i=0; i < iibnSize; i++)
	{
		if(idata[i].funcAddr_RVA == addr)
		{
			*iatData = idata[i];
			return 1;
		}
	}

	return 0;
}

/*
convertHexToAsm에서 data영역에서 읽은 값이 code영역을 가리키는지 확인하는 함수

return
1	: data영역에서 읽은 값이 code영역을 가리킬 경우
0	: data영역에서 읽은 값이 code영역을 가리키지 않을 경우
*/
int convert::pointToTextSection(char* str)
{
	int tempAddr = 0;

	tempAddr = tempAddr | (str[3] << 24);
	tempAddr = tempAddr | (str[2] << 16);
	tempAddr = tempAddr | (str[1] << 8);
	tempAddr = tempAddr | (str[0] & 0x000000FF);

	if((tempAddr >= (ish[0].PointerToRawData | inh.OptionalHeader.ImageBase)) && (tempAddr <= ((ish[0].PointerToRawData + ish[0].SizeOfRawData) | inh.OptionalHeader.ImageBase)))
	{
		return 1;
	}

	return 0;
}

/*
subStr함수
전체String에서 sub string을 찾는 함수.
*/
void convert::subStr(char* input, char* output, int nStart, int nLen)
{
	int nLoop;
	int nLength;
	char* temp;

	if(input == NULL)
		return;

	temp = (char*)malloc(sizeof(char) * nLen + 1);

	nLength = strlen(input);

	if(nLength > nStart + nLen)
	{
		nLength = nStart + nLen;
	}

	for(nLoop = nStart; nLoop < nLength; nLoop++)
	{
		temp[nLoop-nStart] = input[nLoop];
	}
	temp[nLoop-nStart] = '\0';

	memset(output, 0, strlen(output));

	strcpy(output, temp);

	free(temp);
}

/*
stringContain함수
문자열을 포함하고 있는지 없는지를 검사하는 함수
*/
int convert::stringContain(char* str1, char* str2)
{
	int count = strlen(str2);
	int findFirst = 0;
	int j=0;

	for(int i = 0; i < count; i++)
	{
		if(findFirst == 0)
		{
			if(str1[i] == str2[j])
			{
				j++;
				findFirst = 1;
			}
		}
		else
		{
			if(str1[i] != str2[j])
			{
				j = 0;
			}

			else
			{
				j++;
			}
		}
	}

	if(j == 0)
		return 0;

	else
		return 1;
}

/*
stringIsNull함수
string이 null인지 판별하는 함수
*/
int convert::stringIsNull(char* str)
{
	for(unsigned int i=0; i< strlen(str); i++)
	{
		if(str[i] != 0x00)
			return 0;
	}

	return 1;
}

/*
PE header 정보를 database로 전송하는 함수

return
insert_header_info의 return value
*/
int convert::sendPEHeaderInfo()
{
	char machine[512];
	char timeStamp[512];

	struct tm* t;

	if(inh.FileHeader.Machine == 0x014C)
	{
		strcpy(machine, "Intel 386");
	}

	t = localtime((time_t *)&inh.FileHeader.TimeDateStamp);

	sprintf(timeStamp, "%d-%d-%d %d:%d:%d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	//machine, timeStamp, Entry point, number of sections

	return insert_header_info(machine, timeStamp, inh.OptionalHeader.AddressOfEntryPoint, inh.FileHeader.NumberOfSections);
}

/*
PE section 정보를 database로 전송하는 함수
*/
void convert::sendPESectionInfo()
{
	for(int i = 0; i < inh.FileHeader.NumberOfSections; i++)
	{
		//ish[i].Name, ish[i].VirtualAddress, ish[i].Misc.VirtualSize, ish[i].SizeOfRawData
		insert_section((char*)ish[i].Name, ish[i].VirtualAddress, ish[i].Misc.VirtualSize, ish[i].SizeOfRawData);
	}
}

/*
PE import 정보를 database로 전송하는 함수
*/
void convert::sendPEImportInfo()
{
	//iidSize dll 개수, iibnSize function 전체의 개수
	for(int i = 0; i < iibnSize; i++)
	{
		insert_import(idata[i].dllName, idata[i].funcName);
	}
}

/*
operation에 해당하는 hexcode가 database에 존재하는지 확인하는 함수

return
-1	: database에 operation이 없는 경우
1	: database에 operation이 있는 경우
*/
int convert::find_hex (char* hexcode) 
{
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;
	bson_t *query;
	int key = -1;

	collection = mongoc_client_get_collection (client_convert, database_convert, "Hex_to_asm");
	query = bson_new();
	BSON_APPEND_UTF8 (query, "_id", hexcode);

	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
	//----------------------------------------------------------------------------------------------
	if (mongoc_cursor_next (cursor, &doc)) {
		key = 1;
	}

	bson_destroy (query);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return key;
}

/*
operation에 해당하는 hexcode가 가지고 있는 contents를 database에서 받아오는 함수

return
hexcode가 가지고있는 contents 문자열
*/
char* convert::find_hex_contents (char* hexcode) 
{
	int i = 0;
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;
	bson_t *pipeline;//bson_t *query;
	char *str;
	char *temp;

	collection = mongoc_client_get_collection (client_convert, database_convert, "Hex_to_asm");

	pipeline = BCON_NEW ("pipeline", "[",
		"{", "$match", "{", "_id", BCON_UTF8(hexcode), "}","}",
		"{", "$project", "{", "_id", BCON_INT32 (0), "hexcode", BCON_INT32 (1), "contents",  BCON_INT32 (1),"}", "}",
		"]");

	cursor = mongoc_collection_aggregate (collection, MONGOC_QUERY_NONE, pipeline, NULL, NULL);

	if(mongoc_cursor_next (cursor, &doc)) {
		str = bson_as_json (doc, NULL);
	}   

	temp = str;
	temp = strtok(temp,"\"");

	for(i = 0; i < 7; i++) 
		temp = strtok(NULL,"\"");

	if(strcmp(temp, "MORE") !=0 ) {//&& strcmp(temp, "MORE\n") != 0) {
		//temp = strtok(temp, "\\");
		i = strlen(temp);
		temp[i-1] = '\0';
		temp[i-2] = '\n';
		return temp;
	} else //if(strcmp(temp, "MORE") == 0 && strcmp(temp, "MORE\n") == 0)
		return temp;

	bson_free (str);
	bson_destroy (pipeline);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);
}

/*
PE header 정보를 database에 insert하는 함수
이 함수를 이용해 database에 binary file에 대한 데이터가 존재하는지 확인할 수 있다.

return
-1	: 데이터베이스에 PE header가 존재 할 경우
1	: 데이터베이스에 PE header가 존재하지 않을 경우
*/
int convert::insert_header_info(char *target_machine, char *comp_timestamp, int entry_pt, int num_of_section){

	mongoc_collection_t *collection;
	bson_error_t error;
	bson_t *doc;
	char convert_int_to_char[100] = "\0";
	char convert_hex_to_char[100] = "\0";

	/*
		'PE Header basic information' collection에 
		header 정보 insert
		doc => insert할 정보 저장
   */
	collection = mongoc_client_get_collection (client_convert, database_convert, "PE Header basic information");

	doc = bson_new ();

	BSON_APPEND_UTF8 (doc,"_id",ex_name);
	BSON_APPEND_UTF8 (doc, "Target machine", target_machine);
	BSON_APPEND_UTF8 (doc, "Compilation timestamp", comp_timestamp);

	sprintf(convert_hex_to_char, "0x%08X", entry_pt);
	BSON_APPEND_UTF8 (doc, "Entry Point", convert_hex_to_char);
	_itoa(num_of_section,convert_int_to_char,10);
	BSON_APPEND_UTF8 (doc, "Number of sections", convert_int_to_char);

	/*
		insert에 실패하면 -1 리턴
		성공 시 1 리턴
   */
	if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
		printf ("%s\n", error.message);
		bson_destroy (doc);
		mongoc_collection_destroy (collection);

		return -1;
	}

	bson_destroy (doc);
	mongoc_collection_destroy (collection);
	return 1;
}

/*
PE section 정보를 database에 insert하는 함수
*/
int convert::insert_section(char *name, int vir_addr, int vir_size, int raw_size){
	/*
	Name
	Virtual address
	Virtual size
	Raw size
	*/
	mongoc_collection_t *collection;
	bson_error_t error;
	bson_t *doc;
	char convert_addr_to_char[100] = "\0";
	char convert_vir_to_char[100] = "\0";
	char convert_raw_to_char[100] = "\0";
	char *section;
	section = (char *)malloc(sizeof(char)*(strlen(ex_name)+10));
	for(int i=0;i<strlen(ex_name)+10;i++)
		section[i] = '\0';

	/*
		'PE Section' collection에 
		section 정보 insert
		doc => insert할 정보 저장
   */
	collection = mongoc_client_get_collection (client_convert, database_convert, "PE Section");   // set collection

	doc = bson_new ();

	BSON_APPEND_UTF8 (doc, "Name", name);
	_itoa(vir_addr,convert_addr_to_char,10);
	BSON_APPEND_UTF8 (doc, "Virtual address",convert_addr_to_char);
	_itoa(vir_size,convert_vir_to_char,10);
	BSON_APPEND_UTF8 (doc, "Virtual size",convert_vir_to_char);
	_itoa(raw_size,convert_raw_to_char,10);
	BSON_APPEND_UTF8 (doc, "Raw size", convert_raw_to_char);
	BSON_APPEND_UTF8 (doc, "Exe name", ex_name);
	strcat(section,ex_name);
	strcat(section,"_");
	strcat(section,name);
	BSON_APPEND_UTF8 (doc, "_id", section);

	  /*
		insert에 실패하면 -1 리턴
		성공 시 1 리턴
   */
	if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
		printf ("%s\n", error.message);
		free(section);
		bson_destroy (doc);
		mongoc_collection_destroy (collection);
		return -1;
	}

	free(section);
	bson_destroy (doc);
	mongoc_collection_destroy (collection);
	return 1;
}

/*
PE import 정보를 database에 insert하는 함수
*/
int convert::insert_import(char *dll_name,char *func_name){
	/*
	dll 이름
	*/
	bool pass = true;
   for(int i = 0; i < strlen(func_name); i++){
      if( func_name[i] < 65  || func_name[i] > 122){
         pass = false;
         break;
      }
      else if(func_name[i] >90 && func_name[i] <97){
         if(func_name[i] == 95)
            continue;
         else{
            pass = false;
            break;
         }
      }
   }
	if(pass == true){
		mongoc_collection_t *collection, *collection_t;
		mongoc_cursor_t *cursor;
		bson_error_t error;
		const bson_t *doc;
		bson_t *query,*find;
		char *section, *str, header[64];
		memset(header,0,64);

		/*
		collection => 'PE imports' collection, insert할 collection
		collection_t => 'Total function' 함수정보를 찾을 collection
		query => insert할 정보 저장
   */
		collection = mongoc_client_get_collection (client_convert, database_convert, "PE imports");   // set collection

		query = bson_new ();

		BSON_APPEND_UTF8 (query, "DLL name",dll_name);
		BSON_APPEND_UTF8 (query, "Function name",func_name);

		collection_t = mongoc_client_get_collection (client_convert, database_convert, "Total function");   // set collection

		find = bson_new();
		BSON_APPEND_UTF8(find,"_id",func_name);
		cursor = mongoc_collection_find (collection_t, MONGOC_QUERY_NONE, 0, 0, 0, find, NULL, NULL);

		/*
		insert할 Windows함수의 header정보 find
		만약 데이터베이스에 저장되어 있지 않은 함수라면 non_exist함수 호출
   */
		if(mongoc_cursor_next(cursor,&doc)){
			str = bson_as_json(doc,NULL);
			if(str)
				convert_value_strtok(str,"header_name",header);
			bson_free(str);
		}
		else{
			non_exist(func_name);
			strcpy(header,"Unknown header");
		}

		/*
		위에서 찾은 header name, 분석한 파일이름
		primary key에 "분석한 파일이름_함수이름"으로
		insert할 정보에 추가
		*/
		BSON_APPEND_UTF8 (query,"Header name",header);
		BSON_APPEND_UTF8 (query, "Exe name", ex_name);

		section = (char *)malloc(sizeof(char)*(strlen(ex_name)+strlen(func_name)+2));
		memset(section,0,sizeof(char)*(strlen(ex_name)+strlen(func_name)+2));

		strcat(section,ex_name);
		strcat(section,"_");
		strcat(section,func_name);
		BSON_APPEND_UTF8 (query, "_id", section);

		/*
		insert에 실패하면 -1 리턴
		성공 시 1 리턴
		*/
		if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, query, NULL, &error)) {
			printf("%s\n",error.message);
			if(section != NULL)
			{
				free(section);
			}
			mongoc_collection_destroy (collection);
			return -1;
		}
		if(section != NULL)
		{
			free(section);
		}
		bson_destroy(query);
		bson_destroy(find);
		mongoc_cursor_destroy(cursor);
		mongoc_collection_destroy (collection_t);
		mongoc_collection_destroy (collection);

		return 1;
	}
	return -1;
}

/*
데이터베이스에 요청한 query의 결과를 갖는 str에서 
결과의 column인 query_func_name에 해당하는 값을 print_str에 저장하는 함수
*/
void convert::convert_value_strtok(char *str, char *query_func_name, char *print_str){
	int count = strlen(query_func_name);
	int findFirst = 0;
	int rtn_value=0;
	int total_value_cnt = 0;
	int print_str_cnt = 0;

	/*
		데이터베이스에서 받아온 정보에서 원하는 field를 찾는다.
		str => 데이터베이스에서 받아온 문장
		query_func_name => 원하는 field 이름
		ex) field1 : "value", field2 : "value" 
		str문장이 위와같은 식으로 되어있기 때문에  
		field명을 찾아준 후 total_value_cnt 값에 +5를 해준다.
   */
	while(rtn_value < count){
		if(findFirst == 0){
			if(str[total_value_cnt++] == query_func_name[rtn_value]){
				rtn_value++;
				findFirst = 1;
			}
		}
		else{
			if(str[total_value_cnt++] != query_func_name[rtn_value]){
				rtn_value = 0;
				findFirst = 0;
			}
			else{
				rtn_value++;
			}
		}
	}
	total_value_cnt += 5;

	/*
		원하는 field의 value값을 저장하여 리턴해준다.
   */
	while(1){

		print_str[print_str_cnt++] = str[total_value_cnt++];

		if(   str[total_value_cnt] == '\"')
			break;
	}
}

/*
binary file에서 사용하는 함수가 데이터베이스에 없을 경우
해당하는 함수의 이름을 데이터베이스에 insert하는 함수
*/
void convert::non_exist(char *func_name){

	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	bson_error_t error;
	const bson_t *doc;
	bson_t *query;
	int malloc_size = 0;

	/*
		'Nonexist' collection에 해당함수가 있는지 찾는다.
   */
	collection = mongoc_client_get_collection (client_convert, database_convert, "Nonexist");   // load to dll collection

	query = bson_new ();

	BSON_APPEND_UTF8 (query, "_id", func_name);
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	/*
		collection에 찾는 함수가 없으면 Unknown함수로 insert한다.
   */
	if (!mongoc_cursor_next (cursor, &doc)){

		BSON_APPEND_UTF8 (query, "_id", func_name);

		// create content
		query = BCON_NEW("_id", BCON_UTF8(func_name), "function_name",BCON_UTF8(func_name), "dll_name",BCON_UTF8("Unknown DLL name"),
			"header_name",BCON_UTF8("Unknown header"),"parameter_name",BCON_UTF8("Unknown parameter"),"return_type",BCON_UTF8("Unknown return type"),
			"parameter_count",BCON_UTF8("-1"),"function_count",BCON_INT32(1),"function_API",BCON_UTF8("Unknown reference"));

		mongoc_collection_insert (collection, MONGOC_INSERT_NONE, query, NULL, &error);
	}

	bson_destroy (query);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);
}


/*
아래 모든 함수는 get함수로, 위에서 연산된 결과값들을 가져올수 있다.
Path, Machine, TimeStamp등 연산된 결과값을 가지고 온다.
*/
char* convert::getPath()
{
	return ex_name;
}
int convert::getMachine()
{
	return inh.FileHeader.Machine;
}
tm* convert::getTimeStamp()
{
	return localtime((time_t *)&inh.FileHeader.TimeDateStamp);
}
int convert::getAddressOfEntryPoint()
{
	return inh.OptionalHeader.AddressOfEntryPoint;
}
int convert::getNumberOfSections()
{
	return inh.FileHeader.NumberOfSections;
}
char* convert::getSectionName(int i)
{
	return (char *)ish[i].Name;
}
int convert::getVirtualAddress(int i)
{
	return ish[i].VirtualAddress;
}
int convert::getVirtualSize(int i)
{
	return ish[i].Misc.VirtualSize;
}
int convert::getSizeOfRawData(int i)
{
	return ish[i].SizeOfRawData;
}
int convert::getIibnSize()
{
	return iibnSize;
}
char* convert::getDllName(int i)
{
	return idata[i].dllName;
}
char* convert::getFuncName(int i)
{
	return idata[i].funcName;
}

/*
binary file의 각 section의 offset과 raw data를 file로 저장하는 함수
*/
void convert::separateSection()
{
	FILE* fp;
	char buf[1024];
	char fileName[128];
	char offset[16];

	int i = 0;
	int len;
	int lastSectionOffset;

	for(int index = 0; index < inh.FileHeader.NumberOfSections; index++)
	{
		strcpy(fileName, (char*)ish[index].Name);
		strcat(fileName, "Section.txt");

		fp = fopen(fileName, "w");
		lastSectionOffset = ish[index].PointerToRawData + ish[index].SizeOfRawData;
		fseek(in, ish[index].PointerToRawData, SEEK_SET);

		while((unsigned int)ftell(in) < (ish[index].PointerToRawData + ish[index].SizeOfRawData))
		{
			sprintf(offset, "%08X	", ftell(in));
			len = fread(buf, 1, 16, in);

			if(ftell(in) >= lastSectionOffset)
			{
				if(ftell(in) - lastSectionOffset > 0)
				{
					fputs(offset, fp);

					for(i = 0; i < ftell(in) - lastSectionOffset; i++)
					{
						fprintf(fp, "%02X ", buf[i]);
					}

					len = ((16 - i)/2) - 1;
					for(i = 0; i < len; i++)
					{
						fprintf(fp, "	");
					}
				}
				fprintf(fp, "	");

				for(i = 0; i < ftell(in) - lastSectionOffset; i++)
				{
					if(buf[i] >= 0x20 && buf[i] <= 0x7E)
						fprintf(fp, "%c ", buf[i]);
					else
						fprintf(fp, ". ");
				}
				fprintf(fp, "\n");
				break;
			}

			fputs(offset, fp);
			for(i = 0; i < 16; i++)
			{
				fprintf(fp, "%02X ", buf[i] & 0x00FF);
			}

			fprintf(fp, "	");

			for(i = 0; i < 16; i++)
			{
				if(buf[i] >= 0x20 && buf[i] <= 0x7E)
					fprintf(fp, "%c ", buf[i]);
				else
					fprintf(fp, ". ");
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
}
short convert::countImageImportDescriptor(FILE *file, imageNtHeader32 *inh, imageSectionHeader *ish)
{
	const int SIZE = 20; //IMAGE_IMPORT_DESCRIPTOR 구조체의 절대크기 (워낙 예외가 많아 상수처리)
	short count = 0;
	char isEnd = 1;
	unsigned char temp;

	if(fseek(file, convertRVAToRAW(inh, ish, inh->OptionalHeader.DataDirectory[1].VirtualAddress), SEEK_SET) == -1)
	{
		printf("Offset setting failed\n");
		return -1;
	}

	while(1) {
		for(int i = 0; i < SIZE; i++) {
			if(temp = fgetc(file) != 0)
			{
				isEnd = 0;
			}
			//	printf("%X ", temp);
		}
		//	printf("\n");
		if(isEnd)
		{
			break;
		}
		isEnd = 1;
		count++;
	}
	return count;
}
