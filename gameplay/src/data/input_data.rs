use alloc::vec::Vec;
use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct FightingInputStateData {
    pub x: f32,           // x轴方向的输入
    pub y: f32,           // y轴方向的输入
}

// 输入状态数据, 每帧都存在，服务端直接用最新的就好
#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub enum InputStateData {
    Fighting(FightingInputStateData),
    #[default]
    Other,
}

// 输入动作数据, 每帧最多一个, 服务端不可丢弃
#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub enum InputActionData {
    #[default]
    None,
    ChoseCard(u8),
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct InputPlayerData {
    pub state: InputStateData,
    pub action: InputActionData,
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct InputData {
    pub player_0: InputPlayerData,
}