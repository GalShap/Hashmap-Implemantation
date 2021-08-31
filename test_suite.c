#include "hashmap.h"
#include "hash_funcs.h"
#include "test_pairs.h"
#include <assert.h>
#include "test_suite.h"
#include <stdio.h>
#define TEST_KEY_STRING_1 "test1"
#define FIRST_REHASH_UP 13
#define TEST_KEY_2 'b'
#define FIRST_REHASH_DOWN 8
#define TEST_KEY_1 'a'
#define TEST_VAL_1 1
#define TEST_VAL_2 2
#define STRING_TEST_LEN 8
void test_null_insert(hashmap *map);
void test_invalid_insert();
void insert_single_pair(hashmap *map,char *key,int *val,int expected);
void test_single_insert();
void test_hashmap_insert_copy();
void test_insert_after_removal();
void test_hash_map_at_after_rehash();
void test_invalid_hashmap_at();
void test_hash_map_at_string_pair();
void test_rehash_on_insert ();
void test_rehash_on_invalid_insert ();
void erase_n_pairs (hashmap *p_hashmap, int start, int end);
void test_single_removal ();
void test_invalid_removal ();
void test_removal_rehash ();
void test_invalid_removal_rehash ();
void insert_n_pairs (hashmap *map, int start, int end);
void check_load_factor_before_rehash_up ();
void check_load_factor_after_rehash_up ();
void check_load_factor_before_rehash_down ();
void check_load_factor_after_rehash_down ();
void check_invalid_load_factor ();
/**
 * This function checks the hashmap_insert function of the hashmap library.
 * If hashmap_insert fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_insert(void){
  test_invalid_insert();
  test_single_insert();
  test_hashmap_insert_copy();
  test_insert_after_removal();
  test_rehash_on_insert();
  test_rehash_on_invalid_insert();
}
/**
 * checking inserting invalid input to insert function on the bridge between
 * rehash
 */
void test_rehash_on_invalid_insert ()
{
  hashmap *map = hashmap_alloc (hash_char);
  int i =0;
  for(; i<FIRST_REHASH_UP-1;i++){
      insert_single_pair (map,(char*)&i,&i,1);
    }
  i--;
  assert(map->size==(FIRST_REHASH_UP-1));//all items inserted
  assert(map->capacity==HASH_MAP_INITIAL_CAP);//no rehash preformed
  insert_single_pair (map,(char*)&i,&i,0);//this insert should fail because of
  // duplicate keys
  assert(hashmap_insert (map,NULL)==0);
  assert(map->size==(FIRST_REHASH_UP-1));//no items should be added
  assert(map->capacity==HASH_MAP_INITIAL_CAP);//rehash should no preform
  hashmap_free (&map);
}
/**
 * cheking rehash only accured when suitable
 */
void test_rehash_on_insert ()
{
  hashmap *map = hashmap_alloc (hash_char);
  int i =0;
  for(; i<FIRST_REHASH_UP-1;i++){
      insert_single_pair (map,(char*)&i,&i,1);
  }
  assert(map->size==12);//all items inserted
  assert(map->capacity==16);//no rehash preformed
  i++;
  insert_single_pair (map,(char*)&i,&i,1);//this insert should trigger a rehash
  assert(map->size==13);
  assert(map->capacity==32);
  hashmap_free (&map);
}
/**
 * checking invalid input to insert function
 */
void test_invalid_insert(){
  hashmap *map = hashmap_alloc (hash_char);
  char test_key = TEST_KEY_1;
  int test_val = TEST_VAL_1;
  //test null inset
  test_null_insert (map);
  insert_single_pair(map,&test_key,&test_val,1);//insert first item
  insert_single_pair(map,&test_key,&test_val,0);//inserting same item twice
  test_val=TEST_VAL_2;
  insert_single_pair(map,&test_key,&test_val,0);//inserting twice after
  // changing value
  hashmap_free (&map);
}
/**
 * checkiong null insert to the map
 * @param map  map to check
 */
void test_null_insert(hashmap *map){
  assert(hashmap_insert(map,NULL)==0);
}
/**
 * inserting given paoir and checking if the result is good
 * @param map map to insert
 * @param key key of pair
 * @param val value of pair
 * @param expected excpected return from insert
 */
