#include "prog.h"



/* saved words of assembly we need to check the label is not one of them */

char *savedWords[NUM_OF_SAVED_WORDS] = {"mov",
                    									  "cmp",
                     									  "add",
                     									  "sub",
                   									    "not",
                  							    	  "clr",
                  									    "lea",
                   									    "inc",
                  									    "dec",
                    									  "jmp",
                    									  "bne",
                    									  "red",
                    									  "prn",
                      								  "jsr",
                     								    "rts",
                    								    "stop",
                   								      "data",
                   								      "string",
                   								      "mat",
                    								    "entry",
                    								    "extern",
                    								    "r0",
                    								    "r1",
                    								    "r2",
                 								        "r3",
                   								      "r4",
                    								    "r5",
                   								      "r6",
                   								      "r7"};


opTable opArr[NUM_OF_OP] = {{"mov",0,2,{0,1,2,3,9},{1,2,3,9}},
			   									  {"cmp",1,2,{0,1,2,3,9},{0,1,2,3,9}},
          								  {"add",2,2,{0,1,2,3,9},{1,2,3,9}},
         								    {"sub",3,2,{0,1,2,3,9},{1,2,3,9}},
			   								    {"not",4,1,{1,2,3,9},{9}},
			  								    {"clr",5,1,{1,2,3,9},{9}},
			  								    {"lea",6,2,{1,2,9},{1,2,3,9}},
			   								    {"inc",7,1,{1,2,3,9},{9}},
			  								    {"dec",8,1,{1,2,3,9},{9}},
			  								    {"jmp",9,1,{1,2,3,9},{9}},
			  								    {"bne",10,1,{1,2,3,9},{9}},
        								    {"red",11,1,{1,2,3,9},{9}},
			  								    {"prn",12,1,{0,1,2,3,9},{9}},
			  								    {"jsr",13,1,{1,2,3,9},{9}},
			  								    {"rts",14,0,{9},{9}},
			   							  	  {"stop",15,0,{9},{9}},
			   							  	  {".data",16,1,{9},{9}},
			  								    {".string",17,1,{9},{9}},
			  								    {".mat",18,5,{9},{9}},
			  								    {".entry",19,1,{1,9},{9}},
			  								    {".extern",20,1,{1,9},{9}} };




/* pass line line of the input file
	 on every line pass char by char and check if the line valid
	 and explit the line to tokens,
	 if the line invalid put error alert and ignore this line. */

