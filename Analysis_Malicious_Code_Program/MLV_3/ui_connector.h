#ifndef MONGOC_CONNECT_UI
#define MONGOC_CONNECT_UI
#include <bson.h>
#include <mongoc.h>

#define database_ui "database"
#define server_addr_ui   "mongodb://59.11.39.160:43572/"

char *exe_name;
char **func_name;
char *func_info[2];
char **h_info;
char ***s_info;
char **i_info;

int t_func_cnt = 0;
int i_func_cnt = 0;
int section_cnt = 0;
int ui_connect_cnt = 5;
mongoc_client_t *client_ui;

/*
	set_path 함수
	분석할 파일을 선택하면 전체 파일 경로에서 실행파일 이름만 저장하는 함수이다,
*/
void set_path(char *input_exe){
	char *ptr;
	char *path;
	char *cut = NULL;

	/*
      global var initialize
	  exe_name에 실제 파일 이름을 저장한다.
   */
	if(exe_name)
		free(exe_name);

	path = (char *)malloc(sizeof(char) * strlen(input_exe) + 1);
	strcpy_s(path,strlen(input_exe)+1 ,input_exe);

	ptr = strtok_s(path, "\\",&cut);

	/*
		파일 경로에서 파일 이름 부분을 찾고 .exe부분도 잘라서 저장한다.
   */
	while(ptr != NULL) {
		if(strstr(ptr, ".exe") != NULL) {
			exe_name = (char *)malloc(sizeof(char) * strlen(ptr) + 1);
			strcpy_s(exe_name,strlen(ptr)+1 ,strtok_s(ptr, ".exe",&cut));
			break;
		}

		ptr = strtok_s(NULL, "\\",&cut);
	}

	free(path);
}
/*
	start_db 함수
	데이터베이스에 접속하는 함수이다. 
	접속이 제대로 되면 true를 반환하고 실패한다면 최대 5회까지 시도하고 false를 반환한다.
*/
bool start_db(){

	bson_t *query = NULL;
	const bson_t *doc;
	mongoc_cursor_t *cursor = NULL;
	mongoc_collection_t *collection = NULL;
	bool connect_success = false;

	/*
		데이터베이스에 접속되어 있거나 5번의 접속 실패하 않았을 경우
		접속을 시도한다.
   */
	while(connect_success == false && ui_connect_cnt > 0){

		/*
			반복문 시작하기 전 null값이 아닌 변수 메모리 할당 해제
	   */
		if(client_ui)      mongoc_client_destroy(client_ui);
		if(cursor)      mongoc_cursor_destroy(cursor);
		if(query)      bson_destroy(query);
		if(collection)   mongoc_collection_destroy(collection);

		/*
			초기화 및 데이터베이스 접속
			접속이 제대로 되었는지 확인하기 위해 find query 실행
	  */
		mongoc_init ();
		query = bson_new ();
		client_ui = mongoc_client_new (server_addr_ui);         // connect ip address
		collection = mongoc_client_get_collection (client_ui, database_ui, "Total function");
		cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

		/*
			접속이 제대로 되었으면 connect_success의 값 true, 실패하면 시도횟수 -1 후 처음부터 반복
			connect_success => 데이터베이스에 접속이 되었는지 확인하는 변수
	  */
		if(mongoc_cursor_next(cursor,&doc))   //Check connection
			connect_success = true;
		else{
			if(ui_connect_cnt != 0)         //reconnection 5 times
				ui_connect_cnt--;
		}
	}

	/*
		메모리 할당 해제
		connect_success 값이 false 이면 false 리턴한다.
		connect_success 값이 true 이면 true 리턴한다.
   */
	if(cursor)      mongoc_cursor_destroy(cursor);
	if(query)      bson_destroy (query);
	if(collection)   mongoc_collection_destroy (collection);
	if(connect_success == false) if(client_ui) {
		mongoc_client_destroy(client_ui);
		return false;
	}
	return true;
}

