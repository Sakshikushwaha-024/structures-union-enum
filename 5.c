#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TOLERANCE 0.0001f

typedef struct {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

// Function prototypes
Rectangle* create_rectangle_array(int initial_capacity, int *capacity, int *count);
int resize_rectangle_array(Rectangle **rectangles, int *capacity);
int find_rectangle_index(Rectangle *rectangles, int count, int id);
int add_rectangle(Rectangle **rectangles, int *capacity, int *count, int id, float width, float height);
void display_rectangles(Rectangle *rectangles, int count);
int search_rectangle(Rectangle *rectangles, int count, int id);
int update_rectangle(Rectangle *rectangles, int count, int id, float width, float height);
int delete_rectangle(Rectangle *rectangles, int *count, int id);
void display_geometry_analytics(Rectangle *rectangles, int count);
void display_sorted_rectangles(Rectangle *rectangles, int count);
void display_filtered_rectangles(Rectangle *rectangles, int count, float threshold);
void free_rectangle_array(Rectangle *rectangles);
void clear_input_buffer();
int compare_rectangles_area(const void *a, const void *b);

// 1. Dynamic Allocation & Initialization
Rectangle* create_rectangle_array(int initial_capacity, int *capacity, int *count) {
    if (initial_capacity <= 0 || initial_capacity > 1000) {
        *capacity = 0;
        *count = 0;
        return NULL;
    }
    Rectangle *rects = (Rectangle*) malloc(initial_capacity * sizeof(Rectangle));
    if (rects == NULL) {
        printf("[Error] Memory allocation failed!\n");
        *capacity = 0;
        *count = 0;
        return NULL;
    }
    *capacity = initial_capacity;
    *count = 0;
    return rects;
}

// 4 & 8. Dynamic Resizing with Overflow Protection
int resize_rectangle_array(Rectangle **rectangles, int *capacity) {
    int new_capacity;
    if (*capacity == 0) {
        new_capacity = 4;
    } else {
        if (*capacity > __INT_MAX__ / 2) {
            printf("[Error] Capacity limit reached. Cannot expand further.\n");
            return 0;
        }
        new_capacity = (*capacity) * 2;
    }

    Rectangle *temp = (Rectangle*) realloc(*rectangles, new_capacity * sizeof(Rectangle));
    if (temp == NULL) {
        printf("[Error] Reallocation failed! Out of memory.\n");
        return 0;
    }

    *rectangles = temp;
    *capacity = new_capacity;
    return 1;
}

int find_rectangle_index(Rectangle *rectangles, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (rectangles[i].id == id) {
            return i;
        }
    }
    return -1;
}

// 2 & 3. Add Rectangle with Validation & Geometry Computations
int add_rectangle(Rectangle **rectangles, int *capacity, int *count, int id, float width, float height) {
    if (id <= 0) {
        printf("[Error] Rectangle ID must be a positive integer.\n");
        return 0;
    }
    if (find_rectangle_index(*rectangles, *count, id) != -1) {
        printf("[Error] Rectangle ID %d already exists. Duplicate IDs are rejected.\n", id);
        return 0;
    }
    if (width <= 0.0f || height <= 0.0f || !isfinite(width) || !isfinite(height)) {
        printf("[Error] Dimensions must be finite positive values (no zeros, negatives, or NaNs).\n");
        return 0;
    }

    if (*count >= *capacity) {
        if (!resize_rectangle_array(rectangles, capacity)) {
            return 0;
        }
    }

    (*rectangles)[*count].id = id;
    (*rectangles)[*count].width = width;
    (*rectangles)[*count].height = height;
    (*rectangles)[*count].area = width * height;
    (*rectangles)[*count].perimeter = 2.0f * (width + height);
    (*count)++;

    printf("[Success] Rectangle added successfully!\n");
    return 1;
}

// 4. Display All Rectangles with Square Classification
void display_rectangles(Rectangle *rectangles, int count) {
    if (count == 0) {
        printf("[Info] The rectangle collection is currently empty.\n");
        return;
    }
    printf("\n--- Rectangle Inventory List (%d total) ---\n", count);
    printf("%-8s | %-10s | %-10s | %-10s | %-10s | %-12s\n", "ID", "Width", "Height", "Area", "Perimeter", "Type");
    printf("-----------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        int is_square = (fabsf(rectangles[i].width - rectangles[i].height) < TOLERANCE);
        printf("%-8d | %-10.2f | %-10.2f | %-10.2f | %-10.2f | %-12s\n", 
               rectangles[i].id, rectangles[i].width, rectangles[i].height, 
               rectangles[i].area, rectangles[i].perimeter, 
               is_square ? "Square" : "Non-Square");
    }
    printf("-----------------------------------------------------------------------------------\n");
}

