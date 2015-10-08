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
	set_path �Լ�
	�м��� ������ �����ϸ� ��ü ���� ��ο��� �������� �̸��� �����ϴ� �Լ��̴�,
*/
void set_path(char *input_exe){
	char *ptr;
	char *path;
	char *cut = NULL;

	/*
      global var initialize
	  exe_name�� ���� ���� �̸��� �����Ѵ�.
   */
	if(exe_name)
		free(exe_name);

	path = (char *)malloc(sizeof(char) * strlen(input_exe) + 1);
	strcpy_s(path,strlen(input_exe)+1 ,input_exe);

	ptr = strtok_s(path, "\\",&cut);

	/*
		���� ��ο��� ���� �̸� �κ��� ã�� .exe�κе� �߶� �����Ѵ�.
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
	start_db �Լ�
	�����ͺ��̽��� �����ϴ� �Լ��̴�. 
	������ ����� �Ǹ� true�� ��ȯ�ϰ� �����Ѵٸ� �ִ� 5ȸ���� �õ��ϰ� false�� ��ȯ�Ѵ�.
*/
bool start_db(){

	bson_t *query = NULL;
	const bson_t *doc;
	mongoc_cursor_t *cursor = NULL;
	mongoc_collection_t *collection = NULL;
	bool connect_success = false;

	/*
		�����ͺ��̽��� ���ӵǾ� �ְų� 5���� ���� ������ �ʾ��� ���
		������ �õ��Ѵ�.
   */
	while(connect_success == false && ui_connect_cnt > 0){

		/*
			�ݺ��� �����ϱ� �� null���� �ƴ� ���� �޸� �Ҵ� ����
	   */
		if(client_ui)      mongoc_client_destroy(client_ui);
		if(cursor)      mongoc_cursor_destroy(cursor);
		if(query)      bson_destroy(query);
		if(collection)   mongoc_collection_destroy(collection);

		/*
			�ʱ�ȭ �� �����ͺ��̽� ����
			������ ����� �Ǿ����� Ȯ���ϱ� ���� find query ����
	  */
		mongoc_init ();
		query = bson_new ();
		client_ui = mongoc_client_new (server_addr_ui);         // connect ip address
		collection = mongoc_client_get_collection (client_ui, database_ui, "Total function");
		cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

		/*
			������ ����� �Ǿ����� connect_success�� �� true, �����ϸ� �õ�Ƚ�� -1 �� ó������ �ݺ�
			connect_success => �����ͺ��̽��� ������ �Ǿ����� Ȯ���ϴ� ����
	  */
		if(mongoc_cursor_next(cursor,&doc))   //Check connection
			connect_success = true;
		else{
			if(ui_connect_cnt != 0)         //reconnection 5 times
				ui_connect_cnt--;
		}
	}

	/*
		�޸� �Ҵ� ����
		connect_success ���� false �̸� false �����Ѵ�.
		connect_success ���� true �̸� true �����Ѵ�.
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
	exit_db �Լ�
	��� �۾��� ������ �����ͺ��̽� ���� ���� �� ����� �����Ҵ��� ������ �� ȣ���Ѵ�.
*/
void exit_db(){
	/*
		�޸� �Ҵ� �����ȵ� ������ ����
		func_name => �Լ� ��� �����ϴ� �Լ�
		func_info => �Լ����� �����ϴ� �Լ�
		h_info => header info ���� ����� ����ϴ� �Լ�
		s_info => section info ���� ����� ����ϴ� �Լ�
		i_info => import info ���� ����� ����ϴ� �Լ�
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
		���� ���� �̸� ����
		�����ͺ��̽� ���� ����
   */
	if(exe_name)
		free(exe_name);

	if(client_ui)
		mongoc_client_destroy (client_ui);
}

/*
	ui_dynamic_size �Լ�
	�����ͺ��̽����� �޾ƿ� �������� ���ϴ� field�� ���� ���� Count�Ͽ� �������ִ� �Լ�.
*/
int ui_dynamic_size(char *str, char *query_var){
	int count = strlen(query_var);
	int findFirst = 0;
	int rtn_value=0;
	int total_value_cnt = 0;
	int print_str_cnt = 0;

	/*
		�����ͺ��̽����� �޾ƿ� �������� ���ϴ� field�� ã�´�.
		str => �����ͺ��̽����� �޾ƿ� ����
		query_func_name => ���ϴ� field �̸�
		ex) field1 : "value", field2 : "value" 
		str������ ���Ͱ��� ������ �Ǿ��ֱ� ������  
		field���� ã���� �� total_value_cnt ���� +5�� ���ش�.
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
		���ϴ� field�� value���� ���̸� count�Ͽ� �������ش�.
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
	ui_value_strtok �Լ�
	�����ͺ��̽����� �޾ƿ� �������� field�� ������ ã�� �������ִ� �Լ�.
*/
void ui_value_strtok(char *str, char *query_var, char *print_str){
	int count = strlen(query_var);
	int findFirst = 0;
	int rtn_value=0;
	int total_value_cnt = 0;
	int print_str_cnt = 0;

	/*
		�����ͺ��̽����� �޾ƿ� �������� ���ϴ� field�� ã�´�.
		str => �����ͺ��̽����� �޾ƿ� ����
		query_func_name => ���ϴ� field �̸�
		ex) field1 : "value", field2 : "value" 
		str������ ���Ͱ��� ������ �Ǿ��ֱ� ������  
		field���� ã���� �� total_value_cnt ���� +5�� ���ش�.
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
		���ϴ� field�� value���� �����Ͽ� �������ش�.
   */
	while(1){

		print_str[print_str_cnt++] = str[total_value_cnt++];

		if(   str[total_value_cnt] == '\"')
			break;
	}
}

/*
	import_info �Լ�
	UI�� ����� Binary ���Ͽ��� ����� �Լ����� 'dll�̸�:function�̸�'���� �迭�� �����Ͽ� �����Ѵ�.
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
		dll�� function �̸��� ������ �������迭��
		�Ҵ� ������ �ȵǾ����� ��� �Ҵ� ����
   */
	if(i_info){
		for(int i=0;i<i_func_cnt;i++){
			free(i_info[i]);
		}
		free(i_info);
		i_func_cnt = 0;
	}

	/*
		�����ͺ��̽����� find query ����
   */
	collection = mongoc_client_get_collection (client_ui, database_ui, "PE imports");

	query = bson_new ();
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	/*
		�����ͺ��̽��� 'PE imports' collection���� 
		�м��� ���Ͽ��� ����� �Լ����� �� ����� Ȯ��
		str_length => �����ͺ��̽����� �޾ƿ� ������ �ִ� ���̰� ����
		i_func_cnt => �м��� ���Ͽ��� ����� �Լ����� �� ���� ����
		����� cursor ����
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
		dll�� function �̸��� ������ �������迭 ����
   */
	i_info = (char**)malloc(sizeof(char*)*i_func_cnt);   // create dynamic array
	for(int i=0;i<i_func_cnt;i++){
		i_info[i] = (char*)malloc(sizeof(char)*str_length);
		memset(i_info[i],0,sizeof(char)*str_length);
	}

	/*
		�����ͺ��̽����� dll name�� function name�� �����Ͽ� Find query ����
   */
	pipeline = BCON_NEW ("pipeline", "[",
		"{", "$sort", "{", "DLL name", BCON_INT32 (1), "Function name", BCON_INT32 (1), "}", "}"
		,"]");

	cursor = mongoc_collection_aggregate (collection, MONGOC_QUERY_NONE, pipeline, NULL, NULL);

	/*
		Collection������ ������ ���������� �˻�
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
			�м��� ������ �Լ��� ������ 'DLL �̸�:function �̸�'���� �迭�� ����
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
		count�� ������ �Լ����� �� ����
		����� ������ �޸� ���� �� ������ ���� ����
   */
	*count = info_cnt - 1;

	bson_destroy (query);
	bson_destroy (pipeline);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return i_info;
}

/*
	section_info �Լ�
	UI�� ����� Binary ������ section������ �迭�� �����Ͽ� �����Ѵ�.
*/
char ***section_info(){
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;         // do not adjust because of const.
	bson_t *query;
	char *str,*exe, *name, *address, *size, *raw;
	int info_cnt = 0;


	/*
		section ������ ������ 2���� �迭 ����
   */
	s_info = (char***)malloc(sizeof(char**)*section_cnt);   // create dynamic array
	for(int i=0; i<section_cnt; i++){
		s_info[i] = (char**)malloc(sizeof(char*)*4);
		for(int j=0;j<4;j++)
			s_info[i][j] = (char*)malloc(sizeof(char)*300);
	}

	/*
		�����ͺ��̽����� find query ����
   */
	collection = mongoc_client_get_collection (client_ui, database_ui, "PE Section");

	query = bson_new ();
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	/*
		Collection ������ �м��� ������ section������ ������ �迭�� ����
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
		�Ҵ� ���� ��
		������ ���� ����
   */
	bson_destroy (query);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return s_info;
}

/*
	header_info �Լ�
	UI�� ����� Binary ������ header������ �迭�� �����Ͽ� �����Ѵ�.
*/
char **header_info(int *err_msg){
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;         // do not adjust because of const.
	bson_t *query;
	char *str;
	*err_msg = -1;

	/*
		�Ҵ� ���� �ȵ��ִ� ������ �Ҵ�����
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
		�����ͺ��̽����� primary key(_id)�� �м��� ������ �̸��� ���� find query ����
   */
	collection = mongoc_client_get_collection (client_ui, database_ui, "PE Header basic information");

	query = bson_new ();
	BSON_APPEND_UTF8(query,"_id",exe_name);
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	/*
		ã�� ���� �迭�� ����
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
		������ �Ҵ� ������
		������ �迭 ����
   */
	bson_destroy (query);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return h_info;
}

/*
	print_refer �Լ�
	UI�� ����� �Լ��� ��� ������ �����Ѵ�.
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
		���� �ȵ� ������ �Ҵ� ����
	*/
	if(func_info){
		free(func_info[0]);
		free(func_info[1]);
	}

	/*
		�����ͺ��̽����� primary key(_id)�� �м��� ������ �̸��� ���� find query ����
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
		�޸� �Ҵ� ���� ��
		������ ���� ����
   */
	bson_destroy (query);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);

	return func_info;
}

