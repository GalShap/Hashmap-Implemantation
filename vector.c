#include "vector.h"
#include <stdbool.h>
#define NOT_FOUND -1
#define VEC_ERR -1
/**
 * Dynamically allocates a new vector.
 * @param elem_copy_func func which copies the element stored in the vector (returns
 * dynamically allocated copy).
 * @param elem_cmp_func func which is used to compare elements stored in the vector.
 * @param elem_free_func func which frees elements stored in the vector.
 * @return pointer to dynamically allocated vector.
 * @if_fail return NULL.
 */
vector *vector_alloc(vector_elem_cpy elem_copy_func, vector_elem_cmp
    elem_cmp_func, vector_elem_free elem_free_func){

    // first allocate the memory for the vector and check it succeeded
    vector* new_vector = malloc(sizeof(vector));

    // check the allocation and funcs are legal.
    if (new_vector == NULL || elem_cmp_func == NULL || elem_copy_func == NULL
    || elem_free_func == NULL){
        return NULL;
    }

    // initialize the vector
    new_vector->size = 0;
    new_vector->capacity = VECTOR_INITIAL_CAP;
    new_vector->elem_free_func = elem_free_func;
    new_vector->elem_copy_func = elem_copy_func;
    new_vector->elem_cmp_func = elem_cmp_func;

    new_vector->data = calloc(new_vector->capacity, sizeof(void*));
    if (new_vector->data == NULL){
        free(new_vector);
        return NULL;
    }

    // initialization succeeded so we can return the vector created.
    return new_vector;
}

/**
 * Frees a vector and the elements the vector itself allocated.
 * @param p_vector pointer to dynamically allocated pointer to vector.
 */
void vector_free(vector **p_vector){

    vector* cur_vector = *p_vector;
    for (int i = 0; i < cur_vector->size; ++i) {

        // free the current element and secure it with Null
        cur_vector->elem_free_func(&cur_vector->data[i]);
        cur_vector->data[i] = NULL;
        cur_vector->size -= 1;
    }

    // now every element is freed so we can free the memory allocated when
    // creating the vector.
    free(cur_vector->data);
    cur_vector->data = NULL;

    free(cur_vector);
    cur_vector = NULL;
}

/**
 * Returns the element at the given index.
 * @param vector pointer to a vector.
 * @param ind the index of the element we want to get.
 * @return the element at the given index if exists (the element itself, not a copy of it),
 * NULL otherwise.
 */
void *vector_at(const vector *vector, size_t ind){

   if (ind > vector->size - 1 || vector->data[ind] == NULL){
        // it means we try to get to an index that doesn't hold anything
        // so it must be null.
        return NULL;
   }

  void* data_ptr = vector->data[ind];
  return data_ptr;
}

/**
 * Gets a value and checks if the value is in the vector.
 * @param vector a pointer to vector.
 * @param value the value to look for.
 * @return the index of the given value if it is in the vector ([0, vector_size - 1]).
 * Returns -1 if no such value in the vector.
 */
int vector_find(const vector *vector, const void *value){

    // iterate on every data and get the value from it.
    for (int i = 0 ; i < vector->size ; ++i){
        void* cur_value = vector_at(vector, i);
        if (cur_value == NULL){
            continue;
        }

        if (vector->elem_cmp_func(cur_value, value) == true){
            return i;
        }
    }

    // if all iterations are made and the value was not found then it's not
    // in vector.
    return NOT_FOUND;
}

/**
 * Adds a new value to the back (index vector_size) of the vector.
 * @param vector a pointer to vector.
 * @param value the value to be added to the vector.
 * @return 1 if the adding has been done successfully, 0 otherwise.
 */
int vector_push_back(vector *vector, const void *value) {

    // first we need to check it available for another element to get in the
    //vector.
    if (vector->size == vector->capacity){
        return false;
    }

    vector->data[vector->size] = vector->elem_copy_func(value);
    vector->size += 1;
    if (vector_get_load_factor(vector) > VECTOR_MAX_LOAD_FACTOR){

        //this means the vector needs to be resized
        vector->capacity *= VECTOR_GROWTH_FACTOR;
        vector->data = realloc( vector->data ,sizeof(value) * vector->capacity);
        if (vector->data == NULL){
            return false;
        }

    }

    return true;

}

/**
 * This function returns the load factor of the vector.
 * @param vector a vector.
 * @return the vector's load factor, -1 if the function failed.
 */
double vector_get_load_factor(const vector *vector){

    if (vector == NULL){
        return VEC_ERR;
    }

    return (double) vector->size / (double) vector->capacity;
}


/**
 * Removes the element at the given index from the vector. alters the indices of the remaining
 * elements so that there are no empty indices in the range [0, size-1] (inclusive).
 * @param vector a pointer to vector.
 * @param ind the index of the element to be removed.
 * @return 1 if the removing has been done successfully, 0 otherwise.
 */
int vector_erase(vector *vector, size_t ind){

    //first, check we actually get a valid vector ptr and that there is
    // an element in that index.
    if (vector == NULL || vector_at(vector, ind) == NULL){
        return false;
    }

    vector->elem_free_func(&vector->data[ind]);
    vector->data[ind] = NULL;


    for (int i = (int) ind + 1 ; i < vector->size ; ++i){
         vector->data[i-1] = vector->elem_copy_func(vector->data[i]);

    }

    vector->size -= 1;

    // this means that after the removal, we need to resize the vector
    if (vector_get_load_factor(vector) < VECTOR_MIN_LOAD_FACTOR){

        vector->capacity /= VECTOR_GROWTH_FACTOR;

        vector->data = realloc(vector->data, sizeof(void *) * vector->capacity);

        if (vector->data == NULL){
            return false;
        }
    }

    return true;

}

/**
 * Deletes all the elements in the vector.
 * @param vector vector a pointer to vector.
 */
void vector_clear(vector *vector){

    for (int i = 0 ; i < vector->size ; ++i){
        vector_erase(vector, i);
    }

}
