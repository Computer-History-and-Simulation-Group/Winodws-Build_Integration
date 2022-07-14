/*
 * pthread_attr_setname_np.c
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads4w - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999-2018, Pthreads4w contributors
 *
 *      Homepage: https://sourceforge.net/projects/pthreads4w/
 *
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      https://sourceforge.net/p/pthreads4w/wiki/Contributors/
 *
 * This file is part of Pthreads4w.
 *
 *    Pthreads4w is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Pthreads4w is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Pthreads4w.  If not, see <http://www.gnu.org/licenses/>. *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include "pthread.h"
#include "implement.h"

#if defined(PTW32_COMPATIBILITY_BSD) || defined(PTW32_COMPATIBILITY_TRU64)
int
pthread_attr_setname_np(pthread_attr_t * attr, const char *name, void *arg)
{
  int len;
  int result;
  char tmpbuf[PTHREAD_MAX_NAMELEN_NP];
  char * newname;
  char * oldname;

  /*
   * According to the MSDN description for snprintf()
   * where count is the second parameter:
   * If len < count, then len characters are stored in buffer, a null-terminator is appended, and len is returned.
   * If len = count, then len characters are stored in buffer, no null-terminator is appended, and len is returned.
   * If len > count, then count characters are stored in buffer, no null-terminator is appended, and a negative value is returned.
   *
   * This is different to the POSIX behaviour which returns the number of characters that would have been written in all cases.
   */
  len = snprintf(tmpbuf, PTHREAD_MAX_NAMELEN_NP-1, name, arg);
  tmpbuf[PTHREAD_MAX_NAMELEN_NP-1] = '\0';
  if (len < 0)
    {
      return EINVAL;
    }

  newname = _strdup(tmpbuf);

  oldname = (*attr)->thrname;
  (*attr)->thrname = newname;
  if (oldname)
    {
      free(oldname);
    }

  return 0;
}
#else
int
pthread_attr_setname_np(pthread_attr_t * attr, const char *name)
{
  char * newname;
  char * oldname;

  newname = _strdup(name);

  oldname = (*attr)->thrname;
  (*attr)->thrname = newname;
  if (oldname)
    {
      free(oldname);
    }

  return 0;
}
#endif