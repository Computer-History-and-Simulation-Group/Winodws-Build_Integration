/*
 * pthread_mutex_init.c
 *
 * Description:
 * This translation unit implements mutual exclusion (mutex) primitives.
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

#include "pthread.h"
#include "implement.h"


int
pthread_mutex_init (pthread_mutex_t * mutex, const pthread_mutexattr_t * attr)
{
  int result = 0;
  pthread_mutex_t mx;

  if (mutex == NULL)
    {
      return EINVAL;
    }

  if (attr != NULL && *attr != NULL)
    {
      if ((*attr)->pshared == PTHREAD_PROCESS_SHARED)
        {
          /*
           * Creating mutex that can be shared between
           * processes.
           */
#if _POSIX_THREAD_PROCESS_SHARED >= 0

          /*
           * Not implemented yet.
           */

#error ERROR [__FILE__, line __LINE__]: Process shared mutexes are not supported yet.

#else

          return ENOSYS;

#endif /* _POSIX_THREAD_PROCESS_SHARED */
        }
    }

  mx = (pthread_mutex_t) calloc (1, sizeof (*mx));

  if (mx == NULL)
    {
      result = ENOMEM;
    }
  else
    {
      mx->lock_idx = 0;
      mx->recursive_count = 0;
      mx->robustNode = NULL;
      if (attr == NULL || *attr == NULL)
        {
          mx->kind = PTHREAD_MUTEX_DEFAULT;
        }
      else
        {
          mx->kind = (*attr)->kind;
          if ((*attr)->robustness == PTHREAD_MUTEX_ROBUST)
            {
              /*
               * Use the negative range to represent robust types.
               * Replaces a memory fetch with a register negate and incr
               * in pthread_mutex_lock etc.
               *
               * Map 0,1,..,n to -1,-2,..,(-n)-1
               */
              mx->kind = -mx->kind - 1;

              mx->robustNode = (ptw32_robust_node_t*) malloc(sizeof(ptw32_robust_node_t));
              if (NULL == mx->robustNode)
        	{
        	  result = ENOMEM;
        	}
              else
        	{
        	  mx->robustNode->stateInconsistent = PTW32_ROBUST_CONSISTENT;
        	  mx->robustNode->mx = mx;
        	  mx->robustNode->next = NULL;
        	  mx->robustNode->prev = NULL;
        	}
            }
        }

      if (0 == result)
	{
	  mx->ownerThread.p = NULL;

	  mx->event = CreateEvent (NULL, PTW32_FALSE,    /* manual reset = No */
				   PTW32_FALSE,           /* initial state = not signalled */
				   NULL);                 /* event name */

	  if (0 == mx->event)
	    {
	      result = ENOSPC;
	    }
	}
    }

  if (0 != result)
    {
      if (NULL != mx->robustNode)
	{
	  free (mx->robustNode);
	}
      free (mx);
      mx = NULL;
    }

  *mutex = mx;

  return (result);
}