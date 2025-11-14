#include <stdio.h>
#include <cudd/cudd.h>

int main(int argc, char *argv[]) {
  DdManager *manager;
  
  manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  
  if (manager == NULL) {
    fprintf(stderr, "Failed to initialize CUDD manager\n");
    return 1;
  }

  Cudd_PrintVersion(stdout);
  Cudd_Quit(manager);
  
  return 0;
}