int firstRun(FILE *p, stp *sth, dtp *dth, actSP *actsh, int* lastIc, int* lastDc)
{


				/* variable declartion: */


	int line_char_counter; /* count the number of chars in line(max 80) */

  int labelFound;

	int isOperation;

	int opFound;
	int guideOp ;

	int firstParam ;
	int secParam ;

  /* .string */
	int firstQmark;
	int secQmark;

	/* .data */
	int sign;
	int firstData; /* we turn to no after we pass the first char of .data param */

	/*	.mat	*/
	int numOf2brack;
	int leftBrack;
	int mat1param;
	int mat2param;
	int matLen;

  int first_char_line ;
	int firstWord ; /* we turn to no if we pass the first word in line */

  int state ;

 /* if the line start with ; its a note line, we need to skip it */
	int lineType;

	int error_in_file; /* we turn to yes if we found error */
	int error_in_line;

	int comma; /* if we find a comma between params */

	int lineIndex;
	int c,i;
	int reset_all_variable;
	int ic, dc;

	char operation[MAX_OPERATION_LENGTH];
	char label[MAX_LABAL_LENGTH];

	/* parameters array */
	int paramInd;
	char parameters[MAX_NUM_PARAM][MAX_LENGTH_PARAM];
	int intParam[MAX_NUM_PARAM];

	/* temp array */
	int  tempInd;
  char temp[MAX_LABAL_LENGTH];


	reset_all_variable = yes;
	error_in_file = no;
	lineIndex = 0;
	ic = START_OF_CODE;
	dc = RESET_TO_DEFAULT;



	/*

			start of while loop that read char by char and analyzing it
			and split the line to tokens.

	*/


	while( FOREVER )
	{

		if( reset_all_variable == yes )
		{
				/* reset all variable */
				lineType = norm;
				lineIndex++;
				line_char_counter = RESET_TO_DEFAULT;

				error_in_line = no;
				first_char_line = yes;
				firstWord = yes;
				state = space;
				guideOp = no;

				labelFound = no;
				opFound = no;
				comma = no;

				leftBrack = no;
				mat1param = no;
				mat2param = no;
				numOf2brack = RESET_TO_DEFAULT;
				matLen = RESET_TO_DEFAULT;

				firstParam = no;
				secParam = no;

				firstQmark = no;
				secQmark = no;

				sign = no;
				firstData = yes;

				tempInd = RESET_TO_DEFAULT;
				paramInd = RESET_TO_DEFAULT;

				reset_all_variable = no;

		}


		c = fgetc(p);

		line_char_counter++;



		/* when we get to the '\n' or EOF we make some checks on the line
			 and if the line is valid we send it to the next step */

		if( (c == '\n' || c == EOF) && lineType==norm && first_char_line==no )
		{


				/* number of chars in line > 80 */
				if( line_char_counter > MAX_LINE_LENGTH )
				{

						fprintf(stderr, "error found in line %d, too much characters in line, max 80 characters allowed in one line\n",lineIndex);
						error_in_line=yes;

				}


				/* <label:>  '\n' - only a lable in the line */
				if( labelFound==yes && opFound==no && state==space )
				{

						fprintf(stderr, "error found in line %d, missing operation.\n",lineIndex);
						error_in_line=yes;

				}


				/* <string>'\n'   or  <label:> <string>'\n'
			 		check if the string is a operation and if he is we'll
					check if he is valid operation and
					check if he get zero parameters.	 */

				else if( (firstWord==yes || (labelFound==yes && opFound==no))  && state==inword )
				{

						temp[tempInd] = '\0';
						opFound = yes;
						strcpy(operation, temp);
						tempInd = RESET_TO_DEFAULT;
						state = space;
						firstWord = no;


						/* check if the operation exist */
						if( findNumParam(operation)==NO_OPERATION_FOUND )
						{

								fprintf(stderr, "error found in line %d, missing valid operation\n",lineIndex);
								error_in_line=yes;

						}

						/* check if the operation gets more than zero param */
						else if( findNumParam(operation)>0 )
						{

								fprintf(stderr, "error found in line %d, missing parameters\n",lineIndex);
								error_in_line=yes;

						}

				}



				/* <operation> <parameter>'/n' or <operation> <parameter>,<parameter>'/n'
					 we in a middle of reading one of the parameters */
				else if( firstParam==in || secParam==in )
				{

						if( firstParam==in )
						{
								firstParam = yes;
								temp[tempInd] = '\0';
								strcpy( parameters[paramInd], temp );
								paramInd++;
								tempInd = RESET_TO_DEFAULT;
						}
						else
						{
								secParam = yes;
								temp[tempInd] = '\0';
								strcpy( parameters[paramInd], temp );
								paramInd++;
								tempInd = RESET_TO_DEFAULT;
						}

				}



				/* in a middle of string of .string(operation) */
				if( firstQmark==yes && secQmark==no )
				{

						fprintf(stderr, "error found in line %d, missing \" mark\n",lineIndex);

						error_in_line=yes;

				}



				/* check for all the operation(exept .mat and .data)
				 	if there is too few parameters */

				else if( (strcmp(operation,".mat")!=EQUAL && strcmp(operation,".data")!=EQUAL && opFound==yes && ((findNumParam(operation)==2 && secParam==no)||(findNumParam(operation)==1 && firstParam==no))) )
				{

						fprintf(stderr, "error found in line %d, fewer parameters\n",lineIndex);

						error_in_line=yes;

				}



				/* check if there is problem in parameters(numbers)
					after  <.data> ......    or after  <.mat[r1][r2]> ......  */
				if( strcmp(operation,".data")==EQUAL || numOf2brack==2 )
				{

						/* if the last char of parameter is a sign(+,-)
							 without a number */
						if( sign==yes )
						{
								fprintf(stderr, "error found in line %d, only a sign without a number\n",lineIndex);

								error_in_line=yes;
						}

						/*	in a middle of reading a parameter(number)	*/
						if( state==inword )
						{
								temp[tempInd] = '\0';
								strcpy( parameters[paramInd], temp );
								paramInd++;
								tempInd = RESET_TO_DEFAULT;
								sign = no;
								state = space;
						}

						/* the index of parameters == 0 at least 1 parameter required  */
						if( paramInd==0 )
						{
								fprintf(stderr, "error found in line %d, fewer parameters\n",lineIndex);

								error_in_line=yes;
						}

						/* ',' without a parameter after */
						if( comma==yes )
						{
								fprintf(stderr, "error found in line %d, a comma without a parameter after\n",lineIndex);

								error_in_line=yes;
						}


				}



				/* check if we get a valid size variables <[r1][r2]> for .mat */
				if( strcmp(operation,".mat")==EQUAL && numOf2brack<2 )
				{

						fprintf(stderr, "error found in line %d, invalid size variable of mat\n",lineIndex);
						error_in_line=yes;
				}


				/* check if we get the right number of parameters for the mat
						<.mat [r1][r2]>
						matLen = r1*r2 */
				if( numOf2brack==2 )
				{
						if( matLen < (paramInd-2) )
						{
								fprintf(stderr, "error found in line %d, too much parameters\n",lineIndex);

								error_in_line=yes;
						}
						if( matLen > (paramInd-2) )
						{
								fprintf(stderr, "error found in line %d, too few parameters\n",lineIndex);

								error_in_line=yes;
						}
				}



				/*	check the validation of the type of parameters for the operation
						for example: the operation <not> cant get one parameter from type
						1 or 2 or 3	*/
				if( findOp(operation)<END_OF_ATLEAST1_PARAMETER_OPERATIONS ||
						strcmp(operation,".entry")==EQUAL || strcmp(operation,".extern")==EQUAL )
				{

						if( checkValidationParam(operation,parameters[THE_FIRST_PARAMETER] ,parameters[THE_SEC_PARAMETER])==0 )
						{
								fprintf(stderr, "error found in line %d, one of the parameter is invalid\n",lineIndex);

								error_in_line=yes;
						}

				}


				if( error_in_line==yes )
						error_in_file = yes;



				if( error_in_line==no )
				{

						if( strcmp(operation, ".mat") == EQUAL ||
								strcmp(operation, ".data") == EQUAL )	
						{
								for( i=0; i<paramInd; i++ )
								{
										intParam[i] = atoi( parameters[i] );
								}
						}					

					/* 		  LINE IS VALID!  	  */
				


        if( findOp(operation) < END_LIST_OF_OPERATION )
                isOperation = 1;
        else
                isOperation = 0;


				if( strcmp(operation, ".extern") == EQUAL )
        		addInfoToSymbolTable(parameters[THE_FIRST_PARAMETER], ic, 1,isOperation, findOp(operation), sth);



        if( labelFound == yes &&  strcmp(operation, ".entry") != EQUAL &&strcmp(operation, ".extern") != EQUAL )
				{

						if( isOperation )	
            		addInfoToSymbolTable(label, ic, 0, isOperation, findOp(operation), sth);				else
								addInfoToSymbolTable(label, dc, 0, isOperation, findOp(operation), sth);
				}


        if( strcmp(operation, ".data") == EQUAL )
            dc = addDataToDataTable(intParam, paramInd, dc, dth);
        if( strcmp(operation, ".mat") == EQUAL )
            dc = addMatToDataTable(intParam, dc, dth);
        if( strcmp(operation, ".string") == EQUAL )
            dc = addStringToDataTable(parameters[THE_FIRST_PARAMETER], dc, dth);

				if( isOperation==1 || (strcmp(operation, ".entry") == EQUAL) )
						addActionToActionStatementList(operation, parameters[THE_FIRST_PARAMETER], parameters[THE_SEC_PARAMETER], ic, lineIndex, actsh);

				if( isOperation == 1 )
        		ic += findIc(operation,parameters[THE_FIRST_PARAMETER],parameters[THE_SEC_PARAMETER],paramInd,tempInd);

				

				}

				reset_all_variable = yes;

				if( c == EOF )
				{
						*lastIc = ic;
						*lastDc = dc;
						updateSymbolTable(sth, ic);
						updateDataTable(dth, ic);
						return error_in_file;
				}
				continue;

		}

    if( c == EOF && (first_char_line==yes || lineType == note) )
		{
				*lastIc = ic;
				*lastDc = dc;
				updateSymbolTable(sth, ic);
				updateDataTable(dth, ic);
				return error_in_file;
		}

		/* the first char in the line is ; so its a note line */

		if( (error_in_line==yes || (first_char_line==yes && (c == ';' || lineType == note))) && c !='\n' )
		{

				lineType = note;
				continue;

		}

		 /* we finish the note line */
		if( lineType == note && (c == '\n') )
		{
				if( error_in_line==yes )
						error_in_file = yes;
				reset_all_variable = yes;
				continue;
		}

 /* check if the first char in line is not an alphabetic and not a point */

		if( first_char_line==yes && (isalpha(c)==0) && c!='.' && c!=' '
 && c!='	' && c!='\n' )
		{

				fprintf(stderr, "error found in line %d, the first char in line have to be an 								alphabetic or a point.\n",lineIndex);

			/* use the note line condition above to skip the rest of the line */
				error_in_line = yes;
				continue;

		}

		/* if read spaces and not in sepecific word, skip */
		if( state==space && firstParam!=in && secParam!=in && (c==' ' || c=='\t') )
				continue;


		/* end of empty line */
		if( c=='\n' && first_char_line==yes )
		{

				lineIndex++;
				continue;

		}

		/* insert the first char in line or the first char after the label to the temp array */
		if( (first_char_line==yes||(labelFound==yes && opFound==no)) && state==space )
		{

				if( c=='.' )
					guideOp = yes;

				temp[tempInd++] = (char)c;   /* insert the char */
				state = inword;     				 /* now we're in a word */
				first_char_line = no;  	/* we done with the first char in line  */
				continue;

		}

		/* now we insret the rest of the first word */
		if( (firstWord==yes||(labelFound==yes&&opFound==no)) && state==inword && c!=' ' && c!='\t' && c!='[' )
		{

				/* if we get : its mean its a label */

				if( c==':'&&labelFound==no )
				{

						labelFound = yes;
						temp[tempInd] = '\0';
						strcpy(label, temp);
						tempInd = RESET_TO_DEFAULT;
						firstWord = no;
						state = space;

   					/* check if the label is a saved word of assembly */
						if( findSavedWord(label)==1 )
						{

								fprintf(stderr, "error found in line %d, the label name is a saved 													word of assembly\n",lineIndex);

							/* V use the note line condition above to skip the rest of the line V */
								error_in_line = yes;

								continue;

						}
				}
				else
						temp[tempInd++] = (char)c;

				continue;

		}

		if( (firstWord==yes||(labelFound==yes && opFound==no)) && state==inword && (c==' '||c=='\t'||c=='[' ))
		{

				temp[tempInd] = '\0';

				if( findOp(temp)==NO_OPERATION_FOUND )
				{

						fprintf(stderr, "error found in line %d the operation or the label is illegel\n",lineIndex);

						error_in_line = yes;

				}

				else
				{

							 /* operation found in opTable */
						opFound = yes;
						strcpy(operation, temp);
						tempInd = RESET_TO_DEFAULT;
						state = space;
						firstWord = no;

						if( (strcmp(operation,".entry")==EQUAL) ||
								(strcmp(operation,".extern")==EQUAL) )
								guideOp=no;
						if( c=='[' )
						{
								leftBrack = yes;
						}

				}

		continue;

		}


		/* start getting the parameters */
		if( guideOp==no && firstWord==no && opFound==yes && state==space && firstParam==no )
		{

       /* <operation> ,<param> , before the first param isnt allowed */
				if( c==',' )
				{
						fprintf(stderr, "error found in line %d, there is a \',\' before the first parameter\n",lineIndex);

						error_in_line = yes;

						continue;

				}

/* <operation>(without params)  after the operation only '\n' allowed  */
				if( c != '\n' && (findNumParam(operation)==0) )
				{

						fprintf(stderr, "error found in line %d, the operation shoulnt get parameter\n",lineIndex);

			 			error_in_line = yes;

						continue;

				}

				temp[tempInd++] = (char)c;
				firstParam = in;


				continue;

		}

		if( guideOp==no && firstWord==no && opFound==yes && state==space && (firstParam==in || (firstParam==yes && (secParam==no || secParam==in)) ))
		{


				if( findNumParam(operation)==1 && firstParam==yes && c!='\n')
				{

						fprintf(stderr, "error found in line %d, only one parameter allowed\n",lineIndex);

						first_char_line = yes;

						continue;

				}


				if( c==',' || c==' ' || c=='\t' )
				{



	/* <operation>(with one param) <param>, || <operation> <param>,,  */
						if( (findNumParam(operation)==1||comma==yes) && c==',' )
						{

								fprintf(stderr, "error found in line %d, too much commas\n",lineIndex);

								error_in_line = yes;

								continue;

						}

						if( c==',' )
						{
								comma = yes;
						}

						if( firstParam==in )
						{

								firstParam = yes;
								temp[tempInd] = '\0';
								strcpy( parameters[paramInd], temp );
								paramInd++;
								tempInd = RESET_TO_DEFAULT;

								continue;

						}

						if( secParam==in )
						{

								secParam = yes;
								temp[tempInd] = '\0';
								strcpy( parameters[paramInd], temp );
								paramInd++;
								tempInd = RESET_TO_DEFAULT;

								continue;

						}

						continue;

				}

				if( firstParam==yes && secParam==no && comma==no )
				{

						fprintf(stderr, "error found in line %d, missing comma\n",lineIndex);

						error_in_line = yes;

						continue;

				}

				if( firstParam==yes && secParam==no )
						secParam = in;


				temp[tempInd++] = (char)c;
				continue;

		}

		if( guideOp==no && firstParam==yes && secParam==yes && c!='\n' )
		{

				fprintf(stderr, "error found in line %d, after two parameters only \\n allowed\n",lineIndex);

				error_in_line = yes;

				continue;

		}


		 /*

						guide operation

		 */

		if( opFound==yes && guideOp==yes )
		{

				/*

 							.string

  			*/

				if( strcmp(operation, ".string")==EQUAL )
				{

						if( firstQmark==no && c!='"' )
						{

								fprintf(stderr, "error found in line %d, only \" mark allowed after \".string\" operation\n",lineIndex);

								error_in_line = yes;

								continue;

						}

						if( firstQmark==no && c=='"' )
						{

								firstQmark = yes;
								state = inword;

								continue;

						}

						if( firstQmark==yes && secQmark==no )
						{
								if( c=='"' )
								{
										secQmark = yes;
										temp[tempInd] = '\0';
										strcpy( parameters[paramInd], temp );
										paramInd++;
										tempInd++;
										state = space;
										firstParam = yes;
								}

								else
										temp[tempInd++] = (char)c;

								continue;

						}

						if( firstQmark==yes && secQmark==yes )
						{

								fprintf(stderr, "error found in line %d, only the '\\n' char allowed after the string end\n",lineIndex);

								error_in_line = yes;

								continue;

						}
				}

		/*

					 .data  and  .mat after the <.mat[r1][r2] >....

   */

		if( strcmp(operation, ".data")==EQUAL || numOf2brack==2 )
		{

				if( firstData==yes )
				{

						if( (isdigit(c)==0) && c!='+' && c!='-'  )
            {
                fprintf(stderr, "error found in line %d, only a number or a number with a sign allowed as a parameter\n",lineIndex);

								error_in_line = yes;

								continue;
            }

            if( c=='+' || c=='-' )
            {
								sign = yes;
            }

                state = inword;
                temp[tempInd++] = (char)c;
                firstData = no;
								comma = no;

                continue;


				}

				if( sign==yes )
        {

        		if( isdigit(c)==0 )
            {
            		fprintf(stderr, "error found in line %d, digit missing\n",lineIndex);

								error_in_line = yes;

								continue;

						}

            state = inword;
            temp[tempInd++] = (char)c;
            firstData = no;
            sign = no;

            continue;

        }

        if( state==inword )
        {

        		if( c==',' || c==' ' || c=='	' )
						{

								if( c==',' )
								{
							  		comma = yes;
										firstData = yes;
								}

								temp[tempInd] = '\0';
								strcpy( parameters[paramInd], temp );
								paramInd++;
								tempInd = RESET_TO_DEFAULT;
								sign = no;
								state = space;

								continue;

						}

						if( isdigit(c)==0 )
            {
           		 fprintf(stderr, "error found in line %d, only a digits allowed\n",lineIndex);

								error_in_line = yes;

								continue;

            }

						temp[tempInd++] = (char)c;

        }

				if( state==space )
				{

						if( c==',' )
						{

								if( comma==yes )
								{

										fprintf(stderr, "error found in line %d, too many commas\n",lineIndex);

										error_in_line = yes;

										continue;

								}

								comma = yes;
								firstData = yes;

								continue;

						}

								if( comma==no )
								{

										fprintf(stderr, "error found in line %d, missing comma\n",lineIndex);

										error_in_line = yes;


										continue;

								}



						}

				}

		}



    /*

				 	.mat

				treating the .mat operation case and after the <.mat[r2][r1] >
				the .data case above treats the reading of the parameter.

		*/

		if( strcmp(operation, ".mat")==EQUAL )
		{

				if( leftBrack==no && (mat1param==no || (mat1param==yes && mat2param==no)) )
				{

						if( c!='[' )
						{
								fprintf(stderr, "error found in line %d, missing [\n",lineIndex);

								error_in_line = yes;

								continue;
						}

						leftBrack = yes;
						continue;

				}

				if( leftBrack==yes )
				{

						if( state==space )
						{

								if( (mat1param==yes && numOf2brack==0) || (mat2param==yes && numOf2brack==1) )
								{
										if( c!=']' )
										{

												fprintf(stderr, "error found in line %d, missing ] sign\n",lineIndex);

												error_in_line = yes;

												continue;


										}

										if( mat2param==yes )
												matLen = (atoi(parameters[THE_FIRST_PARAMETER])*atoi(parameters[THE_SEC_PARAMETER]));

										leftBrack = no;
										numOf2brack++;
										continue;
								}

								if( (isdigit(c)==0) && c!='+'  )
            		{
               		 fprintf(stderr, "error found in line %d, only a number or a number with a + sign allowed as a parameter\n",lineIndex);

										error_in_line = yes;

										continue;
            		}

								if( c=='+' )
            		{
										sign = yes;
            		}

                state = inword;
                temp[tempInd++] = (char)c;

                continue;
						}

						if( state==inword )
						{

								if( sign==yes )
        				{

        						if( isdigit(c)==0 )
            				{
            						fprintf(stderr, "error found in line %d, digit missing\n",lineIndex);

												error_in_line = yes;

												continue;

										}

            				temp[tempInd++] = (char)c;
            				sign = no;

            				continue;

        				}

								if( isdigit(c) )
								{
										temp[tempInd++] = (char)c;
										continue;
								}

								if( c==' ' || c=='	' || c==']' )
								{

										if( c==']' )
										{
												leftBrack = no;
												numOf2brack++;
										}

										state = space;
										temp[tempInd] = '\0';
										strcpy( parameters[paramInd], temp );
										paramInd++;
										tempInd = RESET_TO_DEFAULT;

										if( mat1param==no )
												mat1param = yes;
										else
										{
												mat2param = yes;
												matLen = (atoi(parameters[THE_FIRST_PARAMETER])*atoi(parameters[THE_SEC_PARAMETER]));
										}

										continue;

								}

						}

				}

		}

	}	
		

}/* 	end of first run!		 */





