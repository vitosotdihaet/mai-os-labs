#ifndef STRING_H
#define STRING_H

#include <stdint.h>

typedef struct string {
    char *values;
    uint64_t last_element;
    uint64_t memory_size;
} string;


// Initialize string with default capacity
int init_string(string* s);

// Adds memory
int string_resize(string* s, uint64_t capacity);

// Adds a char to string, adding more memory if needed 
int push_char(string* s, char value);

// Pushes whole s2 to s1
int push_string(string* s1, string s2);

// Pushes native C string, adding more memory if needed
int push_chars(string* s, char values[]);

// Copies s2 to s1 (taking minimal memory size) -> s1='aboba', s2='bobr' -> s1='bobr '
int copy_string(string* s1, string s2);

// Fully changes parameters of s1 to s2
int set_string(string* s1, string s2);

// Reads string char by char returning -1 if EOF
int read_string(string* s);

// Returns index of char if present, else -1
uint64_t index_string(string s1, char c);

// Returns 1 if char is present else 0
int string_contains(string s1, char c);

// Returns length of a string
uint64_t string_len(string s);

#endif