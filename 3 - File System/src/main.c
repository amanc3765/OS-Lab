#include "simplefs-ops.h"
#include "simplefs-disk.h"

extern struct filehandle_t file_handle_array[MAX_OPEN_FILES];

void print_FT();

int main()
{

    simplefs_formatDisk();
    simplefs_dump();

    int ch;

    do
    {
        printf(
            "\n************ Simple File System ************\n"
            "\n 1. Create                                    "
            "\n 2. Delete                                    "
            "\n 3. Open                                      "
            "\n 4. Close                                     "
            "\n 5. Read                                      "
            "\n 6. Write                                     "
            "\n 7. Seek                                      ");

        printf("\n\nEnter your choice: ");
        scanf("%d", &ch);

        char filename[MAX_NAME_STRLEN], buf[MAX_FILE_SIZE * BLOCKSIZE + 1];
        char c;
        int ret, filehandle, inputLimit, nbytes;

        switch (ch)
        {

        case 1: /************************* Create *************************/

            printf("\nEnter file name: ");
            scanf("%s", filename);

            ret = simplefs_create(filename);

            if (ret == 0)
                printf("\nCreate successful.\n");
            else
                printf("\nCreate failed.\n");

            break;

        case 2: /************************* Delete *************************/

            printf("\nEnter file name: ");
            scanf("%s", filename);

            simplefs_delete(filename);

            break;

        case 3: /************************** Open **************************/

            printf("\nEnter file name: ");
            scanf("%s", filename);

            ret = simplefs_open(filename);
            if (ret == -1)
                printf("\nOpen failed.\n");
            else
                printf("\nOpen successful...File Handle[%d] [ Inode: %d, Offset: %d ]\n",
                       ret, file_handle_array[ret].inode_number, file_handle_array[ret].offset);

            break;

        case 4: /************************** Close **************************/

            printf("\nEnter file handle: ");
            scanf("%d", &filehandle);

            simplefs_close(filehandle);
            printf("\nClose successful.\n");

            break;

        case 5: /************************** Read **************************/

            printf("\nEnter file handle: ");
            scanf("%d", &filehandle);

            printf("\nEnter amount of data to be read ");
            scanf("%d", &nbytes);

            ret = simplefs_read(filehandle, buf, nbytes);
            if (ret == -1)
                printf("\nRead failed.\n");
            else
            {
                printf("\n%s\n", buf);
                printf("\nRead successful.\n");
            }

            break;

        case 6: /************************** Write **************************/

            printf("\nEnter file handle: ");
            scanf("%d", &filehandle);

            getchar();
            inputLimit = MAX_FILE_SIZE * BLOCKSIZE + 1;
            printf("\nEnter data to be written: ");
            fgets(buf, inputLimit, stdin);

            ret = simplefs_write(filehandle, buf, strlen(buf));
            if (ret == -1)
                printf("\nWrite failed.\n");
            else
                printf("\nWrite successful.\n");

            break;

        case 7: /************************** Seek **************************/

            printf("\nEnter file handle: ");
            scanf("%d", &filehandle);

            printf("\nEnter amount of seek: ");
            scanf("%d", &nbytes);

            ret = simplefs_seek(filehandle, nbytes);
            if (ret == -1)
                printf("\nSeek failed.\n");
            else
                printf("\nSeek successful.\n");

            break;

        default:
            ch = 0;
            break;
        }

        simplefs_dump();
        print_FT();
    } while (ch);
}

void print_FT()
{
    line for (int i = 0; i < NUM_INODES; ++i)
        printf("[%d]\t", i);
    line for (int i = 0; i < NUM_INODES; ++i)
        printf("%d\t", file_handle_array[i].inode_number);
    line for (int i = 0; i < NUM_INODES; ++i)
        printf("%d\t", file_handle_array[i].offset);
    line
}
