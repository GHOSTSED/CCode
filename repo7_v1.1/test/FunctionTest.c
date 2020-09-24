#include "../include/DMailListener.h"

int main()
{
    DMailListener *pl = dmailListener_init(20, 20);
    dmailListener_run(pl, "../InputFile/test.pcapng");
    dmailListener_print_all_mailInfo(pl);
    dmailListener_delete(pl);
    return 0;
}