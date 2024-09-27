#ifndef __TRY4C_H
#define __TRY4C_H

#include <setjmp.h>

/* TODO : Let's hide type */
typedef struct TryBlock {
    struct TryBlock* left;

    int level;

    int break_;

    int code;
    int throw_;

    int catch_;
    int finally_;

    jmp_buf entryPoint;
    int entryState; /* 2: throw, 7: break, 8: continue, 9: return */

    jmp_buf breakPoint;
} TryBlock;

typedef void (*TryExit)(int code);

extern TryExit tryExit;

void tryEnter();
void tryLeave();

void tryBegin();
void tryEnd();

int tryBreak();
void tryThrow(int code);
int tryCatch(int code);
int tryCatchAny();
int tryFinally();

TryBlock* tryBlock();
int tryCode();

#define __TRY \
    tryEnter(); \
    { \
        tryBegin(); \
        tryBlock()->entryState = setjmp(tryBlock()->entryPoint); \
        if (tryBlock()->entryState == 0) {

#define __BREAK \
            if (tryBreak()) { \
                if (setjmp(tryBlock()->breakPoint) == 0) { \
                    longjmp(tryBlock()->entryPoint, 7); \
                } \
            } \
            break

#define __CONTINUE \
            if (tryBreak()) { \
                if (setjmp(tryBlock()->breakPoint) == 0) { \
                    longjmp(tryBlock()->entryPoint, 8); \
                } \
            } \
            continue

#define __RETURN(value) \
            do { \
                if (tryBreak()) { \
                    if (setjmp(tryBlock()->breakPoint) == 0) { \
                        longjmp(tryBlock()->entryPoint, 9); \
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
        if (tryFinally()) {

#define __TRY_END \
        } \
        tryEnd(); \
    } \
    tryLeave()

#define __TRY_CODE tryCode()

#endif
