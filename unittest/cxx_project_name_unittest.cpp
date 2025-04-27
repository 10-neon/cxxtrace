#include <gtest/gtest.h>

TEST(ProjectName, TestCase1) {
    std::vector<int> nums = {1, 2, 3, 4, 5};
    int sum = 0;
    for (int num : nums) {
        sum += num;
    }
    EXPECT_EQ(sum, 15);
}
