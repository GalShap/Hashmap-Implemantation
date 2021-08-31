#include <stdio.h>
#include "test_suite.h"

int main ()
{
  test_hash_map_insert();
  test_hash_map_erase();
  test_hash_map_at();
  test_hash_map_get_load_factor();
  test_hash_map_apply_if();

  return 0;
}
