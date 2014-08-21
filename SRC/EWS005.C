/*
 * Copyright (c) 1995,1996
 * David M. Howard
 * daveh@hendrix.reno.nv.us
 * www.greatbasin.net/~daveh
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  
 * David M. Howard makes no representations about the suitability of this
 * software for any purpose.  It is provided 'as is' without express or
 * implied warranty. Do not use it in systems where damage to property
 * or injury to people may occur in the event of a malfunction
 *
 * A supported version of this software including updates is available.
 * Consulting services for porting, modification, web user interface design
 * and implementation are available also.
 * Contact David M. Howard at the above email address for details.
 */
/*H*/
#include <ctype.h>
#include "ews.h"

/* =============================== */
/* MISCELLANEOUS SUPPORT FUNCTIONS */
/* =============================== */

/* convert a two digit ascii hex string to a char with that binary value */
/* doesn't work if input is not a two character ascii hex value */
/* can be upper or lower case                                   */
char em_hex2char( char *what )
{
   char digit;
   char lsn;
   char msn;

   msn = what[0];
   lsn = what[1];

   /* the & 0xdf converts lower case to upper case */
   /*lint -e734 int to char */
   digit = ( msn >= 'A' ? (( msn & 0xdf ) - 'A' ) + 10 : ( msn - '0' ));
   digit *= 16;
   digit += ( lsn >= 'A' ? (( lsn & 0xdf ) - 'A' ) + 10 : ( lsn - '0' ));
   /*lint +e734 */
   return (digit);
}

/* remove escape sequences from a string                            */
/* it operates on the input buffer because                          */
/* removing escapes will always make the string smaller             */
/* so there is no overflow danger                                   */
/* it expects that % will only be found as the leader of the escape */
/* sequence and never as an actual character                        */
/* it also converts plus signs in the input to spaces in the output */
/* THE CODE FOR THE FOLLOWING FUNCTION, EM_UNESCAPE WAS TAKEN FROM  */
/* THE CERN HTTPD SOURCE CODE AND IS NOT SUBJECT TO THE COPYRIGHT   */
/* THAT APPLIES TO THE RESET OF THE CODE                            */
char *em_unescape( char *s )
{
   char *r = s;
   int x, y;

   /*  x points at converted string, y points at original string */
   for (x = 0, y = 0; r[y]; ++x, ++y) {
      /* copy 1 character */
      r[x] = r[y];

      /* if a % is found, convert next hex digit to a single char */
      /* hex digits in the input stream are 3 chars, %nn          */
      if (r[x] == '%') {
         /* convert and overwrite original character */
         r[x] = em_hex2char( &r[y + 1] );
         /* skip forward two to next input character */
         y += 2;
      }
      /* if a '+' is found, convert it to a space */
      else if (r[x] == '+') {
         r[x] = ' ';
      }
   }
   /* delimit the character */
   r[x] = '\0';

   /* return pointer to original string */
   return s;
}

/* convert all instances of character 'from' to character 'to' */
char *em_strccvt( char *s, char from, char to )
{
   char *r = s;

   while (*r) {
      if (*r == from) {
         *r = to;
      }
      r++;
   }

   return s;
}

/* convert an ascii string to all lower case */
/* requires tolower function                 */
char *em_strlwr( char *s )
{
   char *r;

   r = s;
   while (*r) {
      *r = ( char ) tolower( *r );
      r++;
   }

   return s;
}


/* compare argument strings up to the equal sign  */
int em_argcmp(const char *templte,const char *arg)
{
   const char *t;
   const char *a;
   int         match;

   if ((templte == 0)||(arg == 0)) {
      return 1; /* no match */
   }

   t = templte;
   a = arg;
   /* the argument always has an '=' delimiter */
   match = 0;
   for(;;) {
      if ((*a == '=')&&(*t == 0)) {
         /* at end of both strings, OK */
         match = 0;
         break;
      }

      /* at end of argument, no match */
      if (*a == '=') {
         match = 1;
         break;
      }

      /* at end of template, no match */
      if (*t == 0) {
         match = 1;
         break;
      }

      /* check for match */
      if (*t != *a) {
         /* no match */
         match = 1;
         break;
      }
      a++;
      t++;
   }

   return match;
}

/* copy an argument, delimited by & or \0             */
/* return pointer to next position in source          */
/* no length check. be sure destination is big enough */
char *em_argcopy(char *dest,char *src)
{
   char *d = dest;
   char *s = src;

   /* skip over name of argument just past '=' */
   while((*s != 0)&&(*s != '=')) {
      s++;
   }

   /* skip over equal sign */
   if (*s == '=') {
      s++;
   }

   while((*s != 0)&&(*s != '&')) {
      *d = *s;
      d++;
      s++;
   }
   if (*s == '&') {
      s++;
   }

   /* delimit destination */
   *d = 0;

   /* unescape the destination */
   em_unescape(dest);

   return s;
}

char *em_argnext(char *arg) /* skip over argument */
{
   char *a = arg;

   /* find end of argument (& or \0) */
   for(;;) {
      /* next argument found, increment a to next char */
      if (*a == '&') {
         a++;
         break;
      }

      /* end of arguments, leave a pointing at 0 */
      if (*a == 0) {
         break;
      }

      /* next character */
      a++;
   }
   /* return pointer to first char of next argument or 0 */
   return a;
}