/*
	exit_db 함수
	모든 작업이 끈나고 데이터베이스 접속 종료 및 사용한 동적할당을 해제할 때 호출한다.
*/
void exit_db(){
	/*
		메모리 할당 해제안된 포인터 해제
		func_name => 함수 목록 저장하는 함수
		func_info => 함수정보 저장하는 함수
		h_info => header info 정보 저장시 사용하는 함수
		s_info => section info 정보 저장시 사용하는 함수
		i_info => import info 정보 저장시 사용하는 함수
	*/
	if(func_name){
		for(int i=0; i<t_func_cnt; i++)
			free(func_name[i]);
		free(func_name);
	}
	if(func_info){
		free(func_info[0]);
		free(func_info[1]);
	}

	if(h_info){
		for(int i=0;i<4;i++)
			free(h_info[i]);
		free(h_info);
	}
	if(s_info){
		for (int i = 0; i < section_cnt; i++){
			for (int j = 0; j < 4; j++)
				free(s_info[i][j]);
			free(s_info[i]);
		}
		free(s_info);
	}
	if(i_info){
		for(int i=0;i<i_func_cnt-7;i++){
			free(i_info[i]);
		}
		free(i_info);
	}

	/*
		실행 파일 이름 해제
		데이터베이스 접속 해제
   */
	if(exe_name)
		free(exe_name);

	if(client_ui)
		mongoc_client_destroy (client_ui);
}

