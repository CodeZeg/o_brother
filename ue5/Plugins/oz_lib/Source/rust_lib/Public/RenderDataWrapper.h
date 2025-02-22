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