#include "assembler.h"

int DC = 0;
struct data data_image[MAX_DATA_IMAGE];

/* The func gets the line after the command .data and saves the numbers in the data image.
The func returns VALID_DATA (which its value is 0) if the data is valid, and if its not valid, the func returns as follows:
it returns INVALID_NUMBER (which its value is -2) if the if there is an invalid number in the line, it returns NO_DATA (which its value is -1) if the line doesnt consist any numbers, it returns DOUBLE_COMMA (which its value is -5) if there is multiple consecutive commas, and it returns END_COMMA (which its value is -4) if the line ends with an extra comma */
int save_data(char * line)
{
	int is_valid_num;/* this will help us determine if the numbers we get are valid */ 
	char * stop = ",";/* this will be used in order to seperate one number from the other */
	char * data;/* this will store the parts of the line between the commas */
	int num;/* this will store the num we want to save in the data image */
	int kind_of_error;/* this will store the kind of the error that the func valid_data return*/

	if((kind_of_error = is_valid_line(line)) != VALID_LINE)
		return kind_of_error;

	data = strtok(line, stop);
	num = strtoi(data, &is_valid_num);

	if(is_valid_num == -1)
		return NO_DATA;
	else if(is_valid_num == 0)
		return INVALID_NUMBER;

	data_image[DC].address = DC;
	data_image[DC].word = MASK & num;
	DC++;

	while((data = strtok(NULL, stop)))
	{
		num = strtoi(data, &is_valid_num);

		if(is_valid_num == 0)
			return INVALID_NUMBER;

		data_image[DC].address = DC;
		data_image[DC].word = MASK & num;
		DC++;
	}
	return VALID_DATA;
}

/* The func gets the string after the command .string and saves every letter in the string in the data image, and afterwards a null.
The func returns VALID_STRING (which its value is 0) if the string is valid, and if its not valid, the func returns as follows:
it returns NO_STRING (which its value is -3) if there is no string after the command .string, it returns LEFT_QUOTATION_MARK (which its value is -2) if the left quotation mark is missing, and returns RIGHT_QUOTATION_MARK (which its value is -1) if the right quotation mark is missing */
int save_string(char * line)
{
	int i;

	if(!(line = clear_space(line)))/* if there is no string at all after the command .string */
		return NO_STRING;

	if(*line != '"')
	{
		return LEFT_QUOTATION_MARK;/* if there is no " at the begining of the string it is error*/
	}

	if(line[strlen(line)-1] != '"')
	{
		return RIGHT_QUOTATION_MARK;/* if there is no " at the end of the string it is error*/
	}
	
	/* saves every charachter in the data image */
	for(i = 1; i < (strlen(line)-1); i++)
	{
		data_image[DC].address = DC;
		data_image[DC].word = line[i];
		DC++;
	}

	/* null in the end of the string */
	data_image[DC].address = DC;
	data_image[DC].word = 0;
	DC++;

	return VALID_STRING;
}

/* The func gets the line after the command ".entry", and saves the name of the label in the symbol table with address of -1 and attribute N. It returns 1 when everything is valid and 0 otherwise */
int entry_data(char * line)
{
	char label[MAXLINE];/* this will store the label that is written in the line we got */
	char attribute = '\0';/* this will store the attribute of the label if it had already been declared */ 

	sscanf(line, "%s", label);
	seek_symbol(label, &attribute);
	if(attribute == 'E')/* if the label has already been declared as extern it is an error*/
		return 0;

	add_symbol(-1, label, 'N');/* N means entry. The func stores the the label with address of -1, because right now it cant know what is the address of the entry label. After the program finishes reading the file for the first time, the func "update symbols" updates the addresses of the entry labels */

	return 1;
}

/* The func gets the line after the command ".extern", and saves the name of the label in the symbol table with address of -1 and attribute E. It returns 1 when everything is valid and 0 otherwise */
int extern_data(char * line, char * arg)
{
	char label[MAXLINE];/* this will store the label that is written in the line we got */
	char attribute = '\0';/* this will store the attribute of the label if it had already been declared */ 
	int address;/* this will store the address of the label if it had already been declared */ 

	sscanf(line, "%s", label);
	address = seek_symbol(label, &attribute);/* check if the label had already been declared */
	if((address != -1 && attribute !='E')||attribute == 'N')/* if the label has already been declared,i.e. the address is not -1, it is an error, because it is a contradiction to the fact that the label is extern. However, if the attribute is E it is valid, because there is no problem declaring twice the same label as extern.  If the attribute is N, that means the label was declared as entry and it is an error */
		return 0;

	add_symbol(0, label, 'E');/* save the extern label with address 0 and attribute 'E' - extern */

	return 1;
}
