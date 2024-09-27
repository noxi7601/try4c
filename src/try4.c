#include <stdio.h>
#include <stdlib.h>

#include "try4c.h"

static TryBlock* tryBlockLast = NULL;

TryExit tryExit = NULL;

void tryEnter() {
}

void tryLeave() {
}

void tryBegin() {
    TryBlock* tryBlock = malloc(sizeof(TryBlock));
    tryBlock->left = tryBlockLast;
    if (tryBlockLast != NULL) {
        tryBlock->level = tryBlockLast->level + 1;
    } else {
        tryBlock->level = 0;
    }
    tryBlockLast = tryBlock;

    tryBlock->break_ = 0;

    tryBlock->code = 0;
    tryBlock->throw_ = 0;

    tryBlock->catch_ = 0;
    tryBlock->finally_ = 0;
}

void tryEnd() {
    if (tryBlockLast != NULL) {
        TryBlock* tryBlockOld = tryBlockLast;
        tryBlockLast = tryBlockLast->left;

        if ((tryBlockOld->throw_ > 0) && (tryBlockOld->catch_ == 0)) {
            tryThrow(tryBlockOld->code);
        } else if (tryBlockOld->break_ > 0) {
            longjmp(tryBlockOld->breakPoint, 9);
        }
    }
}

int tryBreak() {
    if (tryBlockLast != NULL) {
        if (tryBlockLast->break_ == 0) {
            tryBlockLast->break_++;

            return 1;
        }
    }

    return 0;
}

void tryThrow(int code) {
    if (tryBlockLast != NULL) {
        if ((tryBlockLast->catch_ > 0) || (tryBlockLast->finally_ > 0)) {
            tryBlockLast = tryBlockLast->left;
        }

        if (tryBlockLast != NULL) {
            tryBlockLast->code = code;
            tryBlockLast->throw_++;

            longjmp(tryBlockLast->entryPoint, 2);
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
    if (tryBlockLast != NULL) {
        if (tryBlockLast->catch_ == 0) {
            if ((code == tryBlockLast->code) && (tryBlockLast->throw_ > 0)) {
                tryBlockLast->catch_++;

                return 1;
            }
        }
    }

    return 0;
}

int tryCatchAny() {
    if (tryBlockLast != NULL) {
        return tryCatch(tryBlockLast->code);
    }

    return 0;
}

int tryFinally() {
    if (tryBlockLast != NULL) {
        if (tryBlockLast->finally_ == 0) {
            tryBlockLast->finally_++;

            return 1;
        }
    }

    return 0;
}

TryBlock* tryBlock() {
    return tryBlockLast;
}

int tryCode() {
    return tryBlockLast != NULL ? tryBlockLast->code : -1;
}
