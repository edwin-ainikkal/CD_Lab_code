#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TableLength 30

typedef struct token{
    char name[100];
    unsigned int row, col;
    int id;
    char type[20];
    char dtype[20];
} token;

typedef struct node{
    int idx;
    char name[20];
    int size;
    char type[20];
}node;

token TokenList[1000];
int tidx = 0;

int rowc = 1, colc = 0;

typedef struct ListElement{
    node n;
    struct ListElement *next;
}ListElement;

ListElement* TABLE[TableLength];

void InitializeSym(){
    for(int i=0;i<TableLength;i++)
        TABLE[i]=NULL;
}

int HashSym(char *str){
    unsigned int hash = 0;
    for(int i = 0; str[i] != '\0'; i++){
        hash = (hash * 31 + str[i]) % TableLength;
    }
    return hash;
}

void DisplaySym(){
    for(int i = 0; i < TableLength; i++){
        ListElement *temp = TABLE[i];
        if(temp != NULL){
            printf("Index %d:\n", i);
            while(temp != NULL){
                printf("  Name: %s, Type: %s, ID: %d, Size: %d\n",
                       temp->n.name, temp->n.type,
                       temp->n.idx, temp->n.size);
                temp = temp->next;
            }
        }
    }
}

int SearchSym(char *str){
    int index = HashSym(str);
    ListElement *temp = TABLE[index];

    while(temp != NULL){
        if(strcmp(temp->n.name, str) == 0){
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

int InsertSym(token tk){
    int val = HashSym(tk.name);
    if(SearchSym(tk.name) == 1){
        return val;
    }

    ListElement *cur = (ListElement *)malloc(sizeof(ListElement));
    cur->n.idx = val;
    strcpy(cur->n.name, tk.name);
    strcpy(cur->n.type, tk.dtype);

    if (strcmp(tk.dtype, "int") == 0)
        cur->n.size = 4;
    else if (strcmp(tk.dtype, "char") == 0)
        cur->n.size = 1;
    else if (strcmp(tk.dtype, "double") == 0)
        cur->n.size = 8;
    else
        cur->n.size = 0;

    cur->next = NULL;

    if(TABLE[val] == NULL){
        TABLE[val] = cur;
    } else {
        ListElement *ele = TABLE[val];
        while(ele->next != NULL)
            ele = ele->next;
        ele->next = cur;
    }

    return val;
}

char newfgetc(FILE* fp) {
    char t = fgetc(fp);
    if(t != EOF) colc++;
    if(t == '\n') {
        rowc++;
        colc = 0;
    }
    return t;
}

int isAssignmentOp(char buff[]){
    if(strcmp(buff,"=")==0 ||
       strcmp(buff,"+=")==0 ||
       strcmp(buff,"-=")==0 ||
       strcmp(buff,"*=")==0 ||
       strcmp(buff,"/=")==0 ||
       strcmp(buff,"%=")==0)
        return 1;
    else
        return 0;
}
int isArithmeticOp(char buff[]){
    if(strcmp(buff,"+")==0 ||
       strcmp(buff,"-")==0 ||
       strcmp(buff,"*")==0 ||
       strcmp(buff,"/")==0 ||
       strcmp(buff,"%")==0 ||
       strcmp(buff,"++")==0 ||
       strcmp(buff,"--")==0)
        return 1;
    else
        return 0;
}


int isRelationalOp(char buff[]){
    if(strcmp(buff,"<")==0 || strcmp(buff,">")==0 ||
       strcmp(buff,"<=")==0 || strcmp(buff,">=")==0 ||
       strcmp(buff,"==")==0 || strcmp(buff,"!=")==0)
        return 1;
    return 0;
}

int isLogicalOp(char buff[]){
    if(strcmp(buff,"&&")==0 || strcmp(buff,"||")==0 ||
       strcmp(buff,"!")==0||strcmp(buff,"&")==0)
        return 1;
    return 0;
}

int isSpecialSym(char buff) {
    if (buff == ';' || buff == ',' || buff == '(' ||
        buff == ')' || buff == '{' || buff == '}' ||
        buff == '[' || buff == ']')
        return 1;
    return 0;
}

int isKeyword(char buff[]){
    char keywords[12][10] =
    {"int","char","double","if","else",
     "while","for","return","scanf","printf","void","main"};

    for(int i=0;i<10;i++){
        if(strcmp(buff,keywords[i])==0)
            return 1;
    }
    return 0;
}

int isNumber(char buff[]){
    for(int i=0;buff[i];i++){
        if(!isdigit(buff[i]))
            return 0;
    }
    return 1;
}

int isString(char buff[]){
    int len = strlen(buff);
    if(len >= 2 && buff[0]=='"' && buff[len-1]=='"')
        return 1;
    return 0;
}

int isCharac(char buff[]){
    if(strlen(buff)==3 && buff[0]=='\'' && buff[2]=='\'')
        return 1;
    return 0;
}

int isIdentifier(char buff[]){
    if(!isalpha(buff[0]) && buff[0]!='_')
        return 0;

    for(int i=1;buff[i];i++){
        if(!isalnum(buff[i]))
            return 0;
    }

    if(isKeyword(buff))
        return 0;

    return 1;
}

void getTokenType(char src[], char dest[], char type[], int* n){

    if(isKeyword(src)){
        strcpy(dest, src);
        strcpy(type, src);
    }
    else if(isIdentifier(src)){
        strcpy(dest, src);
        strcpy(type, "id");
        if(n) (*n)++;
    }
    else if(isNumber(src)){
        strcpy(dest, src);
        strcpy(type,"num");
    }
    else if(isString(src)){
        strcpy(dest, src);
        strcpy(type,"string");
    }
    else if(isCharac(src)){
        strcpy(dest, src);
        strcpy(type,"char");
    }
    else if(isAssignmentOp(src) ||
            isArithmeticOp(src) ||
            isRelationalOp(src) ||
            isLogicalOp(src)){
        strcpy(dest, src);
        strcpy(type, "operator");
    }
    else if(strlen(src)==1 && isSpecialSym(src[0])){
        dest[0]=src[0];
        dest[1]='\0';
        strcpy(type,"special");
    }
}


token getNextToken(FILE *fp) {

    static int c = 0;
    static int flag = 0;
    static char lastdtype[20] = "";

    char buf[100];
    int i = 0;

    token tk;
    tk.id = 0;
    strcpy(tk.dtype, "");

    if (!flag) {
        c = newfgetc(fp);
        flag = 1;
    }

    while (c != EOF) {
        if (c == '#') {
            while (c != '\n' && c != EOF)
                c = newfgetc(fp);
            continue;
        }

        if (isspace(c)) {
            c = newfgetc(fp);
            continue;
        }

        if (c == '"') {
            int start = colc;
            i = 0;

            buf[i++] = c;
            c = newfgetc(fp);

            while (c != EOF) {
                buf[i++] = c;
                if (c == '"' && buf[i-2] != '\\')
                    break;
                c = newfgetc(fp);
            }

            buf[i] = '\0';

            getTokenType(buf, tk.name, tk.type, NULL);
            tk.row = rowc;
            tk.col = start;

            c = newfgetc(fp);
            return tk;
        }

        if (c == '\'') {
            int start = colc;
            i = 0;

            buf[i++] = c;
            c = newfgetc(fp);

            if (c != EOF) {
                buf[i++] = c;
                c = newfgetc(fp);
            }

            if (c == '\'') {
                buf[i++] = c;
                buf[i] = '\0';

                getTokenType(buf, tk.name, tk.type, NULL);
                tk.row = rowc;
                tk.col = start;

                c = newfgetc(fp);
                return tk;
            }
        }

        if (strchr("+-*/%=<>!&|", c)) {

            int start = colc;
            buf[0] = c;
            int next = newfgetc(fp);

            if ((c == '+' && next == '+') ||
                (c == '-' && next == '-') ||
                (c == '=' && next == '=') ||
                (c == '!' && next == '=') ||
                (c == '<' && next == '=') ||
                (c == '>' && next == '=') ||
                (c == '&' && next == '&') ||
                (c == '|' && next == '|') ||
                (c == '+' && next == '=') ||
                (c == '-' && next == '=') ||
                (c == '*' && next == '=') ||
                (c == '/' && next == '=') ||
                (c == '%' && next == '=')) {

                buf[1] = next;
                buf[2] = '\0';
                c = newfgetc(fp);
            }
            else {
                buf[1] = '\0';
                c = next;
            }

            getTokenType(buf, tk.name, tk.type, NULL);
            tk.row = rowc;
            tk.col = start;
            return tk;
        }

        if (isSpecialSym(c)) {

            buf[0] = c;
            buf[1] = '\0';

            getTokenType(buf, tk.name, tk.type, NULL);
            tk.row = rowc;
            tk.col = colc;

            c = newfgetc(fp);
            return tk;
        }

        i = 0;
        int start = colc;

        while (c != EOF &&
               !isspace(c) &&
               !isSpecialSym(c) &&
               !strchr("+-*/%=<>!&|", c)) {

            buf[i++] = c;
            c = newfgetc(fp);
        }

        buf[i] = '\0';

        getTokenType(buf, tk.name, tk.type, NULL);
        tk.row = rowc;
        tk.col = start;

        if (isKeyword(tk.name)) {
            strcpy(lastdtype, tk.name);
        }
        else if (strcmp(tk.type, "id") == 0) {
            strcpy(tk.dtype, lastdtype);
            tk.id = InsertSym(tk);
        }

        return tk;
    }

    strcpy(tk.name, "EOF");
    strcpy(tk.type, "EOF");
    tk.row = rowc;
    tk.col = colc;
    tk.id = 0;

    return tk;
}

int main() {

    InitializeSym();

    FILE *fp = fopen("token.c", "r");
    if (fp == NULL) {
        printf("Cannot open file\n");
        exit(0);
    }

    token tk;

    do {
        tk = getNextToken(fp);
        TokenList[tidx++] = tk;

        printf("<%s, %s, %d, %d, %d>\n",
               tk.name, tk.type,
               tk.row, tk.col, tk.id);

    } while (strcmp(tk.name, "EOF") != 0);

    printf("\nSymbol Table:\n");
    DisplaySym();

    fclose(fp);
    return 0;
}
