#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <errno.h>
#include <signal.h>

// Create generic tun/tap interface, name of interface is stored in *dev, istun = 0 means tap and istun != 0 (usually 1) means tun, returns the open fd
static int tun_open_generic(char *dev, int istun)
{
    struct ifreq ifr;
    int fd;

    // test if /dev/net/tun is accesible
    if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
    {
       syslog(LOG_ERR, "Can't open /dev/net/tun");
       return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = (istun ? IFF_TUN : IFF_TAP) | IFF_NO_PI;
    if (*dev)
    {
       strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0)
    {
        syslog(LOG_ERR, "Error trying to ioctl(TUNSETIFF)");
        close(fd);
        return -1;
    } 

    strcpy(dev, ifr.ifr_name);
    return fd;
}

int tun_open(char *dev) { return tun_open_generic(dev, 1); }
int tap_open(char *dev) { return tun_open_generic(dev, 0); }