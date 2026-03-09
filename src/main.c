#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define RESET "\x1B[0m"

typedef struct Node
{
    WIN32_FIND_DATA data;
    struct Node* next;
} Node;

void free_list(Node* head)
{
    while (head)
    {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

void list_dir(const char* base, const char* prefix, int* file_count, int* dir_count) {
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", base);

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(search_path, &ffd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }

    Node *head = NULL, *tail = NULL;

    do
    {
        if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0)
        {
            continue;
        }

        Node* new_node = (Node*)malloc(sizeof(Node));

        if (!new_node)
        {
            break;
        }

        new_node->data = ffd;
        new_node->next = NULL;

        if (!head)
        {
            head = tail = new_node;
        }
        else
        { 
            tail->next = new_node;
            tail = new_node;
        }
    }
    while (FindNextFile(hFind, &ffd));
    
    FindClose(hFind);

    Node* current = head;

    while (current)
    {
        int is_last = (current->next == NULL);
        
        printf("%s%s " GREEN "%s" RESET "\n", prefix, is_last ? "└──" : "├──", current->data.cFileName);

        if (current->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            int needed = snprintf(NULL, 0, "%s%s   ", prefix, is_last ? " " : "│") + 1;
            char* next_prefix = malloc(needed);

            if (next_prefix)
            {
                snprintf(next_prefix, needed, "%s%s   ", prefix, is_last ? " " : "│");

                char next_path[MAX_PATH];

                snprintf(next_path, MAX_PATH, "%s\\%s", base, current->data.cFileName);

                (*dir_count)++;
                list_dir(next_path, next_prefix, file_count, dir_count);
                free(next_prefix);
            }
            else
            {
                fprintf(stderr, "Error: Memory exhausted at " YELLOW "%s" RESET ". Skipping directory contents.\n", current->data.cFileName);
            }
        }
        else
        {
            (*file_count)++;
        }

        current = current->next;
    }

    free_list(head);
}

int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    const char* path = (argc > 1) ? argv[1] : ".";

    char full[MAX_PATH];
    DWORD result = GetFullPathNameA(path, MAX_PATH, full, NULL);

    if (result == 0)
    {
        fprintf(stderr, "Error: GetFullPathNameA failed with code " YELLOW "%lu" RESET "\n", GetLastError());
        return EXIT_FAILURE;
    }
    else if (result >= MAX_PATH)
    {
        fprintf(stderr, "Error: The full path is longer than the buffer size.\n");
        return EXIT_FAILURE;
    }

    char root[MAX_PATH];
    char type[32];

    if (GetVolumePathNameA(full, root, MAX_PATH))
    {
        DWORD serial = 0;

        printf("Drive: %s\n", root);

        if (GetVolumeInformationA(root, NULL, 0, &serial, NULL, NULL, type, sizeof(type)))
        {
            printf("Volume Serial Number: " YELLOW "%04X-%04X" RESET "\n", (UINT)(serial >> 16), (UINT)(serial & 0xFFFF));
            printf("File System Type: " GREEN "%s" RESET "\n\n", type);
        }
    }

    // Count of Files and Dirs
	int files = 0;
    int dirs = 0;

	printf(GREEN "%s" RESET "\n", full);

	list_dir(full, "", &files, &dirs);

	printf("\n" YELLOW "%d" RESET " directories, " YELLOW "%d" RESET " files\n", dirs, files);
	fflush(stdout);

    return EXIT_SUCCESS;
}
