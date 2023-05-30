#include "assembler.h"

/* The func gets a name of a label and return VALID_LABEL (which its value is 0) if it is valid and non zero if its not valid: It returns RESERVED (which its value is -2) if the label is a reserved word, it returns SYNTAX (which its value is -1) if the label doesnt begin with a letter, or it consists more than just letteres and digits, it returns EXIT (which its value is -3)  if the same name has already been defined and returns LENGTH (which its value is -4) if the label is too long */
int is_valid_label(char * label)
{
	char *reserved_words[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop", "r0", "r1","r2","r3","r4","r5","r6", "r7", ".enrty", ".extern", ".string", ".data", NULL};/* this is an array of reserved words*/
	int i;
	char attribute;
	
	/* check whether the label is a reserved words */
	for(i = 0; reserved_words[i]; i++)
		if(!strcmp(label, reserved_words[i]))
			return RESERVED;

	/* check whether the first character of the label is not a letter */
	if(!isalpha(label[0]))
		return SYNTAX;

	/* check whether the label does not consist just from letters and digits */
	for(i = 0; i < (strlen(label)); i++)
		if(!isalnum(label[i]))
			return SYNTAX;

	/* check whether the label is too long */
	if(strlen(label) > 31)
		return LENGTH;

	/* check whether the same name of label has already been defined */
	if(seek_symbol(label, &attribute) != -1)
		return EXIST;

	return VALID_LABEL;
}

/* The func gets a line that has some values that are seperated with comma. The func returns END_COMMA (which its value is -4) if the line ends with an extra comma,
returns DOUBLE_COMMA (which its value is -5) if there is multiple consecutive commas, and returns VALID_LINE (which its value is 0) if everything is o.k. */
int is_valid_line(char * line)
{
	if(!(line = clear_space(line)))
		return VALID_LINE;/* if there is nothing in the line the line is valid */
	
	/* check if there is extra comma at the end of the line */
	if(line[strlen(line)-1] == ',')
		return END_COMMA;
	/* check if there is multiple consecutive comma in the line */
	while(*line++)
		if(*line == ',')
		{
			while(isspace(*++line));/* skip all the spaces */
			if(*line == ',')
				return DOUBLE_COMMA; /* if there is one comma, and after some spaces there is another comma, it means there is multiple consecutive commas and the line is not valid */
		}
			
	return VALID_LINE;
}

/* The func gets the name of the operation, the addressing mode of the source operand (if there is no source operand it gets -1) and the addressing mode of the target operand (if there is no target operand it gets -1).
The func returns VALID_OPERARATION (which its value is 0) if everyting is valid,  and if its not valid, the func returns as follows: it returns NUM_OPERANDS (which its value is -2) if the number of operands doesnt match the operation, and it returns KIND_OPERANDS (which its value is -1)  if the addressing mode of the source or target operand doesnt match the valid addressing modes that the operation can get */
int is_valid_operand(char * operation, int source,int destination)
{
	if(!strcmp(operation, "mov") || !strcmp(operation, "add") || !strcmp(operation, "sub"))
	{
		if(source == -1 || destination == -1)
			return NUM_OPERANDS;
		if(source == RELATIVE || destination == IMMEDIATE || destination == RELATIVE)
			return KIND_OPERANDS;
	}
	else if(!strcmp(operation, "cmp"))
	{
		if(source == -1 || destination == -1)
			return NUM_OPERANDS;
		if(source == RELATIVE || destination == RELATIVE)
			return KIND_OPERANDS;
	}
	else if(!strcmp(operation, "lea"))
	{
		if(source == -1 || destination == -1)		
			return NUM_OPERANDS;
		if(source != DIRECT || destination == IMMEDIATE || destination == RELATIVE)
			return KIND_OPERANDS;
	}
	else if(!strcmp(operation, "clr") || !strcmp(operation, "not") || !strcmp(operation, "inc") || !strcmp(operation, "dec") || !strcmp(operation, "red"))
	{
		if(source != -1 || destination == -1)	
			return NUM_OPERANDS;
		if(destination == IMMEDIATE || destination == RELATIVE)
			return KIND_OPERANDS;
	}
	else if(!strcmp(operation, "jmp") || !strcmp(operation, "bne") || !strcmp(operation, "jsr"))
	{
		if(source != -1 || destination == -1)	
			return NUM_OPERANDS;
		if(destination == IMMEDIATE || destination == REGISTER)
			return KIND_OPERANDS;
	}
	else if(!strcmp(operation, "prn"))
	{
		if(source != -1 || destination == -1)		
			return NUM_OPERANDS;
		if(destination == RELATIVE)
			return KIND_OPERANDS;
	}
	else/* operation is "rts" or "stp" */
	{
		if(source != -1 || destination != -1)	
			return NUM_OPERANDS;
	}

	return VALID_OPERATION;
}

/* The func gets a string and a pointer to int, converts the string to int and returns the converted integer. If the number in str was an integer (ignoring leading or ending white spaces), the int variable is stored with 1, if it is not a valid integer it isstored with 0, and if str is empty or has only white spaces it is stored with -1, and the func returns 0 */
int strtoi(char * str, int * is_valid)
{
	double num;/* in this we will store the double num converted from str */
	char * p;/* after we convert str to double this will point to the first unconverted suffix in str */

	*is_valid = 1;
	str = clear_space(str);
	if(!str)
	{
		*is_valid = -1;
		return 0;
	}

	num = strtod(str, &p);

	if(*p)/* if *p is not null that means the data is not valid */
		*is_valid = 0;
	else if(num != (int)num)/* if the number is not an integer */
		*is_valid = 0;

	return (int)num;
}



