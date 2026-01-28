#pragma once

#define PI 3.14159265358979f
#define DEG_TO_RAD(DEG) ((PI / 180.f) * DEG)
#define RAD_TO_DEG(RAD) ((180.f / PI) * RAD)

#include <cmath>
#include <type_traits>	//print functions need to know if the values are integer or floating points
						//it's also pretty nice to know if you mean char or int of 8 bits

#pragma region vec2
template <typename T>
struct vec2
{
	T x, y;

	//constructors
	inline constexpr vec2() :
		x(0), y(0)
	{}
	inline constexpr vec2(const vec2& vec) :
		x(vec.x), y(vec.y)
	{}
	inline constexpr vec2(T a) :
		x(a), y(a)
	{}
	inline constexpr vec2(T a_x, T a_y) :
		x(a_x), y(a_y)
	{}

	~vec2() = default;

	//functions
	inline constexpr void Zero() 		//resets values to 0
	{
		x = y = 0;
	}
	inline const vec2& normalizeMe()
	{
		float notNullMagnitude = magnitude(*this);
		if (notNullMagnitude != 0)
			return *this /= notNullMagnitude;
	}
	//rotate this vec2 around origin or {0,0} by some degrees
	inline const vec2& rotate(vec2<float>* origin, float deg)
	{
		if (origin == NULL)	//we can simplyfy if the origin is nullptr
		{
			deg = DEG_TO_RAD(deg);	//degrees are converted to radians
			float sin = sinf(deg), cos = cosf(deg);		//calculate cos & sin once
			float localX = static_cast<float>(x), localY = static_cast<float>(y);

			x = static_cast<T>(localX * cos - localY * sin);
			y = static_cast<T>(localX * sin + localY * cos);
		}
		else
		{
			deg = DEG_TO_RAD(deg);	//degrees are converted to radians
			float sin = sinf(deg), cos = cosf(deg);		//calculate cos & sin once
			float localX = static_cast<float>(x) - origin->x, localY = static_cast<float>(y) - origin->y;  //subtract the origin

			x = static_cast<T>((localX * cos - localY * sin) + origin->x);
			y = static_cast<T>((localX * sin + localY * cos) + origin->y);	//add the origin after we are done
		}
		return *this;
	}

	//friend functions
	friend void printvec(const vec2& vec)		//resets values to 0
	{
		//https://en.cppreference.com/w/cpp/types/is_floating_point
		if (std::is_floating_point<T>::value)
			printf("floating point vec2 { %f, %f }\n", static_cast<double>(vec.x), static_cast<double>(vec.y));
		else printf("integer vec2 { %i, %i }\n", static_cast<int>(vec.x), static_cast<int>(vec.y));
	}
	friend inline float magnitude(const vec2& vec) 	//static cast to suppres warnings
	{
		return sqrtf(static_cast<float>(vec.x * vec.x + vec.y * vec.y));
	}
	inline friend vec2<float> normalize(const vec2& vec)
	{
		vec2<float> normalize(vec);
		return vec / magnitude(vec);
	}
	friend inline T scalar(const vec2& vec)
	{
		return vec.x * vec.y;
	}
	friend inline T dot(const vec2& lhs, const vec2& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}
	friend inline T sum(const vec2& vec)
	{
		return vec.x + vec.y;
	}

	//operations
	//returns self (left hand side of operator)
	inline const vec2& operator *=(const float rhs)
	{
		x *= rhs; y *= rhs;
		return *this;
	}
	inline const vec2& operator *=(const vec2& rhs)
	{
		x *= rhs.x; y *= rhs.y;
		return *this;
	}
	inline const vec2& operator /=(const float rhs)
	{
		assert(rhs == 0.0f);
		x /= rhs; y /= rhs;
		return *this;
	}
	inline const vec2& operator /=(const vec2& rhs)
	{
		assert(rhs.x == 0 || rhs.y = 0);
		x /= rhs.x; y /= rhs.y;
		return *this;
	}
	inline const vec2& operator +=(const T rhs)
	{
		x += rhs; y += rhs;
		return *this;
	}
	inline const vec2& operator +=(const vec2& rhs)
	{
		x += rhs.x; y += rhs.y;
		return *this;
	}
	inline const vec2& operator -=(const T rhs)
	{
		x -= rhs; y -= rhs;
		return *this;
	}
	inline const vec2& operator -=(const vec2& rhs)
	{
		x -= rhs.x; y -= rhs.y;
		return *this;
	}

