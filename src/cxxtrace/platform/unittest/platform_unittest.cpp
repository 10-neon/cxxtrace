#include "platform.h"

#include <gtest/gtest.h>

namespace neon {
namespace platform {
namespace test {

TEST(PlatformTest, PlatformName) {
    const char* platform_name = "";
#if defined(PLATFORM_LINUX)
    platform_name = "Linux";
#elif defined(PLATFORM_WIN)
    platform_name = "Windows";
#elif defined(PLATFORM_MAC)
    platform_name = "Mac";
#elif defined(PLATFORM_ANDROID)
    platform_name = "Android";
#elif defined(PLATFORM_IOS)
    platform_name = "iOS";
#endif
    EXPECT_EQ(name(), platform_name);
}

}  // namespace test
}  // namespace platform
}  // namespace neon
