#include <gtest/gtest.h>
#include "../src/helpers/helpers.h"

using namespace Borealis::Helpers;
using namespace Borealis::Math;

// ------------------------- BITMASK ------------------------- //


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
    mask1 |= 1;

    EXPECT_TRUE(mask1.IsBitEnabled(0));
    EXPECT_TRUE(mask1.IsBitEnabled(2));
}

TEST(BitMaskTest, BitwiseAndAssignmentOperator) {
    BitMask mask1;
    mask1.EnableBit(0);
    mask1.EnableBit(1);
    mask1 &= 1;

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


// ------------------------- VECTOR2 ------------------------- //


TEST(Vector2Test, DefaultConstructor) {
    Vector2<int> v;
    EXPECT_EQ(v.x, 0);
    EXPECT_EQ(v.y, 0);
}

TEST(Vector2Test, ParameterizedConstructor) {
    Vector2<float> v(1.0f, 2.0f);
    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
}

TEST(Vector2Test, Addition) {
    Vector2<int> v1(1, 2);
    Vector2<int> v2(3, 4);
    Vector2<int> result = v1 + v2;
    EXPECT_EQ(result.x, 4);
    EXPECT_EQ(result.y, 6);
}

TEST(Vector2Test, Subtraction) {
    Vector2<double> v1(5.0, 7.0);
    Vector2<double> v2(2.0, 3.0);
    Vector2<double> result = v1 - v2;
    EXPECT_DOUBLE_EQ(result.x, 3.0);
    EXPECT_DOUBLE_EQ(result.y, 4.0);
}

TEST(Vector2Test, ScalarMultiplication) {
    Vector2<int> v(2, 3);
    Vector2<int> result = v * 3.f;
    EXPECT_EQ(result.x, 6);
    EXPECT_EQ(result.y, 9);
}

TEST(Vector2Test, DotProduct) {
    Vector2<int> v1(1, 2);
    Vector2<int> v2(3, 4);
    EXPECT_EQ(v1.Dot(v2), 11);
}

TEST(Vector2Test, Length) {
    Vector2<float> v(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.Length(), 5.0f); 
}

TEST(Vector2Test, Normalize) {
    Vector2<float> v(3.0f, 4.0f);
    Vector2<float> result = v.Normalize();
    EXPECT_FLOAT_EQ(result.Length(), 1.0f);
}

TEST(Vector2Test, Equality) {
    Vector2<int> v1(1, 2);
    Vector2<int> v2(1, 2);
    EXPECT_TRUE(v1 == v2);
}

TEST(Vector2Test, Inequality) {
    Vector2<int> v1(1, 2);
    Vector2<int> v2(3, 4);
    EXPECT_FALSE(v1 == v2);
}

TEST(Vector2Test, Negation) {
    Vector2<int> v(1, -2);
    Vector2<int> result = v * -1.f;
    EXPECT_EQ(result.x, -1);
    EXPECT_EQ(result.y, 2);
}

TEST(Vector2Test, AdditionWithZero) {
    Vector2<int> v(5, 10);
    Vector2<int> zero;
    Vector2<int> result = v + zero;
    EXPECT_EQ(result.x, 5);
    EXPECT_EQ(result.y, 10);
}

TEST(Vector2Test, SubtractionWithItself) {
    Vector2<int> v(7, 9);
    Vector2<int> result = v - v;
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
}

TEST(Vector2Test, DotProductWithZeroVector) {
    Vector2<int> v(2, 3);
    Vector2<int> zero;
    EXPECT_EQ(v.Dot(zero), 0);
}

TEST(Vector2Test, ScalarMultiplicationByZero) {
    Vector2<int> v(2, 3);
    Vector2<int> result = v * (Borealis::Types::Uint64) 0;
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
}


// ------------------------- VECTOR3 ------------------------- //


TEST(Vector3Test, DefaultConstructor) {
    Vector3<int> v;
    EXPECT_EQ(v.x, 0);
    EXPECT_EQ(v.y, 0);
    EXPECT_EQ(v.z, 0);
}

TEST(Vector3Test, ParameterizedConstructor) {
    Vector3<double> v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.x, 1.0);
    EXPECT_DOUBLE_EQ(v.y, 2.0);
    EXPECT_DOUBLE_EQ(v.z, 3.0);
}

TEST(Vector3Test, Addition) {
    Vector3<int> v1(1, 2, 3);
    Vector3<int> v2(4, 5, 6);
    Vector3<int> result = v1 + v2;
    EXPECT_EQ(result.x, 5);
    EXPECT_EQ(result.y, 7);
    EXPECT_EQ(result.z, 9);
}

TEST(Vector3Test, Subtraction) {
    Vector3<int> v1(10, 20, 30);
    Vector3<int> v2(5, 10, 15);
    Vector3<int> result = v1 - v2;
    EXPECT_EQ(result.x, 5);
    EXPECT_EQ(result.y, 10);
    EXPECT_EQ(result.z, 15);
}

TEST(Vector3Test, ScalarMultiplication) {
    Vector3<int> v(1, 2, 3);
    Vector3<int> result = v * 2.f;
    EXPECT_EQ(result.x, 2);
    EXPECT_EQ(result.y, 4);
    EXPECT_EQ(result.z, 6);
}

TEST(Vector3Test, DotProduct) {
    Vector3<int> v1(1, 2, 3);
    Vector3<int> v2(4, 5, 6);
    EXPECT_EQ(v1.Dot(v2), 32);
}