/* gets string and check if exist in the saved words table */
int findSavedWord(char *word)
{
		int i;

		for( i=0; i<NUM_OF_SAVED_WORDS; i++ )
		{
				if( strcmp(savedWords[i], word)==EQUAL )
						return 1;
		}

		return 0;

}




/*	return 1 if the sentence is a guide sentence 
				or 0 if the sentence is operation sentence.
*/
int isGuide(char *op)
{

		if( findOp(op) < END_LIST_OF_OPERATION )
				return 0;

		return 1;

}





/* gets string and check if exist in operation table, return the opcode
   or -1 if cant find */
int findOp(char *op)
{

		int i;

		for( i=0; i<NUM_OF_OP; i++ )
		{
				if( strcmp(opArr[i].opName, op)==EQUAL )
						return opArr[i].opcode;
		}

		return -1;

}





/* gets string with name of operation and return the number of parameters
   the operation should get */
int findNumParam(char *op)
{

		int i;

		for( i=0; i<NUM_OF_OP; i++ )
		{
				if( strcmp(opArr[i].opName, op)==EQUAL )
						return opArr[i].numOfParam;
		}

		return -1;

}





/*	check if the parameters are valid to the type of operation	*/
int checkValidationParam( char *opName, char *param1, char *param2 )
{

		int j, r, opNum, type1param, type2param;
		opNum = findOp( opName );
		type1param = typeOfParam( param1, REGULAR );
		type2param = typeOfParam( param2, REGULAR );

		if( opNum == -1 )
				return -1;

		for( j=0; opArr[opNum].param1Type[j]!=NO_TYPE_FOUND ; j++  )
		{
				if( opArr[opNum].param1Type[j]==type1param )
				{
						if( opArr[opNum].numOfParam == 2 )
						{
								for( r=0; opArr[opNum].param2Type[r]!=NO_TYPE_FOUND ; r++ )
								{
										if( opArr[opNum].param2Type[r] == type2param )
												return 1;
								}
								return 0;
						}
						else
								return 1;
				}
		}

		return 0;

}



