/*
 * Copyright (c) 1990 Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 */

/*
FUNCTION
<<exit>>---end program execution

INDEX
	exit

SYNOPSIS
	#include <stdlib.h>
	void exit(int <[code]>);

DESCRIPTION
Use <<exit>> to return control from a program to the host operating
environment.  Use the argument <[code]> to pass an exit status to the
operating environment: two particular values, <<EXIT_SUCCESS>> and
<<EXIT_FAILURE>>, are defined in `<<stdlib.h>>' to indicate success or
failure in a portable fashion.

<<exit>> does two kinds of cleanup before ending execution of your
program.  First, it calls all application-defined cleanup functions
you have enrolled with <<atexit>>.  Second, files and streams are
cleaned up: any pending output is delivered to the host system, each
open file or stream is closed, and files created by <<tmpfile>> are
deleted.

RETURNS
<<exit>> does not return to its caller.

PORTABILITY
ANSI C requires <<exit>>, and specifies that <<EXIT_SUCCESS>> and
<<EXIT_FAILURE>> must be defined.

Supporting OS subroutines required: <<_exit>>.
*/

#include <stdlib.h>
#include <unistd.h>	/* for _exit() declaration */
#include <reent.h>
#include "atexit.h"

/*
 * Exit, flushing stdio buffers if necessary.
 */

void
exit (int code)
{
#ifdef _LITE_EXIT
  /* Refer to comments in __atexit.c for more details of lite exit.  */
  void __call_exitprocs (int, void *) __attribute__((weak));
  if (__call_exitprocs)
#endif
    __call_exitprocs (code, NULL);

  if (__stdio_exit_handler != NULL)
    (*__stdio_exit_handler) ();

#ifdef __TRICORE__
  /* the tsim will test a14 for 0x900d to output pass or fail at the
   * symbol _exit. Therefor we set a14 to 0x900d if the
   * exit code is zero
   */
  if (code)
    {
      __asm__ volatile ("mov.a %%a14, %0;mov  %%d4,%1;j _exit":: "d" (code | 0xffff0000), "d" (code));
    }
  else
    __asm__ volatile ("mov.a %%a14, %0;mov  %%d4,%1;j _exit":: "d" (0x900d),
                      "d" (code));
#endif
  
  _exit (code);
}
