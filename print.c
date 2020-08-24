#include "prog.h"



void printList( char* input_name, stp sth, dtp dth, actSP actsh, int lastIc, int lastDc )
{
	
	int i, ic, numOfParams, tparam, addres, regis1, regis2, codeOp;	

	char *Param1, *Param;

	FILE *ob, *ent, *ext;	
	
	actSP currA;
	stp		currS;
	dtp   currD;
	
	

		/* create 3 diffrent files( .ob , .ent , .ext ) */

	if(!(ob = fopen(addExtension(input_name,".ob"), "w")))
	{
		fprintf(stderr, "Can't open file.\n");
		exit(0);
	}
	if(!(ent = fopen(addExtension(input_name,".ent"), "w")))
	{
		fprintf(stderr, "Can't open file.\n");
		exit(0);
	}
	if(!(ext = fopen(addExtension(input_name,".ext"), "w")))
	{
		fprintf(stderr, "Can't open file.\n");
		exit(0);
	}


	fprintf(ob,"Base 4 address\tBase 4 code\n");


	fprintf(ob,"\t%s\t",convertIntToNew4Base(lastIc,3));
		
	fprintf(ob,"%s\n",convertIntToNew4Base(lastDc,3));




	currA = actsh;

	while( currA )
	{   
			currS = sth;
			

			if( strcmp(currA->action,".entry")==EQUAL )
			{
				
					while(currS)
					{
							if( strcmp(currA->param1, currS->param)==EQUAL )
							{
									fprintf(ent,"%s\t",currA->param1);
									fprintf(ent,"%s\n",convertIntToNew4Base(currS->address,4));			
							}				
							
							currS = currS->next;
				
					}		
			}
		

		/* an action statement that can continue and extern parameter. */
			else
			{		
												
					ic = currA->address;

					/*	print the first ic and the code to the file.ob	*/
					fprintf(ob,"\t%s\t",convertIntToNew4Base(ic++,4));
					codeOp = wordCodeOp(currA->action, currA->param1, currA->param2);	
					fprintf(ob,"%s\n",convertIntToNew4Base(codeOp,5));				


					numOfParams = findNumParam( currA->action );
					
					for( i=0; i<numOfParams ; i++ )	
					{	  
							if(i==0)
									Param = currA->param1 ;
							else
									Param = currA->param2 ;
					
							tparam = typeOfParam( Param , 0 );
							

							switch( tparam )
							{
										case 0	:
															fprintf(ob,"\t%s\t",convertIntToNew4Base(ic++,4));
								
															fprintf(ob,"%s\n",convertIntToNew4Base(convertFromIntToMemWord(atoi(strtok(Param,"#")),0),5));

															break;
						
										case 1	:
															
									  case 2	:
																													
															Param1 = strtok(Param, "[r]");
															addres = returnIcOfSymbol(sth, Param1 );															
																if( addres == 0 )
															{
																	/*	print to .ob	*/
																	fprintf(ob,"\t%s\t",convertIntToNew4Base(ic,4));
																	fprintf(ob,"%s\n",convertIntToNew4Base(convertFromIntToMemWord(0,1),5));
																	/*	print to .ext	*/
																	fprintf(ext,"%s\t",Param1);
																	fprintf(ext,"%s\n",convertIntToNew4Base(ic++,4));				

															}
															else
															{  		
																	/*	print to .ob	*/
																	fprintf(ob,"\t%s\t",convertIntToNew4Base(ic++,4));
																	fprintf(ob,"%s\n",convertIntToNew4Base(convertFromIntToMemWord(addres,2),5));
															}

															if( tparam == 2 )
															{   
																	regis1 = atoi(strtok(NULL, "[r]"));
																	regis2 = atoi(strtok(NULL, "[r]"));
					
																	fprintf(ob,"\t%s\t",convertIntToNew4Base(ic++,4));

																	fprintf(ob,"%s",convertIntToNew4Base(regis1,2));
																	fprintf(ob,"%s",convertIntToNew4Base(regis2,2));
																	fprintf(ob,"%s\n",convertIntToNew4Base(0,1));
															}

															break;


										case 3	:  
															if( i==0 && numOfParams==2 && typeOfParam(currA->param2,0)==3 )
															{ 
																	regis1 = atoi(strtok(Param, "r"));
																	regis2 = atoi(strtok(currA->param2, "r"));

																	fprintf(ob,"\t%s\t",convertIntToNew4Base(ic++,4));

																	fprintf(ob,"%s",convertIntToNew4Base(regis1,2));
																	fprintf(ob,"%s",convertIntToNew4Base(regis2,2));
																	fprintf(ob,"%s\n",convertIntToNew4Base(0,1));
																	
															}
															if( !((typeOfParam(currA->param1,0)==3) && (typeOfParam(currA->param2,0)==3)) ) 												
															{

																	Param = strtok(Param, "r");
																	fprintf(ob,"\t%s\t",convertIntToNew4Base(ic++,4));

																	if( i==0 )
																	{
																			fprintf(ob,"%s",convertIntToNew4Base(atoi(Param),2));
																			fprintf(ob,"%s",convertIntToNew4Base(0,2));
																	}
																	else
																	{
																			fprintf(ob,"%s",convertIntToNew4Base(0,2));
																			fprintf(ob,"%s",convertIntToNew4Base(atoi(Param),2));
																	}																	
																			fprintf(ob,"%s\n",convertIntToNew4Base(0,1));
															}	

															break;
	
							}/*end of switch*/

					}/*end of for*/	
					
					
			}/*end of else*/
		
				currA = currA->next;		
	}/*end of while(print all code)*/
	
	currD = dth;
	/* print data table */
	while( currD )
	{
		
			fprintf(ob,"\t%s\t",convertIntToNew4Base(ic++,4));
			fprintf(ob,"%s\n",convertIntToNew4Base(currD->data,5));

			currD = currD->next;	
	}

	fclose(ob);
	fclose(ent);
	fclose(ext);
				
}





