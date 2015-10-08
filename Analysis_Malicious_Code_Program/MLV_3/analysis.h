/*
	<�� �ּ��� �����ϰ��� �ϴ� ������ ��������>

	������ϰ� define �����̴�
	���� operation, address, Information�� ũ�⸦ ����
	RETURN_VALUE�� �Լ� ȣ�� �� return �Ǵ� ���� RETURN_VALUE��� ǥ��
	computer ����ü���� ����ϴ� �⺻������ REG_SIZE��ŭ�� ũ�⸦ ���´�
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
	���������� ���������� ���� ����ü�̴�.
	������ ����ü�� �����̸��� ����� value���� ������ �ִ�.
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
	�� �ڵ忡�� ���� �߿��� COMPUTER ����ü�̴�.
	�� ����ü�� computer�� �⺻ register�� flag, Stack���� ������ �ִ�.
	�⺻���� �������� ����� ������ ������ ����.

	condition_index		-> JMP�� JE�� jump�� �ؾ��ϴ� ��ɾ ������ �� �پ�� �Ǵ� �������� �ּ�(�ε���)�� ������ �ִ�.
	Stack				-> ��ǻ���� Stack�����̴�.
	file_start_index	-> �Լ��� main���� ������ ���Ͽ����� ���� �ּ�(�ε���)�� ������ �ִ�.
	file_end_index		-> �Լ��� main���� ������ ���Ͽ����� �� �ּ�(�ε���)�� ������ �ִ�.
	operator			-> �����ڸ� �����ϴ� ����
	operand				-> �ǿ����ڸ� �����ϴ� ����
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
	�� ������ ���������� �Լ����� ��Ÿ����.
	push_count			-> call��ɾ ������ �� �������� �� parameter�� ����� database���� �� �� ���� ��츦 ����� push�� ��ü������ count�ϱ� ���� ����
	separate_count		-> �Լ��� main�� �� ��� ���Ϸ� ������������ ���� ������ �����ϰ� �ִ� ����
	blockFlag			->
	File_Data			-> ó���� Assembly�ڵ带 ������ �� char�� �ϳ��ϳ� �� �޾Ƽ� �����ϰ� �ִ� �ʱ�迭
	main_address		-> �ڵ念������ main�ڵ��� �����ּҰ��� �����ϰ� �ִ� ����
	search_idx			->
	line				-> �и��ǰ� ���� �� ��� ��ɾ ������ �ִ����� ���� ����(�� �˰����� ���� Ƚ���� ����)
	operation			-> File_Data���� operation���� ����, �����ϰ� �ִ� �迭
	address				-> File_Data���� address���� ����, �����ϰ� �ִ� �迭
	information			-> File_Data���� operation�� ������ ����, �����ϰ� �ִ� �迭
	return_value		-> calculate_global�Լ����� ���Ǵ� return����
	c_result			-> calculate_global�Լ����� ���Ǵ� return����
	computer			-> �� �˰��򿡼� ���� ���� computer ����ü

	------------------�Լ�----------------
	File_ReadText					-> Assembly�ڵ带 �ҷ��� File_Data�� �����ϴ� �Լ�
	File_ReadText_Address			-> Assembly�ڵ� �߿��� main�ּҰ��� �ҷ��� �����ϴ� �Լ�
	save_registry_file				-> ���� ���߿����� register�� ��ȭ���� ������ �Լ�
	save_separate_code				-> �Լ��� main���� �и���Ų ������ �����ϴ� �Լ�
	save_separate_c_file			-> �и��� Assembly���� C���� ��ȯ �� �������� txt�� �������ִ� �Լ�
	set_information					-> ���ľ��� �������� ����� File_Data�� address, operation, information���� �и����� �����ϴ� �Լ�
	separate_code					-> �Լ��� main���� �и���Ű�� �Լ�
	set_order						-> �� �ڵ��� ������ ����ϰ� �ִ� �Լ�
	calculate_global				-> �������͸� ����, ������ ��Ģ������ ���ִ� �κ��� �������ִ� �Լ�
	find_variable					-> ����, ������������ ã�����ϴ� ������ ã�� �ε����� ���Ͻ����ִ� �Լ�
	convertToDecimal				-> 16������ 10������ �ٲ��ִ� �Լ�

	a_operation						-> ������� ��ɾ� �߿� a�� �����ϴ� ��ɾ ���� �Լ�
	c_operation						-> ������� ��ɾ� �߿� c�� �����ϴ� ��ɾ ���� �Լ�
	d_operation						-> ������� ��ɾ� �߿� d�� �����ϴ� ��ɾ ���� �Լ�
	h_operation						-> ������� ��ɾ� �߿� h�� �����ϴ� ��ɾ ���� �Լ�
	i_operation						-> ������� ��ɾ� �߿� i�� �����ϴ� ��ɾ ���� �Լ�
	j_operation						-> ������� ��ɾ� �߿� j�� �����ϴ� ��ɾ ���� �Լ�
	l_operation						-> ������� ��ɾ� �߿� l�� �����ϴ� ��ɾ ���� �Լ�
	m_operation						-> ������� ��ɾ� �߿� m�� �����ϴ� ��ɾ ���� �Լ�
	n_operation						-> ������� ��ɾ� �߿� n�� �����ϴ� ��ɾ ���� �Լ�
	o_operation						-> ������� ��ɾ� �߿� o�� �����ϴ� ��ɾ ���� �Լ�
	p_operation						-> ������� ��ɾ� �߿� p�� �����ϴ� ��ɾ ���� �Լ�
	r_operation						-> ������� ��ɾ� �߿� r�� �����ϴ� ��ɾ ���� �Լ�
	s_operation						-> ������� ��ɾ� �߿� s�� �����ϴ� ��ɾ ���� �Լ�
	t_operation						-> ������� ��ɾ� �߿� t�� �����ϴ� ��ɾ ���� �Լ�
	u_operation						-> ������� ��ɾ� �߿� u�� �����ϴ� ��ɾ ���� �Լ�
	w_operation						-> ������� ��ɾ� �߿� w�� �����ϴ� ��ɾ ���� �Լ�
	x_operation						-> ������� ��ɾ� �߿� x�� �����ϴ� ��ɾ ���� �Լ�
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