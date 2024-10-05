#include <gtest/gtest.h>
#include "../src/helpers/helpers.h"

using namespace Borealis::Helpers;

TEST(BitMaskTest, EnableAndCheckBit) {
    BitMask mask1;
    mask1.EnableBit(0);
    EXPECT_TRUE(mask1.IsBitEnabled(0));

    mask1.EnableBit(63);
    EXPECT_TRUE(mask1.IsBitEnabled(63));

    EXPECT_FALSE(mask1.IsBitEnabled(1));
}

TEST(BitMaskTest, ResetMask) {
    BitMask mask1;
    mask1.EnableBit(10);
    EXPECT_TRUE(mask1.IsBitEnabled(10));

    mask1.Reset();
    EXPECT_FALSE(mask1.IsBitEnabled(10));
}

TEST(BitMaskTest, DisableBit) {
    BitMask mask1;
    mask1.EnableBit(5);
    EXPECT_TRUE(mask1.IsBitEnabled(5));

    mask1.DisableBit(5);
    EXPECT_FALSE(mask1.IsBitEnabled(5));
}

TEST(BitMaskTest, AllBitsEnabled) {
    BitMask mask1(~static_cast<Borealis::Types::Uint64>(0));
    EXPECT_TRUE(mask1.AllBitsEnabled());

    mask1.DisableBit(5);
    EXPECT_FALSE(mask1.AllBitsEnabled());

    BitMask mask2;
    mask2 = std::move(mask1);
    
    EXPECT_FALSE(mask2.AllBitsEnabled());

    mask2.EnableBit(5);
    EXPECT_TRUE(mask2.AllBitsEnabled());

    mask2.Reset();
    EXPECT_FALSE(mask2.AllBitsEnabled());
}

TEST(BitMaskTest, EqualityOperator) {
    BitMask mask1, mask2;
    mask1.EnableBit(5);
    mask2.EnableBit(5);
    EXPECT_TRUE(mask1 == mask2);

    mask2.EnableBit(6);
    EXPECT_FALSE(mask1 == mask2);
}

TEST(BitMaskTest, InequalityOperator) {
    BitMask mask1, mask2;
    mask1.EnableBit(4);
    mask2.EnableBit(7);
    EXPECT_TRUE(mask1 != mask2);

    mask2.EnableBit(4);
    mask1.EnableBit(7);
    EXPECT_FALSE(mask1 != mask2);
}

TEST(BitMaskTest, LessThanOperator) {
    BitMask mask1, mask2;
    mask1.EnableBit(3);
    mask2.EnableBit(4);
    EXPECT_TRUE(mask1 < mask2);
    EXPECT_FALSE(mask2 < mask1);
}

TEST(BitMaskTest, GreaterThanOperator) {
    BitMask mask1, mask2;
    mask1.EnableBit(15);
    mask2.EnableBit(14);
    EXPECT_TRUE(mask1 > mask2);
    EXPECT_FALSE(mask2 > mask1);
}

TEST(BitMaskTest, BitwiseAndOperator) {
    BitMask mask1, mask2;
    mask1.EnableBit(0);
    mask1.EnableBit(1);
    mask2.EnableBit(1);

    EXPECT_TRUE((mask1 & mask2).GetRawBitMask() != 0);
    EXPECT_FALSE((mask1 & BitMask(4)).GetRawBitMask() != 0);
}

TEST(BitMaskTest, BitwiseOrOperator) {
    BitMask mask1, mask2;
    mask1.EnableBit(0);
    mask2.EnableBit(1);

    BitMask result = mask1 | mask2;
    EXPECT_TRUE(result.IsBitEnabled(0));
    EXPECT_TRUE(result.IsBitEnabled(1));
}

TEST(BitMaskTest, BitwiseOrAssignmentOperator) {
    BitMask mask1;
    mask1.EnableBit(2);
    mask1 |= 1;  // Set bit 0

    EXPECT_TRUE(mask1.IsBitEnabled(0));
    EXPECT_TRUE(mask1.IsBitEnabled(2));
}

TEST(BitMaskTest, BitwiseAndAssignmentOperator) {
    BitMask mask1;
    mask1.EnableBit(0);
    mask1.EnableBit(1);
    mask1 &= 1;  // Only keep bit 0

    EXPECT_TRUE(mask1.IsBitEnabled(0));
    EXPECT_FALSE(mask1.IsBitEnabled(1));
}

TEST(BitMaskTest, BitwiseXorAssignmentOperator) {
    BitMask mask1;
    mask1.EnableBit(0);
    mask1.EnableBit(1);
    mask1 ^= 1;

    EXPECT_FALSE(mask1.IsBitEnabled(0));
    EXPECT_TRUE(mask1.IsBitEnabled(1));
}

TEST(BitMaskTest, LeftShiftOperator) {
    BitMask mask1;
    mask1.EnableBit(0);
    EXPECT_TRUE((mask1 << 1).IsBitEnabled(1));
}

TEST(BitMaskTest, RightShiftOperator) {
    BitMask mask1;
    mask1.EnableBit(1);
    mask1 = mask1 >> 1;
    EXPECT_TRUE(mask1.IsBitEnabled(0));
}

TEST(BitMaskTest, GetRawBitMask) {
    BitMask mask1;
    mask1.EnableBit(2);
    mask1.EnableBit(5);

    EXPECT_EQ(mask1.GetRawBitMask(), 0b100100);
}

TEST(BitMaskTest, GetRawBitMaskRef) {
    BitMask mask1;
    mask1.EnableBit(2);
    mask1.EnableBit(5);

    const Borealis::Types::Uint64& maskRef = mask1.GetRawBitMaskRef();
    EXPECT_EQ(maskRef, 0b100100);
}

TEST(BitMaskTest, CopyConstructor) {
    BitMask original;
    original.EnableBit(5);
    original.EnableBit(10);

    BitMask copy(original);

    EXPECT_TRUE(copy == original);

    original.EnableBit(15);
    EXPECT_FALSE(copy.IsBitEnabled(15));
}

TEST(BitMaskTest, CopyAssignmentOperator) {
    BitMask original;
    original.EnableBit(7);
    original.EnableBit(20);

    BitMask copy;
    copy = original;

    EXPECT_TRUE(copy == original);

    original.EnableBit(30);
    EXPECT_FALSE(copy.IsBitEnabled(30));
}

TEST(BitMaskTest, MoveConstructor) {
    BitMask original;
    original.EnableBit(8);
    original.EnableBit(12);

    BitMask moved(std::move(original));

    EXPECT_TRUE(moved.IsBitEnabled(8));
    EXPECT_TRUE(moved.IsBitEnabled(12));

    EXPECT_FALSE(original.IsBitEnabled(8));
    EXPECT_FALSE(original.IsBitEnabled(12));
    EXPECT_EQ(original.GetRawBitMask(), 0);
}

TEST(BitMaskTest, MoveAssignmentOperator) {
    BitMask original;
    original.EnableBit(9);
    original.EnableBit(14);

    BitMask moved;
    moved = std::move(original);  

    EXPECT_TRUE(moved.IsBitEnabled(9));
    EXPECT_TRUE(moved.IsBitEnabled(14));

    EXPECT_FALSE(original.IsBitEnabled(9));
    EXPECT_FALSE(original.IsBitEnabled(14));
    EXPECT_EQ(original.GetRawBitMask(), 0);
}