/* Add new parameter to symbol table. */
void addInfoToSymbolTable(char *param, int ic, int extern_param, int operation, int typeOfOp, stp *p)
{
	stp currRow, nextRow;
	stp newRow = (stp)malloc(sizeof(st));
		
	if(newRow == NULL)
	{
		fprintf(stderr, "Memory allocation failed.");
		exit(0);
	}
		
	strcpy(newRow->param, param);
	
	if(extern_param == 0)
		newRow->address = ic;
	else
		newRow->address = 0;
		
	newRow->extern_param = extern_param;
	newRow->operation = operation;
	newRow->typeOfOp = typeOfOp;
	newRow->next = NULL;
		  
	 /* First addition to symbol table. */ 
	if(*p == NULL)
		*p = newRow;
	
	/* The symbol table is not empty, make the addition to the end of the table. */
	else
	{
		nextRow = *p;
		
		while(nextRow)
		{
			/* This parameter is exist in symbol table. */
			if(strcmp(nextRow->param, param) == 0)
			{
				fprintf(stderr,"error: The parameter %s is exist in symbol table\n", param);
				free(nextRow);
				return;
			}
			
			currRow = nextRow;
			nextRow = nextRow->next;
		}
		
		/* Add parameter to the end of symbol table. */
		currRow->next = newRow;
		newRow->next = NULL;
	}
} 



