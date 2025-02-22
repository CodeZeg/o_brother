#pragma once

#include "CoreMinimal.h"
#include "render_data_generated.h"

inline FVector GetPosition(const GPVec2D& pos)
{
	return FVector(pos.x(), pos.y(), 0.0f);
}

inline FVector GetPosition(const GPTrans2D* trans)
{
	return GetPosition(trans->pos());
}

inline FRotator YawToRotator(const float yaw)
{
	return FRotator(0.0f, yaw, 0.0f);
}

inline FRotator GetRotation(const GPTrans2D* trans)
{
	return YawToRotator(trans->yaw());
}

// 基础数据结构
struct FGPVector2D {
	float x;
	float y;
	
	FVector ToFVector() const
	{
		return FVector(x, y, 0.0f);
	}
};

struct FGPRotation {
	float yaw;

	FGPRotation() : yaw(0.0f) {}
	
	FRotator ToFRotator() const
	{
		return FRotator(0.0f, yaw, 0.0f);
	}
	
	operator FRotator() const
	{
		return ToFRotator();
	}
};

struct FGPTransform2D {
	FGPVector2D pos;
	float yaw;

	FVector GetPosition() const
	{
		return pos.ToFVector();
	}
	
	FRotator GetRotation() const
	{
		return FRotator(0.0f, yaw, 0.0f);
	}
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