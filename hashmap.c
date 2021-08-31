#include "hashmap.h"
#include "stdbool.h"



#define LOAD_FACTOR_ERR -1
#define VACANT -1
#define INSERT 5
#define DELETE 4

/**
 * allocates a new buckets array for the hash map;
 * @param hash_map a hash map
 * @return a new buckets array
 * @if_fail return NULL.
 */
vector** buckets_alloc(hashmap* hash_map){

    // first alloc an array of pointers
    vector** new_buckets = calloc(hash_map->capacity, sizeof(vector*));

    if (new_buckets == NULL){
        return NULL;
    }

    for (int i = 0; i < hash_map->capacity; ++i) {

        // allocate every vector
        new_buckets[i] = vector_alloc(pair_copy, pair_cmp, pair_free);

        // in case the memory allocation has not succeeded we need to free all
        // vectors
        if (new_buckets[i] == NULL){

            for (int j = 0; j < i; ++j) {
                vector_free(&new_buckets[j]);

            }

            // the hash map and the pointer array of buckets
            // also needs to be freed

            return NULL;

        }

    }

    return new_buckets;
}

/**
 * frees all the buckets until the current vector
 * @param hash_map a hash map
 * @param index the index of the current vector
 */
void free_all_buckets(vector** buckets , int index) {

    for (int i = 0; i < index; ++i) {

        vector_clear(buckets[i]);
        vector_free(&buckets[i]);

        buckets[i] = NULL;

    }

    free(buckets);
    buckets = NULL;
}

/**
 * Allocates dynamically new hash map element.
 * @param func a function which "hashes" keys.
 * @return pointer to dynamically allocated hashmap.
 * @if_fail return NULL.
 */
hashmap *hashmap_alloc (hash_func func){

    // first create a new hash map and allocate the memory
    hashmap *new_hash_map = malloc(sizeof(hashmap));

    if (new_hash_map == NULL){
        return NULL;
    }

    // initialize the hash map data members.
    new_hash_map->capacity = HASH_MAP_INITIAL_CAP;

    new_hash_map->size = 0;

    new_hash_map->hash_func = func;

    new_hash_map->buckets = buckets_alloc(new_hash_map);

    if (new_hash_map->buckets == NULL){
        return NULL;
    }


    // if code got here everything is okay, return the new allocated hashmap
    return new_hash_map;
}

/**
 * Frees a hash map and the elements the hash map itself allocated.
 * @param p_hash_map pointer to dynamically allocated pointer to hash_map.
 */
void hashmap_free (hashmap **p_hash_map){

    hashmap *hash_map_ptr = *p_hash_map;

    // first we need to free all the vectors
    free_all_buckets(hash_map_ptr->buckets, (int) hash_map_ptr->capacity);

    // now free the hash map itself
    free(hash_map_ptr);
}

/**
 * re hashes all values in the hash map
 * @param hash_map a hash map
 * @return 1 if the re assign succeeded, 0 otherwise.
 */
int assign_all_pairs(hashmap *hash_map, int action){

    // first initialize a new buckets array to assign the pairs to.
    vector **temp_buckets = buckets_alloc(hash_map);

    if (temp_buckets == NULL){
        return false;
    }

    for (int i = 0; i < hash_map->size; ++i) {

        // get the current bucket/vector
        vector* cur_vector = hash_map->buckets[i];

        for (int j = 0; j < cur_vector->size ; ++j) {

            //get the pair object in that bucket and its hash key.
            pair *cur_pair = cur_vector->data[j];

            size_t hash_key = hash_map->hash_func(cur_pair->key) &
                    (hash_map->capacity - 1);

            // put the pair in the proper bucket key.
            int is_success = vector_push_back(temp_buckets[hash_key], cur_pair);

            if (!is_success) {

                // couldn't assign one of the pairs,
                free_all_buckets(temp_buckets, i);
                return false;
            }
        }

    }

    // we should now free the former buckets array of hash map
    switch (action) {
        case INSERT:
            for (int i = 0; i < (hash_map->capacity) /
            VECTOR_GROWTH_FACTOR; ++i) {
                vector_free(&hash_map->buckets[i]);
            }
            break;
        case DELETE:
            for (int i = 0; i < (hash_map->capacity) *
            VECTOR_GROWTH_FACTOR; ++i) {
                vector_free(&hash_map->buckets[i]);
            }
            break;
    }

    free(hash_map->buckets);

    // assign the temp buckets array to the buckets array of the hash map
    hash_map->buckets = temp_buckets;

    return true;

}

