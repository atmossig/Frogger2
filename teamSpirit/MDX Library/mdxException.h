#ifndef MDXEXCEPTION_H_INCLUDED
#define MDXEXCEPTION_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

int Eval_Exception (int n_except,LPEXCEPTION_POINTERS exceptinf);

#define EXCEPTION_HANDLE(x) {__try {x} __except (Eval_Exception(GetExceptionCode(),GetExceptionInformation())){}};

#ifdef __cplusplus
}
#endif

#endif