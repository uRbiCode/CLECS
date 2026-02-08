#pragma once
#include <type_traits>

template<typename T>
requires std::is_arithmetic_v<T>
struct Vector2D
{
    T X;
    T Y;

    // Constructors
    constexpr Vector2D() : X(T{}), Y(T{}) {}
    constexpr Vector2D(T x, T y) : X(x), Y(y) {}

    // Basic arithmetic operators
    constexpr Vector2D operator+(const Vector2D& other) const
    {
        return Vector2D(X + other.X, Y + other.Y);
    }

    constexpr Vector2D operator-(const Vector2D& other) const
    {
        return Vector2D(X - other.X, Y - other.Y);
    }

    constexpr Vector2D operator*(T scalar) const
    {
        return Vector2D(X * scalar, Y * scalar);
    }

    constexpr Vector2D operator/(T scalar) const
    {
        return Vector2D(X / scalar, Y / scalar);
    }

    // Compound assignment operators
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

    // Comparison operators
    constexpr bool operator==(const Vector2D& other) const
    {
        return X == other.X && Y == other.Y;
    }

    constexpr bool operator!=(const Vector2D& other) const
    {
        return !(*this == other);
    }
};