void insert_single_pair(hashmap *map,char *key,int *val,int expected){
  pair* test_pair = pair_alloc (key,val,char_key_cpy,int_value_cpy,
                                char_key_cmp,int_value_cmp,char_key_free,
                                int_value_free);
  if(expected==1){
      assert(hashmap_insert (map,test_pair)!=0);
  }
  else{
      assert(hashmap_insert (map,test_pair)==0);
  }
  
  pair_free ((void **) &test_pair);
}
/**
 * inserting given paoir and checking if the result is good
 * @param map map to insert
 * @param key key of pair
 * @param val value of pair
 * @param expected excpected return from insert
 */
void insert_single_pair_string(hashmap *map,char *key,char *val,int expected){
  pair* test_pair = pair_alloc (key,val,char_key_cpy,string_value_cpy,
                                char_key_cmp,string_value_cmp,char_key_free,
                                string_value_free);
  if(expected==1){
      assert(hashmap_insert (map,test_pair)!=0);
    }
  else{
      assert(hashmap_insert (map,test_pair)==0);
    }
  pair_free ((void **) &test_pair);
}
/**
 * checking single insert changes capacity correctly
 */
void test_single_insert(){
  hashmap *map = hashmap_alloc (hash_char);
  char key = TEST_KEY_1;
  int val = TEST_VAL_1;
  insert_single_pair (map,&key,&val,1);
  assert(map->size==1);
  assert(map->capacity==HASH_MAP_INITIAL_CAP);
  hashmap_erase (map,&key);
  insert_single_pair (map,&key,&val,1);//checking adding same item after
  // erasure
  hashmap_free (&map);
}
/**
 * checking that what you insert to the table is a copy and not pointer
 */
void test_hashmap_insert_copy(void)
{
  hashmap  *map= hashmap_alloc (hash_char);
  char key = TEST_KEY_1;
  char *val = TEST_KEY_STRING_1;
  insert_single_pair_string (map,&key,val,1);
  char *returned_val = hashmap_at (map,&key);
  assert(returned_val!=val);
  assert(strcmp (returned_val,val)==0);
  hashmap_free (&map);
}
/**
 * this test checks inserting element after removing elements that will
 * cause rehasing.
 */
void test_insert_after_removal(void)
{
  hashmap *map = hashmap_alloc (hash_char);
  for(int i=0; i < FIRST_REHASH_UP; i++){//this will cause a rehash
      insert_single_pair (map,(char*)&i,&i,1);
  }
  for(int i=FIRST_REHASH_DOWN; i < FIRST_REHASH_UP; i++){//this will cause a rehash down
      hashmap_erase (map,&i);
    }
  for(int i =0;i<FIRST_REHASH_DOWN;i++){
      int *return_val = hashmap_at (map,&i);
      assert(*return_val==i);//checking keys not lost
  }
  for(int i =0;i<FIRST_REHASH_DOWN;i++){
      insert_single_pair (map,(char*)&i,&i,0);//checking can't add on
      // existing keys
    }
  for(int i=FIRST_REHASH_DOWN; i < FIRST_REHASH_UP; i++){
      insert_single_pair (map,(char*)&i,&i,1);//checking can add erased keys
    }
  hashmap_free (&map);
}
/**
 * This function checks the hashmap_at function of the hashmap library.
 * If hashmap_at fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_at(void){
  test_invalid_hashmap_at();
  test_hash_map_at_after_rehash();
  test_hash_map_at_string_pair();
}
/**
 * checking invalid hashmap at
 */
void test_invalid_hashmap_at(){
  hashmap  *map = hashmap_alloc (hash_char);
  char key = TEST_KEY_1;
  int val = TEST_VAL_1;
  assert(hashmap_at (map,NULL)==NULL);//cheking null key
  assert(hashmap_at (NULL,&key)==NULL);//checking null map
  insert_single_pair (map,&key,&val,1);
  assert(hashmap_at (NULL,&key)==NULL);//checking null map on existing key
  char not_key = TEST_KEY_2;
  assert(hashmap_at (map,&not_key)==NULL);//checking worng key
  assert(*(char*)hashmap_at(map,&key)==val);
  hashmap_free (&map);
}
/**
 * checking hashmap at after rehash up and down
 */
