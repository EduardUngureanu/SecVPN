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



// Return current state of port forward
int check_port_forward_state() {
    FILE *fp = popen("sysctl net.ipv4.ip_forward", "r");

    fseek(fp, 0L, SEEK_END);
    int lSize = ftell(fp);
    rewind(fp);
    
    char *buffer;
    buffer = calloc(1, lSize+1);

    fread(buffer, lSize, 1, fp);

    char *tok = strtok(buffer, "=");
    tok = strtok(NULL, " ");

    int state = atoi(tok);

    pclose(fp);

    return state;
}

// Create and return tap device
int tap_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    if((fd = open(clonedev, O_RDWR)) < 0) {
        perror("Opening /dev/net/tun");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);

    return fd;
}

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