TEST(Vector3Test, CrossProduct) {
    Vector3<int> v1(1, 0, 0);
    Vector3<int> v2(0, 1, 0);
    Vector3<int> result = v1.Cross(v2);
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
    EXPECT_EQ(result.z, 1);
}

TEST(Vector3Test, Length) {
    Vector3<float> v(3.0f, 4.0f, 0.0f);
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);
}

TEST(Vector3Test, Normalize) {
    Vector3<float> v(3.0f, 4.0f, 0.0f);
    Vector3<float> result = v.Normalize();
    EXPECT_FLOAT_EQ(result.Length(), 1.0f);
}

TEST(Vector3Test, Equality) {
    Vector3<int> v1(1, 2, 3);
    Vector3<int> v2(1, 2, 3);
    EXPECT_TRUE(v1 == v2);
}

TEST(Vector3Test, Inequality) {
    Vector3<int> v1(1, 2, 3);
    Vector3<int> v2(3, 2, 1);
    EXPECT_FALSE(v1 == v2);
}

TEST(Vector3Test, AdditionWithZero) {
    Vector3<int> v(1, 2, 3);
    Vector3<int> zero;
    Vector3<int> result = v + zero;
    EXPECT_EQ(result.x, 1);
    EXPECT_EQ(result.y, 2);
    EXPECT_EQ(result.z, 3);
}

TEST(Vector3Test, SubtractionWithItself) {
    Vector3<int> v(1, 2, 3);
    Vector3<int> result = v - v;
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
    EXPECT_EQ(result.z, 0);
}

TEST(Vector3Test, CrossProductWithItself) {
    Vector3<int> v(1, 2, 3);
    Vector3<int> result = v.Cross(v);
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
    EXPECT_EQ(result.z, 0);
}

TEST(Vector3Test, ScalarMultiplicationByZero) {
    Vector3<int> v(2, 3, 4);
    Vector3<int> result = v * 0.0f;
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
    EXPECT_EQ(result.z, 0);
}

TEST(Vector3Test, DotProductWithZeroVector) {
    Vector3<int> v(2, 3, 4);
    Vector3<int> zero;
    EXPECT_EQ(v.Dot(zero), 0);
}



// ------------------------- VECTOR4 ------------------------- //



TEST(Vector4Test, DefaultConstructor) {
    Vector4<int> v;
    EXPECT_EQ(v.x, 0);
    EXPECT_EQ(v.y, 0);
    EXPECT_EQ(v.z, 0);
    EXPECT_EQ(v.w, 0);
}

TEST(Vector4Test, ParameterizedConstructor) {
    Vector4<double> v(1.0, 2.0, 3.0, 4.0);
    EXPECT_DOUBLE_EQ(v.x, 1.0);
    EXPECT_DOUBLE_EQ(v.y, 2.0);
    EXPECT_DOUBLE_EQ(v.z, 3.0);
    EXPECT_DOUBLE_EQ(v.w, 4.0);
}

TEST(Vector4Test, Addition) {
    Vector4<int> v1(1, 2, 3, 4);
    Vector4<int> v2(4, 5, 6, 7);
    Vector4<int> result = v1 + v2;
    EXPECT_EQ(result.x, 5);
    EXPECT_EQ(result.y, 7);
    EXPECT_EQ(result.z, 9);
    EXPECT_EQ(result.w, 11);
}

TEST(Vector4Test, Subtraction) {
    Vector4<int> v1(10, 20, 30, 40);
    Vector4<int> v2(5, 10, 15, 20);
    Vector4<int> result = v1 - v2;
    EXPECT_EQ(result.x, 5);
    EXPECT_EQ(result.y, 10);
    EXPECT_EQ(result.z, 15);
    EXPECT_EQ(result.w, 20);
}

TEST(Vector4Test, ScalarMultiplication) {
    Vector4<int> v(1, 2, 3, 4);
    Vector4<int> result = v * 2.f;
    EXPECT_EQ(result.x, 2);
    EXPECT_EQ(result.y, 4);
    EXPECT_EQ(result.z, 6);
    EXPECT_EQ(result.w, 8);
}

TEST(Vector4Test, Equality) {
    Vector4<int> v1(1, 2, 3, 4);
    Vector4<int> v2(1, 2, 3, 4);
    EXPECT_TRUE(v1 == v2);
}

TEST(Vector4Test, Inequality) {
    Vector4<int> v1(1, 2, 3, 4);
    Vector4<int> v2(4, 3, 2, 1);
    EXPECT_FALSE(v1 == v2);
}

TEST(Vector4Test, AdditionWithZero) {
    Vector4<int> v(1, 2, 3, 4);
    Vector4<int> zero;
    Vector4<int> result = v + zero;
    EXPECT_EQ(result.x, 1);
    EXPECT_EQ(result.y, 2);
    EXPECT_EQ(result.z, 3);
    EXPECT_EQ(result.w, 4);
}

TEST(Vector4Test, SubtractionWithItself) {
    Vector4<int> v(1, 2, 3, 4);
    Vector4<int> result = v - v;
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
    EXPECT_EQ(result.z, 0);
    EXPECT_EQ(result.w, 0);
}

TEST(Vector4Test, ScalarMultiplicationByZero) {
    Vector4<int> v(2, 3, 4, 5);
    Vector4<int> result = v * 0.f;
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
    EXPECT_EQ(result.z, 0);
    EXPECT_EQ(result.w, 0);
}
