#ifndef LIB_OPERATOR_H
#define LIB_OPERATOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handle for a managed USB2CAN connection.
 */
typedef struct {
    char device_path[256];
    int fd;
} OpHandle;

/**
 * @brief Open a managed connection.
 * @param device_path Initial path (e.g., "/dev/ttyACM0").
 * @return Pointer to OpHandle.
 */
OpHandle* op_open(const char *device_path);

/**
 * @brief Close a managed connection and free memory.
 * @param handle The OpHandle to close.
 */
void op_close_handle(OpHandle *handle);

/**
 * @brief Send motor duty cycle with automatic reconnection.
 */
int op_send_duty_auto(OpHandle *handle, uint32_t can_id, float duty);

/**
 * @brief Send movement direction with automatic reconnection.
 */
int op_send_direction_auto(OpHandle *handle, uint32_t can_id, int8_t direction);

/**
 * @brief Send target angle with automatic reconnection.
 */
int op_send_target_angle_auto(OpHandle *handle, uint32_t can_id, float angle_rad);


/* --- Legacy/Internal Low-Level API --- */

int op_init(const char *device_path);
void op_close(int fd);
int op_send_raw(int fd, uint32_t can_id, int32_t value);
int op_send_duty(int fd, uint32_t can_id, float duty);
int op_send_direction(int fd, uint32_t can_id, int8_t direction);
int op_send_target_angle(int fd, uint32_t can_id, float angle_rad);

#ifdef __cplusplus
}
#endif

#endif // LIB_OPERATOR_H
