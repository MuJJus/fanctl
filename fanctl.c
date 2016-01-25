#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <wiringPi.h>
#include <signal.h>
#include <syslog.h>

#define FAN 22
#define ON LOW
#define OFF HIGH

int fan_force;
int fan_state;

int get_cpu_temp() {
        int fd;
        int ret;
        char buff[32];

        fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
        if (fd < 0) return 100;
        if (read(fd, buff, 32) < 0) return 100;
        ret = atoi(buff) / 1000;
        close(fd);

        return ret;
}

void set_fan_state(int state) {
        fan_state = state;
        digitalWrite(FAN, state);
        syslog(LOG_USER | LOG_NOTICE, "Fan state change to %s.",
                state == ON ? "up" : "down");
}

void signal_handler(int signum) {
        if (signum == SIGUSR1) {
                fan_force = 1;
                set_fan_state(ON);
                syslog(LOG_USER | LOG_INFO, "Force fan running.");
        } else if (signum == SIGUSR2) {
                fan_force = 0;
                syslog(LOG_USER | LOG_INFO, "Cancel force.");
        }
}

int main(int argc, char **argv) {
        switch (fork()) {
        case 0:
                openlog(argv[0], LOG_CONS | LOG_PID, 0);

                if (setsid() == -1) {
                        syslog(LOG_USER | LOG_ERR, "Cannot setsid.");
                        return -1;
                }

                umask(0);
                chdir("/");
                break;
        case -1:
                printf("Cannot fork daemon.");
                return -1;
        default:
                return 0;
        }

        if (wiringPiSetup() == -1) {
                syslog(LOG_USER | LOG_ERR, "Cannot init GPIO.");
                return -1;
        }
        pinMode(FAN, OUTPUT);

        fan_state = digitalRead(FAN);

        signal(SIGUSR1, signal_handler);
        signal(SIGUSR2, signal_handler);

        int cpu_temp;

        while (1) {
                if (!fan_force) {
                        cpu_temp = get_cpu_temp();

                        if (cpu_temp >= 43 && fan_state == OFF) {
                                set_fan_state(ON);
                        }

                        if (cpu_temp <= 35 && fan_state == ON) {
                                set_fan_state(OFF);
                        }
                }

                delay(5000);
        }

        closelog();
        return 0;
}
