// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_Trace.generated.h"

class UEnvQueryContext;

UCLASS(MinimalAPI)
class UEnvQueryTest_Trace : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()

	/** trace data */
	UPROPERTY(EditDefaultsOnly, Category=Trace)
	FEnvTraceData TraceData;

	/** trace direction */
	UPROPERTY(EditDefaultsOnly, Category=Trace)
	FAIDataProviderBoolValue TraceFromContext;

	/** Z offset from item */
	UPROPERTY(EditDefaultsOnly, Category=Trace, AdvancedDisplay)
	FAIDataProviderFloatValue ItemHeightOffset;

	/** Z offset from querier */
	UPROPERTY(EditDefaultsOnly, Category=Trace, AdvancedDisplay)
	FAIDataProviderFloatValue ContextHeightOffset;

	/** context: other end of trace test */
	UPROPERTY(EditDefaultsOnly, Category=Trace)
	TSubclassOf<UEnvQueryContext> Context;

	// BEGIN: deprecated properties 
	UPROPERTY()
	FEnvBoolParam TraceToItem;

	UPROPERTY()
	FEnvFloatParam ItemOffsetZ;

	UPROPERTY()
	FEnvFloatParam ContextOffsetZ;
	// END: deprecated properties

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	virtual void PostLoad() override;

	virtual FString GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;

protected:

	DECLARE_DELEGATE_RetVal_SevenParams(bool, FRunTraceSignature, const FVector&, const FVector&, AActor*, UWorld*, enum ECollisionChannel, const FCollisionQueryParams&, const FVector&);

	bool RunLineTraceTo(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params, const FVector& Extent);
	bool RunLineTraceFrom(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params, const FVector& Extent);
	bool RunBoxTraceTo(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params, const FVector& Extent);
	bool RunBoxTraceFrom(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params, const FVector& Extent);
	bool RunSphereTraceTo(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params, const FVector& Extent);
	bool RunSphereTraceFrom(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params, const FVector& Extent);
	bool RunCapsuleTraceTo(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params, const FVector& Extent);
	bool RunCapsuleTraceFrom(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params, const FVector& Extent);
};
