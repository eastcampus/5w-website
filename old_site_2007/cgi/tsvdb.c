#include <stdio.h>
#include <stdlib.h>

#include "tsvdb.h"

#define ROW_CHUNKSIZE 64
#define COLUMN_CHUNKSIZE 16

#define BUFFSIZE 2048

#define SEPARATOR '\t'

/* returns NULL on failure */

TSVdb TSVdb_create(char* filename)
{
  TSVdb tsvdb;
  
  if ((tsvdb=(TSVdb) malloc(sizeof(TSVdbRecord)))==NULL)
    return NULL;
  
  tsvdb->rows=Vector_create(ROW_CHUNKSIZE);
  tsvdb->maxcolumns=0;

  TSVdb_addRowsFromFile(tsvdb,filename);

  return tsvdb;
}

/* returns a particular cell or a pointer to "" if it doesn't exist */

char* TSVdb_getCell(TSVdb tsvdb, int y, int x)
{
  Vector row;
  char *cell;

  row=(Vector) Vector_elementAt(tsvdb->rows,y);
  if (row==NULL)
    return "";

  cell=Vector_elementAt(row,x);
  if (cell==NULL)
    return "";

  return cell;
}

int TSVdb_columns(TSVdb tsvdb)
{
  return (tsvdb->maxcolumns);
}

int TSVdb_rows(TSVdb tsvdb)
{
  return (Vector_getSize(tsvdb->rows));
}

int TSVdb_deleteRow(TSVdb tsvdb, int row)
{
 return (Vector_delete(tsvdb->rows,row));
}

/* returns <0 on failure */

int TSVdb_addRowsFromFile(TSVdb tsvdb, char *filename)
{
  FILE *f;
  char buffer[BUFFSIZE];
  Vector row;
  int startmark,readmark,writemark,len;
  char *cell;
  int columns;

  if ((f=fopen(filename,"r"))==NULL)
    return -1;
  
  while (fgets(buffer,BUFFSIZE,f)!=NULL)
    {
      row=Vector_create(COLUMN_CHUNKSIZE);
     
      Vector_add(tsvdb->rows,(char*) row);

      startmark=0;
      readmark=0;
      writemark=0;

      columns=0;

      len=strlen(buffer);
      
      for (readmark=0;readmark<len;readmark++)
	{
	  switch (buffer[readmark])
	    {
	    case '\0':
	    case '\n':
	    case SEPARATOR:
	      {
		buffer[writemark]='\0';
		
		cell=(char*) malloc(writemark-startmark+1);
		memcpy(cell,&buffer[startmark],writemark-startmark+1);
		
		Vector_add(row,cell);
		
		startmark=readmark+1;
		writemark=startmark;
		
		columns++;
		
		if (columns>tsvdb->maxcolumns)
		  tsvdb->maxcolumns=columns;
		
		break;
	      }

	    /* is it an escape sequence? */
	    case '\\':
	      {
		if ((readmark+1)>=len)
		  {
		    buffer[writemark]=buffer[readmark+1];
		    writemark++;
		  }
		break;
	      }
	    
	    default:
	      {
		buffer[writemark]=buffer[readmark];
		writemark++;
	      }
	      
	    }
	}
    }
  fclose(f);

  return 0;
}

int TSVdb_swapRows(TSVdb tsvdb, int row1, int row2)
{
  Vector t;

  t=(Vector) Vector_elementAt(tsvdb->rows,row1);
  Vector_setElementAt(tsvdb->rows, row1, Vector_elementAt(tsvdb->rows,row2));
  Vector_setElementAt(tsvdb->rows, row2, (char*) t);

  return 0;
}
