#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_DESC_LEN 99

typedef enum {
    STATUS_PENDING,
    STATUS_DONE
} TaskStatus;

typedef struct {
    int id;
    char description[MAX_DESC_LEN + 1];
    TaskStatus status;
} Task;

typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

// Function prototypes
Task* create_task_list(int initial_capacity, int *capacity, int *count);
int resize_task_list(Task **tasks, int *capacity);
int find_task_index(Task *tasks, int count, int id);
int add_task(Task **tasks, int *capacity, int *count, int id, const char *desc);
int delete_task(Task *tasks, int *count, int id);
int update_task(Task *tasks, int count, int id, const char *new_desc);
int complete_task(Task *tasks, int count, int id);
void view_tasks(const Task *tasks, int count);
void find_task(const Task *tasks, int count, int id);
void print_help();
void free_task_list(Task *tasks);
CommandType get_command_type(const char *commandStr);
void trim_newline(char *str);

// Case-insensitive string comparison helper
int string_equalsIgnoreCase(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            return 0;
        }
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

CommandType get_command_type(const char *commandStr) {
    if (string_equalsIgnoreCase(commandStr, "ADD")) return CMD_ADD;
    if (string_equalsIgnoreCase(commandStr, "DELETE")) return CMD_DELETE;
    if (string_equalsIgnoreCase(commandStr, "VIEW")) return CMD_VIEW;
    if (string_equalsIgnoreCase(commandStr, "UPDATE")) return CMD_UPDATE;
    if (string_equalsIgnoreCase(commandStr, "COMPLETE")) return CMD_COMPLETE;
    if (string_equalsIgnoreCase(commandStr, "FIND")) return CMD_FIND;
    if (string_equalsIgnoreCase(commandStr, "HELP")) return CMD_HELP;
    if (string_equalsIgnoreCase(commandStr, "EXIT")) return CMD_EXIT;
    return CMD_UNKNOWN;
}

// 1. Dynamic List Initialization
Task* create_task_list(int initial_capacity, int *capacity, int *count) {
    if (initial_capacity <= 0) {
        *capacity = 0;
        *count = 0;
        return NULL;
    }
    Task *tasks = (Task*) malloc(initial_capacity * sizeof(Task));
    if (tasks == NULL) {
        printf("[Error] Memory allocation failed!\n");
        *capacity = 0;
        *count = 0;
        return NULL;
    }
    *capacity = initial_capacity;
    *count = 0;
    return tasks;
}

// 3. Dynamic Resizing with Overflow Protection
int resize_task_list(Task **tasks, int *capacity) {
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

    Task *temp = (Task*) realloc(*tasks, new_capacity * sizeof(Task));
    if (temp == NULL) {
        printf("[Error] Reallocation failed! Out of memory.\n");
        return 0;
    }

    *tasks = temp;
    *capacity = new_capacity;
    return 1;
}

int find_task_index(Task *tasks, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (tasks[i].id == id) {
            return i;
        }
    }
    return -1;
}

// 3. Add Task
int add_task(Task **tasks, int *capacity, int *count, int id, const char *desc) {
    if (id <= 0) {
        printf("[Error] Task ID must be a positive integer.\n");
        return 0;
    }
    if (find_task_index(*tasks, *count, id) != -1) {
        printf("[Error] Task ID %d already exists. Duplicate IDs are rejected.\n", id);
        return 0;
    }
    if (strlen(desc) == 0) {
        printf("[Error] Task description cannot be empty.\n");
        return 0;
    }
    if (strlen(desc) > MAX_DESC_LEN) {
        printf("[Error] Description exceeds maximum length of %d characters.\n", MAX_DESC_LEN);
        return 0;
    }

    if (*count >= *capacity) {
        if (!resize_task_list(tasks, capacity)) {
            return 0;
        }
    }

    (*tasks)[*count].id = id;
    strncpy((*tasks)[*count].description, desc, MAX_DESC_LEN);
    (*tasks)[*count].description[MAX_DESC_LEN] = '\0';
    (*tasks)[*count].status = STATUS_PENDING;
    (*count)++;

    printf("[Success] Task %d added successfully.\n", id);
    return 1;
}

// 4. Delete Task (closing gaps)
int delete_task(Task *tasks, int *count, int id) {
    int idx = find_task_index(tasks, *count, id);
    if (idx == -1) {
        printf("[Error] Task ID %d not found for deletion.\n", id);
        return 0;
    }

    for (int i = idx; i < *count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    (*count)--;
    printf("[Success] Task %d deleted successfully.\n", id);
    return 1;
}

// 4. Update Task Description
int update_task(Task *tasks, int count, int id, const char *new_desc) {
    int idx = find_task_index(tasks, count, id);
    if (idx == -1) {
        printf("[Error] Task ID %d not found for update.\n", id);
        return 0;
    }
    if (strlen(new_desc) == 0 || strlen(new_desc) > MAX_DESC_LEN) {
        printf("[Error] Invalid description length.\n");
        return 0;
    }

    strncpy(tasks[idx].description, new_desc, MAX_DESC_LEN);
    tasks[idx].description[MAX_DESC_LEN] = '\0';
    printf("[Success] Task %d description updated successfully.\n", id);
    return 1;
}

// 4. Complete Task
int complete_task(Task *tasks, int count, int id) {
    int idx = find_task_index(tasks, count, id);
    if (idx == -1) {
        printf("[Error] Task ID %d not found.\n", id);
        return 0;
    }

    tasks[idx].status = STATUS_DONE;
    printf("[Success] Task %d marked as DONE.\n", id);
    return 1;
}

// 5. View All Tasks
void view_tasks(const Task *tasks, int count) {
    if (count == 0) {
        printf("[Info] The task list is currently empty.\n");
        return;
    }
    printf("\n--- Task List (%d total) ---\n", count);
    printf("%-8s | %-40s | %-10s\n", "ID", "Description", "Status");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-8d | %-40s | %-10s\n", 
               tasks[i].id, 
               tasks[i].description, 
               tasks[i].status == STATUS_DONE ? "DONE" : "PENDING");
    }
    printf("-------------------------------------------------------------\n");
}

