#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE 81
#define MAXWORD 81
#define INITIAL_ADDRESS 100
#define MAX_DATA_IMAGE 1000
#define MAX_CODE_IMAGE 1000
#define BITS_TO_OPCODE 8
#define BITS_TO_FUNCT 4
#define BITS_TO_SOURCE 2
#define MASK 07777

struct function
{
	char * name;
	int opcode;
	int funct;
};

typedef struct symbol_table_node * Symbolp;/* Symbolp is a name to a pointer to a struct symbol_table_node*/

struct symbol_table_node
{
	int address;
	char name[MAXWORD];
	char attribute;	
	Symbolp next;

};

typedef struct label_node * Labelp;/* Labelp is a name to a pointer to a struct label_node*/

/*This struct helps us to build a label list which will be used in the second pass, when we will go over it and in that way we will be able to know which words in the code image we need to fill*/
struct label_node
{
	int address;
	char label[MAXWORD];
	struct label_node * next;
};

/* This struct is for the code image */
struct code
{
	int address;
	int word;
	char field;
};

/* This struct is for the data image */
struct data
{
	int address;
	int word;
};

enum{INVALID_IMMEDIATE = -1,IMMEDIATE, DIRECT, RELATIVE, REGISTER};
enum{INVALID_NUMBER= -2, NO_DATA, VALID_DATA};
enum{DOUBLE_COMMA= -5, END_COMMA = -4,  VALID_LINE = 0};
enum{LENGTH = -4, EXIST, RESERVED, SYNTAX, VALID_LABEL};
enum{NOT_INTEGER = -6,ERROR_NAME=-3, NUM_OPERANDS, KIND_OPERANDS, VALID_OPERATION};
enum{NO_STRING = -3,LEFT_QUOTATION_MARK, RIGHT_QUOTATION_MARK, VALID_STRING};


extern int DC;
extern int IC;
extern struct data data_image[MAX_DATA_IMAGE];
extern struct code code_image[MAX_CODE_IMAGE];

/* **********************************in file assembler.c**************************** */
void reset(void);
/* The func resets the data image, code image, symbol table and label list and makes the program ready for the next file */

void remove_files(char * fname);
/* The func gets a name, for example "prog", and removes the object file, the external file, and the entry file. In our example, it removes: "prog.ob", "prog.ent" and "prog.ext" */

char * clear_space (char * str);
/* The func gets a string, delete all the spaces at the begining of the string and at the end of the string, and returns the clear string. If it gets NULL, an empty string, or a string with only white spaces, it returns NULL */


/* ********************************in file data.c************************************** */
int save_data(char * line);
/* The func gets the line after the command .data and saves the numbers in the data image.
The func returns VALID_DATA (which its value is 0) if the data is valid, and if its not valid, the func returns as follows:
it returns INVALID_NUMBER (which its value is -2) if the if there is an invalid number in the line, it returns NO_DATA (which its value is -1) if the line doesnt consist any numbers, it returns DOUBLE_COMMA (which its value is -5) if there is multiple consecutive commas, and it returns END_COMMA (which its value is -4) if the line ends with an extra comma */

int save_string(char * line);
/* The func gets the string after the command .string and saves every letter in the string in the data image, and afterwards a null.
The func returns VALID_STRING (which its value is 0) if the string is valid, and if its not valid, the func returns as follows:
it returns NO_STRING (which its value is -3) if there is no string after the command .string, it returns LEFT_QUOTATION_MARK (which its value is -2) if the left quotation mark is missing, and returns RIGHT_QUOTATION_MARK (which its value is -1) if the right quotation mark is missing */

int entry_data(char * line);
/* The func gets the line after the command ".entry", and saves the name of the label in the symbol table with address of -1 and attribute N. It returns 1 when everything is valid and 0 otherwise */

int extern_data(char * line, char * arg);
/* The func gets the line after the command ".extern", and saves the name of the label in the symbol table with address of -1 and attribute E. It returns 1 when everything is valid and 0 otherwise */


/* ******************************in file operation.c***************************************** */
int operation(char * op_name, char * line);
/* The function gets a name of an operation, and the line after it, and the func writes in the code_image the suitable words.
the func returns VALID_OPERATION (which its value is 0) is the operation is valid, and if its not it return as follows:
it returns NOT_INTEGER (which its value is -6) if one of the operands is not an integer, it returns DOUBLE_COMMA (which its value is -5) if there is multiple consecutive commas, it returns END_COMMA (which its value is -4) if the line ends with an extra comma, it returns ERROR_NAME (which its value is -3) if the name of the operation is not valid,  it returns NUM_OPERANDS (which its value is -2) if the number of operands doesnt match the kind of the operation and it returns KIND_OPERANDS (which its value is -1) if the addressing modes of the operand is not valid */

