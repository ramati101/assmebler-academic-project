#include "prog.h"




int main(int argc, char *argv[])
{
	
		int i, ic, dc;
		char *file_name;	


		stp sth; /* pointer to head of symbol table */
 		dtp dth; /* pointer to head of data table */
		actSP actsh; /* pointer to head of action statement list. */	

		FILE *fp; /* file pointer */


		if(argc == 1)
		{
				fprintf(stderr, "No arguments only program name.\n");
				exit(0);
		}


		/* running on every file in argv array */
		for(i = FILE_START_INDEX; i < argc; i++)
		{

				sth = NULL;
  			dth = NULL;
  			actsh = NULL;


   			file_name = addExtension( argv[i], ".as" );

				if(!(fp = fopen(file_name, "r")))
				{
						fprintf(stderr, "Can't open file.\n");
						exit(0);
				}
				


				/*		start first run and if no error
							start sec run			*/
				if(firstRun(fp,&sth,&dth,&actsh, &ic, &dc) == no)
				{		
						fclose(fp);						
					
					 if(secondRun( sth, actsh) == no)
						{
								
								printList(argv[i], sth, dth, actsh, ic, dc);
						}
					
				}
			

		}

		return 0;
}

