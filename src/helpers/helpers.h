#pragma once

#include "../core/types/types.h"
#include <math.h>

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

			~BitMask() = default;

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
				return *this;
			}

			BitMask& operator=(BitMask&& other) noexcept
			{
				bitMask = other.bitMask;
				other.Reset();
				return *this;
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

			BitMask operator&(const BitMask& other) const
			{
				return BitMask(bitMask & other.bitMask);
			}

			BitMask operator|(const BitMask& other) const
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

			BitMask operator<<(const Types::Uint8 bit)
			{
				return BitMask(bitMask << bit);
			}

			BitMask operator>>(const Types::Uint8 bit)
			{
				return BitMask(bitMask >> bit);
			}

			inline void EnableBit(const Types::Uint8 bit)
			{
				bitMask |= (static_cast<Types::Uint64>(1) << bit);
			}

			inline void DisableBit(const Types::Uint8 bit)
			{
				bitMask &= ~(static_cast<Types::Uint64>(1) << bit);
			}

			inline bool IsBitEnabled(const Types::Uint8 bit) const
			{
				return bitMask & (static_cast<Types::Uint64>(1) << bit);
			}

			inline bool AllBitsEnabled() const
			{
				return bitMask == ~static_cast<Types::Uint64>(0);
			}

			const Types::Uint64& GetRawBitMaskRef() const
			{
				return bitMask;
			}

			Types::Uint64 GetRawBitMask() const
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


			bool operator==(const Vector2& other) const
			{
				return x == other.x && y == other.y;
			}

			bool operator!=(const Vector2& other) const
			{
				return x != other.x || y != other.y;
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


			Vector2 operator*(Types::Uint64 mul)
			{
				return Vector2{x * (T)mul, y * (T)mul};
			}

			Vector2 operator/(Types::Uint64 div)
			{
				return Vector2{x / (T)div, y / (T)div};
			}

			Vector2 operator+(Types::Uint64 add)
			{
				return Vector2{x + (T)add, y + (T)add};
			}

			Vector2 operator-(Types::Uint64 sub)
			{
				return Vector2{x - (T)sub, y - (T)sub};
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


			Vector2 operator*(float mul)
			{
				return Vector2 (x * mul, y * mul );
			}

			Vector2 operator/(float div)
			{
				return Vector2{ x / div, y / div };
			}

			Vector2 operator+(float add)
			{
				return Vector2{ x + add, y + add };
			}

			Vector2 operator-(float sub)
			{
				return Vector2{ x - sub, y - sub };
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


			Vector2 operator*(Vector2<T> mul)
			{
				return Vector2( x * mul.x, y * mul.y );
			}

			Vector2 operator/(Vector2<T> div)
			{
				return Vector2( x / div.x, y / div.y );
			}

			Vector2 operator+(Vector2<T> add)
			{
				return Vector2(x + add.x, y + add.y);
			}

			Vector2 operator-(Vector2<T> sub)
			{
				return Vector2(x - sub.x, y - sub.y);
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

			float Dot(const Vector2<T>& other) const
			{
				if (Length() == 0 || other.Length() == 0) return 0.f;
				return (x * other.x) + (y * other.y);
			}

			float Length() const
			{
				return sqrtf( (x * x) + (y * y) );
			}

			Vector2 Normalize() const
			{
				float len = Length();
				return Vector2(x / len, y / len);
			}

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

			bool operator==(const Vector3<T>& other) const
			{
				return x == other.x && y == other.y && z == other.z;
			}

			bool operator!=(const Vector3<T>& other) const
			{
				return x != other.x || y != other.y || z != other.z;
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


			Vector3 operator*(Types::Uint64 mul)
			{
				return Vector3{ x * mul, y * mul, z * mul };
			}

			Vector3 operator/(Types::Uint64 div)
			{
				return Vector3{ x / div, y / div, z / div };
			}

			Vector3 operator+(Types::Uint64 add)
			{
				return Vector3{ x + add, y + add, z + add };
			}

			Vector3 operator-(Types::Uint64 sub)
			{
				return Vector3{ x - sub, y - sub, z - sub };
			}


			Vector3& operator*=(Types::Uint64 mul)
			{
				x *= mul;
				y *= mul;
				z *= mul;
				return *this;
			}

			Vector3& operator/=(Types::Uint64 div)
			{
				x /= div;
				y /= div;
				z /= div;

				return *this;
			}

			Vector3& operator+=(Types::Uint64 add)
			{
				x += add;
				y += add;
				z += add;
				return *this;
			}

			Vector3& operator-=(Types::Uint64 sub)
			{
				x -= sub;
				y -= sub;
				z -= sub;
				return *this;
			}


			Vector3 operator*(float mul)
			{
				return Vector3{ x * (T)mul, y * (T)mul, z * (T)mul };
			}

			Vector3 operator/(float div)
			{
				return Vector3{ x / (T)div, y / (T)div, z / (T)div };
			}

			Vector3 operator+(float add)
			{
				return Vector3{ x + (T)add, y + (T)add, z + (T)add };
			}

			Vector3 operator-(float sub)
			{
				return Vector3{ x - (T)sub, y - (T)sub, z - (T)sub };
			}


			Vector3& operator*=(float mul)
			{
				x *= mul;
				y *= mul;
				z *= mul;
				return *this;
			}

			Vector3& operator/=(float div)
			{
				x /= div;
				y /= div;
				z /= div;
				return *this;
			}

			Vector3& operator+=(float add)
			{
				x += add;
				y += add;
				z += add;
				return *this;
			}

			Vector3& operator-=(float sub)
			{
				x -= sub;
				y -= sub;
				z -= sub;
				return *this;
			}

			
			Vector3 operator*(Vector3<T> mul)
			{
				return Vector3{ x * mul.x, y * mul.y, z * mul.z };
			}

			Vector3 operator/(Vector3<T> div)
			{
				return Vector3{ x / div.x, y / div.y, z / div.z };
			}

			Vector3 operator+(Vector3<T> add)
			{
				return Vector3{ x + add.x, y + add.y, z + add.z };
			}

			Vector3 operator-(Vector3<T> sub)
			{
				return Vector3{ x - sub.x, y - sub.y, z - sub.z };
			}


			Vector3& operator*=(Vector3<T> mul)
			{
				x *= mul.x;
				y *= mul.y;
				z *= mul.z;
				return *this;
			}

			Vector3& operator/=(Vector3<T> div)
			{
				x /= div.x;
				y /= div.y;
				z /= div.z;
				return *this;
			}

			Vector3& operator+=(Vector3<T> add)
			{
				x += add.x;
				y += add.y;
				z += add.z;
				return *this;
			}

			Vector3& operator-=(Vector3<T> sub)
			{
				x -= sub.x;
				y -= sub.y;
				z -= sub.z;
				return *this;
			}


			void Reset()
			{
				x = {};
				y = {};
				z = {};
			}


			float Dot(const Vector3<T>& other) const
			{
				if (Length() == 0 || other.Length() == 0) return 0.f;
				return (x * other.x) + (y * other.y) + (z * other.z);
			}

			float Length() const
			{
				return sqrtf((x * x) + (y * y) + (z * z));
			}

			Vector3 Normalize() const
			{
				float len = Length();
				return Vector3( x / len, y / len, z / len );
			}

			Vector3 Cross(const Vector3<T>& other) const
			{
				T newX = (y * other.z) - (z * other.y);
				T newY = (z * other.x) - (x * other.z);
				T newZ = (x * other.y) - (y * other.x);

				return Vector3<T>(newX, newY, newZ);
			}

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

			bool operator==(const Vector4<T>& other) const
			{
				return x == other.x && y == other.y &&
					z == other.z && w == other.w;
			}

			bool operator!=(const Vector4<T>& other) const
			{
				return x != other.x || y != other.y || 
					z != other.z || w != other.w;
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


			Vector4 operator*(Types::Uint64 mul)
			{
				return Vector4{ x * mul, y * mul, z * mul, w * mul };
			}

			Vector4 operator/(Types::Uint64 div)
			{
				return Vector4{ x / div, y / div, z / div, w / div };
			}

			Vector4 operator+(Types::Uint64 add)
			{
				return Vector4{ x + add, y + add, z + add, w + add };
			}

			Vector4 operator-(Types::Uint64 sub)
			{
				return Vector4{ x - sub, y - sub, z - sub, w - sub };
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


			Vector4 operator*(float mul)
			{
				return Vector4{ x * (T)mul, y * (T)mul, z * (T)mul, w * (T)mul };
			}

			Vector4 operator/(float div)
			{
				return Vector4{ x / (T)div, y / (T)div, z / (T)div, w / (T)div };
			}

			Vector4 operator+(float add)
			{
				return Vector4{ x + (T)add, y + (T)add, z + (T)add, w + (T)add };
			}

			Vector4 operator-(float sub)
			{
				return Vector4{ x - (T)sub, y - (T)sub, z - (T)sub, w - (T)sub };
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


			Vector4 operator*(Vector4<T> mul)
			{
				return Vector4{ x * mul.x, y * mul.y, z * mul.z, w * mul.w };
			}

			Vector4 operator/(Vector4<T> div)
			{
				return Vector4{ x / div.x, y / div.y, z / div.z, w / div.w };
			}

			Vector4 operator+(Vector4<T> add)
			{
				return Vector4{ x + add.x, y + add.y, z + add.z, w + add.w };
			}

			Vector4 operator-(Vector4<T> sub)
			{
				return Vector4{ x - sub.x, y - sub.y, z - sub.z, w - sub.w };
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

			T x;
			T y;
			T z;
			T w;
		};

	}
}
