// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types.h"
#include "Defense_PrototypeCharacter.generated.h"

class UResourceComponent;
class ABuildGridManager;
class AWallActor;

UCLASS(Blueprintable)
class ADefense_PrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADefense_PrototypeCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resources")
	UResourceComponent* ResourceComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	bool bBuildMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	float GridCellSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	TSubclassOf<AWallActor> WallClass;

	UFUNCTION(BlueprintCallable)
	void ToggleBuildMode(bool bEnable);

	UFUNCTION(BlueprintCallable)
	bool TryPlaceWallAtCursor();

	UFUNCTION(BlueprintCallable)
	bool TryHarvestAtCursor();

	// 빌드 프리뷰 머티리얼(선택) 없으면 원래 메시 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	UMaterialInterface* BuildPreviewMaterial = nullptr;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	static bool TraceUnderCursor(APlayerController* PC, FHitResult& OutHit);
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	ABuildGridManager* FindGridManager() const;

	UPROPERTY()
	UStaticMeshComponent* BuildPreviewMesh = nullptr;
};