// 4. Search by ID
int search_rectangle(Rectangle *rectangles, int count, int id) {
    int idx = find_rectangle_index(rectangles, count, id);
    if (idx == -1) {
        printf("[Error] Rectangle with ID %d not found.\n", id);
        return -1;
    }
    int is_square = (fabsf(rectangles[idx].width - rectangles[idx].height) < TOLERANCE);
    printf("\n[Found] Rectangle Details:\n");
    printf("ID: %d\nWidth: %.2f\nHeight: %.2f\nArea: %.2f\nPerimeter: %.2f\nClassification: %s\n", 
           rectangles[idx].id, rectangles[idx].width, rectangles[idx].height, 
           rectangles[idx].area, rectangles[idx].perimeter, is_square ? "Square" : "Non-Square");
    return idx;
}

// 4. Update Dimensions by ID (recomputes area & perimeter)
int update_rectangle(Rectangle *rectangles, int count, int id, float width, float height) {
    int idx = find_rectangle_index(rectangles, count, id);
    if (idx == -1) {
        printf("[Error] Rectangle with ID %d not found for update.\n", id);
        return 0;
    }
    if (width <= 0.0f || height <= 0.0f || !isfinite(width) || !isfinite(height)) {
        printf("[Error] Dimensions must be finite positive values.\n");
        return 0;
    }

    rectangles[idx].width = width;
    rectangles[idx].height = height;
    rectangles[idx].area = width * height;
    rectangles[idx].perimeter = 2.0f * (width + height);

    printf("[Success] Rectangle dimensions and geometry updated successfully!\n");
    return 1;
}

// 4. Delete by ID (preserves sequence order)
int delete_rectangle(Rectangle *rectangles, int *count, int id) {
    int idx = find_rectangle_index(rectangles, *count, id);
    if (idx == -1) {
        printf("[Error] Rectangle with ID %d not found for deletion.\n", id);
        return 0;
    }

    for (int i = idx; i < *count - 1; i++) {
        rectangles[i] = rectangles[i + 1];
    }
    (*count)--;
    printf("[Success] Rectangle deleted successfully!\n");
    return 1;
}

// 5. Geometry Analytics (Largest, Smallest, Total, Average Area)
void display_geometry_analytics(Rectangle *rectangles, int count) {
    if (count == 0) {
        printf("[Info] Collection is empty. No geometry analytics available.\n");
        return;
    }

    int max_idx = 0;
    int min_idx = 0;
    float total_area = 0.0f;

    for (int i = 0; i < count; i++) {
        total_area += rectangles[i].area;
        if (rectangles[i].area > rectangles[max_idx].area) {
            max_idx = i;
        }
        if (rectangles[i].area < rectangles[min_idx].area) {
            min_idx = i;
        }
    }

    float avg_area = total_area / count;

    printf("\n--- Geometry Analytics Summary ---\n");
    printf("Total Rectangles: %d\n", count);
    printf("Total Area:       %.2f\n", total_area);
    printf("Average Area:     %.2f\n", avg_area);
    printf("Largest Area:     %.2f (ID: %d, %.2f x %.2f)\n", 
           rectangles[max_idx].area, rectangles[max_idx].id, 
           rectangles[max_idx].width, rectangles[max_idx].height);
    printf("Smallest Area:    %.2f (ID: %d, %.2f x %.2f)\n", 
           rectangles[min_idx].area, rectangles[min_idx].id, 
           rectangles[min_idx].width, rectangles[min_idx].height);
    printf("----------------------------------\n");
}

int compare_rectangles_area(const void *a, const void *b) {
    const Rectangle *r1 = (const Rectangle *)a;
    const Rectangle *r2 = (const Rectangle *)b;

    if (r1->area < r2->area) return -1;
    if (r1->area > r2->area) return 1;

    // Tie-breaker: Ascending ID
    if (r1->id < r2->id) return -1;
    if (r1->id > r2->id) return 1;
    return 0;
}

// 7. Sorted View (Ascending area, tie-breaker by ID, using a copy)
void display_sorted_rectangles(Rectangle *rectangles, int count) {
    if (count == 0) {
        printf("[Info] The rectangle collection is currently empty.\n");
        return;
    }

    Rectangle *copy = (Rectangle*) malloc(count * sizeof(Rectangle));
    if (!copy) {
        printf("[Error] Memory allocation for sorting failed.\n");
        return;
    }
    memcpy(copy, rectangles, count * sizeof(Rectangle));

    qsort(copy, count, sizeof(Rectangle), compare_rectangles_area);

    printf("\n--- Rectangles Sorted by Area (Ascending) ---\n");
    printf("%-8s | %-10s | %-10s | %-10s | %-12s\n", "ID", "Width", "Height", "Area", "Type");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        int is_square = (fabsf(copy[i].width - copy[i].height) < TOLERANCE);
        printf("%-8d | %-10.2f | %-10.2f | %-10.2f | %-12s\n", 
               copy[i].id, copy[i].width, copy[i].height, copy[i].area, 
               is_square ? "Square" : "Non-Square");
    }
    printf("--------------------------------------------------------------------\n");

    free(copy);
}