// 5. Find Task by ID
void find_task(const Task *tasks, int count, int id) {
    int idx = find_task_index((Task*)tasks, count, id);
    if (idx == -1) {
        printf("[Error] Task ID %d not found.\n", id);
        return;
    }
    printf("\n[Found] Task Details:\n");
    printf("ID:          %d\n", tasks[idx].id);
    printf("Description: %s\n", tasks[idx].description);
    printf("Status:      %s\n", tasks[idx].status == STATUS_DONE ? "DONE" : "PENDING");
}

// 5. Help Command Guide
void print_help() {
    printf("\n=== Task Manager Command Reference ===\n");
    printf("  ADD      - Add a new task (prompts for ID and description)\n");
    printf("  DELETE   - Remove a task by its ID\n");
    printf("  VIEW     - List all stored tasks and their statuses\n");
    printf("  UPDATE   - Modify an existing task description by ID\n");
    printf("  COMPLETE - Mark a task as DONE by ID\n");
    printf("  FIND     - Display details for a specific task ID\n");
    printf("  HELP     - Display this command reference guide\n");
    printf("  EXIT     - Terminate the program and free memory\n");
    printf("--------------------------------------\n");
}

void free_task_list(Task *tasks) {
    if (tasks != NULL) {
        free(tasks);
    }
}

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

int main() {
    int initial_cap = 5;
    int capacity = 0;
    int count = 0;

    Task *task_list = create_task_list(initial_cap, &capacity, &count);
    if (!task_list) {
        printf("[Fatal] Initialization failed.\n");
        return 1;
    }

    char input_line[256];
    char cmd_str[64];

    printf("=== Interactive Task-List Manager ===\n");
    printf("Type 'HELP' for a list of valid commands.\n\n");

    while (1) {
        printf("TaskMgr> ");
        if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
            printf("\n[Info] EOF detected. Exiting program...\n");
            break;
        }

        trim_newline(input_line);
        if (strlen(input_line) == 0) {
            continue; // Ignore empty lines
        }

        // Extract command keyword
        int scanned = sscanf(input_line, "%s", cmd_str);
        if (scanned != 1) {
            continue;
        }

        CommandType cmd = get_command_type(cmd_str);

        int id;
        char desc[128];

        switch (cmd) {
            case CMD_ADD:
                printf("Enter Task ID (positive integer): ");
                if (scanf("%d", &id) != 1) {
                    printf("[Error] Invalid ID format.\n");
                    while (getchar() != '\n'); // clear buffer
                    break;
                }
                while (getchar() != '\n'); // clear buffer newline

                printf("Enter Description (up to %d chars): ", MAX_DESC_LEN);
                if (fgets(desc, sizeof(desc), stdin) != NULL) {
                    trim_newline(desc);
                    add_task(&task_list, &capacity, &count, id, desc);
                }
                break;

            case CMD_DELETE:
                printf("Enter Task ID to delete: ");
                if (scanf("%d", &id) != 1) {
                    printf("[Error] Invalid ID format.\n");
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n');
                delete_task(task_list, &count, id);
                break;

            case CMD_VIEW:
                view_tasks(task_list, count);
                break;

            case CMD_UPDATE:
                printf("Enter Task ID to update: ");
                if (scanf("%d", &id) != 1) {
                    printf("[Error] Invalid ID format.\n");
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n');

                printf("Enter New Description: ");
                if (fgets(desc, sizeof(desc), stdin) != NULL) {
                    trim_newline(desc);
                    update_task(task_list, count, id, desc);
                }
                break;

            case CMD_COMPLETE:
                printf("Enter Task ID to mark complete: ");
                if (scanf("%d", &id) != 1) {
                    printf("[Error] Invalid ID format.\n");
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n');
                complete_task(task_list, count, id);
                break;

            case CMD_FIND:
                printf("Enter Task ID to find: ");
                if (scanf("%d", &id) != 1) {
                    printf("[Error] Invalid ID format.\n");
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n');
                find_task(task_list, count, id);
                break;

            case CMD_HELP:
                print_help();
                break;

            case CMD_EXIT:
                printf("Exiting Task Manager and freeing allocated memory.\n");
                free_task_list(task_list);
                return 0;

            case CMD_UNKNOWN:
            default:
                printf("[Error] Unknown command \"%s\". Type 'HELP' for valid commands.\n", cmd_str);
        }
    }

    free_task_list(task_list);
    return 0;
}