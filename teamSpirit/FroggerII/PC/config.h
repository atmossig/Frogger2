#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

//

typedef struct TAG_CONFIG
{
	char *name;
	char *type;
	void *var;
} CONFIG;

extern void GetArgs(char *arglist);

//

#ifdef __cplusplus
}
#endif

#endif