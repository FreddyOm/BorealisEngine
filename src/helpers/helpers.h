#pragma once

#include "../core/types/types.h"

namespace Borealis
{
	namespace Helpers
	{
		struct BitMask
		{
			BitMask() :
				bitMask(0)
			{ }

			BitMask(Types::Uint64 mask) : 
				bitMask(mask)
			{ }

			~BitMask()
			{ }

			BitMask(const BitMask& other)
				: bitMask(other.bitMask)
			{ }

			BitMask(BitMask&& other) noexcept
				: bitMask(other.bitMask)
			{ 
				other.Reset();
			}

			BitMask& operator=(const BitMask& other)
			{
				bitMask = other.bitMask;
			}

			BitMask& operator=(BitMask&& other) noexcept
			{
				bitMask = other.bitMask;
				other.Reset();
			}

			bool operator==(const BitMask& other) const
			{
				return bitMask == other.bitMask;
			}

			bool operator!=(const BitMask& other) const
			{
				return bitMask != other.bitMask;
			}

			bool operator<(const BitMask& other) const
			{
				return bitMask < other.bitMask;
			}

			bool operator>(const BitMask& other) const
			{
				return bitMask > other.bitMask;
			}

			bool operator&(const BitMask& other) const
			{
				return bitMask & other.bitMask;
			}

			bool operator|(const BitMask& other) const
			{
				return bitMask | other.bitMask;
			}

			void operator|=(const Types::Uint8 bit)
			{
				bitMask |= bit;
			}

			void operator&=(const Types::Uint8 bit)
			{
				bitMask &= bit;
			}

			void operator^=(const Types::Uint8 bit)
			{
				bitMask ^= bit;
			}

			Types::Uint8 operator~()
			{
				return ~bitMask;
			}

			void operator<<(const Types::Uint8 bit)
			{
				bitMask << bit;
			}

			void operator>>(const Types::Uint8 bit)
			{
				bitMask >> bit;
			}

			inline void SetBit(const Types::Uint8 bit)
			{
				bitMask |= (static_cast<Types::Uint64>(1) << bit);
			}

			inline void UnSetBit(const Types::Uint8 bit)
			{
				bitMask |= (static_cast<Types::Uint64>(0) << bit);
			}

			inline bool IsBitSet(const Types::Uint8 bit) const
			{
				return bitMask & (static_cast<Types::Uint64>(1) << bit);
			}

			inline bool AllBitsSet() const
			{
				return bitMask == ~static_cast<Types::Uint64>(0);
			}

			const Types::Uint64& GetRawBitMaskRef()
			{
				return bitMask;
			}

			Types::Uint64 GetRawBitMask()
			{
				return bitMask;
			}

			inline void Reset()
			{
				bitMask = 0;
			}

		private:
			Types::Uint64 bitMask;
		};
	}

	namespace Math
	{
		template<typename T>
		struct Vector2
		{
			Vector2() :
				x(T{}), y(T{})
			{ }

			Vector2(T x, T y) :
				x(x), y(y)
			{ }

			Vector2(const Vector2<T>& other) :
				x(other.x), y(other.y)
			{ }

			Vector2(Vector2<T>&& other) noexcept
				: x(other.x), y(other.y)
			{
				other.Reset();
			}


			Vector2& operator=(const Vector2<T>& other)
			{
				x = other.x;
				y = other.y;
			}

			Vector2& operator=(Vector2<T>&& other) noexcept
			{
				x = other.x;
				y = other.y;

				other.Reset();
			}


			Vector2& operator*(Types::Uint64 mul)
			{
				return {x * mul, y * mul};
			}

			Vector2& operator/(Types::Uint64 div)
			{
				return {x / div, y / div};
			}

			Vector2& operator+(Types::Uint64 add)
			{
				return {x + add, y + add};
			}

			Vector2& operator-(Types::Uint64 sub)
			{
				return {x - sub, y - sub};
			}
			

			void operator*=(Types::Uint64 mul)
			{
				x *= mul;
				y *= mul;
			}

			void operator/=(Types::Uint64 div)
			{
				x /= div;
				y /= div;
			}

			void operator+=(Types::Uint64 add)
			{
				x += add;
				y += add;
			}

			void operator-=(Types::Uint64 sub)
			{
				x -= sub;
				y -= sub;
			}


			Vector2& operator*(float mul)
			{
				return { x * mul, y * mul };
			}

			Vector2& operator/(float div)
			{
				return { x / div, y / div };
			}

			Vector2& operator+(float add)
			{
				return { x + add, y + add };
			}

