// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustPredictionValidator.h"
#include "Prediction/JoustPredictionTypes.h"

bool FJoustPredictionValidator::ValidateInitial(const FJoustPredictionCircle& InCircle, const FVector2D& InTargetPoint, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
{
    if (!ValidateLanceBox(InLanceBoxMin, InLanceBoxMax) || !IsPointInLanceBox(InTargetPoint, InLanceBoxMin, InLanceBoxMax) ||
        !ValidateCircle(InCircle, InLanceBoxMin, InLanceBoxMax))
        return false;

    if (InCircle.Radius <= 0.0f)
        return false;

    return ContainsTargetPoint(InCircle, InTargetPoint);
}

bool FJoustPredictionValidator::ValidateNext(const FJoustPredictionCircle& InCurrentCircle, const FJoustPredictionCircle& InNextCircle, const FVector2D& InTargetPoint, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
{
    if (!ValidateLanceBox(InLanceBoxMin, InLanceBoxMax) ||
        !IsPointInLanceBox(InTargetPoint, InLanceBoxMin, InLanceBoxMax) ||
        !ValidateCircle(InCurrentCircle, InLanceBoxMin, InLanceBoxMax) ||
        !ValidateCircle(InNextCircle, InLanceBoxMin, InLanceBoxMax))
        return false;

    if (InCurrentCircle.Radius <= 0.0f || InNextCircle.Radius >= InCurrentCircle.Radius)
        return false;

    if (!ContainsTargetPoint(InCurrentCircle, InTargetPoint) ||
        !ContainsTargetPoint(InNextCircle, InTargetPoint))
        return false;

    return ContainsCircle(InCurrentCircle, InNextCircle);
}

bool FJoustPredictionValidator::ValidateLanceBox(const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
{
    if (!FMath::IsFinite(InLanceBoxMin.X) || !FMath::IsFinite(InLanceBoxMin.Y) ||
        !FMath::IsFinite(InLanceBoxMax.X) || !FMath::IsFinite(InLanceBoxMax.Y))
        return false;

    return InLanceBoxMin.X <= InLanceBoxMax.X && InLanceBoxMin.Y <= InLanceBoxMax.Y;
}

bool FJoustPredictionValidator::IsPointInLanceBox(const FVector2D& InPoint, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
{
    if (!FMath::IsFinite(InPoint.X) || !FMath::IsFinite(InPoint.Y))
        return false;

    return InPoint.X >= InLanceBoxMin.X && InPoint.X <= InLanceBoxMax.X &&
        InPoint.Y >= InLanceBoxMin.Y && InPoint.Y <= InLanceBoxMax.Y;
}

bool FJoustPredictionValidator::ValidateCircle(const FJoustPredictionCircle& InCircle, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
{
    if (!FMath::IsFinite(InCircle.Radius) || InCircle.Radius < 0.0f)
        return false;

    return IsPointInLanceBox(InCircle.Center, InLanceBoxMin, InLanceBoxMax);
}

bool FJoustPredictionValidator::ContainsTargetPoint(const FJoustPredictionCircle& InCircle, const FVector2D& InTargetPoint)
{
    if (InCircle.Radius == 0.0f)
    {
        return InCircle.Center == InTargetPoint;
    }

    return (InCircle.Center - InTargetPoint).SizeSquared() <= FMath::Square(InCircle.Radius + KINDA_SMALL_NUMBER);
}

bool FJoustPredictionValidator::ContainsCircle(const FJoustPredictionCircle& InOuterCircle, const FJoustPredictionCircle& InInnerCircle)
{
    if (InInnerCircle.Radius > InOuterCircle.Radius)
        return false;

    return (InOuterCircle.Center - InInnerCircle.Center).SizeSquared() <= FMath::Square(InOuterCircle.Radius - InInnerCircle.Radius + KINDA_SMALL_NUMBER);
}