	//friend operators
	friend inline const bool operator ==(const vec2& lhs, const vec2& rhs)
	{
		if (lhs.x == rhs.x && lhs.y == rhs.y)
			return true;
		else return false;
	}
	friend inline const bool operator !=(const vec2& lhs, const vec2& rhs)
	{
		if (lhs.x != rhs.x || lhs.y != rhs.y)
			return true;
		else return false;
	}

	friend inline vec2 operator *(const vec2& lhs, const float rhs)
	{
		return { static_cast<T>(lhs.x * rhs), static_cast<T>(lhs.y * rhs) };
	}
	friend inline vec2 operator *(const vec2& lhs, const vec2& rhs)
	{
		return { lhs.x * rhs.x, lhs.y * rhs.y };
	}
	friend inline vec2 operator /(const vec2& lhs, const float rhs)
	{
		return { static_cast<T>(lhs.x / rhs), static_cast<T>(lhs.y / rhs) };
	}
	friend inline vec2 operator /(const vec2& lhs, const vec2& rhs)
	{
		return { lhs.x / rhs.x, lhs.y / rhs.y };
	}
	friend inline vec2 operator +(const vec2& lhs, const T rhs)
	{
		return { lhs.x + rhs, lhs.y + rhs };
	}
	friend inline vec2 operator +(const vec2& lhs, const vec2& rhs)
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}
	friend inline vec2 operator -(const vec2& lhs, const T rhs)
	{
		return { lhs.x - rhs, lhs.y - rhs };
	}
	friend inline vec2 operator -(const vec2& lhs, const vec2& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

	friend inline void operator /(const float lhs, const vec2& rhs)
	{
		//solely exists so that we don't do it anyways
		printf("Undefined behaviour: dividing a float by a vec2!\n");
		return;
	}
	friend inline vec2 operator *(const float lhs, const vec2& rhs)
	{
		return { static_cast<T>(lhs * rhs.x), static_cast<T>(lhs * rhs.y) };
	}
	friend inline vec2 operator +(const T lhs, const vec2& rhs)
	{
		return { lhs + rhs.x, lhs + rhs.y };
	}
	friend inline vec2 operator -(const T lhs, const vec2& rhs)
	{
		return { lhs - rhs.x, lhs - rhs.y };
	}
};
#pragma endregion

#pragma region VECTOR3
template <typename T>
struct vec3
{
	T x, y, z;

	//constructors
	inline vec3() :
		x(0), y(0), z(0)
	{}
	inline vec3(const vec3& vec) :
		x(vec.x), y(vec.y), z(vec.z)
	{}
	inline vec3(T a) :
		x(a), y(a), z(a)
	{}
	inline vec3(T a_x, T a_y, T a_z) :
		x(a_x), y(a_y), z(a_z)
	{}

	~vec3() = default;