int operand_word(int address_mode, char * operand);
/* The func gets an addressing modes of a operand and the operand itself, and returns the word in machine code */

int addressing_modes(char * operand);
/* The func gets a string and returns its addressing modes */

int first_word(int opcode, int funct, int source, int destination);
/* The func gets opcode, funct, source and destination and returns the first word in machine code*/


/* *******************************in file label.c********************************************** */
void add_symbol(int address, char * label, char attribute);
/* The func gets a label, address and an arrtibute and saves them in in the symbol_table */ 

void update_symbols(FILE * fp);
/* The func gets a pointer to the entries file, prints the entry labels and their address in the file, and update the address of all the symbols that are attributed as data */ 

int seek_symbol(char * label, char * attribute);
/* The func gets a name of a label and a pointer to a char variable. The func returns the address of the label and stores in the char variable the attribute of the label. The func returns -1 if there is no such a symbol in the symbol table */

void free_symbol_table(void);
/* The func frees the storage allocated for the nodes in the symbol table, and resets it */ 

void add_label (int address,  char * name);
/* The func gets an address and a name of a label and adds it to the label list. The label list is used in the second pass, when we will go over the label list and in that way we will be able to know which words in the code image we need to complete*/

int second_pass(FILE * fp, char * fname);
/* The func gets a file pointer to the external file and the name of the file, and prints the extern labels and the address in it. In addition, it completes all the words in the code image that we couldnt know them in the first pass. The func retuns 0 if everything is o.k., and if the assembler file uses a label which wasnt defined- it prints an error message and returns 1 */

void free_label_list(void);
/* The func frees the storage allocated for the nodes in the label list, and resets it*/ 


/* **********************************in file valid.c************************************** */
int is_valid_label(char * label);
/* The func gets a name of a label and return VALID_LABEL (which its value is 0) if it is valid and non zero if its not valid: It returns RESERVED (which its value is -2) if the label is a reserved word, it returns SYNTAX (which its value is -1) if the label doesnt begin with a letter, or it consists more than just letteres and digits, it returns EXIT (which its value is -3)  if the same name has already been defined and returns LENGTH (which its value is -4) if the label is too long */

int is_valid_line(char * line);
/* The func gets a line that has some values that are seperated with comma. The func returns END_COMMA (which its value is -4) if the line ends with an extra comma,
returns DOUBLE_COMMA (which its value is -5) if there is multiple consecutive commas, and returns VALID_LINE (which its value is 0) if everything is o.k. */

int is_valid_operand(char * operation,int source,int destination);
/* The func gets the name of the operation, the addressing mode of the source operand (if there is no source operand it gets -1) and the addressing mode of the target operand (if there is no target operand it gets -1).
The func returns VALID_OPERARATION (which its value is 0) if everyting is valid,  and if its not valid, the func returns as follows: it returns NUM_OPERANDS (which its value is -2) if the number of operands doesnt match the operation, and it returns KIND_OPERANDS (which its value is -1)  if the addressing mode of the source or target operand doesnt match the valid addressing modes that the operation can get */

int strtoi(char * str, int * is_valid);/* The func gets a string and a pointer to int, converts the string to int and returns the converted integer. If the number in str was an integer (ignoring leading or ending white spaces), the int variable is stored with 1, if it is not a valid integer it isstored with 0, and if str is empty or has only white spaces it is stored with -1, and the func returns 0 */


/* ************************************in file print.c*************************************** */
void print_assembler(FILE * fp);
/* The func gets a pointer file in which it prints the code image and data image */

void invalid_label(int error, char * fname, int line, char * label);
/* The func gets a number which implies of the kind of the error, the name of the file the number of line and the label in which the error occures, and the func prints a suitable error messsage to the stdout */ 

void invalid_data(int error, char * fname, int line);
/* The func gets a number which implies of the kind of the error, the name of the file and the number of line in which the error occures, and the func prints a suitable error messsage to stdout */ 

void invalid_string(int error, char * fname, int line);
/* The func gets a number which implies of the kind of the error, the name of the file and the number of line in which the error occures, and the func prints a suitable error messsage to the stdout */

void invalid_operation(int error, char * fname, int line);
/* The func gets a number which implies of the kind of the error, the name of the file and the number of line in which the error occures, and the func prints a suitable error messsage to the stdout */ 

