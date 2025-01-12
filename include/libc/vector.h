#include <types.h>

#define VECTOR_DEFAULT_CAPACITY 3;

#define vector_create(type) \
    typedef struct { \
        u32_ct size; \
        u32_ct capacity; \
        type* _back_ptr; \
        \
        bool_ct (*is_empty)(const struct _vector_##type_t*); \
        u32_ct (*size)(const struct _vector_##type_t*); \
        const type (*get)(const struct _vector_##type_t*, u32_ct index); \
        void (*push_back)(struct _vector_##type_t*, type); \
    } _vector_##type_t; \
    \
    bool_ct _vector_is_empty_##type(const _vector_##type_t* vector_ptr) \
    { \
        return vector_ptr->_size == 0; \
    } \
    \
    u32_ct _vector_size_##type(const _vector_##type_t* vector_ptr) \
    { \
        return vector_ptr->_size; \
    } \
    \
    const type _vector_get_##type(const _vector_##type_t* vector_ptr, u32_ct index) \
    { \
        return vector_ptr[index]; \
    } \
    \
    void _vector_push_back_##type(const _vector_##type_t* vector_ptr, type element) \
    { \
        if (vector_ptr->size >= vector_ptr->capacity) { \
            \
        } \
        vector_ptr[vector_ptr->size] = element; \
        vector_ptr->size++; \
    } \
    \
    _list_functions_##type _list_funcs_##type = { \
        &list_is_empty_##type, \
        &list_size_##type, \
        &list_front_##type, \
        &list_push_front_##type, \
    }; \
    \
    List_##type* new_list_##type() \
    { \
        List_##type* res = (List_##type*) malloc(sizeof(List_##type)); \
        res->_size = 0; \
        res->_first = NULL; \
        res->_functions = &_list_funcs_##type; \
        return res; \
    }