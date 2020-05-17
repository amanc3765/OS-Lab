
/*******************************************************************
                    SIMPLE FILE SYSTEM
********************************************************************/

#ifndef SIMPLEFS_OPS
#define SIMPLEFS_OPS

static int simplefs_search(char *filename);

int  simplefs_create (char *filename);
void simplefs_delete (char *filename); 

int  simplefs_open  (char *filename);
void simplefs_close (int file_handle);

int simplefs_read  (int file_handle, char *buf, int nbytes);
int simplefs_write (int file_handle, char *buf, int nbytes);

int simplefs_seek  (int file_handle, int nseek);

#endif

