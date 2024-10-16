#include "acutest.h"

void sample_test() {
    TEST_CHECK(1 == 1);
}

TEST_LIST = {
    {"sample_test", sample_test},
    {NULL, NULL}
};