# MiniPhysFS

This is the PhysFS contained in a single header to be bundled in C/C++ applications with ease.
[PhysFS](https://icculus.org/physfs/) is a library to provide abstract access to various archives.

# Example Usage

```C
#define PHYSFS_IMPL
#define PHYSFS_PLATFORM_IMPL
#define PHYSFS_SUPPORTS_ONLY_ZIP
#include "miniphysfs.h"

int main(int argc, char **argv) {
    if(!PHYSFS_init(argv[0])) {
        printf("PhysFS initialization failed: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        return -1;
    }
    /* your PhysFS logic here */
    PHYSFS_deinit();
    return 0;
}
```

# Usage

MiniPhysFS do very little modification to the original library, the PhysFS sources were concatenated
to a single C header and some defines were added to make easy of use.

To use the header do this
```C
#define PHYSFS_IMPL
#define PHYSFS_PLATFORM_IMPL
```
before you include this file in *one* C file to create the implementation.

On MacOS you need to define PHYSFS_PLATFORM_IMPL in a separate a `.m` file.
And on Haiku or WinRT you need to define PHYSFS_PLATFORM_IMPL in a separate `.cpp` file.

Note that almost no modification was made in the PhysFS implementation code,
thus there are some C variable names that may collide with your code,
therefore it is best to declare the PhysFS implementation in dedicated C file.

By default all archives are supported, you can disable any of them by defining:
```
PHYSFS_SUPPORTS_NO_ZIP
PHYSFS_SUPPORTS_NO_7Z
PHYSFS_SUPPORTS_NO_GRP
PHYSFS_SUPPORTS_NO_WAD
PHYSFS_SUPPORTS_NO_HOG
PHYSFS_SUPPORTS_NO_MVL
PHYSFS_SUPPORTS_NO_QPAK
PHYSFS_SUPPORTS_NO_SLB
PHYSFS_SUPPORTS_NO_ISO9660
PHYSFS_SUPPORTS_NO_VDF
```
Or you can request *only* certain archives to be supported by defining one of these:
```
PHYSFS_SUPPORTS_ONLY_ZIP
PHYSFS_SUPPORTS_ONLY_7Z
PHYSFS_SUPPORTS_ONLY_GRP
PHYSFS_SUPPORTS_ONLY_WAD
PHYSFS_SUPPORTS_ONLY_HOG
PHYSFS_SUPPORTS_ONLY_MVL
PHYSFS_SUPPORTS_ONLY_QPAK
PHYSFS_SUPPORTS_ONLY_SLB
PHYSFS_SUPPORTS_ONLY_ISO9660
PHYSFS_SUPPORTS_ONLY_VDF
```

Optionally provide the following defines with your own implementations:
  - `PHYSFS_DECL`     - public function declaration prefix (default: extern)


# Documentation

For documentation on how to use PhysFS read the header or
its [official documentation](https://icculus.org/physfs/docs/).

# Updates

- **20-Nov-2020**: Library created, using PhysFS changeset
  [acdcf93d1f9b](https://hg.icculus.org/icculus/physfs/rev/acdcf93d1f9b)

# Notes

PhysFS 3.1.0 with latest changeset (as 20-Nov-2020) [acdcf93d1f9b](https://hg.icculus.org/icculus/physfs/rev/acdcf93d1f9b) 
was used to make this library, the PhysFS 3.1.0 is not officially released yet,
but this changeset was chosen due to the added support for Android.

# License

Same license as PhysFS, zlib license, see LICENSE.txt for licensing information.
