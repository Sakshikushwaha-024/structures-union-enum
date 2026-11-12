#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 49

typedef struct {
    int roll_no;
    char name[MAX_NAME_LEN + 1];
    int age;
    float gpa;
} Student;

// Function prototypes
Student* create_student_list(int initial_capacity, int *capacity, int *count);
int resize_student_list(Student **studs, int *capacity);
int add_student(Student **studs, int *capacity, int *count, int roll_no, const char *name, int age, float gpa);
void display_students(Student *students, int count);
int search_student(Student *students, int count, int roll_no);
int update_student(Student *students, int count, int roll_no, const char *name, int age, float gpa);
int delete_student(Student *students, int *count, int roll_no);
void display_analytics(Student *students, int count);
void display_sorted_students(Student *students, int count);
void free_student_list(Student *students);
void clear_input_buffer();
int compare_students_gpa(const void *a, const void *b);

// 1. Dynamic Allocation & Initialization
Student* create_student_list(int initial_capacity, int *capacity, int *count) {
    if (initial_capacity <= 0) {
        *capacity = 0;
        *count = 0;
        return NULL;
    }
    Student *studs = (Student*) malloc(initial_capacity * sizeof(Student));
    if (studs == NULL) {
        printf("[Error] Memory allocation failed!\n");
        *capacity = 0;
        *count = 0;
        return NULL;
    }
    *capacity = initial_capacity;
    *count = 0;
    return studs;
}

// 4. Dynamic Resizing with Overflow Protection
int resize_student_list(Student **studs, int *capacity) {
    int new_capacity;
    if (*capacity == 0) {
        new_capacity = 4;
    } else {
        // Prevent arithmetic overflow
        if (*capacity > __INT_MAX__ / 2) {
            printf("[Error] Capacity limit reached. Cannot expand further.\n");
            return 0;
        }
        new_capacity = (*capacity) * 2;
    }

    Student *temp = (Student*) realloc(*studs, new_capacity * sizeof(Student));
    if (temp == NULL) {
        printf("[Error] Reallocation failed! Out of memory.\n");
        return 0;
    }

    *studs = temp;
    *capacity = new_capacity;
    return 1;
}

// Check for duplicate roll number
int find_roll_index(Student *students, int count, int roll_no) {
    for (int i = 0; i < count; i++) {
        if (students[i].roll_no == roll_no) {
            return i;
        }
    }
    return -1;
}

// 2 & 3. Add Student with Validation
int add_student(Student **studs, int *capacity, int *count, int roll_no, const char *name, int age, float gpa) {
    if (roll_no <= 0) {
        printf("[Error] Roll number must be a positive integer.\n");
        return 0;
    }
    if (find_roll_index(*studs, *count, roll_no) != -1) {
        printf("[Error] Roll number %d already exists. Duplicate roll numbers are rejected.\n", roll_no);
        return 0;
    }
    if (strlen(name) > MAX_NAME_LEN) {
        printf("[Error] Name exceeds maximum limit of %d characters.\n", MAX_NAME_LEN);
        return 0;
    }
    if (age < 16 || age > 100) {
        printf("[Error] Age must be between 16 and 100.\n");
        return 0;
    }
    if (gpa < 0.00f || gpa > 4.00f) {
        printf("[Error] GPA must be between 0.00 and 4.00.\n");
        return 0;
    }

    // Expand capacity if full
    if (*count >= *capacity) {
        if (!resize_student_list(studs, capacity)) {
            return 0;
        }
    }

    // Insert record
    (*studs)[*count].roll_no = roll_no;
    strncpy((*studs)[*count].name, name, MAX_NAME_LEN);
    (*studs)[*count].name[MAX_NAME_LEN] = '\0';
    (*studs)[*count].age = age;
    (*studs)[*count].gpa = gpa;
    (*count)++;

    printf("[Success] Student added successfully!\n");
    return 1;
}

// Display all records
void display_students(Student *students, int count) {
    if (count == 0) {
        printf("[Info] The student collection is currently empty.\n");
        return;
    }
    printf("\n--- Student Records List (%d total) ---\n", count);
    printf("%-10s | %-30s | %-5s | %-6s\n", "Roll No", "Name", "Age", "GPA");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-10d | %-30s | %-5d | %-6.2f\n", 
               students[i].roll_no, students[i].name, students[i].age, students[i].gpa);
    }
    printf("--------------------------------------------------------\n");
}

