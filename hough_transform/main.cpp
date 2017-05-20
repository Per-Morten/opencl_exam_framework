#include <cp_lib.h>

int
main(CP_UNUSED int argc,
     CP_UNUSED char** argv)
{
    cp_log_init();
    CP_DEBUG("Hey");

    cp_log_shutdown();

    return 0;
}
