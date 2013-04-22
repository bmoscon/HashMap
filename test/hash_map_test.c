/*
 * hash_map_test.c
 *
 * Hash Map Test
 * 
 *
 * Copyright (C) 2013  Bryant Moscon - bmoscon@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution, and in the same 
 *    place and form as other copyright, license and disclaimer information.
 *
 * 3. The end-user documentation included with the redistribution, if any, must 
 *    include the following acknowledgment: "This product includes software 
 *    developed by Bryant Moscon (http://www.bryantmoscon.org/)", in the same 
 *    place and form as other third-party acknowledgments. Alternately, this 
 *    acknowledgment may appear in the software itself, in the same form and 
 *    location as other such third-party acknowledgments.
 *
 * 4. Except as contained in this notice, the name of the author, Bryant Moscon,
 *    shall not be used in advertising or otherwise to promote the sale, use or 
 *    other dealings in this Software without prior written authorization from 
 *    the author.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../src/hash_map.h"

//#define MAP_LEN 503
//#define MAP_SIZE 70000


#define MAP_LEN 503
#define MAP_SIZE 70000

uint32_t chksum(void *str) 
{
  char *s = (char *)str;
  int len = strlen(s);
  int i;
  uint32_t c = 0;

  for (i = 0; i < len; ++i) {
    c = (c >> 1) + ((c & 1) << (32-1));
    c += s[i];
  }
  return (c);
}

int main()
{ 
  int i;
  hash_map_st *map = hm_init(MAP_LEN, chksum);
  
  printf("verifying inserts with overflow...\n");
  for(i = 0; i < MAP_SIZE; ++i) {
    char key[10];
    snprintf(key, 10, "%d", i);
    assert(hm_insert(map, key, strlen(key), &i, sizeof(int)));
  }
  
  printf("verifying duplicate inserts fail...\n");
  for(i = 0; i < MAP_SIZE; ++i) {
    char key[10];
    snprintf(key, 10, "%d", i);
    assert(hm_insert(map, key, strlen(key), &i, sizeof(int)) == DUPLICATE);
  }
  
  printf("verifying data structure integrity...\n");
  assert(map->len == MAP_LEN);
  if (MAP_SIZE > MAP_LEN) {
    assert(map->overflow >= MAP_SIZE - MAP_LEN);
  }
  assert(map->entries == MAP_SIZE);
  
  printf("verifying key->value pairs exist...\n");
  for(i = 0; i < MAP_SIZE; ++i) {
    char key[10];
    snprintf(key, 10, "%d", i);
    assert(hm_exists(map, key, strlen(key)));
    assert(*(int *)hm_get_value(map, key, strlen(key)) == i);
  }


  hm_free(map);  

  printf("\n-------- PASS --------\n");
  return (0);
}
