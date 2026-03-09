#include <ctype.h>
#include <direct.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define RESET "\x1B[0m"

void list_dir(const char* base_path, const char* prefix, int is_last, int* file_count, int* dir_count)
{
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", base_path);

    WIN32_FIND_DATA entries[1024];
    int count = 0;
    HANDLE hFind = FindFirstFile(search_path, &entries[count]);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (strcmp(entries[count].cFileName, ".") != 0 && strcmp(entries[count].cFileName, "..") != 0)
            {
                count++;
            }
        }
        while (FindNextFile(hFind, &entries[count]) && count < 1024);

        FindClose(hFind);
    }

    for (int i = 0; i < count; i++)
    {
        int last_child = (i == count - 1);
        char current_path[MAX_PATH];
        snprintf(current_path, MAX_PATH, "%s\\%s", base_path, entries[i].cFileName);

        printf("%s%s " GREEN "%s" RESET "\n", prefix, last_child ? "└──" : "├──", entries[i].cFileName);

        if (entries[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            (*dir_count)++;
            char new_prefix[MAX_PATH];
            snprintf(new_prefix, MAX_PATH, "%s%s   ", prefix, last_child ? " " : "│");
            
            list_dir(current_path, new_prefix, last_child, file_count, dir_count);
        }
        else
        {
            (*file_count)++;
        }
    }
}

int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    const char* path = (argc > 1) ? argv[1] : ".";
	int files = 0;
    int dirs = 0;

	printf(GREEN "%s" RESET "\n", path);
	list_dir(path, "", 1, &files, &dirs);
	printf("\n" YELLOW "%d" RESET " directories, " YELLOW "%d" RESET " files\n", dirs, files);
	fflush(stdout);

    return EXIT_SUCCESS;
}