/*
	print_exe_list �Լ�
	Binary ���Ͽ��� ����� �Լ������ �������ش�.
*/
char **print_exe_list() {
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;         // do not adjust because of const.
	bson_t *query, *pipeline;
	char *str, *f_name_tmp, *exe, **arr_func;
	int malloc_size = 0;

	/*
		�Ҵ� �ȵ� ������ �Ҵ� ����
   */
	if(func_name){
		for(int i=0; i<t_func_cnt; i++)
			free(func_name[i]);
		free(func_name);
		t_func_cnt = 0;
	}

	/*
		�����ͺ��̽����� find query ����
   */
	collection = mongoc_client_get_collection (client_ui, database_ui, "PE imports");
	query = bson_new();
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	/*
		�����ͺ��̽��� 'PE imports' collection���� 
		�м��� ���Ͽ��� ����� �Լ����� �� ����� Ȯ��
		t_func_cnt => �м��� ���Ͽ��� ����� �Լ����� �� ���� ����
		����� cursor ����
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
		cursor �ٽ� �ʱ�ȭ
   */
	mongoc_cursor_destroy (cursor);
	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL); // cursor resetting


	/*
		�����ؼ� ������ ���� �迭 ����
		func_name[0] ���� �� �Լ����� ����
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
		�м��� ���Ͽ��� ���� �Լ��̸� ����
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
		������ ������ �Լ� ����� 
		�� �Լ�ȣ�� Ƚ���� ���� ������������ �����Ͽ� �����ϱ� ����
		'Total function' collection���� function_count�� ���� �������� �����Ͽ� Find
	*/

	collection = mongoc_client_get_collection (client_ui, database_ui, "Total function");

	pipeline = BCON_NEW ("pipeline", "[",
		"{", "$sort", "{", "function_count", BCON_INT32 (-1), "}", "}"
		,"]");

	cursor = mongoc_collection_aggregate (collection, MONGOC_QUERY_NONE, pipeline, NULL, NULL);

	info=1;

	/*
		����� �Լ� ����� function_count������ �����Ͽ� ����
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
		�Ҵ� ���� ��
		������ ���� ����
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