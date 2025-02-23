use alloc::string::String;
use core::ops::Sub;
use serde::{Deserialize, Serialize};
use libm::{sqrtf, atan2f};

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct Vector2D {
    pub x: f32,
    pub y: f32,
}

impl Sub for &Vector2D {
    type Output = Vector2D;

    fn sub(self, rhs: Self) -> Self::Output {
        Vector2D {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
        }
    }
}

impl Vector2D {
    pub fn length(&self) -> f32 {
        sqrtf(self.x * self.x + self.y * self.y)
    }

    pub fn normalize(&self) -> Vector2D {
        let length = self.length();
        if length == 0.0 {
            return Vector2D::default();
        }
        Vector2D {
            x: self.x / length,
            y: self.y / length,
        }
    }

    pub fn yaw(&self) -> f32 {
        atan2f(self.y, self.x) * (180.0 / core::f32::consts::PI) - 90.0
    }
}

pub fn fix_yaw(yaw: f32) -> f32 {
    if yaw < -180.0 {
        yaw + 360.0
    } else if yaw > 180.0 {
        yaw - 360.0
    } else {
        yaw
    }
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct Transform2D {
    pub pos: Vector2D,
    pub yaw: f32,
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct MotionState {
    // locomotion混合树参数
    pub locomotion_speed: f32,
    // 蒙太奇动画
    pub montage_id: i32,
    pub montage_progress: f32,
}