#include "assembler.h"

/* The func gets a pointer file in which it prints the code image and data image */
void print_assembler(FILE * fp)
{
	int i;
	fprintf(fp, "   %d %d\n", IC-INITIAL_ADDRESS, DC);
	
	/* prints the code image */
	for(i = 0; i<IC-INITIAL_ADDRESS; i++)
	{
		fprintf(fp, "%04d %.3X %c\n", code_image[i].address, code_image[i].word, code_image[i].field);
	}

	/* prints the data image */
	for(i = 0; i<DC; i++)
	{
		fprintf(fp, "%04d %.3X A\n", data_image[i].address + IC, data_image[i].word);
	}

}

/* The func gets a number which implies of the kind of the error, the name of the file the number of line and the label in which the error occures, and the func prints a suitable error messsage to the stdout */ 
void invalid_label(int error, char * fname, int line, char * label)
{	
	if(error == RESERVED)
		printf("File %s line %d: Error, the label %s is a reserved word\n",fname, line,label);
	else if(error ==SYNTAX)
		printf("File %s line %d: Error, invalid syntax to label %s\n",fname, line,label);
	else if(error == EXIST)
		printf("File %s line %d: Error, the label %s had already been defined\n",fname,  line,label);
	else /* if the error is "LENGTH" */
		printf("File %s line %d: Error, the label %s is too long\n",fname,  line, label);
}

/* The func gets a number which implies of the kind of the error, the name of the file and the number of line in which the error occures, and the func prints a suitable error messsage to stdout */ 
void invalid_data(int error, char * fname, int line)
{
	if(error == DOUBLE_COMMA)
		printf("File %s line %d: Error, multiple consecutive comma\n",fname, line);
	else if(error == END_COMMA)
		printf("File %s line %d: Error, extra comma in the end of the line\n",fname, line);
	else if(error == INVALID_NUMBER)
		printf("File %s line %d: Error, invalid number\n",fname, line);
	else/*if error is "NO_DATA"*/
		printf("File %s line %d: Error, no data after .data\n",fname, line);
}

/* The func gets a number which implies of the kind of the error, the name of the file and the number of line in which the error occures, and the func prints a suitable error messsage to the stdout */ 
void invalid_string(int error, char * fname, int line)
{
	if(error == LEFT_QUOTATION_MARK)
		printf("File %s line %d: Error, left quotation mark is missing\n",fname, line);
	else if(error == RIGHT_QUOTATION_MARK)
		printf("File %s line %d: Error, right quotation mark is missing\n",fname, line);
	else/*if error is "NO_STRING"*/
		printf("File %s line %d: Error, no string after the command .string\n",fname, line);
}

/* The func gets a number which implies of the kind of the error, the name of the file and the number of line in which the error occures, and the func prints a suitable error messsage to the stdout */ 
void invalid_operation(int error, char * fname, int line)
{
	if(error == NUM_OPERANDS)
		printf("File %s line %d: Error, number of operands doesnt match the kind of the operation\n",fname, line);
	else if(error == ERROR_NAME)
		printf("File %s line %d: Error, invalid name of operation\n",fname, line);
	else if(error == DOUBLE_COMMA)
		printf("File %s line %d: Error, multiple consecutive comma\n",fname, line);
	else if(error == END_COMMA)
		printf("File %s line %d: Error, extra comma in the end of the line\n",fname, line);
	else if(error == NOT_INTEGER)
		printf("File %s line %d: Error, one of the operands is not an integer \n",fname, line);
	else/* temp is KIND_OPERANDS */
		printf("File %s line %d: Error, invalid addressing modes\n",fname, line);
}

