#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function for checking whether it's a keyword or not !
static FILE *source_fp, *dest_fp;

struct pair {
    char* label;
    char value[20]; // maximum number of accepted word length is 20
};

struct pair pairs[5][10];
int k=0,l=0; // for the array of structs (pairs)
int bufferCnt = 0; // Buffer Counter for concatenating chars and cutting them

int isKeyword(char buffer[]){

    // Samy Code of manipulating the keywords or adding some of them below .. ( PYTHON )

    char keywords[33][10] = {"False", "class", "finally", "is", "return", "None", "continue",
                             "for", "lambda", "try", "and", "del", "global", "not", "with",
                             "True", "def", "from", "nonlocal", "while", "as", "elif",
                             "if", "or", "yield", "assert", "else", "import",
                             "pass", "break", "except", "in", "raise"};

    int i, flag = 0; // the return value which would let us know if it's a reserved keyword or not

    // Checking if the word is a keyword or not using STRCMP BuiltIn Function

    for (i = 0; i < 32; ++i)
    {
        if (strcmp(keywords[i], buffer) == 0)
        {
            flag = 1;
            break;
        }
    }

    return flag;
}

// remove end of line
void remove_end_of_line(char line[]){
    int i = 0;
    while (1)
    {
        if (line[i] != '\n')
            i++;
        else
            break;
    }
    line[i] = '\0';
}

// check for extension we accept .hapd only
void extention(const char path[]){
    int i = 0;
    char check[5];
    while (1)
    {
        if (path[i] == '.')
        {
            check[0] = path[i + 1];
            check[1] = path[i + 2];
            check[2] = path[i + 3];
            check[3] = path[i + 4];
            check[4] = '\0';
            if (strcmp(check, "hapd") != 0)
            {
                printf("error in your file extension, your file extension is %s ,please make sure to make it .hapd\n", check);
                exit(0);
            }
            else

                break;
        }
        i++;
    }
}

/* added for operators (Salim) */
int isOp(char *c){
    char operators[] = "+-*/%=><!";
    int i = 0;
    for (i = 0; i < 9; i++)
    { // 6 to be replaced with size of operators array
        if (*c == operators[i])
            return 1;
    }
    return -1;
}

char* return_path(){
    // Nassar Code of reading the path of the file and checking it's extension

    FILE *fp;
    // Enter PATH of file which want to compile
    char PATH[100];
    printf("Enter Full PAth to your file\n");
    fgets(PATH, 100, stdin);
    remove_end_of_line(PATH); // remove \n from input file
    extention(PATH);


    return PATH;
}

void init_io(FILE **fp, FILE *std, const char mode[], const char fn[]) {
    if (fn[0] == '\0')
        *fp = std;
    else if(fn[0] == '-' && fn[1] == 'p'){
        //from a certain given path
        fn = return_path();                  // File Pointer to open the file needed..
        if ((*fp = fopen(fn, mode)) == NULL)
            printf("Failed to open file");
    }
    else{
        //from argv (in the same folder)
        extention(fn);
        if ((*fp = fopen(fn, mode)) == NULL)
            printf("Failed to open file");
    }
}

void checkKwdOrIdnt(char buffer[]){
    if (buffer[0] == '\0'){}
    else if (isKeyword(buffer) == 1){
    	pairs[k][l] = (struct pair){"keyword","\0"};
        strcpy(pairs[k][l].value, buffer);
        l++;
       // printf("%s is keyword\n", buffer);
    }
    else if (isKeyword(buffer) == 0){
    	pairs[k][l] = (struct pair){"identifier","\0"};
        strcpy(pairs[k][l].value, buffer);
        l++;
      //  printf("%s is indentifier\n", buffer);
    }
}

void checkOperators (char *ch, char buffer[]){

    int first = 0; // to catch the operators
    first = isOp(ch);
    if (first == 1)   //first char is operator
    {
    	checkKwdOrIdnt(buffer);
        bufferCnt = 0;
        char ch2 = fgetc(source_fp);
        int second = isOp(&ch2);

        if (second == 1)
        {
            char ch3 = fgetc(source_fp);
            int third = isOp(&ch3);
            if(third == 1) printf("There is no such operator as: %c%c%c\n", *ch, ch2, ch3);
            else
            {
                if ((ch2 == '=' && (*ch == '+' || *ch == '-' || *ch == '<' || *ch == '>')) || (ch2 == '=' && *ch == '=') || (ch2 == '+' && *ch == '+') || (ch2 == '-' && *ch == '-'))
                {
                    char op[2];
                    op[0] = *ch;
                    op[1] = ch2;
                    pairs[k][l] = (struct pair){"double operator",""};  /// check with strcat()
                    strcpy(pairs[k][l].value, op);
                    l++;
                   // printf("%c%c is the double operator\n", *ch, ch2); // here we have the double operators
                }
                else
                {
                   // printf("There is no such operator as: %c%c\n", *ch, ch2);
                }
                *ch = ch3;
            }
        }
        else
        {
        	pairs[k][l] = (struct pair){"single operator",*ch};
        	l++;
           // printf("%c is the single operator\n", *ch); // here we have the double operator
            *ch = ch2;
        }
    }
}


