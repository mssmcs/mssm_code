#include "sizebound.h"

#include <gtest/gtest.h>

#include <vector>

TEST(DistributeSizes, MinimumFitExactSpan)
{
    std::vector<SizeBound> bounds{{10, 100}, {20, 100}, {30, 100}};
    auto sizes = distributeSizes(bounds, 60, false, -1);
    ASSERT_EQ(sizes.size(), 3u);
    EXPECT_EQ(sizes[0], 10);
    EXPECT_EQ(sizes[1], 20);
    EXPECT_EQ(sizes[2], 30);
}

TEST(DistributeSizes, OverflowReturnsMinimums)
{
    std::vector<SizeBound> bounds{{10, 100}, {20, 100}};
    auto sizes = distributeSizes(bounds, 15, false, -1);
    ASSERT_EQ(sizes.size(), 2u);
    EXPECT_EQ(sizes[0], 10);
    EXPECT_EQ(sizes[1], 20);
}

TEST(DistributeSizes, GrowIdxReceivesExtraSpace)
{
    std::vector<SizeBound> bounds{{10, 50}, {10, 100}, {10, 100}};
    auto sizes = distributeSizes(bounds, 80, false, 1);
    ASSERT_EQ(sizes.size(), 3u);
    EXPECT_EQ(sizes[0], 10);
    // All leftover span (50) goes to growIdx when stretchAll is false.
    EXPECT_EQ(sizes[1], 60);
    EXPECT_EQ(sizes[2], 10);
    EXPECT_EQ(sizes[0] + sizes[1] + sizes[2], 80);
}

TEST(DistributeSizes, GrowIdxStopsAtMax)
{
    std::vector<SizeBound> bounds{{10, 100}, {10, 40}, {10, 100}};
    auto sizes = distributeSizes(bounds, 80, false, 1);
    ASSERT_EQ(sizes.size(), 3u);
    EXPECT_EQ(sizes[1], 40);
    EXPECT_EQ(sizes[0] + sizes[1] + sizes[2], 60);
}

TEST(DistributeSizes, StretchAllDistributesEvenly)
{
    std::vector<SizeBound> bounds{{10, 100}, {10, 100}, {10, 100}};
    auto sizes = distributeSizes(bounds, 90, true, -1);
    ASSERT_EQ(sizes.size(), 3u);
    EXPECT_EQ(sizes[0], 30);
    EXPECT_EQ(sizes[1], 30);
    EXPECT_EQ(sizes[2], 30);
}

TEST(DistributeSizes, StretchRespectsMaxSize)
{
    std::vector<SizeBound> bounds{{10, 15}, {10, 100}, {10, 100}};
    auto sizes = distributeSizes(bounds, 100, true, -1);
    ASSERT_EQ(sizes.size(), 3u);
    EXPECT_EQ(sizes[0], 15);
    EXPECT_LE(sizes[1], 100);
    EXPECT_LE(sizes[2], 100);
    EXPECT_EQ(sizes[0] + sizes[1] + sizes[2], 100);
}

TEST(HStack, MergesXAndIntersectsY)
{
    SizeBound2d left{{0, 100}, {20, 50}};
    SizeBound2d right{{0, 80}, {30, 40}};
    auto merged = hStack(left, right);
    EXPECT_EQ(merged.xBound.minSize, 0);
    EXPECT_EQ(merged.xBound.maxSize, 180);
    EXPECT_EQ(merged.yBound.minSize, 30);
    EXPECT_EQ(merged.yBound.maxSize, 40);
}

TEST(VStack, MergesYAndIntersectsX)
{
    SizeBound2d top{{10, 50}, {0, 100}};
    SizeBound2d bottom{{0, 100}, {20, 80}};
    auto merged = vStack(top, bottom);
    EXPECT_EQ(merged.xBound.minSize, 10);
    EXPECT_EQ(merged.xBound.maxSize, 50);
    EXPECT_EQ(merged.yBound.minSize, 20);
    EXPECT_EQ(merged.yBound.maxSize, 180);
}

TEST(ConstraintIntersectionSafe, IncompatibleCollapsesToMin)
{
    SizeBound a{50, 100};
    SizeBound b{0, 40};
    auto result = constraintIntersectionSafe(a, b);
    EXPECT_EQ(result.minSize, 50);
    EXPECT_EQ(result.maxSize, 50);
}
