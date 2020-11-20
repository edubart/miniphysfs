#define PHYSFS_IMPL
#define PHYSFS_PLATFORM_IMPL
#include "miniphysfs.h"

int main(int argc, char **argv) {
    if(!PHYSFS_init(argv[0])) {
        printf("PhysFS initialization failed: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        return -1;
    }
    PHYSFS_deinit();
    return 0;
}