int convertFromIntToMemWord( int num, int typeARE )
{
	
		int i;
		int mask = 1; 
		int data = 0;
		int binNum = 4;
		int base2 = 2;
	
	
		for(i = 0; i < 8; i++)
		{
				if(	num	&	mask	)
						data += binNum;

				binNum = binNum * base2;
				mask <<= 1;
		}
  
		switch( typeARE )
	    	  {	
						case 1: 
									data += 1;
									break;
						case 2: 
									data += 2;
									break;
	      
					}
		return data;

}



int returnIcOfSymbol( stp sth, char *symbol )
{

		stp currS;
		currS = sth;

		while(currS)
		{
				if( strcmp( currS->param, symbol ) == EQUAL )
				{		
						if( currS->extern_param == 1 )
								return 0;
						else						
								return currS->address;
				}
				currS = currS->next;
		}

		return -1;

}





char *convertIntToNew4Base(int num, int len)	
{	
		int i = 0;
		char *add = (char *)calloc(len+1, sizeof(char));
		char *res = (char *)calloc(len+1, sizeof(char));
	
		if(add == NULL || res == NULL)
		{
		
				fprintf(stderr, "Memory allocation failed.");
				exit(0);
		}
	
		while(i < len+1)
		{
				switch(num % 4)
							{
								case 0 : add[i++] = 'a';
													break;
								case 1 : add[i++] = 'b';
													break;
								case 2 : add[i++] = 'c';
													break;
								case 3 : add[i++] = 'd';
													break;
							}
		
				num /= 4;
		}
	
		while(i < len)
				add[i++] = 'a';
	
		add[i] = '\0';
	
	
		for(i = 0; i < len; i++)
		{
				res[len-1-i] = add[i];
		}
 		res[len] = '\0';
 	
		free(add); 
	
		return res;

}



/*		turn	<file_name>    and   <.as>
				to		<file_name.as>
*/
char* addExtension( char *orgName, char *exten )
{

				char *file_name;

				file_name = calloc((strlen(orgName))+MAX_EXTENSION,sizeof(char));
   			strcpy(file_name, orgName);
   			strcat(file_name, exten);
				
				return file_name;
}



/*	return int that if we convert to new 4 base we get the word of
		the operation sentence.
*/
int wordCodeOp( char *opName, char *param1, char *param2	)
{

		int WCO;
		int numOp, type1, type2;

		numOp = findOp( opName );
		WCO = 0;



		if( numOp >= 8 )
				WCO = 512;

		if( (numOp > 3 && numOp < 8) || numOp > 11 )
				WCO += 256;

		if( numOp == 2 || numOp == 3 || numOp == 6 || numOp == 7 || numOp == 10 || numOp == 11 || numOp > 13)
				WCO += 128;

		if( numOp == 1 || numOp % 2 == 1 )
				WCO += 64;



		if( findNumParam(opName) == 0 )
				return WCO;

		if( findNumParam(opName) == 1 )
		{
				type1 = typeOfParam(param1, REGULAR);

				if( type1==1 )
						WCO += 4;
				if( type1==2 )
						WCO += 8;
				if( type1==3 )
						WCO += 12;
		}

		if( findNumParam(opName) == 2 )
		{
				type1 = typeOfParam(param1, REGULAR);
				type2 = typeOfParam(param2, REGULAR);

				switch( type1 )
							{
							case 1 : WCO += 16;
											 break;

							case 2 : WCO += 32;
											 break;

							case 3 : WCO += 48;
											 break;
							}

				if( type2==1 )
						WCO += 4;
				if( type2==2 )
						WCO += 8;
				if( type2==3 )
						WCO += 12;				

		}

		return WCO;

}


