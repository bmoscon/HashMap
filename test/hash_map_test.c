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


#define MAP_SIZE 100000

uint32_t hash(const void *str) 
{
  return atoi(str)/2;
}

int main()
{ 
  int i;
  hash_map_st *map = hm_init(hash);
  hash_map_it *it = NULL;
  hm_pair_st *dump;

  printf("verifying inserts with overflow...\n");
  for(i = 0; i < MAP_SIZE; ++i) {
    char key[10];
    snprintf(key, 10, "%d", i);
    assert(hm_insert(map, key, strlen(key), &i, sizeof(int)) == OK);
  }
  
  printf("verifying duplicate inserts fail...\n");
  for(i = 0; i < MAP_SIZE; ++i) {
    char key[10];
    snprintf(key, 10, "%d", i);
    assert(hm_insert(map, key, strlen(key), &i, sizeof(int)) == DUPLICATE);
  }
  
  printf("verifying data structure integrity...\n");
  assert(map->entries == MAP_SIZE);
  
  printf("verifying key->value pairs exist...\n");
  for(i = 0; i < MAP_SIZE; ++i) {
    char key[10];
    snprintf(key, 10, "%d", i);
    assert(hm_exists(map, key, strlen(key)));
    assert(*(int *)hm_get_value(map, key, strlen(key)) == i);
  }

  printf("verifying non-present keys fail existence test...\n");
  for(i = MAP_SIZE; i < MAP_SIZE*5; ++i) {
    char key[10];
    snprintf(key, 10, "%d", i);
    assert(hm_exists(map, key, strlen(key)) == FALSE);
  }
  
  printf("iterator test...\n");
  it = hm_it_init(map);
  assert(it);

  // start at 1 beause init'ing the iterator 
  // gives us the first value, so we only have N-1 to test
  for (i = 1; i < MAP_SIZE; ++i) {
    assert(*(int *)hm_it_value(it) < MAP_SIZE);
    assert(atoi((const char *)hm_it_key(it)) < MAP_SIZE);
    assert(hm_it_next(it) == OK);
  }

  hm_it_free(it);
  it = hm_it_init(map);
  assert(it);
  
  dump = hm_dump(map);
  
  printf("verifying iterator values...\n");
  for (i = 0; i < MAP_SIZE; ++i) {
    assert(*(int *)hm_it_value(it) == *(int *)dump[i].value);
    assert(strcmp(hm_it_key(it), dump[i].key) == 0);
    hm_it_next(it);
  }
  
  hm_dump_free(dump);
  hm_it_free(it);
  hm_free(map);  

  printf("\n-------- PASS --------\n");
  return (0);
}