void checkSpecial(char *ch, char buffer[]){
    int i;
    char special[13]="[]{},.();:'\"\?";
    for (i = 0; i < 13; ++i)
    {
        if (*ch == special[i])
        {
            if (bufferCnt != 0)
            {
            	bufferCnt = 0;
                checkKwdOrIdnt(buffer);
            }
            pairs[k][l] = (struct pair){"special character",*ch};
            l++;
            //printf("%c is special character\n", *ch);
        }
    }
}


void check_all(){
    char ch;
    char buffer[20];
    int i;
    while ((ch = fgetc(source_fp)) != EOF)
    {

        checkOperators(&ch, buffer);

        checkSpecial(&ch, buffer);

        if (isalnum(ch))
    	{
        	buffer[bufferCnt++] = ch;
        	buffer[bufferCnt] = '\0';  // added this to modify the buffer dynamically
    	}
        if ((ch == ' ' || ch == '\n' ) && (bufferCnt != 0))
        {
            checkKwdOrIdnt(buffer);
            bufferCnt = 0;
            buffer[bufferCnt] = '\0';  // to check with it in the checkKwdOrIdnt for nulls
        }

        if(ch=='\n'){
        	k++;
        	l++;
	    	pairs[k][l] = (struct pair){'\0', '\0'};
	    	l=0;
	    	// I will add this to exit when pressing enter in stdin mode to test the output of (pairs)
	    	//break;
        }


    }
    if(bufferCnt != 0)checkKwdOrIdnt(buffer);
    int j=0;
/*
    for (i = 0; i < 5; i++){
        while(pairs[i][j].label != '\0'){
    		printf("%s: %s\n",  pairs[i][j].label, pairs[i][j].value);
            j++;
        }
        j=0;
    }*/
}
char* a[40];
int top=-1,i;

void error(){
printf("Syntax Error");
}
void push(char k[])
{
    int i = 0;
    char m[30];

    //printf("m=%s\n",m);
    char* args[10];
    args[i] = strtok(k," ");
    //printf("[0]%s\n",args[i]);
    while(args[i] != NULL){
        i++;
        args[i] = strtok(NULL," ");
        //printf("[%d]%s\n",i,args[i]);
    }
    printf("args=%s\n",args[i-1]);
    for(i=i-1;i>=0;i--){
        a[++top]=args[i];
        //printf("a[%d]=%s\n",top,a[top]);
    }

    for(i=0;i<=top;i++){
        printf("[%d]%s\n",i,a[i]);
    }
}
char* TOS()        //Returns TOP of the Stack
{
  return a[top];
}
void pop()       //Pops 1 element from the Stack
{
  a[top--]='\0';
  printf("top=%s\n",a[top]);
}
void display()  //Displays Elements Of Stack
{
  for(i=0;i<=top;i++)
    printf("%s",a[i]);
}

char non_terminal[31][5]={"S","D","AT","ST","CT","BT","DSC","AS","X","ET","ARE","A","AP","B","DP","C","SE","BE","ALE","DSE","DT","EX","IFE","CON","V","OP","LE","WE","FE","I","IC"};
  char terminal[42][20]={";","int","float","double","long","string","char","bool","stack","queue","vector","identifier","=","[number]","[number][number]","+","-","*","/","number","(",")","letters","true","false","character","<",">","if",":","else:","finish","gr","sm","greq","smeq","eq","neq","while","for","in","$"};
  char t[31][42][30]={"$","D ;","D ;","D ;","D ;","D ;","D ;","D ;","D ;","D ;","D ;","AS ;","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","EX ;","$","e","e","$","$","$","$","$","$","EX ;","EX ;","$","e",
  "$","AT AS","AT AS","AT AS","AT AS","ST AS","CT AS","BT AS","DSC AS","DSC AS","DSC AS","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","int","float","double","long","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","string","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","char","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","bool","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","stack","queue","vector","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","identifier X","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "e","$","$","$","$","$","$","$","$","$","$","$","ET","[number]","[number][number]","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","ARE","$","$","$","$","ARE","$","$","ARE","ARE","$","SE","BE","BE","ALE","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","A","$","$","$","$","A","$","$","A","A","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","B AP","$","$","$","$","B AP","$","$","B AP","B AP","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "e","$","$","$","$","$","$","$","$","$","$","$","$","$","$","+ B AP","- B AP","$","$","$","$","e","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","C DP","$","$","$","$","C DP","$","$","C DP","C DP","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "e","$","$","$","$","$","$","$","$","$","$","$","$","$","$","e","e","* C DP","/ C DP","$","$","e","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","identifier","$","$","$","$","- C","$","$","number","( A )","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","letters","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","true","false","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","character","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","< DT > identifier","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","AT","AT","AT","AT","ST","CT","BT","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","IFE","$","$","$","$","$","$","$","$","$","LE","LE","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","if CON : S else: S finish","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","( V OP V )","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","identifier","$","$","$","$","$","$","$","number","$","$","$","$","$","character","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","gr","sm","greq","smeq","eq","neq","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","WE","FE","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","while CON : S finish","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","for I in IC : S finish","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","identifier","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$",
  "$","$","$","$","$","$","$","$","$","$","$","identifier","$","$","$","$","$","$","$","number","( IC )","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$","$"
  };
