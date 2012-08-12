#include <stdio.h>

int core_go   (void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_call (void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_ret  (void)
{
    printf("%s\n", __func__);
    return 0;
}
