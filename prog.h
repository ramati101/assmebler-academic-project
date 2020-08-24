#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define FILE_START_INDEX 1 /* Start index for run on argv array */
#define MAX_LINE_LENGTH 81 /* Maximum number of characters in line */
#define MAX_LABAL_LENGTH 71 /* Maximum length of label(+ \0) */
#define MAX_OPERATION_LENGTH 15 /* Maximum length of operation */
#define NUM_OF_OP 21 /* 16 Operation and 5 guidance */
#define NUM_OF_SAVED_WORDS 29 /* Saved words of assembly */
#define NO_OPERATION_FOUND -1 /* The method findOp return -1 if he cant find the operation in the opTable */
#define MAX_NUM_PARAM 35 /* Max number of parameters */
#define MAX_LENGTH_PARAM 71 /* Max length of each parameter */
#define MAX_LENGTH_STRING 71 /* Max length of string */
#define yes 0
#define in 1
#define no 2
#define norm 0
#define note 1
#define inword 0
#define space 1
#define FOREVER 1
#define THE_FIRST_PARAMETER 0
#define THE_SEC_PARAMETER 1
#define END_OF_ATLEAST1_PARAMETER_OPERATIONS 1
#define NO_TYPE_FOUND 9
#define EQUAL 0
#define RESET_TO_DEFAULT 0
#define START_OF_CODE 100
#define MAX_MEMORY_WORD_LENGTH 10
#define END_LIST_OF_OPERATION 16
#define THE_FIRST_RUN_VALID 2
#define MAX_EXTENSION 5
#define ERROR_SEC_RUN 0
#define DIRECT 1
#define MAT 2
#define RETURN_MAT_LABEL_LENGTH 1
#define REGULAR 0



typedef struct operationTable
{

		char *opName;
		int   opcode;
		int   numOfParam;
		int 	param1Type[5];
		int 	param2Type[5];

}opTable;




/* Pointer to symbol table node. */
typedef struct symbolTable *stp;

/* Struct which describe symbol table. */
typedef struct symbolTable
{
	char param[MAX_LABAL_LENGTH];
	int address;
	int extern_param;
	int operation;
	int typeOfOp;
	stp next;
}st;



/* Pointer to data table. */
typedef struct dataTable *dtp;

/* Struct which describe data table. */
typedef struct dataTable
{
	int address;
	int data; 
	dtp next;
}dt;


/* Pointer to action statment. */
typedef struct actionStatement *actSP;

/* Struct which represent every action statement in file. */
typedef struct actionStatement
{
	char action[MAX_OPERATION_LENGTH];
	char param1[MAX_LENGTH_PARAM];
	char param2[MAX_LENGTH_PARAM];
	int address;
	int line;
	actSP next;
}actS;

char *addExtension( char*, char* );
void printList(char*, stp, dtp, actSP, int, int );
int convertFromIntToMemWord( int, int  );
char *convertIntToNew4Base(int, int);
int returnIcOfSymbol( stp, char * );


/* First run file - function declartion. */
int firstRun( FILE *, stp*, dtp*, actSP*, int*, int* );
int findNumParam( char * );
int findOp( char * );
int findSavedWord( char * );
int typeOfParam(char *, int  );
int checkValidationParam( char *, char *, char * );
int findIc( char *, char *, char *, int, int );
int wordCodeOp( char *,char *,char *);
int addDataToDataTable(int[], int, int, dtp *);
int addMatToDataTable(int[], int, dtp*);
int addStringToDataTable(char *, int, dtp*);
int addInfoToDataTable(int, int, dtp *);
void addInfoToSymbolTable(char *, int, int, int, int, stp*);
void addActionToActionStatementList(char *, char *, char *, int, int, actSP*);
void updateSymbolTable(stp*, int);
void updateDataTable(dtp *, int );
int isGuide(char *);

/* Second run file - function declartion. */
int secondRun( stp, actSP);
char *convertDecimalToNew4Base(int);
char *convertBinaryToNew4Base(char *);
void printToEntryFile(char *, int);
void printToExternFile(char *, int);
void printCodeToOBFile();
void printDataToOBFile(dtp);		
int checkLine( stp , actSP  )	;		
void removeSymbolTable(stp *);
