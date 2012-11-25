/*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * File path:     generator.c
 * Description:   ANSI C program that generates C-includes from
 *                files.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 *
 * $Id: generator.c,v 1.4 2002/08/31 12:49:34 simon Exp $
 *
 ********************************************************************/
#include <stdio.h>   /* fgets etc */

/* Convenience - Ok, slightly ugly ... */
#define FPUTS(str, file) do                                                           \
                           {                                                          \
                             if (fputs((str), (file)) < 0)                            \
                               {                                                      \
                                 fprintf(stderr, "Error in fputs\n");                 \
                                 return -1;                                           \
                               }                                                      \
			   } while(0)


/* Print a helpful message */
static void print_help(void)
{
  fprintf(stderr,
	  "Usage: generator [option(s)] [file(s)]\n"
	  " Generate a REX addin header file from text input.\n"
	  " The options are:\n"
	  "   -n memo-name, --name memo-name\n"
	  "               Set the name of the memo to memo-name\n"
	  "   -o outfile, --outfile outfile\n"
	  "               Use outfile as output file (defaults to stdout)\n"
	  "   -h, --help\n"
	  "               This text\n\n"
	 );
}

/* Parse the options */
static int parse_options(int argc, char *argv[], char **pp_memo_name,
			 char **pp_outfile_name)
{
  int last_opt=1;
  int i=0;

  /* Assign an option argument. */
#define assign_optarg(opt, pp) do                                                      \
                                 {                                                     \
                                   if (i+1 >= argc || argv[i+1][0] == '-')             \
                                     {                                                 \
			 	       fprintf(stderr, opt" requires an argument!\n"); \
			 	       return -1;                                      \
				     }                                                 \
                                   else                                                \
                                     {                                                 \
                                       *(pp) = argv[++i];                              \
                                       last_opt++;                                     \
                                     }                                                 \
                                 } while(0)

#define str_equals(str) (strcmp((str), argv[i]) == 0)
  while (i < argc)
    {
      if (argv[i][0] == '-')
	last_opt++;

      if (str_equals("-n") || str_equals("--name")) /* Memo-name */
	assign_optarg("-n", pp_memo_name);
      else if (str_equals("-o") || str_equals("--outfile"))
	assign_optarg("-i", pp_outfile_name);
      else if (str_equals("-h") || str_equals("--help"))
	{
	  print_help();
	  exit(0);
	}
      i++;
    }
#undef str_equals
#undef assign_optarg

  return last_opt;
}

/* Write a header for the memo */
static int write_convert_header(FILE *p_outfile, char *p_memo_name)
{
  /* Write some header information and the filename */
  FPUTS("/* Don't change this file! It is auto-generated! */\n\n", p_outfile);
  FPUTS("#define FILENAME \"", p_outfile);
  FPUTS(p_memo_name, p_outfile);
  FPUTS("\"\n\n", p_outfile);

  /* Start of the data */
  FPUTS("char *pp_file_data[]={", p_outfile);

  return 0;
}

/* Write a footer for the memo */
static int write_convert_footer(FILE *p_outfile, int nr_chunks)
{
  /* End of the outfile */
  FPUTS("};\n\n", p_outfile);
  /* Output the number of chunks */
  if (fprintf(p_outfile, "#define NR_CHUNKS %d\n", nr_chunks) <= 0)
    {
      fprintf(stderr, "convert_file(): error in fprintf\n");
      return -1;
    }


  return 0;
}

/* Convert a text file to C-source */
static int convert_file(FILE *p_infile, FILE *p_outfile, char *p_memo_name)
{
  static int first=1;
  static int line=0;
  static int chars=0;
  int nr_chunks=1;
  char buf[255];
  char *p;

  /* For each line in the infile */
  while( (p = fgets(buf, 255, p_infile)) )
    {
      /* Remove linefeeds from the input */
      while (strlen(p) >= 1 &&
	  (p[strlen(p)-1] == '\n' ||
	   p[strlen(p)-1] == '\r'))
	p[strlen(p)-1] = '\0';

      /* Do not indent the first chunk */
      if (first)
	{
	  FPUTS("\"", p_outfile);
	  first = 0;
	}
      else
	FPUTS("                      \"", p_outfile);

      FPUTS(p, p_outfile);
      chars += strlen(p);
      line++;

      /* Is this the last entry? */
      if (!feof(p_infile))
	{
	  /* Every 5th line starts a new chunk */
	  if (chars > 300)
	    {
	      FPUTS("\\r\",\n\n", p_outfile);
	      nr_chunks++;
	      chars = 0;
	    }
	  else
	    FPUTS("\\r\"\n", p_outfile);
	}
      else
	FPUTS("\"", p_outfile);
    }

  return nr_chunks;
}

#undef FPUTS


int main(int argc, char *argv[])
{
  char *p_outfile_name = NULL;
  char *p_memo_name = NULL;
  FILE *p_outfile = stdout;
  FILE *p_infile = stdin;
  int nr_chunks;
  int last_opt;

  /* Parse the arguments to the program */
  if ( (last_opt = parse_options(argc, argv, &p_memo_name, &p_outfile_name)) < 0)
    return -1;

  /* Was any options given? */
  if (p_memo_name == NULL)
    p_memo_name = "STD_memo";

  if (p_outfile_name != NULL)
    {
      if ( !(p_outfile = fopen(p_outfile_name, "w")) )
	{
	  fprintf(stderr, "ERROR: Could not open %s\n", p_outfile_name);
	  return -1;
	}
    }

  /* Convert the file */
  if (write_convert_header(p_outfile, p_memo_name) < 0)
    {
      fprintf(stderr, "File conversion header failed!\n");
      return -1;
    }

  /* If no infile was given, assume stdin */
  if (last_opt >= argc)
    {
      if ( (nr_chunks = convert_file(p_infile, p_outfile, p_memo_name)) < 0)
	{
	  fprintf(stderr, "File conversion failed!\n");
	  return -1;
	}
    }
  else
    {
      /* Else, convert each file */
      while (last_opt < argc)
	{
	  if ( !(p_infile = fopen(argv[last_opt], "r")) )
	    {
	      fprintf(stderr, "ERROR: Could not open %s\n", argv[last_opt]);
	      return -1;
	    }
	  if ( (nr_chunks = convert_file(p_infile, p_outfile, p_memo_name)) < 0)
	    {
	      fprintf(stderr, "File conversion failed!\n");
	      return -1;
	    }
	  fclose(p_infile);
	  last_opt++;
	}
    }


  if (write_convert_footer(p_outfile, nr_chunks) < 0)
    {
      fprintf(stderr, "File conversion header failed!\n");
      return -1;
    }



  if (p_outfile != stdout)
    fclose(p_outfile);

  return 0;
}