			Vector2& operator-(float sub)
			{
				return { x - sub, y - sub };
			}


			void operator*=(float mul)
			{
				x *= mul;
				y *= mul;
			}

			void operator/=(float div)
			{
				x /= div;
				y /= div;
			}

			void operator+=(float add)
			{
				x += add;
				y += add;
			}

			void operator-=(float sub)
			{
				x -= sub;
				y -= sub;
			}


			Vector2& operator*(Vector2<T> mul)
			{
				return { x * mul.x, y * mul.y };
			}

			Vector2& operator/(Vector2<T> div)
			{
				return { x / div.x, y / div.y };
			}

			Vector2& operator+(Vector2<T> add)
			{
				return { x + add.x, y + add.y };
			}

			Vector2& operator-(Vector2<T> sub)
			{
				return { x - sub.x, y - sub.y };
			}


			void operator*=(Vector2<T> mul)
			{
				x *= mul.x;
				y *= mul.y;
			}

			void operator/=(Vector2<T> div)
			{
				x /= div.x;
				y /= div.y;
			}

			void operator+=(Vector2<T> add)
			{
				x += add.x;
				y += add.y;
			}

			void operator-=(Vector2<T> sub)
			{
				x -= sub.x;
				y -= sub.y;
			}
			

			void Reset()
			{
				x = {};
				y = {};
			}

		private:

			T x;
			T y;
		};

		template<typename T>
		struct Vector3
		{
			Vector3() :
				x(T{}), y(T{}), z(T{})
			{ }

			Vector3(T x, T y, T z) :
				x(x), y(y), z(z)
			{ }

			Vector3(const Vector3<T>& other) :
				x(other.x), y(other.y), z(other.z)
			{ }

			Vector3(Vector3<T>&& other) noexcept
				: x(other.x), y(other.y), z(other.z)
			{
				other.Reset();
			}


			Vector3& operator=(const Vector3<T>& other)
			{
				x = other.x;
				y = other.y;
				z = other.z;
			}

			Vector3& operator=(Vector3<T>&& other) noexcept
			{
				x = other.x;
				y = other.y;
				z = other.z;

				other.Reset();
			}


			Vector3& operator*(Types::Uint64 mul)
			{
				return { x * mul, y * mul, z * mul };
			}

			Vector3& operator/(Types::Uint64 div)
			{
				return { x / div, y / div, z / div };
			}

			Vector3& operator+(Types::Uint64 add)
			{
				return { x + add, y + add, z + add };
			}

			Vector3& operator-(Types::Uint64 sub)
			{
				return { x - sub, y - sub, z - sub };
			}


			void operator*=(Types::Uint64 mul)
			{
				x *= mul;
				y *= mul;
				z *= mul;
			}

			void operator/=(Types::Uint64 div)
			{
				x /= div;
				y /= div;
				z /= div;
			}

			void operator+=(Types::Uint64 add)
			{
				x += add;
				y += add;
				z += add;
			}

			void operator-=(Types::Uint64 sub)
			{
				x -= sub;
				y -= sub;
				z -= sub;
			}


			Vector3& operator*(float mul)
			{
				return { x * mul, y * mul, z * mul };
			}

			Vector3& operator/(float div)
			{
				return { x / div, y / div, z / div };
			}

			Vector3& operator+(float add)
			{
				return { x + add, y + add, z + add };
			}

			Vector3& operator-(float sub)
			{
				return { x - sub, y - sub, z - sub };
			}


			void operator*=(float mul)
			{
				x *= mul;
				y *= mul;
				z *= mul;
			}

			void operator/=(float div)
			{
				x /= div;
				y /= div;
				z /= div;
			}

			void operator+=(float add)
			{
				x += add;
				y += add;
				z += add;
			}

			void operator-=(float sub)
			{
				x -= sub;
				y -= sub;
				z -= sub;
			}

			
			Vector3& operator*(Vector3<T> mul)
			{
				return { x * mul.x, y * mul.y, z * mul.z };
			}

			Vector3& operator/(Vector3<T> div)
			{
				return { x / div.x, y / div.y, z / div.z };
			}

			Vector3& operator+(Vector3<T> add)
			{
				return { x + add.x, y + add.y, z + add.z };
			}

			Vector3& operator-(Vector3<T> sub)
			{
				return { x - sub.x, y - sub.y, z - sub.z };
			}


			void operator*=(Vector3<T> mul)
			{
				x *= mul.x;
				y *= mul.y;
				z *= mul.z;
			}

			void operator/=(Vector3<T> div)
			{
				x /= div.x;
				y /= div.y;
				z /= div.z;
			}

