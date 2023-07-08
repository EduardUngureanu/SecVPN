#define DEV_LEN 20

#define DEV_TAP 0
#define DEV_TUN 1

#define PROTO_TCP 0
#define PROTO_UDP 1

#define FRAME_SIZE 2048

struct generic_addr {
   char *ip;
   int port;
};

struct host {
    char *host;
    char *dev;

    int loc_fd;
    int rmt_fd;

    int loc_addr;
    int loc_port;

    int rmt_addr;
    int rmt_port;
};

struct secvpn_opts {
    struct generic_addr bind_addr;

    int dev_type;

    int protocol;
};

extern struct secvpn_opts secvpn;