/**
 * gets a vector and a key and checks if a pair with a key is in the vector
 * @param vector a bucket in hash map
 * @param key the key to find
 * @return the index of the pair with that key, -1 otherwise.
 */
int get_pair_by_key(vector* vector , const_keyT key){

    pair* cur_pair;
    for (int i = 0; i < vector->size ; ++i) {
        cur_pair = vector->data[i];
        if (cur_pair->key_cmp(cur_pair->key, key) == true){
            return i;
        }

    }

    return VACANT;

}
/**
 * Inserts a new in_pair to the hash map.
 * The function inserts *new*, *copied*, *dynamically allocated* in_pair,
 * NOT the in_pair it receives as a parameter.
 * @param hash_map the hash map to be inserted with new element.
 * @param in_pair a in_pair the hash map would contain.
 * @return returns 1 for successful insertion, 0 otherwise.
 */
int hashmap_insert (hashmap *hash_map, const pair *in_pair){

    if (hash_map == NULL || in_pair == NULL){
        return false;
    }
    // activate hash function on the pair.
    size_t hash_key = hash_map->hash_func(in_pair->key)  &
            (hash_map->capacity - 1);

    // get to the proper bucket in the vector.
    vector* cur_vector = hash_map->buckets[hash_key];

    if (get_pair_by_key(cur_vector,in_pair->key) == VACANT){

        // this means there is no value like this in the vector so we can
        // insert it.
        int value_addition = vector_push_back(cur_vector, in_pair);

        // check the insertion to the vector was ok
        if (!value_addition) {
            return false;
        }

        hash_map->size += 1;

        if (hashmap_get_load_factor(hash_map) > HASH_MAP_MAX_LOAD_FACTOR){

            // there are too many values in hashmap, so it needs to be resized.
            hash_map->capacity *= HASH_MAP_GROWTH_FACTOR;

            int is_success = assign_all_pairs(hash_map, INSERT);

            if (!is_success) {

                // the reassign of the pairs was unsuccessful so size and
                // capacity changes needs to be undone.
                hash_map->size -= 1;
                hash_map->capacity /= HASH_MAP_GROWTH_FACTOR;
                vector_erase(cur_vector, hash_map->size);
                return false;
            }

        }

        // finally, the new value is in the hash map.
        return true;

    }

    // this means that the value with the same key is already in hash map.
    return false;

}

/**
 * The function returns the value associated with the given key.
 * @param hash_map a hash map.
 * @param key the key to be checked.
 * @return the value associated with key if exists, NULL otherwise (the value itself,
 * not a copy of it).
 */
valueT hashmap_at (const hashmap *hash_map, const_keyT key){

    if (key == NULL || hash_map == NULL){
        return NULL;
    }

    // first get the hash code for the key and the vector in that index.
    size_t hash_key = hash_map->hash_func(key) & (hash_map->capacity - 1);

    const vector *cur_vector = hash_map->buckets[hash_key];

    for (int i = 0 ; i < cur_vector->size ; ++i){

        // get the current pair for every index
        pair *cur_Value = vector_at(cur_vector, i);

        // compare between the key given and the key in that pair, if they are
        // the same this is the value we need to return.
        if (cur_Value->key_cmp(key, cur_Value->key) == true){
            return cur_Value->value;
        }

    }

    // if the loop has ended it means there is no pair with that key in the cur
    // bucket, and that means pair with this key is not in hashmap.
    return NULL;
}



