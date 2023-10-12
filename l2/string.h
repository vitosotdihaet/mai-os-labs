#ifndef STRING_H
#define STRING_H

#include <stdint.h>

typedef struct string {
    char *values;
    uint32_t last_element;
    uint32_t memory_size;
} string;


// Initialize string with default capacity
int string_init(string* s);

// Adds memory
int string_resize(string* s, uint32_t capacity);

// Adds a char to string, adding more memory if needed 
int string_push_char(string* s, char value);

// Pushes native C string to the custom string
int string_push_str(string* s, char values[]);

// Pushes whole s2 to s1
int string_push_string(string* s1, string s2);

// Copies s2 to s1 (taking minimal memory size) -> s1='aboba', s2='bobr' -> s1='bobr '
int string_copy(string* s1, string s2);

// Fully changes parameters of s1 to s2
int string_set(string* s1, string s2);

// Reads string char by char returning EOF if EOF
int string_read(string* s, FILE* stream);

// Returns index of char if present, else -1
int64_t string_index(string s1, char c);

// Returns 1 if char is present else 0
int string_contains(string s1, char c);

// Returns length of a string
uint32_t string_len(string s);

// Returns element at ith position if it exists, else EOF
int string_at(string s, uint32_t i);

#endif