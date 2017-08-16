//gtest example
#include "fermat/common/clock.h"
#include "gtest/gtest.h"

class FermatClockTest : public testing::Test {
protected:
    static void SetUpTestCase() {
        //do prepare work before test 
    }

    static void TearDownTestCase() {
        //clean work after test
    }
};

TEST_F(FermatClockTest, test_clock_class) {
    fermat::Clock clock1;
    fermat::Clock clock2;

    int64_t ret = clock1.raw() - clock2.raw();
    EXPECT_EQ(0, ret);
}

int main(int argc, char *argv[]) {
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
