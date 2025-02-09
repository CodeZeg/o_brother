#pragma once

#include "CoreMinimal.h"

// 基础数据结构
struct FGPPosition {
	float x;
	float y;

	FGPPosition() : x(0.0f), y(0.0f) {}
};

struct FGPRotation {
	float yaw;

	FGPRotation() : yaw(0.0f) {}
};

struct FGPTransform2D {
	FGPPosition position;
	FGPRotation rotation;
};

struct FGPMotionState {
	float locomotion_speed;
	float montage_progress;

	FGPMotionState() 
		: locomotion_speed(0.0f)
		, montage_progress(0.0f) 
	{}
};

struct FGPRenderCharacterData {
	int32 id;
	FGPTransform2D transform;
	FGPMotionState motion_state;
};

struct FGPRenderData {
	int32 generation;
	FGPRenderCharacterData character0;
};