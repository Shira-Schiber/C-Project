#include "assembler.h" 

int IC = INITIAL_ADDRESS;
struct code code_image[MAX_CODE_IMAGE];

int main(int argc, char **argv)
{
	FILE *asfp;/* This will store a pointer to the assembler file */
	FILE *obfp;/* This will store a pointer to the object file */

	FILE *extfp = NULL;/* This will store a pointer to the external file */
	FILE *entfp = NULL;/* This will store a pointer to the entry file */

	if(argc == 1)/* If there is no names of files in the command line */
	{
		printf("Error: missing name of files in the command line\n");
		return 1;
	}
	while(--argc)
	{
		int iserror = 0;/* If we find an error in the assembler file we will change the value of iserror to 1. This variable will help us determine if we need to create the output files */ 
		int is_extern = 0;/* If there is an extern line the value will change to 1. This will help us to know if the program needs to open an extern file or not */
		int is_entry = 0;/* If there is an entry line the value will change to 1. This will help us to know if the program needs to open an entry file or not */
		char line[MAXLINE];/* This will be used in rorder to scan line after line in the assembler code */
		char fname[MAXWORD];/* This will store the name of the files */
		char command[MAXWORD];/* This will store the first word in every line */
		int counter = 0;/* counts the lines in the source file */

		++argv;
		strcpy(fname, *argv);	
		strcat(fname, ".as");
		if((asfp = fopen(fname, "r")) == NULL)
		{
			printf("Error: cannot open %s file\n", fname);
			continue;/* continue to the other files */
		}
		(fname)[strlen(fname)-3] = '\0';/* deleting the postfix "as" in fname */

		strcat(fname, ".ob");
		if((obfp = fopen(fname, "w")) == NULL)
		{
			fclose(asfp);
			printf("Error: cannot open %s file\n", fname);
			continue;/* continue to the other files */
		}
		(fname)[strlen(fname)-3] = '\0';/* deleting the postfix "ob" in fname */

		/* first time reading the file */
		while(fgets(line, MAXLINE, asfp) != NULL)
		{	
			char * pline = line;

			counter++;/* count the lines in the input file- the assembler file */
			line[strlen(line)-1] = '\0';/* turn the \n in the end of the line to null */

			pline = clear_space(pline);

			if(sscanf(line, "%s", command) < 1 || command[0] == ';')
				continue;/* if there is a blank line or a line that start with ";" , continue to the next line */

			pline+= strlen(command);/* deleting the command in the line */
			
			if(command[strlen(command) - 1] == ':')/* if the command is a label */
			{
				int temp;
				char label[MAXWORD];
				strcpy(label,command);
				label[strlen(label) - 1] = '\0';/* deleting the : in the end of the label*/

				if((temp = is_valid_label(label)) != VALID_LABEL)/* if the label is not valid */
				{
					iserror = 1;
					invalid_label(temp, fname, counter, label);	
				}

				sscanf(pline, "%s", command);
				pline = clear_space(pline);
				pline+= strlen(command);/* deleting the command in the line */

				if(!strcmp(command,".entry"))
					printf("File %s line %d: Warning, label before .entry \n",fname, counter);
				else if(!strcmp(command,".extern"))
					printf("File %s line %d: Warning, label before .extern \n",fname, counter);
				else if(command[0] == '.')/* if the command after the label is .string or .data */
					add_symbol(DC, label, 'D');/* the D means "data" */
				else
					add_symbol(IC, label, 'C');/* the C means "code" */
			}

			if(!strcmp(command,".data"))
			{
				int kind_of_error;
				if((kind_of_error = save_data(pline)) != VALID_DATA)
				{
					iserror = 1;
					invalid_data(kind_of_error, fname, counter);
				}
			}else if(!strcmp(command,".string"))
			{
				int kind_of_error;
				if((kind_of_error = save_string(pline)) != VALID_STRING)
				{
					iserror = 1;
					invalid_string(kind_of_error,fname, counter);
				}
			}else if(!strcmp(command,".entry"))
			{
				is_entry = 1;
				if(!entry_data(pline))
				{
					iserror = 1;
					printf("File %s line %d: Error, cannot daclare the label as an entry label, ",fname, counter);
 					printf("because the label had already been declared as extern\n");
				}
			}else if(!strcmp(command,".extern"))
			{
				is_extern = 1;
				if(!extern_data(pline, fname))
				{
					iserror = 1;
					printf("File %s line %d: Error, cannot define the label as extern \n",fname, counter);
				}
			}
			else
			{
				int kind_of_error;
				if((kind_of_error=operation(command, pline)) != VALID_OPERATION)
				{
					iserror = 1;
					invalid_operation(kind_of_error, fname, counter);
				}
			}	
			
		}

		/*second time reading the file*/
		fclose(asfp);


		strcat(fname, ".ent");
		if((entfp = fopen(fname, "w")) == NULL)
		{
			iserror = 1;
			printf("Error: cannot open %s file\n", fname);
			fclose(obfp);
			continue;/*continue to the other files*/
		}
		if(!is_entry)
			remove(fname);/* if there is no entry label remove the entries file */
		fname[strlen(fname)-4] = '\0';
		update_symbols(entfp);
		fclose(entfp);
		
		strcat(fname, ".ext");
		if((extfp = fopen(fname, "w")) == NULL)
		{
			iserror = 1;
			printf("Error: cannot open %s file\n", fname);
			fclose(obfp);
			continue;/*continue to the other files*/
		}
		if(!is_extern)
			remove(fname);/* if there is no extern label remove the externals file */
		fname[strlen(fname)-4] = '\0';
		
		if(second_pass(extfp, fname))
			iserror = 1;
		fclose(extfp);

		if(!iserror)
			print_assembler(obfp);
		else
			remove_files(fname);
		fclose(obfp);

		reset();
	}
	return 0;
}/* end of main */

/* the func gets a name, for example "prog", and removes the object file, the external file, and the entry file. In our example, it removes: "prog.ob", "prog.ent" and "prog.ext" */
void remove_files(char * fname)
{
	strcat(fname, ".ob");
	remove(fname);
	(fname)[strlen(fname)-3] = '\0';/* deleting the postfix "ob" in the name of the program */
	strcat(fname, ".ent");
	remove(fname);
	(fname)[strlen(fname)-4] = '\0';/* deleting the postfix "ent" in the name of the program */
	strcat(fname, ".ext");
	remove(fname);
}

/* The func resets the data image, code image, symbol table and label list and makes the program ready for the next file */
void reset()
{
	DC = 0;
	IC =  INITIAL_ADDRESS;

	free_symbol_table();
	free_label_list();
}

/* The func gets a string, delete all the spaces at the begining of the string and at the end of the string, and returns the clear string.
If it gets NULL, an empty string, or a string with only white spaces, it returns NULL */
char * clear_space (char * str)
{
	int i;
	
	/* if str is NULL it returns NULL */
	if(!str)
		return NULL;
	/* skip all the spaces at the begining of the string */
	while(isspace(*str))
		str++;
	/* if str is empty, it returns NULL */
	if(!*str)
		return NULL;
	/* skip all the spaces at the end of the string */
	for(i = (strlen(str)-1); isspace(str[i]); i--);
	str[i+1] = '\0';
	return str;
}



