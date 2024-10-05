#include <gtest/gtest.h>
#include "../src/helpers/helpers.h"

using namespace Borealis::Helpers;

TEST(BorealisBorealisBitMaskTest, SetAndCheckBit) {
    BitMask mask1;

    mask1.SetBit(0);
    EXPECT_TRUE(mask1.IsBitSet(0));

    mask1.SetBit(63);
    EXPECT_TRUE(mask1.IsBitSet(63));

    EXPECT_FALSE(mask1.IsBitSet(1)); 
}

TEST(BorealisBitMaskTest, ResetMask) {
    BitMask mask1;

    mask1.SetBit(10);
    EXPECT_TRUE(mask1.IsBitSet(10));

    mask1.Reset();
    EXPECT_FALSE(mask1.IsBitSet(10));
}

TEST(BorealisBitMaskTest, EqualityOperator) {
    BitMask mask1, mask2;

    mask1.SetBit(5);
    mask2.SetBit(5);
    EXPECT_TRUE(mask1 == mask2);

    mask2.SetBit(6);
    EXPECT_FALSE(mask1 == mask2);
}

TEST(BorealisBitMaskTest, InequalityOperator) {
    BitMask mask1, mask2;

    mask1.SetBit(4);
    mask2.SetBit(7);
    EXPECT_TRUE(mask1 != mask2);

    mask2.SetBit(4);
    mask1.SetBit(7);
    EXPECT_FALSE(mask1 != mask2);
}

TEST(BorealisBitMaskTest, LessThanOperator) {
    BitMask mask1, mask2;

    mask1.SetBit(3);
    mask2.SetBit(4);
    EXPECT_TRUE(mask1 < mask2);
    EXPECT_FALSE(mask2 < mask1);
}

TEST(BorealisBitMaskTest, GreaterThanOperator) {
    BitMask mask1, mask2;

    mask1.SetBit(15);
    mask2.SetBit(14);
    EXPECT_TRUE(mask1 > mask2);
    EXPECT_FALSE(mask2 > mask1);
}

TEST(BorealisBitMaskTest, BitwiseAndOperator) {
    BitMask mask1, mask2;

    mask1.SetBit(0);
    mask1.SetBit(1);
    mask2.SetBit(1);

    EXPECT_TRUE(mask1 & mask2);  // Both have bit 1 set
    EXPECT_FALSE(mask1 & BitMask(4));  // No common bits
}

TEST(BorealisBitMaskTest, BitwiseOrOperator) {
    BitMask mask1, mask2;

    mask1.SetBit(0);
    mask2.SetBit(1);

    BitMask result = mask1 | mask2;
    EXPECT_TRUE(result.IsBitSet(0));
    EXPECT_TRUE(result.IsBitSet(1));
}

TEST(BorealisBitMaskTest, BitwiseOrAssignmentOperator) {
    BitMask mask1;

    mask1.SetBit(2);
    mask1 |= 1;  // Set bit 0

    EXPECT_TRUE(mask1.IsBitSet(0));
    EXPECT_TRUE(mask1.IsBitSet(2));
}

TEST(BorealisBitMaskTest, BitwiseAndAssignmentOperator) {
    BitMask mask1;

    mask1.SetBit(0);
    mask1.SetBit(1);
    mask1 &= 1;  // Only keep bit 0

    EXPECT_TRUE(mask1.IsBitSet(0));
    EXPECT_FALSE(mask1.IsBitSet(1));
}

TEST(BorealisBitMaskTest, BitwiseXorAssignmentOperator) {
    BitMask mask1;

    mask1.SetBit(0);
    mask1.SetBit(1);
    mask1 ^= 1;  // Toggle bit 0

    EXPECT_FALSE(mask1.IsBitSet(0));
    EXPECT_TRUE(mask1.IsBitSet(1));
}

TEST(BorealisBitMaskTest, LeftShiftOperator) {
    BitMask mask1;

    mask1.SetBit(0);
    mask1 << 1;
    EXPECT_TRUE(mask1.IsBitSet(1));  // Bit 0 shifted to bit 1
}

TEST(BorealisBitMaskTest, RightShiftOperator) {
    BitMask mask1;

    mask1.SetBit(1);
    mask1 >> 1;
    EXPECT_TRUE(mask1.IsBitSet(0));  // Bit 1 shifted to bit 0
}

TEST(BitMaskTest, SetAndCheckBit) {
    BitMask mask1;
    mask1.SetBit(0);
    EXPECT_TRUE(mask1.IsBitSet(0));

    mask1.SetBit(63);
    EXPECT_TRUE(mask1.IsBitSet(63));

    EXPECT_FALSE(mask1.IsBitSet(1));  // Bit 1 should not be set
}

