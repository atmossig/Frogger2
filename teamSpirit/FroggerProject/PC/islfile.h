#ifndef ISLFILE_H
#define ISLFILE_H

#ifdef __cplusplus
extern "C" {
#endif

void fileInitialise(char *fileSystem);
unsigned char *fileLoad(char *fName, int *length);

#ifdef __cplusplus
}
#endif
#endif

