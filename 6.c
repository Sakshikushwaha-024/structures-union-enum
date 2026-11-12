#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdalign.h>

// 1. Basic Union and Structure Definitions
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

// 2. Tagged Record Definitions for Safe Access
typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT,
    VALUE_INVALID = -1
} ValueType;

typedef struct {
    UnionData data;
    ValueType type;
} TaggedRecord;

// Validated setters
void set_int(TaggedRecord *rec, int val) {
    if (!rec) return;
    rec->data.i = val;
    rec->type = VALUE_INT;
}

void set_float(TaggedRecord *rec, float val) {
    if (!rec) return;
    rec->data.f = val;
    rec->type = VALUE_FLOAT;
}

int set_text(TaggedRecord *rec, const char *val) {
    if (!rec || !val) return -1;
    size_t len = strlen(val);
    if (len >= sizeof(rec->data.str)) {
        printf("  [Edge Case Warning] String rejected: length %zu exceeds buffer limit (%zu max including null terminator).\n", 
               len, sizeof(rec->data.str));
        return -1;
    }
    // Safe copy into the union array
    strcpy(rec->data.str, val);
    rec->type = VALUE_TEXT;
    return 0;
}

// Safe display function based strictly on the tag
void print_record(const TaggedRecord *rec) {
    if (!rec) return;
    switch (rec->type) {
        case VALUE_INT:
            printf("  -> [Active Tag: INT] Value: %d\n", rec->data.i);
            break;
        case VALUE_FLOAT:
            printf("  -> [Active Tag: FLOAT] Value: %.2f\n", rec->data.f);
            break;
        case VALUE_TEXT:
            printf("  -> [Active Tag: TEXT] Value: \"%s\"\n", rec->data.str);
            break;
        default:
            printf("  -> [Error] Invalid or corrupted tag detected (%d). Access blocked.\n", (int)rec->type);
            break;
    }
}

int main() {
    printf("==================================================\n");
    printf(" PART 1: Sizes, Alignments, and Memory Addresses\n");
    printf("==================================================\n");
    
    UnionData udata;
    StructData sdata;

    printf("Size of UnionData : %zu bytes (Alignment: %zu bytes)\n", sizeof(UnionData), _Alignof(UnionData));
    printf("Size of StructData: %zu bytes (Alignment: %zu bytes)\n\n", sizeof(StructData), _Alignof(StructData));

    printf("Union Member Addresses (All share base address):\n");
    printf("  udata.i   : %p\n", (void *)&udata.i);
    printf("  udata.f   : %p\n", (void *)&udata.f);
    printf("  udata.str : %p\n\n", (void *)&udata.str);

    printf("Structure Member Addresses & Offsets (Using offsetof):\n");
    printf("  sdata.i   : %p | offset: %2zu bytes\n", (void *)&sdata.i, offsetof(StructData, i));
    printf("  sdata.f   : %p | offset: %2zu bytes\n", (void *)&sdata.f, offsetof(StructData, f));
    printf("  sdata.str : %p | offset: %2zu bytes\n\n", (void *)&sdata.str, offsetof(StructData, str));

    printf("==================================================\n");
    printf(" PART 2: Memory Footprint Comparison (100 Records)\n");
    printf("==================================================\n");
    printf("  100 x StructData = %zu bytes\n", sizeof(StructData) * 100);
    printf("  100 x UnionData  = %zu bytes\n\n", sizeof(UnionData) * 100);

    printf("==================================================\n");
    printf(" PART 3: Safe Tagged Records & Edge-Case Testing\n");
    printf("==================================================\n");

    // 1. Store five tagged records of mixed types
    TaggedRecord records[5];
    set_int(&records[0], 10);
    set_float(&records[1], 220.5f);
    set_text(&records[2], "Union Programming");
    set_int(&records[3], 42);
    set_text(&records[4], "Safe Records");

    printf("Displaying 5 Initial Mixed Tagged Records:\n");
    for (int i = 0; i < 5; i++) {
        printf("Record %d: ", i + 1);
        print_record(&records[i]);
    }
    printf("\n");

    // Edge Case 1: Maximum-length valid string (19 characters + 1 null terminator)
    printf("[Edge Case Test 1] Setting record 1 to a 19-character string:\n");
    char max_valid_str[20] = "1234567890123456789";
    set_text(&records[0], max_valid_str);
    printf("  Assigned \"%s\" (strlen: %zu)\n", max_valid_str, strlen(max_valid_str));
    printf("Record 1: "); print_record(&records[0]);
    printf("\n");

    // Edge Case 2: Overlength string rejection
    printf("[Edge Case Test 2] Attempting to set an overlength string (25 chars):\n");
    set_text(&records[0], "ThisStringIsWayTooLongForBuffer");
    printf("Record 1 remains intact: "); print_record(&records[0]);
    printf("\n");

    // Edge Case 3: Dynamic type switching on a single record
    printf("[Edge Case Test 3] Switching a single record from INT -> TEXT -> FLOAT:\n");
    TaggedRecord dyn_rec;
    set_int(&dyn_rec, 500);
    printf("  Step 1 (Int): "); print_record(&dyn_rec);
    
    set_text(&dyn_rec, "Dynamic Text");
    printf("  Step 2 (Text): "); print_record(&dyn_rec);
    
    set_float(&dyn_rec, 99.99f);
    printf("  Step 3 (Float): "); print_record(&dyn_rec);
    printf("\n");

    // Edge Case 4: Invalid/corrupted tag handling
    printf("[Edge Case Test 4] Handling invalid/corrupted tag state:\n");
    TaggedRecord bad_rec;
    bad_rec.data.i = 1337;
    bad_rec.type = (ValueType)99; // Corrupted tag
    print_record(&bad_rec);
    printf("\n");

    printf("==================================================\n");
    printf(" PART 4: Tagged Record Size Analysis\n");
    printf("==================================================\n");
    printf("Size of TaggedRecord: %zu bytes\n", sizeof(TaggedRecord));
    printf("(UnionData size + enum tag size, adjusted for platform struct padding alignment)\n");

    return 0;
}