use alloc::string::String;
use alloc::vec::Vec;
use serde::{Deserialize, Serialize};
use crate::data::protocol::{MotionState, Transform2D};

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct RenderCharacterData {
    pub id: i32,
    pub transform: Transform2D,
    pub motion_state: MotionState,
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct RenderData {
    pub generation: i32,
    pub character0: RenderCharacterData,
}