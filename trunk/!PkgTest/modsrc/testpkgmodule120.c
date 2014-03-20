/* Copyright (c) 2005-2007 GCCSDK Developers
 *
 * Example module using GCC.
 */

#include <stdio.h>

#include "module120.h"

_kernel_oserror *
gccmodule_init(const char *cmd_tail, int podule_base, void *pw)
{
  puts("GCC example module initialise");

  return NULL;
}


_kernel_oserror *
gccmodule_final(int fatal, int podule, void *pw)
{
  puts("GCC example module finalise");

  return NULL;
}
