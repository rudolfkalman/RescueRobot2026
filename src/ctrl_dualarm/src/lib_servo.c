/* Enable termios extensions such as CRTSCTS under strict -std=c11. */
#define _DEFAULT_SOURCE

#include "lib_servo.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* Configure the tty as 115200 8N1, raw mode. Returns 0 on success. */
static int configure_port(int fd) {
  struct termios tio;
  memset(&tio, 0, sizeof(tio));
  if (tcgetattr(fd, &tio) != 0) {
    return -1;
  }

  cfsetispeed(&tio, B115200);
  cfsetospeed(&tio, B115200);

  tio.c_cflag &= ~CSIZE;
  tio.c_cflag |= CS8;
  tio.c_cflag |= (CLOCAL | CREAD);
  tio.c_cflag &= ~PARENB;
  tio.c_cflag &= ~CSTOPB;
  tio.c_cflag &= ~CRTSCTS;

  tio.c_iflag = 0; /* raw */
  tio.c_oflag = 0;
  tio.c_lflag = 0;

  tio.c_cc[VMIN] = 0;
  tio.c_cc[VTIME] = 1;

  tcflush(fd, TCIOFLUSH);
  return tcsetattr(fd, TCSANOW, &tio);
}

servo_handle_t* servo_open(const char* dev_path) {
  servo_handle_t* handle = (servo_handle_t*)calloc(1, sizeof(servo_handle_t));
  if (handle == NULL) {
    return NULL;
  }
  handle->fd = -1;
  strncpy(handle->dev_path, dev_path, sizeof(handle->dev_path) - 1);
  servo_try_reopen(handle);
  return handle;
}

bool servo_try_reopen(servo_handle_t* handle) {
  if (handle == NULL) {
    return false;
  }
  if (handle->fd >= 0) {
    return true;
  }

  int fd = open(handle->dev_path, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    return false;
  }
  if (configure_port(fd) != 0) {
    close(fd);
    return false;
  }
  handle->fd = fd;
  return true;
}

/* Write a line; on failure close the device so the next call can reconnect. */
static bool write_line(servo_handle_t* handle, const char* line, size_t len) {
  if (handle == NULL || handle->fd < 0) {
    return false;
  }
  ssize_t written = write(handle->fd, line, len);
  if (written < 0) {
    close(handle->fd);
    handle->fd = -1;
    return false;
  }
  return true;
}

bool servo_send_all(servo_handle_t* handle, const double angles_deg[SERVO_COUNT]) {
  char line[128];
  int len = snprintf(line, sizeof(line),
      "S %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n",
      angles_deg[0], angles_deg[1], angles_deg[2], angles_deg[3],
      angles_deg[4], angles_deg[5], angles_deg[6], angles_deg[7],
      angles_deg[8]);
  if (len <= 0) {
    return false;
  }
  return write_line(handle, line, (size_t)len);
}

bool servo_send_one(servo_handle_t* handle, int id, double angle_deg) {
  char line[64];
  int len = snprintf(line, sizeof(line), "%d %.1f\n", id, angle_deg);
  if (len <= 0) {
    return false;
  }
  return write_line(handle, line, (size_t)len);
}

void servo_close(servo_handle_t* handle) {
  if (handle == NULL) {
    return;
  }
  if (handle->fd >= 0) {
    close(handle->fd);
  }
  free(handle);
}
