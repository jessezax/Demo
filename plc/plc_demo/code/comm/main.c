#include "libx.h"
#include "comm.h"
#include <getopt.h>
#include <stdint.h>

static char *g_policy_ver = "1.0.1";

struct option long_options[] = {
    {"config",  required_argument,  NULL,   'c'},
    {"version", no_argument,        NULL,   'v'},
    {"num",     no_argument,        NULL,   'n'},
    {"help",    no_argument,        NULL,   'h'},
    {"Version", no_argument,        NULL,   'V'},
    {0,0,0,0}
};

static const char *short_options = "c:v:n:V:h";
static const char *useage ="\nuseage:\n"
                            "-c --config [PATH] Config file's absoulate path.\n"
                            "-v --version       Version of the program.\n"
                            "-n --num           the max num of ack.\n"
                            "-V --Version       Detail version information.\n"
                            "-h --help          Show help information.\n"
                            ;
u32 g_max_plc_num_for_ack = 1;                      ///< 回复ACK的最大规则数量

static void parse_program_arg(int argc,char**argv)
{
    int c;
    int option_index = 0;
    int count = 0;

    while((c = getopt_long (argc, argv, short_options, long_options, &option_index)) != -1)
    {
        count++;
        switch (c)
        {
            case 'c':{
                printf("config file:%s.\n", optarg);
                strncpy(g_comm_conf.conf_file, optarg, 255);
                count++;
                break;
            }
            case 'n':{
                g_max_plc_num_for_ack = atoi(optarg);
                printf("g_max_plc_num_for_ack is:%u.\n", g_max_plc_num_for_ack);
                count++;
                break;
            }
            case 'v': {
                printf("%s\n", g_policy_ver);
                exit(0);
                break;
            }
            case 'V': {
                printf("%s\n", g_policy_ver);
                exit(0);
                break;
            }
            case 'h': {
                printf("%s", useage);
                exit(0);
                break;
            }
            default:
                //printf("This is default option!\n");
                break;
        }
    }
}

int main(int argc ,char ** argv)
{
    g_comm_conf.start_time = time(NULL);
    parse_program_arg(argc,argv);
    if (module_init() < 0) {
        printf("comm core module init error\n");
        return -1;
    }
    while(true) {
        sleep(1);
    }
    return 0;
}

