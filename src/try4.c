#include <stdio.h>
#include <stdlib.h>

#include "try4c.h"

/* The maximum depth is possible up to 1000. */
TryBlock tryBlocks[1000];
int tryLevel = 0;

TryBlock* tryBlock = NULL;
TryExit tryExit = NULL;

void tryEnter() {
}

void tryLeave() {
}

TryBlock* tryBegin() {
    TryBlock* result = &tryBlocks[tryLevel++];
    result->break_ = 0;

    result->code = 0;
    result->throw_ = 0;

    result->catch_ = 0;
    result->finally_ = 0;

    return result;
}

int tryBreak() {
    if (tryBlock != NULL) {
        if (tryBlock->break_ == 0) {
            tryBlock->break_++;

            return 1;
        }
    }

    return 0;
}

void tryThrow(int code) {
    if (tryBlock != NULL) {
        if ((tryBlock->catch_ > 0) || (tryBlock->finally_ > 0)) {
            tryBlock = --tryLevel > 0 ? &tryBlocks[tryLevel - 1] : NULL;
        }
        if (tryBlock != NULL) {
            tryBlock->code = code;
            tryBlock->throw_++;

            longjmp(tryBlock->entryPoint, 2);
        }
    }

    /* Unhandled exception */
    if (tryExit != NULL) {
        tryExit(code);
    } else {
        exit(code);
    }
}

int tryCatch(int code) {
    if (tryBlock->catch_ == 0) {
        if ((code == tryBlock->code) && (tryBlock->throw_ > 0)) {
            tryBlock->catch_++;

            return 1;
        }
    }

    return 0;
}

int tryCatchAny() {
    return tryCatch(tryBlock->code);
}

int tryFinally() {
    if (tryBlock->finally_ == 0) {
        tryBlock->finally_++;

        return 1;
    }

    return 0;
}

void tryEnd() {
    TryBlock* tryBlockOld = tryBlock;
    tryBlock = --tryLevel > 0 ? &tryBlocks[tryLevel - 1] : NULL;
    if ((tryBlockOld->throw_ > 0) && (tryBlockOld->catch_ == 0)) {
        tryThrow(tryBlockOld->code);
    } else if (tryBlockOld->break_ > 0) {
        longjmp(tryBlockOld->breakPoint, 9);
    }
}

int tryCode() {
    return tryBlock != NULL ? tryBlock->code : -1;
}
