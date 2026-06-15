#ifndef CTRL_DUALARM_LIB_SERVO_H
#define CTRL_DUALARM_LIB_SERVO_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SERVO_COUNT 9
#define SERVO_DEV_PATH_MAX 256

/* Managed serial connection to the ESP32 servo controller. */
typedef struct {
  int fd; /* < 0 when the device is not currently open */
  char dev_path[SERVO_DEV_PATH_MAX];
} servo_handle_t;

/*
 * Open the device. The handle is always returned (unless out of memory); if the
 * device could not be opened, fd < 0 and servo_try_reopen() can retry later.
 */
servo_handle_t* servo_open(const char* dev_path);

/* Reopen if currently closed. Returns true when the device is open. */
bool servo_try_reopen(servo_handle_t* handle);

/* Send all 8 target angles in degrees as "S a0 a1 .. a7\n". */
bool servo_send_all(servo_handle_t* handle, const double angles_deg[SERVO_COUNT]);

/* Send a single "<id> <angle>\n" command. */
bool servo_send_one(servo_handle_t* handle, int id, double angle_deg);

/* Close the device and free the handle. */
void servo_close(servo_handle_t* handle);

#ifdef __cplusplus
}
#endif

#endif  /* CTRL_DUALARM_LIB_SERVO_H */