// 7. Filter Rectangles exceeding a threshold
void display_filtered_rectangles(Rectangle *rectangles, int count, float threshold) {
    if (count == 0) {
        printf("[Info] The rectangle collection is currently empty.\n");
        return;
    }

    int matches = 0;
    printf("\n--- Rectangles with Area Exceeding %.2f ---\n", threshold);
    printf("%-8s | %-10s | %-10s | %-10s\n", "ID", "Width", "Height", "Area");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        if (rectangles[i].area > threshold) {
            printf("%-8d | %-10.2f | %-10.2f | %-10.2f\n", 
                   rectangles[i].id, rectangles[i].width, rectangles[i].height, rectangles[i].area);
            matches++;
        }
    }
    printf("--------------------------------------------------\n");
    if (matches == 0) {
        printf("[Info] No rectangles found with area greater than %.2f.\n", threshold);
    }
}

void free_rectangle_array(Rectangle *rectangles) {
    if (rectangles != NULL) {
        free(rectangles);
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

    printf("=== Rectangle Inventory and Geometry Analysis System ===\n");
    printf("Enter initial capacity (1-1000): ");
    if (scanf("%d", &initial_cap) != 1 || initial_cap <= 0 || initial_cap > 1000) {
        printf("[Warning] Invalid capacity entered. Defaulting initial capacity to 5.\n");
        initial_cap = 5;
    }
    clear_input_buffer();

    Rectangle *rectangles = create_rectangle_array(initial_cap, &capacity, &count);
    if (rectangles == NULL && capacity > 0) {
        printf("[Fatal] Could not initialize rectangle array. Exiting.\n");
        return 1;
    }

    // Pre-load default demo records (201: 5.0x4.0, 202: 6.5x3.5, 203: 4.0x4.0) if capacity >= 3
    if (capacity >= 3) {
        add_rectangle(&rectangles, &capacity, &count, 201, 5.0f, 4.0f);
        add_rectangle(&rectangles, &capacity, &count, 202, 6.5f, 3.5f);
        add_rectangle(&rectangles, &capacity, &count, 203, 4.0f, 4.0f);
    }

    int choice;
    do {
        printf("\n--- MENU ---\n");
        printf("1. Add Rectangle\n");
        printf("2. Display All Rectangles (with Square Classification)\n");
        printf("3. Search Rectangle by ID\n");
        printf("4. Update Rectangle Dimensions by ID\n");
        printf("5. Delete Rectangle by ID\n");
        printf("6. Geometry Analytics Summary\n");
        printf("7. Display Rectangles Sorted by Area\n");
        printf("8. Filter Rectangles by Area Threshold\n");
        printf("9. Exit\n");
        printf("Enter your choice (1-9): ");

        if (scanf("%d", &choice) != 1) {
            printf("[Error] Invalid input format. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        int id;
        float w, h, threshold;

        switch (choice) {
            case 1:
                printf("Enter Rectangle ID (positive integer): ");
                scanf("%d", &id);
                clear_input_buffer();
                printf("Enter Width: ");
                scanf("%f", &w);
                clear_input_buffer();
                printf("Enter Height: ");
                scanf("%f", &h);
                clear_input_buffer();

                add_rectangle(&rectangles, &capacity, &count, id, w, h);
                break;

            case 2:
                display_rectangles(rectangles, count);
                break;

            case 3:
                printf("Enter Rectangle ID to search: ");
                scanf("%d", &id);
                clear_input_buffer();
                search_rectangle(rectangles, count, id);
                break;

            case 4:
                printf("Enter Rectangle ID to update: ");
                scanf("%d", &id);
                clear_input_buffer();
                printf("Enter New Width: ");
                scanf("%f", &w);
                clear_input_buffer();
                printf("Enter New Height: ");
                scanf("%f", &h);
                clear_input_buffer();

                update_rectangle(rectangles, count, id, w, h);
                break;

            case 5:
                printf("Enter Rectangle ID to delete: ");
                scanf("%d", &id);
                clear_input_buffer();
                delete_rectangle(rectangles, &count, id);
                break;

            case 6:
                display_geometry_analytics(rectangles, count);
                break;

            case 7:
                display_sorted_rectangles(rectangles, count);
                break;

            case 8:
                printf("Enter minimum area threshold: ");
                scanf("%f", &threshold);
                clear_input_buffer();
                display_filtered_rectangles(rectangles, count, threshold);
                break;

            case 9:
                printf("Exiting program and releasing allocated memory...\n");
                break;

            default:
                printf("[Error] Invalid choice. Please select between 1 and 9.\n");
        }
    } while (choice != 9);

    free_rectangle_array(rectangles);
    return 0;
}