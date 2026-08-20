// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/JoustPredictionSeriesController.h"
#include "Prediction/JoustPredictionInterpolation.h"

UJoustPredictionSeriesController::UJoustPredictionSeriesController() : FTickableGameObject(ETickableTickType::Never)
{}

void UJoustPredictionSeriesController::Initialize()
{
	if (Interpolation == nullptr)
	{
		Interpolation = NewObject<UJoustPredictionInterpolation>(this);
	}
}

bool UJoustPredictionSeriesController::StartPlayback(
	const FJoustPredictionSettings & InSettings, const FJoustPredictionSeries & InRealSeries, const TArray<FJoustPredictionSeries>&InFakeSeries)
{
	if (bIsPlaying || Interpolation == nullptr || !ValidatePlaybackData(InSettings, InRealSeries, InFakeSeries))
		return false;

	ResetPlaybackData();

	Settings = InSettings;
	RealSeries = InRealSeries;
	FakeSeries = InFakeSeries;

	InitializeDisplayCircles();

	bIsPlaying = true;

	SetTickableTickType(ETickableTickType::Always);

	return true;
}

void UJoustPredictionSeriesController::StopPlayback()
{
	SetTickableTickType(ETickableTickType::Never);

	ResetPlaybackData();
}

void UJoustPredictionSeriesController::BuildPredictionState(FJoustPredictionState& OutState) const
{
	OutState = FJoustPredictionState{};

	OutState.CurrentStage = CurrentStageIdx;

	OutState.TransitionAlpha = CurrentAlpha;

	if (bIsPlaying)
	{
		OutState.bIsPredictionVisible = true;

		OutState.DisplayCircles.Reserve(1 + FakeDisplayCircles.Num());

		OutState.DisplayCircles.Add(RealDisplayCircle);

		OutState.DisplayCircles.Append(FakeDisplayCircles);

		OutState.DisplayCircles.Sort([](const FJoustPredictionDisplayCircle& A, const FJoustPredictionDisplayCircle& B)
			{
				if (A.Center.X != B.Center.X)
				{
					return A.Center.X < B.Center.X;
				}

				return A.Center.Y < B.Center.Y;
			});

		return;
	}

	if (bCompleted && !RealSeries.Circles.IsEmpty())
	{
		OutState.bIsAttackPointRevealed = true;

		OutState.RevealedAttackPoint = RealSeries.Circles.Last().Center;
	}
}

void UJoustPredictionSeriesController::BeginDestroy()
{
	SetTickableTickType(ETickableTickType::Never);

	Super::BeginDestroy();
}

void UJoustPredictionSeriesController::Tick(float DeltaTime)
{
	if (!bIsPlaying || !FMath::IsFinite(DeltaTime) || DeltaTime <= 0.0f)
		return;

	float RemainingTime = DeltaTime;

	while (bIsPlaying && RemainingTime > 0.0f)
	{
		const float StageDuration = Settings.StageDurations[CurrentStageIdx];

		const float TimeToStageEnd = StageDuration - StageElapsedTime;

		const float ConsumedTime = FMath::Min(RemainingTime, TimeToStageEnd);

		StageElapsedTime += ConsumedTime;
		RemainingTime -= ConsumedTime;

		if (!UpdateDisplayCircles())
		{
			StopPlayback();

			return;
		}

		if (StageElapsedTime < StageDuration)
			break;

		AdvanceStage();
	}
}

TStatId UJoustPredictionSeriesController::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT( UJoustPredictionSeriesController, STATGROUP_Tickables);
}

UWorld* UJoustPredictionSeriesController::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

