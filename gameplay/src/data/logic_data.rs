use alloc::vec::Vec;
use serde::{Deserialize, Serialize};
use crate::data::protocol::{MotionState, Transform2D, Vector2D};

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct MoveController {
    pub cur_vel: Vector2D,
    pub tar_vel: Vector2D,
    pub tar_yaw: f32,
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct LogicCharacterData {
    pub id: i32,
    pub transform: Transform2D,
    pub move_controller: MoveController,
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct LogicData {
    pub character0: LogicCharacterData,
    pub monsters: Vec<LogicCharacterData>,
}