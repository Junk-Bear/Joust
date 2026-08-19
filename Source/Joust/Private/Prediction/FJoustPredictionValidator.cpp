// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustPredictionValidator.h"
#include "Prediction/JoustPredictionTypes.h"

bool FJoustPredictionValidator::ValidateInitial(const FJoustPredictionCircle& Circle, const FVector2D& TargetPoint, const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax)
{
    if (!ValidateLanceBox(LanceBoxMin, LanceBoxMax) || !IsPointInLanceBox(TargetPoint, LanceBoxMin, LanceBoxMax) ||
        !ValidateCircle(Circle, LanceBoxMin, LanceBoxMax))
        return false;

    if (Circle.Radius <= 0.0f)
        return false;

    return ContainsTargetPoint(Circle, TargetPoint);
}

bool FJoustPredictionValidator::ValidateNext(const FJoustPredictionCircle& CurrentCircle, const FJoustPredictionCircle& NextCircle, const FVector2D& TargetPoint, const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax)
{
    if (!ValidateLanceBox(LanceBoxMin, LanceBoxMax) ||
        !IsPointInLanceBox(TargetPoint, LanceBoxMin, LanceBoxMax) ||
        !ValidateCircle(CurrentCircle, LanceBoxMin, LanceBoxMax) ||
        !ValidateCircle(NextCircle, LanceBoxMin, LanceBoxMax))
        return false;

    if (CurrentCircle.Radius <= 0.0f || NextCircle.Radius >= CurrentCircle.Radius)
        return false;

    if (!ContainsTargetPoint(CurrentCircle, TargetPoint) ||
        !ContainsTargetPoint(NextCircle, TargetPoint))
        return false;

    return ContainsCircle(CurrentCircle, NextCircle);
}

bool FJoustPredictionValidator::ValidateLanceBox(const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax)
{
    if (!FMath::IsFinite(LanceBoxMin.X) || !FMath::IsFinite(LanceBoxMin.Y) ||
        !FMath::IsFinite(LanceBoxMax.X) || !FMath::IsFinite(LanceBoxMax.Y))
        return false;

    return LanceBoxMin.X <= LanceBoxMax.X && LanceBoxMin.Y <= LanceBoxMax.Y;
}

bool FJoustPredictionValidator::IsPointInLanceBox(const FVector2D& Point, const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax)
{
    if (!FMath::IsFinite(Point.X) || !FMath::IsFinite(Point.Y))
        return false;

    return Point.X >= LanceBoxMin.X && Point.X <= LanceBoxMax.X &&
        Point.Y >= LanceBoxMin.Y && Point.Y <= LanceBoxMax.Y;
}

bool FJoustPredictionValidator::ValidateCircle(const FJoustPredictionCircle& Circle, const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax)
{
    if (!FMath::IsFinite(Circle.Radius) || Circle.Radius < 0.0f)
        return false;

    return IsPointInLanceBox(Circle.Center, LanceBoxMin, LanceBoxMax);
}

bool FJoustPredictionValidator::ContainsTargetPoint(const FJoustPredictionCircle& Circle, const FVector2D& TargetPoint)
{
    if (Circle.Radius == 0.0f)
    {
        return Circle.Center == TargetPoint;
    }

    return (Circle.Center - TargetPoint).SizeSquared() <= FMath::Square(Circle.Radius + KINDA_SMALL_NUMBER);
}

bool FJoustPredictionValidator::ContainsCircle(const FJoustPredictionCircle& OuterCircle, const FJoustPredictionCircle& InnerCircle)
{
    if (InnerCircle.Radius > OuterCircle.Radius)
        return false;

    return (OuterCircle.Center - InnerCircle.Center).SizeSquared() <= FMath::Square(OuterCircle.Radius - InnerCircle.Radius + KINDA_SMALL_NUMBER);
}
