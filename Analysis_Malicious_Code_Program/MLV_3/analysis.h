/*
	<각 주석은 설명하고자 하는 영역의 위에있음>

	헤더파일과 define 영역이다
	각각 operation, address, Information의 크기를 지정
	RETURN_VALUE로 함수 호출 후 return 되는 값은 RETURN_VALUE라고 표기
	computer 구조체에서 사용하는 기본적으로 REG_SIZE만큼의 크기를 갖는다
*/
#define _CRT_SECURE_NO_WARNINGS
//#define _ITERATOR_DEBUG_LEVEL 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <bson.h>
#include <mongoc.h>

#define OPERATION_LENGTH_MAX 8
#define ADDRESS_LENGTH_MAX 9
#define INFORMATION_LENGTH_MAX 1024
#define DATABASE_SIZE 112
#define RETURN_VALUE "ret_value"
#define REG_SIZE 50

#define database		"database"
#define server_address	"mongodb://59.11.39.160:43572/"

using namespace std;

/*
	l_variable, g_variable
	지역변수와 전역변수에 대한 구조체이다.
	각각의 구조체는 변수이름과 저장될 value값을 가지고 있다.
*/
typedef struct l_variable{
	char variable_name[INFORMATION_LENGTH_MAX];
	int varable_value;
}LOCAL_VARIABLE;

typedef struct g_variable{
	char variable_name[INFORMATION_LENGTH_MAX];
	int varable_value;
}global_VARIABLE;

/*
	이 코드에서 가장 중요한 COMPUTER 구조체이다.
	이 구조체는 computer의 기본 register와 flag, Stack등을 가지고 있다.
	기본적인 구조외의 선언된 변수는 다음과 같다.

	condition_index		-> JMP나 JE등 jump를 해야하는 명령어를 수행할 때 뛰어야 되는 목적지의 주소(인덱스)를 가지고 있다.
	Stack				-> 컴퓨터의 Stack구조이다.
	file_start_index	-> 함수와 main으로 나눠진 파일에서의 시작 주소(인덱스)를 가지고 있다.
	file_end_index		-> 함수와 main으로 나눠진 파일에서의 끝 주소(인덱스)를 가지고 있다.
	operator			-> 연산자를 저장하는 변수
	operand				-> 피연산자를 저장하는 변수
*/
typedef struct __computer{
	char EAX[REG_SIZE];
	char EBX[REG_SIZE];
	char ECX[REG_SIZE];
	char EDX[REG_SIZE];

	char ESP[REG_SIZE];
	char EBP[REG_SIZE];
	char EIP[REG_SIZE];
	int local_size;
	int global_size;

	bool OF;
	bool DF;
	bool IF;
	bool SF;
	bool ZF;
	bool AF;
	bool PF;
	bool CF;

	char ESI[REG_SIZE];
	char EDI[REG_SIZE];

	LOCAL_VARIABLE localVariable[1024];
	global_VARIABLE globalVariable[1024];

	vector<char *> condition_index;
	vector<char *> J_address;
	vector<char *> Stack;
	vector<int> file_start_index;
	vector<int> file_end_index;

	string operater;
	string operand;
}COMPUTER;

#ifdef __errno_t
errno_t fopen_s(
	FILE** pFile,
	const char *filename,
	const char *mode
	);
#endif

#pragma pack(push, 1)

typedef struct __file{
	FILE* stream;
	errno_t err;
	char Temp[sizeof(FILE*)];
}ST_FILE;

#pragma pack(pop)

