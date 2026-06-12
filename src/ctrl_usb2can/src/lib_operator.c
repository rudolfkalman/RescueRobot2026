#include "lib_operator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>

int op_init(const char *device_path) {
    int fd = open(device_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        return -1;
    }

    struct termios tio;
    if (tcgetattr(fd, &tio) != 0) {
        close(fd);
        return -1;
    }

    cfmakeraw(&tio);
    tio.c_cflag |= CLOCAL | CREAD;
    tio.c_cflag &= ~CRTSCTS;
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tio) != 0) {
        close(fd);
        return -1;
    }

    tcflush(fd, TCIOFLUSH);
    return fd;
}

void op_close(int fd) {
    if (fd >= 0) {
        close(fd);
    }
}

int op_send_raw(int fd, uint32_t can_id, int32_t value) {
    if (fd < 0) return -1;

    uint8_t packet[8];
    packet[0] = (uint8_t)(can_id >> 24);
    packet[1] = (uint8_t)(can_id >> 16);
    packet[2] = (uint8_t)(can_id >> 8);
    packet[3] = (uint8_t)can_id;
    packet[4] = (uint8_t)((uint32_t)value >> 24);
    packet[5] = (uint8_t)((uint32_t)value >> 16);
    packet[6] = (uint8_t)((uint32_t)value >> 8);
    packet[7] = (uint8_t)value;

    ssize_t ret = write(fd, packet, sizeof(packet));
    if (ret != (ssize_t)sizeof(packet)) {
        return -1;
    }
    return 0;
}

/* --- Managed API Implementation --- */

OpHandle* op_open(const char *device_path) {
    OpHandle *handle = (OpHandle*)malloc(sizeof(OpHandle));
    if (!handle) return NULL;

    strncpy(handle->device_path, device_path, sizeof(handle->device_path) - 1);
    handle->device_path[sizeof(handle->device_path) - 1] = '\0';
    handle->fd = op_init(handle->device_path);
    
    return handle;
}

void op_close_handle(OpHandle *handle) {
    if (!handle) return;
    op_close(handle->fd);
    free(handle);
}

static int _ensure_connection(OpHandle *handle) {
    if (handle->fd >= 0) return 0;

    handle->fd = op_init(handle->device_path);
    if (handle->fd >= 0) return 0;

    return -1;
}

int op_send_duty_auto(OpHandle *handle, uint32_t can_id, float duty) {
    if (!handle) return -1;
    if (_ensure_connection(handle) < 0) return -1;

    if (duty > 100.0f) duty = 100.0f;
    if (duty < -100.0f) duty = -100.0f;

    int ret = op_send_raw(handle->fd, can_id, (int32_t)duty);
    if (ret < 0) {
        op_close(handle->fd);
        handle->fd = -1;
    }
    return ret;
}

int op_send_direction_auto(OpHandle *handle, uint32_t can_id, int8_t direction) {
    if (!handle) return -1;
    if (_ensure_connection(handle) < 0) return -1;

    if (direction > 1) direction = 1;
    if (direction < -1) direction = -1;

    int ret = op_send_raw(handle->fd, can_id, (int32_t)direction);
    if (ret < 0) {
        op_close(handle->fd);
        handle->fd = -1;
    }
    return ret;
}

int op_send_target_angle_auto(OpHandle *handle, uint32_t can_id, float angle_rad) {
    if (!handle) return -1;
    if (_ensure_connection(handle) < 0) return -1;

    int32_t value = (int32_t)(angle_rad * 1000.0f);
    int ret = op_send_raw(handle->fd, can_id, value);
    if (ret < 0) {
        op_close(handle->fd);
        handle->fd = -1;
    }
    return ret;
}

/* --- Legacy API --- */
int op_send_duty(int fd, uint32_t can_id, float duty) {
    if (duty > 100.0f) duty = 100.0f;
    if (duty < -100.0f) duty = -100.0f;
    return op_send_raw(fd, can_id, (int32_t)duty);
}

int op_send_direction(int fd, uint32_t can_id, int8_t direction) {
    if (direction > 1) direction = 1;
    if (direction < -1) direction = -1;
    return op_send_raw(fd, can_id, (int32_t)direction);
}

int op_send_target_angle(int fd, uint32_t can_id, float angle_rad) {
    int32_t value = (int32_t)(angle_rad * 1000.0f);
    return op_send_raw(fd, can_id, value);
}
