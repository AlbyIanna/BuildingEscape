// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (!InputComponent) {
		UE_LOG(LogTemp, Error, TEXT("%s has no component InputComponent!"), *GetOwner()->GetName())
		return;
	}
	InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
	InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
}

 void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("%s missing PhysicsHandle component"), *GetOwner()->GetName())
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent()) {
		PhysicsHandle->SetTargetLocation(GetReachLineStruct().End);
	}
}

void UGrabber::Grab() {
	// Try and reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	if (ActorHit && ComponentToGrab && PhysicsHandle) {
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None, // No bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // Allow rotation of grabbed component
		);
	}
}

void UGrabber::Release() {
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent()) {
		PhysicsHandle->ReleaseComponent();
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach() {
	ReachLineStruct ReachLine = GetReachLineStruct();
	
	// Uncomment to draw debug line
	// DrawDebugLine(GetWorld(), ReachLine.Start, ReachLine.End, FColor(255, 0, 0), false, 0.f, 0.f, 10.f);
	
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		ReachLine.Start,
		ReachLine.End,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	return HitResult;
}

const ReachLineStruct UGrabber::GetReachLineStruct() {
	ReachLineStruct ReachLine = ReachLineStruct({FVector(), FVector()});
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT ReachLine.Start, OUT PlayerViewPointRotation);
	ReachLine.End = ReachLine.Start + PlayerViewPointRotation.Vector() * Reach;
	return ReachLine;
}