#pragma once

/* intŒ^Vector2  */
class Vector2
{
public:

	int x; // XÀ•W
	int y; // YÀ•W

	/// @brief ƒfƒXƒgƒ‰ƒNƒ^
	~Vector2(void) = default;

	/// @brief ‘ã“üˆ—
	Vector2 operator=(const Vector2& _vec);

	/// @brief ‰ÁZˆ—
	Vector2 operator+(const Vector2& _vec)const;
	void operator+=(const Vector2& _vec);

	/// @brief Œ¸Zˆ—
	Vector2 operator-(const Vector2& _vec)const;
	void operator-=(const Vector2& _vec);

	/// @brief æZˆ—
	Vector2 operator*(const Vector2& _vec)const;
	void operator*=(const Vector2& _vec);
	void operator*=(int _value);
	void operator*=(float _value);

	/// @brief œZˆ—
	Vector2 operator/(const Vector2& _vec)const;
	void operator/=(const Vector2& _vec);
	void operator/=(int _value);
};


/* floatŒ^Vector2  */
class Vector2F
{
public:

	float x; // XÀ•W
	float y; // YÀ•W

	/// @brief ƒfƒXƒgƒ‰ƒNƒ^
	~Vector2F(void) = default;

	/// @brief ‘ã“üˆ—
	Vector2F operator=(const Vector2F& _vec);

	/// @brief ‰ÁZˆ—
	Vector2F operator+(const Vector2F& _vec)const;
	void operator+=(const Vector2F& _vec);
	void operator+=(float _value);

	/// @brief Œ¸Zˆ—
	Vector2F operator-(const Vector2F& _vec)const;
	void operator-=(const Vector2F& _vec);
	void operator-=(float _value);

	/// @brief æZˆ—
	Vector2F operator*(const Vector2F& _vec)const;
	void operator*=(const Vector2F& _vec);
	void operator*=(float _value);

	/// @brief œZˆ—
	Vector2F operator/(const Vector2F& _vec)const;
	void operator/=(const Vector2F& _vec);
	void operator/=(float _value);
};