int main(int argc, char *argv[]){

    // get data from stdin or path or from argv
    init_io(&source_fp, stdin,  "r",  argc > 1 ? argv[1] : "");
    int i=0,j=0,ir=0,ic=0,k;
    char* term,*nonterm,*test,*r,ah[5];
    check_all();
    char line[10]="S $";
    push(line);

    for (i = 0; i < 5; i++){
        while(pairs[i][j].label != '\0'){
            int n=0;
    		printf("%s: %s\n",  pairs[i][j].label, pairs[i][j].value);
            if(!strcmp(TOS(),term))
                {
                    pop();
                    test=TOS();
                    printf("test=%s\n",test);
                    if(strcmp(term,";"))
                    j++;
                }
            if(strcmp(term,";")&&strcmp(TOS(),"$")){
                term=pairs[i][j].value;
                printf("term=%s\n",term);
                nonterm=TOS();
                printf("nonterm=%s\n",nonterm);
                for (n=0;n<31;n++){
                    k=strcmp(non_terminal[n],nonterm);
                    if(k==0){
                        ir=n;
                        break;
                    }
                }
                printf("ir=%d\n",ir);
                if(ir!=n){
                    error();
                    break;
                }

                for (n=0;n<42;n++){
                    k=strcmp(terminal[n],term);
                    if(k==0){
                        ic=n;
                        break;
                    }
                }
                printf("ic=%d\n",ic);

                if(ic!=n){
                //printf("label=%s\n",pairs[i][j].label);
                if(!strcmp(term,"'")|| term[0]=='"'){
                    j++;
                    term=pairs[i][j].value;
                    if((term>="a" && term<="z")||(term>="A" && term<="Z")||(term>="0" && term<="9")){
                        term="character";
                        ic=25;
                    }
                    else{
                        term="letters";
                        ic=22;
                    }
                }
                else if(!strcmp(pairs[i][j].label,"identifier"))
                {
                    term="identifier";
                    ic=11;
                    printf("newic=%d\n",ic);
                }
                else if(term>="0" && term <="1000000"){
                    term="number";
                    ic=19;
                }
                else if(term[0]=='['){
                    int end=strlen(term);
                    for(n=0;n<end;n++){
                        if((term[n]>='a' && term[n]<='z')||(term[n]>='A' && term[n]<='Z')){
                            error();
                            break;
                        }
                        if(term[end-1]!=']'){
                            error();
                            break;
                        }
                        if (term[n]==']' && n+1==end){
                            term="[number]";
                            ic=13;
                        }
                        else if(term[n]==']' && term[n+1]=='[' &&term[end-1]==']'){
                            term="[number][number]";
                            ic=14;
                            break;
                        }
                    }

                }
                else{
                    error();
                    break;
                }
                }
                printf("topbefore=%s\n",TOS());
                //strcpy(ah,t[ir][ic]);
               // printf("topbefore=%s\n",TOS());
                pop();
                r=t[ir][ic];
                //strcpy(r,t[ir][ic]);
                //printf("old r=%s\n",r);
                push(r);                                 //error t[ir][ic] changed
                //strcpy(t[ir][ic],ah);
                //printf("table=%s\n",t[ir][ic]);
                //printf("new r=%s\n",r);
                if(!strcmp(TOS(),"e"))
                {
                    pop();

                }
            }
            printf("TOS=%s\nterm=%s\n",TOS(),term);
            if(!strcmp(TOS(),"$")&&!strcmp(term,";")){
                printf("\n Given String is accepted\n\n");
                break;

            }
            if(!strcmp(TOS(),"$")){
                error();
                break;
            }
            }
            push("S");
            term="";
            j=0;
    }

    fclose(source_fp); // close the file

    return 0;
}