TEST(BitMaskTest, ResetMask) {
    BitMask mask1;
    mask1.SetBit(10);
    EXPECT_TRUE(mask1.IsBitSet(10));

    mask1.Reset();
    EXPECT_FALSE(mask1.IsBitSet(10));
}

TEST(BitMaskTest, UnSetBit) {
    BitMask mask1;
    mask1.SetBit(5);
    EXPECT_TRUE(mask1.IsBitSet(5));

    mask1.UnSetBit(5);
    EXPECT_FALSE(mask1.IsBitSet(5));
}

TEST(BitMaskTest, AllBitsSet) {
    BitMask mask1(~static_cast<Borealis::Types::Uint64>(0));  // All bits set
    EXPECT_TRUE(mask1.AllBitsSet());

    mask1.UnSetBit(5);  // Now one bit is unset
    EXPECT_FALSE(mask1.AllBitsSet());
}

TEST(BitMaskTest, EqualityOperator) {
    BitMask mask1, mask2;
    mask1.SetBit(5);
    mask2.SetBit(5);
    EXPECT_TRUE(mask1 == mask2);

    mask2.SetBit(6);
    EXPECT_FALSE(mask1 == mask2);
}

TEST(BitMaskTest, InequalityOperator) {
    BitMask mask1, mask2;
    mask1.SetBit(4);
    mask2.SetBit(7);
    EXPECT_TRUE(mask1 != mask2);

    mask2.SetBit(4);
    mask1.SetBit(7);
    EXPECT_FALSE(mask1 != mask2);
}

TEST(BitMaskTest, LessThanOperator) {
    BitMask mask1, mask2;
    mask1.SetBit(3);
    mask2.SetBit(4);
    EXPECT_TRUE(mask1 < mask2);
    EXPECT_FALSE(mask2 < mask1);
}

TEST(BitMaskTest, GreaterThanOperator) {
    BitMask mask1, mask2;
    mask1.SetBit(15);
    mask2.SetBit(14);
    EXPECT_TRUE(mask1 > mask2);
    EXPECT_FALSE(mask2 > mask1);
}

TEST(BitMaskTest, BitwiseAndOperator) {
    BitMask mask1, mask2;
    mask1.SetBit(0);
    mask1.SetBit(1);
    mask2.SetBit(1);

    EXPECT_TRUE(mask1 & mask2);  // Both have bit 1 set
    EXPECT_FALSE(mask1 & BitMask(4));  // No common bits
}

TEST(BitMaskTest, BitwiseOrOperator) {
    BitMask mask1, mask2;
    mask1.SetBit(0);
    mask2.SetBit(1);

    BitMask result = mask1 | mask2;
    EXPECT_TRUE(result.IsBitSet(0));
    EXPECT_TRUE(result.IsBitSet(1));
}

TEST(BitMaskTest, BitwiseOrAssignmentOperator) {
    BitMask mask1;
    mask1.SetBit(2);
    mask1 |= 1;  // Set bit 0

    EXPECT_TRUE(mask1.IsBitSet(0));
    EXPECT_TRUE(mask1.IsBitSet(2));
}

TEST(BitMaskTest, BitwiseAndAssignmentOperator) {
    BitMask mask1;
    mask1.SetBit(0);
    mask1.SetBit(1);
    mask1 &= 1;  // Only keep bit 0

    EXPECT_TRUE(mask1.IsBitSet(0));
    EXPECT_FALSE(mask1.IsBitSet(1));
}

TEST(BitMaskTest, BitwiseXorAssignmentOperator) {
    BitMask mask1;
    mask1.SetBit(0);
    mask1.SetBit(1);
    mask1 ^= 1;  // Toggle bit 0

    EXPECT_FALSE(mask1.IsBitSet(0));
    EXPECT_TRUE(mask1.IsBitSet(1));
}

TEST(BitMaskTest, LeftShiftOperator) {
    BitMask mask1;
    mask1.SetBit(0);
    mask1 << 1;
    EXPECT_TRUE(mask1.IsBitSet(1));  // Bit 0 shifted to bit 1
}

TEST(BitMaskTest, RightShiftOperator) {
    BitMask mask1;
    mask1.SetBit(1);
    mask1 >> 1;
    EXPECT_TRUE(mask1.IsBitSet(0));  // Bit 1 shifted to bit 0
}

TEST(BitMaskTest, GetRawBitMask) {
    BitMask mask1;
    mask1.SetBit(2);
    mask1.SetBit(5);

    EXPECT_EQ(mask1.GetRawBitMask(), 0b100100);  // Should match the raw value of the bitmask
}

TEST(BitMaskTest, GetRawBitMaskRef) {
    BitMask mask1;
    mask1.SetBit(2);
    mask1.SetBit(5);

    const Borealis::Types::Uint64& maskRef = mask1.GetRawBitMaskRef();
    EXPECT_EQ(maskRef, 0b100100);  // The reference should hold the same value as the bitmask
}