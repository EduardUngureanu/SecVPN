#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define BUFSIZE (4 * 1024)
#define CLIENT 0
#define SERVER 1

// Configure the tap device with the the given name
int tap_config(char *dev, char *addr, int mtu) {
    system("ip link set tun11 up");
    system("ip addr add 192.168.0.1/24 dev tun11");

    return 0;
}

// Read routine
int cread(int fd, char *buff, int n) {
    int nread;

    if((nread=read(fd, buff, n)) < 0) {
        perror("Reading data");
        exit(1);
    }

    return nread;
}

// Write routine
int cwrite(int fd, char *buff, int n) {
    int nwrite;

    if((nwrite=write(fd, buff, n)) < 0) {
        perror("Writing data");
        exit(1);
    }

    return nwrite;
}

int run_as_server() {
    int pf_state = check_port_forward_state();

    if(pf_state == 0) {
        printf("Port forwarding is off, turning it on\n");
        if(system("sysctl net.ipv4.ip_forward=1") < 0) {
            perror("Turning on port forwarding");
            exit(1);
        }
    }

    if(pf_state == 0) {
        printf("Turning port forwarding back off\n");
        if(system("sysctl net.ipv4.ip_forward=0") < 0) {
            perror("Turning off port forwarding");
            exit(1);
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int tap_fd, option;


    return 0;
}