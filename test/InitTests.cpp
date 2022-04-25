#include "gtest/gtest.h"
#include <stdlib.h>

static const unsigned int SEED = 13;

TEST(InitTests, SetRandomSeed)
{
    srand(SEED);
    SUCCEED();
}