			void operator+=(Vector3<T> add)
			{
				x += add.x;
				y += add.y;
				z += add.z;
			}

			void operator-=(Vector3<T> sub)
			{
				x -= sub.x;
				y -= sub.y;
				z -= sub.z;
			}


			void Reset()
			{
				x = {};
				y = {};
				z = {};
			}

		private:

			T x;
			T y;
			T z;
		};

		template<typename T>
		struct Vector4
		{
			Vector4() :
				x(T{}), y(T{}), z(T{}), w(T{})
			{ }

			Vector4(T x, T y, T z, T w) :
				x(x), y(y), z(z), w(w)
			{ }

			Vector4(const Vector4<T>& other) :
				x(other.x), y(other.y), z(other.z), w(other.w)
			{ }

			Vector4(Vector4<T>&& other) noexcept
				: x(other.x), y(other.y), z(other.z), w(other.w)
			{
				other.Reset();
			}


			Vector4& operator=(const Vector4<T>& other)
			{
				x = other.x;
				y = other.y;
				z = other.z;
				w = other.w;
			}

			Vector4& operator=(Vector4<T>&& other) noexcept
			{
				x = other.x;
				y = other.y;
				z = other.z;
				w = other.w;

				other.Reset();
			}


			Vector4& operator*(Types::Uint64 mul)
			{
				return { x * mul, y * mul, z * mul, w * mul };
			}

			Vector4& operator/(Types::Uint64 div)
			{
				return { x / div, y / div, z / div, w / div };
			}

			Vector4& operator+(Types::Uint64 add)
			{
				return { x + add, y + add, z + add, w + add };
			}

			Vector4& operator-(Types::Uint64 sub)
			{
				return { x - sub, y - sub, z - sub, w - sub };
			}


			void operator*=(Types::Uint64 mul)
			{
				x *= mul;
				y *= mul;
				z *= mul;
				w *= mul;
			}

			void operator/=(Types::Uint64 div)
			{
				x /= div;
				y /= div;
				z /= div;
				w /= div;
			}

			void operator+=(Types::Uint64 add)
			{
				x += add;
				y += add;
				z += add;
				w += add;
			}

			void operator-=(Types::Uint64 sub)
			{
				x -= sub;
				y -= sub;
				z -= sub;
				w -= sub;
			}


			Vector4& operator*(float mul)
			{
				return { x * mul, y * mul, z * mul, w * mul };
			}

			Vector4& operator/(float div)
			{
				return { x / div, y / div, z / div, w / div };
			}

			Vector4& operator+(float add)
			{
				return { x + add, y + add, z + add, w + add };
			}

			Vector4& operator-(float sub)
			{
				return { x - sub, y - sub, z - sub, w - sub };
			}


			void operator*=(float mul)
			{
				x *= mul;
				y *= mul;
				z *= mul;
				w *= mul;
			}

			void operator/=(float div)
			{
				x /= div;
				y /= div;
				z /= div;
				w /= div;
			}

			void operator+=(float add)
			{
				x += add;
				y += add;
				z += add;
				w += add;
			}

			void operator-=(float sub)
			{
				x -= sub;
				y -= sub;
				z -= sub;
				w -= sub;
			}


			Vector4& operator*(Vector4<T> mul)
			{
				return { x * mul.x, y * mul.y, z * mul.z, w * mul.w };
			}

			Vector4& operator/(Vector4<T> div)
			{
				return { x / div.x, y / div.y, z / div.z, w / div.w };
			}

			Vector4& operator+(Vector4<T> add)
			{
				return { x + add.x, y + add.y, z + add.z, w + add.w };
			}

			Vector4& operator-(Vector4<T> sub)
			{
				return { x - sub.x, y - sub.y, z - sub.z, w - sub.w };
			}


			void operator*=(Vector4<T> mul)
			{
				x *= mul.x;
				y *= mul.y;
				z *= mul.z;
				w *= mul.w;
			}

			void operator/=(Vector4<T> div)
			{
				x /= div.x;
				y /= div.y;
				z /= div.z;
				w /= div.w;
			}

			void operator+=(Vector4<T> add)
			{
				x += add.x;
				y += add.y;
				z += add.z;
				w += add.w;
			}

			void operator-=(Vector4<T> sub)
			{
				x -= sub.x;
				y -= sub.y;
				z -= sub.z;
				w -= sub.w;
			}


			void Reset()
			{
				x = {};
				y = {};
				z = {};
				w = {};
			}

		private:

			T x;
			T y;
			T z;
			T w;
		};

	}
}