void test_hash_map_at_after_rehash(){
  hashmap *map = hashmap_alloc (hash_char);
  for(int i =0; i < FIRST_REHASH_UP; i++){
      insert_single_pair (map,(char*)&i,&i,1);
  }
  for(int i = FIRST_REHASH_DOWN;i<FIRST_REHASH_UP;i++){
      hashmap_erase (map,&i);
  }
  for(int i =0; i < FIRST_REHASH_DOWN; i++){
      assert(*(int*)hashmap_at (map,&i)==i);
  }
  hashmap_free (&map);
}
/**
 * checking hashmap ar for string pair
 */
void test_hash_map_at_string_pair(){
  hashmap *map = hashmap_alloc (hash_char);
  char* values[STRING_TEST_LEN] = {"val1","val2","val3","val4","val5","val6","val7",
                   "val8"};
  char keys[STRING_TEST_LEN] = {'a','b','c','d','e','f','g','h'};
  for(int i=0;i<STRING_TEST_LEN;i++){
      insert_single_pair_string (map,&keys[i],values[i],1);
  }
  for(int i=0;i<STRING_TEST_LEN;i++){
      char * returned_val = hashmap_at (map,&keys[i]);
      assert(strcmp(returned_val,values[i])==0);
  }
  hashmap_free (&map);
}

/**
 * This function checks the hashmap_erase function of the hashmap library.
 * If hashmap_erase fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_erase(void)
{
  test_single_removal();
  test_invalid_removal();
  test_removal_rehash();
  test_invalid_removal_rehash();
}
/**
 * testing that rehash does not accure on invalid removal
 */
void test_invalid_removal_rehash ()
{
  hashmap *map = hashmap_alloc (hash_char);
  for(int i = 0; i<FIRST_REHASH_UP;i++){
      insert_single_pair (map,(char*)&i,&i,1);
    }//current status 13/32
  int i = FIRST_REHASH_DOWN;
  for(; i<FIRST_REHASH_UP;i++){
      assert(hashmap_erase (map,&i)!=0);
    }//current status 8/32 (0.25)
  assert(map->size==8);
  assert(map->capacity==32);
  assert(hashmap_erase (map,&i)==0);//this should not trigger a rehash
  // because key not exists
  assert(hashmap_erase (map,NULL)==0);
  assert(map->size==8);
  assert(map->capacity==32);
  hashmap_free (&map);
}

/*
 * checking that removal triggers rehash correctly
 */
void test_removal_rehash ()
{
  hashmap *map = hashmap_alloc (hash_char);
  insert_n_pairs (map, 0, FIRST_REHASH_UP);//rehash should accure(13/32)
  int i = FIRST_REHASH_DOWN;
  for(; i<FIRST_REHASH_UP;i++){
      assert(hashmap_erase (map,&i)!=0);
  }//current status 8/32 (0.25)

  assert(map->size==8);
  assert(map->capacity==32);
  i=7;
  assert(hashmap_erase (map,&i)!=0);//this sould trigger a rehash
  assert(map->size==7);
  assert(map->capacity==16);
  hashmap_free (&map);
}
/**
 * insert n pairst to the map
 * @param map map to insert to
 * @param start index of first pair to insert
 * @param end index of last pair to insert
 */
void insert_n_pairs (hashmap *map, int start, int end)
{
  for(int i = start; i < end; i++){
      insert_single_pair (map,(char*)&i,&i,1);
  }//current status 14/32
}
/**
 * checking invalid removal of items
 */
void test_invalid_removal ()
{
  hashmap *map = hashmap_alloc (hash_char);
  char key = TEST_KEY_1;
  int val = TEST_VAL_1;
  assert(hashmap_erase (map,&key)==0);
  assert(hashmap_erase (NULL,&key)==0);
  assert(hashmap_erase (map,NULL)==0);
  insert_single_pair (map,&key,&val,1);
  assert(hashmap_erase (map,&key)==1);//the item is removed
  assert(hashmap_erase (map,&key)==0);//checking you cant remove twice
  hashmap_free (&map);
}
/**
 * checking removing single item acttuly removes it
 */
