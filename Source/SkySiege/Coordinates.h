// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Coordinates.generated.h"


USTRUCT(BlueprintType)
struct FCoordinates
{
	GENERATED_BODY()

	FCoordinates() {};
	
	FCoordinates(int32 InRow, int32 InCol)
		: Row(InRow)
		, Col(InCol)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	int32 Row = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Col = -1;

	bool operator==(const FCoordinates& other) const
	{
		return Row == other.Row && Col == other.Col;
	}

	// Cantor's enumeration of pairs
	// https://stackoverflow.com/questions/682438/hash-function-providing-unique-uint-from-an-integer-coordinate-pair
	friend uint32 GetTypeHash(const FCoordinates& other)
	{
		int32 r = other.Row;
		int32 c = other.Col;
		return ((r + c) * (r+c+1) / 2) + c;
	}
};
