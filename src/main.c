/*
 *
 *    file  :  main.c
 *    name  :  Marcus Tibbitts
 *    date  :  21-08-2021 
 *  
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "mezu.h"

void print_str(char* str)
{
   printf("%s", str);
}

int main(int arvc, char *argv[])
{
   if(MEZU_SUCCESS == mezu_init(NULL, print_str, NULL,   "\r\n   ***   MEZU   ***\r\n"
                                                         "   Test Program\r\n"))
   {
      while (true)
      {
         /* code */
      }
   }
   else
   {
      printf("\r\nERR:mezu-Initialisation_Failed\r\n");
   }
   return 0;
}