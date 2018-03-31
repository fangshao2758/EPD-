Cnf* read_cnf(const char* filename)
{
	char* buffer = read_file(filename);
	char filtered = filter_comments(buffer);
	Cnf* cnf = parse_cnf_file(filtered);
	cnf->op=CONJOIN;
	for(SddSize i=0;i<cnf->litset_count;i++)
		cnf->litsets[i].op=DISJOIN;
	return cnf;
}

static char* read_file(const char* filename){
	FILE *file = fopen(filename,"rb");//检查文件是否为空
	if(file == NULL){
		printf("could not open the file %s\n",filename);
		exit(1);
	}

	fseek(file,0,SEEK_END);  //计算文件大小
	unsigned int file_size = ftell(file);
	rewind(file);

	char* buffer = (char*)calloc(file_size+1,sizeof(char)); //根据文件大小给buffer分配空间

	unsigned int result = fread(buffer,sizeof(char),file_size,file); //将文件的数据读到buffer,result是读入buffer的项数

    if(result != file_size){
    	printf("could not read the file %s\n",filename);
    	exit(1);
    }
    buffer[file_size] = 0;
    fclose(file);

    return buffer;
}

static char* filter_comments(const char* buffer) {
  unsigned int file_length = 0, line_length;
  const char* read_head = buffer;
  char* write_head;

  while (*read_head != '\0') {    //通过遍历buffer计算file_length
    line_length = 0;
    while (*read_head != '\0') {
      read_head++;
      line_length++;
      if (*read_head == '\n') break;
    }
    if (*read_head != 'c')
      file_length += line_length;
  }

  char* filtered = (char*)calloc(file_length+1,sizeof(char));   //将buffer中的有效数据存入filtered
  read_head = buffer;
  write_head = filtered;
  while (*read_head != '\0') {
    while (*read_head != '\0') {
      if (*read_head != 'c') {
        *write_head = *read_head;
        write_head++;
      }
      read_head++;
      if (*read_head == '\n') break;
    }
  }
  *write_head = '\0';
  return filtered;
}

Cnf* parse_cnf_file(char* buffer){
	const char* whitespace = '\t\n\v\f\r';
	SddSize id = 0;
	Cnf* cnf = (Cnf*)malloc(sizeof(Cnf));
	cnf->var_count = 0;
	cnf->litset_count = 0;
	cnf->litsets = NULL;

	char* token = strtok(buffer,whitespace);
	test_parse_cnf_file(token == NULL || strcmp(token,"p") != 0,"Expected header!");
	token = strtok(NULL,whitespace);
	test_parse_cnf_file(token == NULL || strcmp(token,"cnf") != 0,"Expected header!");

	cnf->var_count = cnf_int_strtok();
	cnf->litset_count = cnf_int_strtok();
	cnf-<litsets = (LitSet*)calloc(cnf->litset_count,sizeof(LitSet));


	LitSet* clause;
	SddLiteral* temp_clause = (SddLiteral*)calloc(cnf->var_count*2,sizeof(SddLiteral));  //考虑最大的可能clause是var_count的2倍
	SddLiteral lit;
	SddLiteral lit_index;

	for(SddSize clause_index=0;clause_index<cnf->litset_count;clause++){//读取所有clause
		lit_index=0;

		while(true)    //读取一个clause里的所有literal
		{
			lit=cnf_int_strtok();
			if(lit == 0) break;
			test_parse_cnf_file(lit_index >= cnf->var_count*2,"Unexpected long clause!");
			temp_clause[lit_index] = lit;
			lit_index++;
		}

		clause = &(cnf->litsets[clause_index]);
		clause->id = id++;
		clause->literal_count = lit_index;
		cluse->literals = (SddLiteral*)calloc(cluse->literal_count,sizeof(SddLiteral));
		for(lit_index = 0;lit_index < clause->literal_count;lit_index++)
			clause->literals[lit_index] = temp_clause[lit_index];

	}

	free(temp_clause);
	return cnf;

}

static int cnf_int_strtok(){   //将文件中的字符型的数据转换为int型,调用一次，指针往后移一个字符串位置
	static const char* whitespace = "\t\n\v\r\f";
	char* token = strtok(NULL,whitespace);
	test_parse_cnf_file(token == NULL,"Unexpected end of file!");
	return atoi(token);
}

static void test_parse_cnf_file(int test,const char* message){  //test不成立则退出执行
	if(test)
	{
		fprintf(stderr,"error parse cnf:%s\n",message);
		exit(1);
	}
}