/* Add data to data table. */
int addDataToDataTable(int arr[], int count, int dc, dtp *p)
{
	int i, num;
	
	for(i = 0; i < count; i++)
	{
		num = arr[i];
		dc = addInfoToDataTable(num, dc, p);
	}
	
	return dc;
}




/* Add the matrix data to data table. */
int addMatToDataTable(int arr[], int dc, dtp *p)
{	
	int i = 0;  	
	int row = arr[i++];
	int column = arr[i++];
	int num;
	
	/* the array data start from the second index. */
	for(i = 2; i < (row*column + 2); i++)
	{
		num = arr[i];
		dc = addInfoToDataTable(num, dc , p);
	}
	
	return dc;
}





/* Take an ASCII value, convert to binary and add to data table. */
int addStringToDataTable(char *arr, int dc, dtp *p)
{
	int i = 0;
	
	while(arr[i])
			dc = addInfoToDataTable(arr[i++], dc, p);

	dc = addInfoToDataTable(0, dc, p);
	
	
	return dc;
}




int addInfoToDataTable(int num, int dc, dtp *p)
{

	dtp currRow, nextRow;
	dtp newRow = (dtp)malloc(sizeof(dt));  
	
	if(newRow == NULL)
	{
		fprintf(stderr, "Memory allocation failed.");
		exit(0);
	}
	
	newRow->address = dc++;
	
	newRow->data = num;

	
	
	/* Data table is empty - first addition. */
	if(*p == NULL)
	{
		*p = newRow;
	}
	
	/* Data table is not empty - add the new number to the end of the table. */
	else
	{
		nextRow = *p;
		
		while(nextRow)
		{
			currRow = nextRow;
			nextRow = nextRow->next;
		}
		currRow->next = newRow;
		newRow->next = NULL;
	}
	
	return dc;
}	





