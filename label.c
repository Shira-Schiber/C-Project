#include "assembler.h"

static Symbolp head_symbol = NULL;

/* The func gets a label, address and an arrtibute and saves them in in the symbol_table */ 
void add_symbol(int address, char * label, char attribute)
{
	Symbolp pnode;/* this will store the new node we add to the symbol table */

	pnode = (Symbolp) malloc(sizeof(struct symbol_table_node));

	pnode->address = address;
	strcpy(pnode->name , label);
	pnode->attribute = attribute;
	pnode->next = head_symbol;
	head_symbol = pnode;
}

/* The func gets a pointer to the entries file, prints the entry labels and their address in the file, and update the address of all the symbols that are attributed as data */  
void update_symbols(FILE * fp)
{	
	Symbolp pnode = head_symbol;/* This node will go over the symbol table*/

	while(pnode)
	{
		if(pnode->attribute == 'D')
			pnode->address = pnode->address + IC;/* every symbol that its attribute is D, it appends the IC, because in the machine code the data image is seperated from the code image, and it appears after it */ 
		if(pnode->address == -1)/* that means that the symbol was declared as entry */
		{
			char attribute;
			pnode->address = seek_symbol(pnode->name, &attribute);/* I dont need the field, because I already know that the symbol is entry */
			fprintf(fp,"%s %04d\n", pnode->name, pnode->address);/* print the name and the address of the entry label in the entries file */
		}
		pnode = pnode->next;
	}
}

/* The func gets a name of a label and a pointer to a char variable. The func returns the address of the label and stores in the char variable the attribute of the label. The func returns -1 if there is no such a symbol in the symbol table */
int seek_symbol(char * label, char * attribute)
{
	Symbolp pnode = head_symbol;/* This node will go over the symbol table */

	while(pnode)
	{
		if(!strcmp(pnode->name, label) && pnode->address != -1)/* if the address of the node is -1 that means it is the entry node we appended, so we continue to seek the label because we want to know its address */
		{
			*attribute = pnode->attribute;
			return pnode->address;
		}
		pnode = pnode->next;
	}
	return -1;/* if it doesnt find the name of the label in the symbol table it returns -1*/
}

static Labelp head_label = NULL;

/* The func gets an address and a name of a label and adds it to the label list. The label list is used in the second pass, when we will go over the label list and in that way we will be able to know which words in the code image we need to complete*/
void add_label(int address,  char * name)
{
	Labelp pnode;/* this will store the new node we add to the label list*/
	pnode = (Labelp) malloc(sizeof(struct label_node));
	
	pnode->next = head_label;
	pnode->address = address;
	strcpy(pnode->label, name);
	head_label = pnode;
}

/* The func gets a file pointer to the external file and the name of the file, and prints the extern labels and the address in it. In addition, it completes all the words in the code image that we couldnt know them in the first pass. The func retuns 0 if everything is o.k., and if the assembler file uses a label which wasnt defined- it prints an error message and returns 1 */
int second_pass(FILE * fp, char * fname)
{
	Labelp pnode = head_label;/* This node will go over the label list */
	int iserror = 0;/* If we find an error we will change the value of iserror to 1. This variable will help us know which value we need to return from the func */

	while(pnode)
	{
		int i = pnode->address - INITIAL_ADDRESS;/* i is the index of the line in the code image we need to change */
		char field;/* This will store the attribute of the label we seek in the symbol table*/

		if((code_image[i].word = seek_symbol(pnode->label, &field)) == -1)
		{
			iserror = 1;
			printf("File %s: Error, label %s is used but is not defined\n", fname, pnode->label);
		}
						
		if(code_image[i].field == 'A')
			code_image[i].word = MASK & (code_image[i].word - code_image[i].address);/* if the field is A that means we need to calculate the jump. We use  MASK in order to delete the bits after the 12 first bits */
		else if(field == 'E')
		{
			code_image[i].field = 'E';	
			fprintf(fp,"%s %04d\n", pnode->label, code_image[i].address);/* print the name and the address of the extern label in the external file */
		}
		pnode = pnode->next;
	}
	return iserror;
}

/* The func frees the storage allocated for the nodes in the symbol table, and resets it */ 
void free_symbol_table()
{
	Symbolp pnode = head_symbol;/* this node will be used to go over the list */
	Symbolp pnext = pnode;/* this node will be allways one node before pnode. We need this node because when we free one node, the next node is lost. so we use pnext to save the address of the next node */
	
	/* free the storage allocated for the nodes */
	while(pnext)
	{
		pnext = pnode-> next; 
		free(pnode);
		pnode = pnext;
	}

	/* reset the list */
	head_symbol = NULL;
}

/* The func frees the storage allocated for the nodes in the label list, and resets it*/ 
void free_label_list()
{
	Labelp pnode = head_label;/* this will be used to go over the list */
	Labelp pnext = head_label;/* this node will be allways one node before pnode. We need this node because when we free one node, the next node is lost. so we use pnext to save address of the next node */

	/* free the storage allocated for the nodes */
	while(pnext)
	{
		pnext = pnode-> next; 
		free(pnode);
		pnode = pnext;
	}

	/* reset the list */
	head_label = NULL;
}