	//functions
	inline void Zero()		//reset to 0
	{
		x = y = z = 0;
	}
	inline const vec3& normalizeMe()
	{
		return *this /= magnitude(*this);
	}
	//pls fix: https://stackoverflow.com/questions/42421611/3d-vector-rotation-in-c
	//rotation around x, then y, then z axes.
	inline const vec3& rotate(vec3<float>* origin, float* rotX, float* rotY, float* rotZ)
	{
		if (origin == nullptr)		//select origin point if necessary
		{
			if (rotX != nullptr)
			{
				T cy = y, cz = z;
				float rad = DEG_TO_RAD(*rotX);	//degrees are converted to radians
				y = T(cy * cosf(rad) - cz * sinf(rad));
				z = T(cz * cosf(rad) + cy * sinf(rad));

			}
			if (rotY != nullptr)
			{
				T cx = x, cz = z;
				float rad = DEG_TO_RAD(*rotY);	//degrees are converted to radians
				x = T(cx * cosf(rad) + cz * sinf(rad));
				z = T(cz * cosf(rad) - cx * sinf(rad));

			}
			if (rotZ != nullptr)
			{
				T cx = x, cy = y;
				float rad = DEG_TO_RAD(*rotZ);	//degrees are converted to radians
				x = T(cx * cosf(rad) + cy * sinf(rad));
				y = T(cy * cosf(rad) - cx * sinf(rad));
			}
		} 
		else 
		{
			if (rotX != nullptr)
			{
				T cy = y, cz = z;
				float rad = DEG_TO_RAD(*rotX);	//degrees are converted to radians
				y = T((static_cast<float>(cy) - origin->y) * cosf(rad) - (origin->z - static_cast<float>(cz)) * sinf(rad) + origin->y);
				z = T((origin->z - static_cast<float>(cz)) * cosf(rad) + (static_cast<float>(cy) - origin->y) * sinf(rad) + origin->z);

			}
			if (rotY != nullptr)
			{
				T cx = x, cz = z;
				float rad = DEG_TO_RAD(*rotY);	//degrees are converted to radians
				x = T((static_cast<float>(cx) - origin->x) * cosf(rad) + (origin->z - static_cast<float>(cz)) * sinf(rad) + origin->x);
				z = T((origin->z - static_cast<float>(cz)) * cosf(rad) - (static_cast<float>(cx) - origin->x) * sinf(rad) + origin->z);

			}
			if (rotZ != nullptr)
			{
				T cx = x, cy = y;
				float rad = DEG_TO_RAD(*rotZ);	//degrees are converted to radians
				x = T((static_cast<float>(cx) - origin->x) * cosf(rad) + (origin->y - static_cast<float>(cy)) * sinf(rad) + origin->x);
				y = T((origin->y - static_cast<float>(cy)) * cosf(rad) - (static_cast<float>(cx) - origin->x) * sinf(rad) + origin->y);
			}
		}
		return *this;
	}

	//friended functions
	friend void printvec(const vec3& vec)		//resets values to 0
	{
		//https://en.cppreference.com/w/cpp/types/is_floating_point
		if (std::is_floating_point<T>::value)
			printf("floating point vector3 { %f, %f, %f }\n", static_cast<double>(vec.x), static_cast<double>(vec.y), static_cast<double>(vec.z));
		else printf("integer vector3 { %i , %i, %i }\n", static_cast<int>(vec.x), static_cast<int>(vec.y), static_cast<int>(vec.z));
	}
	friend inline float magnitude(const vec3& vec)
	{
		return sqrtf(static_cast<float>(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
	}
	friend inline vec3<float> normalize(const vec3& vec)
	{
		vec3<float> normalize(vec);
		return vec / magnitude(vec);
	}
	friend inline T scalar(const vec3& vec)
	{
		return vec.x * vec.y * vec.z;
	}
	friend inline T dot(const vec3& lhs, const vec3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}
	friend inline vec3 cross(const vec3& a, const vec3& b)
	{
		return { a.y * b.z - b.y * a.z,		//x
					a.z * b.x - b.z * a.x,		//y
					a.x * b.y - b.x * a.y };	//z
	}
	friend inline T sum(const vec3& vec)
	{
		return vec.x + vec.y + vec.z;
	}

	//operations
	//returns self (left hand side of operator)
	inline const vec3& operator *=(const float rhs)
	{
		x *= rhs; y *= rhs; z *= rhs;
		return *this;
	}
	inline const vec3& operator *=(const vec3& rhs)
	{
		x *= rhs.x; y *= rhs.y; z *= rhs.z;
		return *this;
	}
	inline const vec3& operator /=(const float rhs)
	{
		x /= rhs; y /= rhs; z /= rhs;
		return *this;
	}
	inline const vec3& operator /=(const vec3& rhs)
	{
		x /= rhs.x; y /= rhs.y; z /= rhs.z;
		return *this;
	}
	inline const vec3& operator +=(const T rhs)
	{
		x += rhs; y += rhs; z += rhs;
		return *this;
	}
	inline const vec3& operator +=(const vec3& rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
		return *this;
	}
	inline const vec3& operator -=(const T rhs)
	{
		x -= rhs; y -= rhs; z -= rhs;
		return *this;
	}
	inline const vec3& operator -=(const vec3& rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
		return *this;
	}

	//friended operators
	friend inline const bool operator ==(const vec3& lhs, const vec3& rhs)
	{
		if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z)
			return true;
		else return false;
	}
	friend inline const bool operator !=(const vec3& lhs, const vec3& rhs)
	{
		if (lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z)
			return true;
		else return false;
	}

	friend inline vec3 operator *(const vec3& lhs, const float rhs)
	{
		return { static_cast<T>(lhs.x * rhs), static_cast<T>(lhs.y * rhs), static_cast<T>(lhs.z * rhs) };
	}
	friend inline vec3 operator *(const vec3& lhs, const vec3& rhs)
	{
		return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
	}
	friend inline vec3 operator /(const vec3& lhs, const float rhs)
	{
		return { static_cast<T>(lhs.x / rhs), static_cast<T>(lhs.y / rhs), static_cast<T>(lhs.z / rhs) };
	}
	friend inline vec3 operator /(const vec3& lhs, const vec3& rhs)
	{
		return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
	}
	friend inline vec3 operator +(const vec3& lhs, const T rhs)
	{
		return { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs };
	}
	friend inline vec3 operator +(const vec3& lhs, const vec3& rhs)
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}
	friend inline vec3 operator -(const vec3& lhs, const T rhs)
	{
		return { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs };
	}
	friend inline vec3 operator -(const vec3& lhs, const vec3& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	friend inline void operator /(const float lhs, const vec3& rhs)
	{
		//solely exists so that we don't do it anyways
		printf("Undefined behaviour: dividing a float by a vec3!\n");
		return;
	}
	friend inline vec3 operator *(const float lhs, const vec3& rhs)
	{
		return { static_cast<T>(lhs * rhs.x), static_cast<T>(lhs * rhs.y), static_cast<T>(lhs * rhs.z) };
	}
	friend inline vec3 operator +(const T lhs, const vec3& rhs)
	{
		return { lhs + rhs.x, lhs + rhs.y, lhs + rhs.z };
	}
	friend inline vec3 operator -(const T lhs, const vec3& rhs)
	{
		return { lhs - rhs.x, lhs - rhs.y, lhs - rhs.z };
	}
};
#pragma endregion

#pragma region VECTOR4
template <typename T>
struct vec4
{
	T v, x, y, z;