/* Add new action statement to the end of action statemen list. */
void addActionToActionStatementList(char *action, char *param1, char *param2, int address, int line, actSP *p)
{
	actSP currNode, nextNode;
	actSP newNode = (actSP)malloc(sizeof(actS));
	
	if(newNode == NULL)
	{
		fprintf(stderr, "Memory allocation failed.");
		exit(0);
	}

	strcpy(newNode->action, action);
	strcpy(newNode->param1, param1);
	strcpy(newNode->param2, param2);
	newNode->address = address;
	newNode->line = line;
 	
	/* action statement list is empty - first addition. */
	if(*p == NULL)
	{
		*p = newNode;
	}
	
	/* action statement is not empty - add new action statement to the end of list. */
	else
	{
		nextNode = *p;
		
		while(nextNode)
		{
			currNode = nextNode;
			nextNode = nextNode->next;
		}
		
		currNode->next = newNode;
		newNode->next = NULL;
	}
}





/* gets the number of type of the parameters and the number of 
	 parameters and return the number ofword memory the it takes. */

int findIc( char *opName, char *param1, char *param2, int paramInd, int strLength )
{

        int size, typeParam1, typeParam2, numOfP;
        size = 0;
				numOfP = findNumParam(opName);


				if( strcmp(opName,".entry")==EQUAL	||
						strcmp(opName,".extern")==EQUAL 	)
						return 0;

				if( strcmp(opName,".string")==EQUAL )
						return strLength;

				if( strcmp(opName,".data")==EQUAL )
						return paramInd;

				if(	strcmp(opName,".mat")==EQUAL 	)
						return paramInd-2;

				if( numOfP==0 )
						return 1;


				typeParam1 = typeOfParam(param1, REGULAR);


        if( typeParam1==0 )
                size = 1;

        if( typeParam1==1 )
                size = 1;

        if( typeParam1==2 )
                size = 2;

        if( typeParam1==3 )
                size = 1;

        if( numOfP==2 )
        {

								typeParam2 = typeOfParam(param2, REGULAR);

                if( typeParam2==0 )
                        size += 1;

                if( typeParam2==1 )
                        size += 1;

                if( typeParam2==2 )
                        size += 2;

                if( typeParam2==3 )
                {
                        if( typeParam1==3 )
                                size = 1;
                        else
                                size += 1;
                }

        }


        return size+1;

}



/* After first run and ic and dc calculation, we update the address for Guideline statement. */
void updateDataTable(dtp *p, int ic)
{
	 dtp currD;
	 currD = *p;	
	
	 while( currD )
	 {
	 	
	 		currD->address += ic;
	 	
	 		currD = currD->next;
	 }
}


/* After first run and ic and dc calculation, we update the address for Guideline statement. */
void updateSymbolTable(stp *p, int ic)
{
	 stp currS;
	 currS = *p;

	 while( currS )
	 {
	 	if(currS->operation == 0 && currS->extern_param == 0)
	 	{
	 		currS->address += ic;
	 	}
	 	currS = currS->next;
	 }
}
