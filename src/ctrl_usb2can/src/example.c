#include "lib_operator.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s /dev/ttyACM0\n", argv[0]);
        return -1;
    }

    int fd = op_init(argv[1]);
    if (fd < 0) return -1;

    printf("Starting control example...\n");

    // Example: Send duty 50% to Robot A PWM 1
    printf("Sending 50%% duty to 0x11\n");
    op_send_duty(fd, 0x11, 50.0f);
    sleep(1);

    // Example: Send direction forward to Robot A Limit 1
    printf("Sending forward to 0x15\n");
    op_send_direction(fd, 0x15, 1);
    sleep(1);

    // Example: Send target angle 1.57 rad (90 deg) to Robot B ABS 1
    printf("Sending 1.57 rad to 0x25\n");
    op_send_target_angle(fd, 0x25, 1.57f);
    sleep(1);

    // Stop everything
    op_send_duty(fd, 0x11, 0.0f);
    op_send_direction(fd, 0x15, 0);

    op_close(fd);
    printf("Done.\n");

    return 0;
}
