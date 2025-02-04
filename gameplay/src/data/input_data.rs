use alloc::vec::Vec;
use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FightingInputStateData {
    pub direction: f32,           // 移动方向
    pub magnitude: f32,           // 意向强度
}

// 输入状态数据, 每帧都存在，服务端直接用最新的就好
#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum InputStateData {
    Fighting(FightingInputStateData),
    Other,
}

// 输入动作数据, 每帧最多一个, 服务端不可丢弃
#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum InputActionData {
    None,
    ChoseCard(u8),
}

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct InputPlayerData {
    pub state: InputStateData,
    pub action: InputActionData,
}

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct InputData {
    pub player_0: InputPlayerData,
}