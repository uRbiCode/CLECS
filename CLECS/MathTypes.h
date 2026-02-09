#pragma once
#include <type_traits>
#include <cmath>

template<typename T>
requires std::is_arithmetic_v<T>
struct Vector2D
{
    T X;
    T Y;

    constexpr Vector2D() : X(T{}), Y(T{}) {}
    constexpr Vector2D(T x, T y) : X(x), Y(y) {}

    constexpr Vector2D operator+(const Vector2D& other) const
    {
        return Vector2D(X + other.X, Y + other.Y);
    }

    constexpr Vector2D operator-(const Vector2D& other) const
    {
        return Vector2D(X - other.X, Y - other.Y);
    }

    constexpr Vector2D operator-() const
    {
        return Vector2D(-X, -Y);
    }

    constexpr Vector2D operator*(T scalar) const
    {
        return Vector2D(X * scalar, Y * scalar);
    }

    constexpr Vector2D operator/(T scalar) const
    {
        return Vector2D(X / scalar, Y / scalar);
    }

    constexpr Vector2D& operator+=(const Vector2D& other)
    {
        X += other.X;
        Y += other.Y;
        return *this;
    }

    constexpr Vector2D& operator-=(const Vector2D& other)
    {
        X -= other.X;
        Y -= other.Y;
        return *this;
    }

    constexpr Vector2D& operator*=(T scalar)
    {
        X *= scalar;
        Y *= scalar;
        return *this;
    }

    constexpr Vector2D& operator/=(T scalar)
    {
        X /= scalar;
        Y /= scalar;
        return *this;
    }

    constexpr bool operator==(const Vector2D& other) const
    {
        return X == other.X && Y == other.Y;
    }

    constexpr bool operator!=(const Vector2D& other) const
    {
        return !(*this == other);
    }

    // Get length/magnitude of vector
    T Length() const
    {
        return std::sqrt(X * X + Y * Y);
    }

    // Get squared length (faster, no sqrt)
    constexpr T LengthSquared() const
    {
        return X * X + Y * Y;
    }

    // Get normalized vector (unit length)
    Vector2D Normalized() const
    {
        T len = Length();
        if (len > T{})
        {
            return Vector2D(X / len, Y / len);
        }
        return Vector2D{};
    }
};