/*
	ui_dynamic_size 함수
	데이터베이스에서 받아온 정보에서 원하는 field의 글자 수를 Count하여 리턴해주는 함수.
*/
int ui_dynamic_size(char *str, char *query_var){
	int count = strlen(query_var);
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
			if(str[total_value_cnt++] == query_var[rtn_value]){
				rtn_value++;
				findFirst = 1;
			}
		}
		else{
			if(str[total_value_cnt++] != query_var[rtn_value]){
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
		원하는 field의 value값의 길이를 count하여 리턴해준다.
   */
	while(1){
		str[total_value_cnt++];
		print_str_cnt++; 
		if(str[total_value_cnt] == '\"')
			break;
	}

	return print_str_cnt+1;
}

/*
	ui_value_strtok 함수
	데이터베이스에서 받아온 정보에서 field의 정보를 찾아 리턴해주는 함수.
*/
void ui_value_strtok(char *str, char *query_var, char *print_str){
	int count = strlen(query_var);
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
			if(str[total_value_cnt++] == query_var[rtn_value]){
				rtn_value++;
				findFirst = 1;
			}
		}
		else{
			if(str[total_value_cnt++] != query_var[rtn_value]){
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
	import_info 함수
	UI에 출력할 Binary 파일에서 사용한 함수들을 'dll이름:function이름'으로 배열에 저장하여 리턴한다.
*/
char **import_info(int *count){
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;         // do not adjust because of const.
	bson_t *pipeline,*query;
	char *str,*dll,*func,*exe;
	int str_length = 0,malloc_size;
	int size = 0;

	/*
		dll과 function 이름을 저장할 이차원배열이
		할당 해제가 안되어있을 경우 할당 해제
   */
	if(i_info){
		for(int i=0;i<i_func_cnt;i++){
			free(i_info[i]);
		}
		free(i_info);
		i_func_cnt = 0;
	}

	/*
		데이터베이스에서 find query 실행
   */
	collection = mongoc_client_get_collection (client_ui, database_ui, "PE imports");

	query = bson_new ();
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	/*
		데이터베이스의 'PE imports' collection에서 
		분석한 파일에서 사용한 함수들이 총 몇개인지 확인
		str_length => 데이터베이스에서 받아온 문장의 최대 길이값 저장
		i_func_cnt => 분석한 파일에서 사용한 함수들이 총 개수 저장
		사용한 cursor 해제
	*/
	while(mongoc_cursor_next (cursor, &doc)){   // total dll count
		str = bson_as_json (doc, NULL);
		if(str){
			exe = (char *)malloc(sizeof(char) *strlen(str));
			memset(exe,0,sizeof(char)*strlen(str));
			ui_value_strtok(str,"Exe name",exe);
			size = strlen(str);
			if(str_length < size)
				str_length = size;

			if(strcmp(exe,exe_name) == 0)
				i_func_cnt++;
			free(exe);
			//bson_free(str);
		}
		bson_free(str);
	}

	mongoc_cursor_destroy (cursor);

	/*
		dll과 function 이름을 저장할 이차원배열 생성
   */
	i_info = (char**)malloc(sizeof(char*)*i_func_cnt);   // create dynamic array
	for(int i=0;i<i_func_cnt;i++){
		i_info[i] = (char*)malloc(sizeof(char)*str_length);
		memset(i_info[i],0,sizeof(char)*str_length);
	}

	/*
		데이터베이스에서 dll name과 function name을 정렬하여 Find query 실행
   */
	pipeline = BCON_NEW ("pipeline", "[",
		"{", "$sort", "{", "DLL name", BCON_INT32 (1), "Function name", BCON_INT32 (1), "}", "}"
		,"]");

	cursor = mongoc_collection_aggregate (collection, MONGOC_QUERY_NONE, pipeline, NULL, NULL);

	/*
		Collection내에서 정렬후 문서끝까지 검색
   */
	int info_cnt = 0;
	while(mongoc_cursor_next (cursor, &doc)){

		str = bson_as_json (doc, NULL);
		if(str){
			malloc_size = ui_dynamic_size(str,"Exe name");
			exe = (char *)malloc(sizeof(char) *malloc_size);
			memset(exe,0,sizeof(char)*malloc_size);      
			ui_value_strtok(str,"Exe name",exe);

			/*
			분석한 파일의 함수가 맞으면 'DLL 이름:function 이름'으로 배열에 저장
			*/
			if(strcmp(exe,exe_name) == 0){
				dll = (char *)malloc(sizeof(char) *strlen(str));
				func = (char *)malloc(sizeof(char) *strlen(str));
				memset(dll,0,sizeof(char)*strlen(str));
				memset(func,0,sizeof(char)*strlen(str));

				ui_value_strtok(str,"DLL name",dll);
				strcat_s(dll,strlen(str)," : ");

				ui_value_strtok(str,"Function name",func);
				strcat_s(dll,strlen(str),func);

				strcpy_s(i_info[info_cnt],strlen(dll)+strlen(func)+1, dll);
				//strcpy(i_info[info_cnt],dll);

				info_cnt++;
				free(dll);free(func);
			}
			free(exe);
		}
		bson_free(str);
	}

	/*
		count는 저장한 함수들의 총 개수
		사용한 포인터 메모리 해제 후 저장한 정보 리턴
   */
	*count = info_cnt - 1;

	bson_destroy (query);
	bson_destroy (pipeline);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return i_info;
}

/*
	section_info 함수
	UI에 출력할 Binary 파일의 section정보를 배열에 저장하여 리턴한다.
*/
char ***section_info(){
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;         // do not adjust because of const.
	bson_t *query;
	char *str,*exe, *name, *address, *size, *raw;
	int info_cnt = 0;


	/*
		section 정보를 리턴할 2차원 배열 생성
   */
	s_info = (char***)malloc(sizeof(char**)*section_cnt);   // create dynamic array
	for(int i=0; i<section_cnt; i++){
		s_info[i] = (char**)malloc(sizeof(char*)*4);
		for(int j=0;j<4;j++)
			s_info[i][j] = (char*)malloc(sizeof(char)*300);
	}

	/*
		데이터베이스에서 find query 실행
   */
	collection = mongoc_client_get_collection (client_ui, database_ui, "PE Section");

	query = bson_new ();
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	/*
		Collection 내에서 분석한 파일의 section정보가 맞으면 배열에 저장
	*/
	while(mongoc_cursor_next (cursor, &doc)){

		str = bson_as_json (doc, NULL);
		if(str){
			exe = (char *)malloc(sizeof(char) *strlen(str));
			memset(exe,0,sizeof(char)*strlen(str));

			ui_value_strtok(str,"Exe name",exe);
			if(strcmp(exe,exe_name) == 0){
				name = (char *)malloc(sizeof(char) *strlen(str));
				address = (char *)malloc(sizeof(char) *strlen(str));
				size = (char *)malloc(sizeof(char) *strlen(str));
				raw = (char *)malloc(sizeof(char) *strlen(str));
				memset(name,0,sizeof(char)*strlen(str));
				memset(address,0,sizeof(char)*strlen(str));
				memset(size,0,sizeof(char)*strlen(str));
				memset(raw,0,sizeof(char)*strlen(str));

				ui_value_strtok(str,"Name",name);
				ui_value_strtok(str,"Virtual address",address);
				ui_value_strtok(str,"Virtual size",size);
				ui_value_strtok(str,"Raw size",raw);

				strcpy_s(s_info[info_cnt][0],strlen(name)+1,name);
				strcpy_s(s_info[info_cnt][1],strlen(address)+1,address);
				strcpy_s(s_info[info_cnt][2],strlen(size)+1,size);
				strcpy_s(s_info[info_cnt][3],strlen(raw)+1,raw);
				info_cnt++;

				free(name); free(address); free(size); free(raw);
			}
		}
		bson_free(str);
	}

	/*
		할당 해제 후
		저장한 정보 리턴
   */
	bson_destroy (query);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return s_info;
}

/*
	header_info 함수
	UI에 출력할 Binary 파일의 header정보를 배열에 저장하여 리턴한다.
*/
char **header_info(int *err_msg){
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;         // do not adjust because of const.
	bson_t *query;
	char *str;
	*err_msg = -1;

	/*
		할당 해제 안되있는 포인터 할당해제
   */
	if(h_info)
	{
		for(int i=0;i<4;i++)
		{
			if(h_info[i] != NULL)
			{
				free(h_info[i]);
			}
		}
		free(h_info);
	}
	if(s_info){
		for (int i = 0; i < section_cnt; i++){
			for (int j = 0; j < 4; j++)
				free(s_info[i][j]);
			free(s_info[i]);
		}
		free(s_info);
		section_cnt = 0;
	}

	/*
		데이터베이스에서 primary key(_id)가 분석한 파일의 이름에 대한 find query 실행
   */
	collection = mongoc_client_get_collection (client_ui, database_ui, "PE Header basic information");

	query = bson_new ();
	BSON_APPEND_UTF8(query,"_id",exe_name);
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	/*
		찾은 정보 배열에 저장
   */
	if(mongoc_cursor_next (cursor, &doc)){

		str = bson_as_json (doc, NULL);
		if(str){

			h_info = (char**)malloc(sizeof(char*)*4);   // create dynamic array
			for(int i=0; i<4; i++){
				h_info[i] = (char*)malloc(sizeof(char)*strlen(str));
				memset(h_info[i],0,sizeof(char)*strlen(str));
			}

			*err_msg = 1;
			ui_value_strtok(str,"Target machine",h_info[0]);
			ui_value_strtok(str,"Compilation timestamp",h_info[1]);
			ui_value_strtok(str,"Entry Point",h_info[2]);
			ui_value_strtok(str,"Number of sections",h_info[3]);
			section_cnt = atoi(h_info[3]);
		}
		bson_free(str);
	}

	/*
		포인터 할당 해제후
		저장한 배열 리턴
   */
	bson_destroy (query);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return h_info;
}

/*
	print_refer 함수
	UI에 출력할 함수의 모든 정보를 리턴한다.
*/
char **print_refer(char *f_name){
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;         // do not adjust because of const.
	bson_t *query;
	char *str, *dll, *h_name, *p_name, *r_type, *refer;
	char *tmp = '\0';
	int malloc_size = 0, size_tmp = 0;

	/*
		해제 안된 포인터 할당 해제
	*/
	if(func_info){
		free(func_info[0]);
		free(func_info[1]);
	}

	/*
		데이터베이스에서 primary key(_id)가 분석한 파일의 이름에 대한 find query 실행
   */
	collection = mongoc_client_get_collection (client_ui, database_ui, "Total function");

	query = bson_new ();
	BSON_APPEND_UTF8(query,"_id",f_name);
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
	/*
	func_info[0] : except for reference
	func_info[1] : reference
	*/
	if(mongoc_cursor_next (cursor, &doc)){
		strcat_s(f_name,strlen(f_name)+2,"\n");
		str = bson_as_json (doc, NULL);
		if(str){
			func_info[0] = (char*)malloc(sizeof(char)*(strlen(str)+1));   // create dynamic array
			memset(func_info[0],0,sizeof(char)*(strlen(str)+1));

			malloc_size = ui_dynamic_size(str,"dll_name");
			dll = (char *)malloc(sizeof(char) *(malloc_size+1));
			memset(dll,0,sizeof(char)*malloc_size);
			ui_value_strtok(str,"dll_name",dll);
			strcat_s(dll,malloc_size+1,"\n");

			malloc_size = ui_dynamic_size(str,"header_name");
			h_name = (char *)malloc(sizeof(char) *(malloc_size+1));
			memset(h_name,0,sizeof(char)*malloc_size);
			ui_value_strtok(str,"header_name",h_name);
			strcat_s(h_name,malloc_size+1,"\n");

			malloc_size = ui_dynamic_size(str,"parameter_name");
			p_name = (char *)malloc(sizeof(char) *(malloc_size+1));
			memset(p_name,0,sizeof(char)*malloc_size);
			ui_value_strtok(str,"parameter_name",p_name);
			strcat_s(p_name,malloc_size+1,"\n");

			malloc_size = ui_dynamic_size(str,"return_type");
			r_type = (char *)malloc(sizeof(char) *(malloc_size+1));
			memset(r_type,0,sizeof(char)*malloc_size);
			ui_value_strtok(str,"return_type",r_type);
			strcat_s(r_type,malloc_size+1,"\n");

			malloc_size = ui_dynamic_size(str,"function_API");
			refer = (char *)malloc(sizeof(char) *(malloc_size+1));
			memset(refer,0,sizeof(char)*malloc_size);
			ui_value_strtok(str,"function_API",refer);
			strcat_s(refer,malloc_size+1,"\n");

			func_info[1] = (char*)malloc(sizeof(char)*(malloc_size+1));
			memset(func_info[1],0,sizeof(char)*(malloc_size+1));

			strcpy_s(func_info[0],17,"Function Name : ");         strcat_s(func_info[0],strlen(str),f_name);
			strcat_s(func_info[0],strlen(str),"DLL Name : ");      strcat_s(func_info[0],strlen(str),dll);
			strcat_s(func_info[0],strlen(str),"Header Name : ");   strcat_s(func_info[0],strlen(str),h_name);
			strcat_s(func_info[0],strlen(str),"Parameter Name : "); strcat_s(func_info[0],strlen(str),p_name);
			strcat_s(func_info[0],strlen(str),"Return Type : ");   strcat_s(func_info[0],strlen(str),r_type);
			strcat_s(func_info[1],malloc_size+1,refer);

			free(dll);free(h_name);free(p_name);free(r_type);free(refer);
		}
		bson_free(str);
	}

	/*
		메모리 할당 해제 후
		저장한 정보 리턴
   */
	bson_destroy (query);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return func_info;
}

/*
	print_exe_list 함수
	Binary 파일에서 사용한 함수목록을 리턴해준다.
*/
char **print_exe_list() {
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;         // do not adjust because of const.
	bson_t *query, *pipeline;
	char *str, *f_name_tmp, *exe, **arr_func;
	int malloc_size = 0;

	/*
		할당 안된 포인터 할당 해제
   */
	if(func_name){
		for(int i=0; i<t_func_cnt; i++)
			free(func_name[i]);
		free(func_name);
		t_func_cnt = 0;
	}

	/*
		데이터베이스에서 find query 실행
   */
	collection = mongoc_client_get_collection (client_ui, database_ui, "PE imports");
	query = bson_new();
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	/*
		데이터베이스의 'PE imports' collection에서 
		분석한 파일에서 사용한 함수들이 총 몇개인지 확인
		t_func_cnt => 분석한 파일에서 사용한 함수들이 총 개수 저장
		사용한 cursor 해제
	*/
	while (mongoc_cursor_next (cursor, &doc)){   // mongoc_cursor_next (cursor, &doc) means if next doesn't exist, then return false. if it is exist, return true
		str = bson_as_json (doc, NULL);
		if(str){
			malloc_size = ui_dynamic_size(str,"Exe name");
			exe = (char *)malloc(sizeof(char) *malloc_size);
			memset(exe,0,sizeof(char)*malloc_size);
			ui_value_strtok(str,"Exe name",exe);

			if(strcmp(exe,exe_name) == 0)
				t_func_cnt++;
			free(exe);
		}
		bson_free(str);
	}

	/*
		cursor 다시 초기화
   */
	mongoc_cursor_destroy (cursor);
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL); // cursor resetting


	/*
		저장해서 리턴할 동적 배열 생성
		func_name[0] 에는 총 함수개수 저장
   */
	t_func_cnt++;
	if(t_func_cnt == 1)
		t_func_cnt++;
	func_name = (char**)malloc(sizeof(char*)*t_func_cnt);   // create dynamic array
	arr_func = (char**)malloc(sizeof(char*)*t_func_cnt);
	for(int j=0; j<t_func_cnt; j++){
		func_name[j] = (char*)malloc(sizeof(char)*200);
		arr_func[j] = (char*)malloc(sizeof(char)*200);
		memset(arr_func[j],0,sizeof(char)*200);
		memset(func_name[j],0,sizeof(char)*200);
	}

	_itoa_s(t_func_cnt,func_name[0],t_func_cnt,10);         // convert int to char

	int info=1;      // start point

	/*
		분석한 파일에서 사용된 함수이름 저장
   */

	while (mongoc_cursor_next (cursor, &doc)){      // load db to func_info

		str = bson_as_json (doc, NULL);
		if(str){
			malloc_size = ui_dynamic_size(str,"Exe name");
			exe = (char *)malloc(sizeof(char) *malloc_size);
			memset(exe,0,sizeof(char)*malloc_size);
			ui_value_strtok(str,"Exe name",exe);

			if(strcmp(exe,exe_name) == 0){
				malloc_size = ui_dynamic_size(str,"Function name");
				f_name_tmp = (char *)malloc(sizeof(char) *malloc_size);
				memset(f_name_tmp,0,sizeof(char)*malloc_size);

				ui_value_strtok(str,"Function name",f_name_tmp);   // insert func_name
				strcpy_s(arr_func[info],strlen(f_name_tmp)+1,f_name_tmp);
				info++;
				free(f_name_tmp);
			}
			free(exe);
		}
		bson_free(str);
	}

	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	/*
		위에서 저장한 함수 목록을 
		총 함수호출 횟수에 대해 내림차순으로 정렬하여 저장하기 위해
		'Total function' collection에서 function_count에 대해 내림차순 정렬하여 Find
	*/

	collection = mongoc_client_get_collection (client_ui, database_ui, "Total function");

	pipeline = BCON_NEW ("pipeline", "[",
		"{", "$sort", "{", "function_count", BCON_INT32 (-1), "}", "}"
		,"]");

	cursor = mongoc_collection_aggregate (collection, MONGOC_QUERY_NONE, pipeline, NULL, NULL);

	info=1;

	/*
		저장된 함수 목록을 function_count에대해 정렬하여 저장
   */
	while(mongoc_cursor_next(cursor,&doc)){
		str = bson_as_json(doc,NULL);
		if(str){
			malloc_size = ui_dynamic_size(str,"function_name");
			f_name_tmp = (char *)malloc(sizeof(char) *malloc_size);
			memset(f_name_tmp,0,sizeof(char)*malloc_size);
			ui_value_strtok(str,"function_name",f_name_tmp);   // insert func_name

			for(int i=1;i<t_func_cnt;i++){
				if(strcmp(f_name_tmp,arr_func[i]) == 0)
					strcpy_s(func_name[info++],strlen(arr_func[i])+1,arr_func[i]);
			}
			free(f_name_tmp);
		}
		bson_free(str);
		if(info == t_func_cnt)   break;
	}

	/*
		할당 해제 후
		저장한 정보 리턴
   */
	if(arr_func){
		for(int i=0; i<t_func_cnt; i++)
			free(arr_func[i]);
		free(arr_func);
	}
	bson_destroy (query);
	bson_destroy (pipeline);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return func_name;
}
#endif