	//constructors
	inline vec4() :
		v(0), x(0), y(0), z(0)
	{}
	inline vec4(const vec4& vec) :
		v(vec.v), x(vec.x), y(vec.y), z(vec.z)
	{}
	inline vec4(T a) :
		v(a), x(a), y(a), z(a)
	{}
	inline vec4(T a_v, T a_x, T a_y, T a_z) :
		v(a_v), x(a_x), y(a_y), z(a_z)
	{}

	~vec4() = default;

	//functions
	inline void Zero()				//reset to 0
	{
		v = x = y = z = 0;
	}
	inline const vec4& normalizeMe()
	{
		return *this /= magnitude(*this);
	}

	//friended functions
	friend void printvec(const vec4& vec)		//resets values to 0
	{
		//https://en.cppreference.com/w/cpp/types/is_floating_point
		if (std::is_floating_point<T>::value)
			printf("floating point vector4 { %f, %f, %f, %f }\n", static_cast<double>(vec.v), static_cast<double>(vec.x), static_cast<double>(vec.y), static_cast<double>(vec.z));
		else printf("integer vector4 { %i, %i, %i, %i }\n", static_cast<int>(vec.v), static_cast<int>(vec.x), static_cast<int>(vec.y), static_cast<int>(vec.z));
	}
	friend inline float magnitude(const vec4& vec)
	{
		return sqrtf(static_cast<float>(vec.v * vec.v + vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
	}
	friend inline vec4 normalize(const vec4& vec)
	{
		vec4<float> normalize(vec);
		return normalize / magnitude(vec);
	}
	friend inline T scalar(const vec4& vec)
	{
		return vec.v * vec.x * vec.y * vec.z;
	}
	friend inline T dot(const vec4& lhs, const vec4& rhs)
	{
		return lhs.v * rhs.v + lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}
	friend inline T sum(const vec4& vec)
	{
		return vec.v + vec.x + vec.y + vec.z;
	}


	//operations
	//returns self (left hand side of operator)
	inline const vec4& operator *=(const float rhs)
	{
		v *= rhs; x *= rhs; y *= rhs; z *= rhs;
		return *this;
	}
	inline const vec4& operator *=(const vec4& rhs)
	{
		v *= rhs.v; x *= rhs.x; y *= rhs.y; z *= rhs.z;
		return *this;
	}
	inline const vec4& operator /=(const float rhs)
	{
		v /= rhs; x /= rhs; y /= rhs; z /= rhs;
		return *this;
	}
	inline const vec4& operator /=(const vec4& rhs)
	{
		v /= rhs.v; x /= rhs.x; y /= rhs.y; z /= rhs.z;
		return *this;
	}
	inline const vec4& operator +=(const T rhs)
	{
		v += rhs; x += rhs; y += rhs; z += rhs;
		return *this;
	}
	inline const vec4& operator +=(const vec4& rhs)
	{
		v += rhs.v; x += rhs.x; y += rhs.y; z += rhs.z;
		return *this;
	}
	inline const vec4& operator -=(const T rhs)
	{
		v -= rhs; x -= rhs; y -= rhs; z -= rhs;
		return *this;
	}
	inline const vec4& operator -=(const vec4& rhs)
	{
		v -= rhs.v; x -= rhs.x; y -= rhs.y; z -= rhs.z;
		return *this;
	}

	//friended operators
	friend inline const bool operator ==(const vec4& lhs, const vec4& rhs)
	{
		if (lhs.v == rhs.v && lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z)
			return true;
		else return false;
	}
	friend inline const bool operator !=(const vec4& lhs, const vec4& rhs)
	{
		if (lhs.v != rhs.v || lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z)
			return true;
		else return false;
	}

	friend inline vec4 operator *(const vec4& lhs, const float rhs)
	{
		return { static_cast<T>(lhs.v * rhs), static_cast<T>(lhs.x * rhs), static_cast<T>(lhs.y * rhs), static_cast<T>(lhs.z * rhs) };
	}
	friend inline vec4 operator *(const vec4& lhs, const vec4& rhs)
	{
		return { lhs.v * rhs.v, lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
	}
	friend inline vec4 operator /(const vec4& lhs, const float rhs)
	{
		return { static_cast<T>(lhs.v / rhs), static_cast<T>(lhs.x / rhs), static_cast<T>(lhs.y / rhs), static_cast<T>(lhs.z / rhs) };
	}
	friend inline vec4 operator /(const vec4& lhs, const vec4& rhs)
	{
		return { lhs.v / rhs.v, lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
	}
	friend inline vec4 operator +(const vec4& lhs, const T rhs)
	{
		return { lhs.v + rhs, lhs.x + rhs, lhs.y + rhs, lhs.z + rhs };
	}
	friend inline vec4 operator +(const vec4& lhs, const vec4& rhs)
	{
		return { lhs.v + rhs.v, lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}
	friend inline vec4 operator -(const vec4& lhs, const T rhs)
	{
		return { lhs.v - rhs, lhs.x - rhs, lhs.y - rhs, lhs.z - rhs };
	}
	friend inline vec4 operator -(const vec4& lhs, const vec4& rhs)
	{
		return { lhs.v - rhs.v, lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	//T value is on the right side
	friend inline void operator /(const float lhs, const vec4& rhs)
	{
		//solely exists so that we don't do it anyways
		printf("Undefined behaviour: dividing a float by a vec4!\n");
		return;
	}
	friend inline vec4 operator *(const float lhs, const vec4& rhs)
	{
		return { static_cast<T>(lhs * rhs.v), static_cast<T>(lhs * rhs.x), static_cast<T>(lhs * rhs.y), static_cast<T>(lhs * rhs.z) };
	}
	friend inline vec4 operator +(const T lhs, const vec4& rhs)
	{
		return { lhs + rhs.v, lhs + rhs.x, lhs + rhs.y, lhs + rhs.z };
	}
	friend inline vec4 operator -(const T lhs, const vec4& rhs)
	{
		return { lhs - rhs.v, lhs - rhs.x, lhs - rhs.y, lhs - rhs.z };
	}
};
#pragma endregion

//conversion macro vec2
#define vec2convert(type, vec) vec2<type>{static_cast<type>(vec.x), static_cast<type>(vec.y)}
//conversion macro vec3
#define vec3convert(type, vec) vec3<type>{static_cast<type>(vec.x), static_cast<type>(vec.y), static_cast<type>(vec.z)}
//conversion macro vec4
#define vec4convert(type, vec) vec4<type>{static_cast<type>(vec.v), static_cast<type>(vec.x), static_cast<type>(vec.y), static_cast<type>(vec.z)}