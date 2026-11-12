#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define MAX_STR_LEN 19  // 19 chars + 1 null terminator = 20 bytes

typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

// Tagged Union to ensure safe access to active members
typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT,
    VALUE_EMPTY
} ValueType;

typedef struct {
    UnionData data;
    ValueType type;
} TaggedRecord;

// Function prototypes for safe tagged record operations
void set_int_record(TaggedRecord *record, int val);
void set_float_record(TaggedRecord *record, float val);
int set_text_record(TaggedRecord *record, const char *text);
void display_tagged_record(const TaggedRecord *record, int index);
void print_memory_analysis();

// Safe mutators
void set_int_record(TaggedRecord *record, int val) {
    record->data.i = val;
    record->type = VALUE_INT;
}

void set_float_record(TaggedRecord *record, float val) {
    record->data.f = val;
    record->type = VALUE_FLOAT;
}

int set_text_record(TaggedRecord *record, const char *text) {
    if (strlen(text) > MAX_STR_LEN) {
        printf("[Error] String exceeds maximum allowed length of %d characters.\n", MAX_STR_LEN);
        return 0;
    }
    strncpy(record->data.str, text, sizeof(record->data.str));
    record->data.str[sizeof(record->data.str) - 1] = '\0'; // Ensure null-termination
    record->type = VALUE_TEXT;
    return 1;
}

// Safe display function based strictly on the tag
void display_tagged_record(const TaggedRecord *record, int index) {
    printf("Record %d -> ", index);
    switch (record->type) {
        case VALUE_INT:
            printf("[Type: INT] Value: %d\n", record->data.льнай ? record->data.i : record->data.i); // standard access
            break;
        case VALUE_FLOAT:
            printf("[Type: FLOAT] Value: %.2f\n", record->data.f);
            break;
        case VALUE_TEXT:
            printf("[Type: TEXT] Value: \"%s\"\n", record->data.str);
            break;
        case VALUE_EMPTY:
        default:
            printf("[Type: EMPTY] No valid data set.\n");
            break;
    }
}

// Memory Analysis and Demonstration
void print_memory_analysis() {
    printf("=== Part 1: Sizes, Alignments, and Offsets ===\n");
    printf("Size of UnionData:     %zu bytes\n", sizeof(UnionData));
    printf("Size of StructData:    %zu bytes\n", sizeof(StructData));
    printf("Size of TaggedRecord:  %zu bytes (Union + Tag enum)\n\n", sizeof(TaggedRecord));

    printf("Array Mass Comparison:\n");
    printf("100 StructData records: %zu bytes\n", 100 * sizeof(StructData));
    printf("100 UnionData records:  %zu bytes\n", 100 * sizeof(UnionData));
    printf("100 TaggedRecord records: %zu bytes\n\n", 100 * sizeof(TaggedRecord));

    StructData sdata;
    UnionData udata;

    printf("=== Part 2: Member Addresses & Struct Offsets ===\n");
    printf("Struct member addresses & offsets (using offsetof):\n");
    printf("  sdata.i   -> Address: %p | Offset: %zu bytes\n", (void*)&sdata.i, offsetof(StructData, i));
    printf("  sdata.f   -> Address: %p | Offset: %zu bytes\n", (void*)&sdata.f, offsetof(StructData, f));
    printf("  sdata.str -> Address: %p | Offset: %zu bytes\n\n", (void*)&sdata.str, offsetof(StructData, str));

    printf("Union member addresses (all share the base address):\n");
    printf("  udata.i   -> Address: %p\n", (void*)&udata.i);
    printf("  udata.f   -> Address: %p\n", (void*)&udata.f);
    printf("  udata.str -> Address: %p\n\n", (void*)&udata.str);
}

int main() {
    print_memory_analysis();

    printf("=== Part 3: Tagged Record System Demo (5 Mixed Records) ===\n");
    TaggedRecord records[5];

    // Initialize 5 mixed records
    set_int_record(&records[0], 10);
    set_float_record(&records[1], 220.5f);
    set_text_record(&records[2], "Union Programming");
    set_int_record(&records[3], 999);
    set_text_record(&records[4], "Max Len 19 Chrs!!"); // Exactly 17 chars

    for (int i = 0; i < 5; i++) {
        display_tagged_record(&records[i], i + 1);
    }

    printf("\n=== Part 4: Edge-Case Testing ===\n");
    
    // Edge case 1: Overlength string rejection
    printf("Testing overlength string (>19 chars):\n");
    if (!set_text_record(&records[0], "This string is definitely way too long for the buffer!")) {
        printf("[Handled] Overlength string successfully rejected.\n");
    }

    // Edge case 2: Dynamic type switching on a single record
    printf("\nSwitching Record #1 types dynamically:\n");
    set_int_record(&records[0], 42);
    display_tagged_record(&records[0], 1);

    set_text_record(&records[0], "Switched to Text");
    display_tagged_record(&records[0], 1);

    set_float_record(&records[0], 3.14159f);
    display_tagged_record(&records[0], 1);

    return 0;
}