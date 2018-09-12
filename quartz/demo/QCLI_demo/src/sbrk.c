/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
void __aeabi_assert(const char *expr, const char *file, int line)
{
   for(;;)
   {
   }
}

typedef void (*ctor_fn)(void);

extern ctor_fn __init_array_begin;
extern ctor_fn __init_array_end;

void __cpp_initialize__aeabi_(void)
{
   ctor_fn *ctors;

   /* The .init_array section is populated by both RealView and GCC as a list of
      function pointers. All that needs to be done is to go through the array in
      order, and call each function pointer. This needs to happen before any of
      the application code runs (i.e. main()). */
   for(ctors = &__init_array_begin; ctors != &__init_array_end; ctors++)
   {
      (*ctors)();
   }
}

/* Provides one of RealView's optimized versions of __cxa_vec_ctor that is used
   when no destructor is provided and no exceptions are being used. */
void *__aeabi_vec_ctor_nocookie_nodtor(void *user_array, void *(*constructor)(void *), size_t element_size, size_t element_count)
{
   size_t   index;
   intptr_t obj;

   if(constructor != NULL)
   {
      obj = (intptr_t)user_array;

      for(index = 0; index < element_count; ++index)
      {
         (*constructor)((void *)obj);
         obj += element_size;
      }
   }

   /* AEABI specifies this must return its first argument. */
   return user_array;
}

void *_sbrk(int x)
{
   return(NULL);
}

