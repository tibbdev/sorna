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
#include "../core/sorna.h"

void print_str(char* str)
{
   printf("%s", str);
}

int main(int arvc, char *argv[])
{
   if(SORNA_SUCCESS == sorna_init(NULL, print_str, NULL,   "\r\n   ***   SORNA   ***\r\n"
                                                         "   Test Program\r\n"))
   {
      while (true)
      {
         /* code */
      }
   }
   else
   {
      printf("\r\nERR:sorna-Initialisation_Failed\r\n");
   }
   return 0;
}