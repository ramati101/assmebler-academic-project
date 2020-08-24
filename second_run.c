#include "prog.h"




/*
			check errors of second run
*/
int secondRun( stp sth, actSP actsh)
{

	int error_in_file, count;	

	stp currS;
	actSP currA;

	currA = actsh;

	error_in_file = no;


	/* runs on every node in action statement list. */
													
	while( currA )
	{
																	
		count = 0;
		currS =  sth; 
		if( strcmp(currA->action,".entry")==EQUAL )
		{
			
			while(currS)
			{

				/* check if the label exist in symbol table.
			if yes-print to entry file the label and address.*/				  
				if( strcmp(currA->param1, currS->param)==EQUAL )
					count = 1;
				
				currS = currS->next;
				
			}
			
			/* the label is not exist in symbol tabel. */
			if(!count)
			{
			
				error_in_file = yes;
			}
			
			
		}
		
		/* an action statement that can continue and extern parameter. */
		else
		{		
				if( checkLine( currS, currA ) )
						error_in_file = yes;
					
		}
		
		currA = currA->next;		
	}
	
	return error_in_file;			
}


int checkLine( stp sth, actSP currA )
{														

		char *matLabel;		
		stp currS;
		int numParam, typeOfparam, sizeOfMatLabel, equal, i;
		numParam = findNumParam( currA->action );
		


		for( i=0; i<numParam; i++ )
		{
				currS = sth;				
			
				if( i==0 )
						typeOfparam = typeOfParam( currA->param1, REGULAR );
				else
						typeOfparam = typeOfParam( currA->param2, REGULAR );

				if( typeOfparam == DIRECT || typeOfparam == MAT )
				{
						equal = no;				

						if( typeOfparam == MAT )
						{		
								if( i==0 )
								{			

										sizeOfMatLabel = typeOfParam( currA->param1,RETURN_MAT_LABEL_LENGTH );
										if(!(matLabel = malloc( sizeOfMatLabel+1 )))
										{
												fprintf(stderr, "Memory allocation failed.");
												exit(0);
										}
										memset(matLabel,'\0',sizeOfMatLabel+1 );
										strncpy(matLabel,currA->param1,sizeOfMatLabel);
											

								}
								else
								{
										sizeOfMatLabel = typeOfParam( currA->param2, RETURN_MAT_LABEL_LENGTH );
										if(!(matLabel = malloc( sizeOfMatLabel+1 )))
										{
												fprintf(stderr, "Memory allocation failed.");
												exit(0);
										}
										memset(matLabel,'\0',sizeOfMatLabel+1 );
										strncpy(matLabel,currA->param2,sizeOfMatLabel);
								}										
					
								while( currS  )
								{
										if( strcmp( currS->param, matLabel )==EQUAL )
										{	
												equal = yes;
												break;										
										}
										currS = currS->next;		
								}
								if( equal!=yes )
								{
										fprintf(stderr,"error found in line %d, the label of mat parameter didnt declared\n",currA->line);
										return 1;
								}
								if( currS->typeOfOp != 18 && currS->extern_param != 1 )
								{
										fprintf(stderr,"error found in line %d, the label of mat parameter didnt declared as a mat\n",currA->line);
										return 1;
								}
							
						}

						else  /* type of parameter is direct */
						{
								while( currS )
								{
										if( i==0 )
												if( strcmp( currS->param, currA->param1 )==EQUAL )
														equal = yes;
										if( i==1 )
												if( strcmp( currS->param, currA->param2 )==EQUAL )
														equal = yes;

										currS = currS->next;		
								}
								if( equal==no )
								{
										fprintf(stderr,"error found in line %d, the label of direct parameter didnt declared\n",currA->line);
										return 1;
								}		
						}
						
				}

		}

		return 0;
}






/* return the num of param type or -1 if error
		# - 0
		x - 1
		mat[r2][r4] - 2
		r6 - 3

		and if its a mat, we can send 1 to wantLabelLength parameter and
		the function return us the length of the label before the <[r1][r2]>
 */

int typeOfParam(char *param, int wantLabelLength )
{

		int k, ladderSign, regis, alpha, mat, startMat ;

		ladderSign = 0; /* # */
		regis = 0;
		alpha = 0;
		mat = 0;
		startMat = 0;


		for( k=0; param[k]!='\0'; k++ )
		{

				/* the first char of parameter */

				if( k==0 )
				{

						if( param[k]=='#' )
								ladderSign = 1;

						else if( param[k]=='r' )
								regis = 1;

									else if( isalpha(param[k]) )
											alpha = 1;

												else
														return -1;

						continue;

				}

				if( ladderSign==1 )
				{
						if( isdigit(param[k]) || (k==1 && (param[k]=='-' || param[k]=='+')) )
						{
								if(	wantLabelLength && isdigit(param[k])	)	
										return k;
								continue;
						}

						else
								return -1;
				}

				if( regis==1 && k==1 && param[k]>='0' && param[k]<='7' && param[k+1]=='\0')
						return 3;

				else if( regis==1 || alpha==1 )
				{
						regis = 0;
						alpha = 1;

						if( param[k]=='[' )
						{
								mat = 1;
								startMat = k;
								alpha =0;
								if( wantLabelLength )
										return k;
								continue;
						}

						if( isalnum(param[k])==0 )
								return -1;

						continue;
				}

				if( mat==1 )
				{

						if( (k == startMat+1 && param[k]=='r') ||
								(k == startMat+2 && param[k]>='0' && param[k]<='7') ||
								(k == startMat+3 && param[k]==']') ||
								(k == startMat+4 && param[k]=='[') ||
								(k == startMat+5 && param[k]=='r') ||
								(k == startMat+6 && param[k]>='0' && param[k]<='7') ||
								(k == startMat+7 && param[k]==']' && param[k+1]=='\0') )
								continue;

						else
								return -1;

				}

		}


		if( ladderSign==1 )
		{
				if( isdigit(param[1]) || isdigit(param[2]) )
						return 0;
				else
						return -1;
		}

		if( mat==1 )
				return 2;

		if( alpha==1 )
				return 1;


		return -1;

}


