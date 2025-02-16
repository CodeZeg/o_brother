use crate::data::protocol::{fix_yaw, Vector2D};
use libm::expf;

pub struct Damper {
    damping: f32,
}

impl Damper {
    pub fn new(response_time: f32) -> Self {
        Damper {
            damping: 1.0 / response_time,
        }
    }
    pub fn update_movement_1d(
        &self,
        current_position: f32,
        current_velocity: f32,
        target_velocity: f32,
        delta_time: f32
    ) -> (f32, f32) {
        // 速度差
        let velocity_diff = target_velocity - current_velocity;

        // 计算新速度
        let new_velocity = current_velocity + velocity_diff * (1.0 - expf(-self.damping * delta_time));

        // 使用平均速度计算位置变化
        let avg_velocity = (current_velocity + new_velocity) * 0.5;
        let new_position = current_position + avg_velocity * delta_time;

        (new_position, new_velocity)
    }

    pub fn update_movement_2d(
        &self,
        current_position: &Vector2D,
        current_velocity: &Vector2D,
        target_velocity: &Vector2D,
        delta_time: f32
    ) -> (Vector2D, Vector2D) {
        let (new_x, new_vx) = self.update_movement_1d(current_position.x, current_velocity.x, target_velocity.x, delta_time);
        let (new_y, new_vy) = self.update_movement_1d(current_position.y, current_velocity.y, target_velocity.y, delta_time);
        (Vector2D {
            x: new_x,
            y: new_y,
        }, Vector2D {
            x: new_vx,
            y: new_vy,
        })
    }

    pub fn update_yaw(
        &self,
        current_yaw: f32,
        target_yaw: f32,
        delta_time: f32
    ) -> f32 {
        fix_yaw(current_yaw + fix_yaw(target_yaw - current_yaw) * (1.0 - expf(-self.damping * delta_time)))
    }
}
