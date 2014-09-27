#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gd.h>
#include <gdfontl.h>
#include <gdfontt.h>

#include "table.h"
#include "vector.h"
#include "cgiparse.h"
#include "utils.h"

#define DATAFILE "cgidata.txt"
#define DEFAULTWIDTH 300
#define DEFAULTHEIGHT 100
#define DEFAULTLABELHEIGHT 22
#define DEFAULTHORIZPADDING 20
#define DEFAULTVARIANCE 0.3

#define LOWESTSCORE 1.0
#define HIGHESTSCORE 10.0

void readdatafile(Vector stats, char *user);
double safeatof(char *s);

int main(int argc, char *argv[])
{
  Table formt;
  Vector stats;
  gdImagePtr im;
  int black,white,blue;
  char *user, *formdata;
  int size,pos,x;
  float searchscore,searchincrement;
  double thisscore,score,lineheight;
  char label[5];
  int labellen;
  float variance;
  int height,width,labelheight,horizpad;
  int scoremark;
  char *votelabel;
  int lightblue;

  /**********************************
    HTTP header
    ********************************/
  /* use image/png if possible */
  printf("content-type: image/gif\n\n");
  
  /**********************************
    get info about request
    ********************************/
  formt=cgiparse();
  
  formdata=Table_gets(formt,"WIDTH");
  if (formdata!=NULL)
    width=atoi(formdata);
  else
    width=DEFAULTWIDTH;
  
  formdata=Table_gets(formt,"HEIGHT");
  if (formdata!=NULL)
    height=atoi(formdata);
  else
    height=DEFAULTHEIGHT;
 
  formdata=Table_gets(formt,"LABELHEIGHT");
  if (formdata!=NULL)
    labelheight=atoi(formdata);
  else
    labelheight=DEFAULTLABELHEIGHT;

  formdata=Table_gets(formt,"HORIZPADDING");
  if (formdata!=NULL)
    horizpad=atoi(formdata);
  else
    horizpad=DEFAULTHORIZPADDING;
 
  formdata=Table_gets(formt,"VARIANCE");
  if (formdata!=NULL)
    variance=atof(formdata);
  else
    variance=DEFAULTVARIANCE;
 
  stats=Vector_create(0);
  user=Table_gets(formt,"USER");

  if (user!=NULL)
    {
      readdatafile(stats,user);
    }

  size=Vector_getSize(stats);
  
  /**********************************
    create the image object and set it up
    ********************************/
  
  im=gdImageCreate(width+2*horizpad,height+labelheight);

  black=gdImageColorAllocate(im,0,0,0);
  white=gdImageColorAllocate(im,255,255,255);
  blue=gdImageColorAllocate(im,0,0,200);
  lightblue=gdImageColorAllocate(im,80,80,255);

  /**********************************
    perform the search. awfully inefficient.
    ********************************/
  searchincrement=(HIGHESTSCORE-LOWESTSCORE)/width;
  
  searchscore=LOWESTSCORE;
  x=0;

  while (searchscore<=HIGHESTSCORE)
    {
      score=0;

      for (pos=0;pos<size;pos++)
	{
	  thisscore= *((double*) Vector_elementAt(stats,pos));

	  score+=exp(-fabs(pow(thisscore-searchscore,2)/variance));
	}

      if (size>0)
	lineheight=score*height/size;
      else
	lineheight=0;

      gdImageLine(im,x+horizpad,height,x+horizpad,height-lineheight,white);

      searchscore+=searchincrement;
      x++;
    }

  /**********************************
    Label the x axis
    ********************************/

  for (scoremark=LOWESTSCORE;scoremark<=HIGHESTSCORE;scoremark++)
    {
      x=(scoremark-LOWESTSCORE)*(width/(HIGHESTSCORE-LOWESTSCORE));

      snprintf(label,5,"%i",scoremark);
      labellen=strlen(label);

      gdImageLine(im,x+horizpad,0,x+horizpad,height,blue);
      gdImageString(im,gdFontLarge, x+horizpad-(gdFontLarge->w/2)*labellen,height+2,label,white);
    }

  /**********************************
    Outline the actual data
    ********************************/
  gdImageRectangle(im,horizpad,1,width+horizpad,height,white);
  
  votelabel=sprintfalloc("%i samples",Vector_getSize(stats));
  gdImageString(im,gdFontLarge,width+horizpad-(gdFontLarge->w)*strlen(votelabel)-2,2,votelabel,lightblue);
  free(votelabel);

  /**********************************
    Output the data 
    ********************************/

  /* use gdImagePng if possible */
  gdImageGif(im,stdout);

  gdImageDestroy(im);

  return 0;
}

/* somehow get scores for user and put them in stats */
void readdatafile(Vector stats, char *user)
{
  FILE *dfile;
  char *line,*userstr,*moviestr,*scorestr,*found;
  double score, *scorep;

  dfile=fopen(DATAFILE,"r");
  if (dfile==NULL)
    return;

  while ((line=freadln(dfile))!=NULL)
    {
      userstr=line;
      
      found=strchr(userstr,':');
      if (found==NULL)
	continue;

      moviestr=&found[1];
      found[0]='\0';
      
      found=strchr(moviestr,':');
      if (found==NULL)
	continue;

      scorestr=&found[1];
      found[0]='\0';
      
      if (!strcmp(userstr,user))
	{
	  score=safeatof(scorestr);
	  if (score>=0)
	    {
	      scorep=malloc(sizeof(double));
	      *scorep=score;
	      Vector_add(stats,scorep);
	    }
	}
      
      free(line);
    }
  
  fclose(dfile);
}

double safeatof(char *s)
{
  char *returnptr;
  double candidate;

  if (s==NULL)
    return -1;

  candidate=strtod(s,&returnptr);
  if (returnptr==s)
    return -1;

  return (candidate);
}


