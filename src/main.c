#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "try4c.h"

void print(const char* format, ...) {
    va_list list;

    va_start(list, format);
    vprintf(format, list);
    fflush(stdout);
    va_end(list);
}

void testDummy() {
}

int testCase1() {
    __TRY {
        testDummy();
    } __TRY_END;

    /* Do not use __RETURN outside of a try block. */
    return 0;
}

int testCase2() {
    __TRY {
        testDummy();

        __RETURN(1);
    } __FINALLY {
        __RETURN(2);
    } __TRY_END;

    return 0;
}

int testCase3() {
    __TRY {
        testDummy();
    } __FINALLY {
        __RETURN(1);
    } __TRY_END;

    return 0;
}

int testCase4() {
    __TRY {
        testDummy();

        __THROW(1);

        __RETURN(1);
    } __CATCH(1) {
        __RETURN(2);
    } __FINALLY {
        __RETURN(3);
    } __TRY_END;

    return 0;
}

int testCase5() {
    __TRY {
        __TRY {
            __TRY {
                testDummy();

                __THROW(1);
            } __TRY_END;

            __RETURN(1);
        } __TRY_END;

        __RETURN(2);
    } __CATCH_ANY {
        __RETURN(3);
    } __TRY_END;

    return 0;
}

int testCase6() {
    __TRY {
        __TRY {
            testDummy();

            __THROW(1);
        } __CATCH_ANY {
            testDummy();
        } __TRY_END;

        __RETURN(1);
    } __FINALLY {
        __RETURN(2);
    } __TRY_END;

    return 0;
}

int testCase7() {
    int result = 1;

    __TRY {
        __TRY {
            __TRY {
                testDummy();

                __THROW(1);
            } __FINALLY {
                result++;
            } __TRY_END;

            __RETURN(-1);
        } __FINALLY {
            result++;
        } __TRY_END;

        __RETURN(-2);
    } __CATCH_ANY {
        result++;
    } __FINALLY {
        __RETURN(result++);
    } __TRY_END;

    return 0;
}

void testCase8_1() {
    __TRY {
        __TRY {
            __THROW(5);
        } __TRY_END;
    } __TRY_END;
}

int testCase8() {
    __TRY {
        __TRY {
            testCase8_1();
        } __TRY_END;
    } __CATCH_ANY {
        int result = __TRY_CODE;

        __RETURN(result);
        /* __RETURN(__TRY_CODE) : The value changes because tryCode is executed at the return point. */
    } __TRY_END;

    return 0;
}

int testCase9() {
    int result = 0;

    int index;
    for (index = 0; index < 100; index++) {
        __TRY {
            if (index >= 10) {
                __BREAK;
            }

            if ((index % 2) == 0) {
                __CONTINUE;
            }

            result++;
        } __FINALLY {
            result++;
        } __TRY_END;
    }

    return result;
}

typedef int (*TestTest)();

void testTest(const char* name, const TestTest test, int result) {
    print("[begin (%s)]\n", name);

    int result_ = test();
    int flag = result_ == result;
    print("%s %s\n", name, flag ? "SUCCEED" : "FAIL");

    if (!flag) {
        print("  result (%d / %d)\n", result_, result);
        exit(1);
    }

    print("[end]\n \n");
}

int main() {
    __TRY {
        testTest("case1", &testCase1, 0);
        testTest("case2", &testCase2, 1);
        testTest("case3", &testCase3, 1);
        testTest("case4", &testCase4, 2);
        testTest("case5", &testCase5, 3);
        testTest("case6", &testCase6, 1);
        testTest("case7", &testCase7, 4);
        testTest("case8", &testCase8, 5);
        testTest("case9", &testCase9, 16);

        print("test SUCCEED all\n");
    } __CATCH_ANY {
        print("main.catch: %d\n", __TRY_CODE);
    } __TRY_END;

    return 0;
}
