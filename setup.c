#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/if.h>

#define SERVER 0
#define CLIENT 1
#define PORT 55555

static void run(char *cmd) {
    printf("Executing : %s\n", cmd);
    if (system(cmd)) {
        perror(cmd);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (geteuid() != 0) {
        fprintf(stderr, "App needs root\n");
        exit(1);
    }

    int option;
    int srv_flag = 0;
    int cli_flag = 0;
    char if_name[IFNAMSIZ] = "";
    char remote_ip[16] = "";
    unsigned short int port = PORT;

    // Check command line options
    while((option = getopt(argc, argv, "sc:i:a:p:h")) > 0) {
        switch(option) {
        case 's':
            srv_flag = 1;
            break;

        case 'c':
            cli_flag = 1;
            strncpy(remote_ip, optarg, 15);
            break;

        case 'i':
            strncpy(if_name, optarg, IFNAMSIZ-1);
            break;

        case 'p':
            port = atoi(optarg);
            break;

        case 'h':
            usage();
            return 0;
            break;

        default:
            my_err("Unknown option %c\n", option);
            usage();
            exit(1);
        }
    }

    return 0;
}