/*
	이 파일의 전역변수와 함수들을 나타낸다.
	push_count			-> call명령어를 수행할 때 가져가야 될 parameter가 몇개인지 database에서 알 수 없을 경우를 대비해 push를 자체적으로 count하기 위한 변수
	separate_count		-> 함수와 main이 총 몇개의 파일로 나눠졌는지에 대한 개수를 저장하고 있는 변수
	blockFlag			->
	File_Data			-> 처음의 Assembly코드를 나누기 전 char로 하나하나 다 받아서 저장하고 있는 초기배열
	main_address		-> 코드영역에서 main코드의 시작주소값을 저장하고 있는 변수
	search_idx			->
	line				-> 분리되고 나서 총 몇개의 명령어를 가지고 있는지에 대한 변수(이 알고리즘의 동작 횟수와 같다)
	operation			-> File_Data에서 operation만을 추출, 저장하고 있는 배열
	address				-> File_Data에서 address만을 추출, 저장하고 있는 배열
	information			-> File_Data에서 operation의 정보를 추출, 저장하고 있는 배열
	return_value		-> calculate_global함수에서 사용되는 return변수
	c_result			-> calculate_global함수에서 사용되는 return변수
	computer			-> 이 알고리즘에서 사용될 가상 computer 구조체

	------------------함수----------------
	File_ReadText					-> Assembly코드를 불러와 File_Data에 저장하는 함수
	File_ReadText_Address			-> Assembly코드 중에서 main주소값을 불러와 저장하는 함수
	save_registry_file				-> 추후 개발예정인 register의 변화값을 저장할 함수
	save_separate_code				-> 함수와 main들을 분리시킨 내용을 저장하는 함수
	save_separate_c_file			-> 분리된 Assembly에서 C언어로 변환 된 정보들을 txt로 저장해주는 함수
	set_information					-> 형식없이 무작위로 저장된 File_Data를 address, operation, information으로 분리시켜 저장하는 함수
	separate_code					-> 함수와 main들을 분리시키는 함수
	set_order						-> 이 코드의 동작을 담당하고 있는 함수
	calculate_global				-> 레지스터를 포함, 각각의 사칙연산이 되있는 부분을 연산해주는 함수
	find_variable					-> 지역, 전역변수에서 찾고자하는 변수를 찾아 인덱스를 리턴시켜주는 함수
	convertToDecimal				-> 16진수를 10진수로 바꿔주는 함수

	a_operation						-> 어셈블리어 명령어 중에 a로 시작하는 명령어에 대한 함수
	c_operation						-> 어셈블리어 명령어 중에 c로 시작하는 명령어에 대한 함수
	d_operation						-> 어셈블리어 명령어 중에 d로 시작하는 명령어에 대한 함수
	h_operation						-> 어셈블리어 명령어 중에 h로 시작하는 명령어에 대한 함수
	i_operation						-> 어셈블리어 명령어 중에 i로 시작하는 명령어에 대한 함수
	j_operation						-> 어셈블리어 명령어 중에 j로 시작하는 명령어에 대한 함수
	l_operation						-> 어셈블리어 명령어 중에 l로 시작하는 명령어에 대한 함수
	m_operation						-> 어셈블리어 명령어 중에 m로 시작하는 명령어에 대한 함수
	n_operation						-> 어셈블리어 명령어 중에 n로 시작하는 명령어에 대한 함수
	o_operation						-> 어셈블리어 명령어 중에 o로 시작하는 명령어에 대한 함수
	p_operation						-> 어셈블리어 명령어 중에 p로 시작하는 명령어에 대한 함수
	r_operation						-> 어셈블리어 명령어 중에 r로 시작하는 명령어에 대한 함수
	s_operation						-> 어셈블리어 명령어 중에 s로 시작하는 명령어에 대한 함수
	t_operation						-> 어셈블리어 명령어 중에 t로 시작하는 명령어에 대한 함수
	u_operation						-> 어셈블리어 명령어 중에 u로 시작하는 명령어에 대한 함수
	w_operation						-> 어셈블리어 명령어 중에 w로 시작하는 명령어에 대한 함수
	x_operation						-> 어셈블리어 명령어 중에 x로 시작하는 명령어에 대한 함수
*/
class analysis
{
private:
	int push_count;
	int separate_count;
	int blockFlag;
	char* File_Data;
	char* main_address; 
	int search_idx;
	int line;
	char** operation;
	char** address;
	char** information;
	char **parameter_info;
	char return_value[INFORMATION_LENGTH_MAX];
	char c_result[INFORMATION_LENGTH_MAX];
	char exeName[512];
	COMPUTER computer;
	
	void File_ReadText(char* filename);
	void File_ReadText_Address(char* filename);
	void File_ReadText_Separate_Code(char* filename);

	void save_registry_file(char* operation, char* address, char* information);
	void save_separate_code(char** operation, char** address, char**information, int start_index, int end_index, int file_index);
	void save_separate_c_file(char** operation, char** address, char**information, int start_index, int end_index, int file_index);

	void set_information(int line, char** operation, char** address, char** information);
	void separate_code(int line, char** operation, char** address, char** information);
	void set_order(char** operation, char** address, char** information, int line);
	char* calculate_global(char *calculate);
	int find_variable(char* name);
	unsigned long convertToDecimal(char hex[]);

	void a_operation(int index);
	void c_operation(int index, char** separate_address);
	void d_operation(int index);
	void h_operation(int index);
	void i_operation(int index);
	void j_operation(int index, char** separate_address, int selected_file_index);
	void l_operation(int index);
	void m_operation(int index);
	void n_operation(int index);
	void o_operation(int index);
	void p_operation(int index);
	void r_operation(int index);
	void s_operation(int index, char** separate_address, int selected_file_index);
	void t_operation(int index);
	void u_operation(int index);
	void w_operation(int index);
	void x_operation(int index);

	int para_cnt_type_int;
	int connect_count;
	bool check_decompile;
	mongoc_client_t *client_decompile;

	void start_db();
	int decompile_dynamic_size(char *str, char *query_var);
	void decompile_value_strtok(char *str, char *query_var, char *print_str);
	char **find_func(char* filename,int& count);
	char** find_value(char *query_info);

public:
	analysis();
	analysis(char* readTextFile, char* readTextAddressFile);
	void setExeName(char* fileName);
	void doSetInformation();
	void doSeparateCode();
	void doSetOrder();
	void setBasicFile(char* readTextFile, char* readTextAddressFile);
	void makeCfile();
	void clear();
	~analysis();
};