void test_single_removal ()
{
   hashmap *map = hashmap_alloc (hash_char);
   char key = TEST_KEY_1;
   int val = TEST_VAL_1;
   insert_single_pair (map,&key,&val,1);
   assert(hashmap_erase (map,&key)==1);
   assert(hashmap_at (map,&key)==NULL);
  hashmap_free (&map);
}


/**
 * This function checks the hashmap_get_load_factor function of the hashmap library.
 * If hashmap_get_load_factor fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_get_load_factor(void){
  check_load_factor_before_rehash_up();
  check_load_factor_after_rehash_up();
  check_load_factor_before_rehash_down();
  check_load_factor_after_rehash_down();
  check_invalid_load_factor();

}
/**
 * cheking invalid input to load factor function
 */
void check_invalid_load_factor ()
{
  assert(hashmap_get_load_factor (NULL)==-1);
  hashmap *map = hashmap_alloc (hash_char);
  assert(hashmap_get_load_factor (map)==0);
  hashmap_free (&map);
}
/**
 * checking the load changes after rehash down
 */
void check_load_factor_after_rehash_down ()
{
  hashmap *map = hashmap_alloc (hash_char);
  insert_n_pairs (map,0,FIRST_REHASH_UP);//13/32 - rehash up
  erase_n_pairs (map,FIRST_REHASH_DOWN,FIRST_REHASH_UP);//8/32 - no rehash
  // yet
  assert(hashmap_get_load_factor (map)==HASH_MAP_MIN_LOAD_FACTOR);
  int i = FIRST_REHASH_DOWN-1;
  hashmap_erase (map,&i);//this dould cause a rehash down 7/16
  assert(hashmap_get_load_factor (map)==(float)7.0/16.0);
  hashmap_free (&map);
}

/**
 * checking load factor before rehash down is preformed
 */
void check_load_factor_before_rehash_down ()
{
  hashmap *map = hashmap_alloc (hash_char);
  insert_n_pairs (map,0,FIRST_REHASH_UP);//13/32 - rehash up
  erase_n_pairs (map,FIRST_REHASH_DOWN,FIRST_REHASH_UP);//8/32 - no rehash
  // yet
  assert(hashmap_get_load_factor (map)==HASH_MAP_MIN_LOAD_FACTOR);
  hashmap_free (&map);
}
/**
 * removing entries from the map
 * @param map map top remove from
 * @param start index of first entry
 * @param end index of last entry
 */
void erase_n_pairs (hashmap *map, int start, int end){
  for(int i = start; i<end;i++){
      assert(hashmap_erase (map,&i)!=0);
  }
}

/**
 * checking the load factor is correct after rehash
 */
void check_load_factor_after_rehash_up ()
{
  hashmap *map = hashmap_alloc (hash_char);
  insert_n_pairs (map,0,FIRST_REHASH_UP);//13/32
  assert(hashmap_get_load_factor (map)==(float)13.0/32.0);
  hashmap_free (&map);
}
/**
 * checking the load factor is correct before rehash
 */
void check_load_factor_before_rehash_up ()
{
  hashmap *map = hashmap_alloc (hash_char);
  insert_n_pairs (map,0,FIRST_REHASH_UP-1);//12/16
  assert(hashmap_get_load_factor (map)==HASH_MAP_MAX_LOAD_FACTOR);
  hashmap_free (&map);
}
/*
* multiply the value by 2
* @param val value to multiply
*/
void mult_int(valueT val)
{
  int *num = (int *) val;
  *num *= 2;
}
/**
 * returns true id the key char is even in asciii
 * @param key char key
 * @return true id even
 */
int is_key_even(const_keyT key){
  char *c = (char*)key;
  return *c%2==0;
}
/**
 * This function checks the HashMapGetApplyIf function of the hashmap library.
 * If HashMapGetApplyIf fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_apply_if(){
  hashmap* test_map = hashmap_alloc (hash_char);
  insert_n_pairs(test_map,0,50);
  int change_count = hashmap_apply_if (test_map,is_key_even,mult_int);
  assert(change_count==25);
  for(int i=0;i<50;i+=2){
      assert(*(int*)hashmap_at (test_map,&i)==i*2);
    }
  hashmap_free (&test_map);
}