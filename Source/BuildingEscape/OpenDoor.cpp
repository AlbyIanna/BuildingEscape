// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	if (!Owner) {
		UE_LOG(LogTemp, Error, TEXT("######## Reeeeeeeaaaaally weird!!! This Object doesn't have an Owner!!! #########"))
	}
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s missing PressurePlate component"), *Owner->GetName())
			return;
	}

}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetTotalMassOfActorsOnPlate() > TriggerMass) {
		OnOpen.Broadcast();
	} else {
		OnClose.Broadcast();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate() {
	float TotalMass = 0.f;
	
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) {
		return TotalMass;
	}
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (const auto& Actor: OverlappingActors) {
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		// UE_LOG(LogTemp, Warning, TEXT("%s is pressing, mass is %f"), *Actor->GetName(), Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass())
	}
	// UE_LOG(LogTemp, Warning, TEXT("Total mass is %f"), TotalMass)
	return TotalMass;
}