bool UJoustPredictionSeriesController::ValidatePlaybackData(
	const FJoustPredictionSettings& InSettings, const FJoustPredictionSeries& InRealSeries, const TArray<FJoustPredictionSeries>& InFakeSeries) const
{
	if (InSettings.StageCount < 2 ||
		InSettings.StageDurations.Num() != InSettings.StageCount - 1 ||
		InRealSeries.Circles.Num() != InSettings.StageCount ||
		InFakeSeries.Num() != InSettings.FakeCircleCount)
		return false;

	for (float Item : InSettings.StageDurations)
	{
		if (!FMath::IsFinite(Item) || Item <= 0.0f)
			return false;
	}

	for (const FJoustPredictionSeries& Item : InFakeSeries)
	{
		if (Item.Circles.Num() != InSettings.StageCount)
			return false;
	}

	if (!InFakeSeries.IsEmpty() &&
		(InSettings.FakeRemoveStage <= 0 || InSettings.FakeRemoveStage >= InSettings.StageCount))
		return false;

	return true;
}

void UJoustPredictionSeriesController::InitializeDisplayCircles()
{
	CurrentStageIdx = 0;
	StageElapsedTime = 0.0f;
	CurrentAlpha = 0.0f;

	RealDisplayCircle.Center = RealSeries.Circles[0].Center;

	RealDisplayCircle.Radius = RealSeries.Circles[0].Radius;

	FakeDisplayCircles.Reset();

	if (FakeSeries.IsEmpty())
		return;

	FakeDisplayCircles.Reserve(FakeSeries.Num());

	for (const FJoustPredictionSeries& Item : FakeSeries)
	{
		FJoustPredictionDisplayCircle DisplayCircle;

		DisplayCircle.Center = Item.Circles[0].Center;
		DisplayCircle.Radius = Item.Circles[0].Radius;

		FakeDisplayCircles.Add(DisplayCircle);
	}
}

bool UJoustPredictionSeriesController::UpdateDisplayCircles()
{
	if (CurrentStageIdx < 0 || (CurrentStageIdx >= Settings.StageCount - 1))
		return false;

	if (!Interpolation->Interpolate(
		RealSeries.Circles[CurrentStageIdx],
		RealSeries.Circles[CurrentStageIdx + 1],
		StageElapsedTime,
		Settings.StageDurations[CurrentStageIdx],
		RealDisplayCircle,
		CurrentAlpha))
		return false;

	if (FakeSeries.IsEmpty() || (CurrentStageIdx + 1 >= Settings.FakeRemoveStage))
	{
		FakeDisplayCircles.Reset();

		return true;
	}

	FakeDisplayCircles.SetNum(FakeSeries.Num());

	for (int32 i = 0; i < FakeSeries.Num(); ++i)
	{
		if (!Interpolation->Interpolate(
			FakeSeries[i].Circles[CurrentStageIdx],
			FakeSeries[i].Circles[CurrentStageIdx + 1],
			StageElapsedTime,
			Settings.StageDurations[CurrentStageIdx],
			FakeDisplayCircles[i],
			CurrentAlpha))
			return false;
	}

	return true;
}

void UJoustPredictionSeriesController::AdvanceStage()
{

	StageCompletedEvent.Broadcast(CurrentStageIdx);

	if (!bIsPlaying)
		return;

	++CurrentStageIdx;

	StageElapsedTime = 0.0f;
	CurrentAlpha = 0.0f;

	if (CurrentStageIdx >= Settings.StageCount - 1)
	{
		CompletePlayback();
	}
}

void UJoustPredictionSeriesController::CompletePlayback()
{
	bIsPlaying = false;
	bCompleted = true;

	CurrentStageIdx = Settings.StageCount - 1;

	CurrentAlpha = 1.0f;

	FakeDisplayCircles.Reset();

	SetTickableTickType(ETickableTickType::Never);

	PlaybackCompletedEvent.Broadcast();
}

void UJoustPredictionSeriesController::ResetPlaybackData()
{
	Settings = FJoustPredictionSettings{};

	RealSeries = FJoustPredictionSeries{};

	FakeSeries.Reset();

	RealDisplayCircle = FJoustPredictionDisplayCircle{};

	FakeDisplayCircles.Reset();

	CurrentStageIdx = 0;
	StageElapsedTime = 0.0f;
	CurrentAlpha = 0.0f;

	bIsPlaying = false;
	bCompleted = false;

}