// 3. Search Student by Roll Number
int search_student(Student *students, int count, int roll_no) {
    int idx = find_roll_index(students, count, roll_no);
    if (idx == -1) {
        printf("[Error] Student with roll number %d not found.\n", roll_no);
        return -1;
    }
    printf("\n[Found] Student Details:\n");
    printf("Roll No: %d\nName: %s\nAge: %d\nGPA: %.2f\n", 
           students[idx].roll_no, students[idx].name, students[idx].age, students[idx].gpa);
    return idx;
}

// 3. Update Student Details
int update_student(Student *students, int count, int roll_no, const char *name, int age, float gpa) {
    int idx = find_roll_index(students, count, roll_no);
    if (idx == -1) {
        printf("[Error] Student with roll number %d not found for update.\n", roll_no);
        return 0;
    }
    if (strlen(name) > MAX_NAME_LEN) {
        printf("[Error] Name exceeds maximum limit of %d characters.\n", MAX_NAME_LEN);
        return 0;
    }
    if (age < 16 || age > 100) {
        printf("[Error] Age must be between 16 and 100.\n");
        return 0;
    }
    if (gpa < 0.00f || gpa > 4.00f) {
        printf("[Error] GPA must be between 0.00 and 4.00.\n");
        return 0;
    }

    strncpy(students[idx].name, name, MAX_NAME_LEN);
    students[idx].name[MAX_NAME_LEN] = '\0';
    students[idx].age = age;
    students[idx].gpa = gpa;

    printf("[Success] Student record updated successfully!\n");
    return 1;
}

// 3. Delete Student (Preserves order of remaining records)
int delete_student(Student *students, int *count, int roll_no) {
    int idx = find_roll_index(students, *count, roll_no);
    if (idx == -1) {
        printf("[Error] Student with roll number %d not found for deletion.\n", roll_no);
        return 0;
    }

    // Shift elements left to preserve order
    for (int i = idx; i < *count - 1; i++) {
        students[i] = students[i + 1];
    }
    (*count)--;
    printf("[Success] Student record deleted successfully!\n");
    return 1;
}

// 5. Academic Analytics (Highest, Lowest, Average GPA)
void display_analytics(Student *students, int count) {
    if (count == 0) {
        printf("[Info] Collection is empty. No analytics available.\n");
        return;
    }

    int highest_idx = 0;
    int lowest_idx = 0;
    float sum_gpa = 0.0f;

    for (int i = 0; i < count; i++) {
        sum_gpa += students[i].gpa;
        if (students[i].gpa > students[highest_idx].gpa) {
            highest_idx = i;
        }
        if (students[i].gpa < students[lowest_idx].gpa) {
            lowest_idx = i;
        }
    }

    float avg_gpa = sum_gpa / count;

    printf("\n--- Academic Analytics Summary ---\n");
    printf("Total Records: %d\n", count);
    printf("Average GPA:   %.2f\n", avg_gpa);
    printf("Highest GPA:   %.2f (Roll No: %d, Name: %s)\n", 
           students[highest_idx].gpa, students[highest_idx].roll_no, students[highest_idx].name);
    printf("Lowest GPA:    %.2f (Roll No: %d, Name: %s)\n", 
           students[lowest_idx].gpa, students[lowest_idx].roll_no, students[lowest_idx].name);
    printf("----------------------------------\n");
}

// Comparison function for qsort (Descending GPA, tie-breaker: Ascending Roll No)
int compare_students_gpa(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;

    if (s2->gpa > s1->gpa) return 1;
    if (s2->gpa < s1->gpa) return -1;

    // Tie-breaker: Ascending roll number
    if (s1->roll_no < s2->roll_no) return -1;
    if (s1->roll_no > s2->roll_no) return 1;
    return 0;
}