/**
 * The function erases the pair associated with key.
 * @param hash_map a hash map.
 * @param key a key of the pair to be erased.
 * @return 1 if the erasing was done successfully, 0 otherwise. (if key not in map,
 * considered fail).
 */
int hashmap_erase (hashmap *hash_map, const_keyT key){

    if (hash_map == NULL || key == NULL){
        return false;
    }

    // first we need to check if hash map contains a value with this key.
    valueT given_key_value = hashmap_at(hash_map, key);

    if (given_key_value == NULL){
        // there is nothing to delete
        return false;
    }

    int erase_succsses = false;

    pair* cur_pair = NULL;

    // get the hash key and the vector in that index
    size_t hash_value = hash_map->hash_func(key) & (hash_map->capacity - 1);

    vector* proper_vector = hash_map->buckets[hash_value];

    // find the pair in the vector
    for (int i = 0; i < proper_vector->size; ++i) {

        cur_pair = proper_vector->data[i];

        if (cur_pair->key_cmp(cur_pair->key, key)){
            erase_succsses = vector_erase(proper_vector, i);
            break;
        }

    }

    // iteration on the pairs in the vector has ended, check if a value was
    // deleted successfully
    if (!erase_succsses){
        return erase_succsses;
    }

    hash_map->size -= 1;

    // now check if a resizing of the hash map is required
    if (hashmap_get_load_factor(hash_map) < HASH_MAP_MIN_LOAD_FACTOR){

        hash_map->capacity /= HASH_MAP_GROWTH_FACTOR;

        int is_success = assign_all_pairs(hash_map, DELETE);

        if (!is_success){

            // the reassign wasn't successful, so everything must be back in
            // place
            hash_map->size += 1;
            hash_map->capacity *= HASH_MAP_GROWTH_FACTOR;
            vector_push_back(proper_vector, cur_pair);
            return false;

        }

        // if we got here then the re assign was successful and the
        // pair is of the hash map
        return true;

    }

    return true;
}

/**
 * This function returns the load factor of the hash map.
 * @param hash_map a hash map.
 * @return the hash map's load factor, -1 if the function failed.
 */
double hashmap_get_load_factor (const hashmap *hash_map){
    if (hash_map == NULL){
        return LOAD_FACTOR_ERR;
    }
    return (double) hash_map->size / (double) hash_map->capacity;

}



/**
 * This function receives a hashmap and 2 functions, the first checks a condition on the keys,
 * and the seconds apply some modification on the values. The function should apply the modification
 * only on the values that are associated with keys that meet the condition.
 *
 * Example: if the hashmap maps char->int, keyT_func checks if the char is a capital letter (A-Z),
 * and val_t_func multiples the number by 2, hashmap_apply_if will change the map:
 * {('C',2),('#',3),('X',5)}, to: {('C',4),('#',3),('X',10)}, and the return value will be 2.
 * @param hash_map a hashmap
 * @param keyT_func a function that checks a condition on keyT and return 1 if true, 0 else
 * @param valT_func a function that modifies valueT, in-place
 * @return number of changed values
 */
int hashmap_apply_if (const hashmap *hash_map, keyT_func keyT_func,
                      valueT_func valT_func){

    int changed_values = 0;

    for (int i = 0; i < hash_map->capacity ; ++i) {

        //get the current vector
        vector* cur_vector = hash_map->buckets[i];

        for (int j = 0; j < cur_vector->size; ++j) {

            //get the cur pair
            pair* cur_pair = vector_at(cur_vector, j);

            // check if the condition applies on the cur pair key
            if (keyT_func(cur_pair->key) == true){

                // the condition applies so activate the val func on the cur
                // pair value.
                valT_func(cur_pair->value);
                changed_values += 1;
            }

        }
    }

    return changed_values;
}
