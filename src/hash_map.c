/*
 * hash_set.c
 *
 * Hash Set Implementation
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

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "hash_map.h"


hash_map_st* hm_init(size_t size, uint32_t (*hash_fp)(void *)) 
{
  hash_map_st *ret = NULL;

  assert(size > 0);

  ret = (hash_map_st *)malloc(sizeof(hash_map_st));
  assert(ret);

  ret->entries = 0;
  ret->overflow = 0;
  ret->hash_fp = hash_fp;
  ret->len = size;
  ret->array = calloc(size, sizeof(bucket_st));
  assert(ret->array);

  return (ret);
}


void hm_free(hash_map_st *map)
{
  bucket_st *next = NULL;
  size_t i;
  
  if (!map) {
    return;
  }

  for (i = 0; i < map->len; ++i) {
    next = map->array[i].next;
    free(map->array[i].key);
    free(map->array[i].value);
    while (next) {
      map->array[i].next = next->next;
      free(next->key);
      free(next->value);
      free(next);
      next = map->array[i].next;
    }
  }
  free(map->array);
  free(map);
}

void hm_insert(hash_map_st *map, void *key, size_t k, void *val, size_t v)
{
  uint32_t hash;
  uint32_t index;
  

  hash = map->hash_fp(key);
  index = hash % map->len;

  if (!map->array[index].hash) {
    map->array[index].hash = hash;
    map->array[index].key = malloc(k);
    assert(map->array[index].key);
    memcpy(map->array[index].key, key, k);
    map->array[index].value = malloc(v);
    assert(map->array[index].value);
    memcpy(map->array[index].value, val, v);
    map->array[index].key_size = k;
  } else {
    if (map->array[index].hash == hash) {
      if (map->array[index].key_size == k) {
	if (memcmp(map->array[index].key, key, k) == 0) {
	  // no duplicates allowed
	  return;
	}
      }
    }
    
    if (map->array[index].next == NULL) {
      map->array[index].next = malloc(sizeof(bucket_st));
      assert(map->array[index].next);
      map->array[index].next->hash = hash;
      map->array[index].next->next = NULL;
      map->array[index].next->key = malloc(k);
      assert(map->array[index].next->key);
      memcpy(map->array[index].next->key, key, k);
      map->array[index].next->value = malloc(v);
      assert(map->array[index].next->value);
      memcpy(map->array[index].next->value, val, v);
      ++map->overflow;
    } else {
      bucket_st *b = map->array[index].next;
      
      while (b->next) {
	if (b->hash == hash) {
	  if (b->key_size == k) {
	    if (memcmp(b->key, key, k) == 0) {
	      // no duplicates allowed
	      return;
	    }
	  }
	}
	
	b = b->next;
      }

      if (b->hash == hash) {
	if (b->key_size == k) {
	  if (memcmp(b->key, key, k) == 0) {
	    // no duplicates allowed
	    return;
	  }
	}
      }
      
      b->next = malloc(sizeof(bucket_st));
      assert(b->next);
      b->next->hash = hash;
      b->next->key = malloc(k);
      assert(b->next->key);
      memcpy(b->next->key, key, k);
      b->next->value = malloc(v);
      assert(b->next->value);
      memcpy(b->next->value, val, v);
      b->next->next = NULL;
      ++map->overflow;
    }
  }
  ++map->entries;
}

int hm_exists(const hash_map_st *map, void *key, size_t size)
{
  uint32_t hash;
  uint32_t index;
  bucket_st *b;
  hash = map->hash_fp(key);
  index = hash % map->len;
  
  b = &(map->array[index]);
  
  while (b) {
    if (b->hash == hash) {
      if (b->key_size == size) {
	if (memcmp(b->key, key, size)) {
	  return (1);
	}
      }
    }
    b = b->next;
  }
  
  return (0);
}

void hm_clear(hash_map_st *map)
{
  bucket_st *next = NULL;
  size_t i;
  
  if (!map) {
    return;
  }

 for (i = 0; i < map->len; ++i) {
    next = map->array[i].next;
    free(map->array[i].key);
    free(map->array[i].value);
    while (next) {
      map->array[i].next = next->next;
      free(next->key);
      free(next->value);
      free(next);
      next = map->array[i].next;
    }
  }

  memset(map->array, 0, map->len * sizeof(bucket_st));
  
  map->overflow = 0;
  map->entries = 0;

}


void *hm_get_value(const hash_map_st *map, void *key, size_t size)
{
  uint32_t hash;
  uint32_t index;
  bucket_st *b;

  hash = map->hash_fp(key);
  index = hash % map->len;
  
  b = &(map->array[index]);
  
  while (b) {
    if (b->hash == hash) {
      if (b->key_size == size) {
	if (memcmp(b->key, key, size) == 0) {
	  return (b->value);
	}
      }
    }
    b = b->next;
  }
  
  return (NULL);
}

