#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TrackedValue {
  void* pointer; 
  bool marked;
} TrackedValue;

static const uint64_t HEAP_SIZE = 50000;
static TrackedValue managedHeap[HEAP_SIZE];
static uint64_t nextValue = 0;

int main() {
  uint64_t half = HEAP_SIZE /2;
  // allocated
  for(uint64_t i = 0; i < half; i++) {
    managedHeap[i] = (TrackedValue) {
      .pointer = calloc(sizeof(int*), 1),
      .marked = 0,
    };
  }

  // marking
  for(uint64_t i = 0; i < half; i++) {
    if(i % 2 == 0) continue;
    managedHeap[i].marked = true;
  }
  nextValue = half;

  // GC run
  for(uint64_t i = 0; i < nextValue; i++) {
    if(managedHeap[i].marked) {
      free(managedHeap[i].pointer);
    }
  }

  // reset
  for(uint64_t i = 0; i < nextValue; i++) {
    managedHeap[i].marked = false;
  }

  printf("RUN\n");
}
