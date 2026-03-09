#include <ctype.h>
#include <stdio.h>
#include <windows.h>

#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define RESET "\x1B[0m"

void list_dir(const char* base, const char* prefix, int* file_count, int* dir_count)
{
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", base);

    WIN32_FIND_DATA entries;
    HANDLE hFind = FindFirstFile(search_path, &entries);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }

    int total = 0;

    do
    {
        if (strcmp(entries.cFileName, ".") != 0 && strcmp(entries.cFileName, "..") != 0)
        {
            total++;
        }
    }
    while (FindNextFile(hFind, &entries));
    
    FindClose(hFind);

    hFind = FindFirstFile(search_path, &entries);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }
    
    int current = 0;

    do
    {
        if (strcmp(entries.cFileName, ".") == 0 || strcmp(entries.cFileName, "..") == 0)
        {
            continue;
        }

        current++;
        int is_last = (current == total);

        printf("%s%s " GREEN "%s" RESET "\n", prefix, is_last ? "└──" : "├──", entries.cFileName);

        if (entries.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            (*dir_count)++;
            char new_prefix[MAX_PATH];

            snprintf(new_prefix, MAX_PATH, "%s%s   ", prefix, is_last ? " " : "│");
            
            char next_path[MAX_PATH];

            snprintf(next_path, MAX_PATH, "%s\\%s", base, entries.cFileName);
            list_dir(next_path, new_prefix, file_count, dir_count);
        }
        else
        {
            (*file_count)++;
        }
    }
    while (FindNextFile(hFind, &entries));

    FindClose(hFind);
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
        fprintf(stderr, "Error: GetFullPathNameA failed with code %lu\n", GetLastError());
        return EXIT_FAILURE;
    }
    else if (result > MAX_PATH)
    {
        fprintf(stderr, "Error: The full path is longer than the buffer size.\n");
        return EXIT_FAILURE;
    }

    char root[MAX_PATH];
    char type[MAX_PATH];

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