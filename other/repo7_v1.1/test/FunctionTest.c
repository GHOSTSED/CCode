#include "../include/DMailListener.h"

int main()
{
    DMailListener *pl = dmailListener_init(20, 20);
    dmailListener_run(pl, NULL, MODE_ONLINE);
    dmailListener_delete(pl);
    return 0;
}