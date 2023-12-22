#ifndef __TRY4C_H
#define __TRY4C_H

#include <setjmp.h>

typedef struct TryBlock {
    int return_;

    int code;
    int throw_;

    int catch_;
    int finally_;

    jmp_buf entryPoint;
    int entryState;

    jmp_buf returnPoint;
} TryBlock;

typedef void (*TryExit)(int code);

extern TryBlock* tryBlock;
extern TryExit tryExit;

TryBlock* tryBegin();
int tryReturn();
void tryThrow(int code);
int tryCatch(int code);
int tryCatchAny();
int tryFinally();
void tryEnd();

int tryCode();

#define __TRY \
    do { \
        tryBlock = tryBegin(); \
        tryBlock->entryState = setjmp(tryBlock->entryPoint); \
        if (tryBlock->entryState == 0) {

#define __RETURN(value) \
            do { \
                if (tryReturn()) { \
                    if (setjmp(tryBlock->returnPoint) == 0) { \
                        longjmp(tryBlock->entryPoint, 9); \
                    } else { \
                        return value; \
                    } \
                } \
            } while (0)

#define __THROW(code) \
            tryThrow(code)

#define __CATCH(code) \
        } else if (tryCatch(code)) {

#define __CATCH_ANY \
        } else if (tryCatchAny()) {

#define __FINALLY \
        } \
\
        if (tryFinally()) {

#define __TRY_END \
        } \
\
        tryEnd(); \
    } while (0)

#define __TRY_CODE tryCode()

#endif
