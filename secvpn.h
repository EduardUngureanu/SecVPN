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