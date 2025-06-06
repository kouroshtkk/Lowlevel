#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n",argv[0]);
  printf("\t -n - create new database file\n");
  printf("\t -f - required path ro database file\n");
}

int main(int argc, char *argv[]) { 
  char *filepath =NULL;
  char *addstring = NULL;
  char *remove_emp= NULL;
  char *update_emp= NULL;
  int c;
  bool newfile=false;
  int dbfd=-1;
  bool list_emp=false;
  struct dbheader_t *dbhd=NULL;
  struct employee_t *employees = NULL;
  while((c= getopt(argc,argv,"nf:a:lr:u:"))!=-1)
    {
      switch(c) {
      case 'n':
	newfile=true;
	break;
      case 'f':
	filepath = optarg;
	break;
      case 'a':
	addstring = optarg;
	break;
      case 'l':
	list_emp=true;
	break;
      case 'r':
	remove_emp=optarg;
	break;
      case 'u':
	update_emp=optarg;
	break;
      case '?':
	printf("unknown usage\n");
	break;
      default:
	  return -1;
      }
    }
  if(filepath==NULL)
    {
      printf("Filepath is a required argument\n");
      print_usage(argv);
      return 0;
    }
  if(newfile)
    {
      dbfd = create_db_file(filepath);
      if (dbfd == STATUS_ERROR)
	{
	  printf("unable to create database file\n");
	  return -1;
	}
      if(create_db_header(dbfd,&dbhd)==STATUS_ERROR)
	{
	  printf("Failed to create database header\n");
	  return -1; 
	}
    }
  else
    {
      dbfd = open_db_file(filepath);
      if(dbfd== STATUS_ERROR)
	{
	  printf("unable to open database file\n");
	  return -1;
	}
      if(validate_db_header(dbfd,&dbhd) == STATUS_ERROR)
	{
	  printf("Database not valid! \n");
	  return STATUS_ERROR;
	}
    }
  if(read_employees(dbfd,dbhd,&employees)!=STATUS_SUCCESS)
    {
      printf("Failed to read employees\n");
      return STATUS_ERROR;
    }
  if(addstring)
    {
      ++dbhd->count;
      employees = realloc(employees,dbhd->count*sizeof(struct employee_t));
      add_employee(dbhd,employees,addstring);
    }
  if(remove_emp)
    {
      remove_employee(dbhd,&employees,remove_emp);
    }
  if(update_emp)
    {
      update_employee(dbhd,employees,update_emp);
    }
  if(list_emp)
    {
      list_employees(dbhd,employees);
    }
  output_file(dbfd,dbhd,employees);
  free(dbhd);
  free(employees);
  printf("Newfile: %d\n", newfile);
  printf("Filepath: %s\n", filepath);
  return 0;
}

