#include "assembler.h"

/* The function gets a name of an operaton, and the line after it, and the func writes in the code_image the suitable words.
the func returns VALID_OPERATION (which its value is 0) is the operation is valid, and if its not it return as follows:
it returns NOT_INTEGER (which its value is -6) if one of the operands is not an integer, it returns DOUBLE_COMMA (which its value is -5) if there is multiple consecutive commas, it returns END_COMMA (which its value is -4) if the line ends with an extra comma, it returns ERROR_NAME (which its value is -3) if the name of the operation is not valid,  it returns NUM_OPERANDS (which its value is -2) if the number of operands doesnt match the kind of the operation and it returns KIND_OPERANDS (which its value is -1) if the addressing modes of the operand is not valid */
int operation(char * op_name, char * line)
{
	int error = VALID_OPERATION;/* this will store a num that implies the kind of the error there is, or VALID_OPERANDS if everything is valid */
	int source, destination;/* this will store the addressing mode of the source and the destination */
	int i;
	char *op1, *op2;/* this will store the two operands */
	char *stop = ",";/* this will be used in order to seperate one operand from the other */
	struct function op[] = {{"mov",  0 , 0}, {"cmp", 1, 0}, {"add", 2, 10}, {"sub", 2, 11},
	 {"lea", 4, 0}, {"clr", 5, 10}, {"not",5, 11}, {"inc", 5, 12}, {"dec", 5, 13}, {"jmp",  9, 10}, 
	{"bne", 9, 11}, {"jsr",9, 12},{"red",12, 0}, {"prn", 13, 0},{"rts",14, 0}, {"stop",15, 0}, {NULL,  0, 0}};/* this is  an array which stores the names of the opeations, their opcode and their funct */
	
	while(isspace(*line))
		line++;/* skip the spaces at the begining of the line */
	if((error = is_valid_line(line)) != VALID_LINE)
		return error;
	op1 = clear_space(strtok(line, stop));/* reads the first operand and clear the spaces */
	op2 = clear_space(strtok(NULL, stop));/* reads the second operand and clear the spaces */
	if(!op1)
	{
		source = -1;
		destination = -1;
	}else if(!op2)
	{
		source = -1;
		destination = addressing_modes(op1);
		if(destination == INVALID_IMMEDIATE)
			return NOT_INTEGER;
	}else
	{
		source = addressing_modes(op1);
		destination = addressing_modes(op2);
		if(destination == INVALID_IMMEDIATE || source == INVALID_IMMEDIATE)
			return NOT_INTEGER;
	}

	error = is_valid_operand(op_name, source, destination);	

	for(i = 0; op[i].name; i++)
		if(!strcmp(op[i].name,op_name))
			{
				code_image[IC-INITIAL_ADDRESS].address = IC;
				code_image[IC-INITIAL_ADDRESS].word = first_word(op[i].opcode, op[i].funct, (source == -1 ? 0: source), (destination == -1 ? 0 : destination));
				code_image[IC-INITIAL_ADDRESS].field = 'A';
				IC++;
				if(source != -1)
				{
					code_image[IC-INITIAL_ADDRESS].address = IC;
					code_image[IC-INITIAL_ADDRESS].word = operand_word(source, op1);
					IC++;
					code_image[IC-INITIAL_ADDRESS].address = IC;
					code_image[IC-INITIAL_ADDRESS].word = operand_word(destination, op2); 
					IC++;
				}
				else if(destination != -1)
				{
					code_image[IC-INITIAL_ADDRESS].address = IC;
					code_image[IC-INITIAL_ADDRESS].word =  operand_word(destination, op1); 
					IC++;
				}
				break;
			}
	if(!op[i].name)/* if the name of the op_name doesnt exist */
		error = ERROR_NAME;
	return error;
}

/* The func gets an addressing modes of a operand and the operand itself, and returns the word in machine code */
int operand_word(int address_mode, char * operand)
{
	if(address_mode == 0)
	{
		code_image[IC-INITIAL_ADDRESS].field = 'A';
		return MASK & atoi(++operand);/* MASK deletes the bits after the 12 first bits, because the registers have only 12 bits */
	}
	else if (address_mode == 1)
	{
		code_image[IC-INITIAL_ADDRESS].field = 'R';
		add_label(IC, operand);
		return 0;
	}
	else if (address_mode == 2)
	{
		code_image[IC-INITIAL_ADDRESS].field = 'A';
		add_label(IC, ++operand);
		return 0;
	}
	else 
	{
		code_image[IC-INITIAL_ADDRESS].field = 'A';
		return (01 << atoi(++operand));
	}
}

/* The func gets a string and returns its addressing modes */
int addressing_modes(char * operand)
{
	if(operand[0] == '#')
	{
		int is_valid;
		strtoi(++operand, &is_valid);
		if(!is_valid)
			return INVALID_IMMEDIATE;
		return IMMEDIATE;
	}
	else if(operand[0] == '%')
		return RELATIVE;
	else if((operand[0] == 'r') && (operand[1] >= '0' && operand[1] <= '7') && (strlen(operand) == 2 || isspace(operand[2])))/* check if the operand is a register */
		return REGISTER;
	else 
		return DIRECT;
}

/* The func gets opcode, funct, source and destination and returns the first word in machine code*/
int first_word(int opcode, int funct, int source, int destination)
{
	int word = 0;
	
	word = (opcode <<= BITS_TO_OPCODE);
	word |= (funct <<= BITS_TO_FUNCT);
	word |= (source <<= BITS_TO_SOURCE);
	word |= destination;
	return word;
}