// 6. Sorted View (Documentation choice: works on a temporary copy to preserve original storage order)
void display_sorted_students(Student *students, int count) {
    if (count == 0) {
        printf("[Info] The student collection is currently empty.\n");
        return;
    }

    // Allocate temporary copy for sorting
    Student *copy = (Student*) malloc(count * sizeof(Student));
    if (!copy) {
        printf("[Error] Memory allocation for sorting failed.\n");
        return;
    }
    memcpy(copy, students, count * sizeof(Student));

    qsort(copy, count, sizeof(Student), compare_students_gpa);

    printf("\n--- Students Sorted by GPA (Descending) ---\n");
    printf("(Note: Sorted view generated on a temporary copy; original insertion order preserved)\n");
    printf("%-10s | %-30s | %-5s | %-6s\n", "Roll No", "Name", "Age", "GPA");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-10d | %-30s | %-5d | %-6.2f\n", 
               copy[i].roll_no, copy[i].name, copy[i].age, copy[i].gpa);
    }
    printf("--------------------------------------------------------\n");

    free(copy);
}

// Free allocated memory
void free_student_list(Student *students) {
    if (students != NULL) {
        free(students);
    }
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int initial_cap = 0;
    int capacity = 0;
    int count = 0;

    printf("=== Student Records and Academic Analytics System ===\n");
    printf("Enter initial capacity for student records: ");
    if (scanf("%d", &initial_cap) != 1 || initial_cap <= 0) {
        printf("[Warning] Invalid or zero capacity entered. Defaulting initial capacity to 3.\n");
        initial_cap = 3;
    }
    clear_input_buffer();

    Student *student_list = create_student_list(initial_cap, &capacity, &count);
    if (student_list == NULL && capacity > 0) {
        printf("[Fatal] Could not initialize student list. Exiting.\n");
        return 1;
    }

    // Pre-load default demo records (Aladdin, Batman, Catwoman) if capacity >= 3
    if (capacity >= 3) {
        add_student(&student_list, &capacity, &count, 101, "Aladdin", 20, 3.5f);
        add_student(&student_list, &capacity, &count, 102, "Batman", 21, 3.7f);
        add_student(&student_list, &capacity, &count, 103, "Catwoman", 22, 3.8f);
    }

    int choice;
    do {
        printf("\n--- MENU ---\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student by Roll Number\n");
        printf("4. Update Student Details\n");
        printf("5. Delete Student by Roll Number\n");
        printf("6. Academic Analytics Summary\n");
        printf("7. Display Students Sorted by GPA\n");
        printf("8. Exit\n");
        printf("Enter your choice (1-8): ");

        if (scanf("%d", &choice) != 1) {
            printf("[Error] Invalid input format. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        int roll;
        char name[100];
        int age;
        float gpa;

       switch (choice) {
            case 1:
                printf("Enter Roll Number (positive integer): ");
                scanf("%d", &roll);
                clear_input_buffer();
                printf("Enter Name (up to %d chars): ", MAX_NAME_LEN);
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';

                printf("Enter Age (16-100): ");
                scanf("%d", &age);
                clear_input_buffer();

                printf("Enter GPA (0.00-4.00): ");
                scanf("%f", &gpa);
                clear_input_buffer();

                add_student(&student_list, &capacity, &count, roll, name, age, gpa);
                break;

            case 2:
                display_students(student_list, count);
                break;

            case 3:
                printf("Enter Roll Number to search: ");
                scanf("%d", &roll);
                clear_input_buffer();
                search_student(student_list, count, roll);
                break;

            case 4:
                printf("Enter Roll Number of student to update: ");
                scanf("%d", &roll);
                clear_input_buffer();
                printf("Enter New Name (up to %d chars): ", MAX_NAME_LEN);
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';

                printf("Enter New Age (16-100): ");
                scanf("%d", &age);
                clear_input_buffer();

                printf("Enter New GPA (0.00-4.00): ");
                scanf("%f", &gpa);
                clear_input_buffer();

                update_student(student_list, count, roll, name, age, gpa);
                break;

            case 5:
                printf("Enter Roll Number to delete: ");
                scanf("%d", &roll);
                clear_input_buffer();
                delete_student(student_list, &count, roll);
                break;

            case 6:
                display_analytics(student_list, count);
                break;

            case 7:
                display_sorted_students(student_list, count);
                break;

            case 8:
                printf("Exiting program and freeing allocated memory...\n");
                break;

            default:
                printf("[Error] Invalid choice. Please select between 1 and 8.\n");
        }
    } while (choice != 8);

    free_student_list(student_list);
    return 0;
}