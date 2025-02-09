use alloc::string::String;
use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Position {
    pub x: f32,
    pub y: f32,
}

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Rotation {
    pub yaw: f32,
}

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Transform2D {
    pub position: Position,
    pub rotation: Rotation,
}

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct MotionState {
    // locomotion混合树参数
    pub locomotion_speed: f32,
    // 蒙太奇动画
    pub montage_id: i32,
    pub montage_progress: f32,
}