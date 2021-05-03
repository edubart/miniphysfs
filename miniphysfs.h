/*
    miniphysfs.h -- PhysFS in a single header

    Project URL: https://github.com/edubart/miniphysfs

    Do this:
        #define PHYSFS_IMPL
        #define PHYSFS_PLATFORM_IMPL
    before you include this file in *one* C file to create the implementation.

    On MacOS you need to define PHYSFS_PLATFORM_IMPL in a separate a `.m` file.
    And on Haiku or WinRT you need to define PHYSFS_PLATFORM_IMPL in a separate `.cpp` file.

    Note that almost no modification was made in the PhysFS implementation code,
    thus there are some C variable names that may collide with your code,
    therefore it is best to declare the PhysFS implementation in dedicated C file.

    By default all archives are supported, you can disable any of them by defining:
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

    Or you can request *only* certain archives to be supported by defining one of these:
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

    Optionally provide the following defines with your own implementations:
        PHYSFS_DECL     - public function declaration prefix (default: extern)


    LICENSE
        Same license as PhysFS, see end of file.
*/

#ifndef PHYSFS_DECL
#define PHYSFS_DECL extern
#endif

/**
 * \file physfs.h
 *
 * Main header file for PhysicsFS.
 */

/**
 * \mainpage PhysicsFS
 *
 * The latest version of PhysicsFS can be found at:
 *     https://icculus.org/physfs/
 *
 * PhysicsFS; a portable, flexible file i/o abstraction.
 *
 * This API gives you access to a system file system in ways superior to the
 *  stdio or system i/o calls. The brief benefits:
 *
 *   - It's portable.
 *   - It's safe. No file access is permitted outside the specified dirs.
 *   - It's flexible. Archives (.ZIP files) can be used transparently as
 *      directory structures.
 *
 * With PhysicsFS, you have a single writing directory and multiple
 *  directories (the "search path") for reading. You can think of this as a
 *  filesystem within a filesystem. If (on Windows) you were to set the
 *  writing directory to "C:\MyGame\MyWritingDirectory", then no PHYSFS calls
 *  could touch anything above this directory, including the "C:\MyGame" and
 *  "C:\" directories. This prevents an application's internal scripting
 *  language from piddling over c:\\config.sys, for example. If you'd rather
 *  give PHYSFS full access to the system's REAL file system, set the writing
 *  dir to "C:\", but that's generally A Bad Thing for several reasons.
 *
 * Drive letters are hidden in PhysicsFS once you set up your initial paths.
 *  The search path creates a single, hierarchical directory structure.
 *  Not only does this lend itself well to general abstraction with archives,
 *  it also gives better support to operating systems like MacOS and Unix.
 *  Generally speaking, you shouldn't ever hardcode a drive letter; not only
 *  does this hurt portability to non-Microsoft OSes, but it limits your win32
 *  users to a single drive, too. Use the PhysicsFS abstraction functions and
 *  allow user-defined configuration options, too. When opening a file, you
 *  specify it like it was on a Unix filesystem: if you want to write to
 *  "C:\MyGame\MyConfigFiles\game.cfg", then you might set the write dir to
 *  "C:\MyGame" and then open "MyConfigFiles/game.cfg". This gives an
 *  abstraction across all platforms. Specifying a file in this way is termed
 *  "platform-independent notation" in this documentation. Specifying a
 *  a filename in a form such as "C:\mydir\myfile" or
 *  "MacOS hard drive:My Directory:My File" is termed "platform-dependent
 *  notation". The only time you use platform-dependent notation is when
 *  setting up your write directory and search path; after that, all file
 *  access into those directories are done with platform-independent notation.
 *
 * All files opened for writing are opened in relation to the write directory,
 *  which is the root of the writable filesystem. When opening a file for
 *  reading, PhysicsFS goes through the search path. This is NOT the
 *  same thing as the PATH environment variable. An application using
 *  PhysicsFS specifies directories to be searched which may be actual
 *  directories, or archive files that contain files and subdirectories of
 *  their own. See the end of these docs for currently supported archive
 *  formats.
 *
 * Once the search path is defined, you may open files for reading. If you've
 *  got the following search path defined (to use a win32 example again):
 *
 *  - C:\\mygame
 *  - C:\\mygame\\myuserfiles
 *  - D:\\mygamescdromdatafiles
 *  - C:\\mygame\\installeddatafiles.zip
 *
 * Then a call to PHYSFS_openRead("textfiles/myfile.txt") (note the directory
 *  separator, lack of drive letter, and lack of dir separator at the start of
 *  the string; this is platform-independent notation) will check for
 *  C:\\mygame\\textfiles\\myfile.txt, then
 *  C:\\mygame\\myuserfiles\\textfiles\\myfile.txt, then
 *  D:\\mygamescdromdatafiles\\textfiles\\myfile.txt, then, finally, for
 *  textfiles\\myfile.txt inside of C:\\mygame\\installeddatafiles.zip.
 *  Remember that most archive types and platform filesystems store their
 *  filenames in a case-sensitive manner, so you should be careful to specify
 *  it correctly.
 *
 * Files opened through PhysicsFS may NOT contain "." or ".." or ":" as dir
 *  elements. Not only are these meaningless on MacOS Classic and/or Unix,
 *  they are a security hole. Also, symbolic links (which can be found in
 *  some archive types and directly in the filesystem on Unix platforms) are
 *  NOT followed until you call PHYSFS_permitSymbolicLinks(). That's left to
 *  your own discretion, as following a symlink can allow for access outside
 *  the write dir and search paths. For portability, there is no mechanism for
 *  creating new symlinks in PhysicsFS.
 *
 * The write dir is not included in the search path unless you specifically
 *  add it. While you CAN change the write dir as many times as you like,
 *  you should probably set it once and stick to it. Remember that your
 *  program will not have permission to write in every directory on Unix and
 *  NT systems.
 *
 * All files are opened in binary mode; there is no endline conversion for
 *  textfiles. Other than that, PhysicsFS has some convenience functions for
 *  platform-independence. There is a function to tell you the current
 *  platform's dir separator ("\\" on windows, "/" on Unix, ":" on MacOS),
 *  which is needed only to set up your search/write paths. There is a
 *  function to tell you what CD-ROM drives contain accessible discs, and a
 *  function to recommend a good search path, etc.
 *
 * A recommended order for the search path is the write dir, then the base dir,
 *  then the cdrom dir, then any archives discovered. Quake 3 does something
 *  like this, but moves the archives to the start of the search path. Build
 *  Engine games, like Duke Nukem 3D and Blood, place the archives last, and
 *  use the base dir for both searching and writing. There is a helper
 *  function (PHYSFS_setSaneConfig()) that puts together a basic configuration
 *  for you, based on a few parameters. Also see the comments on
 *  PHYSFS_getBaseDir(), and PHYSFS_getPrefDir() for info on what those
 *  are and how they can help you determine an optimal search path.
 *
 * PhysicsFS 2.0 adds the concept of "mounting" archives to arbitrary points
 *  in the search path. If a zipfile contains "maps/level.map" and you mount
 *  that archive at "mods/mymod", then you would have to open
 *  "mods/mymod/maps/level.map" to access the file, even though "mods/mymod"
 *  isn't actually specified in the .zip file. Unlike the Unix mentality of
 *  mounting a filesystem, "mods/mymod" doesn't actually have to exist when
 *  mounting the zipfile. It's a "virtual" directory. The mounting mechanism
 *  allows the developer to seperate archives in the tree and avoid trampling
 *  over files when added new archives, such as including mod support in a
 *  game...keeping external content on a tight leash in this manner can be of
 *  utmost importance to some applications.
 *
 * PhysicsFS is mostly thread safe. The errors returned by
 *  PHYSFS_getLastErrorCode() are unique by thread, and library-state-setting
 *  functions are mutex'd. For efficiency, individual file accesses are
 *  not locked, so you can not safely read/write/seek/close/etc the same
 *  file from two threads at the same time. Other race conditions are bugs
 *  that should be reported/patched.
 *
 * While you CAN use stdio/syscall file access in a program that has PHYSFS_*
 *  calls, doing so is not recommended, and you can not directly use system
 *  filehandles with PhysicsFS and vice versa (but as of PhysicsFS 2.1, you
 *  can wrap them in a PHYSFS_Io interface yourself if you wanted to).
 *
 * Note that archives need not be named as such: if you have a ZIP file and
 *  rename it with a .PKG extension, the file will still be recognized as a
 *  ZIP archive by PhysicsFS; the file's contents are used to determine its
 *  type where possible.
 *
 * Currently supported archive types:
 *   - .ZIP (pkZip/WinZip/Info-ZIP compatible)
 *   - .7Z  (7zip archives)
 *   - .ISO (ISO9660 files, CD-ROM images)
 *   - .GRP (Build Engine groupfile archives)
 *   - .PAK (Quake I/II archive format)
 *   - .HOG (Descent I/II/III HOG file archives)
 *   - .MVL (Descent II movielib archives)
 *   - .WAD (DOOM engine archives)
 *   - .VDF (Gothic I/II engine archives)
 *   - .SLB (Independence War archives)
 *
 * String policy for PhysicsFS 2.0 and later:
 *
 * PhysicsFS 1.0 could only deal with null-terminated ASCII strings. All high
 *  ASCII chars resulted in undefined behaviour, and there was no Unicode
 *  support at all. PhysicsFS 2.0 supports Unicode without breaking binary
 *  compatibility with the 1.0 API by using UTF-8 encoding of all strings
 *  passed in and out of the library.
 *
 * All strings passed through PhysicsFS are in null-terminated UTF-8 format.
 *  This means that if all you care about is English (ASCII characters <= 127)
 *  then you just use regular C strings. If you care about Unicode (and you
 *  should!) then you need to figure out what your platform wants, needs, and
 *  offers. If you are on Windows before Win2000 and build with Unicode
 *  support, your TCHAR strings are two bytes per character (this is called
 *  "UCS-2 encoding"). Any modern Windows uses UTF-16, which is two bytes
 *  per character for most characters, but some characters are four. You
 *  should convert them to UTF-8 before handing them to PhysicsFS with
 *  PHYSFS_utf8FromUtf16(), which handles both UTF-16 and UCS-2. If you're
 *  using Unix or Mac OS X, your wchar_t strings are four bytes per character
 *  ("UCS-4 encoding", sometimes called "UTF-32"). Use PHYSFS_utf8FromUcs4().
 *  Mac OS X can give you UTF-8 directly from a CFString or NSString, and many
 *  Unixes generally give you C strings in UTF-8 format everywhere. If you
 *  have a single-byte high ASCII charset, like so-many European "codepages"
 *  you may be out of luck. We'll convert from "Latin1" to UTF-8 only, and
 *  never back to Latin1. If you're above ASCII 127, all bets are off: move
 *  to Unicode or use your platform's facilities. Passing a C string with
 *  high-ASCII data that isn't UTF-8 encoded will NOT do what you expect!
 *
 * Naturally, there's also PHYSFS_utf8ToUcs2(), PHYSFS_utf8ToUtf16(), and
 *  PHYSFS_utf8ToUcs4() to get data back into a format you like. Behind the
 *  scenes, PhysicsFS will use Unicode where possible: the UTF-8 strings on
 *  Windows will be converted and used with the multibyte Windows APIs, for
 *  example.
 *
 * PhysicsFS offers basic encoding conversion support, but not a whole string
 *  library. Get your stuff into whatever format you can work with.
 *
 * Most platforms supported by PhysicsFS 2.1 and later fully support Unicode.
 *  Some older platforms have been dropped (Windows 95, Mac OS 9). Some, like
 *  OS/2, might be able to convert to a local codepage or will just fail to
 *  open/create the file. Modern OSes (macOS, Linux, Windows, etc) should all
 *  be fine.
 *
 * Many game-specific archivers are seriously unprepared for Unicode (the
 *  Descent HOG/MVL and Build Engine GRP archivers, for example, only offer a
 *  DOS 8.3 filename, for example). Nothing can be done for these, but they
 *  tend to be legacy formats for existing content that was all ASCII (and
 *  thus, valid UTF-8) anyhow. Other formats, like .ZIP, don't explicitly
 *  offer Unicode support, but unofficially expect filenames to be UTF-8
 *  encoded, and thus Just Work. Most everything does the right thing without
 *  bothering you, but it's good to be aware of these nuances in case they
 *  don't.
 *
 *
 * Other stuff:
 *
 * Please see the file LICENSE.txt in the source's root directory for
 *  licensing and redistribution rights.
 *
 * Please see the file CREDITS.txt in the source's "docs" directory for
 *  a more or less complete list of who's responsible for this.
 *
 *  \author Ryan C. Gordon.
 */

#ifndef _INCLUDE_PHYSFS_H_
#define _INCLUDE_PHYSFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(PHYSFS_DECL)
/* do nothing. */
#elif defined(_MSC_VER)
#define PHYSFS_DECL __declspec(dllexport)
#elif defined(__SUNPRO_C)
#define PHYSFS_DECL __global
#elif ((__GNUC__ >= 3) && (!defined(__EMX__)) && (!defined(sun)))
#define PHYSFS_DECL __attribute__((visibility("default")))
#else
#define PHYSFS_DECL
#endif

#if defined(PHYSFS_DEPRECATED)
/* do nothing. */
#elif (__GNUC__ >= 4)  /* technically, this arrived in gcc 3.1, but oh well. */
#define PHYSFS_DEPRECATED __attribute__((deprecated))
#else
#define PHYSFS_DEPRECATED
#endif

#if 0  /* !!! FIXME: look into this later. */
#if defined(PHYSFS_CALL)
/* do nothing. */
#elif defined(__WIN32__) && !defined(__GNUC__)
#define PHYSFS_CALL __cdecl
#elif defined(__OS2__) || defined(OS2) /* should work across all compilers. */
#define PHYSFS_CALL _System
#else
#define PHYSFS_CALL
#endif
#endif

/**
 * \typedef PHYSFS_uint8
 * \brief An unsigned, 8-bit integer type.
 */
typedef unsigned char         PHYSFS_uint8;

/**
 * \typedef PHYSFS_sint8
 * \brief A signed, 8-bit integer type.
 */
typedef signed char           PHYSFS_sint8;

/**
 * \typedef PHYSFS_uint16
 * \brief An unsigned, 16-bit integer type.
 */
typedef unsigned short        PHYSFS_uint16;

/**
 * \typedef PHYSFS_sint16
 * \brief A signed, 16-bit integer type.
 */
typedef signed short          PHYSFS_sint16;

/**
 * \typedef PHYSFS_uint32
 * \brief An unsigned, 32-bit integer type.
 */
typedef unsigned int          PHYSFS_uint32;

/**
 * \typedef PHYSFS_sint32
 * \brief A signed, 32-bit integer type.
 */
typedef signed int            PHYSFS_sint32;

/**
 * \typedef PHYSFS_uint64
 * \brief An unsigned, 64-bit integer type.
 * \warning on platforms without any sort of 64-bit datatype, this is
 *           equivalent to PHYSFS_uint32!
 */

/**
 * \typedef PHYSFS_sint64
 * \brief A signed, 64-bit integer type.
 * \warning on platforms without any sort of 64-bit datatype, this is
 *           equivalent to PHYSFS_sint32!
 */


#if (defined PHYSFS_NO_64BIT_SUPPORT)  /* oh well. */
typedef PHYSFS_uint32         PHYSFS_uint64;
typedef PHYSFS_sint32         PHYSFS_sint64;
#elif (defined _MSC_VER)
typedef signed __int64        PHYSFS_sint64;
typedef unsigned __int64      PHYSFS_uint64;
#else
typedef unsigned long long    PHYSFS_uint64;
typedef signed long long      PHYSFS_sint64;
#endif


#ifndef DOXYGEN_SHOULD_IGNORE_THIS
/* Make sure the types really have the right sizes */
#define PHYSFS_COMPILE_TIME_ASSERT(name, x) \
       typedef int PHYSFS_compile_time_assert_##name[(x) * 2 - 1]

PHYSFS_COMPILE_TIME_ASSERT(uint8IsOneByte, sizeof(PHYSFS_uint8) == 1);
PHYSFS_COMPILE_TIME_ASSERT(sint8IsOneByte, sizeof(PHYSFS_sint8) == 1);
PHYSFS_COMPILE_TIME_ASSERT(uint16IsTwoBytes, sizeof(PHYSFS_uint16) == 2);
PHYSFS_COMPILE_TIME_ASSERT(sint16IsTwoBytes, sizeof(PHYSFS_sint16) == 2);
PHYSFS_COMPILE_TIME_ASSERT(uint32IsFourBytes, sizeof(PHYSFS_uint32) == 4);
PHYSFS_COMPILE_TIME_ASSERT(sint32IsFourBytes, sizeof(PHYSFS_sint32) == 4);

#ifndef PHYSFS_NO_64BIT_SUPPORT
PHYSFS_COMPILE_TIME_ASSERT(uint64IsEightBytes, sizeof(PHYSFS_uint64) == 8);
PHYSFS_COMPILE_TIME_ASSERT(sint64IsEightBytes, sizeof(PHYSFS_sint64) == 8);
#endif

#undef PHYSFS_COMPILE_TIME_ASSERT

#endif  /* DOXYGEN_SHOULD_IGNORE_THIS */


/**
 * \struct PHYSFS_File
 * \brief A PhysicsFS file handle.
 *
 * You get a pointer to one of these when you open a file for reading,
 *  writing, or appending via PhysicsFS.
 *
 * As you can see from the lack of meaningful fields, you should treat this
 *  as opaque data. Don't try to manipulate the file handle, just pass the
 *  pointer you got, unmolested, to various PhysicsFS APIs.
 *
 * \sa PHYSFS_openRead
 * \sa PHYSFS_openWrite
 * \sa PHYSFS_openAppend
 * \sa PHYSFS_close
 * \sa PHYSFS_read
 * \sa PHYSFS_write
 * \sa PHYSFS_seek
 * \sa PHYSFS_tell
 * \sa PHYSFS_eof
 * \sa PHYSFS_setBuffer
 * \sa PHYSFS_flush
 */
typedef struct PHYSFS_File
{
    void *opaque;  /**< That's all you get. Don't touch. */
} PHYSFS_File;


/**
 * \def PHYSFS_file
 * \brief 1.0 API compatibility define.
 *
 * PHYSFS_file is identical to PHYSFS_File. This #define is here for backwards
 *  compatibility with the 1.0 API, which had an inconsistent capitalization
 *  convention in this case. New code should use PHYSFS_File, as this #define
 *  may go away someday.
 *
 * \sa PHYSFS_File
 */
#define PHYSFS_file PHYSFS_File


/**
 * \struct PHYSFS_ArchiveInfo
 * \brief Information on various PhysicsFS-supported archives.
 *
 * This structure gives you details on what sort of archives are supported
 *  by this implementation of PhysicsFS. Archives tend to be things like
 *  ZIP files and such.
 *
 * \warning Not all binaries are created equal! PhysicsFS can be built with
 *          or without support for various archives. You can check with
 *          PHYSFS_supportedArchiveTypes() to see if your archive type is
 *          supported.
 *
 * \sa PHYSFS_supportedArchiveTypes
 * \sa PHYSFS_registerArchiver
 * \sa PHYSFS_deregisterArchiver
 */
typedef struct PHYSFS_ArchiveInfo
{
    const char *extension;   /**< Archive file extension: "ZIP", for example. */
    const char *description; /**< Human-readable archive description. */
    const char *author;      /**< Person who did support for this archive. */
    const char *url;         /**< URL related to this archive */
    int supportsSymlinks;    /**< non-zero if archive offers symbolic links. */
} PHYSFS_ArchiveInfo;


/**
 * \struct PHYSFS_Version
 * \brief Information the version of PhysicsFS in use.
 *
 * Represents the library's version as three levels: major revision
 *  (increments with massive changes, additions, and enhancements),
 *  minor revision (increments with backwards-compatible changes to the
 *  major revision), and patchlevel (increments with fixes to the minor
 *  revision).
 *
 * \sa PHYSFS_VERSION
 * \sa PHYSFS_getLinkedVersion
 */
typedef struct PHYSFS_Version
{
    PHYSFS_uint8 major; /**< major revision */
    PHYSFS_uint8 minor; /**< minor revision */
    PHYSFS_uint8 patch; /**< patchlevel */
} PHYSFS_Version;


#ifndef DOXYGEN_SHOULD_IGNORE_THIS
#define PHYSFS_VER_MAJOR 3
#define PHYSFS_VER_MINOR 1
#define PHYSFS_VER_PATCH 0
#endif  /* DOXYGEN_SHOULD_IGNORE_THIS */


/* PhysicsFS state stuff ... */

/**
 * \def PHYSFS_VERSION(x)
 * \brief Macro to determine PhysicsFS version program was compiled against.
 *
 * This macro fills in a PHYSFS_Version structure with the version of the
 *  library you compiled against. This is determined by what header the
 *  compiler uses. Note that if you dynamically linked the library, you might
 *  have a slightly newer or older version at runtime. That version can be
 *  determined with PHYSFS_getLinkedVersion(), which, unlike PHYSFS_VERSION,
 *  is not a macro.
 *
 * \param x A pointer to a PHYSFS_Version struct to initialize.
 *
 * \sa PHYSFS_Version
 * \sa PHYSFS_getLinkedVersion
 */
#define PHYSFS_VERSION(x) \
{ \
    (x)->major = PHYSFS_VER_MAJOR; \
    (x)->minor = PHYSFS_VER_MINOR; \
    (x)->patch = PHYSFS_VER_PATCH; \
}


/**
 * \fn void PHYSFS_getLinkedVersion(PHYSFS_Version *ver)
 * \brief Get the version of PhysicsFS that is linked against your program.
 *
 * If you are using a shared library (DLL) version of PhysFS, then it is
 *  possible that it will be different than the version you compiled against.
 *
 * This is a real function; the macro PHYSFS_VERSION tells you what version
 *  of PhysFS you compiled against:
 *
 * \code
 * PHYSFS_Version compiled;
 * PHYSFS_Version linked;
 *
 * PHYSFS_VERSION(&compiled);
 * PHYSFS_getLinkedVersion(&linked);
 * printf("We compiled against PhysFS version %d.%d.%d ...\n",
 *           compiled.major, compiled.minor, compiled.patch);
 * printf("But we linked against PhysFS version %d.%d.%d.\n",
 *           linked.major, linked.minor, linked.patch);
 * \endcode
 *
 * This function may be called safely at any time, even before PHYSFS_init().
 *
 * \sa PHYSFS_VERSION
 */
PHYSFS_DECL void PHYSFS_getLinkedVersion(PHYSFS_Version *ver);


#ifdef __ANDROID__
typedef struct PHYSFS_AndroidInit
{
    void *jnienv;
    void *context;
} PHYSFS_AndroidInit;
#endif

/**
 * \fn int PHYSFS_init(const char *argv0)
 * \brief Initialize the PhysicsFS library.
 *
 * This must be called before any other PhysicsFS function.
 *
 * This should be called prior to any attempts to change your process's
 *  current working directory.
 *
 * \warning On Android, argv0 should be a non-NULL pointer to a
 *          PHYSFS_AndroidInit struct. This struct must hold a valid JNIEnv *
 *          and a JNI jobject of a Context (either the application context or
 *          the current Activity is fine). Both are cast to a void * so we
 *          don't need jni.h included wherever physfs.h is. PhysicsFS
 *          uses these objects to query some system details. PhysicsFS does
 *          not hold a reference to the JNIEnv or Context past the call to
 *          PHYSFS_init(). If you pass a NULL here, PHYSFS_init can still
 *          succeed, but PHYSFS_getBaseDir() and PHYSFS_getPrefDir() will be
 *          incorrect.
 *
 *   \param argv0 the argv[0] string passed to your program's mainline.
 *          This may be NULL on most platforms (such as ones without a
 *          standard main() function), but you should always try to pass
 *          something in here. Many Unix-like systems _need_ to pass argv[0]
 *          from main() in here. See warning about Android, too!
 *  \return nonzero on success, zero on error. Specifics of the error can be
 *          gleaned from PHYSFS_getLastError().
 *
 * \sa PHYSFS_deinit
 * \sa PHYSFS_isInit
 */
PHYSFS_DECL int PHYSFS_init(const char *argv0);


/**
 * \fn int PHYSFS_deinit(void)
 * \brief Deinitialize the PhysicsFS library.
 *
 * This closes any files opened via PhysicsFS, blanks the search/write paths,
 *  frees memory, and invalidates all of your file handles.
 *
 * Note that this call can FAIL if there's a file open for writing that
 *  refuses to close (for example, the underlying operating system was
 *  buffering writes to network filesystem, and the fileserver has crashed,
 *  or a hard drive has failed, etc). It is usually best to close all write
 *  handles yourself before calling this function, so that you can gracefully
 *  handle a specific failure.
 *
 * Once successfully deinitialized, PHYSFS_init() can be called again to
 *  restart the subsystem. All default API states are restored at this
 *  point, with the exception of any custom allocator you might have
 *  specified, which survives between initializations.
 *
 *  \return nonzero on success, zero on error. Specifics of the error can be
 *          gleaned from PHYSFS_getLastError(). If failure, state of PhysFS is
 *          undefined, and probably badly screwed up.
 *
 * \sa PHYSFS_init
 * \sa PHYSFS_isInit
 */
PHYSFS_DECL int PHYSFS_deinit(void);


/**
 * \fn const PHYSFS_ArchiveInfo **PHYSFS_supportedArchiveTypes(void)
 * \brief Get a list of supported archive types.
 *
 * Get a list of archive types supported by this implementation of PhysicFS.
 *  These are the file formats usable for search path entries. This is for
 *  informational purposes only. Note that the extension listed is merely
 *  convention: if we list "ZIP", you can open a PkZip-compatible archive
 *  with an extension of "XYZ", if you like.
 *
 * The returned value is an array of pointers to PHYSFS_ArchiveInfo structures,
 *  with a NULL entry to signify the end of the list:
 *
 * \code
 * PHYSFS_ArchiveInfo **i;
 *
 * for (i = PHYSFS_supportedArchiveTypes(); *i != NULL; i++)
 * {
 *     printf("Supported archive: [%s], which is [%s].\n",
 *              (*i)->extension, (*i)->description);
 * }
 * \endcode
 *
 * The return values are pointers to internal memory, and should
 *  be considered READ ONLY, and never freed. The returned values are
 *  valid until the next call to PHYSFS_deinit(), PHYSFS_registerArchiver(),
 *  or PHYSFS_deregisterArchiver().
 *
 *   \return READ ONLY Null-terminated array of READ ONLY structures.
 *
 * \sa PHYSFS_registerArchiver
 * \sa PHYSFS_deregisterArchiver
 */
PHYSFS_DECL const PHYSFS_ArchiveInfo **PHYSFS_supportedArchiveTypes(void);


/**
 * \fn void PHYSFS_freeList(void *listVar)
 * \brief Deallocate resources of lists returned by PhysicsFS.
 *
 * Certain PhysicsFS functions return lists of information that are
 *  dynamically allocated. Use this function to free those resources.
 *
 * It is safe to pass a NULL here, but doing so will cause a crash in versions
 *  before PhysicsFS 2.1.0.
 *
 *   \param listVar List of information specified as freeable by this function.
 *                  Passing NULL is safe; it is a valid no-op.
 *
 * \sa PHYSFS_getCdRomDirs
 * \sa PHYSFS_enumerateFiles
 * \sa PHYSFS_getSearchPath
 */
PHYSFS_DECL void PHYSFS_freeList(void *listVar);


/**
 * \fn const char *PHYSFS_getLastError(void)
 * \brief Get human-readable error information.
 *
 * \deprecated Use PHYSFS_getLastErrorCode() and PHYSFS_getErrorByCode() instead.
 *
 * \warning As of PhysicsFS 2.1, this function has been nerfed.
 *          Before PhysicsFS 2.1, this function was the only way to get
 *          error details beyond a given function's basic return value.
 *          This was meant to be a human-readable string in one of several
 *          languages, and was not useful for application parsing. This was
 *          a problem, because the developer and not the user chose the
 *          language at compile time, and the PhysicsFS maintainers had
 *          to (poorly) maintain a significant amount of localization work.
 *          The app couldn't parse the strings, even if they counted on a
 *          specific language, since some were dynamically generated.
 *          In 2.1 and later, this always returns a static string in
 *          English; you may use it as a key string for your own
 *          localizations if you like, as we'll promise not to change
 *          existing error strings. Also, if your application wants to
 *          look at specific errors, we now offer a better option:
 *          use PHYSFS_getLastErrorCode() instead.
 *
 * Get the last PhysicsFS error message as a human-readable, null-terminated
 *  string. This will return NULL if there's been no error since the last call
 *  to this function. The pointer returned by this call points to an internal
 *  buffer. Each thread has a unique error state associated with it, but each
 *  time a new error message is set, it will overwrite the previous one
 *  associated with that thread. It is safe to call this function at anytime,
 *  even before PHYSFS_init().
 *
 * PHYSFS_getLastError() and PHYSFS_getLastErrorCode() both reset the same
 *  thread-specific error state. Calling one will wipe out the other's
 *  data. If you need both, call PHYSFS_getLastErrorCode(), then pass that
 *  value to PHYSFS_getErrorByCode().
 *
 * As of PhysicsFS 2.1, this function only presents text in the English
 *  language, but the strings are static, so you can use them as keys into
 *  your own localization dictionary. These strings are meant to be passed on
 *  directly to the user.
 *
 * Generally, applications should only concern themselves with whether a
 *  given function failed; however, if your code require more specifics, you
 *  should use PHYSFS_getLastErrorCode() instead of this function.
 *
 *   \return READ ONLY string of last error message.
 *
 * \sa PHYSFS_getLastErrorCode
 * \sa PHYSFS_getErrorByCode
 */
PHYSFS_DECL const char *PHYSFS_getLastError(void) PHYSFS_DEPRECATED;


/**
 * \fn const char *PHYSFS_getDirSeparator(void)
 * \brief Get platform-dependent dir separator string.
 *
 * This returns "\\" on win32, "/" on Unix, and ":" on MacOS. It may be more
 *  than one character, depending on the platform, and your code should take
 *  that into account. Note that this is only useful for setting up the
 *  search/write paths, since access into those dirs always use '/'
 *  (platform-independent notation) to separate directories. This is also
 *  handy for getting platform-independent access when using stdio calls.
 *
 *   \return READ ONLY null-terminated string of platform's dir separator.
 */
PHYSFS_DECL const char *PHYSFS_getDirSeparator(void);


/**
 * \fn void PHYSFS_permitSymbolicLinks(int allow)
 * \brief Enable or disable following of symbolic links.
 *
 * Some physical filesystems and archives contain files that are just pointers
 *  to other files. On the physical filesystem, opening such a link will
 *  (transparently) open the file that is pointed to.
 *
 * By default, PhysicsFS will check if a file is really a symlink during open
 *  calls and fail if it is. Otherwise, the link could take you outside the
 *  write and search paths, and compromise security.
 *
 * If you want to take that risk, call this function with a non-zero parameter.
 *  Note that this is more for sandboxing a program's scripting language, in
 *  case untrusted scripts try to compromise the system. Generally speaking,
 *  a user could very well have a legitimate reason to set up a symlink, so
 *  unless you feel there's a specific danger in allowing them, you should
 *  permit them.
 *
 * Symlinks are only explicitly checked when dealing with filenames
 *  in platform-independent notation. That is, when setting up your
 *  search and write paths, etc, symlinks are never checked for.
 *
 * Please note that PHYSFS_stat() will always check the path specified; if
 *  that path is a symlink, it will not be followed in any case. If symlinks
 *  aren't permitted through this function, PHYSFS_stat() ignores them, and
 *  would treat the query as if the path didn't exist at all.
 *
 * Symbolic link permission can be enabled or disabled at any time after
 *  you've called PHYSFS_init(), and is disabled by default.
 *
 *   \param allow nonzero to permit symlinks, zero to deny linking.
 *
 * \sa PHYSFS_symbolicLinksPermitted
 */
PHYSFS_DECL void PHYSFS_permitSymbolicLinks(int allow);


/**
 * \fn char **PHYSFS_getCdRomDirs(void)
 * \brief Get an array of paths to available CD-ROM drives.
 *
 * The dirs returned are platform-dependent ("D:\" on Win32, "/cdrom" or
 *  whatnot on Unix). Dirs are only returned if there is a disc ready and
 *  accessible in the drive. So if you've got two drives (D: and E:), and only
 *  E: has a disc in it, then that's all you get. If the user inserts a disc
 *  in D: and you call this function again, you get both drives. If, on a
 *  Unix box, the user unmounts a disc and remounts it elsewhere, the next
 *  call to this function will reflect that change.
 *
 * This function refers to "CD-ROM" media, but it really means "inserted disc
 *  media," such as DVD-ROM, HD-DVD, CDRW, and Blu-Ray discs. It looks for
 *  filesystems, and as such won't report an audio CD, unless there's a
 *  mounted filesystem track on it.
 *
 * The returned value is an array of strings, with a NULL entry to signify the
 *  end of the list:
 *
 * \code
 * char **cds = PHYSFS_getCdRomDirs();
 * char **i;
 *
 * for (i = cds; *i != NULL; i++)
 *     printf("cdrom dir [%s] is available.\n", *i);
 *
 * PHYSFS_freeList(cds);
 * \endcode
 *
 * This call may block while drives spin up. Be forewarned.
 *
 * When you are done with the returned information, you may dispose of the
 *  resources by calling PHYSFS_freeList() with the returned pointer.
 *
 *   \return Null-terminated array of null-terminated strings.
 *
 * \sa PHYSFS_getCdRomDirsCallback
 */
PHYSFS_DECL char **PHYSFS_getCdRomDirs(void);


/**
 * \fn const char *PHYSFS_getBaseDir(void)
 * \brief Get the path where the application resides.
 *
 * Helper function.
 *
 * Get the "base dir". This is the directory where the application was run
 *  from, which is probably the installation directory, and may or may not
 *  be the process's current working directory.
 *
 * You should probably use the base dir in your search path.
 *
 * \warning On most platforms, this is a directory; on Android, this gives
 *          you the path to the app's package (APK) file. As APK files are
 *          just .zip files, you can mount them in PhysicsFS like regular
 *          directories. You'll probably want to call
 *          PHYSFS_setRoot(basedir, "/assets") after mounting to make your
 *          app's actual data available directly without all the Android
 *          metadata and directory offset. Note that if you passed a NULL to
 *          PHYSFS_init(), you will not get the APK file here.
 *
 *  \return READ ONLY string of base dir in platform-dependent notation.
 *
 * \sa PHYSFS_getPrefDir
 */
PHYSFS_DECL const char *PHYSFS_getBaseDir(void);


/**
 * \fn const char *PHYSFS_getUserDir(void)
 * \brief Get the path where user's home directory resides.
 *
 * \deprecated As of PhysicsFS 2.1, you probably want PHYSFS_getPrefDir().
 *
 * Helper function.
 *
 * Get the "user dir". This is meant to be a suggestion of where a specific
 *  user of the system can store files. On Unix, this is her home directory.
 *  On systems with no concept of multiple home directories (MacOS, win95),
 *  this will default to something like "C:\mybasedir\users\username"
 *  where "username" will either be the login name, or "default" if the
 *  platform doesn't support multiple users, either.
 *
 *  \return READ ONLY string of user dir in platform-dependent notation.
 *
 * \sa PHYSFS_getBaseDir
 * \sa PHYSFS_getPrefDir
 */
PHYSFS_DECL const char *PHYSFS_getUserDir(void) PHYSFS_DEPRECATED;


/**
 * \fn const char *PHYSFS_getWriteDir(void)
 * \brief Get path where PhysicsFS will allow file writing.
 *
 * Get the current write dir. The default write dir is NULL.
 *
 *  \return READ ONLY string of write dir in platform-dependent notation,
 *           OR NULL IF NO WRITE PATH IS CURRENTLY SET.
 *
 * \sa PHYSFS_setWriteDir
 */
PHYSFS_DECL const char *PHYSFS_getWriteDir(void);


/**
 * \fn int PHYSFS_setWriteDir(const char *newDir)
 * \brief Tell PhysicsFS where it may write files.
 *
 * Set a new write dir. This will override the previous setting.
 *
 * This call will fail (and fail to change the write dir) if the current
 *  write dir still has files open in it.
 *
 *   \param newDir The new directory to be the root of the write dir,
 *                   specified in platform-dependent notation. Setting to NULL
 *                   disables the write dir, so no files can be opened for
 *                   writing via PhysicsFS.
 *  \return non-zero on success, zero on failure. All attempts to open a file
 *           for writing via PhysicsFS will fail until this call succeeds.
 *           Use PHYSFS_getLastErrorCode() to obtain the specific error.
 *
 * \sa PHYSFS_getWriteDir
 */
PHYSFS_DECL int PHYSFS_setWriteDir(const char *newDir);


/**
 * \fn int PHYSFS_addToSearchPath(const char *newDir, int appendToPath)
 * \brief Add an archive or directory to the search path.
 *
 * \deprecated As of PhysicsFS 2.0, use PHYSFS_mount() instead. This
 *             function just wraps it anyhow.
 *
 * This function is equivalent to:
 *
 * \code
 *  PHYSFS_mount(newDir, NULL, appendToPath);
 * \endcode
 *
 * You must use this and not PHYSFS_mount if binary compatibility with
 *  PhysicsFS 1.0 is important (which it may not be for many people).
 *
 * \sa PHYSFS_mount
 * \sa PHYSFS_removeFromSearchPath
 * \sa PHYSFS_getSearchPath
 */
PHYSFS_DECL int PHYSFS_addToSearchPath(const char *newDir, int appendToPath)
                                        PHYSFS_DEPRECATED;

/**
 * \fn int PHYSFS_removeFromSearchPath(const char *oldDir)
 * \brief Remove a directory or archive from the search path.
 *
 * \deprecated As of PhysicsFS 2.1, use PHYSFS_unmount() instead. This
 *             function just wraps it anyhow. There's no functional difference
 *             except the vocabulary changed from "adding to the search path"
 *             to "mounting" when that functionality was extended, and thus
 *             the preferred way to accomplish this function's work is now
 *             called "unmounting."
 *
 * This function is equivalent to:
 *
 * \code
 *  PHYSFS_unmount(oldDir);
 * \endcode
 *
 * You must use this and not PHYSFS_unmount if binary compatibility with
 *  PhysicsFS 1.0 is important (which it may not be for many people).
 *
 * \sa PHYSFS_addToSearchPath
 * \sa PHYSFS_getSearchPath
 * \sa PHYSFS_unmount
 */
PHYSFS_DECL int PHYSFS_removeFromSearchPath(const char *oldDir)
                                            PHYSFS_DEPRECATED;


/**
 * \fn char **PHYSFS_getSearchPath(void)
 * \brief Get the current search path.
 *
 * The default search path is an empty list.
 *
 * The returned value is an array of strings, with a NULL entry to signify the
 *  end of the list:
 *
 * \code
 * char **i;
 *
 * for (i = PHYSFS_getSearchPath(); *i != NULL; i++)
 *     printf("[%s] is in the search path.\n", *i);
 * \endcode
 *
 * When you are done with the returned information, you may dispose of the
 *  resources by calling PHYSFS_freeList() with the returned pointer.
 *
 *   \return Null-terminated array of null-terminated strings. NULL if there
 *            was a problem (read: OUT OF MEMORY).
 *
 * \sa PHYSFS_getSearchPathCallback
 * \sa PHYSFS_addToSearchPath
 * \sa PHYSFS_removeFromSearchPath
 */
PHYSFS_DECL char **PHYSFS_getSearchPath(void);


/**
 * \fn int PHYSFS_setSaneConfig(const char *organization, const char *appName, const char *archiveExt, int includeCdRoms, int archivesFirst)
 * \brief Set up sane, default paths.
 *
 * Helper function.
 *
 * The write dir will be set to the pref dir returned by
 *  \code PHYSFS_getPrefDir(organization, appName) \endcode, which is
 *  created if it doesn't exist.
 *
 * The above is sufficient to make sure your program's configuration directory
 *  is separated from other clutter, and platform-independent.
 *
 *  The search path will be:
 *
 *    - The Write Dir (created if it doesn't exist)
 *    - The Base Dir (PHYSFS_getBaseDir())
 *    - All found CD-ROM dirs (optionally)
 *
 * These directories are then searched for files ending with the extension
 *  (archiveExt), which, if they are valid and supported archives, will also
 *  be added to the search path. If you specified "PKG" for (archiveExt), and
 *  there's a file named data.PKG in the base dir, it'll be checked. Archives
 *  can either be appended or prepended to the search path in alphabetical
 *  order, regardless of which directories they were found in. All archives
 *  are mounted in the root of the virtual file system ("/").
 *
 * All of this can be accomplished from the application, but this just does it
 *  all for you. Feel free to add more to the search path manually, too.
 *
 *    \param organization Name of your company/group/etc to be used as a
 *                         dirname, so keep it small, and no-frills.
 *
 *    \param appName Program-specific name of your program, to separate it
 *                   from other programs using PhysicsFS.
 *
 *    \param archiveExt File extension used by your program to specify an
 *                      archive. For example, Quake 3 uses "pk3", even though
 *                      they are just zipfiles. Specify NULL to not dig out
 *                      archives automatically. Do not specify the '.' char;
 *                      If you want to look for ZIP files, specify "ZIP" and
 *                      not ".ZIP" ... the archive search is case-insensitive.
 *
 *    \param includeCdRoms Non-zero to include CD-ROMs in the search path, and
 *                         (if (archiveExt) != NULL) search them for archives.
 *                         This may cause a significant amount of blocking
 *                         while discs are accessed, and if there are no discs
 *                         in the drive (or even not mounted on Unix systems),
 *                         then they may not be made available anyhow. You may
 *                         want to specify zero and handle the disc setup
 *                         yourself.
 *
 *    \param archivesFirst Non-zero to prepend the archives to the search path.
 *                         Zero to append them. Ignored if !(archiveExt).
 *
 *  \return nonzero on success, zero on error. Use PHYSFS_getLastErrorCode()
 *          to obtain the specific error.
 */
PHYSFS_DECL int PHYSFS_setSaneConfig(const char *organization,
                                     const char *appName,
                                     const char *archiveExt,
                                     int includeCdRoms,
                                     int archivesFirst);


/* Directory management stuff ... */

/**
 * \fn int PHYSFS_mkdir(const char *dirName)
 * \brief Create a directory.
 *
 * This is specified in platform-independent notation in relation to the
 *  write dir. All missing parent directories are also created if they
 *  don't exist.
 *
 * So if you've got the write dir set to "C:\mygame\writedir" and call
 *  PHYSFS_mkdir("downloads/maps") then the directories
 *  "C:\mygame\writedir\downloads" and "C:\mygame\writedir\downloads\maps"
 *  will be created if possible. If the creation of "maps" fails after we
 *  have successfully created "downloads", then the function leaves the
 *  created directory behind and reports failure.
 *
 *   \param dirName New dir to create.
 *  \return nonzero on success, zero on error. Use
 *          PHYSFS_getLastErrorCode() to obtain the specific error.
 *
 * \sa PHYSFS_delete
 */
PHYSFS_DECL int PHYSFS_mkdir(const char *dirName);


/**
 * \fn int PHYSFS_delete(const char *filename)
 * \brief Delete a file or directory.
 *
 * (filename) is specified in platform-independent notation in relation to the
 *  write dir.
 *
 * A directory must be empty before this call can delete it.
 *
 * Deleting a symlink will remove the link, not what it points to, regardless
 *  of whether you "permitSymLinks" or not.
 *
 * So if you've got the write dir set to "C:\mygame\writedir" and call
 *  PHYSFS_delete("downloads/maps/level1.map") then the file
 *  "C:\mygame\writedir\downloads\maps\level1.map" is removed from the
 *  physical filesystem, if it exists and the operating system permits the
 *  deletion.
 *
 * Note that on Unix systems, deleting a file may be successful, but the
 *  actual file won't be removed until all processes that have an open
 *  filehandle to it (including your program) close their handles.
 *
 * Chances are, the bits that make up the file still exist, they are just
 *  made available to be written over at a later point. Don't consider this
 *  a security method or anything.  :)
 *
 *   \param filename Filename to delete.
 *  \return nonzero on success, zero on error. Use PHYSFS_getLastErrorCode()
 *          to obtain the specific error.
 */
PHYSFS_DECL int PHYSFS_delete(const char *filename);


/**
 * \fn const char *PHYSFS_getRealDir(const char *filename)
 * \brief Figure out where in the search path a file resides.
 *
 * The file is specified in platform-independent notation. The returned
 *  filename will be the element of the search path where the file was found,
 *  which may be a directory, or an archive. Even if there are multiple
 *  matches in different parts of the search path, only the first one found
 *  is used, just like when opening a file.
 *
 * So, if you look for "maps/level1.map", and C:\\mygame is in your search
 *  path and C:\\mygame\\maps\\level1.map exists, then "C:\mygame" is returned.
 *
 * If a any part of a match is a symbolic link, and you've not explicitly
 *  permitted symlinks, then it will be ignored, and the search for a match
 *  will continue.
 *
 * If you specify a fake directory that only exists as a mount point, it'll
 *  be associated with the first archive mounted there, even though that
 *  directory isn't necessarily contained in a real archive.
 *
 * \warning This will return NULL if there is no real directory associated
 *          with (filename). Specifically, PHYSFS_mountIo(),
 *          PHYSFS_mountMemory(), and PHYSFS_mountHandle() will return NULL
 *          even if the filename is found in the search path. Plan accordingly.
 *
 *     \param filename file to look for.
 *    \return READ ONLY string of element of search path containing the
 *             the file in question. NULL if not found.
 */
PHYSFS_DECL const char *PHYSFS_getRealDir(const char *filename);


/**
 * \fn char **PHYSFS_enumerateFiles(const char *dir)
 * \brief Get a file listing of a search path's directory.
 *
 * \warning In PhysicsFS versions prior to 2.1, this function would return
 *          as many items as it could in the face of a failure condition
 *          (out of memory, disk i/o error, etc). Since this meant apps
 *          couldn't distinguish between complete success and partial failure,
 *          and since the function could always return NULL to report
 *          catastrophic failures anyway, in PhysicsFS 2.1 this function's
 *          policy changed: it will either return a list of complete results
 *          or it will return NULL for any failure of any kind, so we can
 *          guarantee that the enumeration ran to completion and has no gaps
 *          in its results.
 *
 * Matching directories are interpolated. That is, if "C:\mydir" is in the
 *  search path and contains a directory "savegames" that contains "x.sav",
 *  "y.sav", and "z.sav", and there is also a "C:\userdir" in the search path
 *  that has a "savegames" subdirectory with "w.sav", then the following code:
 *
 * \code
 * char **rc = PHYSFS_enumerateFiles("savegames");
 * char **i;
 *
 * for (i = rc; *i != NULL; i++)
 *     printf(" * We've got [%s].\n", *i);
 *
 * PHYSFS_freeList(rc);
 * \endcode
 *
 *  \...will print:
 *
 * \verbatim
 * We've got [x.sav].
 * We've got [y.sav].
 * We've got [z.sav].
 * We've got [w.sav].\endverbatim
 *
 * Feel free to sort the list however you like. However, the returned data
 *  will always contain no duplicates, and will be always sorted in alphabetic
 *  (rather: case-sensitive Unicode) order for you.
 *
 * Don't forget to call PHYSFS_freeList() with the return value from this
 *  function when you are done with it.
 *
 *    \param dir directory in platform-independent notation to enumerate.
 *   \return Null-terminated array of null-terminated strings, or NULL for
 *           failure cases.
 *
 * \sa PHYSFS_enumerate
 */
PHYSFS_DECL char **PHYSFS_enumerateFiles(const char *dir);


/**
 * \fn int PHYSFS_exists(const char *fname)
 * \brief Determine if a file exists in the search path.
 *
 * Reports true if there is an entry anywhere in the search path by the
 *  name of (fname).
 *
 * Note that entries that are symlinks are ignored if
 *  PHYSFS_permitSymbolicLinks(1) hasn't been called, so you
 *  might end up further down in the search path than expected.
 *
 *    \param fname filename in platform-independent notation.
 *   \return non-zero if filename exists. zero otherwise.
 */
PHYSFS_DECL int PHYSFS_exists(const char *fname);


/**
 * \fn int PHYSFS_isDirectory(const char *fname)
 * \brief Determine if a file in the search path is really a directory.
 *
 * \deprecated As of PhysicsFS 2.1, use PHYSFS_stat() instead. This
 *             function just wraps it anyhow.
 *
 * Determine if the first occurence of (fname) in the search path is
 *  really a directory entry.
 *
 * Note that entries that are symlinks are ignored if
 *  PHYSFS_permitSymbolicLinks(1) hasn't been called, so you
 *  might end up further down in the search path than expected.
 *
 *    \param fname filename in platform-independent notation.
 *   \return non-zero if filename exists and is a directory.  zero otherwise.
 *
 * \sa PHYSFS_stat
 * \sa PHYSFS_exists
 */
PHYSFS_DECL int PHYSFS_isDirectory(const char *fname) PHYSFS_DEPRECATED;


/**
 * \fn int PHYSFS_isSymbolicLink(const char *fname)
 * \brief Determine if a file in the search path is really a symbolic link.
 *
 * \deprecated As of PhysicsFS 2.1, use PHYSFS_stat() instead. This
 *             function just wraps it anyhow.
 *
 * Determine if the first occurence of (fname) in the search path is
 *  really a symbolic link.
 *
 * Note that entries that are symlinks are ignored if
 *  PHYSFS_permitSymbolicLinks(1) hasn't been called, and as such,
 *  this function will always return 0 in that case.
 *
 *    \param fname filename in platform-independent notation.
 *   \return non-zero if filename exists and is a symlink.  zero otherwise.
 *
 * \sa PHYSFS_stat
 * \sa PHYSFS_exists
 */
PHYSFS_DECL int PHYSFS_isSymbolicLink(const char *fname) PHYSFS_DEPRECATED;


/**
 * \fn PHYSFS_sint64 PHYSFS_getLastModTime(const char *filename)
 * \brief Get the last modification time of a file.
 *
 * \deprecated As of PhysicsFS 2.1, use PHYSFS_stat() instead. This
 *             function just wraps it anyhow.
 *
 * The modtime is returned as a number of seconds since the Unix epoch
 *  (midnight, Jan 1, 1970). The exact derivation and accuracy of this time
 *  depends on the particular archiver. If there is no reasonable way to
 *  obtain this information for a particular archiver, or there was some sort
 *  of error, this function returns (-1).
 *
 * You must use this and not PHYSFS_stat() if binary compatibility with
 *  PhysicsFS 2.0 is important (which it may not be for many people).
 *
 *   \param filename filename to check, in platform-independent notation.
 *  \return last modified time of the file. -1 if it can't be determined.
 *
 * \sa PHYSFS_stat
 */
PHYSFS_DECL PHYSFS_sint64 PHYSFS_getLastModTime(const char *filename)
                                                PHYSFS_DEPRECATED;


/* i/o stuff... */

/**
 * \fn PHYSFS_File *PHYSFS_openWrite(const char *filename)
 * \brief Open a file for writing.
 *
 * Open a file for writing, in platform-independent notation and in relation
 *  to the write dir as the root of the writable filesystem. The specified
 *  file is created if it doesn't exist. If it does exist, it is truncated to
 *  zero bytes, and the writing offset is set to the start.
 *
 * Note that entries that are symlinks are ignored if
 *  PHYSFS_permitSymbolicLinks(1) hasn't been called, and opening a
 *  symlink with this function will fail in such a case.
 *
 *   \param filename File to open.
 *  \return A valid PhysicsFS filehandle on success, NULL on error. Use
 *          PHYSFS_getLastErrorCode() to obtain the specific error.
 *
 * \sa PHYSFS_openRead
 * \sa PHYSFS_openAppend
 * \sa PHYSFS_write
 * \sa PHYSFS_close
 */
PHYSFS_DECL PHYSFS_File *PHYSFS_openWrite(const char *filename);


/**
 * \fn PHYSFS_File *PHYSFS_openAppend(const char *filename)
 * \brief Open a file for appending.
 *
 * Open a file for writing, in platform-independent notation and in relation
 *  to the write dir as the root of the writable filesystem. The specified
 *  file is created if it doesn't exist. If it does exist, the writing offset
 *  is set to the end of the file, so the first write will be the byte after
 *  the end.
 *
 * Note that entries that are symlinks are ignored if
 *  PHYSFS_permitSymbolicLinks(1) hasn't been called, and opening a
 *  symlink with this function will fail in such a case.
 *
 *   \param filename File to open.
 *  \return A valid PhysicsFS filehandle on success, NULL on error. Use
 *          PHYSFS_getLastErrorCode() to obtain the specific error.
 *
 * \sa PHYSFS_openRead
 * \sa PHYSFS_openWrite
 * \sa PHYSFS_write
 * \sa PHYSFS_close
 */
PHYSFS_DECL PHYSFS_File *PHYSFS_openAppend(const char *filename);


/**
 * \fn PHYSFS_File *PHYSFS_openRead(const char *filename)
 * \brief Open a file for reading.
 *
 * Open a file for reading, in platform-independent notation. The search path
 *  is checked one at a time until a matching file is found, in which case an
 *  abstract filehandle is associated with it, and reading may be done.
 *  The reading offset is set to the first byte of the file.
 *
 * Note that entries that are symlinks are ignored if
 *  PHYSFS_permitSymbolicLinks(1) hasn't been called, and opening a
 *  symlink with this function will fail in such a case.
 *
 *   \param filename File to open.
 *  \return A valid PhysicsFS filehandle on success, NULL on error.
 *          Use PHYSFS_getLastErrorCode() to obtain the specific error.
 *
 * \sa PHYSFS_openWrite
 * \sa PHYSFS_openAppend
 * \sa PHYSFS_read
 * \sa PHYSFS_close
 */
PHYSFS_DECL PHYSFS_File *PHYSFS_openRead(const char *filename);


/**
 * \fn int PHYSFS_close(PHYSFS_File *handle)
 * \brief Close a PhysicsFS filehandle.
 *
 * This call is capable of failing if the operating system was buffering
 *  writes to the physical media, and, now forced to write those changes to
 *  physical media, can not store the data for some reason. In such a case,
 *  the filehandle stays open. A well-written program should ALWAYS check the
 *  return value from the close call in addition to every writing call!
 *
 *   \param handle handle returned from PHYSFS_open*().
 *  \return nonzero on success, zero on error. Use PHYSFS_getLastErrorCode()
 *          to obtain the specific error.
 *
 * \sa PHYSFS_openRead
 * \sa PHYSFS_openWrite
 * \sa PHYSFS_openAppend
 */
PHYSFS_DECL int PHYSFS_close(PHYSFS_File *handle);


/**
 * \fn PHYSFS_sint64 PHYSFS_read(PHYSFS_File *handle, void *buffer, PHYSFS_uint32 objSize, PHYSFS_uint32 objCount)
 * \brief Read data from a PhysicsFS filehandle
 *
 * The file must be opened for reading.
 *
 * \deprecated As of PhysicsFS 2.1, use PHYSFS_readBytes() instead. This
 *             function just wraps it anyhow. This function never clarified
 *             what would happen if you managed to read a partial object, so
 *             working at the byte level makes this cleaner for everyone,
 *             especially now that PHYSFS_Io interfaces can be supplied by the
 *             application.
 *
 *   \param handle handle returned from PHYSFS_openRead().
 *   \param buffer buffer to store read data into.
 *   \param objSize size in bytes of objects being read from (handle).
 *   \param objCount number of (objSize) objects to read from (handle).
 *  \return number of objects read. PHYSFS_getLastErrorCode() can shed light
 *          on the reason this might be < (objCount), as can PHYSFS_eof().
 *          -1 if complete failure.
 *
 * \sa PHYSFS_readBytes
 * \sa PHYSFS_eof
 */
PHYSFS_DECL PHYSFS_sint64 PHYSFS_read(PHYSFS_File *handle,
                                      void *buffer,
                                      PHYSFS_uint32 objSize,
                                      PHYSFS_uint32 objCount)
                                        PHYSFS_DEPRECATED;

/**
 * \fn PHYSFS_sint64 PHYSFS_write(PHYSFS_File *handle, const void *buffer, PHYSFS_uint32 objSize, PHYSFS_uint32 objCount)
 * \brief Write data to a PhysicsFS filehandle
 *
 * The file must be opened for writing.
 *
 * \deprecated As of PhysicsFS 2.1, use PHYSFS_writeBytes() instead. This
 *             function just wraps it anyhow. This function never clarified
 *             what would happen if you managed to write a partial object, so
 *             working at the byte level makes this cleaner for everyone,
 *             especially now that PHYSFS_Io interfaces can be supplied by the
 *             application.
 *
 *   \param handle retval from PHYSFS_openWrite() or PHYSFS_openAppend().
 *   \param buffer buffer of bytes to write to (handle).
 *   \param objSize size in bytes of objects being written to (handle).
 *   \param objCount number of (objSize) objects to write to (handle).
 *  \return number of objects written. PHYSFS_getLastErrorCode() can shed
 *          light on the reason this might be < (objCount). -1 if complete
 *          failure.
 *
 * \sa PHYSFS_writeBytes
 */
PHYSFS_DECL PHYSFS_sint64 PHYSFS_write(PHYSFS_File *handle,
                                       const void *buffer,
                                       PHYSFS_uint32 objSize,
                                       PHYSFS_uint32 objCount)
                                        PHYSFS_DEPRECATED;


/* File position stuff... */

/**
 * \fn int PHYSFS_eof(PHYSFS_File *handle)
 * \brief Check for end-of-file state on a PhysicsFS filehandle.
 *
 * Determine if the end of file has been reached in a PhysicsFS filehandle.
 *
 *   \param handle handle returned from PHYSFS_openRead().
 *  \return nonzero if EOF, zero if not.
 *
 * \sa PHYSFS_read
 * \sa PHYSFS_tell
 */
PHYSFS_DECL int PHYSFS_eof(PHYSFS_File *handle);


/**
 * \fn PHYSFS_sint64 PHYSFS_tell(PHYSFS_File *handle)
 * \brief Determine current position within a PhysicsFS filehandle.
 *
 *   \param handle handle returned from PHYSFS_open*().
 *  \return offset in bytes from start of file. -1 if error occurred.
 *           Use PHYSFS_getLastErrorCode() to obtain the specific error.
 *
 * \sa PHYSFS_seek
 */
PHYSFS_DECL PHYSFS_sint64 PHYSFS_tell(PHYSFS_File *handle);


/**
 * \fn int PHYSFS_seek(PHYSFS_File *handle, PHYSFS_uint64 pos)
 * \brief Seek to a new position within a PhysicsFS filehandle.
 *
 * The next read or write will occur at that place. Seeking past the
 *  beginning or end of the file is not allowed, and causes an error.
 *
 *   \param handle handle returned from PHYSFS_open*().
 *   \param pos number of bytes from start of file to seek to.
 *  \return nonzero on success, zero on error. Use PHYSFS_getLastErrorCode()
 *          to obtain the specific error.
 *
 * \sa PHYSFS_tell
 */
PHYSFS_DECL int PHYSFS_seek(PHYSFS_File *handle, PHYSFS_uint64 pos);


/**
 * \fn PHYSFS_sint64 PHYSFS_fileLength(PHYSFS_File *handle)
 * \brief Get total length of a file in bytes.
 *
 * Note that if another process/thread is writing to this file at the same
 *  time, then the information this function supplies could be incorrect
 *  before you get it. Use with caution, or better yet, don't use at all.
 *
 *   \param handle handle returned from PHYSFS_open*().
 *  \return size in bytes of the file. -1 if can't be determined.
 *
 * \sa PHYSFS_tell
 * \sa PHYSFS_seek
 */
PHYSFS_DECL PHYSFS_sint64 PHYSFS_fileLength(PHYSFS_File *handle);


/* Buffering stuff... */

/**
 * \fn int PHYSFS_setBuffer(PHYSFS_File *handle, PHYSFS_uint64 bufsize)
 * \brief Set up buffering for a PhysicsFS file handle.
 *
 * Define an i/o buffer for a file handle. A memory block of (bufsize) bytes
 *  will be allocated and associated with (handle).
 *
 * For files opened for reading, up to (bufsize) bytes are read from (handle)
 *  and stored in the internal buffer. Calls to PHYSFS_read() will pull
 *  from this buffer until it is empty, and then refill it for more reading.
 *  Note that compressed files, like ZIP archives, will decompress while
 *  buffering, so this can be handy for offsetting CPU-intensive operations.
 *  The buffer isn't filled until you do your next read.
 *
 * For files opened for writing, data will be buffered to memory until the
 *  buffer is full or the buffer is flushed. Closing a handle implicitly
 *  causes a flush...check your return values!
 *
 * Seeking, etc transparently accounts for buffering.
 *
 * You can resize an existing buffer by calling this function more than once
 *  on the same file. Setting the buffer size to zero will free an existing
 *  buffer.
 *
 * PhysicsFS file handles are unbuffered by default.
 *
 * Please check the return value of this function! Failures can include
 *  not being able to seek backwards in a read-only file when removing the
 *  buffer, not being able to allocate the buffer, and not being able to
 *  flush the buffer to disk, among other unexpected problems.
 *
 *   \param handle handle returned from PHYSFS_open*().
 *   \param bufsize size, in bytes, of buffer to allocate.
 *  \return nonzero if successful, zero on error.
 *
 * \sa PHYSFS_flush
 * \sa PHYSFS_read
 * \sa PHYSFS_write
 * \sa PHYSFS_close
 */
PHYSFS_DECL int PHYSFS_setBuffer(PHYSFS_File *handle, PHYSFS_uint64 bufsize);


/**
 * \fn int PHYSFS_flush(PHYSFS_File *handle)
 * \brief Flush a buffered PhysicsFS file handle.
 *
 * For buffered files opened for writing, this will put the current contents
 *  of the buffer to disk and flag the buffer as empty if possible.
 *
 * For buffered files opened for reading or unbuffered files, this is a safe
 *  no-op, and will report success.
 *
 *   \param handle handle returned from PHYSFS_open*().
 *  \return nonzero if successful, zero on error.
 *
 * \sa PHYSFS_setBuffer
 * \sa PHYSFS_close
 */
PHYSFS_DECL int PHYSFS_flush(PHYSFS_File *handle);


/* Byteorder stuff... */

/**
 * \fn PHYSFS_sint16 PHYSFS_swapSLE16(PHYSFS_sint16 val)
 * \brief Swap littleendian signed 16 to platform's native byte order.
 *
 * Take a 16-bit signed value in littleendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 */
PHYSFS_DECL PHYSFS_sint16 PHYSFS_swapSLE16(PHYSFS_sint16 val);


/**
 * \fn PHYSFS_uint16 PHYSFS_swapULE16(PHYSFS_uint16 val)
 * \brief Swap littleendian unsigned 16 to platform's native byte order.
 *
 * Take a 16-bit unsigned value in littleendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 */
PHYSFS_DECL PHYSFS_uint16 PHYSFS_swapULE16(PHYSFS_uint16 val);

/**
 * \fn PHYSFS_sint32 PHYSFS_swapSLE32(PHYSFS_sint32 val)
 * \brief Swap littleendian signed 32 to platform's native byte order.
 *
 * Take a 32-bit signed value in littleendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 */
PHYSFS_DECL PHYSFS_sint32 PHYSFS_swapSLE32(PHYSFS_sint32 val);


/**
 * \fn PHYSFS_uint32 PHYSFS_swapULE32(PHYSFS_uint32 val)
 * \brief Swap littleendian unsigned 32 to platform's native byte order.
 *
 * Take a 32-bit unsigned value in littleendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 */
PHYSFS_DECL PHYSFS_uint32 PHYSFS_swapULE32(PHYSFS_uint32 val);

/**
 * \fn PHYSFS_sint64 PHYSFS_swapSLE64(PHYSFS_sint64 val)
 * \brief Swap littleendian signed 64 to platform's native byte order.
 *
 * Take a 64-bit signed value in littleendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 *
 * \warning Remember, PHYSFS_sint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL PHYSFS_sint64 PHYSFS_swapSLE64(PHYSFS_sint64 val);


/**
 * \fn PHYSFS_uint64 PHYSFS_swapULE64(PHYSFS_uint64 val)
 * \brief Swap littleendian unsigned 64 to platform's native byte order.
 *
 * Take a 64-bit unsigned value in littleendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 *
 * \warning Remember, PHYSFS_uint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL PHYSFS_uint64 PHYSFS_swapULE64(PHYSFS_uint64 val);


/**
 * \fn PHYSFS_sint16 PHYSFS_swapSBE16(PHYSFS_sint16 val)
 * \brief Swap bigendian signed 16 to platform's native byte order.
 *
 * Take a 16-bit signed value in bigendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 */
PHYSFS_DECL PHYSFS_sint16 PHYSFS_swapSBE16(PHYSFS_sint16 val);


/**
 * \fn PHYSFS_uint16 PHYSFS_swapUBE16(PHYSFS_uint16 val)
 * \brief Swap bigendian unsigned 16 to platform's native byte order.
 *
 * Take a 16-bit unsigned value in bigendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 */
PHYSFS_DECL PHYSFS_uint16 PHYSFS_swapUBE16(PHYSFS_uint16 val);

/**
 * \fn PHYSFS_sint32 PHYSFS_swapSBE32(PHYSFS_sint32 val)
 * \brief Swap bigendian signed 32 to platform's native byte order.
 *
 * Take a 32-bit signed value in bigendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 */
PHYSFS_DECL PHYSFS_sint32 PHYSFS_swapSBE32(PHYSFS_sint32 val);


/**
 * \fn PHYSFS_uint32 PHYSFS_swapUBE32(PHYSFS_uint32 val)
 * \brief Swap bigendian unsigned 32 to platform's native byte order.
 *
 * Take a 32-bit unsigned value in bigendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 */
PHYSFS_DECL PHYSFS_uint32 PHYSFS_swapUBE32(PHYSFS_uint32 val);


/**
 * \fn PHYSFS_sint64 PHYSFS_swapSBE64(PHYSFS_sint64 val)
 * \brief Swap bigendian signed 64 to platform's native byte order.
 *
 * Take a 64-bit signed value in bigendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 *
 * \warning Remember, PHYSFS_sint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL PHYSFS_sint64 PHYSFS_swapSBE64(PHYSFS_sint64 val);


/**
 * \fn PHYSFS_uint64 PHYSFS_swapUBE64(PHYSFS_uint64 val)
 * \brief Swap bigendian unsigned 64 to platform's native byte order.
 *
 * Take a 64-bit unsigned value in bigendian format and convert it to
 *  the platform's native byte order.
 *
 *    \param val value to convert
 *   \return converted value.
 *
 * \warning Remember, PHYSFS_uint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL PHYSFS_uint64 PHYSFS_swapUBE64(PHYSFS_uint64 val);


/**
 * \fn int PHYSFS_readSLE16(PHYSFS_File *file, PHYSFS_sint16 *val)
 * \brief Read and convert a signed 16-bit littleendian value.
 *
 * Convenience function. Read a signed 16-bit littleendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_readSLE16(PHYSFS_File *file, PHYSFS_sint16 *val);


/**
 * \fn int PHYSFS_readULE16(PHYSFS_File *file, PHYSFS_uint16 *val)
 * \brief Read and convert an unsigned 16-bit littleendian value.
 *
 * Convenience function. Read an unsigned 16-bit littleendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 *
 */
PHYSFS_DECL int PHYSFS_readULE16(PHYSFS_File *file, PHYSFS_uint16 *val);


/**
 * \fn int PHYSFS_readSBE16(PHYSFS_File *file, PHYSFS_sint16 *val)
 * \brief Read and convert a signed 16-bit bigendian value.
 *
 * Convenience function. Read a signed 16-bit bigendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_readSBE16(PHYSFS_File *file, PHYSFS_sint16 *val);


/**
 * \fn int PHYSFS_readUBE16(PHYSFS_File *file, PHYSFS_uint16 *val)
 * \brief Read and convert an unsigned 16-bit bigendian value.
 *
 * Convenience function. Read an unsigned 16-bit bigendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 *
 */
PHYSFS_DECL int PHYSFS_readUBE16(PHYSFS_File *file, PHYSFS_uint16 *val);


/**
 * \fn int PHYSFS_readSLE32(PHYSFS_File *file, PHYSFS_sint32 *val)
 * \brief Read and convert a signed 32-bit littleendian value.
 *
 * Convenience function. Read a signed 32-bit littleendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_readSLE32(PHYSFS_File *file, PHYSFS_sint32 *val);


/**
 * \fn int PHYSFS_readULE32(PHYSFS_File *file, PHYSFS_uint32 *val)
 * \brief Read and convert an unsigned 32-bit littleendian value.
 *
 * Convenience function. Read an unsigned 32-bit littleendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 *
 */
PHYSFS_DECL int PHYSFS_readULE32(PHYSFS_File *file, PHYSFS_uint32 *val);


/**
 * \fn int PHYSFS_readSBE32(PHYSFS_File *file, PHYSFS_sint32 *val)
 * \brief Read and convert a signed 32-bit bigendian value.
 *
 * Convenience function. Read a signed 32-bit bigendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_readSBE32(PHYSFS_File *file, PHYSFS_sint32 *val);


/**
 * \fn int PHYSFS_readUBE32(PHYSFS_File *file, PHYSFS_uint32 *val)
 * \brief Read and convert an unsigned 32-bit bigendian value.
 *
 * Convenience function. Read an unsigned 32-bit bigendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 *
 */
PHYSFS_DECL int PHYSFS_readUBE32(PHYSFS_File *file, PHYSFS_uint32 *val);


/**
 * \fn int PHYSFS_readSLE64(PHYSFS_File *file, PHYSFS_sint64 *val)
 * \brief Read and convert a signed 64-bit littleendian value.
 *
 * Convenience function. Read a signed 64-bit littleendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 *
 * \warning Remember, PHYSFS_sint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL int PHYSFS_readSLE64(PHYSFS_File *file, PHYSFS_sint64 *val);


/**
 * \fn int PHYSFS_readULE64(PHYSFS_File *file, PHYSFS_uint64 *val)
 * \brief Read and convert an unsigned 64-bit littleendian value.
 *
 * Convenience function. Read an unsigned 64-bit littleendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 *
 * \warning Remember, PHYSFS_uint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL int PHYSFS_readULE64(PHYSFS_File *file, PHYSFS_uint64 *val);


/**
 * \fn int PHYSFS_readSBE64(PHYSFS_File *file, PHYSFS_sint64 *val)
 * \brief Read and convert a signed 64-bit bigendian value.
 *
 * Convenience function. Read a signed 64-bit bigendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 *
 * \warning Remember, PHYSFS_sint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL int PHYSFS_readSBE64(PHYSFS_File *file, PHYSFS_sint64 *val);


/**
 * \fn int PHYSFS_readUBE64(PHYSFS_File *file, PHYSFS_uint64 *val)
 * \brief Read and convert an unsigned 64-bit bigendian value.
 *
 * Convenience function. Read an unsigned 64-bit bigendian value from a
 *  file and convert it to the platform's native byte order.
 *
 *    \param file PhysicsFS file handle from which to read.
 *    \param val pointer to where value should be stored.
 *   \return zero on failure, non-zero on success. If successful, (*val) will
 *           store the result. On failure, you can find out what went wrong
 *           from PHYSFS_getLastErrorCode().
 *
 * \warning Remember, PHYSFS_uint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL int PHYSFS_readUBE64(PHYSFS_File *file, PHYSFS_uint64 *val);


/**
 * \fn int PHYSFS_writeSLE16(PHYSFS_File *file, PHYSFS_sint16 val)
 * \brief Convert and write a signed 16-bit littleendian value.
 *
 * Convenience function. Convert a signed 16-bit value from the platform's
 *  native byte order to littleendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_writeSLE16(PHYSFS_File *file, PHYSFS_sint16 val);


/**
 * \fn int PHYSFS_writeULE16(PHYSFS_File *file, PHYSFS_uint16 val)
 * \brief Convert and write an unsigned 16-bit littleendian value.
 *
 * Convenience function. Convert an unsigned 16-bit value from the platform's
 *  native byte order to littleendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_writeULE16(PHYSFS_File *file, PHYSFS_uint16 val);


/**
 * \fn int PHYSFS_writeSBE16(PHYSFS_File *file, PHYSFS_sint16 val)
 * \brief Convert and write a signed 16-bit bigendian value.
 *
 * Convenience function. Convert a signed 16-bit value from the platform's
 *  native byte order to bigendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_writeSBE16(PHYSFS_File *file, PHYSFS_sint16 val);


/**
 * \fn int PHYSFS_writeUBE16(PHYSFS_File *file, PHYSFS_uint16 val)
 * \brief Convert and write an unsigned 16-bit bigendian value.
 *
 * Convenience function. Convert an unsigned 16-bit value from the platform's
 *  native byte order to bigendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_writeUBE16(PHYSFS_File *file, PHYSFS_uint16 val);


/**
 * \fn int PHYSFS_writeSLE32(PHYSFS_File *file, PHYSFS_sint32 val)
 * \brief Convert and write a signed 32-bit littleendian value.
 *
 * Convenience function. Convert a signed 32-bit value from the platform's
 *  native byte order to littleendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_writeSLE32(PHYSFS_File *file, PHYSFS_sint32 val);


/**
 * \fn int PHYSFS_writeULE32(PHYSFS_File *file, PHYSFS_uint32 val)
 * \brief Convert and write an unsigned 32-bit littleendian value.
 *
 * Convenience function. Convert an unsigned 32-bit value from the platform's
 *  native byte order to littleendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_writeULE32(PHYSFS_File *file, PHYSFS_uint32 val);


/**
 * \fn int PHYSFS_writeSBE32(PHYSFS_File *file, PHYSFS_sint32 val)
 * \brief Convert and write a signed 32-bit bigendian value.
 *
 * Convenience function. Convert a signed 32-bit value from the platform's
 *  native byte order to bigendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_writeSBE32(PHYSFS_File *file, PHYSFS_sint32 val);


/**
 * \fn int PHYSFS_writeUBE32(PHYSFS_File *file, PHYSFS_uint32 val)
 * \brief Convert and write an unsigned 32-bit bigendian value.
 *
 * Convenience function. Convert an unsigned 32-bit value from the platform's
 *  native byte order to bigendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 */
PHYSFS_DECL int PHYSFS_writeUBE32(PHYSFS_File *file, PHYSFS_uint32 val);


/**
 * \fn int PHYSFS_writeSLE64(PHYSFS_File *file, PHYSFS_sint64 val)
 * \brief Convert and write a signed 64-bit littleendian value.
 *
 * Convenience function. Convert a signed 64-bit value from the platform's
 *  native byte order to littleendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 *
 * \warning Remember, PHYSFS_sint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL int PHYSFS_writeSLE64(PHYSFS_File *file, PHYSFS_sint64 val);


/**
 * \fn int PHYSFS_writeULE64(PHYSFS_File *file, PHYSFS_uint64 val)
 * \brief Convert and write an unsigned 64-bit littleendian value.
 *
 * Convenience function. Convert an unsigned 64-bit value from the platform's
 *  native byte order to littleendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 *
 * \warning Remember, PHYSFS_uint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL int PHYSFS_writeULE64(PHYSFS_File *file, PHYSFS_uint64 val);


/**
 * \fn int PHYSFS_writeSBE64(PHYSFS_File *file, PHYSFS_sint64 val)
 * \brief Convert and write a signed 64-bit bigending value.
 *
 * Convenience function. Convert a signed 64-bit value from the platform's
 *  native byte order to bigendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 *
 * \warning Remember, PHYSFS_sint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL int PHYSFS_writeSBE64(PHYSFS_File *file, PHYSFS_sint64 val);


/**
 * \fn int PHYSFS_writeUBE64(PHYSFS_File *file, PHYSFS_uint64 val)
 * \brief Convert and write an unsigned 64-bit bigendian value.
 *
 * Convenience function. Convert an unsigned 64-bit value from the platform's
 *  native byte order to bigendian and write it to a file.
 *
 *    \param file PhysicsFS file handle to which to write.
 *    \param val Value to convert and write.
 *   \return zero on failure, non-zero on success. On failure, you can
 *           find out what went wrong from PHYSFS_getLastErrorCode().
 *
 * \warning Remember, PHYSFS_uint64 is only 32 bits on platforms without
 *          any sort of 64-bit support.
 */
PHYSFS_DECL int PHYSFS_writeUBE64(PHYSFS_File *file, PHYSFS_uint64 val);


/* Everything above this line is part of the PhysicsFS 1.0 API. */

/**
 * \fn int PHYSFS_isInit(void)
 * \brief Determine if the PhysicsFS library is initialized.
 *
 * Once PHYSFS_init() returns successfully, this will return non-zero.
 *  Before a successful PHYSFS_init() and after PHYSFS_deinit() returns
 *  successfully, this will return zero. This function is safe to call at
 *  any time.
 *
 *  \return non-zero if library is initialized, zero if library is not.
 *
 * \sa PHYSFS_init
 * \sa PHYSFS_deinit
 */
PHYSFS_DECL int PHYSFS_isInit(void);


/**
 * \fn int PHYSFS_symbolicLinksPermitted(void)
 * \brief Determine if the symbolic links are permitted.
 *
 * This reports the setting from the last call to PHYSFS_permitSymbolicLinks().
 *  If PHYSFS_permitSymbolicLinks() hasn't been called since the library was
 *  last initialized, symbolic links are implicitly disabled.
 *
 *  \return non-zero if symlinks are permitted, zero if not.
 *
 * \sa PHYSFS_permitSymbolicLinks
 */
PHYSFS_DECL int PHYSFS_symbolicLinksPermitted(void);


/**
 * \struct PHYSFS_Allocator
 * \brief PhysicsFS allocation function pointers.
 *
 * (This is for limited, hardcore use. If you don't immediately see a need
 *  for it, you can probably ignore this forever.)
 *
 * You create one of these structures for use with PHYSFS_setAllocator.
 *  Allocators are assumed to be reentrant by the caller; please mutex
 *  accordingly.
 *
 * Allocations are always discussed in 64-bits, for future expansion...we're
 *  on the cusp of a 64-bit transition, and we'll probably be allocating 6
 *  gigabytes like it's nothing sooner or later, and I don't want to change
 *  this again at that point. If you're on a 32-bit platform and have to
 *  downcast, it's okay to return NULL if the allocation is greater than
 *  4 gigabytes, since you'd have to do so anyhow.
 *
 * \sa PHYSFS_setAllocator
 */
typedef struct PHYSFS_Allocator
{
    int (*Init)(void);   /**< Initialize. Can be NULL. Zero on failure. */
    void (*Deinit)(void);  /**< Deinitialize your allocator. Can be NULL. */
    void *(*Malloc)(PHYSFS_uint64);  /**< Allocate like malloc(). */
    void *(*Realloc)(void *, PHYSFS_uint64); /**< Reallocate like realloc(). */
    void (*Free)(void *); /**< Free memory from Malloc or Realloc. */
} PHYSFS_Allocator;


/**
 * \fn int PHYSFS_setAllocator(const PHYSFS_Allocator *allocator)
 * \brief Hook your own allocation routines into PhysicsFS.
 *
 * (This is for limited, hardcore use. If you don't immediately see a need
 *  for it, you can probably ignore this forever.)
 *
 * By default, PhysicsFS will use whatever is reasonable for a platform
 *  to manage dynamic memory (usually ANSI C malloc/realloc/free, but
 *  some platforms might use something else), but in some uncommon cases, the
 *  app might want more control over the library's memory management. This
 *  lets you redirect PhysicsFS to use your own allocation routines instead.
 *  You can only call this function before PHYSFS_init(); if the library is
 *  initialized, it'll reject your efforts to change the allocator mid-stream.
 *  You may call this function after PHYSFS_deinit() if you are willing to
 *  shut down the library and restart it with a new allocator; this is a safe
 *  and supported operation. The allocator remains intact between deinit/init
 *  calls. If you want to return to the platform's default allocator, pass a
 *  NULL in here.
 *
 * If you aren't immediately sure what to do with this function, you can
 *  safely ignore it altogether.
 *
 *    \param allocator Structure containing your allocator's entry points.
 *   \return zero on failure, non-zero on success. This call only fails
 *           when used between PHYSFS_init() and PHYSFS_deinit() calls.
 */
PHYSFS_DECL int PHYSFS_setAllocator(const PHYSFS_Allocator *allocator);


/**
 * \fn int PHYSFS_mount(const char *newDir, const char *mountPoint, int appendToPath)
 * \brief Add an archive or directory to the search path.
 *
 * If this is a duplicate, the entry is not added again, even though the
 *  function succeeds. You may not add the same archive to two different
 *  mountpoints: duplicate checking is done against the archive and not the
 *  mountpoint.
 *
 * When you mount an archive, it is added to a virtual file system...all files
 *  in all of the archives are interpolated into a single hierachical file
 *  tree. Two archives mounted at the same place (or an archive with files
 *  overlapping another mountpoint) may have overlapping files: in such a case,
 *  the file earliest in the search path is selected, and the other files are
 *  inaccessible to the application. This allows archives to be used to
 *  override previous revisions; you can use the mounting mechanism to place
 *  archives at a specific point in the file tree and prevent overlap; this
 *  is useful for downloadable mods that might trample over application data
 *  or each other, for example.
 *
 * The mountpoint does not need to exist prior to mounting, which is different
 *  than those familiar with the Unix concept of "mounting" may expect.
 *  As well, more than one archive can be mounted to the same mountpoint, or
 *  mountpoints and archive contents can overlap...the interpolation mechanism
 *  still functions as usual.
 *
 * Specifying a symbolic link to an archive or directory is allowed here,
 *  regardless of the state of PHYSFS_permitSymbolicLinks(). That function
 *  only deals with symlinks inside the mounted directory or archive.
 *
 *   \param newDir directory or archive to add to the path, in
 *                   platform-dependent notation.
 *   \param mountPoint Location in the interpolated tree that this archive
 *                     will be "mounted", in platform-independent notation.
 *                     NULL or "" is equivalent to "/".
 *   \param appendToPath nonzero to append to search path, zero to prepend.
 *  \return nonzero if added to path, zero on failure (bogus archive, dir
 *          missing, etc). Use PHYSFS_getLastErrorCode() to obtain
 *          the specific error.
 *
 * \sa PHYSFS_removeFromSearchPath
 * \sa PHYSFS_getSearchPath
 * \sa PHYSFS_getMountPoint
 * \sa PHYSFS_mountIo
 */
PHYSFS_DECL int PHYSFS_mount(const char *newDir,
                             const char *mountPoint,
                             int appendToPath);

/**
 * \fn int PHYSFS_getMountPoint(const char *dir)
 * \brief Determine a mounted archive's mountpoint.
 *
 * You give this function the name of an archive or dir you successfully
 *  added to the search path, and it reports the location in the interpolated
 *  tree where it is mounted. Files mounted with a NULL mountpoint or through
 *  PHYSFS_addToSearchPath() will report "/". The return value is READ ONLY
 *  and valid until the archive is removed from the search path.
 *
 *   \param dir directory or archive previously added to the path, in
 *              platform-dependent notation. This must match the string
 *              used when adding, even if your string would also reference
 *              the same file with a different string of characters.
 *  \return READ-ONLY string of mount point if added to path, NULL on failure
 *          (bogus archive, etc). Use PHYSFS_getLastErrorCode() to obtain the
 *          specific error.
 *
 * \sa PHYSFS_removeFromSearchPath
 * \sa PHYSFS_getSearchPath
 * \sa PHYSFS_getMountPoint
 */
PHYSFS_DECL const char *PHYSFS_getMountPoint(const char *dir);


/**
 * \typedef PHYSFS_StringCallback
 * \brief Function signature for callbacks that report strings.
 *
 * These are used to report a list of strings to an original caller, one
 *  string per callback. All strings are UTF-8 encoded. Functions should not
 *  try to modify or free the string's memory.
 *
 * These callbacks are used, starting in PhysicsFS 1.1, as an alternative to
 *  functions that would return lists that need to be cleaned up with
 *  PHYSFS_freeList(). The callback means that the library doesn't need to
 *  allocate an entire list and all the strings up front.
 *
 * Be aware that promises data ordering in the list versions are not
 *  necessarily so in the callback versions. Check the documentation on
 *  specific APIs, but strings may not be sorted as you expect.
 *
 *    \param data User-defined data pointer, passed through from the API
 *                that eventually called the callback.
 *    \param str The string data about which the callback is meant to inform.
 *
 * \sa PHYSFS_getCdRomDirsCallback
 * \sa PHYSFS_getSearchPathCallback
 */
typedef void (*PHYSFS_StringCallback)(void *data, const char *str);


/**
 * \typedef PHYSFS_EnumFilesCallback
 * \brief Function signature for callbacks that enumerate files.
 *
 * \warning As of PhysicsFS 2.1, Use PHYSFS_EnumerateCallback with
 *  PHYSFS_enumerate() instead; it gives you more control over the process.
 *
 * These are used to report a list of directory entries to an original caller,
 *  one file/dir/symlink per callback. All strings are UTF-8 encoded.
 *  Functions should not try to modify or free any string's memory.
 *
 * These callbacks are used, starting in PhysicsFS 1.1, as an alternative to
 *  functions that would return lists that need to be cleaned up with
 *  PHYSFS_freeList(). The callback means that the library doesn't need to
 *  allocate an entire list and all the strings up front.
 *
 * Be aware that promised data ordering in the list versions are not
 *  necessarily so in the callback versions. Check the documentation on
 *  specific APIs, but strings may not be sorted as you expect and you might
 *  get duplicate strings.
 *
 *    \param data User-defined data pointer, passed through from the API
 *                that eventually called the callback.
 *    \param origdir A string containing the full path, in platform-independent
 *                   notation, of the directory containing this file. In most
 *                   cases, this is the directory on which you requested
 *                   enumeration, passed in the callback for your convenience.
 *    \param fname The filename that is being enumerated. It may not be in
 *                 alphabetical order compared to other callbacks that have
 *                 fired, and it will not contain the full path. You can
 *                 recreate the fullpath with $origdir/$fname ... The file
 *                 can be a subdirectory, a file, a symlink, etc.
 *
 * \sa PHYSFS_enumerateFilesCallback
 */
typedef void (*PHYSFS_EnumFilesCallback)(void *data, const char *origdir,
                                         const char *fname);


/**
 * \fn void PHYSFS_getCdRomDirsCallback(PHYSFS_StringCallback c, void *d)
 * \brief Enumerate CD-ROM directories, using an application-defined callback.
 *
 * Internally, PHYSFS_getCdRomDirs() just calls this function and then builds
 *  a list before returning to the application, so functionality is identical
 *  except for how the information is represented to the application.
 *
 * Unlike PHYSFS_getCdRomDirs(), this function does not return an array.
 *  Rather, it calls a function specified by the application once per
 *  detected disc:
 *
 * \code
 *
 * static void foundDisc(void *data, const char *cddir)
 * {
 *     printf("cdrom dir [%s] is available.\n", cddir);
 * }
 *
 * // ...
 * PHYSFS_getCdRomDirsCallback(foundDisc, NULL);
 * \endcode
 *
 * This call may block while drives spin up. Be forewarned.
 *
 *    \param c Callback function to notify about detected drives.
 *    \param d Application-defined data passed to callback. Can be NULL.
 *
 * \sa PHYSFS_StringCallback
 * \sa PHYSFS_getCdRomDirs
 */
PHYSFS_DECL void PHYSFS_getCdRomDirsCallback(PHYSFS_StringCallback c, void *d);


/**
 * \fn void PHYSFS_getSearchPathCallback(PHYSFS_StringCallback c, void *d)
 * \brief Enumerate the search path, using an application-defined callback.
 *
 * Internally, PHYSFS_getSearchPath() just calls this function and then builds
 *  a list before returning to the application, so functionality is identical
 *  except for how the information is represented to the application.
 *
 * Unlike PHYSFS_getSearchPath(), this function does not return an array.
 *  Rather, it calls a function specified by the application once per
 *  element of the search path:
 *
 * \code
 *
 * static void printSearchPath(void *data, const char *pathItem)
 * {
 *     printf("[%s] is in the search path.\n", pathItem);
 * }
 *
 * // ...
 * PHYSFS_getSearchPathCallback(printSearchPath, NULL);
 * \endcode
 *
 * Elements of the search path are reported in order search priority, so the
 *  first archive/dir that would be examined when looking for a file is the
 *  first element passed through the callback.
 *
 *    \param c Callback function to notify about search path elements.
 *    \param d Application-defined data passed to callback. Can be NULL.
 *
 * \sa PHYSFS_StringCallback
 * \sa PHYSFS_getSearchPath
 */
PHYSFS_DECL void PHYSFS_getSearchPathCallback(PHYSFS_StringCallback c, void *d);


/**
 * \fn void PHYSFS_enumerateFilesCallback(const char *dir, PHYSFS_EnumFilesCallback c, void *d)
 * \brief Get a file listing of a search path's directory, using an application-defined callback.
 *
 * \deprecated As of PhysicsFS 2.1, use PHYSFS_enumerate() instead. This
 *  function has no way to report errors (or to have the callback signal an
 *  error or request a stop), so if data will be lost, your callback has no
 *  way to direct the process, and your calling app has no way to know.
 *
 * As of PhysicsFS 2.1, this function just wraps PHYSFS_enumerate() and
 *  ignores errors. Consider using PHYSFS_enumerate() or
 *  PHYSFS_enumerateFiles() instead.
 *
 * \sa PHYSFS_enumerate
 * \sa PHYSFS_enumerateFiles
 * \sa PHYSFS_EnumFilesCallback
 */
PHYSFS_DECL void PHYSFS_enumerateFilesCallback(const char *dir,
                                               PHYSFS_EnumFilesCallback c,
                                               void *d) PHYSFS_DEPRECATED;

/**
 * \fn void PHYSFS_utf8FromUcs4(const PHYSFS_uint32 *src, char *dst, PHYSFS_uint64 len)
 * \brief Convert a UCS-4 string to a UTF-8 string.
 *
 * \warning This function will not report an error if there are invalid UCS-4
 *          values in the source string. It will replace them with a '?'
 *          character and continue on.
 *
 * UCS-4 (aka UTF-32) strings are 32-bits per character: \c wchar_t on Unix.
 *
 * To ensure that the destination buffer is large enough for the conversion,
 *  please allocate a buffer that is the same size as the source buffer. UTF-8
 *  never uses more than 32-bits per character, so while it may shrink a UCS-4
 *  string, it will never expand it.
 *
 * Strings that don't fit in the destination buffer will be truncated, but
 *  will always be null-terminated and never have an incomplete UTF-8
 *  sequence at the end. If the buffer length is 0, this function does nothing.
 *
 *   \param src Null-terminated source string in UCS-4 format.
 *   \param dst Buffer to store converted UTF-8 string.
 *   \param len Size, in bytes, of destination buffer.
 */
PHYSFS_DECL void PHYSFS_utf8FromUcs4(const PHYSFS_uint32 *src, char *dst,
                                     PHYSFS_uint64 len);

/**
 * \fn void PHYSFS_utf8ToUcs4(const char *src, PHYSFS_uint32 *dst, PHYSFS_uint64 len)
 * \brief Convert a UTF-8 string to a UCS-4 string.
 *
 * \warning This function will not report an error if there are invalid UTF-8
 *          sequences in the source string. It will replace them with a '?'
 *          character and continue on.
 *
 * UCS-4 (aka UTF-32) strings are 32-bits per character: \c wchar_t on Unix.
 *
 * To ensure that the destination buffer is large enough for the conversion,
 *  please allocate a buffer that is four times the size of the source buffer.
 *  UTF-8 uses from one to four bytes per character, but UCS-4 always uses
 *  four, so an entirely low-ASCII string will quadruple in size!
 *
 * Strings that don't fit in the destination buffer will be truncated, but
 *  will always be null-terminated and never have an incomplete UCS-4
 *  sequence at the end. If the buffer length is 0, this function does nothing.
 *
 *   \param src Null-terminated source string in UTF-8 format.
 *   \param dst Buffer to store converted UCS-4 string.
 *   \param len Size, in bytes, of destination buffer.
 */
PHYSFS_DECL void PHYSFS_utf8ToUcs4(const char *src, PHYSFS_uint32 *dst,
                                   PHYSFS_uint64 len);

/**
 * \fn void PHYSFS_utf8FromUcs2(const PHYSFS_uint16 *src, char *dst, PHYSFS_uint64 len)
 * \brief Convert a UCS-2 string to a UTF-8 string.
 *
 * \warning you almost certainly should use PHYSFS_utf8FromUtf16(), which
 *  became available in PhysicsFS 2.1, unless you know what you're doing.
 *
 * \warning This function will not report an error if there are invalid UCS-2
 *          values in the source string. It will replace them with a '?'
 *          character and continue on.
 *
 * UCS-2 strings are 16-bits per character: \c TCHAR on Windows, when building
 *  with Unicode support. Please note that modern versions of Windows use
 *  UTF-16, which is an extended form of UCS-2, and not UCS-2 itself. You
 *  almost certainly want PHYSFS_utf8FromUtf16() instead.
 *
 * To ensure that the destination buffer is large enough for the conversion,
 *  please allocate a buffer that is double the size of the source buffer.
 *  UTF-8 never uses more than 32-bits per character, so while it may shrink
 *  a UCS-2 string, it may also expand it.
 *
 * Strings that don't fit in the destination buffer will be truncated, but
 *  will always be null-terminated and never have an incomplete UTF-8
 *  sequence at the end. If the buffer length is 0, this function does nothing.
 *
 *   \param src Null-terminated source string in UCS-2 format.
 *   \param dst Buffer to store converted UTF-8 string.
 *   \param len Size, in bytes, of destination buffer.
 *
 * \sa PHYSFS_utf8FromUtf16
 */
PHYSFS_DECL void PHYSFS_utf8FromUcs2(const PHYSFS_uint16 *src, char *dst,
                                     PHYSFS_uint64 len);

/**
 * \fn PHYSFS_utf8ToUcs2(const char *src, PHYSFS_uint16 *dst, PHYSFS_uint64 len)
 * \brief Convert a UTF-8 string to a UCS-2 string.
 *
 * \warning you almost certainly should use PHYSFS_utf8ToUtf16(), which
 *  became available in PhysicsFS 2.1, unless you know what you're doing.
 *
 * \warning This function will not report an error if there are invalid UTF-8
 *          sequences in the source string. It will replace them with a '?'
 *          character and continue on.
 *
 * UCS-2 strings are 16-bits per character: \c TCHAR on Windows, when building
 *  with Unicode support. Please note that modern versions of Windows use
 *  UTF-16, which is an extended form of UCS-2, and not UCS-2 itself. You
 *  almost certainly want PHYSFS_utf8ToUtf16() instead, but you need to
 *  understand how that changes things, too.
 *
 * To ensure that the destination buffer is large enough for the conversion,
 *  please allocate a buffer that is double the size of the source buffer.
 *  UTF-8 uses from one to four bytes per character, but UCS-2 always uses
 *  two, so an entirely low-ASCII string will double in size!
 *
 * Strings that don't fit in the destination buffer will be truncated, but
 *  will always be null-terminated and never have an incomplete UCS-2
 *  sequence at the end. If the buffer length is 0, this function does nothing.
 *
 *   \param src Null-terminated source string in UTF-8 format.
 *   \param dst Buffer to store converted UCS-2 string.
 *   \param len Size, in bytes, of destination buffer.
 *
 * \sa PHYSFS_utf8ToUtf16
 */
PHYSFS_DECL void PHYSFS_utf8ToUcs2(const char *src, PHYSFS_uint16 *dst,
                                   PHYSFS_uint64 len);

/**
 * \fn void PHYSFS_utf8FromLatin1(const char *src, char *dst, PHYSFS_uint64 len)
 * \brief Convert a UTF-8 string to a Latin1 string.
 *
 * Latin1 strings are 8-bits per character: a popular "high ASCII" encoding.
 *
 * To ensure that the destination buffer is large enough for the conversion,
 *  please allocate a buffer that is double the size of the source buffer.
 *  UTF-8 expands latin1 codepoints over 127 from 1 to 2 bytes, so the string
 *  may grow in some cases.
 *
 * Strings that don't fit in the destination buffer will be truncated, but
 *  will always be null-terminated and never have an incomplete UTF-8
 *  sequence at the end. If the buffer length is 0, this function does nothing.
 *
 * Please note that we do not supply a UTF-8 to Latin1 converter, since Latin1
 *  can't express most Unicode codepoints. It's a legacy encoding; you should
 *  be converting away from it at all times.
 *
 *   \param src Null-terminated source string in Latin1 format.
 *   \param dst Buffer to store converted UTF-8 string.
 *   \param len Size, in bytes, of destination buffer.
 */
PHYSFS_DECL void PHYSFS_utf8FromLatin1(const char *src, char *dst,
                                       PHYSFS_uint64 len);

/* Everything above this line is part of the PhysicsFS 2.0 API. */

/**
 * \fn int PHYSFS_caseFold(const PHYSFS_uint32 from, PHYSFS_uint32 *to)
 * \brief "Fold" a Unicode codepoint to a lowercase equivalent.
 *
 * (This is for limited, hardcore use. If you don't immediately see a need
 *  for it, you can probably ignore this forever.)
 *
 * This will convert a Unicode codepoint into its lowercase equivalent.
 *  Bogus codepoints and codepoints without a lowercase equivalent will
 *  be returned unconverted.
 *
 * Note that you might get multiple codepoints in return! The German Eszett,
 *  for example, will fold down to two lowercase latin 's' codepoints. The
 *  theory is that if you fold two strings, one with an Eszett and one with
 *  "SS" down, they will match.
 *
 * \warning Anyone that is a student of Unicode knows about the "Turkish I"
 *          problem. This API does not handle it. Assume this one letter
 *          in all of Unicode will definitely fold sort of incorrectly. If
 *          you don't know what this is about, you can probably ignore this
 *          problem for most of the planet, but perfection is impossible.
 *
 *   \param from The codepoint to fold.
 *   \param to Buffer to store the folded codepoint values into. This should
 *             point to space for at least 3 PHYSFS_uint32 slots.
 *  \return The number of codepoints the folding produced. Between 1 and 3.
 */
PHYSFS_DECL int PHYSFS_caseFold(const PHYSFS_uint32 from, PHYSFS_uint32 *to);


/**
 * \fn int PHYSFS_utf8stricmp(const char *str1, const char *str2)
 * \brief Case-insensitive compare of two UTF-8 strings.
 *
 * This is a strcasecmp/stricmp replacement that expects both strings
 *  to be in UTF-8 encoding. It will do "case folding" to decide if the
 *  Unicode codepoints in the strings match.
 *
 * If both strings are exclusively low-ASCII characters, this will do the
 *  right thing, as that is also valid UTF-8. If there are any high-ASCII
 *  chars, this will not do what you expect!
 *
 * It will report which string is "greater than" the other, but be aware that
 *  this doesn't necessarily mean anything: 'a' may be "less than" 'b', but
 *  a Japanese kuten has no meaningful alphabetically relationship to
 *  a Greek lambda, but being able to assign a reliable "value" makes sorting
 *  algorithms possible, if not entirely sane. Most cases should treat the
 *  return value as "equal" or "not equal".
 *
 * Like stricmp, this expects both strings to be NULL-terminated.
 *
 *   \param str1 First string to compare.
 *   \param str2 Second string to compare.
 *  \return -1 if str1 is "less than" str2, 1 if "greater than", 0 if equal.
 */
PHYSFS_DECL int PHYSFS_utf8stricmp(const char *str1, const char *str2);

/**
 * \fn int PHYSFS_utf16stricmp(const PHYSFS_uint16 *str1, const PHYSFS_uint16 *str2)
 * \brief Case-insensitive compare of two UTF-16 strings.
 *
 * This is a strcasecmp/stricmp replacement that expects both strings
 *  to be in UTF-16 encoding. It will do "case folding" to decide if the
 *  Unicode codepoints in the strings match.
 *
 * It will report which string is "greater than" the other, but be aware that
 *  this doesn't necessarily mean anything: 'a' may be "less than" 'b', but
 *  a Japanese kuten has no meaningful alphabetically relationship to
 *  a Greek lambda, but being able to assign a reliable "value" makes sorting
 *  algorithms possible, if not entirely sane. Most cases should treat the
 *  return value as "equal" or "not equal".
 *
 * Like stricmp, this expects both strings to be NULL-terminated.
 *
 *   \param str1 First string to compare.
 *   \param str2 Second string to compare.
 *  \return -1 if str1 is "less than" str2, 1 if "greater than", 0 if equal.
 */
PHYSFS_DECL int PHYSFS_utf16stricmp(const PHYSFS_uint16 *str1,
                                    const PHYSFS_uint16 *str2);

/**
 * \fn int PHYSFS_ucs4stricmp(const PHYSFS_uint32 *str1, const PHYSFS_uint32 *str2)
 * \brief Case-insensitive compare of two UCS-4 strings.
 *
 * This is a strcasecmp/stricmp replacement that expects both strings
 *  to be in UCS-4 (aka UTF-32) encoding. It will do "case folding" to decide
 *  if the Unicode codepoints in the strings match.
 *
 * It will report which string is "greater than" the other, but be aware that
 *  this doesn't necessarily mean anything: 'a' may be "less than" 'b', but
 *  a Japanese kuten has no meaningful alphabetically relationship to
 *  a Greek lambda, but being able to assign a reliable "value" makes sorting
 *  algorithms possible, if not entirely sane. Most cases should treat the
 *  return value as "equal" or "not equal".
 *
 * Like stricmp, this expects both strings to be NULL-terminated.
 *
 *   \param str1 First string to compare.
 *   \param str2 Second string to compare.
 *  \return -1 if str1 is "less than" str2, 1 if "greater than", 0 if equal.
 */
PHYSFS_DECL int PHYSFS_ucs4stricmp(const PHYSFS_uint32 *str1,
                                   const PHYSFS_uint32 *str2);


/**
 * \typedef PHYSFS_EnumerateCallback
 * \brief Possible return values from PHYSFS_EnumerateCallback.
 *
 * These values dictate if an enumeration callback should continue to fire,
 *  or stop (and why it is stopping).
 *
 * \sa PHYSFS_EnumerateCallback
 * \sa PHYSFS_enumerate
 */
typedef enum PHYSFS_EnumerateCallbackResult
{
    PHYSFS_ENUM_ERROR = -1,   /**< Stop enumerating, report error to app. */
    PHYSFS_ENUM_STOP = 0,     /**< Stop enumerating, report success to app. */
    PHYSFS_ENUM_OK = 1        /**< Keep enumerating, no problems */
} PHYSFS_EnumerateCallbackResult;

/**
 * \typedef PHYSFS_EnumerateCallback
 * \brief Function signature for callbacks that enumerate and return results.
 *
 * This is the same thing as PHYSFS_EnumFilesCallback from PhysicsFS 2.0,
 *  except it can return a result from the callback: namely: if you're looking
 *  for something specific, once you find it, you can tell PhysicsFS to stop
 *  enumerating further. This is used with PHYSFS_enumerate(), which we
 *  hopefully got right this time.  :)
 *
 *    \param data User-defined data pointer, passed through from the API
 *                that eventually called the callback.
 *    \param origdir A string containing the full path, in platform-independent
 *                   notation, of the directory containing this file. In most
 *                   cases, this is the directory on which you requested
 *                   enumeration, passed in the callback for your convenience.
 *    \param fname The filename that is being enumerated. It may not be in
 *                 alphabetical order compared to other callbacks that have
 *                 fired, and it will not contain the full path. You can
 *                 recreate the fullpath with $origdir/$fname ... The file
 *                 can be a subdirectory, a file, a symlink, etc.
 *   \return A value from PHYSFS_EnumerateCallbackResult.
 *           All other values are (currently) undefined; don't use them.
 *
 * \sa PHYSFS_enumerate
 * \sa PHYSFS_EnumerateCallbackResult
 */
typedef PHYSFS_EnumerateCallbackResult (*PHYSFS_EnumerateCallback)(void *data,
                                       const char *origdir, const char *fname);

/**
 * \fn int PHYSFS_enumerate(const char *dir, PHYSFS_EnumerateCallback c, void *d)
 * \brief Get a file listing of a search path's directory, using an application-defined callback, with errors reported.
 *
 * Internally, PHYSFS_enumerateFiles() just calls this function and then builds
 *  a list before returning to the application, so functionality is identical
 *  except for how the information is represented to the application.
 *
 * Unlike PHYSFS_enumerateFiles(), this function does not return an array.
 *  Rather, it calls a function specified by the application once per
 *  element of the search path:
 *
 * \code
 *
 * static PHYSFS_EnumerateCallbackResult printDir(void *data, const char *origdir, const char *fname)
 * {
 *     printf(" * We've got [%s] in [%s].\n", fname, origdir);
 *     return PHYSFS_ENUM_OK;  // give me more data, please.
 * }
 *
 * // ...
 * PHYSFS_enumerate("/some/path", printDir, NULL);
 * \endcode
 *
 * Items sent to the callback are not guaranteed to be in any order whatsoever.
 *  There is no sorting done at this level, and if you need that, you should
 *  probably use PHYSFS_enumerateFiles() instead, which guarantees
 *  alphabetical sorting. This form reports whatever is discovered in each
 *  archive before moving on to the next. Even within one archive, we can't
 *  guarantee what order it will discover data. <em>Any sorting you find in
 *  these callbacks is just pure luck. Do not rely on it.</em> As this walks
 *  the entire list of archives, you may receive duplicate filenames.
 *
 * This API and the callbacks themselves are capable of reporting errors.
 *  Prior to this API, callbacks had to accept every enumerated item, even if
 *  they were only looking for a specific thing and wanted to stop after that,
 *  or had a serious error and couldn't alert anyone. Furthermore, if
 *  PhysicsFS itself had a problem (disk error or whatnot), it couldn't report
 *  it to the calling app, it would just have to skip items or stop
 *  enumerating outright, and the caller wouldn't know it had lost some data
 *  along the way.
 *
 * Now the caller can be sure it got a complete data set, and its callback has
 *  control if it wants enumeration to stop early. See the documentation for
 *  PHYSFS_EnumerateCallback for details on how your callback should behave.
 *
 *    \param dir Directory, in platform-independent notation, to enumerate.
 *    \param c Callback function to notify about search path elements.
 *    \param d Application-defined data passed to callback. Can be NULL.
 *   \return non-zero on success, zero on failure. Use
 *           PHYSFS_getLastErrorCode() to obtain the specific error. If the
 *           callback returns PHYSFS_ENUM_STOP to stop early, this will be
 *           considered success. Callbacks returning PHYSFS_ENUM_ERROR will
 *           make this function return zero and set the error code to
 *           PHYSFS_ERR_APP_CALLBACK.
 *
 * \sa PHYSFS_EnumerateCallback
 * \sa PHYSFS_enumerateFiles
 */
PHYSFS_DECL int PHYSFS_enumerate(const char *dir, PHYSFS_EnumerateCallback c,
                                 void *d);


/**
 * \fn int PHYSFS_unmount(const char *oldDir)
 * \brief Remove a directory or archive from the search path.
 *
 * This is functionally equivalent to PHYSFS_removeFromSearchPath(), but that
 *  function is deprecated to keep the vocabulary paired with PHYSFS_mount().
 *
 * This must be a (case-sensitive) match to a dir or archive already in the
 *  search path, specified in platform-dependent notation.
 *
 * This call will fail (and fail to remove from the path) if the element still
 *  has files open in it.
 *
 * \warning This function wants the path to the archive or directory that was
 *          mounted (the same string used for the "newDir" argument of
 *          PHYSFS_addToSearchPath or any of the mount functions), not the
 *          path where it is mounted in the tree (the "mountPoint" argument
 *          to any of the mount functions).
 *
 *    \param oldDir dir/archive to remove.
 *   \return nonzero on success, zero on failure. Use
 *           PHYSFS_getLastErrorCode() to obtain the specific error.
 *
 * \sa PHYSFS_getSearchPath
 * \sa PHYSFS_mount
 */
PHYSFS_DECL int PHYSFS_unmount(const char *oldDir);


/**
 * \fn const PHYSFS_Allocator *PHYSFS_getAllocator(void)
 * \brief Discover the current allocator.
 *
 * (This is for limited, hardcore use. If you don't immediately see a need
 *  for it, you can probably ignore this forever.)
 *
 * This function exposes the function pointers that make up the currently used
 *  allocator. This can be useful for apps that want to access PhysicsFS's
 *  internal, default allocation routines, as well as for external code that
 *  wants to share the same allocator, even if the application specified their
 *  own.
 *
 * This call is only valid between PHYSFS_init() and PHYSFS_deinit() calls;
 *  it will return NULL if the library isn't initialized. As we can't
 *  guarantee the state of the internal allocators unless the library is
 *  initialized, you shouldn't use any allocator returned here after a call
 *  to PHYSFS_deinit().
 *
 * Do not call the returned allocator's Init() or Deinit() methods under any
 *  circumstances.
 *
 * If you aren't immediately sure what to do with this function, you can
 *  safely ignore it altogether.
 *
 *  \return Current allocator, as set by PHYSFS_setAllocator(), or PhysicsFS's
 *          internal, default allocator if no application defined allocator
 *          is currently set. Will return NULL if the library is not
 *          initialized.
 *
 * \sa PHYSFS_Allocator
 * \sa PHYSFS_setAllocator
 */
PHYSFS_DECL const PHYSFS_Allocator *PHYSFS_getAllocator(void);


/**
 * \enum PHYSFS_FileType
 * \brief Type of a File
 *
 * Possible types of a file.
 *
 * \sa PHYSFS_stat
 */
typedef enum PHYSFS_FileType
{
    PHYSFS_FILETYPE_REGULAR, /**< a normal file */
    PHYSFS_FILETYPE_DIRECTORY, /**< a directory */
    PHYSFS_FILETYPE_SYMLINK, /**< a symlink */
    PHYSFS_FILETYPE_OTHER /**< something completely different like a device */
} PHYSFS_FileType;

/**
 * \struct PHYSFS_Stat
 * \brief Meta data for a file or directory
 *
 * Container for various meta data about a file in the virtual file system.
 *  PHYSFS_stat() uses this structure for returning the information. The time
 *  data will be either the number of seconds since the Unix epoch (midnight,
 *  Jan 1, 1970), or -1 if the information isn't available or applicable.
 *  The (filesize) field is measured in bytes.
 *  The (readonly) field tells you whether the archive thinks a file is
 *  not writable, but tends to be only an estimate (for example, your write
 *  dir might overlap with a .zip file, meaning you _can_ successfully open
 *  that path for writing, as it gets created elsewhere.
 *
 * \sa PHYSFS_stat
 * \sa PHYSFS_FileType
 */
typedef struct PHYSFS_Stat
{
    PHYSFS_sint64 filesize; /**< size in bytes, -1 for non-files and unknown */
    PHYSFS_sint64 modtime;  /**< last modification time */
    PHYSFS_sint64 createtime; /**< like modtime, but for file creation time */
    PHYSFS_sint64 accesstime; /**< like modtime, but for file access time */
    PHYSFS_FileType filetype; /**< File? Directory? Symlink? */
    int readonly; /**< non-zero if read only, zero if writable. */
} PHYSFS_Stat;

/**
 * \fn int PHYSFS_stat(const char *fname, PHYSFS_Stat *stat)
 * \brief Get various information about a directory or a file.
 *
 * Obtain various information about a file or directory from the meta data.
 *
 * This function will never follow symbolic links. If you haven't enabled
 *  symlinks with PHYSFS_permitSymbolicLinks(), stat'ing a symlink will be
 *  treated like stat'ing a non-existant file. If symlinks are enabled,
 *  stat'ing a symlink will give you information on the link itself and not
 *  what it points to.
 *
 *    \param fname filename to check, in platform-indepedent notation.
 *    \param stat pointer to structure to fill in with data about (fname).
 *   \return non-zero on success, zero on failure. On failure, (stat)'s
 *           contents are undefined.
 *
 * \sa PHYSFS_Stat
 */
PHYSFS_DECL int PHYSFS_stat(const char *fname, PHYSFS_Stat *stat);


/**
 * \fn void PHYSFS_utf8FromUtf16(const PHYSFS_uint16 *src, char *dst, PHYSFS_uint64 len)
 * \brief Convert a UTF-16 string to a UTF-8 string.
 *
 * \warning This function will not report an error if there are invalid UTF-16
 *          sequences in the source string. It will replace them with a '?'
 *          character and continue on.
 *
 * UTF-16 strings are 16-bits per character (except some chars, which are
 *  32-bits): \c TCHAR on Windows, when building with Unicode support. Modern
 *  Windows releases use UTF-16. Windows releases before 2000 used TCHAR, but
 *  only handled UCS-2. UTF-16 _is_ UCS-2, except for the characters that
 *  are 4 bytes, which aren't representable in UCS-2 at all anyhow. If you
 *  aren't sure, you should be using UTF-16 at this point on Windows.
 *
 * To ensure that the destination buffer is large enough for the conversion,
 *  please allocate a buffer that is double the size of the source buffer.
 *  UTF-8 never uses more than 32-bits per character, so while it may shrink
 *  a UTF-16 string, it may also expand it.
 *
 * Strings that don't fit in the destination buffer will be truncated, but
 *  will always be null-terminated and never have an incomplete UTF-8
 *  sequence at the end. If the buffer length is 0, this function does nothing.
 *
 *   \param src Null-terminated source string in UTF-16 format.
 *   \param dst Buffer to store converted UTF-8 string.
 *   \param len Size, in bytes, of destination buffer.
 */
PHYSFS_DECL void PHYSFS_utf8FromUtf16(const PHYSFS_uint16 *src, char *dst,
                                      PHYSFS_uint64 len);

/**
 * \fn PHYSFS_utf8ToUtf16(const char *src, PHYSFS_uint16 *dst, PHYSFS_uint64 len)
 * \brief Convert a UTF-8 string to a UTF-16 string.
 *
 * \warning This function will not report an error if there are invalid UTF-8
 *          sequences in the source string. It will replace them with a '?'
 *          character and continue on.
 *
 * UTF-16 strings are 16-bits per character (except some chars, which are
 *  32-bits): \c TCHAR on Windows, when building with Unicode support. Modern
 *  Windows releases use UTF-16. Windows releases before 2000 used TCHAR, but
 *  only handled UCS-2. UTF-16 _is_ UCS-2, except for the characters that
 *  are 4 bytes, which aren't representable in UCS-2 at all anyhow. If you
 *  aren't sure, you should be using UTF-16 at this point on Windows.
 *
 * To ensure that the destination buffer is large enough for the conversion,
 *  please allocate a buffer that is double the size of the source buffer.
 *  UTF-8 uses from one to four bytes per character, but UTF-16 always uses
 *  two to four, so an entirely low-ASCII string will double in size! The
 *  UTF-16 characters that would take four bytes also take four bytes in UTF-8,
 *  so you don't need to allocate 4x the space just in case: double will do.
 *
 * Strings that don't fit in the destination buffer will be truncated, but
 *  will always be null-terminated and never have an incomplete UTF-16
 *  surrogate pair at the end. If the buffer length is 0, this function does
 *  nothing.
 *
 *   \param src Null-terminated source string in UTF-8 format.
 *   \param dst Buffer to store converted UTF-16 string.
 *   \param len Size, in bytes, of destination buffer.
 *
 * \sa PHYSFS_utf8ToUtf16
 */
PHYSFS_DECL void PHYSFS_utf8ToUtf16(const char *src, PHYSFS_uint16 *dst,
                                    PHYSFS_uint64 len);


/**
 * \fn PHYSFS_sint64 PHYSFS_readBytes(PHYSFS_File *handle, void *buffer, PHYSFS_uint64 len)
 * \brief Read bytes from a PhysicsFS filehandle
 *
 * The file must be opened for reading.
 *
 *   \param handle handle returned from PHYSFS_openRead().
 *   \param buffer buffer of at least (len) bytes to store read data into.
 *   \param len number of bytes being read from (handle).
 *  \return number of bytes read. This may be less than (len); this does not
 *          signify an error, necessarily (a short read may mean EOF).
 *          PHYSFS_getLastErrorCode() can shed light on the reason this might
 *          be < (len), as can PHYSFS_eof(). -1 if complete failure.
 *
 * \sa PHYSFS_eof
 */
PHYSFS_DECL PHYSFS_sint64 PHYSFS_readBytes(PHYSFS_File *handle, void *buffer,
                                           PHYSFS_uint64 len);

/**
 * \fn PHYSFS_sint64 PHYSFS_writeBytes(PHYSFS_File *handle, const void *buffer, PHYSFS_uint64 len)
 * \brief Write data to a PhysicsFS filehandle
 *
 * The file must be opened for writing.
 *
 * Please note that while (len) is an unsigned 64-bit integer, you are limited
 *  to 63 bits (9223372036854775807 bytes), so we can return a negative value
 *  on error. If length is greater than 0x7FFFFFFFFFFFFFFF, this function will
 *  immediately fail. For systems without a 64-bit datatype, you are limited
 *  to 31 bits (0x7FFFFFFF, or 2147483647 bytes). We trust most things won't
 *  need to do multiple gigabytes of i/o in one call anyhow, but why limit
 *  things?
 *
 *   \param handle retval from PHYSFS_openWrite() or PHYSFS_openAppend().
 *   \param buffer buffer of (len) bytes to write to (handle).
 *   \param len number of bytes being written to (handle).
 *  \return number of bytes written. This may be less than (len); in the case
 *          of an error, the system may try to write as many bytes as possible,
 *          so an incomplete write might occur. PHYSFS_getLastErrorCode() can
 *          shed light on the reason this might be < (len). -1 if complete
 *          failure.
 */
PHYSFS_DECL PHYSFS_sint64 PHYSFS_writeBytes(PHYSFS_File *handle,
                                            const void *buffer,
                                            PHYSFS_uint64 len);


/**
 * \struct PHYSFS_Io
 * \brief An abstract i/o interface.
 *
 * \warning This is advanced, hardcore stuff. You don't need this unless you
 *          really know what you're doing. Most apps will not need this.
 *
 * Historically, PhysicsFS provided access to the physical filesystem and
 *  archives within that filesystem. However, sometimes you need more power
 *  than this. Perhaps you need to provide an archive that is entirely
 *  contained in RAM, or you need to bridge some other file i/o API to
 *  PhysicsFS, or you need to translate the bits (perhaps you have a
 *  a standard .zip file that's encrypted, and you need to decrypt on the fly
 *  for the unsuspecting zip archiver).
 *
 * A PHYSFS_Io is the interface that Archivers use to get archive data.
 *  Historically, this has mapped to file i/o to the physical filesystem, but
 *  as of PhysicsFS 2.1, applications can provide their own i/o implementations
 *  at runtime.
 *
 * This interface isn't necessarily a good universal fit for i/o. There are a
 *  few requirements of note:
 *
 *  - They only do blocking i/o (at least, for now).
 *  - They need to be able to duplicate. If you have a file handle from
 *    fopen(), you need to be able to create a unique clone of it (so we
 *    have two handles to the same file that can both seek/read/etc without
 *    stepping on each other).
 *  - They need to know the size of their entire data set.
 *  - They need to be able to seek and rewind on demand.
 *
 * ...in short, you're probably not going to write an HTTP implementation.
 *
 * Thread safety: PHYSFS_Io implementations are not guaranteed to be thread
 *  safe in themselves. Under the hood where PhysicsFS uses them, the library
 *  provides its own locks. If you plan to use them directly from separate
 *  threads, you should either use mutexes to protect them, or don't use the
 *  same PHYSFS_Io from two threads at the same time.
 *
 * \sa PHYSFS_mountIo
 */
typedef struct PHYSFS_Io
{
    /**
     * \brief Binary compatibility information.
     *
     * This must be set to zero at this time. Future versions of this
     *  struct will increment this field, so we know what a given
     *  implementation supports. We'll presumably keep supporting older
     *  versions as we offer new features, though.
     */
    PHYSFS_uint32 version;

    /**
     * \brief Instance data for this struct.
     *
     * Each instance has a pointer associated with it that can be used to
     *  store anything it likes. This pointer is per-instance of the stream,
     *  so presumably it will change when calling duplicate(). This can be
     *  deallocated during the destroy() method.
     */
    void *opaque;

    /**
     * \brief Read more data.
     *
     * Read (len) bytes from the interface, at the current i/o position, and
     *  store them in (buffer). The current i/o position should move ahead
     *  by the number of bytes successfully read.
     *
     * You don't have to implement this; set it to NULL if not implemented.
     *  This will only be used if the file is opened for reading. If set to
     *  NULL, a default implementation that immediately reports failure will
     *  be used.
     *
     *   \param io The i/o instance to read from.
     *   \param buf The buffer to store data into. It must be at least
     *                 (len) bytes long and can't be NULL.
     *   \param len The number of bytes to read from the interface.
     *  \return number of bytes read from file, 0 on EOF, -1 if complete
     *          failure.
     */
    PHYSFS_sint64 (*read)(struct PHYSFS_Io *io, void *buf, PHYSFS_uint64 len);

    /**
     * \brief Write more data.
     *
     * Write (len) bytes from (buffer) to the interface at the current i/o
     *  position. The current i/o position should move ahead by the number of
     *  bytes successfully written.
     *
     * You don't have to implement this; set it to NULL if not implemented.
     *  This will only be used if the file is opened for writing. If set to
     *  NULL, a default implementation that immediately reports failure will
     *  be used.
     *
     * You are allowed to buffer; a write can succeed here and then later
     *  fail when flushing. Note that PHYSFS_setBuffer() may be operating a
     *  level above your i/o, so you should usually not implement your
     *  own buffering routines.
     *
     *   \param io The i/o instance to write to.
     *   \param buffer The buffer to read data from. It must be at least
     *                 (len) bytes long and can't be NULL.
     *   \param len The number of bytes to read from (buffer).
     *  \return number of bytes written to file, -1 if complete failure.
     */
    PHYSFS_sint64 (*write)(struct PHYSFS_Io *io, const void *buffer,
                           PHYSFS_uint64 len);

    /**
     * \brief Move i/o position to a given byte offset from start.
     *
     * This method moves the i/o position, so the next read/write will
     *  be of the byte at (offset) offset. Seeks past the end of file should
     *  be treated as an error condition.
     *
     *   \param io The i/o instance to seek.
     *   \param offset The new byte offset for the i/o position.
     *  \return non-zero on success, zero on error.
     */
    int (*seek)(struct PHYSFS_Io *io, PHYSFS_uint64 offset);

    /**
     * \brief Report current i/o position.
     *
     * Return bytes offset, or -1 if you aren't able to determine. A failure
     *  will almost certainly be fatal to further use of this stream, so you
     *  may not leave this unimplemented.
     *
     *   \param io The i/o instance to query.
     *  \return The current byte offset for the i/o position, -1 if unknown.
     */
    PHYSFS_sint64 (*tell)(struct PHYSFS_Io *io);

    /**
     * \brief Determine size of the i/o instance's dataset.
     *
     * Return number of bytes available in the file, or -1 if you
     *  aren't able to determine. A failure will almost certainly be fatal
     *  to further use of this stream, so you may not leave this unimplemented.
     *
     *   \param io The i/o instance to query.
     *  \return Total size, in bytes, of the dataset.
     */
    PHYSFS_sint64 (*length)(struct PHYSFS_Io *io);

    /**
     * \brief Duplicate this i/o instance.
     *
     * This needs to result in a full copy of this PHYSFS_Io, that can live
     *  completely independently. The copy needs to be able to perform all
     *  its operations without altering the original, including either object
     *  being destroyed separately (so, for example: they can't share a file
     *  handle; they each need their own).
     *
     * If you can't duplicate a handle, it's legal to return NULL, but you
     *  almost certainly need this functionality if you want to use this to
     *  PHYSFS_Io to back an archive.
     *
     *   \param io The i/o instance to duplicate.
     *  \return A new value for a stream's (opaque) field, or NULL on error.
     */
    struct PHYSFS_Io *(*duplicate)(struct PHYSFS_Io *io);

    /**
     * \brief Flush resources to media, or wherever.
     *
     * This is the chance to report failure for writes that had claimed
     *  success earlier, but still had a chance to actually fail. This method
     *  can be NULL if flushing isn't necessary.
     *
     * This function may be called before destroy(), as it can report failure
     *  and destroy() can not. It may be called at other times, too.
     *
     *   \param io The i/o instance to flush.
     *  \return Zero on error, non-zero on success.
     */
    int (*flush)(struct PHYSFS_Io *io);

    /**
     * \brief Cleanup and deallocate i/o instance.
     *
     * Free associated resources, including (opaque) if applicable.
     *
     * This function must always succeed: as such, it returns void. The
     *  system may call your flush() method before this. You may report
     *  failure there if necessary. This method may still be called if
     *  flush() fails, in which case you'll have to abandon unflushed data
     *  and other failing conditions and clean up.
     *
     * Once this method is called for a given instance, the system will assume
     *  it is unsafe to touch that instance again and will discard any
     *  references to it.
     *
     *   \param s The i/o instance to destroy.
     */
    void (*destroy)(struct PHYSFS_Io *io);
} PHYSFS_Io;


/**
 * \fn int PHYSFS_mountIo(PHYSFS_Io *io, const char *newDir, const char *mountPoint, int appendToPath)
 * \brief Add an archive, built on a PHYSFS_Io, to the search path.
 *
 * \warning Unless you have some special, low-level need, you should be using
 *          PHYSFS_mount() instead of this.
 *
 * This function operates just like PHYSFS_mount(), but takes a PHYSFS_Io
 *  instead of a pathname. Behind the scenes, PHYSFS_mount() calls this
 *  function with a physical-filesystem-based PHYSFS_Io.
 *
 * (newDir) must be a unique string to identify this archive. It is used
 *  to optimize archiver selection (if you name it XXXXX.zip, we might try
 *  the ZIP archiver first, for example, or directly choose an archiver that
 *  can only trust the data is valid by filename extension). It doesn't
 *  need to refer to a real file at all. If the filename extension isn't
 *  helpful, the system will try every archiver until one works or none
 *  of them do. This filename must be unique, as the system won't allow you
 *  to have two archives with the same name.
 *
 * (io) must remain until the archive is unmounted. When the archive is
 *  unmounted, the system will call (io)->destroy(io), which will give you
 *  a chance to free your resources.
 *
 * If this function fails, (io)->destroy(io) is not called.
 *
 *   \param io i/o instance for archive to add to the path.
 *   \param newDir Filename that can represent this stream.
 *   \param mountPoint Location in the interpolated tree that this archive
 *                     will be "mounted", in platform-independent notation.
 *                     NULL or "" is equivalent to "/".
 *   \param appendToPath nonzero to append to search path, zero to prepend.
 *  \return nonzero if added to path, zero on failure (bogus archive, stream
 *                   i/o issue, etc). Use PHYSFS_getLastErrorCode() to obtain
 *                   the specific error.
 *
 * \sa PHYSFS_unmount
 * \sa PHYSFS_getSearchPath
 * \sa PHYSFS_getMountPoint
 */
PHYSFS_DECL int PHYSFS_mountIo(PHYSFS_Io *io, const char *newDir,
                               const char *mountPoint, int appendToPath);


/**
 * \fn int PHYSFS_mountMemory(const void *buf, PHYSFS_uint64 len, void (*del)(void *), const char *newDir, const char *mountPoint, int appendToPath)
 * \brief Add an archive, contained in a memory buffer, to the search path.
 *
 * \warning Unless you have some special, low-level need, you should be using
 *          PHYSFS_mount() instead of this.
 *
 * This function operates just like PHYSFS_mount(), but takes a memory buffer
 *  instead of a pathname. This buffer contains all the data of the archive,
 *  and is used instead of a real file in the physical filesystem.
 *
 * (newDir) must be a unique string to identify this archive. It is used
 *  to optimize archiver selection (if you name it XXXXX.zip, we might try
 *  the ZIP archiver first, for example, or directly choose an archiver that
 *  can only trust the data is valid by filename extension). It doesn't
 *  need to refer to a real file at all. If the filename extension isn't
 *  helpful, the system will try every archiver until one works or none
 *  of them do. This filename must be unique, as the system won't allow you
 *  to have two archives with the same name.
 *
 * (ptr) must remain until the archive is unmounted. When the archive is
 *  unmounted, the system will call (del)(ptr), which will notify you that
 *  the system is done with the buffer, and give you a chance to free your
 *  resources. (del) can be NULL, in which case the system will make no
 *  attempt to free the buffer.
 *
 * If this function fails, (del) is not called.
 *
 *   \param buf Address of the memory buffer containing the archive data.
 *   \param len Size of memory buffer, in bytes.
 *   \param del A callback that triggers upon unmount. Can be NULL.
 *   \param newDir Filename that can represent this stream.
 *   \param mountPoint Location in the interpolated tree that this archive
 *                     will be "mounted", in platform-independent notation.
 *                     NULL or "" is equivalent to "/".
 *   \param appendToPath nonzero to append to search path, zero to prepend.
 *  \return nonzero if added to path, zero on failure (bogus archive, etc).
 *          Use PHYSFS_getLastErrorCode() to obtain the specific error.
 *
 * \sa PHYSFS_unmount
 * \sa PHYSFS_getSearchPath
 * \sa PHYSFS_getMountPoint
 */
PHYSFS_DECL int PHYSFS_mountMemory(const void *buf, PHYSFS_uint64 len,
                                   void (*del)(void *), const char *newDir,
                                   const char *mountPoint, int appendToPath);


/**
 * \fn int PHYSFS_mountHandle(PHYSFS_File *file, const char *newDir, const char *mountPoint, int appendToPath)
 * \brief Add an archive, contained in a PHYSFS_File handle, to the search path.
 *
 * \warning Unless you have some special, low-level need, you should be using
 *          PHYSFS_mount() instead of this.
 *
 * \warning Archives-in-archives may be very slow! While a PHYSFS_File can
 *          seek even when the data is compressed, it may do so by rewinding
 *          to the start and decompressing everything before the seek point.
 *          Normal archive usage may do a lot of seeking behind the scenes.
 *          As such, you might find normal archive usage extremely painful
 *          if mounted this way. Plan accordingly: if you, say, have a
 *          self-extracting .zip file, and want to mount something in it,
 *          compress the contents of the inner archive and make sure the outer
 *          .zip file doesn't compress the inner archive too.
 *
 * This function operates just like PHYSFS_mount(), but takes a PHYSFS_File
 *  handle instead of a pathname. This handle contains all the data of the
 *  archive, and is used instead of a real file in the physical filesystem.
 *  The PHYSFS_File may be backed by a real file in the physical filesystem,
 *  but isn't necessarily. The most popular use for this is likely to mount
 *  archives stored inside other archives.
 *
 * (newDir) must be a unique string to identify this archive. It is used
 *  to optimize archiver selection (if you name it XXXXX.zip, we might try
 *  the ZIP archiver first, for example, or directly choose an archiver that
 *  can only trust the data is valid by filename extension). It doesn't
 *  need to refer to a real file at all. If the filename extension isn't
 *  helpful, the system will try every archiver until one works or none
 *  of them do. This filename must be unique, as the system won't allow you
 *  to have two archives with the same name.
 *
 * (file) must remain until the archive is unmounted. When the archive is
 *  unmounted, the system will call PHYSFS_close(file). If you need this
 *  handle to survive, you will have to wrap this in a PHYSFS_Io and use
 *  PHYSFS_mountIo() instead.
 *
 * If this function fails, PHYSFS_close(file) is not called.
 *
 *   \param file The PHYSFS_File handle containing archive data.
 *   \param newDir Filename that can represent this stream.
 *   \param mountPoint Location in the interpolated tree that this archive
 *                     will be "mounted", in platform-independent notation.
 *                     NULL or "" is equivalent to "/".
 *   \param appendToPath nonzero to append to search path, zero to prepend.
 *  \return nonzero if added to path, zero on failure (bogus archive, etc).
 *          Use PHYSFS_getLastErrorCode() to obtain the specific error.
 *
 * \sa PHYSFS_unmount
 * \sa PHYSFS_getSearchPath
 * \sa PHYSFS_getMountPoint
 */
PHYSFS_DECL int PHYSFS_mountHandle(PHYSFS_File *file, const char *newDir,
                                   const char *mountPoint, int appendToPath);


/**
 * \enum PHYSFS_ErrorCode
 * \brief Values that represent specific causes of failure.
 *
 * Most of the time, you should only concern yourself with whether a given
 *  operation failed or not, but there may be occasions where you plan to
 *  handle a specific failure case gracefully, so we provide specific error
 *  codes.
 *
 * Most of these errors are a little vague, and most aren't things you can
 *  fix...if there's a permission error, for example, all you can really do
 *  is pass that information on to the user and let them figure out how to
 *  handle it. In most these cases, your program should only care that it
 *  failed to accomplish its goals, and not care specifically why.
 *
 * \sa PHYSFS_getLastErrorCode
 * \sa PHYSFS_getErrorByCode
 */
typedef enum PHYSFS_ErrorCode
{
    PHYSFS_ERR_OK,               /**< Success; no error.                    */
    PHYSFS_ERR_OTHER_ERROR,      /**< Error not otherwise covered here.     */
    PHYSFS_ERR_OUT_OF_MEMORY,    /**< Memory allocation failed.             */
    PHYSFS_ERR_NOT_INITIALIZED,  /**< PhysicsFS is not initialized.         */
    PHYSFS_ERR_IS_INITIALIZED,   /**< PhysicsFS is already initialized.     */
    PHYSFS_ERR_ARGV0_IS_NULL,    /**< Needed argv[0], but it is NULL.       */
    PHYSFS_ERR_UNSUPPORTED,      /**< Operation or feature unsupported.     */
    PHYSFS_ERR_PAST_EOF,         /**< Attempted to access past end of file. */
    PHYSFS_ERR_FILES_STILL_OPEN, /**< Files still open.                     */
    PHYSFS_ERR_INVALID_ARGUMENT, /**< Bad parameter passed to an function.  */
    PHYSFS_ERR_NOT_MOUNTED,      /**< Requested archive/dir not mounted.    */
    PHYSFS_ERR_NOT_FOUND,        /**< File (or whatever) not found.         */
    PHYSFS_ERR_SYMLINK_FORBIDDEN,/**< Symlink seen when not permitted.      */
    PHYSFS_ERR_NO_WRITE_DIR,     /**< No write dir has been specified.      */
    PHYSFS_ERR_OPEN_FOR_READING, /**< Wrote to a file opened for reading.   */
    PHYSFS_ERR_OPEN_FOR_WRITING, /**< Read from a file opened for writing.  */
    PHYSFS_ERR_NOT_A_FILE,       /**< Needed a file, got a directory (etc). */
    PHYSFS_ERR_READ_ONLY,        /**< Wrote to a read-only filesystem.      */
    PHYSFS_ERR_CORRUPT,          /**< Corrupted data encountered.           */
    PHYSFS_ERR_SYMLINK_LOOP,     /**< Infinite symbolic link loop.          */
    PHYSFS_ERR_IO,               /**< i/o error (hardware failure, etc).    */
    PHYSFS_ERR_PERMISSION,       /**< Permission denied.                    */
    PHYSFS_ERR_NO_SPACE,         /**< No space (disk full, over quota, etc) */
    PHYSFS_ERR_BAD_FILENAME,     /**< Filename is bogus/insecure.           */
    PHYSFS_ERR_BUSY,             /**< Tried to modify a file the OS needs.  */
    PHYSFS_ERR_DIR_NOT_EMPTY,    /**< Tried to delete dir with files in it. */
    PHYSFS_ERR_OS_ERROR,         /**< Unspecified OS-level error.           */
    PHYSFS_ERR_DUPLICATE,        /**< Duplicate entry.                      */
    PHYSFS_ERR_BAD_PASSWORD,     /**< Bad password.                         */
    PHYSFS_ERR_APP_CALLBACK      /**< Application callback reported error.  */
} PHYSFS_ErrorCode;


/**
 * \fn PHYSFS_ErrorCode PHYSFS_getLastErrorCode(void)
 * \brief Get machine-readable error information.
 *
 * Get the last PhysicsFS error message as an integer value. This will return
 *  PHYSFS_ERR_OK if there's been no error since the last call to this
 *  function. Each thread has a unique error state associated with it, but
 *  each time a new error message is set, it will overwrite the previous one
 *  associated with that thread. It is safe to call this function at anytime,
 *  even before PHYSFS_init().
 *
 * PHYSFS_getLastError() and PHYSFS_getLastErrorCode() both reset the same
 *  thread-specific error state. Calling one will wipe out the other's
 *  data. If you need both, call PHYSFS_getLastErrorCode(), then pass that
 *  value to PHYSFS_getErrorByCode().
 *
 * Generally, applications should only concern themselves with whether a
 *  given function failed; however, if you require more specifics, you can
 *  try this function to glean information, if there's some specific problem
 *  you're expecting and plan to handle. But with most things that involve
 *  file systems, the best course of action is usually to give up, report the
 *  problem to the user, and let them figure out what should be done about it.
 *  For that, you might prefer PHYSFS_getErrorByCode() instead.
 *
 *   \return Enumeration value that represents last reported error.
 *
 * \sa PHYSFS_getErrorByCode
 */
PHYSFS_DECL PHYSFS_ErrorCode PHYSFS_getLastErrorCode(void);


/**
 * \fn const char *PHYSFS_getErrorByCode(PHYSFS_ErrorCode code)
 * \brief Get human-readable description string for a given error code.
 *
 * Get a static string, in UTF-8 format, that represents an English
 *  description of a given error code.
 *
 * This string is guaranteed to never change (although we may add new strings
 *  for new error codes in later versions of PhysicsFS), so you can use it
 *  for keying a localization dictionary.
 *
 * It is safe to call this function at anytime, even before PHYSFS_init().
 *
 * These strings are meant to be passed on directly to the user.
 *  Generally, applications should only concern themselves with whether a
 *  given function failed, but not care about the specifics much.
 *
 * Do not attempt to free the returned strings; they are read-only and you
 *  don't own their memory pages.
 *
 *   \param code Error code to convert to a string.
 *   \return READ ONLY string of requested error message, NULL if this
 *           is not a valid PhysicsFS error code. Always check for NULL if
 *           you might be looking up an error code that didn't exist in an
 *           earlier version of PhysicsFS.
 *
 * \sa PHYSFS_getLastErrorCode
 */
PHYSFS_DECL const char *PHYSFS_getErrorByCode(PHYSFS_ErrorCode code);

/**
 * \fn void PHYSFS_setErrorCode(PHYSFS_ErrorCode code)
 * \brief Set the current thread's error code.
 *
 * This lets you set the value that will be returned by the next call to
 *  PHYSFS_getLastErrorCode(). This will replace any existing error code,
 *  whether set by your application or internally by PhysicsFS.
 *
 * Error codes are stored per-thread; what you set here will not be
 *  accessible to another thread.
 *
 * Any call into PhysicsFS may change the current error code, so any code you
 *  set here is somewhat fragile, and thus you shouldn't build any serious
 *  error reporting framework on this function. The primary goal of this
 *  function is to allow PHYSFS_Io implementations to set the error state,
 *  which generally will be passed back to your application when PhysicsFS
 *  makes a PHYSFS_Io call that fails internally.
 *
 * This function doesn't care if the error code is a value known to PhysicsFS
 *  or not (but PHYSFS_getErrorByCode() will return NULL for unknown values).
 *  The value will be reported unmolested by PHYSFS_getLastErrorCode().
 *
 *   \param code Error code to become the current thread's new error state.
 *
 * \sa PHYSFS_getLastErrorCode
 * \sa PHYSFS_getErrorByCode
 */
PHYSFS_DECL void PHYSFS_setErrorCode(PHYSFS_ErrorCode code);


/**
 * \fn const char *PHYSFS_getPrefDir(const char *org, const char *app)
 * \brief Get the user-and-app-specific path where files can be written.
 *
 * Helper function.
 *
 * Get the "pref dir". This is meant to be where users can write personal
 *  files (preferences and save games, etc) that are specific to your
 *  application. This directory is unique per user, per application.
 *
 * This function will decide the appropriate location in the native filesystem,
 *  create the directory if necessary, and return a string in
 *  platform-dependent notation, suitable for passing to PHYSFS_setWriteDir().
 *
 * On Windows, this might look like:
 *  "C:\\Users\\bob\\AppData\\Roaming\\My Company\\My Program Name"
 *
 * On Linux, this might look like:
 *  "/home/bob/.local/share/My Program Name"
 *
 * On Mac OS X, this might look like:
 *  "/Users/bob/Library/Application Support/My Program Name"
 *
 * (etc.)
 *
 * You should probably use the pref dir for your write dir, and also put it
 *  near the beginning of your search path. Older versions of PhysicsFS
 *  offered only PHYSFS_getUserDir() and left you to figure out where the
 *  files should go under that tree. This finds the correct location
 *  for whatever platform, which not only changes between operating systems,
 *  but also versions of the same operating system.
 *
 * You specify the name of your organization (if it's not a real organization,
 *  your name or an Internet domain you own might do) and the name of your
 *  application. These should be proper names.
 *
 * Both the (org) and (app) strings may become part of a directory name, so
 *  please follow these rules:
 *
 *    - Try to use the same org string (including case-sensitivity) for
 *      all your applications that use this function.
 *    - Always use a unique app string for each one, and make sure it never
 *      changes for an app once you've decided on it.
 *    - Unicode characters are legal, as long as it's UTF-8 encoded, but...
 *    - ...only use letters, numbers, and spaces. Avoid punctuation like
 *      "Game Name 2: Bad Guy's Revenge!" ... "Game Name 2" is sufficient.
 *
 * The pointer returned by this function remains valid until you call this
 *  function again, or call PHYSFS_deinit(). This is not necessarily a fast
 *  call, though, so you should call this once at startup and copy the string
 *  if you need it.
 *
 * You should assume the path returned by this function is the only safe
 *  place to write files (and that PHYSFS_getUserDir() and PHYSFS_getBaseDir(),
 *  while they might be writable, or even parents of the returned path, aren't
 *  where you should be writing things).
 *
 *   \param org The name of your organization.
 *   \param app The name of your application.
 *  \return READ ONLY string of user dir in platform-dependent notation. NULL
 *          if there's a problem (creating directory failed, etc).
 *
 * \sa PHYSFS_getBaseDir
 * \sa PHYSFS_getUserDir
 */
PHYSFS_DECL const char *PHYSFS_getPrefDir(const char *org, const char *app);


/**
 * \struct PHYSFS_Archiver
 * \brief Abstract interface to provide support for user-defined archives.
 *
 * \warning This is advanced, hardcore stuff. You don't need this unless you
 *          really know what you're doing. Most apps will not need this.
 *
 * Historically, PhysicsFS provided a means to mount various archive file
 *  formats, and physical directories in the native filesystem. However,
 *  applications have been limited to the file formats provided by the
 *  library. This interface allows an application to provide their own
 *  archive file types.
 *
 * Conceptually, a PHYSFS_Archiver provides directory entries, while
 *  PHYSFS_Io provides data streams for those directory entries. The most
 *  obvious use of PHYSFS_Archiver is to provide support for an archive
 *  file type that isn't provided by PhysicsFS directly: perhaps some
 *  proprietary format that only your application needs to understand.
 *
 * Internally, all the built-in archive support uses this interface, so the
 *  best examples for building a PHYSFS_Archiver is the source code to
 *  PhysicsFS itself.
 *
 * An archiver is added to the system with PHYSFS_registerArchiver(), and then
 *  it will be available for use automatically with PHYSFS_mount(); if a
 *  given archive can be handled with your archiver, it will be given control
 *  as appropriate.
 *
 * These methods deal with dir handles. You have one instance of your
 *  archiver, and it generates a unique, opaque handle for each opened
 *  archive in its openArchive() method. Since the lifetime of an Archiver
 *  (not an archive) is generally the entire lifetime of the process, and it's
 *  assumed to be a singleton, we do not provide any instance data for the
 *  archiver itself; the app can just use some static variables if necessary.
 *
 * Symlinks should always be followed (except in stat()); PhysicsFS will
 *  use the stat() method to check for symlinks and make a judgement on
 *  whether to continue to call other methods based on that.
 *
 * Archivers, when necessary, should set the PhysicsFS error state with
 *  PHYSFS_setErrorCode() before returning. PhysicsFS will pass these errors
 *  back to the application unmolested in most cases.
 *
 * Thread safety: PHYSFS_Archiver implementations are not guaranteed to be
 *  thread safe in themselves. PhysicsFS provides thread safety when it calls
 *  into a given archiver inside the library, but it does not promise that
 *  using the same PHYSFS_File from two threads at once is thread-safe; as
 *  such, your PHYSFS_Archiver can assume that locking is handled for you
 *  so long as the PHYSFS_Io you return from PHYSFS_open* doesn't change any
 *  of your Archiver state, as the PHYSFS_Io won't be as aggressively
 *  protected.
 *
 * \sa PHYSFS_registerArchiver
 * \sa PHYSFS_deregisterArchiver
 * \sa PHYSFS_supportedArchiveTypes
 */
typedef struct PHYSFS_Archiver
{
    /**
     * \brief Binary compatibility information.
     *
     * This must be set to zero at this time. Future versions of this
     *  struct will increment this field, so we know what a given
     *  implementation supports. We'll presumably keep supporting older
     *  versions as we offer new features, though.
     */
    PHYSFS_uint32 version;

    /**
     * \brief Basic info about this archiver.
     *
     * This is used to identify your archive, and is returned in
     *  PHYSFS_supportedArchiveTypes().
     */
    PHYSFS_ArchiveInfo info;

    /**
     * \brief Open an archive provided by (io).
     *
     * This is where resources are allocated and data is parsed when mounting
     *  an archive.
     * (name) is a filename associated with (io), but doesn't necessarily
     *  map to anything, let alone a real filename. This possibly-
     *  meaningless name is in platform-dependent notation.
     * (forWrite) is non-zero if this is to be used for
     *  the write directory, and zero if this is to be used for an
     *  element of the search path.
     * (claimed) should be set to 1 if this is definitely an archive your
     *  archiver implementation can handle, even if it fails. We use to
     *  decide if we should stop trying other archivers if you fail to open
     *  it. For example: the .zip archiver will set this to 1 for something
     *  that's got a .zip file signature, even if it failed because the file
     *  was also truncated. No sense in trying other archivers here, we
     *  already tried to handle it with the appropriate implementation!.
     * Return NULL on failure and set (claimed) appropriately. If no archiver
     *  opened the archive or set (claimed), PHYSFS_mount() will report
     *  PHYSFS_ERR_UNSUPPORTED. Otherwise, it will report the error from the
     *  archiver that claimed the data through (claimed).
     * Return non-NULL on success. The pointer returned will be
     *  passed as the "opaque" parameter for later calls.
     */
    void *(*openArchive)(PHYSFS_Io *io, const char *name,
                         int forWrite, int *claimed);

    /**
     * \brief List all files in (dirname).
     *
     * Each file is passed to (cb), where a copy is made if appropriate, so
     *  you can dispose of it upon return from the callback. (dirname) is in
     *  platform-independent notation.
     * If you have a failure, call PHYSFS_SetErrorCode() with whatever code
     *  seem appropriate and return PHYSFS_ENUM_ERROR.
     * If the callback returns PHYSFS_ENUM_ERROR, please call
     *  PHYSFS_SetErrorCode(PHYSFS_ERR_APP_CALLBACK) and then return
     *  PHYSFS_ENUM_ERROR as well. Don't call the callback again in any
     *  circumstances.
     * If the callback returns PHYSFS_ENUM_STOP, stop enumerating and return
     *  PHYSFS_ENUM_STOP as well. Don't call the callback again in any
     *  circumstances. Don't set an error code in this case.
     * Callbacks are only supposed to return a value from
     *  PHYSFS_EnumerateCallbackResult. Any other result has undefined
     *  behavior.
     * As long as the callback returned PHYSFS_ENUM_OK and you haven't
     *  experienced any errors of your own, keep enumerating until you're done
     *  and then return PHYSFS_ENUM_OK without setting an error code.
     *
     * \warning PHYSFS_enumerate returns zero or non-zero (success or failure),
     *          so be aware this function pointer returns different values!
     */
    PHYSFS_EnumerateCallbackResult (*enumerate)(void *opaque,
                     const char *dirname, PHYSFS_EnumerateCallback cb,
                     const char *origdir, void *callbackdata);

    /**
     * \brief Open a file in this archive for reading.
     *
     * This filename, (fnm), is in platform-independent notation.
     * Fail if the file does not exist.
     * Returns NULL on failure, and calls PHYSFS_setErrorCode().
     *  Returns non-NULL on success. The pointer returned will be
     *  passed as the "opaque" parameter for later file calls.
     */
    PHYSFS_Io *(*openRead)(void *opaque, const char *fnm);

    /**
     * \brief Open a file in this archive for writing.
     *
     * If the file does not exist, it should be created. If it exists,
     *  it should be truncated to zero bytes. The writing offset should
     *  be the start of the file.
     * If the archive is read-only, this operation should fail.
     * This filename is in platform-independent notation.
     * Returns NULL on failure, and calls PHYSFS_setErrorCode().
     *  Returns non-NULL on success. The pointer returned will be
     *  passed as the "opaque" parameter for later file calls.
     */
    PHYSFS_Io *(*openWrite)(void *opaque, const char *filename);

    /**
     * \brief Open a file in this archive for appending.
     *
     * If the file does not exist, it should be created. The writing
     *  offset should be the end of the file.
     * If the archive is read-only, this operation should fail.
     * This filename is in platform-independent notation.
     * Returns NULL on failure, and calls PHYSFS_setErrorCode().
     *  Returns non-NULL on success. The pointer returned will be
     *  passed as the "opaque" parameter for later file calls.
     */
    PHYSFS_Io *(*openAppend)(void *opaque, const char *filename);

    /**
     * \brief Delete a file or directory in the archive.
     *
     * This same call is used for both files and directories; there is not a
     *  separate rmdir() call. Directories are only meant to be removed if
     *  they are empty.
     * If the archive is read-only, this operation should fail.
     *
     * Return non-zero on success, zero on failure.
     * This filename is in platform-independent notation.
     * On failure, call PHYSFS_setErrorCode().
     */
    int (*remove)(void *opaque, const char *filename);

    /**
     * \brief Create a directory in the archive.
     *
     * If the application is trying to make multiple dirs, PhysicsFS
     *  will split them up into multiple calls before passing them to
     *  your driver.
     * If the archive is read-only, this operation should fail.
     * Return non-zero on success, zero on failure.
     *  This filename is in platform-independent notation.
     * On failure, call PHYSFS_setErrorCode().
     */
    int (*mkdir)(void *opaque, const char *filename);

    /**
     * \brief Obtain basic file metadata.
     *
     * On success, fill in all the fields in (stat), using
     *  reasonable defaults for fields that apply to your archive.
     *
     * Returns non-zero on success, zero on failure.
     * This filename is in platform-independent notation.
     * On failure, call PHYSFS_setErrorCode().
     */
    int (*stat)(void *opaque, const char *fn, PHYSFS_Stat *stat);

    /**
     * \brief Destruct a previously-opened archive.
     *
     * Close this archive, and free any associated memory,
     *  including the original PHYSFS_Io and (opaque) itself, if
     *  applicable. Implementation can assume that it won't be called if
     *  there are still files open from this archive.
     */
    void (*closeArchive)(void *opaque);
} PHYSFS_Archiver;

/**
 * \fn int PHYSFS_registerArchiver(const PHYSFS_Archiver *archiver)
 * \brief Add a new archiver to the system.
 *
 * \warning This is advanced, hardcore stuff. You don't need this unless you
 *          really know what you're doing. Most apps will not need this.
 *
 * If you want to provide your own archiver (for example, a custom archive
 *  file format, or some virtual thing you want to make look like a filesystem
 *  that you can access through the usual PhysicsFS APIs), this is where you
 *  start. Once an archiver is successfully registered, then you can use
 *  PHYSFS_mount() to add archives that your archiver supports to the
 *  search path, or perhaps use it as the write dir. Internally, PhysicsFS
 *  uses this function to register its own built-in archivers, like .zip
 *  support, etc.
 *
 * You may not have two archivers that handle the same extension. If you are
 *  going to have a clash, you can deregister the other archiver (including
 *  built-in ones) with PHYSFS_deregisterArchiver().
 *
 * The data in (archiver) is copied; you may free this pointer when this
 *  function returns.
 *
 * Once this function returns successfully, PhysicsFS will be able to support
 *  archives of this type until you deregister the archiver again.
 *
 *   \param archiver The archiver to register.
 *  \return Zero on error, non-zero on success.
 *
 * \sa PHYSFS_Archiver
 * \sa PHYSFS_deregisterArchiver
 */
PHYSFS_DECL int PHYSFS_registerArchiver(const PHYSFS_Archiver *archiver);

/**
 * \fn int PHYSFS_deregisterArchiver(const char *ext)
 * \brief Remove an archiver from the system.
 *
 * If for some reason, you only need your previously-registered archiver to
 *  live for a portion of your app's lifetime, you can remove it from the
 *  system once you're done with it through this function.
 *
 * This fails if there are any archives still open that use this archiver.
 *
 * This function can also remove internally-supplied archivers, like .zip
 *  support or whatnot. This could be useful in some situations, like
 *  disabling support for them outright or overriding them with your own
 *  implementation. Once an internal archiver is disabled like this,
 *  PhysicsFS provides no mechanism to recover them, short of calling
 *  PHYSFS_deinit() and PHYSFS_init() again.
 *
 * PHYSFS_deinit() will automatically deregister all archivers, so you don't
 *  need to explicitly deregister yours if you otherwise shut down cleanly.
 *
 *   \param ext Filename extension that the archiver handles.
 *  \return Zero on error, non-zero on success.
 *
 * \sa PHYSFS_Archiver
 * \sa PHYSFS_registerArchiver
 */
PHYSFS_DECL int PHYSFS_deregisterArchiver(const char *ext);


/* Everything above this line is part of the PhysicsFS 2.1 API. */


/**
 * \fn int PHYSFS_setRoot(const char *archive, const char *subdir)
 * \brief Make a subdirectory of an archive its root directory.
 *
 * This lets you narrow down the accessible files in a specific archive. For
 *  example, if you have x.zip with a file in y/z.txt, mounted to /a, if you
 *  call PHYSFS_setRoot("x.zip", "/y"), then the call
 *  PHYSFS_openRead("/a/z.txt") will succeed.
 *
 * You can change an archive's root at any time, altering the interpolated
 *  file tree (depending on where paths shift, a different archive may be
 *  providing various files). If you set the root to NULL or "/", the
 *  archive will be treated as if no special root was set (as if the archive
 *  was just mounted normally).
 *
 * Changing the root only affects future operations on pathnames; a file
 *  that was opened from a path that changed due to a setRoot will not be
 *  affected.
 *
 * Setting a new root is not limited to archives in the search path; you may
 *  set one on the write dir, too, which might be useful if you have files
 *  open for write and thus can't change the write dir at the moment.
 *
 * It is not an error to set a subdirectory that does not exist to be the
 *  root of an archive; however, no files will be visible in this case. If
 *  the missing directories end up getting created (a mkdir to the physical
 *  filesystem, etc) then this will be reflected in the interpolated tree.
 *
 *    \param archive dir/archive on which to change root.
 *    \param subdir new subdirectory to make the root of this archive.
 *   \return nonzero on success, zero on failure. Use
 *           PHYSFS_getLastErrorCode() to obtain the specific error.
 */
PHYSFS_DECL int PHYSFS_setRoot(const char *archive, const char *subdir);


/* Everything above this line is part of the PhysicsFS 3.1 API. */


#ifdef __cplusplus
}
#endif

#endif  /* !defined _INCLUDE_PHYSFS_H_ */

/* end of physfs.h ... */


#if defined(PHYSFS_IMPL) || defined(PHYSFS_PLATFORM_IMPL)

#ifdef PHYSFS_SUPPORTS_ONLY_ZIP
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_ZIP 1
#endif
#ifdef PHYSFS_SUPPORTS_ONLY_7Z
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_7Z 1
#endif
#ifdef PHYSFS_SUPPORTS_ONLY_GRP
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_GRP 1
#endif
#ifdef PHYSFS_SUPPORTS_ONLY_WAD
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_WAD 1
#endif
#ifdef PHYSFS_SUPPORTS_ONLY_HOG
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_HOG 1
#endif
#ifdef PHYSFS_SUPPORTS_ONLY_MVL
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_MVL 1
#endif
#ifdef PHYSFS_SUPPORTS_ONLY_QPAK
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_QPAK 1
#endif
#ifdef PHYSFS_SUPPORTS_ONLY_SLB
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_SLB 1
#endif
#ifdef PHYSFS_SUPPORTS_ONLY_ISO9660
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_ISO966010
#endif
#ifdef PHYSFS_SUPPORTS_ONLY_VDF
#define PHYSFS_SUPPORTS_DEFAULT 0
#define PHYSFS_SUPPORTS_VDF 1
#endif

#ifdef PHYSFS_SUPPORTS_NO_ZIP
#define PHYSFS_SUPPORTS_ZIP 0
#endif
#ifdef PHYSFS_SUPPORTS_NO_7Z
#define PHYSFS_SUPPORTS_7Z 0
#endif
#ifdef PHYSFS_SUPPORTS_NO_GRP
#define PHYSFS_SUPPORTS_GRP 0
#endif
#ifdef PHYSFS_SUPPORTS_NO_WAD
#define PHYSFS_SUPPORTS_WAD 0
#endif
#ifdef PHYSFS_SUPPORTS_NO_HOG
#define PHYSFS_SUPPORTS_HOG 0
#endif
#ifdef PHYSFS_SUPPORTS_NO_MVL
#define PHYSFS_SUPPORTS_MVL 0
#endif
#ifdef PHYSFS_SUPPORTS_NO_QPAK
#define PHYSFS_SUPPORTS_QPAK 0
#endif
#ifdef PHYSFS_SUPPORTS_NO_SLB
#define PHYSFS_SUPPORTS_SLB 0
#endif
#ifdef PHYSFS_SUPPORTS_NO_ISO9660
#define PHYSFS_SUPPORTS_ISO9660 0
#endif
#ifdef PHYSFS_SUPPORTS_NO_VDF
#define PHYSFS_SUPPORTS_VDF 0
#endif

#define __PHYSICSFS_INTERNAL__

/*
 * Internal function/structure declaration. Do NOT include in your
 *  application.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

#ifndef _INCLUDE_PHYSFS_INTERNAL_H_
#define _INCLUDE_PHYSFS_INTERNAL_H_

#ifndef __PHYSICSFS_INTERNAL__
#error Do not include this header from your applications.
#endif

/* Turn off MSVC warnings that are aggressively anti-portability. */
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS 1
#endif

/*#include "physfs.h"*/

/* The holy trinity. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*#include "physfs_platforms.h"*/
#ifndef _INCL_PHYSFS_PLATFORMS
#define _INCL_PHYSFS_PLATFORMS

#ifndef __PHYSICSFS_INTERNAL__
#error Do not include this header from your applications.
#endif

/*
 * These only define the platforms to determine which files in the platforms
 *  directory should be compiled. For example, technically BeOS can be called
 *  a "unix" system, but since it doesn't use unix.c, we don't define
 *  PHYSFS_PLATFORM_UNIX on that system.
 */

#if (defined __HAIKU__)
#  define PHYSFS_PLATFORM_HAIKU 1
#  define PHYSFS_PLATFORM_POSIX 1
#elif ((defined __BEOS__) || (defined __beos__))
#  error BeOS support was dropped since PhysicsFS 2.1. Sorry. Try Haiku!
#elif (defined _WIN32_WCE) || (defined _WIN64_WCE)
#  error PocketPC support was dropped since PhysicsFS 2.1. Sorry. Try WinRT!
#elif (defined(_MSC_VER) && (_MSC_VER >= 1700) && !_USING_V110_SDK71_)  /* _MSC_VER==1700 for MSVC 2012 */
#  include <winapifamily.h>
#  define PHYSFS_PLATFORM_WINDOWS 1
#  if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#    define PHYSFS_NO_CDROM_SUPPORT 1
#    define PHYSFS_PLATFORM_WINRT 1
#  endif
#elif (((defined _WIN32) || (defined _WIN64)) && (!defined __CYGWIN__))
#  define PHYSFS_PLATFORM_WINDOWS 1
#elif defined(__OS2__) || defined(OS2)
#  define PHYSFS_PLATFORM_OS2 1
#elif ((defined __MACH__) && (defined __APPLE__))
/* To check if iOS or not, we need to include this file */
#  include <TargetConditionals.h>
#  if ((TARGET_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE))
#     define PHYSFS_NO_CDROM_SUPPORT 1
#  endif
#  define PHYSFS_PLATFORM_APPLE 1
#  define PHYSFS_PLATFORM_POSIX 1
#elif defined(macintosh)
#  error Classic Mac OS support was dropped from PhysicsFS 2.0. Move to OS X.
#elif defined(__ANDROID__)
 #  define PHYSFS_PLATFORM_LINUX 1
 #  define PHYSFS_PLATFORM_ANDROID 1
 #  define PHYSFS_PLATFORM_POSIX 1
 #  define PHYSFS_NO_CDROM_SUPPORT 1
#elif defined(__linux)
#  define PHYSFS_PLATFORM_LINUX 1
#  define PHYSFS_PLATFORM_UNIX 1
#  define PHYSFS_PLATFORM_POSIX 1
#elif defined(__sun) || defined(sun)
#  define PHYSFS_PLATFORM_SOLARIS 1
#  define PHYSFS_PLATFORM_UNIX 1
#  define PHYSFS_PLATFORM_POSIX 1
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__DragonFly__)
#  define PHYSFS_PLATFORM_FREEBSD 1
#  define PHYSFS_PLATFORM_BSD 1
#  define PHYSFS_PLATFORM_UNIX 1
#  define PHYSFS_PLATFORM_POSIX 1
#elif defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__)
#  define PHYSFS_PLATFORM_BSD 1
#  define PHYSFS_PLATFORM_UNIX 1
#  define PHYSFS_PLATFORM_POSIX 1
#elif defined(__EMSCRIPTEN__)
#  define PHYSFS_NO_CDROM_SUPPORT 1
#  define PHYSFS_PLATFORM_UNIX 1
#  define PHYSFS_PLATFORM_POSIX 1
#elif defined(__QNX__)
#  define PHYSFS_PLATFORM_QNX 1
#  define PHYSFS_PLATFORM_POSIX 1
#elif defined(unix) || defined(__unix__)
#  define PHYSFS_PLATFORM_UNIX 1
#  define PHYSFS_PLATFORM_POSIX 1
#else
#  error Unknown platform.
#endif

#endif  /* include-once blocker. */


#include <assert.h>

#define __PHYSFS_COMPILE_TIME_ASSERT(name, x) \
       typedef int __PHYSFS_compile_time_assert_##name[(x) * 2 - 1]

/* !!! FIXME: remove this when revamping stack allocation code... */
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__WATCOMC__)
#include <malloc.h>
#endif

#ifdef PHYSFS_PLATFORM_SOLARIS
#include <alloca.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define PHYSFS_MINIMUM_GCC_VERSION(major, minor) \
    ( ((__GNUC__ << 16) + __GNUC_MINOR__) >= (((major) << 16) + (minor)) )
#else
#define PHYSFS_MINIMUM_GCC_VERSION(major, minor) (0)
#endif

#ifdef __cplusplus
    /* C++ always has a real inline keyword. */
#elif (defined macintosh) && !(defined __MWERKS__)
#   define inline
#elif (defined _MSC_VER)
#   define inline __inline
#endif

#if defined(PHYSFS_PLATFORM_LINUX) && !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64
#endif

/* All public APIs need to be in physfs.h with a PHYSFS_DECL.
   All file-private symbols need to be marked "static".
   Everything shared between PhysicsFS sources needs to be in this
   file between the visibility pragma blocks. */
#if PHYSFS_MINIMUM_GCC_VERSION(4,0) || defined(__clang__)
#define PHYSFS_HAVE_PRAGMA_VISIBILITY 1
#endif

#if PHYSFS_HAVE_PRAGMA_VISIBILITY
#pragma GCC visibility push(hidden)
#endif

/* These are the build-in archivers. We list them all as "extern" here without
   #ifdefs to keep it tidy, but obviously you need to make sure these are
   wrapped in PHYSFS_SUPPORTS_* checks before actually referencing them. */
extern const PHYSFS_Archiver __PHYSFS_Archiver_DIR;
extern const PHYSFS_Archiver __PHYSFS_Archiver_ZIP;
extern const PHYSFS_Archiver __PHYSFS_Archiver_7Z;
extern const PHYSFS_Archiver __PHYSFS_Archiver_GRP;
extern const PHYSFS_Archiver __PHYSFS_Archiver_QPAK;
extern const PHYSFS_Archiver __PHYSFS_Archiver_HOG;
extern const PHYSFS_Archiver __PHYSFS_Archiver_MVL;
extern const PHYSFS_Archiver __PHYSFS_Archiver_WAD;
extern const PHYSFS_Archiver __PHYSFS_Archiver_SLB;
extern const PHYSFS_Archiver __PHYSFS_Archiver_ISO9660;
extern const PHYSFS_Archiver __PHYSFS_Archiver_VDF;

/* a real C99-compliant snprintf() is in Visual Studio 2015,
   but just use this everywhere for binary compatibility. */
#if defined(_MSC_VER)
#include <stdarg.h>
int __PHYSFS_msvc_vsnprintf(char *outBuf, size_t size, const char *format, va_list ap);
int __PHYSFS_msvc_snprintf(char *outBuf, size_t size, const char *format, ...);
#define vsnprintf __PHYSFS_msvc_vsnprintf
#define snprintf __PHYSFS_msvc_snprintf
#endif

/* Some simple wrappers around WinRT C++ interfaces we can call from C. */
#ifdef PHYSFS_PLATFORM_WINRT
const void *__PHYSFS_winrtCalcBaseDir(void);
const void *__PHYSFS_winrtCalcPrefDir(void);
#endif

/* atomic operations. */
#if defined(_MSC_VER) && (_MSC_VER >= 1500)
#include <intrin.h>
__PHYSFS_COMPILE_TIME_ASSERT(LongEqualsInt, sizeof (int) == sizeof (long));
#define __PHYSFS_ATOMIC_INCR(ptrval) _InterlockedIncrement((long*)(ptrval))
#define __PHYSFS_ATOMIC_DECR(ptrval) _InterlockedDecrement((long*)(ptrval))
#elif defined(__clang__) || (defined(__GNUC__) && (((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100)) >= 40100))
#define __PHYSFS_ATOMIC_INCR(ptrval) __sync_fetch_and_add(ptrval, 1)
#define __PHYSFS_ATOMIC_DECR(ptrval) __sync_fetch_and_add(ptrval, -1)
#else
#define PHYSFS_NEED_ATOMIC_OP_FALLBACK 1
int __PHYSFS_ATOMIC_INCR(int *ptrval);
int __PHYSFS_ATOMIC_DECR(int *ptrval);
#endif


/*
 * Interface for small allocations. If you need a little scratch space for
 *  a throwaway buffer or string, use this. It will make small allocations
 *  on the stack if possible, and use allocator.Malloc() if they are too
 *  large. This helps reduce malloc pressure.
 * There are some rules, though:
 * NEVER return a pointer from this, as stack-allocated buffers go away
 *  when your function returns.
 * NEVER allocate in a loop, as stack-allocated pointers will pile up. Call
 *  a function that uses smallAlloc from your loop, so the allocation can
 *  free each time.
 * NEVER call smallAlloc with any complex expression (it's a macro that WILL
 *  have side effects...it references the argument multiple times). Use a
 *  variable or a literal.
 * NEVER free a pointer from this with anything but smallFree. It will not
 *  be a valid pointer to the allocator, regardless of where the memory came
 *  from.
 * NEVER realloc a pointer from this.
 * NEVER forget to use smallFree: it may not be a pointer from the stack.
 * NEVER forget to check for NULL...allocation can fail here, of course!
 */
#define __PHYSFS_SMALLALLOCTHRESHOLD 256
void *__PHYSFS_initSmallAlloc(void *ptr, const size_t len);

#define __PHYSFS_smallAlloc(bytes) ( \
    __PHYSFS_initSmallAlloc( \
        (((bytes) < __PHYSFS_SMALLALLOCTHRESHOLD) ? \
            alloca((size_t)((bytes)+sizeof(void*))) : NULL), (bytes)) \
)

void __PHYSFS_smallFree(void *ptr);


/* Use the allocation hooks. */
#define malloc(x) Do not use malloc() directly.
#define realloc(x, y) Do not use realloc() directly.
#define free(x) Do not use free() directly.
/* !!! FIXME: add alloca check here. */


/* by default, enable things, so builds can opt out of a few things they
   want to avoid. But you can build with this #defined to 0 if you would
   like to turn off everything except a handful of things you opt into. */
#ifndef PHYSFS_SUPPORTS_DEFAULT
#define PHYSFS_SUPPORTS_DEFAULT 1
#endif


#ifndef PHYSFS_SUPPORTS_ZIP
#define PHYSFS_SUPPORTS_ZIP PHYSFS_SUPPORTS_DEFAULT
#endif
#ifndef PHYSFS_SUPPORTS_7Z
#define PHYSFS_SUPPORTS_7Z PHYSFS_SUPPORTS_DEFAULT
#endif
#ifndef PHYSFS_SUPPORTS_GRP
#define PHYSFS_SUPPORTS_GRP PHYSFS_SUPPORTS_DEFAULT
#endif
#ifndef PHYSFS_SUPPORTS_HOG
#define PHYSFS_SUPPORTS_HOG PHYSFS_SUPPORTS_DEFAULT
#endif
#ifndef PHYSFS_SUPPORTS_MVL
#define PHYSFS_SUPPORTS_MVL PHYSFS_SUPPORTS_DEFAULT
#endif
#ifndef PHYSFS_SUPPORTS_WAD
#define PHYSFS_SUPPORTS_WAD PHYSFS_SUPPORTS_DEFAULT
#endif
#ifndef PHYSFS_SUPPORTS_QPAK
#define PHYSFS_SUPPORTS_QPAK PHYSFS_SUPPORTS_DEFAULT
#endif
#ifndef PHYSFS_SUPPORTS_SLB
#define PHYSFS_SUPPORTS_SLB PHYSFS_SUPPORTS_DEFAULT
#endif
#ifndef PHYSFS_SUPPORTS_ISO9660
#define PHYSFS_SUPPORTS_ISO9660 PHYSFS_SUPPORTS_DEFAULT
#endif
#ifndef PHYSFS_SUPPORTS_VDF
#define PHYSFS_SUPPORTS_VDF PHYSFS_SUPPORTS_DEFAULT
#endif

#if PHYSFS_SUPPORTS_7Z
/* 7zip support needs a global init function called at startup (no deinit). */
extern void SZIP_global_init(void);
#endif

/* The latest supported PHYSFS_Io::version value. */
#define CURRENT_PHYSFS_IO_API_VERSION 0

/* The latest supported PHYSFS_Archiver::version value. */
#define CURRENT_PHYSFS_ARCHIVER_API_VERSION 0

/* This byteorder stuff was lifted from SDL. https://www.libsdl.org/ */
#define PHYSFS_LIL_ENDIAN  1234
#define PHYSFS_BIG_ENDIAN  4321

#ifdef __linux__
#include <endian.h>
#define PHYSFS_BYTEORDER  __BYTE_ORDER
#else /* __linux__ */
#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define PHYSFS_BYTEORDER   PHYSFS_BIG_ENDIAN
#else
#define PHYSFS_BYTEORDER   PHYSFS_LIL_ENDIAN
#endif
#endif /* __linux__ */


/*
 * When sorting the entries in an archive, we use a modified QuickSort.
 *  When there are less then PHYSFS_QUICKSORT_THRESHOLD entries left to sort,
 *  we switch over to a BubbleSort for the remainder. Tweak to taste.
 *
 * You can override this setting by defining PHYSFS_QUICKSORT_THRESHOLD
 *  before #including "physfs_internal.h".
 */
#ifndef PHYSFS_QUICKSORT_THRESHOLD
#define PHYSFS_QUICKSORT_THRESHOLD 4
#endif

/*
 * Sort an array (or whatever) of (max) elements. This uses a mixture of
 *  a QuickSort and BubbleSort internally.
 * (cmpfn) is used to determine ordering, and (swapfn) does the actual
 *  swapping of elements in the list.
 */
void __PHYSFS_sort(void *entries, size_t max,
                   int (*cmpfn)(void *, size_t, size_t),
                   void (*swapfn)(void *, size_t, size_t));

/* These get used all over for lessening code clutter. */
/* "ERRPASS" means "something else just set the error state for us" and is
    just to make it clear where the responsibility for the error state lays. */
#define BAIL(e, r) do { if (e) PHYSFS_setErrorCode(e); return r; } while (0)
#define BAIL_ERRPASS(r) do { return r; } while (0)
#define BAIL_IF(c, e, r) do { if (c) { if (e) PHYSFS_setErrorCode(e); return r; } } while (0)
#define BAIL_IF_ERRPASS(c, r) do { if (c) { return r; } } while (0)
#define BAIL_MUTEX(e, m, r) do { if (e) PHYSFS_setErrorCode(e); __PHYSFS_platformReleaseMutex(m); return r; } while (0)
#define BAIL_MUTEX_ERRPASS(m, r) do { __PHYSFS_platformReleaseMutex(m); return r; } while (0)
#define BAIL_IF_MUTEX(c, e, m, r) do { if (c) { if (e) PHYSFS_setErrorCode(e); __PHYSFS_platformReleaseMutex(m); return r; } } while (0)
#define BAIL_IF_MUTEX_ERRPASS(c, m, r) do { if (c) { __PHYSFS_platformReleaseMutex(m); return r; } } while (0)
#define GOTO(e, g) do { if (e) PHYSFS_setErrorCode(e); goto g; } while (0)
#define GOTO_ERRPASS(g) do { goto g; } while (0)
#define GOTO_IF(c, e, g) do { if (c) { if (e) PHYSFS_setErrorCode(e); goto g; } } while (0)
#define GOTO_IF_ERRPASS(c, g) do { if (c) { goto g; } } while (0)
#define GOTO_MUTEX(e, m, g) do { if (e) PHYSFS_setErrorCode(e); __PHYSFS_platformReleaseMutex(m); goto g; } while (0)
#define GOTO_MUTEX_ERRPASS(m, g) do { __PHYSFS_platformReleaseMutex(m); goto g; } while (0)
#define GOTO_IF_MUTEX(c, e, m, g) do { if (c) { if (e) PHYSFS_setErrorCode(e); __PHYSFS_platformReleaseMutex(m); goto g; } } while (0)
#define GOTO_IF_MUTEX_ERRPASS(c, m, g) do { if (c) { __PHYSFS_platformReleaseMutex(m); goto g; } } while (0)

#define __PHYSFS_ARRAYLEN(x) ( (sizeof (x)) / (sizeof (x[0])) )

#ifdef PHYSFS_NO_64BIT_SUPPORT
#define __PHYSFS_SI64(x) ((PHYSFS_sint64) (x))
#define __PHYSFS_UI64(x) ((PHYSFS_uint64) (x))
#elif (defined __GNUC__)
#define __PHYSFS_SI64(x) x##LL
#define __PHYSFS_UI64(x) x##ULL
#elif (defined _MSC_VER)
#define __PHYSFS_SI64(x) x##i64
#define __PHYSFS_UI64(x) x##ui64
#else
#define __PHYSFS_SI64(x) ((PHYSFS_sint64) (x))
#define __PHYSFS_UI64(x) ((PHYSFS_uint64) (x))
#endif


/*
 * Check if a ui64 will fit in the platform's address space.
 *  The initial sizeof check will optimize this macro out entirely on
 *  64-bit (and larger?!) platforms, and the other condition will
 *  return zero or non-zero if the variable will fit in the platform's
 *  size_t, suitable to pass to malloc. This is kinda messy, but effective.
 */
#define __PHYSFS_ui64FitsAddressSpace(s) ( \
    (sizeof (PHYSFS_uint64) <= sizeof (size_t)) || \
    ((s) < (__PHYSFS_UI64(0xFFFFFFFFFFFFFFFF) >> (64-(sizeof(size_t)*8)))) \
)

/*
 * Like strdup(), but uses the current PhysicsFS allocator.
 */
char *__PHYSFS_strdup(const char *str);

/*
 * Give a hash value for a C string (uses djb's xor hashing algorithm).
 */
PHYSFS_uint32 __PHYSFS_hashString(const char *str, size_t len);


/*
 * The current allocator. Not valid before PHYSFS_init is called!
 */
extern PHYSFS_Allocator __PHYSFS_AllocatorHooks;

/* convenience macro to make this less cumbersome internally... */
#define allocator __PHYSFS_AllocatorHooks
#define physfs_allocator __PHYSFS_AllocatorHooks
/*
 * Create a PHYSFS_Io for a file in the physical filesystem.
 *  This path is in platform-dependent notation. (mode) must be 'r', 'w', or
 *  'a' for Read, Write, or Append.
 */
PHYSFS_Io *__PHYSFS_createNativeIo(const char *path, const int mode);

/*
 * Create a PHYSFS_Io for a buffer of memory (READ-ONLY). If you already
 *  have one of these, just use its duplicate() method, and it'll increment
 *  its refcount without allocating a copy of the buffer.
 */
PHYSFS_Io *__PHYSFS_createMemoryIo(const void *buf, PHYSFS_uint64 len,
                                   void (*destruct)(void *));


/*
 * Read (len) bytes from (io) into (buf). Returns non-zero on success,
 *  zero on i/o error. Literally: "return (io->read(io, buf, len) == len);"
 */
int __PHYSFS_readAll(PHYSFS_Io *io, void *buf, const size_t len);


/* These are shared between some archivers. */

void UNPK_abandonArchive(void *opaque);
void UNPK_closeArchive(void *opaque);
void *UNPK_openArchive(PHYSFS_Io *io);
void *UNPK_addEntry(void *opaque, char *name, const int isdir,
                    const PHYSFS_sint64 ctime, const PHYSFS_sint64 mtime,
                    const PHYSFS_uint64 pos, const PHYSFS_uint64 len);
PHYSFS_Io *UNPK_openRead(void *opaque, const char *name);
PHYSFS_Io *UNPK_openWrite(void *opaque, const char *name);
PHYSFS_Io *UNPK_openAppend(void *opaque, const char *name);
int UNPK_remove(void *opaque, const char *name);
int UNPK_mkdir(void *opaque, const char *name);
int UNPK_stat(void *opaque, const char *fn, PHYSFS_Stat *st);
#define UNPK_enumerate __PHYSFS_DirTreeEnumerate



/* Optional API many archivers use this to manage their directory tree. */
/* !!! FIXME: document this better. */

typedef struct __PHYSFS_DirTreeEntry
{
    char *name;                              /* Full path in archive.        */
    struct __PHYSFS_DirTreeEntry *hashnext;  /* next item in hash bucket.    */
    struct __PHYSFS_DirTreeEntry *children;  /* linked list of kids, if dir. */
    struct __PHYSFS_DirTreeEntry *sibling;   /* next item in same dir.       */
    int isdir;
} __PHYSFS_DirTreeEntry;

typedef struct __PHYSFS_DirTree
{
    __PHYSFS_DirTreeEntry *root;    /* root of directory tree.             */
    __PHYSFS_DirTreeEntry **hash;  /* all entries hashed for fast lookup. */
    size_t hashBuckets;            /* number of buckets in hash.          */
    size_t entrylen;    /* size in bytes of entries (including subclass). */
} __PHYSFS_DirTree;


int __PHYSFS_DirTreeInit(__PHYSFS_DirTree *dt, const size_t entrylen);
void *__PHYSFS_DirTreeAdd(__PHYSFS_DirTree *dt, char *name, const int isdir);
void *__PHYSFS_DirTreeFind(__PHYSFS_DirTree *dt, const char *path);
PHYSFS_EnumerateCallbackResult __PHYSFS_DirTreeEnumerate(void *opaque,
                              const char *dname, PHYSFS_EnumerateCallback cb,
                              const char *origdir, void *callbackdata);
void __PHYSFS_DirTreeDeinit(__PHYSFS_DirTree *dt);



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*------------                                              ----------------*/
/*------------  You MUST implement the following functions  ----------------*/
/*------------        if porting to a new platform.         ----------------*/
/*------------     (see platform/unix.c for an example)     ----------------*/
/*------------                                              ----------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


/*
 * The dir separator; '/' on unix, '\\' on win32, ":" on MacOS, etc...
 *  Obviously, this isn't a function. If you need more than one char for this,
 *  you'll need to pull some old pieces of PhysicsFS out of revision control.
 */
#if defined(PHYSFS_PLATFORM_WINDOWS) || defined(PHYSFS_PLATFORM_OS2)
#define __PHYSFS_platformDirSeparator '\\'
#else
#define __PHYSFS_STANDARD_DIRSEP 1
#define __PHYSFS_platformDirSeparator '/'
#endif

/*
 * Initialize the platform. This is called when PHYSFS_init() is called from
 *  the application.
 *
 * Return zero if there was a catastrophic failure (which prevents you from
 *  functioning at all), and non-zero otherwise.
 */
int __PHYSFS_platformInit(void);


/*
 * Deinitialize the platform. This is called when PHYSFS_deinit() is called
 *  from the application. You can use this to clean up anything you've
 *  allocated in your platform driver.
 */
void __PHYSFS_platformDeinit(void);


/*
 * Open a file for reading. (filename) is in platform-dependent notation. The
 *  file pointer should be positioned on the first byte of the file.
 *
 * The return value will be some platform-specific datatype that is opaque to
 *  the caller; it could be a (FILE *) under Unix, or a (HANDLE *) under win32.
 *
 * The same file can be opened for read multiple times, and each should have
 *  a unique file handle; this is frequently employed to prevent race
 *  conditions in the archivers.
 *
 * Call PHYSFS_setErrorCode() and return (NULL) if the file can't be opened.
 */
void *__PHYSFS_platformOpenRead(const char *filename);


/*
 * Open a file for writing. (filename) is in platform-dependent notation. If
 *  the file exists, it should be truncated to zero bytes, and if it doesn't
 *  exist, it should be created as a zero-byte file. The file pointer should
 *  be positioned on the first byte of the file.
 *
 * The return value will be some platform-specific datatype that is opaque to
 *  the caller; it could be a (FILE *) under Unix, or a (HANDLE *) under win32,
 *  etc.
 *
 * Opening a file for write multiple times has undefined results.
 *
 * Call PHYSFS_setErrorCode() and return (NULL) if the file can't be opened.
 */
void *__PHYSFS_platformOpenWrite(const char *filename);


/*
 * Open a file for appending. (filename) is in platform-dependent notation. If
 *  the file exists, the file pointer should be place just past the end of the
 *  file, so that the first write will be one byte after the current end of
 *  the file. If the file doesn't exist, it should be created as a zero-byte
 *  file. The file pointer should be positioned on the first byte of the file.
 *
 * The return value will be some platform-specific datatype that is opaque to
 *  the caller; it could be a (FILE *) under Unix, or a (HANDLE *) under win32,
 *  etc.
 *
 * Opening a file for append multiple times has undefined results.
 *
 * Call PHYSFS_setErrorCode() and return (NULL) if the file can't be opened.
 */
void *__PHYSFS_platformOpenAppend(const char *filename);

/*
 * Read more data from a platform-specific file handle. (opaque) should be
 *  cast to whatever data type your platform uses. Read a maximum of (len)
 *  8-bit bytes to the area pointed to by (buf). If there isn't enough data
 *  available, return the number of bytes read, and position the file pointer
 *  immediately after those bytes.
 *  On success, return (len) and position the file pointer immediately past
 *  the end of the last read byte. Return (-1) if there is a catastrophic
 *  error, and call PHYSFS_setErrorCode() to describe the problem; the file
 *  pointer should not move in such a case. A partial read is success; only
 *  return (-1) on total failure; presumably, the next read call after a
 *  partial read will fail as such.
 */
PHYSFS_sint64 __PHYSFS_platformRead(void *opaque, void *buf, PHYSFS_uint64 len);

/*
 * Write more data to a platform-specific file handle. (opaque) should be
 *  cast to whatever data type your platform uses. Write a maximum of (len)
 *  8-bit bytes from the area pointed to by (buffer). If there is a problem,
 *  return the number of bytes written, and position the file pointer
 *  immediately after those bytes. Return (-1) if there is a catastrophic
 *  error, and call PHYSFS_setErrorCode() to describe the problem; the file
 *  pointer should not move in such a case. A partial write is success; only
 *  return (-1) on total failure; presumably, the next write call after a
 *  partial write will fail as such.
 */
PHYSFS_sint64 __PHYSFS_platformWrite(void *opaque, const void *buffer,
                                     PHYSFS_uint64 len);

/*
 * Set the file pointer to a new position. (opaque) should be cast to
 *  whatever data type your platform uses. (pos) specifies the number
 *  of 8-bit bytes to seek to from the start of the file. Seeking past the
 *  end of the file is an error condition, and you should check for it.
 *
 * Not all file types can seek; this is to be expected by the caller.
 *
 * On error, call PHYSFS_setErrorCode() and return zero. On success, return
 *  a non-zero value.
 */
int __PHYSFS_platformSeek(void *opaque, PHYSFS_uint64 pos);


/*
 * Get the file pointer's position, in an 8-bit byte offset from the start of
 *  the file. (opaque) should be cast to whatever data type your platform
 *  uses.
 *
 * Not all file types can "tell"; this is to be expected by the caller.
 *
 * On error, call PHYSFS_setErrorCode() and return -1. On success, return >= 0.
 */
PHYSFS_sint64 __PHYSFS_platformTell(void *opaque);


/*
 * Determine the current size of a file, in 8-bit bytes, from an open file.
 *
 * The caller expects that this information may not be available for all
 *  file types on all platforms.
 *
 * Return -1 if you can't do it, and call PHYSFS_setErrorCode(). Otherwise,
 *  return the file length in 8-bit bytes.
 */
PHYSFS_sint64 __PHYSFS_platformFileLength(void *handle);


/*
 * Read filesystem metadata for a specific path.
 *
 * This needs to fill in all the fields of (stat). For fields that might not
 *  mean anything on a platform (access time, perhaps), choose a reasonable
 *  default. if (follow), we want to follow symlinks and stat what they
 *  link to and not the link itself.
 *
 *  Return zero on failure, non-zero on success.
 */
int __PHYSFS_platformStat(const char *fn, PHYSFS_Stat *stat, const int follow);

/*
 * Flush any pending writes to disk. (opaque) should be cast to whatever data
 *  type your platform uses. Be sure to check for errors; the caller expects
 *  that this function can fail if there was a flushing error, etc.
 *
 *  Return zero on failure, non-zero on success.
 */
int __PHYSFS_platformFlush(void *opaque);

/*
 * Close file and deallocate resources. (opaque) should be cast to whatever
 *  data type your platform uses. This should close the file in any scenario:
 *  flushing is a separate function call, and this function should never fail.
 *
 * You should clean up all resources associated with (opaque); the pointer
 *  will be considered invalid after this call.
 */
void __PHYSFS_platformClose(void *opaque);

/*
 * Platform implementation of PHYSFS_getCdRomDirsCallback()...
 *  CD directories are discovered and reported to the callback one at a time.
 *  Pointers passed to the callback are assumed to be invalid to the
 *  application after the callback returns, so you can free them or whatever.
 *  Callback does not assume results will be sorted in any meaningful way.
 */
void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data);

/*
 * Calculate the base dir, if your platform needs special consideration.
 *  Just return NULL if the standard routines will suffice. (see
 *  calculateBaseDir() in physfs.c ...)
 * Your string must end with a dir separator if you don't return NULL.
 *  Caller will allocator.Free() the retval if it's not NULL.
 */
char *__PHYSFS_platformCalcBaseDir(const char *argv0);

/*
 * Get the platform-specific user dir.
 * As of PhysicsFS 2.1, returning NULL means fatal error.
 * Your string must end with a dir separator if you don't return NULL.
 *  Caller will allocator.Free() the retval if it's not NULL.
 */
char *__PHYSFS_platformCalcUserDir(void);


/* This is the cached version from PHYSFS_init(). This is a fast call. */
const char *__PHYSFS_getUserDir(void);  /* not deprecated internal version. */


/*
 * Get the platform-specific pref dir.
 * Returning NULL means fatal error.
 * Your string must end with a dir separator if you don't return NULL.
 *  Caller will allocator.Free() the retval if it's not NULL.
 *  Caller will make missing directories if necessary; this just reports
 *   the final path.
 */
char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app);


/*
 * Return a pointer that uniquely identifies the current thread.
 *  On a platform without threading, (0x1) will suffice. These numbers are
 *  arbitrary; the only requirement is that no two threads have the same
 *  pointer.
 */
void *__PHYSFS_platformGetThreadID(void);


/*
 * Enumerate a directory of files. This follows the rules for the
 *  PHYSFS_Archiver::enumerate() method, except that the (dirName) that is
 *  passed to this function is converted to platform-DEPENDENT notation by
 *  the caller. The PHYSFS_Archiver version uses platform-independent
 *  notation. Note that ".", "..", and other meta-entries should always
 *  be ignored.
 */
PHYSFS_EnumerateCallbackResult __PHYSFS_platformEnumerate(const char *dirname,
                               PHYSFS_EnumerateCallback callback,
                               const char *origdir, void *callbackdata);

/*
 * Make a directory in the actual filesystem. (path) is specified in
 *  platform-dependent notation. On error, return zero and set the error
 *  message. Return non-zero on success.
 */
int __PHYSFS_platformMkDir(const char *path);


/*
 * Remove a file or directory entry in the actual filesystem. (path) is
 *  specified in platform-dependent notation. Note that this deletes files
 *  _and_ directories, so you might need to do some determination.
 *  Non-empty directories should report an error and not delete themselves
 *  or their contents.
 *
 * Deleting a symlink should remove the link, not what it points to.
 *
 * On error, return zero and set the error message. Return non-zero on success.
 */
int __PHYSFS_platformDelete(const char *path);


/*
 * Create a platform-specific mutex. This can be whatever datatype your
 *  platform uses for mutexes, but it is cast to a (void *) for abstractness.
 *
 * Return (NULL) if you couldn't create one. Systems without threads can
 *  return any arbitrary non-NULL value.
 */
void *__PHYSFS_platformCreateMutex(void);

/*
 * Destroy a platform-specific mutex, and clean up any resources associated
 *  with it. (mutex) is a value previously returned by
 *  __PHYSFS_platformCreateMutex(). This can be a no-op on single-threaded
 *  platforms.
 */
void __PHYSFS_platformDestroyMutex(void *mutex);

/*
 * Grab possession of a platform-specific mutex. Mutexes should be recursive;
 *  that is, the same thread should be able to call this function multiple
 *  times in a row without causing a deadlock. This function should block
 *  until a thread can gain possession of the mutex.
 *
 * Return non-zero if the mutex was grabbed, zero if there was an
 *  unrecoverable problem grabbing it (this should not be a matter of
 *  timing out! We're talking major system errors; block until the mutex
 *  is available otherwise.)
 *
 * _DO NOT_ call PHYSFS_setErrorCode() in here! Since setErrorCode calls this
 *  function, you'll cause an infinite recursion. This means you can't
 *  use the BAIL_*MACRO* macros, either.
 */
int __PHYSFS_platformGrabMutex(void *mutex);

/*
 * Relinquish possession of the mutex when this method has been called
 *  once for each time that platformGrabMutex was called. Once possession has
 *  been released, the next thread in line to grab the mutex (if any) may
 *  proceed.
 *
 * _DO NOT_ call PHYSFS_setErrorCode() in here! Since setErrorCode calls this
 *  function, you'll cause an infinite recursion. This means you can't
 *  use the BAIL_*MACRO* macros, either.
 */
void __PHYSFS_platformReleaseMutex(void *mutex);

#if PHYSFS_HAVE_PRAGMA_VISIBILITY
#pragma GCC visibility pop
#endif

#ifdef __cplusplus
}
#endif

#endif

/* end of physfs_internal.h ... */

#endif /* defined(PHYSFS_IMPL) || defined(PHYSFS_PLATFORM_IMPL) */


#ifdef PHYSFS_IMPL

#ifdef __cplusplus
extern "C" {
#endif

/**
 * PhysicsFS; a portable, flexible file i/o abstraction.
 *
 * Documentation is in physfs.h. It's verbose, honest.  :)
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if defined(_MSC_VER)

/* this code came from https://stackoverflow.com/a/8712996 */
int __PHYSFS_msvc_vsnprintf(char *outBuf, size_t size, const char *format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

int __PHYSFS_msvc_snprintf(char *outBuf, size_t size, const char *format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = __PHYSFS_msvc_vsnprintf(outBuf, size, format, ap);
    va_end(ap);

    return count;
}
#endif


typedef struct __PHYSFS_DIRHANDLE__
{
    void *opaque;  /* Instance data unique to the archiver. */
    char *dirName;  /* Path to archive in platform-dependent notation. */
    char *mountPoint; /* Mountpoint in virtual file tree. */
    char *root;  /* subdirectory of archiver to use as root of archive (NULL for actual root) */
    size_t rootlen;  /* subdirectory of archiver to use as root of archive (NULL for actual root) */
    const PHYSFS_Archiver *funcs;  /* Ptr to archiver info for this handle. */
    struct __PHYSFS_DIRHANDLE__ *next;  /* linked list stuff. */
} DirHandle;


typedef struct __PHYSFS_FILEHANDLE__
{
    PHYSFS_Io *io;  /* Instance data unique to the archiver for this file. */
    PHYSFS_uint8 forReading; /* Non-zero if reading, zero if write/append */
    const DirHandle *dirHandle;  /* Archiver instance that created this */
    PHYSFS_uint8 *buffer;  /* Buffer, if set (NULL otherwise). Don't touch! */
    size_t bufsize;  /* Bufsize, if set (0 otherwise). Don't touch! */
    size_t buffill;  /* Buffer fill size. Don't touch! */
    size_t bufpos;  /* Buffer position. Don't touch! */
    struct __PHYSFS_FILEHANDLE__ *next;  /* linked list stuff. */
} FileHandle;


typedef struct __PHYSFS_ERRSTATETYPE__
{
    void *tid;
    PHYSFS_ErrorCode code;
    struct __PHYSFS_ERRSTATETYPE__ *next;
} ErrState;


/* General PhysicsFS state ... */
static int initialized = 0;
static ErrState *errorStates = NULL;
static DirHandle *searchPath = NULL;
static DirHandle *writeDir = NULL;
static FileHandle *openWriteList = NULL;
static FileHandle *openReadList = NULL;
static char *baseDir = NULL;
static char *userDir = NULL;
static char *prefDir = NULL;
static int allowSymLinks = 0;
static PHYSFS_Archiver **archivers = NULL;
static PHYSFS_ArchiveInfo **archiveInfo = NULL;
static volatile size_t numArchivers = 0;
static size_t longest_root = 0;

/* mutexes ... */
static void *errorLock = NULL;     /* protects error message list.        */
static void *stateLock = NULL;     /* protects other PhysFS static state. */

/* allocator ... */
static int externalAllocator = 0;
PHYSFS_Allocator allocator;


#ifdef PHYSFS_NEED_ATOMIC_OP_FALLBACK
static inline int __PHYSFS_atomicAdd(int *ptrval, const int val)
{
    int retval;
    __PHYSFS_platformGrabMutex(stateLock);
    retval = *ptrval;
    *ptrval = retval + val;
    __PHYSFS_platformReleaseMutex(stateLock);
    return retval;
} /* __PHYSFS_atomicAdd */

int __PHYSFS_ATOMIC_INCR(int *ptrval)
{
    return __PHYSFS_atomicAdd(ptrval, 1);
} /* __PHYSFS_ATOMIC_INCR */

int __PHYSFS_ATOMIC_DECR(int *ptrval)
{
    return __PHYSFS_atomicAdd(ptrval, -1);
} /* __PHYSFS_ATOMIC_DECR */
#endif



/* PHYSFS_Io implementation for i/o to physical filesystem... */

/* !!! FIXME: maybe refcount the paths in a string pool? */
typedef struct __PHYSFS_NativeIoInfo
{
    void *handle;
    const char *path;
    int mode;   /* 'r', 'w', or 'a' */
} NativeIoInfo;

static PHYSFS_sint64 nativeIo_read(PHYSFS_Io *io, void *buf, PHYSFS_uint64 len)
{
    NativeIoInfo *info = (NativeIoInfo *) io->opaque;
    return __PHYSFS_platformRead(info->handle, buf, len);
} /* nativeIo_read */

static PHYSFS_sint64 nativeIo_write(PHYSFS_Io *io, const void *buffer,
                                    PHYSFS_uint64 len)
{
    NativeIoInfo *info = (NativeIoInfo *) io->opaque;
    return __PHYSFS_platformWrite(info->handle, buffer, len);
} /* nativeIo_write */

static int nativeIo_seek(PHYSFS_Io *io, PHYSFS_uint64 offset)
{
    NativeIoInfo *info = (NativeIoInfo *) io->opaque;
    return __PHYSFS_platformSeek(info->handle, offset);
} /* nativeIo_seek */

static PHYSFS_sint64 nativeIo_tell(PHYSFS_Io *io)
{
    NativeIoInfo *info = (NativeIoInfo *) io->opaque;
    return __PHYSFS_platformTell(info->handle);
} /* nativeIo_tell */

static PHYSFS_sint64 nativeIo_length(PHYSFS_Io *io)
{
    NativeIoInfo *info = (NativeIoInfo *) io->opaque;
    return __PHYSFS_platformFileLength(info->handle);
} /* nativeIo_length */

static PHYSFS_Io *nativeIo_duplicate(PHYSFS_Io *io)
{
    NativeIoInfo *info = (NativeIoInfo *) io->opaque;
    return __PHYSFS_createNativeIo(info->path, info->mode);
} /* nativeIo_duplicate */

static int nativeIo_flush(PHYSFS_Io *io)
{
    NativeIoInfo *info = (NativeIoInfo *) io->opaque;
    return __PHYSFS_platformFlush(info->handle);
} /* nativeIo_flush */

static void nativeIo_destroy(PHYSFS_Io *io)
{
    NativeIoInfo *info = (NativeIoInfo *) io->opaque;
    __PHYSFS_platformClose(info->handle);
    allocator.Free((void *) info->path);
    allocator.Free(info);
    allocator.Free(io);
} /* nativeIo_destroy */

static const PHYSFS_Io __PHYSFS_nativeIoInterface =
{
    CURRENT_PHYSFS_IO_API_VERSION, NULL,
    nativeIo_read,
    nativeIo_write,
    nativeIo_seek,
    nativeIo_tell,
    nativeIo_length,
    nativeIo_duplicate,
    nativeIo_flush,
    nativeIo_destroy
};

PHYSFS_Io *__PHYSFS_createNativeIo(const char *path, const int mode)
{
    PHYSFS_Io *io = NULL;
    NativeIoInfo *info = NULL;
    void *handle = NULL;
    char *pathdup = NULL;

    assert((mode == 'r') || (mode == 'w') || (mode == 'a'));

    io = (PHYSFS_Io *) allocator.Malloc(sizeof (PHYSFS_Io));
    GOTO_IF(!io, PHYSFS_ERR_OUT_OF_MEMORY, createNativeIo_failed);
    info = (NativeIoInfo *) allocator.Malloc(sizeof (NativeIoInfo));
    GOTO_IF(!info, PHYSFS_ERR_OUT_OF_MEMORY, createNativeIo_failed);
    pathdup = (char *) allocator.Malloc(strlen(path) + 1);
    GOTO_IF(!pathdup, PHYSFS_ERR_OUT_OF_MEMORY, createNativeIo_failed);

    if (mode == 'r')
        handle = __PHYSFS_platformOpenRead(path);
    else if (mode == 'w')
        handle = __PHYSFS_platformOpenWrite(path);
    else if (mode == 'a')
        handle = __PHYSFS_platformOpenAppend(path);

    GOTO_IF_ERRPASS(!handle, createNativeIo_failed);

    strcpy(pathdup, path);
    info->handle = handle;
    info->path = pathdup;
    info->mode = mode;
    memcpy(io, &__PHYSFS_nativeIoInterface, sizeof (*io));
    io->opaque = info;
    return io;

createNativeIo_failed:
    if (handle != NULL) __PHYSFS_platformClose(handle);
    if (pathdup != NULL) allocator.Free(pathdup);
    if (info != NULL) allocator.Free(info);
    if (io != NULL) allocator.Free(io);
    return NULL;
} /* __PHYSFS_createNativeIo */


/* PHYSFS_Io implementation for i/o to a memory buffer... */

typedef struct __PHYSFS_MemoryIoInfo
{
    const PHYSFS_uint8 *buf;
    PHYSFS_uint64 len;
    PHYSFS_uint64 pos;
    PHYSFS_Io *parent;
    int refcount;
    void (*destruct)(void *);
} MemoryIoInfo;

static PHYSFS_sint64 memoryIo_read(PHYSFS_Io *io, void *buf, PHYSFS_uint64 len)
{
    MemoryIoInfo *info = (MemoryIoInfo *) io->opaque;
    const PHYSFS_uint64 avail = info->len - info->pos;
    assert(avail <= info->len);

    if (avail == 0)
        return 0;  /* we're at EOF; nothing to do. */

    if (len > avail)
        len = avail;

    memcpy(buf, info->buf + info->pos, (size_t) len);
    info->pos += len;
    return len;
} /* memoryIo_read */

static PHYSFS_sint64 memoryIo_write(PHYSFS_Io *io, const void *buffer,
                                    PHYSFS_uint64 len)
{
    BAIL(PHYSFS_ERR_OPEN_FOR_READING, -1);
} /* memoryIo_write */

static int memoryIo_seek(PHYSFS_Io *io, PHYSFS_uint64 offset)
{
    MemoryIoInfo *info = (MemoryIoInfo *) io->opaque;
    BAIL_IF(offset > info->len, PHYSFS_ERR_PAST_EOF, 0);
    info->pos = offset;
    return 1;
} /* memoryIo_seek */

static PHYSFS_sint64 memoryIo_tell(PHYSFS_Io *io)
{
    const MemoryIoInfo *info = (MemoryIoInfo *) io->opaque;
    return (PHYSFS_sint64) info->pos;
} /* memoryIo_tell */

static PHYSFS_sint64 memoryIo_length(PHYSFS_Io *io)
{
    const MemoryIoInfo *info = (MemoryIoInfo *) io->opaque;
    return (PHYSFS_sint64) info->len;
} /* memoryIo_length */

static PHYSFS_Io *memoryIo_duplicate(PHYSFS_Io *io)
{
    MemoryIoInfo *info = (MemoryIoInfo *) io->opaque;
    MemoryIoInfo *newinfo = NULL;
    PHYSFS_Io *parent = info->parent;
    PHYSFS_Io *retval = NULL;

    /* avoid deep copies. */
    assert((!parent) || (!((MemoryIoInfo *) parent->opaque)->parent) );

    /* share the buffer between duplicates. */
    if (parent != NULL)  /* dup the parent, increment its refcount. */
        return parent->duplicate(parent);

    /* we're the parent. */

    retval = (PHYSFS_Io *) allocator.Malloc(sizeof (PHYSFS_Io));
    BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    newinfo = (MemoryIoInfo *) allocator.Malloc(sizeof (MemoryIoInfo));
    if (!newinfo)
    {
        allocator.Free(retval);
        BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    } /* if */

    __PHYSFS_ATOMIC_INCR(&info->refcount);

    memset(newinfo, '\0', sizeof (*info));
    newinfo->buf = info->buf;
    newinfo->len = info->len;
    newinfo->pos = 0;
    newinfo->parent = io;
    newinfo->refcount = 0;
    newinfo->destruct = NULL;

    memcpy(retval, io, sizeof (*retval));
    retval->opaque = newinfo;
    return retval;
} /* memoryIo_duplicate */

static int memoryIo_flush(PHYSFS_Io *io) { return 1;  /* it's read-only. */ }

static void memoryIo_destroy(PHYSFS_Io *io)
{
    MemoryIoInfo *info = (MemoryIoInfo *) io->opaque;
    PHYSFS_Io *parent = info->parent;

    if (parent != NULL)
    {
        assert(info->buf == ((MemoryIoInfo *) info->parent->opaque)->buf);
        assert(info->len == ((MemoryIoInfo *) info->parent->opaque)->len);
        assert(info->refcount == 0);
        assert(info->destruct == NULL);
        allocator.Free(info);
        allocator.Free(io);
        parent->destroy(parent);  /* decrements refcount. */
        return;
    } /* if */

    /* we _are_ the parent. */
    assert(info->refcount > 0);  /* even in a race, we hold a reference. */

    if (__PHYSFS_ATOMIC_DECR(&info->refcount) == 0)
    {
        void (*destruct)(void *) = info->destruct;
        void *buf = (void *) info->buf;
        io->opaque = NULL;  /* kill this here in case of race. */
        allocator.Free(info);
        allocator.Free(io);
        if (destruct != NULL)
            destruct(buf);
    } /* if */
} /* memoryIo_destroy */


static const PHYSFS_Io __PHYSFS_memoryIoInterface =
{
    CURRENT_PHYSFS_IO_API_VERSION, NULL,
    memoryIo_read,
    memoryIo_write,
    memoryIo_seek,
    memoryIo_tell,
    memoryIo_length,
    memoryIo_duplicate,
    memoryIo_flush,
    memoryIo_destroy
};

PHYSFS_Io *__PHYSFS_createMemoryIo(const void *buf, PHYSFS_uint64 len,
                                   void (*destruct)(void *))
{
    PHYSFS_Io *io = NULL;
    MemoryIoInfo *info = NULL;

    io = (PHYSFS_Io *) allocator.Malloc(sizeof (PHYSFS_Io));
    GOTO_IF(!io, PHYSFS_ERR_OUT_OF_MEMORY, createMemoryIo_failed);
    info = (MemoryIoInfo *) allocator.Malloc(sizeof (MemoryIoInfo));
    GOTO_IF(!info, PHYSFS_ERR_OUT_OF_MEMORY, createMemoryIo_failed);

    memset(info, '\0', sizeof (*info));
    info->buf = (const PHYSFS_uint8 *) buf;
    info->len = len;
    info->pos = 0;
    info->parent = NULL;
    info->refcount = 1;
    info->destruct = destruct;

    memcpy(io, &__PHYSFS_memoryIoInterface, sizeof (*io));
    io->opaque = info;
    return io;

createMemoryIo_failed:
    if (info != NULL) allocator.Free(info);
    if (io != NULL) allocator.Free(io);
    return NULL;
} /* __PHYSFS_createMemoryIo */


/* PHYSFS_Io implementation for i/o to a PHYSFS_File... */

static PHYSFS_sint64 handleIo_read(PHYSFS_Io *io, void *buf, PHYSFS_uint64 len)
{
    return PHYSFS_readBytes((PHYSFS_File *) io->opaque, buf, len);
} /* handleIo_read */

static PHYSFS_sint64 handleIo_write(PHYSFS_Io *io, const void *buffer,
                                    PHYSFS_uint64 len)
{
    return PHYSFS_writeBytes((PHYSFS_File *) io->opaque, buffer, len);
} /* handleIo_write */

static int handleIo_seek(PHYSFS_Io *io, PHYSFS_uint64 offset)
{
    return PHYSFS_seek((PHYSFS_File *) io->opaque, offset);
} /* handleIo_seek */

static PHYSFS_sint64 handleIo_tell(PHYSFS_Io *io)
{
    return PHYSFS_tell((PHYSFS_File *) io->opaque);
} /* handleIo_tell */

static PHYSFS_sint64 handleIo_length(PHYSFS_Io *io)
{
    return PHYSFS_fileLength((PHYSFS_File *) io->opaque);
} /* handleIo_length */

static PHYSFS_Io *handleIo_duplicate(PHYSFS_Io *io)
{
    /*
     * There's no duplicate at the PHYSFS_File level, so we break the
     *  abstraction. We're allowed to: we're physfs.c!
     */
    FileHandle *origfh = (FileHandle *) io->opaque;
    FileHandle *newfh = (FileHandle *) allocator.Malloc(sizeof (FileHandle));
    PHYSFS_Io *retval = NULL;

    GOTO_IF(!newfh, PHYSFS_ERR_OUT_OF_MEMORY, handleIo_dupe_failed);
    memset(newfh, '\0', sizeof (*newfh));

    retval = (PHYSFS_Io *) allocator.Malloc(sizeof (PHYSFS_Io));
    GOTO_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, handleIo_dupe_failed);

#if 0  /* we don't buffer the duplicate, at least not at the moment. */
    if (origfh->buffer != NULL)
    {
        newfh->buffer = (PHYSFS_uint8 *) allocator.Malloc(origfh->bufsize);
        if (!newfh->buffer)
            GOTO(PHYSFS_ERR_OUT_OF_MEMORY, handleIo_dupe_failed);
        newfh->bufsize = origfh->bufsize;
    } /* if */
#endif

    newfh->io = origfh->io->duplicate(origfh->io);
    GOTO_IF_ERRPASS(!newfh->io, handleIo_dupe_failed);

    newfh->forReading = origfh->forReading;
    newfh->dirHandle = origfh->dirHandle;

    __PHYSFS_platformGrabMutex(stateLock);
    if (newfh->forReading)
    {
        newfh->next = openReadList;
        openReadList = newfh;
    } /* if */
    else
    {
        newfh->next = openWriteList;
        openWriteList = newfh;
    } /* else */
    __PHYSFS_platformReleaseMutex(stateLock);

    memcpy(retval, io, sizeof (PHYSFS_Io));
    retval->opaque = newfh;
    return retval;

handleIo_dupe_failed:
    if (newfh)
    {
        if (newfh->io != NULL) newfh->io->destroy(newfh->io);
        if (newfh->buffer != NULL) allocator.Free(newfh->buffer);
        allocator.Free(newfh);
    } /* if */

    return NULL;
} /* handleIo_duplicate */

static int handleIo_flush(PHYSFS_Io *io)
{
    return PHYSFS_flush((PHYSFS_File *) io->opaque);
} /* handleIo_flush */

static void handleIo_destroy(PHYSFS_Io *io)
{
    if (io->opaque != NULL)
        PHYSFS_close((PHYSFS_File *) io->opaque);
    allocator.Free(io);
} /* handleIo_destroy */

static const PHYSFS_Io __PHYSFS_handleIoInterface =
{
    CURRENT_PHYSFS_IO_API_VERSION, NULL,
    handleIo_read,
    handleIo_write,
    handleIo_seek,
    handleIo_tell,
    handleIo_length,
    handleIo_duplicate,
    handleIo_flush,
    handleIo_destroy
};

static PHYSFS_Io *__PHYSFS_createHandleIo(PHYSFS_File *f)
{
    PHYSFS_Io *io = (PHYSFS_Io *) allocator.Malloc(sizeof (PHYSFS_Io));
    BAIL_IF(!io, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    memcpy(io, &__PHYSFS_handleIoInterface, sizeof (*io));
    io->opaque = f;
    return io;
} /* __PHYSFS_createHandleIo */


/* functions ... */

typedef struct
{
    char **list;
    PHYSFS_uint32 size;
    PHYSFS_ErrorCode errcode;
} EnumStringListCallbackData;

static void enumStringListCallback(void *data, const char *str)
{
    void *ptr;
    char *newstr;
    EnumStringListCallbackData *pecd = (EnumStringListCallbackData *) data;

    if (pecd->errcode)
        return;

    ptr = allocator.Realloc(pecd->list, (pecd->size + 2) * sizeof (char *));
    newstr = (char *) allocator.Malloc(strlen(str) + 1);
    if (ptr != NULL)
        pecd->list = (char **) ptr;

    if ((ptr == NULL) || (newstr == NULL))
    {
        pecd->errcode = PHYSFS_ERR_OUT_OF_MEMORY;
        pecd->list[pecd->size] = NULL;
        PHYSFS_freeList(pecd->list);
        return;
    } /* if */

    strcpy(newstr, str);
    pecd->list[pecd->size] = newstr;
    pecd->size++;
} /* enumStringListCallback */


static char **doEnumStringList(void (*func)(PHYSFS_StringCallback, void *))
{
    EnumStringListCallbackData ecd;
    memset(&ecd, '\0', sizeof (ecd));
    ecd.list = (char **) allocator.Malloc(sizeof (char *));
    BAIL_IF(!ecd.list, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    func(enumStringListCallback, &ecd);

    if (ecd.errcode)
    {
        PHYSFS_setErrorCode(ecd.errcode);
        return NULL;
    } /* if */

    ecd.list[ecd.size] = NULL;
    return ecd.list;
} /* doEnumStringList */


static void __PHYSFS_bubble_sort(void *a, size_t lo, size_t hi,
                                 int (*cmpfn)(void *, size_t, size_t),
                                 void (*swapfn)(void *, size_t, size_t))
{
    size_t i;
    int sorted;

    do
    {
        sorted = 1;
        for (i = lo; i < hi; i++)
        {
            if (cmpfn(a, i, i + 1) > 0)
            {
                swapfn(a, i, i + 1);
                sorted = 0;
            } /* if */
        } /* for */
    } while (!sorted);
} /* __PHYSFS_bubble_sort */


static void __PHYSFS_quick_sort(void *a, size_t lo, size_t hi,
                         int (*cmpfn)(void *, size_t, size_t),
                         void (*swapfn)(void *, size_t, size_t))
{
    size_t i;
    size_t j;
    size_t v;

    if ((hi - lo) <= PHYSFS_QUICKSORT_THRESHOLD)
        __PHYSFS_bubble_sort(a, lo, hi, cmpfn, swapfn);
    else
    {
        i = (hi + lo) / 2;

        if (cmpfn(a, lo, i) > 0) swapfn(a, lo, i);
        if (cmpfn(a, lo, hi) > 0) swapfn(a, lo, hi);
        if (cmpfn(a, i, hi) > 0) swapfn(a, i, hi);

        j = hi - 1;
        swapfn(a, i, j);
        i = lo;
        v = j;
        while (1)
        {
            while(cmpfn(a, ++i, v) < 0) { /* do nothing */ }
            while(cmpfn(a, --j, v) > 0) { /* do nothing */ }
            if (j < i)
                break;
            swapfn(a, i, j);
        } /* while */
        if (i != (hi-1))
            swapfn(a, i, hi-1);
        __PHYSFS_quick_sort(a, lo, j, cmpfn, swapfn);
        __PHYSFS_quick_sort(a, i+1, hi, cmpfn, swapfn);
    } /* else */
} /* __PHYSFS_quick_sort */


void __PHYSFS_sort(void *entries, size_t max,
                   int (*cmpfn)(void *, size_t, size_t),
                   void (*swapfn)(void *, size_t, size_t))
{
    /*
     * Quicksort w/ Bubblesort fallback algorithm inspired by code from here:
     *   https://www.cs.ubc.ca/spider/harrison/Java/sorting-demo.html
     */
    if (max > 0)
        __PHYSFS_quick_sort(entries, 0, max - 1, cmpfn, swapfn);
} /* __PHYSFS_sort */


static ErrState *findErrorForCurrentThread(void)
{
    ErrState *i;
    void *tid;

    if (errorLock != NULL)
        __PHYSFS_platformGrabMutex(errorLock);

    if (errorStates != NULL)
    {
        tid = __PHYSFS_platformGetThreadID();

        for (i = errorStates; i != NULL; i = i->next)
        {
            if (i->tid == tid)
            {
                if (errorLock != NULL)
                    __PHYSFS_platformReleaseMutex(errorLock);
                return i;
            } /* if */
        } /* for */
    } /* if */

    if (errorLock != NULL)
        __PHYSFS_platformReleaseMutex(errorLock);

    return NULL;   /* no error available. */
} /* findErrorForCurrentThread */


/* this doesn't reset the error state. */
static inline PHYSFS_ErrorCode currentErrorCode(void)
{
    const ErrState *err = findErrorForCurrentThread();
    return err ? err->code : PHYSFS_ERR_OK;
} /* currentErrorCode */


PHYSFS_ErrorCode PHYSFS_getLastErrorCode(void)
{
    ErrState *err = findErrorForCurrentThread();
    const PHYSFS_ErrorCode retval = (err) ? err->code : PHYSFS_ERR_OK;
    if (err)
        err->code = PHYSFS_ERR_OK;
    return retval;
} /* PHYSFS_getLastErrorCode */


PHYSFS_DECL const char *PHYSFS_getErrorByCode(PHYSFS_ErrorCode code)
{
    switch (code)
    {
        case PHYSFS_ERR_OK: return "no error";
        case PHYSFS_ERR_OTHER_ERROR: return "unknown error";
        case PHYSFS_ERR_OUT_OF_MEMORY: return "out of memory";
        case PHYSFS_ERR_NOT_INITIALIZED: return "not initialized";
        case PHYSFS_ERR_IS_INITIALIZED: return "already initialized";
        case PHYSFS_ERR_ARGV0_IS_NULL: return "argv[0] is NULL";
        case PHYSFS_ERR_UNSUPPORTED: return "unsupported";
        case PHYSFS_ERR_PAST_EOF: return "past end of file";
        case PHYSFS_ERR_FILES_STILL_OPEN: return "files still open";
        case PHYSFS_ERR_INVALID_ARGUMENT: return "invalid argument";
        case PHYSFS_ERR_NOT_MOUNTED: return "not mounted";
        case PHYSFS_ERR_NOT_FOUND: return "not found";
        case PHYSFS_ERR_SYMLINK_FORBIDDEN: return "symlinks are forbidden";
        case PHYSFS_ERR_NO_WRITE_DIR: return "write directory is not set";
        case PHYSFS_ERR_OPEN_FOR_READING: return "file open for reading";
        case PHYSFS_ERR_OPEN_FOR_WRITING: return "file open for writing";
        case PHYSFS_ERR_NOT_A_FILE: return "not a file";
        case PHYSFS_ERR_READ_ONLY: return "read-only filesystem";
        case PHYSFS_ERR_CORRUPT: return "corrupted";
        case PHYSFS_ERR_SYMLINK_LOOP: return "infinite symbolic link loop";
        case PHYSFS_ERR_IO: return "i/o error";
        case PHYSFS_ERR_PERMISSION: return "permission denied";
        case PHYSFS_ERR_NO_SPACE: return "no space available for writing";
        case PHYSFS_ERR_BAD_FILENAME: return "filename is illegal or insecure";
        case PHYSFS_ERR_BUSY: return "tried to modify a file the OS needs";
        case PHYSFS_ERR_DIR_NOT_EMPTY: return "directory isn't empty";
        case PHYSFS_ERR_OS_ERROR: return "OS reported an error";
        case PHYSFS_ERR_DUPLICATE: return "duplicate resource";
        case PHYSFS_ERR_BAD_PASSWORD: return "bad password";
        case PHYSFS_ERR_APP_CALLBACK: return "app callback reported error";
    } /* switch */

    return NULL;  /* don't know this error code. */
} /* PHYSFS_getErrorByCode */


void PHYSFS_setErrorCode(PHYSFS_ErrorCode errcode)
{
    ErrState *err;

    if (!errcode)
        return;

    err = findErrorForCurrentThread();
    if (err == NULL)
    {
        err = (ErrState *) allocator.Malloc(sizeof (ErrState));
        if (err == NULL)
            return;   /* uhh...? */

        memset(err, '\0', sizeof (ErrState));
        err->tid = __PHYSFS_platformGetThreadID();

        if (errorLock != NULL)
            __PHYSFS_platformGrabMutex(errorLock);

        err->next = errorStates;
        errorStates = err;

        if (errorLock != NULL)
            __PHYSFS_platformReleaseMutex(errorLock);
    } /* if */

    err->code = errcode;
} /* PHYSFS_setErrorCode */


const char *PHYSFS_getLastError(void)
{
    const PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
    return (err) ? PHYSFS_getErrorByCode(err) : NULL;
} /* PHYSFS_getLastError */


/* MAKE SURE that errorLock is held before calling this! */
static void freeErrorStates(void)
{
    ErrState *i;
    ErrState *next;

    for (i = errorStates; i != NULL; i = next)
    {
        next = i->next;
        allocator.Free(i);
    } /* for */

    errorStates = NULL;
} /* freeErrorStates */


void PHYSFS_getLinkedVersion(PHYSFS_Version *ver)
{
    if (ver != NULL)
    {
        ver->major = PHYSFS_VER_MAJOR;
        ver->minor = PHYSFS_VER_MINOR;
        ver->patch = PHYSFS_VER_PATCH;
    } /* if */
} /* PHYSFS_getLinkedVersion */


static const char *find_filename_extension(const char *fname)
{
    const char *retval = NULL;
    if (fname != NULL)
    {
        const char *p = strchr(fname, '.');
        retval = p;

        while (p != NULL)
        {
            p = strchr(p + 1, '.');
            if (p != NULL)
                retval = p;
        } /* while */

        if (retval != NULL)
            retval++;  /* skip '.' */
    } /* if */

    return retval;
} /* find_filename_extension */


static DirHandle *tryOpenDir(PHYSFS_Io *io, const PHYSFS_Archiver *funcs,
                             const char *d, int forWriting, int *_claimed)
{
    DirHandle *retval = NULL;
    void *opaque = NULL;

    if (io != NULL)
        BAIL_IF_ERRPASS(!io->seek(io, 0), NULL);

    opaque = funcs->openArchive(io, d, forWriting, _claimed);
    if (opaque != NULL)
    {
        retval = (DirHandle *) allocator.Malloc(sizeof (DirHandle));
        if (retval == NULL)
            funcs->closeArchive(opaque);
        else
        {
            memset(retval, '\0', sizeof (DirHandle));
            retval->mountPoint = NULL;
            retval->funcs = funcs;
            retval->opaque = opaque;
        } /* else */
    } /* if */

    return retval;
} /* tryOpenDir */


static DirHandle *openDirectory(PHYSFS_Io *io, const char *d, int forWriting)
{
    DirHandle *retval = NULL;
    PHYSFS_Archiver **i;
    const char *ext;
    int created_io = 0;
    int claimed = 0;
    PHYSFS_ErrorCode errcode;

    assert((io != NULL) || (d != NULL));

    if (io == NULL)
    {
        /* file doesn't exist, etc? Just fail out. */
        PHYSFS_Stat statbuf;
        BAIL_IF_ERRPASS(!__PHYSFS_platformStat(d, &statbuf, 1), NULL);

        /* DIR gets first shot (unlike the rest, it doesn't deal with files). */
        if (statbuf.filetype == PHYSFS_FILETYPE_DIRECTORY)
        {
            retval = tryOpenDir(io, &__PHYSFS_Archiver_DIR, d, forWriting, &claimed);
            if (retval || claimed)
                return retval;
        } /* if */

        io = __PHYSFS_createNativeIo(d, forWriting ? 'w' : 'r');
        BAIL_IF_ERRPASS(!io, NULL);
        created_io = 1;
    } /* if */

    ext = find_filename_extension(d);
    if (ext != NULL)
    {
        /* Look for archivers with matching file extensions first... */
        for (i = archivers; (*i != NULL) && (retval == NULL) && !claimed; i++)
        {
            if (PHYSFS_utf8stricmp(ext, (*i)->info.extension) == 0)
                retval = tryOpenDir(io, *i, d, forWriting, &claimed);
        } /* for */

        /* failing an exact file extension match, try all the others... */
        for (i = archivers; (*i != NULL) && (retval == NULL) && !claimed; i++)
        {
            if (PHYSFS_utf8stricmp(ext, (*i)->info.extension) != 0)
                retval = tryOpenDir(io, *i, d, forWriting, &claimed);
        } /* for */
    } /* if */

    else  /* no extension? Try them all. */
    {
        for (i = archivers; (*i != NULL) && (retval == NULL) && !claimed; i++)
            retval = tryOpenDir(io, *i, d, forWriting, &claimed);
    } /* else */

    errcode = currentErrorCode();

    if ((!retval) && (created_io))
        io->destroy(io);

    errcode = claimed ? errcode : PHYSFS_ERR_UNSUPPORTED;
    BAIL_IF(!retval, errcode, NULL);
    return retval;
} /* openDirectory */


/*
 * Make a platform-independent path string sane. Doesn't actually check the
 *  file hierarchy, it just cleans up the string.
 *  (dst) must be a buffer at least as big as (src), as this is where the
 *  cleaned up string is deposited.
 * If there are illegal bits in the path (".." entries, etc) then we
 *  return zero and (dst) is undefined. Non-zero if the path was sanitized.
 */
static int sanitizePlatformIndependentPath(const char *src, char *dst)
{
    char *prev;
    char ch;

    while (*src == '/')  /* skip initial '/' chars... */
        src++;

    /* Make sure the entire string isn't "." or ".." */
    if ((strcmp(src, ".") == 0) || (strcmp(src, "..") == 0))
        BAIL(PHYSFS_ERR_BAD_FILENAME, 0);

    prev = dst;
    do
    {
        ch = *(src++);

        if ((ch == ':') || (ch == '\\'))  /* illegal chars in a physfs path. */
            BAIL(PHYSFS_ERR_BAD_FILENAME, 0);

        if (ch == '/')   /* path separator. */
        {
            *dst = '\0';  /* "." and ".." are illegal pathnames. */
            if ((strcmp(prev, ".") == 0) || (strcmp(prev, "..") == 0))
                BAIL(PHYSFS_ERR_BAD_FILENAME, 0);

            while (*src == '/')   /* chop out doubles... */
                src++;

            if (*src == '\0') /* ends with a pathsep? */
                break;  /* we're done, don't add final pathsep to dst. */

            prev = dst + 1;
        } /* if */

        *(dst++) = ch;
    } while (ch != '\0');

    return 1;
} /* sanitizePlatformIndependentPath */


static inline size_t dirHandleRootLen(const DirHandle *h)
{
    return h ? h->rootlen : 0;
} /* dirHandleRootLen */

static inline int sanitizePlatformIndependentPathWithRoot(const DirHandle *h, const char *src, char *dst)
{
    return sanitizePlatformIndependentPath(src, dst + dirHandleRootLen(h));
} /* sanitizePlatformIndependentPathWithRoot */



/*
 * Figure out if (fname) is part of (h)'s mountpoint. (fname) must be an
 *  output from sanitizePlatformIndependentPath(), so that it is in a known
 *  state.
 *
 * This only finds legitimate segments of a mountpoint. If the mountpoint is
 *  "/a/b/c" and (fname) is "/a/b/c", "/", or "/a/b/c/d", then the results are
 *  all zero. "/a/b" will succeed, though.
 */
static int partOfMountPoint(DirHandle *h, char *fname)
{
    int rc;
    size_t len, mntpntlen;

    if (h->mountPoint == NULL)
        return 0;
    else if (*fname == '\0')
        return 1;

    len = strlen(fname);
    mntpntlen = strlen(h->mountPoint);
    if (len > mntpntlen)  /* can't be a subset of mountpoint. */
        return 0;

    /* if true, must be not a match or a complete match, but not a subset. */
    if ((len + 1) == mntpntlen)
        return 0;

    rc = strncmp(fname, h->mountPoint, len); /* !!! FIXME: case insensitive? */
    if (rc != 0)
        return 0;  /* not a match. */

    /* make sure /a/b matches /a/b/ and not /a/bc ... */
    return h->mountPoint[len] == '/';
} /* partOfMountPoint */


static DirHandle *createDirHandle(PHYSFS_Io *io, const char *newDir,
                                  const char *mountPoint, int forWriting)
{
    DirHandle *dirHandle = NULL;
    char *tmpmntpnt = NULL;

    assert(newDir != NULL);  /* should have caught this higher up. */

    if (mountPoint != NULL)
    {
        const size_t len = strlen(mountPoint) + 1;
        tmpmntpnt = (char *) __PHYSFS_smallAlloc(len);
        GOTO_IF(!tmpmntpnt, PHYSFS_ERR_OUT_OF_MEMORY, badDirHandle);
        if (!sanitizePlatformIndependentPath(mountPoint, tmpmntpnt))
            goto badDirHandle;
        mountPoint = tmpmntpnt;  /* sanitized version. */
    } /* if */

    dirHandle = openDirectory(io, newDir, forWriting);
    GOTO_IF_ERRPASS(!dirHandle, badDirHandle);

    dirHandle->dirName = (char *) allocator.Malloc(strlen(newDir) + 1);
    GOTO_IF(!dirHandle->dirName, PHYSFS_ERR_OUT_OF_MEMORY, badDirHandle);
    strcpy(dirHandle->dirName, newDir);

    if ((mountPoint != NULL) && (*mountPoint != '\0'))
    {
        dirHandle->mountPoint = (char *)allocator.Malloc(strlen(mountPoint)+2);
        if (!dirHandle->mountPoint)
            GOTO(PHYSFS_ERR_OUT_OF_MEMORY, badDirHandle);
        strcpy(dirHandle->mountPoint, mountPoint);
        strcat(dirHandle->mountPoint, "/");
    } /* if */

    __PHYSFS_smallFree(tmpmntpnt);
    return dirHandle;

badDirHandle:
    if (dirHandle != NULL)
    {
        dirHandle->funcs->closeArchive(dirHandle->opaque);
        allocator.Free(dirHandle->dirName);
        allocator.Free(dirHandle->mountPoint);
        allocator.Free(dirHandle);
    } /* if */

    __PHYSFS_smallFree(tmpmntpnt);
    return NULL;
} /* createDirHandle */


/* MAKE SURE you've got the stateLock held before calling this! */
static int freeDirHandle(DirHandle *dh, FileHandle *openList)
{
    FileHandle *i;

    if (dh == NULL)
        return 1;

    for (i = openList; i != NULL; i = i->next)
        BAIL_IF(i->dirHandle == dh, PHYSFS_ERR_FILES_STILL_OPEN, 0);

    dh->funcs->closeArchive(dh->opaque);
    allocator.Free(dh->dirName);
    allocator.Free(dh->mountPoint);
    allocator.Free(dh);
    return 1;
} /* freeDirHandle */


static char *calculateBaseDir(const char *argv0)
{
    const char dirsep = __PHYSFS_platformDirSeparator;
    char *retval = NULL;
    char *ptr = NULL;

    /* Give the platform layer first shot at this. */
    retval = __PHYSFS_platformCalcBaseDir(argv0);
    if (retval != NULL)
        return retval;

    /* We need argv0 to go on. */
    BAIL_IF(argv0 == NULL, PHYSFS_ERR_ARGV0_IS_NULL, NULL);

    ptr = strrchr((char*)argv0, dirsep);
    if (ptr != NULL)
    {
        const size_t size = ((size_t) (ptr - argv0)) + 1;
        retval = (char *) allocator.Malloc(size + 1);
        BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        memcpy(retval, argv0, size);
        retval[size] = '\0';
        return retval;
    } /* if */

    /* argv0 wasn't helpful. */
    BAIL(PHYSFS_ERR_INVALID_ARGUMENT, NULL);
} /* calculateBaseDir */


static int initializeMutexes(void)
{
    errorLock = __PHYSFS_platformCreateMutex();
    if (errorLock == NULL)
        goto initializeMutexes_failed;

    stateLock = __PHYSFS_platformCreateMutex();
    if (stateLock == NULL)
        goto initializeMutexes_failed;

    return 1;  /* success. */

initializeMutexes_failed:
    if (errorLock != NULL)
        __PHYSFS_platformDestroyMutex(errorLock);

    if (stateLock != NULL)
        __PHYSFS_platformDestroyMutex(stateLock);

    errorLock = stateLock = NULL;
    return 0;  /* failed. */
} /* initializeMutexes */


static int doRegisterArchiver(const PHYSFS_Archiver *_archiver);

static int initStaticArchivers(void)
{
    #define REGISTER_STATIC_ARCHIVER(arc) { \
        if (!doRegisterArchiver(&__PHYSFS_Archiver_##arc)) { \
            return 0; \
        } \
    }

    #if PHYSFS_SUPPORTS_ZIP
        REGISTER_STATIC_ARCHIVER(ZIP);
    #endif
    #if PHYSFS_SUPPORTS_7Z
        SZIP_global_init();
        REGISTER_STATIC_ARCHIVER(7Z);
    #endif
    #if PHYSFS_SUPPORTS_GRP
        REGISTER_STATIC_ARCHIVER(GRP);
    #endif
    #if PHYSFS_SUPPORTS_QPAK
        REGISTER_STATIC_ARCHIVER(QPAK);
    #endif
    #if PHYSFS_SUPPORTS_HOG
        REGISTER_STATIC_ARCHIVER(HOG);
    #endif
    #if PHYSFS_SUPPORTS_MVL
        REGISTER_STATIC_ARCHIVER(MVL);
    #endif
    #if PHYSFS_SUPPORTS_WAD
        REGISTER_STATIC_ARCHIVER(WAD);
    #endif
    #if PHYSFS_SUPPORTS_SLB
        REGISTER_STATIC_ARCHIVER(SLB);
    #endif
    #if PHYSFS_SUPPORTS_ISO9660
        REGISTER_STATIC_ARCHIVER(ISO9660);
    #endif
    #if PHYSFS_SUPPORTS_VDF
        REGISTER_STATIC_ARCHIVER(VDF)
    #endif

    #undef REGISTER_STATIC_ARCHIVER

    return 1;
} /* initStaticArchivers */


static void setDefaultAllocator(void);
static int doDeinit(void);

int PHYSFS_init(const char *argv0)
{
    BAIL_IF(initialized, PHYSFS_ERR_IS_INITIALIZED, 0);

    if (!externalAllocator)
        setDefaultAllocator();

    if ((allocator.Init != NULL) && (!allocator.Init())) return 0;

    if (!__PHYSFS_platformInit())
    {
        if (allocator.Deinit != NULL) allocator.Deinit();
        return 0;
    } /* if */

    /* everything below here can be cleaned up safely by doDeinit(). */

    if (!initializeMutexes()) goto initFailed;

    baseDir = calculateBaseDir(argv0);
    if (!baseDir) goto initFailed;

    userDir = __PHYSFS_platformCalcUserDir();
    if (!userDir) goto initFailed;

    /* Platform layer is required to append a dirsep. */
    #ifndef __ANDROID__  /* it's an APK file, not a directory, on Android. */
    assert(baseDir[strlen(baseDir) - 1] == __PHYSFS_platformDirSeparator);
    #endif
    assert(userDir[strlen(userDir) - 1] == __PHYSFS_platformDirSeparator);

    if (!initStaticArchivers()) goto initFailed;

    initialized = 1;

    /* This makes sure that the error subsystem is initialized. */
    PHYSFS_setErrorCode(PHYSFS_getLastErrorCode());

    return 1;

initFailed:
    doDeinit();
    return 0;
} /* PHYSFS_init */


/* MAKE SURE you hold stateLock before calling this! */
static int closeFileHandleList(FileHandle **list)
{
    FileHandle *i;
    FileHandle *next = NULL;

    for (i = *list; i != NULL; i = next)
    {
        PHYSFS_Io *io = i->io;
        next = i->next;

        if (io->flush && !io->flush(io))
        {
            *list = i;
            return 0;
        } /* if */

        io->destroy(io);
        allocator.Free(i);
    } /* for */

    *list = NULL;
    return 1;
} /* closeFileHandleList */


/* MAKE SURE you hold the stateLock before calling this! */
static void freeSearchPath(void)
{
    DirHandle *i;
    DirHandle *next = NULL;

    closeFileHandleList(&openReadList);

    if (searchPath != NULL)
    {
        for (i = searchPath; i != NULL; i = next)
        {
            next = i->next;
            freeDirHandle(i, openReadList);
        } /* for */
        searchPath = NULL;
    } /* if */
} /* freeSearchPath */


/* MAKE SURE you hold stateLock before calling this! */
static int archiverInUse(const PHYSFS_Archiver *arc, const DirHandle *list)
{
    const DirHandle *i;
    for (i = list; i != NULL; i = i->next)
    {
        if (i->funcs == arc)
            return 1;
    } /* for */

    return 0;  /* not in use */
} /* archiverInUse */


/* MAKE SURE you hold stateLock before calling this! */
static int doDeregisterArchiver(const size_t idx)
{
    const size_t len = (numArchivers - idx) * sizeof (void *);
    PHYSFS_ArchiveInfo *info = archiveInfo[idx];
    PHYSFS_Archiver *arc = archivers[idx];

    /* make sure nothing is still using this archiver */
    if (archiverInUse(arc, searchPath) || archiverInUse(arc, writeDir))
        BAIL(PHYSFS_ERR_FILES_STILL_OPEN, 0);

    allocator.Free((void *) info->extension);
    allocator.Free((void *) info->description);
    allocator.Free((void *) info->author);
    allocator.Free((void *) info->url);
    allocator.Free((void *) arc);

    memmove(&archiveInfo[idx], &archiveInfo[idx+1], len);
    memmove(&archivers[idx], &archivers[idx+1], len);

    assert(numArchivers > 0);
    numArchivers--;

    return 1;
} /* doDeregisterArchiver */


/* Does NOT hold the state lock; we're shutting down. */
static void freeArchivers(void)
{
    while (numArchivers > 0)
    {
        if (!doDeregisterArchiver(numArchivers - 1))
            assert(!"nothing should be mounted during shutdown.");
    } /* while */

    allocator.Free(archivers);
    allocator.Free(archiveInfo);
    archivers = NULL;
    archiveInfo = NULL;
} /* freeArchivers */


static int doDeinit(void)
{
    closeFileHandleList(&openWriteList);
    BAIL_IF(!PHYSFS_setWriteDir(NULL), PHYSFS_ERR_FILES_STILL_OPEN, 0);

    freeSearchPath();
    freeArchivers();
    freeErrorStates();

    if (baseDir != NULL)
    {
        allocator.Free(baseDir);
        baseDir = NULL;
    } /* if */

    if (userDir != NULL)
    {
        allocator.Free(userDir);
        userDir = NULL;
    } /* if */

    if (prefDir != NULL)
    {
        allocator.Free(prefDir);
        prefDir = NULL;
    } /* if */

    if (archiveInfo != NULL)
    {
        allocator.Free(archiveInfo);
        archiveInfo = NULL;
    } /* if */

    if (archivers != NULL)
    {
        allocator.Free(archivers);
        archivers = NULL;
    } /* if */

    longest_root = 0;
    allowSymLinks = 0;
    initialized = 0;

    if (errorLock) __PHYSFS_platformDestroyMutex(errorLock);
    if (stateLock) __PHYSFS_platformDestroyMutex(stateLock);

    if (allocator.Deinit != NULL)
        allocator.Deinit();

    errorLock = stateLock = NULL;

    __PHYSFS_platformDeinit();

    return 1;
} /* doDeinit */


int PHYSFS_deinit(void)
{
    BAIL_IF(!initialized, PHYSFS_ERR_NOT_INITIALIZED, 0);
    return doDeinit();
} /* PHYSFS_deinit */


int PHYSFS_isInit(void)
{
    return initialized;
} /* PHYSFS_isInit */


char *__PHYSFS_strdup(const char *str)
{
    char *retval = (char *) allocator.Malloc(strlen(str) + 1);
    if (retval)
        strcpy(retval, str);
    return retval;
} /* __PHYSFS_strdup */


PHYSFS_uint32 __PHYSFS_hashString(const char *str, size_t len)
{
    PHYSFS_uint32 hash = 5381;
    while (len--)
        hash = ((hash << 5) + hash) ^ *(str++);
    return hash;
} /* __PHYSFS_hashString */


/* MAKE SURE you hold stateLock before calling this! */
static int doRegisterArchiver(const PHYSFS_Archiver *_archiver)
{
    const PHYSFS_uint32 maxver = CURRENT_PHYSFS_ARCHIVER_API_VERSION;
    const size_t len = (numArchivers + 2) * sizeof (void *);
    PHYSFS_Archiver *archiver = NULL;
    PHYSFS_ArchiveInfo *info = NULL;
    const char *ext = NULL;
    void *ptr = NULL;
    size_t i;

    BAIL_IF(!_archiver, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(_archiver->version > maxver, PHYSFS_ERR_UNSUPPORTED, 0);
    BAIL_IF(!_archiver->info.extension, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->info.description, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->info.author, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->info.url, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->openArchive, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->enumerate, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->openRead, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->openWrite, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->openAppend, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->remove, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->mkdir, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->closeArchive, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!_archiver->stat, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    ext = _archiver->info.extension;
    for (i = 0; i < numArchivers; i++)
    {
        if (PHYSFS_utf8stricmp(archiveInfo[i]->extension, ext) == 0)
            BAIL(PHYSFS_ERR_DUPLICATE, 0);
    } /* for */

    /* make a copy of the data. */
    archiver = (PHYSFS_Archiver *) allocator.Malloc(sizeof (*archiver));
    GOTO_IF(!archiver, PHYSFS_ERR_OUT_OF_MEMORY, regfailed);

    /* Must copy sizeof (OLD_VERSION_OF_STRUCT) when version changes! */
    memcpy(archiver, _archiver, sizeof (*archiver));

    info = (PHYSFS_ArchiveInfo *) &archiver->info;
    memset(info, '\0', sizeof (*info));  /* NULL in case an alloc fails. */
    #define CPYSTR(item) \
        info->item = __PHYSFS_strdup(_archiver->info.item); \
        GOTO_IF(!info->item, PHYSFS_ERR_OUT_OF_MEMORY, regfailed);
    CPYSTR(extension);
    CPYSTR(description);
    CPYSTR(author);
    CPYSTR(url);
    info->supportsSymlinks = _archiver->info.supportsSymlinks;
    #undef CPYSTR

    ptr = allocator.Realloc(archiveInfo, len);
    GOTO_IF(!ptr, PHYSFS_ERR_OUT_OF_MEMORY, regfailed);
    archiveInfo = (PHYSFS_ArchiveInfo **) ptr;

    ptr = allocator.Realloc(archivers, len);
    GOTO_IF(!ptr, PHYSFS_ERR_OUT_OF_MEMORY, regfailed);
    archivers = (PHYSFS_Archiver **) ptr;

    archiveInfo[numArchivers] = info;
    archiveInfo[numArchivers + 1] = NULL;

    archivers[numArchivers] = archiver;
    archivers[numArchivers + 1] = NULL;

    numArchivers++;

    return 1;

regfailed:
    if (info != NULL)
    {
        allocator.Free((void *) info->extension);
        allocator.Free((void *) info->description);
        allocator.Free((void *) info->author);
        allocator.Free((void *) info->url);
    } /* if */
    allocator.Free(archiver);

    return 0;
} /* doRegisterArchiver */


int PHYSFS_registerArchiver(const PHYSFS_Archiver *archiver)
{
    int retval;
    BAIL_IF(!initialized, PHYSFS_ERR_NOT_INITIALIZED, 0);
    __PHYSFS_platformGrabMutex(stateLock);
    retval = doRegisterArchiver(archiver);
    __PHYSFS_platformReleaseMutex(stateLock);
    return retval;
} /* PHYSFS_registerArchiver */


int PHYSFS_deregisterArchiver(const char *ext)
{
    size_t i;

    BAIL_IF(!initialized, PHYSFS_ERR_NOT_INITIALIZED, 0);
    BAIL_IF(!ext, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    __PHYSFS_platformGrabMutex(stateLock);
    for (i = 0; i < numArchivers; i++)
    {
        if (PHYSFS_utf8stricmp(archiveInfo[i]->extension, ext) == 0)
        {
            const int retval = doDeregisterArchiver(i);
            __PHYSFS_platformReleaseMutex(stateLock);
            return retval;
        } /* if */
    } /* for */
    __PHYSFS_platformReleaseMutex(stateLock);

    BAIL(PHYSFS_ERR_NOT_FOUND, 0);
} /* PHYSFS_deregisterArchiver */


const PHYSFS_ArchiveInfo **PHYSFS_supportedArchiveTypes(void)
{
    BAIL_IF(!initialized, PHYSFS_ERR_NOT_INITIALIZED, NULL);
    return (const PHYSFS_ArchiveInfo **) archiveInfo;
} /* PHYSFS_supportedArchiveTypes */


void PHYSFS_freeList(void *list)
{
    void **i;
    if (list != NULL)
    {
        for (i = (void **) list; *i != NULL; i++)
            allocator.Free(*i);

        allocator.Free(list);
    } /* if */
} /* PHYSFS_freeList */


const char *PHYSFS_getDirSeparator(void)
{
    static char retval[2] = { __PHYSFS_platformDirSeparator, '\0' };
    return retval;
} /* PHYSFS_getDirSeparator */


char **PHYSFS_getCdRomDirs(void)
{
    return doEnumStringList(__PHYSFS_platformDetectAvailableCDs);
} /* PHYSFS_getCdRomDirs */


void PHYSFS_getCdRomDirsCallback(PHYSFS_StringCallback callback, void *data)
{
    __PHYSFS_platformDetectAvailableCDs(callback, data);
} /* PHYSFS_getCdRomDirsCallback */


const char *PHYSFS_getPrefDir(const char *org, const char *app)
{
    const char dirsep = __PHYSFS_platformDirSeparator;
    PHYSFS_Stat statbuf;
    char *ptr = NULL;
    char *endstr = NULL;

    BAIL_IF(!initialized, PHYSFS_ERR_NOT_INITIALIZED, 0);
    BAIL_IF(!org, PHYSFS_ERR_INVALID_ARGUMENT, NULL);
    BAIL_IF(*org == '\0', PHYSFS_ERR_INVALID_ARGUMENT, NULL);
    BAIL_IF(!app, PHYSFS_ERR_INVALID_ARGUMENT, NULL);
    BAIL_IF(*app == '\0', PHYSFS_ERR_INVALID_ARGUMENT, NULL);

    allocator.Free(prefDir);
    prefDir = __PHYSFS_platformCalcPrefDir(org, app);
    BAIL_IF_ERRPASS(!prefDir, NULL);

    assert(strlen(prefDir) > 0);
    endstr = prefDir + (strlen(prefDir) - 1);
    assert(*endstr == dirsep);
    *endstr = '\0';  /* mask out the final dirsep for now. */

    if (!__PHYSFS_platformStat(prefDir, &statbuf, 1))
    {
        for (ptr = strchr(prefDir, dirsep); ptr; ptr = strchr(ptr+1, dirsep))
        {
            *ptr = '\0';
            __PHYSFS_platformMkDir(prefDir);
            *ptr = dirsep;
        } /* for */

        if (!__PHYSFS_platformMkDir(prefDir))
        {
            allocator.Free(prefDir);
            prefDir = NULL;
        } /* if */
    } /* if */

    *endstr = dirsep;  /* readd the final dirsep. */

    return prefDir;
} /* PHYSFS_getPrefDir */


const char *PHYSFS_getBaseDir(void)
{
    return baseDir;   /* this is calculated in PHYSFS_init()... */
} /* PHYSFS_getBaseDir */


const char *__PHYSFS_getUserDir(void)  /* not deprecated internal version. */
{
    return userDir;   /* this is calculated in PHYSFS_init()... */
} /* __PHYSFS_getUserDir */


const char *PHYSFS_getUserDir(void)
{
    return __PHYSFS_getUserDir();
} /* PHYSFS_getUserDir */


const char *PHYSFS_getWriteDir(void)
{
    const char *retval = NULL;

    __PHYSFS_platformGrabMutex(stateLock);
    if (writeDir != NULL)
        retval = writeDir->dirName;
    __PHYSFS_platformReleaseMutex(stateLock);

    return retval;
} /* PHYSFS_getWriteDir */


int PHYSFS_setWriteDir(const char *newDir)
{
    int retval = 1;

    __PHYSFS_platformGrabMutex(stateLock);

    if (writeDir != NULL)
    {
        BAIL_IF_MUTEX_ERRPASS(!freeDirHandle(writeDir, openWriteList),
                            stateLock, 0);
        writeDir = NULL;
    } /* if */

    if (newDir != NULL)
    {
        writeDir = createDirHandle(NULL, newDir, NULL, 1);
        retval = (writeDir != NULL);
    } /* if */

    __PHYSFS_platformReleaseMutex(stateLock);

    return retval;
} /* PHYSFS_setWriteDir */


int PHYSFS_setRoot(const char *archive, const char *subdir)
{
    DirHandle *i;

    BAIL_IF(!archive, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    __PHYSFS_platformGrabMutex(stateLock);

    for (i = searchPath; i != NULL; i = i->next)
    {
        if ((i->dirName != NULL) && (strcmp(archive, i->dirName) == 0))
        {
            if (!subdir || (strcmp(subdir, "/") == 0))
            {
                if (i->root)
                    allocator.Free(i->root);
                i->root = NULL;
                i->rootlen = 0;
            } /* if */
            else
            {
                const size_t len = strlen(subdir) + 1;
                char *ptr = (char *) allocator.Malloc(len);
                BAIL_IF_MUTEX(!ptr, PHYSFS_ERR_OUT_OF_MEMORY, stateLock, 0);
                if (!sanitizePlatformIndependentPath(subdir, ptr))
                {
                    allocator.Free(ptr);
                    BAIL_MUTEX_ERRPASS(stateLock, 0);
                } /* if */

                if (i->root)
                    allocator.Free(i->root);
                i->root = ptr;
                i->rootlen = len;

                if (longest_root < len)
                    longest_root = len;
            } /* else */

            break;
        } /* if */
    } /* for */

    __PHYSFS_platformReleaseMutex(stateLock);
    return 1;
} /* PHYSFS_setRoot */


static int doMount(PHYSFS_Io *io, const char *fname,
                   const char *mountPoint, int appendToPath)
{
    DirHandle *dh;
    DirHandle *prev = NULL;
    DirHandle *i;

    BAIL_IF(!fname, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    if (mountPoint == NULL)
        mountPoint = "/";

    __PHYSFS_platformGrabMutex(stateLock);

    for (i = searchPath; i != NULL; i = i->next)
    {
        /* already in search path? */
        if ((i->dirName != NULL) && (strcmp(fname, i->dirName) == 0))
            BAIL_MUTEX_ERRPASS(stateLock, 1);
        prev = i;
    } /* for */

    dh = createDirHandle(io, fname, mountPoint, 0);
    BAIL_IF_MUTEX_ERRPASS(!dh, stateLock, 0);

    if (appendToPath)
    {
        if (prev == NULL)
            searchPath = dh;
        else
            prev->next = dh;
    } /* if */
    else
    {
        dh->next = searchPath;
        searchPath = dh;
    } /* else */

    __PHYSFS_platformReleaseMutex(stateLock);
    return 1;
} /* doMount */


int PHYSFS_mountIo(PHYSFS_Io *io, const char *fname,
                   const char *mountPoint, int appendToPath)
{
    BAIL_IF(!io, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!fname, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(io->version != 0, PHYSFS_ERR_UNSUPPORTED, 0);
    return doMount(io, fname, mountPoint, appendToPath);
} /* PHYSFS_mountIo */


int PHYSFS_mountMemory(const void *buf, PHYSFS_uint64 len, void (*del)(void *),
                       const char *fname, const char *mountPoint,
                       int appendToPath)
{
    int retval = 0;
    PHYSFS_Io *io = NULL;

    BAIL_IF(!buf, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!fname, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    io = __PHYSFS_createMemoryIo(buf, len, del);
    BAIL_IF_ERRPASS(!io, 0);
    retval = doMount(io, fname, mountPoint, appendToPath);
    if (!retval)
    {
        /* docs say not to call (del) in case of failure, so cheat. */
        MemoryIoInfo *info = (MemoryIoInfo *) io->opaque;
        info->destruct = NULL;
        io->destroy(io);
    } /* if */

    return retval;
} /* PHYSFS_mountMemory */


int PHYSFS_mountHandle(PHYSFS_File *file, const char *fname,
                       const char *mountPoint, int appendToPath)
{
    int retval = 0;
    PHYSFS_Io *io = NULL;

    BAIL_IF(!file, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!fname, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    io = __PHYSFS_createHandleIo(file);
    BAIL_IF_ERRPASS(!io, 0);
    retval = doMount(io, fname, mountPoint, appendToPath);
    if (!retval)
    {
        /* docs say not to destruct in case of failure, so cheat. */
        io->opaque = NULL;
        io->destroy(io);
    } /* if */

    return retval;
} /* PHYSFS_mountHandle */


int PHYSFS_mount(const char *newDir, const char *mountPoint, int appendToPath)
{
    BAIL_IF(!newDir, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    return doMount(NULL, newDir, mountPoint, appendToPath);
} /* PHYSFS_mount */


int PHYSFS_addToSearchPath(const char *newDir, int appendToPath)
{
    return PHYSFS_mount(newDir, NULL, appendToPath);
} /* PHYSFS_addToSearchPath */


int PHYSFS_removeFromSearchPath(const char *oldDir)
{
    return PHYSFS_unmount(oldDir);
} /* PHYSFS_removeFromSearchPath */


int PHYSFS_unmount(const char *oldDir)
{
    DirHandle *i;
    DirHandle *prev = NULL;
    DirHandle *next = NULL;

    BAIL_IF(oldDir == NULL, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    __PHYSFS_platformGrabMutex(stateLock);
    for (i = searchPath; i != NULL; i = i->next)
    {
        if (strcmp(i->dirName, oldDir) == 0)
        {
            next = i->next;
            BAIL_IF_MUTEX_ERRPASS(!freeDirHandle(i, openReadList),
                                stateLock, 0);

            if (prev == NULL)
                searchPath = next;
            else
                prev->next = next;

            BAIL_MUTEX_ERRPASS(stateLock, 1);
        } /* if */
        prev = i;
    } /* for */

    BAIL_MUTEX(PHYSFS_ERR_NOT_MOUNTED, stateLock, 0);
} /* PHYSFS_unmount */


char **PHYSFS_getSearchPath(void)
{
    return doEnumStringList(PHYSFS_getSearchPathCallback);
} /* PHYSFS_getSearchPath */


const char *PHYSFS_getMountPoint(const char *dir)
{
    DirHandle *i;
    __PHYSFS_platformGrabMutex(stateLock);
    for (i = searchPath; i != NULL; i = i->next)
    {
        if (strcmp(i->dirName, dir) == 0)
        {
            const char *retval = ((i->mountPoint) ? i->mountPoint : "/");
            __PHYSFS_platformReleaseMutex(stateLock);
            return retval;
        } /* if */
    } /* for */
    __PHYSFS_platformReleaseMutex(stateLock);

    BAIL(PHYSFS_ERR_NOT_MOUNTED, NULL);
} /* PHYSFS_getMountPoint */


void PHYSFS_getSearchPathCallback(PHYSFS_StringCallback callback, void *data)
{
    DirHandle *i;

    __PHYSFS_platformGrabMutex(stateLock);

    for (i = searchPath; i != NULL; i = i->next)
        callback(data, i->dirName);

    __PHYSFS_platformReleaseMutex(stateLock);
} /* PHYSFS_getSearchPathCallback */


typedef struct setSaneCfgEnumData
{
    const char *archiveExt;
    size_t archiveExtLen;
    int archivesFirst;
    PHYSFS_ErrorCode errcode;
} setSaneCfgEnumData;

static PHYSFS_EnumerateCallbackResult setSaneCfgEnumCallback(void *_data,
                                                const char *dir, const char *f)
{
    setSaneCfgEnumData *data = (setSaneCfgEnumData *) _data;
    const size_t extlen = data->archiveExtLen;
    const size_t l = strlen(f);
    const char *ext;

    if ((l > extlen) && (f[l - extlen - 1] == '.'))
    {
        ext = f + (l - extlen);
        if (PHYSFS_utf8stricmp(ext, data->archiveExt) == 0)
        {
            const char dirsep = __PHYSFS_platformDirSeparator;
            const char *d = PHYSFS_getRealDir(f);
            const size_t allocsize = strlen(d) + l + 2;
            char *str = (char *) __PHYSFS_smallAlloc(allocsize);
            if (str == NULL)
                data->errcode = PHYSFS_ERR_OUT_OF_MEMORY;
            else
            {
                snprintf(str, allocsize, "%s%c%s", d, dirsep, f);
                if (!PHYSFS_mount(str, NULL, data->archivesFirst == 0))
                    data->errcode = currentErrorCode();
                __PHYSFS_smallFree(str);
            } /* else */
        } /* if */
    } /* if */

    /* !!! FIXME: if we want to abort on errors... */
    /*return (data->errcode != PHYSFS_ERR_OK) ? PHYSFS_ENUM_ERROR : PHYSFS_ENUM_OK;*/

    return PHYSFS_ENUM_OK;  /* keep going */
} /* setSaneCfgEnumCallback */


int PHYSFS_setSaneConfig(const char *organization, const char *appName,
                         const char *archiveExt, int includeCdRoms,
                         int archivesFirst)
{
    const char *basedir;
    const char *prefdir;

    BAIL_IF(!initialized, PHYSFS_ERR_NOT_INITIALIZED, 0);

    prefdir = PHYSFS_getPrefDir(organization, appName);
    BAIL_IF_ERRPASS(!prefdir, 0);

    basedir = PHYSFS_getBaseDir();
    BAIL_IF_ERRPASS(!basedir, 0);

    BAIL_IF(!PHYSFS_setWriteDir(prefdir), PHYSFS_ERR_NO_WRITE_DIR, 0);

    /* !!! FIXME: these can fail and we should report that... */

    /* Put write dir first in search path... */
    PHYSFS_mount(prefdir, NULL, 0);

    /* Put base path on search path... */
    PHYSFS_mount(basedir, NULL, 1);

    /* handle CD-ROMs... */
    if (includeCdRoms)
    {
        char **cds = PHYSFS_getCdRomDirs();
        char **i;
        for (i = cds; *i != NULL; i++)
            PHYSFS_mount(*i, NULL, 1);
        PHYSFS_freeList(cds);
    } /* if */

    /* Root out archives, and add them to search path... */
    if (archiveExt != NULL)
    {
        setSaneCfgEnumData data;
        memset(&data, '\0', sizeof (data));
        data.archiveExt = archiveExt;
        data.archiveExtLen = strlen(archiveExt);
        data.archivesFirst = archivesFirst;
        data.errcode = PHYSFS_ERR_OK;
        if (!PHYSFS_enumerate("/", setSaneCfgEnumCallback, &data))
        {
            /* !!! FIXME: use this if we're reporting errors.
            PHYSFS_ErrorCode errcode = currentErrorCode();
            if (errcode == PHYSFS_ERR_APP_CALLBACK)
                errcode = data->errcode; */
        } /* if */
    } /* if */

    return 1;
} /* PHYSFS_setSaneConfig */


void PHYSFS_permitSymbolicLinks(int allow)
{
    allowSymLinks = allow;
} /* PHYSFS_permitSymbolicLinks */


int PHYSFS_symbolicLinksPermitted(void)
{
    return allowSymLinks;
} /* PHYSFS_symbolicLinksPermitted */


/*
 * Verify that (fname) (in platform-independent notation), in relation
 *  to (h) is secure. That means that each element of fname is checked
 *  for symlinks (if they aren't permitted). This also allows for quick
 *  rejection of files that exist outside an archive's mountpoint.
 *
 * With some exceptions (like PHYSFS_mkdir(), which builds multiple subdirs
 *  at a time), you should always pass zero for "allowMissing" for efficiency.
 *
 * (fname) must point to an output from sanitizePlatformIndependentPath(),
 *  since it will make sure that path names are in the right format for
 *  passing certain checks. It will also do checks for "insecure" pathnames
 *  like ".." which should be done once instead of once per archive. This also
 *  gives us license to treat (fname) as scratch space in this function.
 *
 * (fname)'s buffer must have enough space available before it for this
 *  function to prepend any root directory for this DirHandle.
 *
 * Returns non-zero if string is safe, zero if there's a security issue.
 *  PHYSFS_getLastError() will specify what was wrong. (*fname) will be
 *  updated to point past any mount point elements so it is prepared to
 *  be used with the archiver directly.
 */
static int verifyPath(DirHandle *h, char **_fname, int allowMissing)
{
    char *fname = *_fname;
    int retval = 1;
    char *start;
    char *end;

    if ((*fname == '\0') && (!h->root))  /* quick rejection. */
        return 1;

    /* !!! FIXME: This codeblock sucks. */
    if (h->mountPoint != NULL)  /* NULL mountpoint means "/". */
    {
        size_t mntpntlen = strlen(h->mountPoint);
        size_t len = strlen(fname);
        assert(mntpntlen > 1); /* root mount points should be NULL. */
        /* not under the mountpoint, so skip this archive. */
        BAIL_IF(len < mntpntlen-1, PHYSFS_ERR_NOT_FOUND, 0);
        /* !!! FIXME: Case insensitive? */
        retval = strncmp(h->mountPoint, fname, mntpntlen-1);
        BAIL_IF(retval != 0, PHYSFS_ERR_NOT_FOUND, 0);
        if (len > mntpntlen-1)  /* corner case... */
            BAIL_IF(fname[mntpntlen-1]!='/', PHYSFS_ERR_NOT_FOUND, 0);
        fname += mntpntlen-1;  /* move to start of actual archive path. */
        if (*fname == '/')
            fname++;
        *_fname = fname;  /* skip mountpoint for later use. */
        retval = 1;  /* may be reset, below. */
    } /* if */

    /* prepend the root directory, if any. */
    if (h->root)
    {
        const int isempty = (*fname == '\0');
        fname -= h->rootlen - 1;
        strcpy(fname, h->root);
        if (!isempty)
            fname[h->rootlen - 2] = '/';
        *_fname = fname;
    } /* if */

    start = fname;
    if (!allowSymLinks)
    {
        while (1)
        {
            PHYSFS_Stat statbuf;
            int rc = 0;
            end = strchr(start, '/');

            if (end != NULL) *end = '\0';
            rc = h->funcs->stat(h->opaque, fname, &statbuf);
            if (rc)
                rc = (statbuf.filetype == PHYSFS_FILETYPE_SYMLINK);
            else if (currentErrorCode() == PHYSFS_ERR_NOT_FOUND)
                retval = 0;

            if (end != NULL) *end = '/';

            /* insecure path (has a disallowed symlink in it)? */
            BAIL_IF(rc, PHYSFS_ERR_SYMLINK_FORBIDDEN, 0);

            /* break out early if path element is missing. */
            if (!retval)
            {
                /*
                 * We need to clear it if it's the last element of the path,
                 *  since this might be a non-existant file we're opening
                 *  for writing...
                 */
                if ((end == NULL) || (allowMissing))
                    retval = 1;
                break;
            } /* if */

            if (end == NULL)
                break;

            start = end + 1;
        } /* while */
    } /* if */

    return retval;
} /* verifyPath */


/* This must hold the stateLock before calling. */
static int doMkdir(const char *_dname, char *dname)
{
    DirHandle *h = writeDir;
    char *start;
    char *end;
    int retval = 0;
    int exists = 1;  /* force existance check on first path element. */

    assert(h != NULL);

    BAIL_IF_ERRPASS(!sanitizePlatformIndependentPathWithRoot(h, _dname, dname), 0);
    BAIL_IF_ERRPASS(!verifyPath(h, &dname, 1), 0);

    start = dname;
    while (1)
    {
        end = strchr(start, '/');
        if (end != NULL)
            *end = '\0';

        /* only check for existance if all parent dirs existed, too... */
        if (exists)
        {
            PHYSFS_Stat statbuf;
            const int rc = h->funcs->stat(h->opaque, dname, &statbuf);
            if ((!rc) && (currentErrorCode() == PHYSFS_ERR_NOT_FOUND))
                exists = 0;
            retval = ((rc) && (statbuf.filetype == PHYSFS_FILETYPE_DIRECTORY));
        } /* if */

        if (!exists)
            retval = h->funcs->mkdir(h->opaque, dname);

        if (!retval)
            break;

        if (end == NULL)
            break;

        *end = '/';
        start = end + 1;
    } /* while */

    return retval;
} /* doMkdir */


int PHYSFS_mkdir(const char *_dname)
{
    int retval = 0;
    char *dname;
    size_t len;

    BAIL_IF(!_dname, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    __PHYSFS_platformGrabMutex(stateLock);
    BAIL_IF_MUTEX(!writeDir, PHYSFS_ERR_NO_WRITE_DIR, stateLock, 0);
    len = strlen(_dname) + dirHandleRootLen(writeDir) + 1;
    dname = (char *) __PHYSFS_smallAlloc(len);
    BAIL_IF_MUTEX(!dname, PHYSFS_ERR_OUT_OF_MEMORY, stateLock, 0);
    retval = doMkdir(_dname, dname);
    __PHYSFS_platformReleaseMutex(stateLock);
    __PHYSFS_smallFree(dname);
    return retval;
} /* PHYSFS_mkdir */


/* This must hold the stateLock before calling. */
static int doDelete(const char *_fname, char *fname)
{
    DirHandle *h = writeDir;
    BAIL_IF_ERRPASS(!sanitizePlatformIndependentPathWithRoot(h, _fname, fname), 0);
    BAIL_IF_ERRPASS(!verifyPath(h, &fname, 0), 0);
    return h->funcs->remove(h->opaque, fname);
} /* doDelete */


int PHYSFS_delete(const char *_fname)
{
    int retval;
    char *fname;
    size_t len;

    __PHYSFS_platformGrabMutex(stateLock);
    BAIL_IF_MUTEX(!writeDir, PHYSFS_ERR_NO_WRITE_DIR, stateLock, 0);
    len = strlen(_fname) + dirHandleRootLen(writeDir) + 1;
    fname = (char *) __PHYSFS_smallAlloc(len);
    BAIL_IF_MUTEX(!fname, PHYSFS_ERR_OUT_OF_MEMORY, stateLock, 0);
    retval = doDelete(_fname, fname);
    __PHYSFS_platformReleaseMutex(stateLock);
    __PHYSFS_smallFree(fname);
    return retval;
} /* PHYSFS_delete */


static DirHandle *getRealDirHandle(const char *_fname)
{
    DirHandle *retval = NULL;
    char *allocated_fname = NULL;
    char *fname = NULL;
    size_t len;

    BAIL_IF(!_fname, PHYSFS_ERR_INVALID_ARGUMENT, NULL);

    __PHYSFS_platformGrabMutex(stateLock);
    len = strlen(_fname) + longest_root + 1;
    allocated_fname = (char*)__PHYSFS_smallAlloc(len);
    BAIL_IF_MUTEX(!allocated_fname, PHYSFS_ERR_OUT_OF_MEMORY, stateLock, NULL);
    fname = allocated_fname + longest_root;
    if (sanitizePlatformIndependentPath(_fname, fname))
    {
        DirHandle *i;
        for (i = searchPath; i != NULL; i = i->next)
        {
            char *arcfname = fname;
            if (partOfMountPoint(i, arcfname))
            {
                retval = i;
                break;
            } /* if */
            else if (verifyPath(i, &arcfname, 0))
            {
                PHYSFS_Stat statbuf;
                if (i->funcs->stat(i->opaque, arcfname, &statbuf))
                {
                    retval = i;
                    break;
                } /* if */
            } /* if */
        } /* for */
    } /* if */

    __PHYSFS_platformReleaseMutex(stateLock);
    __PHYSFS_smallFree(allocated_fname);
    return retval;
} /* getRealDirHandle */

const char *PHYSFS_getRealDir(const char *fname)
{
    DirHandle *dh = getRealDirHandle(fname);
    return dh ? dh->dirName : NULL;
} /* PHYSFS_getRealDir */


static int locateInStringList(const char *str,
                              char **list,
                              PHYSFS_uint32 *pos)
{
    PHYSFS_uint32 len = *pos;
    PHYSFS_uint32 half_len;
    PHYSFS_uint32 lo = 0;
    PHYSFS_uint32 middle;
    int cmp;

    while (len > 0)
    {
        half_len = len >> 1;
        middle = lo + half_len;
        cmp = strcmp(list[middle], str);

        if (cmp == 0)  /* it's in the list already. */
            return 1;
        else if (cmp > 0)
            len = half_len;
        else
        {
            lo = middle + 1;
            len -= half_len + 1;
        } /* else */
    } /* while */

    *pos = lo;
    return 0;
} /* locateInStringList */


static PHYSFS_EnumerateCallbackResult enumFilesCallback(void *data,
                                        const char *origdir, const char *str)
{
    PHYSFS_uint32 pos;
    void *ptr;
    char *newstr;
    EnumStringListCallbackData *pecd = (EnumStringListCallbackData *) data;

    /*
     * See if file is in the list already, and if not, insert it in there
     *  alphabetically...
     */
    pos = pecd->size;
    if (locateInStringList(str, pecd->list, &pos))
        return PHYSFS_ENUM_OK;  /* already in the list, but keep going. */

    ptr = allocator.Realloc(pecd->list, (pecd->size + 2) * sizeof (char *));
    newstr = (char *) allocator.Malloc(strlen(str) + 1);
    if (ptr != NULL)
        pecd->list = (char **) ptr;

    if ((ptr == NULL) || (newstr == NULL))
    {
        if (newstr)
            allocator.Free(newstr);

        pecd->errcode = PHYSFS_ERR_OUT_OF_MEMORY;
        return PHYSFS_ENUM_ERROR;  /* better luck next time. */
    } /* if */

    strcpy(newstr, str);

    if (pos != pecd->size)
    {
        memmove(&pecd->list[pos+1], &pecd->list[pos],
                 sizeof (char *) * ((pecd->size) - pos));
    } /* if */

    pecd->list[pos] = newstr;
    pecd->size++;

    return PHYSFS_ENUM_OK;
} /* enumFilesCallback */


char **PHYSFS_enumerateFiles(const char *path)
{
    EnumStringListCallbackData ecd;
    memset(&ecd, '\0', sizeof (ecd));
    ecd.list = (char **) allocator.Malloc(sizeof (char *));
    BAIL_IF(!ecd.list, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    if (!PHYSFS_enumerate(path, enumFilesCallback, &ecd))
    {
        const PHYSFS_ErrorCode errcode = currentErrorCode();
        PHYSFS_uint32 i;
        for (i = 0; i < ecd.size; i++)
            allocator.Free(ecd.list[i]);
        allocator.Free(ecd.list);
        BAIL_IF(errcode == PHYSFS_ERR_APP_CALLBACK, ecd.errcode, NULL);
        return NULL;
    } /* if */

    ecd.list[ecd.size] = NULL;
    return ecd.list;
} /* PHYSFS_enumerateFiles */


/*
 * Broke out to seperate function so we can use stack allocation gratuitously.
 */
static PHYSFS_EnumerateCallbackResult enumerateFromMountPoint(DirHandle *i,
                                    const char *arcfname,
                                    PHYSFS_EnumerateCallback callback,
                                    const char *_fname, void *data)
{
    PHYSFS_EnumerateCallbackResult retval;
    const size_t len = strlen(arcfname);
    char *ptr = NULL;
    char *end = NULL;
    const size_t slen = strlen(i->mountPoint) + 1;
    char *mountPoint = (char *) __PHYSFS_smallAlloc(slen);

    BAIL_IF(!mountPoint, PHYSFS_ERR_OUT_OF_MEMORY, PHYSFS_ENUM_ERROR);

    strcpy(mountPoint, i->mountPoint);
    ptr = mountPoint + ((len) ? len + 1 : 0);
    end = strchr(ptr, '/');
    assert(end);  /* should always find a terminating '/'. */
    *end = '\0';
    retval = callback(data, _fname, ptr);
    __PHYSFS_smallFree(mountPoint);

    BAIL_IF(retval == PHYSFS_ENUM_ERROR, PHYSFS_ERR_APP_CALLBACK, retval);
    return retval;
} /* enumerateFromMountPoint */


typedef struct SymlinkFilterData
{
    PHYSFS_EnumerateCallback callback;
    void *callbackData;
    DirHandle *dirhandle;
    const char *arcfname;
    PHYSFS_ErrorCode errcode;
} SymlinkFilterData;

static PHYSFS_EnumerateCallbackResult enumCallbackFilterSymLinks(void *_data,
                                    const char *origdir, const char *fname)
{
    SymlinkFilterData *data = (SymlinkFilterData *) _data;
    const DirHandle *dh = data->dirhandle;
    const char *arcfname = data->arcfname;
    PHYSFS_Stat statbuf;
    const char *trimmedDir = (*arcfname == '/') ? (arcfname + 1) : arcfname;
    const size_t slen = strlen(trimmedDir) + strlen(fname) + 2;
    char *path = (char *) __PHYSFS_smallAlloc(slen);
    PHYSFS_EnumerateCallbackResult retval = PHYSFS_ENUM_OK;

    if (path == NULL)
    {
        data->errcode = PHYSFS_ERR_OUT_OF_MEMORY;
        return PHYSFS_ENUM_ERROR;
    } /* if */

    snprintf(path, slen, "%s%s%s", trimmedDir, *trimmedDir ? "/" : "", fname);

    if (!dh->funcs->stat(dh->opaque, path, &statbuf))
    {
        data->errcode = currentErrorCode();
        retval = PHYSFS_ENUM_ERROR;
    } /* if */
    else
    {
        /* Pass it on to the application if it's not a symlink. */
        if (statbuf.filetype != PHYSFS_FILETYPE_SYMLINK)
        {
            retval = data->callback(data->callbackData, origdir, fname);
            if (retval == PHYSFS_ENUM_ERROR)
                data->errcode = PHYSFS_ERR_APP_CALLBACK;
        } /* if */
    } /* else */

    __PHYSFS_smallFree(path);

    return retval;
} /* enumCallbackFilterSymLinks */


int PHYSFS_enumerate(const char *_fn, PHYSFS_EnumerateCallback cb, void *data)
{
    PHYSFS_EnumerateCallbackResult retval = PHYSFS_ENUM_OK;
    size_t len;
    char *allocated_fname;
    char *fname;

    BAIL_IF(!_fn, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!cb, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    __PHYSFS_platformGrabMutex(stateLock);

    len = strlen(_fn) + longest_root + 1;
    allocated_fname = (char *) __PHYSFS_smallAlloc(len);
    BAIL_IF_MUTEX(!allocated_fname, PHYSFS_ERR_OUT_OF_MEMORY, stateLock, 0);
    fname = allocated_fname + longest_root;
    if (!sanitizePlatformIndependentPath(_fn, fname))
        retval = PHYSFS_ENUM_STOP;
    else
    {
        DirHandle *i;
        SymlinkFilterData filterdata;

        if (!allowSymLinks)
        {
            memset(&filterdata, '\0', sizeof (filterdata));
            filterdata.callback = cb;
            filterdata.callbackData = data;
        } /* if */

        for (i = searchPath; (retval == PHYSFS_ENUM_OK) && i; i = i->next)
        {
            char *arcfname = fname;

            if (partOfMountPoint(i, arcfname))
                retval = enumerateFromMountPoint(i, arcfname, cb, _fn, data);

            else if (verifyPath(i, &arcfname, 0))
            {
                PHYSFS_Stat statbuf;
                if (!i->funcs->stat(i->opaque, arcfname, &statbuf))
                {
                    if (currentErrorCode() == PHYSFS_ERR_NOT_FOUND)
                        continue;  /* no such dir in this archive, skip it. */
                } /* if */

                if (statbuf.filetype != PHYSFS_FILETYPE_DIRECTORY)
                    continue;  /* not a directory in this archive, skip it. */

                else if ((!allowSymLinks) && (i->funcs->info.supportsSymlinks))
                {
                    filterdata.dirhandle = i;
                    filterdata.arcfname = arcfname;
                    filterdata.errcode = PHYSFS_ERR_OK;
                    retval = i->funcs->enumerate(i->opaque, arcfname,
                                                 enumCallbackFilterSymLinks,
                                                 _fn, &filterdata);
                    if (retval == PHYSFS_ENUM_ERROR)
                    {
                        if (currentErrorCode() == PHYSFS_ERR_APP_CALLBACK)
                            PHYSFS_setErrorCode(filterdata.errcode);
                    } /* if */
                } /* else if */
                else
                {
                    retval = i->funcs->enumerate(i->opaque, arcfname,
                                                 cb, _fn, data);
                } /* else */
            } /* else if */
        } /* for */

    } /* if */

    __PHYSFS_platformReleaseMutex(stateLock);

    __PHYSFS_smallFree(allocated_fname);

    return (retval == PHYSFS_ENUM_ERROR) ? 0 : 1;
} /* PHYSFS_enumerate */


typedef struct
{
    PHYSFS_EnumFilesCallback callback;
    void *data;
} LegacyEnumFilesCallbackData;

static PHYSFS_EnumerateCallbackResult enumFilesCallbackAlwaysSucceed(void *d,
                                    const char *origdir, const char *fname)
{
    LegacyEnumFilesCallbackData *cbdata = (LegacyEnumFilesCallbackData *) d;
    cbdata->callback(cbdata->data, origdir, fname);
    return PHYSFS_ENUM_OK;
} /* enumFilesCallbackAlwaysSucceed */

void PHYSFS_enumerateFilesCallback(const char *fname,
                                   PHYSFS_EnumFilesCallback callback,
                                   void *data)
{
    LegacyEnumFilesCallbackData cbdata;
    cbdata.callback = callback;
    cbdata.data = data;
    (void) PHYSFS_enumerate(fname, enumFilesCallbackAlwaysSucceed, &cbdata);
} /* PHYSFS_enumerateFilesCallback */


int PHYSFS_exists(const char *fname)
{
    return (getRealDirHandle(fname) != NULL);
} /* PHYSFS_exists */


PHYSFS_sint64 PHYSFS_getLastModTime(const char *fname)
{
    PHYSFS_Stat statbuf;
    BAIL_IF_ERRPASS(!PHYSFS_stat(fname, &statbuf), -1);
    return statbuf.modtime;
} /* PHYSFS_getLastModTime */


int PHYSFS_isDirectory(const char *fname)
{
    PHYSFS_Stat statbuf;
    BAIL_IF_ERRPASS(!PHYSFS_stat(fname, &statbuf), 0);
    return (statbuf.filetype == PHYSFS_FILETYPE_DIRECTORY);
} /* PHYSFS_isDirectory */


int PHYSFS_isSymbolicLink(const char *fname)
{
    PHYSFS_Stat statbuf;
    BAIL_IF_ERRPASS(!PHYSFS_stat(fname, &statbuf), 0);
    return (statbuf.filetype == PHYSFS_FILETYPE_SYMLINK);
} /* PHYSFS_isSymbolicLink */


static PHYSFS_File *doOpenWrite(const char *_fname, const int appending)
{
    FileHandle *fh = NULL;
    DirHandle *h;
    size_t len;
    char *fname;

    BAIL_IF(!_fname, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    __PHYSFS_platformGrabMutex(stateLock);

    h = writeDir;
    BAIL_IF_MUTEX(!h, PHYSFS_ERR_NO_WRITE_DIR, stateLock, 0);

    len = strlen(_fname) + dirHandleRootLen(h) + 1;
    fname = (char *) __PHYSFS_smallAlloc(len);
    BAIL_IF_MUTEX(!fname, PHYSFS_ERR_OUT_OF_MEMORY, stateLock, 0);

    if (sanitizePlatformIndependentPathWithRoot(h, _fname, fname))
    {
        PHYSFS_Io *io = NULL;
        char *arcfname = fname;
        if (verifyPath(h, &arcfname, 0))
        {
            const PHYSFS_Archiver *f = h->funcs;
            if (appending)
                io = f->openAppend(h->opaque, arcfname);
            else
                io = f->openWrite(h->opaque, arcfname);

            if (io)
            {
                fh = (FileHandle *) allocator.Malloc(sizeof (FileHandle));
                if (fh == NULL)
                {
                    io->destroy(io);
                    PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
                } /* if */
                else
                {
                    memset(fh, '\0', sizeof (FileHandle));
                    fh->io = io;
                    fh->dirHandle = h;
                    fh->next = openWriteList;
                    openWriteList = fh;
                } /* else */
            } /* if */
        } /* if */
    } /* if */

    __PHYSFS_platformReleaseMutex(stateLock);

    __PHYSFS_smallFree(fname);
    return ((PHYSFS_File *) fh);
} /* doOpenWrite */


PHYSFS_File *PHYSFS_openWrite(const char *filename)
{
    return doOpenWrite(filename, 0);
} /* PHYSFS_openWrite */


PHYSFS_File *PHYSFS_openAppend(const char *filename)
{
    return doOpenWrite(filename, 1);
} /* PHYSFS_openAppend */


PHYSFS_File *PHYSFS_openRead(const char *_fname)
{
    FileHandle *fh = NULL;
    char *allocated_fname;
    char *fname;
    size_t len;

    BAIL_IF(!_fname, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    __PHYSFS_platformGrabMutex(stateLock);

    BAIL_IF_MUTEX(!searchPath, PHYSFS_ERR_NOT_FOUND, stateLock, 0);

    len = strlen(_fname) + longest_root + 1;
    allocated_fname = (char *) __PHYSFS_smallAlloc(len);
    BAIL_IF_MUTEX(!allocated_fname, PHYSFS_ERR_OUT_OF_MEMORY, stateLock, 0);
    fname = allocated_fname + longest_root;

    if (sanitizePlatformIndependentPath(_fname, fname))
    {
        PHYSFS_Io *io = NULL;
        DirHandle *i;

        for (i = searchPath; i != NULL; i = i->next)
        {
            char *arcfname = fname;
            if (verifyPath(i, &arcfname, 0))
            {
                io = i->funcs->openRead(i->opaque, arcfname);
                if (io)
                    break;
            } /* if */
        } /* for */

        if (io)
        {
            fh = (FileHandle *) allocator.Malloc(sizeof (FileHandle));
            if (fh == NULL)
            {
                io->destroy(io);
                PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
            } /* if */
            else
            {
                memset(fh, '\0', sizeof (FileHandle));
                fh->io = io;
                fh->forReading = 1;
                fh->dirHandle = i;
                fh->next = openReadList;
                openReadList = fh;
            } /* else */
        } /* if */
    } /* if */

    __PHYSFS_platformReleaseMutex(stateLock);
    __PHYSFS_smallFree(allocated_fname);
    return ((PHYSFS_File *) fh);
} /* PHYSFS_openRead */


static int closeHandleInOpenList(FileHandle **list, FileHandle *handle)
{
    FileHandle *prev = NULL;
    FileHandle *i;

    for (i = *list; i != NULL; i = i->next)
    {
        if (i == handle)  /* handle is in this list? */
        {
            PHYSFS_Io *io = handle->io;
            PHYSFS_uint8 *tmp = handle->buffer;

            /* send our buffer to io... */
            if (!handle->forReading)
            {
                if (!PHYSFS_flush((PHYSFS_File *) handle))
                    return -1;

                /* ...then have io send it to the disk... */
                else if (io->flush && !io->flush(io))
                    return -1;
            } /* if */

            /* ...then close the underlying file. */
            io->destroy(io);

            if (tmp != NULL)  /* free any associated buffer. */
                allocator.Free(tmp);

            if (prev == NULL)
                *list = handle->next;
            else
                prev->next = handle->next;

            allocator.Free(handle);
            return 1;
        } /* if */
        prev = i;
    } /* for */

    return 0;
} /* closeHandleInOpenList */


int PHYSFS_close(PHYSFS_File *_handle)
{
    FileHandle *handle = (FileHandle *) _handle;
    int rc;

    __PHYSFS_platformGrabMutex(stateLock);

    /* -1 == close failure. 0 == not found. 1 == success. */
    rc = closeHandleInOpenList(&openReadList, handle);
    BAIL_IF_MUTEX_ERRPASS(rc == -1, stateLock, 0);
    if (!rc)
    {
        rc = closeHandleInOpenList(&openWriteList, handle);
        BAIL_IF_MUTEX_ERRPASS(rc == -1, stateLock, 0);
    } /* if */

    __PHYSFS_platformReleaseMutex(stateLock);
    BAIL_IF(!rc, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    return 1;
} /* PHYSFS_close */


static PHYSFS_sint64 doBufferedRead(FileHandle *fh, void *_buffer, size_t len)
{
    PHYSFS_uint8 *buffer = (PHYSFS_uint8 *) _buffer;
    PHYSFS_sint64 retval = 0;

    while (len > 0)
    {
        const size_t avail = fh->buffill - fh->bufpos;
        if (avail > 0)  /* data available in the buffer. */
        {
            const size_t cpy = (len < avail) ? len : avail;
            memcpy(buffer, fh->buffer + fh->bufpos, cpy);
            assert(len >= cpy);
            buffer += cpy;
            len -= cpy;
            fh->bufpos += cpy;
            retval += cpy;
        } /* if */

        else   /* buffer is empty, refill it. */
        {
            PHYSFS_Io *io = fh->io;
            const PHYSFS_sint64 rc = io->read(io, fh->buffer, fh->bufsize);
            fh->bufpos = 0;
            if (rc > 0)
                fh->buffill = (size_t) rc;
            else
            {
                fh->buffill = 0;
                if (retval == 0)  /* report already-read data, or failure. */
                    retval = rc;
                break;
            } /* else */
        } /* else */
    } /* while */

    return retval;
} /* doBufferedRead */


PHYSFS_sint64 PHYSFS_read(PHYSFS_File *handle, void *buffer,
                          PHYSFS_uint32 size, PHYSFS_uint32 count)
{
    const PHYSFS_uint64 len = ((PHYSFS_uint64) size) * ((PHYSFS_uint64) count);
    const PHYSFS_sint64 retval = PHYSFS_readBytes(handle, buffer, len);
    return ( (retval <= 0) ? retval : (retval / ((PHYSFS_sint64) size)) );
} /* PHYSFS_read */


PHYSFS_sint64 PHYSFS_readBytes(PHYSFS_File *handle, void *buffer,
                               PHYSFS_uint64 _len)
{
    const size_t len = (size_t) _len;
    FileHandle *fh = (FileHandle *) handle;

#ifdef PHYSFS_NO_64BIT_SUPPORT
    const PHYSFS_uint64 maxlen = __PHYSFS_UI64(0x7FFFFFFF);
#else
    const PHYSFS_uint64 maxlen = __PHYSFS_UI64(0x7FFFFFFFFFFFFFFF);
#endif

    if (!__PHYSFS_ui64FitsAddressSpace(_len))
        BAIL(PHYSFS_ERR_INVALID_ARGUMENT, -1);

    BAIL_IF(_len > maxlen, PHYSFS_ERR_INVALID_ARGUMENT, -1);
    BAIL_IF(!fh->forReading, PHYSFS_ERR_OPEN_FOR_WRITING, -1);
    BAIL_IF_ERRPASS(len == 0, 0);
    if (fh->buffer)
        return doBufferedRead(fh, buffer, len);

    return fh->io->read(fh->io, buffer, len);
} /* PHYSFS_readBytes */


static PHYSFS_sint64 doBufferedWrite(PHYSFS_File *handle, const void *buffer,
                                     const size_t len)
{
    FileHandle *fh = (FileHandle *) handle;

    /* whole thing fits in the buffer? */
    if ((fh->buffill + len) < fh->bufsize)
    {
        memcpy(fh->buffer + fh->buffill, buffer, len);
        fh->buffill += len;
        return (PHYSFS_sint64) len;
    } /* if */

    /* would overflow buffer. Flush and then write the new objects, too. */
    BAIL_IF_ERRPASS(!PHYSFS_flush(handle), -1);
    return fh->io->write(fh->io, buffer, len);
} /* doBufferedWrite */


PHYSFS_sint64 PHYSFS_write(PHYSFS_File *handle, const void *buffer,
                           PHYSFS_uint32 size, PHYSFS_uint32 count)
{
    const PHYSFS_uint64 len = ((PHYSFS_uint64) size) * ((PHYSFS_uint64) count);
    const PHYSFS_sint64 retval = PHYSFS_writeBytes(handle, buffer, len);
    return ( (retval <= 0) ? retval : (retval / ((PHYSFS_sint64) size)) );
} /* PHYSFS_write */


PHYSFS_sint64 PHYSFS_writeBytes(PHYSFS_File *handle, const void *buffer,
                                PHYSFS_uint64 _len)
{
    const size_t len = (size_t) _len;
    FileHandle *fh = (FileHandle *) handle;

#ifdef PHYSFS_NO_64BIT_SUPPORT
    const PHYSFS_uint64 maxlen = __PHYSFS_UI64(0x7FFFFFFF);
#else
    const PHYSFS_uint64 maxlen = __PHYSFS_UI64(0x7FFFFFFFFFFFFFFF);
#endif

    if (!__PHYSFS_ui64FitsAddressSpace(_len))
        BAIL(PHYSFS_ERR_INVALID_ARGUMENT, -1);

    BAIL_IF(_len > maxlen, PHYSFS_ERR_INVALID_ARGUMENT, -1);
    BAIL_IF(fh->forReading, PHYSFS_ERR_OPEN_FOR_READING, -1);
    BAIL_IF_ERRPASS(len == 0, 0);
    if (fh->buffer)
        return doBufferedWrite(handle, buffer, len);

    return fh->io->write(fh->io, buffer, len);
} /* PHYSFS_write */


int PHYSFS_eof(PHYSFS_File *handle)
{
    FileHandle *fh = (FileHandle *) handle;

    if (!fh->forReading)  /* never EOF on files opened for write/append. */
        return 0;

    /* can't be eof if buffer isn't empty */
    if (fh->bufpos == fh->buffill)
    {
        /* check the Io. */
        PHYSFS_Io *io = fh->io;
        const PHYSFS_sint64 pos = io->tell(io);
        const PHYSFS_sint64 len = io->length(io);
        if ((pos < 0) || (len < 0))
            return 0;  /* beats me. */
        return (pos >= len);
    } /* if */

    return 0;
} /* PHYSFS_eof */


PHYSFS_sint64 PHYSFS_tell(PHYSFS_File *handle)
{
    FileHandle *fh = (FileHandle *) handle;
    const PHYSFS_sint64 pos = fh->io->tell(fh->io);
    const PHYSFS_sint64 retval = fh->forReading ?
                                 (pos - fh->buffill) + fh->bufpos :
                                 (pos + fh->buffill);
    return retval;
} /* PHYSFS_tell */


int PHYSFS_seek(PHYSFS_File *handle, PHYSFS_uint64 pos)
{
    FileHandle *fh = (FileHandle *) handle;
    BAIL_IF_ERRPASS(!PHYSFS_flush(handle), 0);

    if (fh->buffer && fh->forReading)
    {
        /* avoid throwing away our precious buffer if seeking within it. */
        PHYSFS_sint64 offset = pos - PHYSFS_tell(handle);
        if ( /* seeking within the already-buffered range? */
             /* forward? */
            ((offset >= 0) && (((size_t)offset) <= fh->buffill-fh->bufpos)) ||
            /* backward? */
            ((offset < 0) && (((size_t) -offset) <= fh->bufpos)) )
        {
            fh->bufpos = (size_t) (((PHYSFS_sint64) fh->bufpos) + offset);
            return 1; /* successful seek */
        } /* if */
    } /* if */

    /* we have to fall back to a 'raw' seek. */
    fh->buffill = fh->bufpos = 0;
    return fh->io->seek(fh->io, pos);
} /* PHYSFS_seek */


PHYSFS_sint64 PHYSFS_fileLength(PHYSFS_File *handle)
{
    PHYSFS_Io *io = ((FileHandle *) handle)->io;
    return io->length(io);
} /* PHYSFS_filelength */


int PHYSFS_setBuffer(PHYSFS_File *handle, PHYSFS_uint64 _bufsize)
{
    FileHandle *fh = (FileHandle *) handle;
    const size_t bufsize = (size_t) _bufsize;

    if (!__PHYSFS_ui64FitsAddressSpace(_bufsize))
        BAIL(PHYSFS_ERR_INVALID_ARGUMENT, 0);

    BAIL_IF_ERRPASS(!PHYSFS_flush(handle), 0);

    /*
     * For reads, we need to move the file pointer to where it would be
     *  if we weren't buffering, so that the next read will get the
     *  right chunk of stuff from the file. PHYSFS_flush() handles writes.
     */
    if ((fh->forReading) && (fh->buffill != fh->bufpos))
    {
        PHYSFS_uint64 pos;
        const PHYSFS_sint64 curpos = fh->io->tell(fh->io);
        BAIL_IF_ERRPASS(curpos == -1, 0);
        pos = ((curpos - fh->buffill) + fh->bufpos);
        BAIL_IF_ERRPASS(!fh->io->seek(fh->io, pos), 0);
    } /* if */

    if (bufsize == 0)  /* delete existing buffer. */
    {
        if (fh->buffer)
        {
            allocator.Free(fh->buffer);
            fh->buffer = NULL;
        } /* if */
    } /* if */

    else
    {
        PHYSFS_uint8 *newbuf;
        newbuf = (PHYSFS_uint8 *) allocator.Realloc(fh->buffer, bufsize);
        BAIL_IF(!newbuf, PHYSFS_ERR_OUT_OF_MEMORY, 0);
        fh->buffer = newbuf;
    } /* else */

    fh->bufsize = bufsize;
    fh->buffill = fh->bufpos = 0;
    return 1;
} /* PHYSFS_setBuffer */


int PHYSFS_flush(PHYSFS_File *handle)
{
    FileHandle *fh = (FileHandle *) handle;
    PHYSFS_Io *io;
    PHYSFS_sint64 rc;

    if ((fh->forReading) || (fh->bufpos == fh->buffill))
        return 1;  /* open for read or buffer empty are successful no-ops. */

    /* dump buffer to disk. */
    io = fh->io;
    rc = io->write(io, fh->buffer + fh->bufpos, fh->buffill - fh->bufpos);
    BAIL_IF_ERRPASS(rc <= 0, 0);
    fh->bufpos = fh->buffill = 0;
    return 1;
} /* PHYSFS_flush */


int PHYSFS_stat(const char *_fname, PHYSFS_Stat *stat)
{
    int retval = 0;
    char *allocated_fname;
    char *fname;
    size_t len;

    BAIL_IF(!_fname, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    BAIL_IF(!stat, PHYSFS_ERR_INVALID_ARGUMENT, 0);

    /* set some sane defaults... */
    stat->filesize = -1;
    stat->modtime = -1;
    stat->createtime = -1;
    stat->accesstime = -1;
    stat->filetype = PHYSFS_FILETYPE_OTHER;
    stat->readonly = 1;

    __PHYSFS_platformGrabMutex(stateLock);
    len = strlen(_fname) + longest_root + 1;
    allocated_fname = (char *) __PHYSFS_smallAlloc(len);
    BAIL_IF_MUTEX(!allocated_fname, PHYSFS_ERR_OUT_OF_MEMORY, stateLock, 0);
    fname = allocated_fname + longest_root;

    if (sanitizePlatformIndependentPath(_fname, fname))
    {
        if (*fname == '\0')
        {
            stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
            stat->readonly = !writeDir; /* Writeable if we have a writeDir */
            retval = 1;
        } /* if */
        else
        {
            DirHandle *i;
            int exists = 0;
            for (i = searchPath; ((i != NULL) && (!exists)); i = i->next)
            {
                char *arcfname = fname;
                exists = partOfMountPoint(i, arcfname);
                if (exists)
                {
                    stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
                    stat->readonly = 1;
                    retval = 1;
                } /* if */
                else if (verifyPath(i, &arcfname, 0))
                {
                    retval = i->funcs->stat(i->opaque, arcfname, stat);
                    if ((retval) || (currentErrorCode() != PHYSFS_ERR_NOT_FOUND))
                        exists = 1;
                } /* else if */
            } /* for */
        } /* else */
    } /* if */

    __PHYSFS_platformReleaseMutex(stateLock);
    __PHYSFS_smallFree(allocated_fname);
    return retval;
} /* PHYSFS_stat */


int __PHYSFS_readAll(PHYSFS_Io *io, void *buf, const size_t _len)
{
    const PHYSFS_uint64 len = (PHYSFS_uint64) _len;
    return ((size_t)io->read(io, buf, len) == len);
} /* __PHYSFS_readAll */


void *__PHYSFS_initSmallAlloc(void *ptr, const size_t len)
{
    void *useHeap = ((ptr == NULL) ? ((void *) 1) : ((void *) 0));
    if (useHeap)  /* too large for stack allocation or alloca() failed. */
        ptr = allocator.Malloc(len+sizeof (void *));

    if (ptr != NULL)
    {
        void **retval = (void **) ptr;
        /*printf("%s alloc'd (%lld) bytes at (%p).\n",
                useHeap ? "heap" : "stack", (long long) len, ptr);*/
        *retval = useHeap;
        return retval + 1;
    } /* if */

    return NULL;  /* allocation failed. */
} /* __PHYSFS_initSmallAlloc */


void __PHYSFS_smallFree(void *ptr)
{
    if (ptr != NULL)
    {
        void **block = ((void **) ptr) - 1;
        const int useHeap = (*block != NULL);
        if (useHeap)
            allocator.Free(block);
        /*printf("%s free'd (%p).\n", useHeap ? "heap" : "stack", block);*/
    } /* if */
} /* __PHYSFS_smallFree */


int PHYSFS_setAllocator(const PHYSFS_Allocator *a)
{
    BAIL_IF(initialized, PHYSFS_ERR_IS_INITIALIZED, 0);
    externalAllocator = (a != NULL);
    if (externalAllocator)
        memcpy(&allocator, a, sizeof (PHYSFS_Allocator));

    return 1;
} /* PHYSFS_setAllocator */


const PHYSFS_Allocator *PHYSFS_getAllocator(void)
{
    BAIL_IF(!initialized, PHYSFS_ERR_NOT_INITIALIZED, NULL);
    return &allocator;
} /* PHYSFS_getAllocator */


static void *mallocAllocatorMalloc(PHYSFS_uint64 s)
{
    if (!__PHYSFS_ui64FitsAddressSpace(s))
        BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    #undef malloc
    return malloc((size_t) s);
} /* mallocAllocatorMalloc */


static void *mallocAllocatorRealloc(void *ptr, PHYSFS_uint64 s)
{
    if (!__PHYSFS_ui64FitsAddressSpace(s))
        BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    #undef realloc
    return realloc(ptr, (size_t) s);
} /* mallocAllocatorRealloc */


static void mallocAllocatorFree(void *ptr)
{
    #undef free
    free(ptr);
} /* mallocAllocatorFree */


static void setDefaultAllocator(void)
{
    assert(!externalAllocator);
    allocator.Init = NULL;
    allocator.Deinit = NULL;
    allocator.Malloc = mallocAllocatorMalloc;
    allocator.Realloc = mallocAllocatorRealloc;
    allocator.Free = mallocAllocatorFree;
} /* setDefaultAllocator */


int __PHYSFS_DirTreeInit(__PHYSFS_DirTree *dt, const size_t entrylen)
{
    static char rootpath[2] = { '/', '\0' };
    size_t alloclen;

    assert(entrylen >= sizeof (__PHYSFS_DirTreeEntry));

    memset(dt, '\0', sizeof (*dt));

    dt->root = (__PHYSFS_DirTreeEntry *) allocator.Malloc(entrylen);
    BAIL_IF(!dt->root, PHYSFS_ERR_OUT_OF_MEMORY, 0);
    memset(dt->root, '\0', entrylen);
    dt->root->name = rootpath;
    dt->root->isdir = 1;
    dt->hashBuckets = 64;
    if (!dt->hashBuckets)
        dt->hashBuckets = 1;
    dt->entrylen = entrylen;

    alloclen = dt->hashBuckets * sizeof (__PHYSFS_DirTreeEntry *);
    dt->hash = (__PHYSFS_DirTreeEntry **) allocator.Malloc(alloclen);
    BAIL_IF(!dt->hash, PHYSFS_ERR_OUT_OF_MEMORY, 0);
    memset(dt->hash, '\0', alloclen);

    return 1;
} /* __PHYSFS_DirTreeInit */


static inline PHYSFS_uint32 hashPathName(__PHYSFS_DirTree *dt, const char *name)
{
    return __PHYSFS_hashString(name, strlen(name)) % dt->hashBuckets;
} /* hashPathName */


/* Fill in missing parent directories. */
static __PHYSFS_DirTreeEntry *addAncestors(__PHYSFS_DirTree *dt, char *name)
{
    __PHYSFS_DirTreeEntry *retval = dt->root;
    char *sep = strrchr(name, '/');

    if (sep)
    {
        *sep = '\0';  /* chop off last piece. */
        retval = (__PHYSFS_DirTreeEntry *) __PHYSFS_DirTreeFind(dt, name);

        if (retval != NULL)
        {
            *sep = '/';
            BAIL_IF(!retval->isdir, PHYSFS_ERR_CORRUPT, NULL);
            return retval;  /* already hashed. */
        } /* if */

        /* okay, this is a new dir. Build and hash us. */
        retval = (__PHYSFS_DirTreeEntry*)__PHYSFS_DirTreeAdd(dt, name, 1);
        *sep = '/';
    } /* if */

    return retval;
} /* addAncestors */


void *__PHYSFS_DirTreeAdd(__PHYSFS_DirTree *dt, char *name, const int isdir)
{
    __PHYSFS_DirTreeEntry *retval = (__PHYSFS_DirTreeEntry *)__PHYSFS_DirTreeFind(dt, name);
    if (!retval)
    {
        const size_t alloclen = strlen(name) + 1 + dt->entrylen;
        PHYSFS_uint32 hashval;
        __PHYSFS_DirTreeEntry *parent = addAncestors(dt, name);
        BAIL_IF_ERRPASS(!parent, NULL);
        assert(dt->entrylen >= sizeof (__PHYSFS_DirTreeEntry));
        retval = (__PHYSFS_DirTreeEntry *) allocator.Malloc(alloclen);
        BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        memset(retval, '\0', dt->entrylen);
        retval->name = ((char *) retval) + dt->entrylen;
        strcpy(retval->name, name);
        hashval = hashPathName(dt, name);
        retval->hashnext = dt->hash[hashval];
        dt->hash[hashval] = retval;
        retval->sibling = parent->children;
        retval->isdir = isdir;
        parent->children = retval;
    } /* if */

    return retval;
} /* __PHYSFS_DirTreeAdd */


/* Find the __PHYSFS_DirTreeEntry for a path in platform-independent notation. */
void *__PHYSFS_DirTreeFind(__PHYSFS_DirTree *dt, const char *path)
{
    PHYSFS_uint32 hashval;
    __PHYSFS_DirTreeEntry *prev = NULL;
    __PHYSFS_DirTreeEntry *retval;

    if (*path == '\0')
        return dt->root;

    hashval = hashPathName(dt, path);
    for (retval = dt->hash[hashval]; retval; retval = retval->hashnext)
    {
        if (strcmp(retval->name, path) == 0)
        {
            if (prev != NULL)  /* move this to the front of the list */
            {
                prev->hashnext = retval->hashnext;
                retval->hashnext = dt->hash[hashval];
                dt->hash[hashval] = retval;
            } /* if */

            return retval;
        } /* if */

        prev = retval;
    } /* for */

    BAIL(PHYSFS_ERR_NOT_FOUND, NULL);
} /* __PHYSFS_DirTreeFind */

PHYSFS_EnumerateCallbackResult __PHYSFS_DirTreeEnumerate(void *opaque,
                              const char *dname, PHYSFS_EnumerateCallback cb,
                              const char *origdir, void *callbackdata)
{
    PHYSFS_EnumerateCallbackResult retval = PHYSFS_ENUM_OK;
    __PHYSFS_DirTree *tree = (__PHYSFS_DirTree *) opaque;
    const __PHYSFS_DirTreeEntry *entry = (__PHYSFS_DirTreeEntry *)__PHYSFS_DirTreeFind(tree, dname);
    BAIL_IF(!entry, PHYSFS_ERR_NOT_FOUND, PHYSFS_ENUM_ERROR);

    entry = entry->children;

    while (entry && (retval == PHYSFS_ENUM_OK))
    {
        const char *name = entry->name;
        const char *ptr = strrchr(name, '/');
        retval = cb(callbackdata, origdir, ptr ? ptr + 1 : name);
        BAIL_IF(retval == PHYSFS_ENUM_ERROR, PHYSFS_ERR_APP_CALLBACK, retval);
        entry = entry->sibling;
    } /* while */

    return retval;
} /* __PHYSFS_DirTreeEnumerate */


void __PHYSFS_DirTreeDeinit(__PHYSFS_DirTree *dt)
{
    if (!dt)
        return;

    if (dt->root)
    {
        assert(dt->root->sibling == NULL);
        assert(dt->hash || (dt->root->children == NULL));
        allocator.Free(dt->root);
    } /* if */

    if (dt->hash)
    {
        size_t i;
        for (i = 0; i < dt->hashBuckets; i++)
        {
            __PHYSFS_DirTreeEntry *entry;
            __PHYSFS_DirTreeEntry *next;
            for (entry = dt->hash[i]; entry; entry = next)
            {
                next = entry->hashnext;
                allocator.Free(entry);
            } /* for */
        } /* for */
        allocator.Free(dt->hash);
    } /* if */
} /* __PHYSFS_DirTreeDeinit */

/* end of physfs.c ... */

/**
 * PhysicsFS; a portable, flexible file i/o abstraction.
 *
 * Documentation is in physfs.h. It's verbose, honest.  :)
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#ifndef PHYSFS_Swap16
static inline PHYSFS_uint16 PHYSFS_Swap16(PHYSFS_uint16 D)
{
    return ((D<<8)|(D>>8));
}
#endif
#ifndef PHYSFS_Swap32
static inline PHYSFS_uint32 PHYSFS_Swap32(PHYSFS_uint32 D)
{
    return ((D<<24)|((D<<8)&0x00FF0000)|((D>>8)&0x0000FF00)|(D>>24));
}
#endif
#ifndef PHYSFS_NO_64BIT_SUPPORT
#ifndef PHYSFS_Swap64
static inline PHYSFS_uint64 PHYSFS_Swap64(PHYSFS_uint64 val) {
    PHYSFS_uint32 hi, lo;

    /* Separate into high and low 32-bit values and swap them */
    lo = (PHYSFS_uint32)(val&0xFFFFFFFF);
    val >>= 32;
    hi = (PHYSFS_uint32)(val&0xFFFFFFFF);
    val = PHYSFS_Swap32(lo);
    val <<= 32;
    val |= PHYSFS_Swap32(hi);
    return val;
}
#endif
#else
#ifndef PHYSFS_Swap64
/* This is mainly to keep compilers from complaining in PHYSFS code.
   If there is no real 64-bit datatype, then compilers will complain about
   the fake 64-bit datatype that PHYSFS provides when it compiles user code.
*/
#define PHYSFS_Swap64(X)    (X)
#endif
#endif /* PHYSFS_NO_64BIT_SUPPORT */


/* Byteswap item from the specified endianness to the native endianness */
#if PHYSFS_BYTEORDER == PHYSFS_LIL_ENDIAN
PHYSFS_uint16 PHYSFS_swapULE16(PHYSFS_uint16 x) { return x; }
PHYSFS_sint16 PHYSFS_swapSLE16(PHYSFS_sint16 x) { return x; }
PHYSFS_uint32 PHYSFS_swapULE32(PHYSFS_uint32 x) { return x; }
PHYSFS_sint32 PHYSFS_swapSLE32(PHYSFS_sint32 x) { return x; }
PHYSFS_uint64 PHYSFS_swapULE64(PHYSFS_uint64 x) { return x; }
PHYSFS_sint64 PHYSFS_swapSLE64(PHYSFS_sint64 x) { return x; }

PHYSFS_uint16 PHYSFS_swapUBE16(PHYSFS_uint16 x) { return PHYSFS_Swap16(x); }
PHYSFS_sint16 PHYSFS_swapSBE16(PHYSFS_sint16 x) { return PHYSFS_Swap16(x); }
PHYSFS_uint32 PHYSFS_swapUBE32(PHYSFS_uint32 x) { return PHYSFS_Swap32(x); }
PHYSFS_sint32 PHYSFS_swapSBE32(PHYSFS_sint32 x) { return PHYSFS_Swap32(x); }
PHYSFS_uint64 PHYSFS_swapUBE64(PHYSFS_uint64 x) { return PHYSFS_Swap64(x); }
PHYSFS_sint64 PHYSFS_swapSBE64(PHYSFS_sint64 x) { return PHYSFS_Swap64(x); }
#else
PHYSFS_uint16 PHYSFS_swapULE16(PHYSFS_uint16 x) { return PHYSFS_Swap16(x); }
PHYSFS_sint16 PHYSFS_swapSLE16(PHYSFS_sint16 x) { return PHYSFS_Swap16(x); }
PHYSFS_uint32 PHYSFS_swapULE32(PHYSFS_uint32 x) { return PHYSFS_Swap32(x); }
PHYSFS_sint32 PHYSFS_swapSLE32(PHYSFS_sint32 x) { return PHYSFS_Swap32(x); }
PHYSFS_uint64 PHYSFS_swapULE64(PHYSFS_uint64 x) { return PHYSFS_Swap64(x); }
PHYSFS_sint64 PHYSFS_swapSLE64(PHYSFS_sint64 x) { return PHYSFS_Swap64(x); }

PHYSFS_uint16 PHYSFS_swapUBE16(PHYSFS_uint16 x) { return x; }
PHYSFS_sint16 PHYSFS_swapSBE16(PHYSFS_sint16 x) { return x; }
PHYSFS_uint32 PHYSFS_swapUBE32(PHYSFS_uint32 x) { return x; }
PHYSFS_sint32 PHYSFS_swapSBE32(PHYSFS_sint32 x) { return x; }
PHYSFS_uint64 PHYSFS_swapUBE64(PHYSFS_uint64 x) { return x; }
PHYSFS_sint64 PHYSFS_swapSBE64(PHYSFS_sint64 x) { return x; }
#endif

static inline int readAll(PHYSFS_File *file, void *val, const size_t len)
{
    return ((size_t)PHYSFS_readBytes(file, val, len) == len);
} /* readAll */

#define PHYSFS_BYTEORDER_READ(datatype, swaptype) \
    int PHYSFS_read##swaptype(PHYSFS_File *file, PHYSFS_##datatype *val) { \
        PHYSFS_##datatype in; \
        BAIL_IF(val == NULL, PHYSFS_ERR_INVALID_ARGUMENT, 0); \
        BAIL_IF_ERRPASS(!readAll(file, &in, sizeof (in)), 0); \
        *val = PHYSFS_swap##swaptype(in); \
        return 1; \
    }

PHYSFS_BYTEORDER_READ(sint16, SLE16)
PHYSFS_BYTEORDER_READ(uint16, ULE16)
PHYSFS_BYTEORDER_READ(sint16, SBE16)
PHYSFS_BYTEORDER_READ(uint16, UBE16)
PHYSFS_BYTEORDER_READ(sint32, SLE32)
PHYSFS_BYTEORDER_READ(uint32, ULE32)
PHYSFS_BYTEORDER_READ(sint32, SBE32)
PHYSFS_BYTEORDER_READ(uint32, UBE32)
PHYSFS_BYTEORDER_READ(sint64, SLE64)
PHYSFS_BYTEORDER_READ(uint64, ULE64)
PHYSFS_BYTEORDER_READ(sint64, SBE64)
PHYSFS_BYTEORDER_READ(uint64, UBE64)


static inline int writeAll(PHYSFS_File *f, const void *val, const size_t len)
{
    return ((size_t)PHYSFS_writeBytes(f, val, len) == len);
} /* writeAll */

#define PHYSFS_BYTEORDER_WRITE(datatype, swaptype) \
    int PHYSFS_write##swaptype(PHYSFS_File *file, PHYSFS_##datatype val) { \
        const PHYSFS_##datatype out = PHYSFS_swap##swaptype(val); \
        BAIL_IF_ERRPASS(!writeAll(file, &out, sizeof (out)), 0); \
        return 1; \
    }

PHYSFS_BYTEORDER_WRITE(sint16, SLE16)
PHYSFS_BYTEORDER_WRITE(uint16, ULE16)
PHYSFS_BYTEORDER_WRITE(sint16, SBE16)
PHYSFS_BYTEORDER_WRITE(uint16, UBE16)
PHYSFS_BYTEORDER_WRITE(sint32, SLE32)
PHYSFS_BYTEORDER_WRITE(uint32, ULE32)
PHYSFS_BYTEORDER_WRITE(sint32, SBE32)
PHYSFS_BYTEORDER_WRITE(uint32, UBE32)
PHYSFS_BYTEORDER_WRITE(sint64, SLE64)
PHYSFS_BYTEORDER_WRITE(uint64, ULE64)
PHYSFS_BYTEORDER_WRITE(sint64, SBE64)
PHYSFS_BYTEORDER_WRITE(uint64, UBE64)

/* end of physfs_byteorder.c ... */


/* #include "physfs_casefolding.h" */
/*
 * This file is part of PhysicsFS (https://icculus.org/physfs/)
 *
 * This data generated by physfs/extras/makecasefoldhashtable.pl ...
 * Do not manually edit this file!
 *
 * Please see the file LICENSE.txt in the source's root directory.
 */

#ifndef _INCLUDE_PHYSFS_CASEFOLDING_H_
#define _INCLUDE_PHYSFS_CASEFOLDING_H_

#ifndef __PHYSICSFS_INTERNAL__
#error Do not include this header from your applications.
#endif

/* We build three simple hashmaps here: one that maps Unicode codepoints to
a one, two, or three lowercase codepoints. To retrieve this info: look at
case_fold_hashX, where X is 1, 2, or 3. Most foldable codepoints fold to one,
a few dozen fold to two, and a handful fold to three. If the codepoint isn't
in any of these hashes, it doesn't fold (no separate upper and lowercase).

Almost all these codepoints fit into 16 bits, so we hash them as such to save
memory. If a codepoint is > 0xFFFF, we have separate hashes for them,
since there are (currently) only about 120 of them and (currently) all of them
map to a single lowercase codepoint. */

typedef struct CaseFoldMapping1_32
{
    PHYSFS_uint32 from;
    PHYSFS_uint32 to0;
} CaseFoldMapping1_32;

typedef struct CaseFoldMapping1_16
{
    PHYSFS_uint16 from;
    PHYSFS_uint16 to0;
} CaseFoldMapping1_16;

typedef struct CaseFoldMapping2_16
{
    PHYSFS_uint16 from;
    PHYSFS_uint16 to0;
    PHYSFS_uint16 to1;
} CaseFoldMapping2_16;

typedef struct CaseFoldMapping3_16
{
    PHYSFS_uint16 from;
    PHYSFS_uint16 to0;
    PHYSFS_uint16 to1;
    PHYSFS_uint16 to2;
} CaseFoldMapping3_16;

typedef struct CaseFoldHashBucket1_16
{
    const CaseFoldMapping1_16 *list;
    const PHYSFS_uint8 count;
} CaseFoldHashBucket1_16;

typedef struct CaseFoldHashBucket1_32
{
    const CaseFoldMapping1_32 *list;
    const PHYSFS_uint8 count;
} CaseFoldHashBucket1_32;

typedef struct CaseFoldHashBucket2_16
{
    const CaseFoldMapping2_16 *list;
    const PHYSFS_uint8 count;
} CaseFoldHashBucket2_16;

typedef struct CaseFoldHashBucket3_16
{
    const CaseFoldMapping3_16 *list;
    const PHYSFS_uint8 count;
} CaseFoldHashBucket3_16;

static const CaseFoldMapping1_16 case_fold1_16_000[] = {
    { 0x0202, 0x0203 },
    { 0x0404, 0x0454 },
    { 0x1E1E, 0x1E1F },
    { 0x2C2C, 0x2C5C },
    { 0xABAB, 0x13DB }
};

static const CaseFoldMapping1_16 case_fold1_16_001[] = {
    { 0x0100, 0x0101 },
    { 0x0405, 0x0455 },
    { 0x0504, 0x0505 },
    { 0x2C2D, 0x2C5D },
    { 0xA7A6, 0xA7A7 },
    { 0xABAA, 0x13DA }
};

static const CaseFoldMapping1_16 case_fold1_16_002[] = {
    { 0x0200, 0x0201 },
    { 0x0406, 0x0456 },
    { 0x1E1C, 0x1E1D },
    { 0x1F1D, 0x1F15 },
    { 0x2C2E, 0x2C5E },
    { 0xABA9, 0x13D9 }
};

static const CaseFoldMapping1_16 case_fold1_16_003[] = {
    { 0x0102, 0x0103 },
    { 0x0407, 0x0457 },
    { 0x0506, 0x0507 },
    { 0x1F1C, 0x1F14 },
    { 0xA7A4, 0xA7A5 },
    { 0xABA8, 0x13D8 }
};

static const CaseFoldMapping1_16 case_fold1_16_004[] = {
    { 0x0206, 0x0207 },
    { 0x0400, 0x0450 },
    { 0x1E1A, 0x1E1B },
    { 0x1F1B, 0x1F13 },
    { 0x2C28, 0x2C58 },
    { 0xABAF, 0x13DF }
};

static const CaseFoldMapping1_16 case_fold1_16_005[] = {
    { 0x0104, 0x0105 },
    { 0x0401, 0x0451 },
    { 0x0500, 0x0501 },
    { 0x1F1A, 0x1F12 },
    { 0x2C29, 0x2C59 },
    { 0xA7A2, 0xA7A3 },
    { 0xABAE, 0x13DE }
};

static const CaseFoldMapping1_16 case_fold1_16_006[] = {
    { 0x0204, 0x0205 },
    { 0x0402, 0x0452 },
    { 0x1E18, 0x1E19 },
    { 0x1F19, 0x1F11 },
    { 0x2C2A, 0x2C5A },
    { 0xABAD, 0x13DD }
};

static const CaseFoldMapping1_16 case_fold1_16_007[] = {
    { 0x0106, 0x0107 },
    { 0x0403, 0x0453 },
    { 0x0502, 0x0503 },
    { 0x1F18, 0x1F10 },
    { 0x2126, 0x03C9 },
    { 0x2C2B, 0x2C5B },
    { 0xA7A0, 0xA7A1 },
    { 0xABAC, 0x13DC }
};

static const CaseFoldMapping1_16 case_fold1_16_008[] = {
    { 0x020A, 0x020B },
    { 0x040C, 0x045C },
    { 0x1E16, 0x1E17 },
    { 0x2C24, 0x2C54 },
    { 0xABA3, 0x13D3 }
};

static const CaseFoldMapping1_16 case_fold1_16_009[] = {
    { 0x0108, 0x0109 },
    { 0x040D, 0x045D },
    { 0x050C, 0x050D },
    { 0x2C25, 0x2C55 },
    { 0xABA2, 0x13D2 }
};

static const CaseFoldMapping1_16 case_fold1_16_010[] = {
    { 0x0208, 0x0209 },
    { 0x040E, 0x045E },
    { 0x1E14, 0x1E15 },
    { 0x212B, 0x00E5 },
    { 0x2C26, 0x2C56 },
    { 0xA7AD, 0x026C },
    { 0xABA1, 0x13D1 }
};

static const CaseFoldMapping1_16 case_fold1_16_011[] = {
    { 0x010A, 0x010B },
    { 0x040F, 0x045F },
    { 0x050E, 0x050F },
    { 0x212A, 0x006B },
    { 0x2C27, 0x2C57 },
    { 0xA7AC, 0x0261 },
    { 0xABA0, 0x13D0 }
};

static const CaseFoldMapping1_16 case_fold1_16_012[] = {
    { 0x020E, 0x020F },
    { 0x0408, 0x0458 },
    { 0x1E12, 0x1E13 },
    { 0x2C20, 0x2C50 },
    { 0xA7AB, 0x025C },
    { 0xABA7, 0x13D7 }
};

static const CaseFoldMapping1_16 case_fold1_16_013[] = {
    { 0x010C, 0x010D },
    { 0x0409, 0x0459 },
    { 0x0508, 0x0509 },
    { 0x2C21, 0x2C51 },
    { 0xA7AA, 0x0266 },
    { 0xABA6, 0x13D6 }
};

static const CaseFoldMapping1_16 case_fold1_16_014[] = {
    { 0x020C, 0x020D },
    { 0x040A, 0x045A },
    { 0x1E10, 0x1E11 },
    { 0x2C22, 0x2C52 },
    { 0xABA5, 0x13D5 }
};

static const CaseFoldMapping1_16 case_fold1_16_015[] = {
    { 0x010E, 0x010F },
    { 0x040B, 0x045B },
    { 0x050A, 0x050B },
    { 0x2C23, 0x2C53 },
    { 0xA7A8, 0xA7A9 },
    { 0xABA4, 0x13D4 }
};

static const CaseFoldMapping1_16 case_fold1_16_016[] = {
    { 0x0212, 0x0213 },
    { 0x0414, 0x0434 },
    { 0x1E0E, 0x1E0F },
    { 0x1F0F, 0x1F07 },
    { 0xABBB, 0x13EB }
};

static const CaseFoldMapping1_16 case_fold1_16_017[] = {
    { 0x0110, 0x0111 },
    { 0x0415, 0x0435 },
    { 0x0514, 0x0515 },
    { 0x1F0E, 0x1F06 },
    { 0xA7B6, 0xA7B7 },
    { 0xABBA, 0x13EA }
};

static const CaseFoldMapping1_16 case_fold1_16_018[] = {
    { 0x0210, 0x0211 },
    { 0x0416, 0x0436 },
    { 0x1E0C, 0x1E0D },
    { 0x1F0D, 0x1F05 },
    { 0xABB9, 0x13E9 }
};

static const CaseFoldMapping1_16 case_fold1_16_019[] = {
    { 0x0112, 0x0113 },
    { 0x0417, 0x0437 },
    { 0x0516, 0x0517 },
    { 0x1F0C, 0x1F04 },
    { 0x2132, 0x214E },
    { 0xA7B4, 0xA7B5 },
    { 0xABB8, 0x13E8 }
};

static const CaseFoldMapping1_16 case_fold1_16_020[] = {
    { 0x0216, 0x0217 },
    { 0x0410, 0x0430 },
    { 0x1E0A, 0x1E0B },
    { 0x1F0B, 0x1F03 },
    { 0xA7B3, 0xAB53 },
    { 0xABBF, 0x13EF }
};

static const CaseFoldMapping1_16 case_fold1_16_021[] = {
    { 0x0114, 0x0115 },
    { 0x0411, 0x0431 },
    { 0x0510, 0x0511 },
    { 0x1F0A, 0x1F02 },
    { 0xA7B2, 0x029D },
    { 0xABBE, 0x13EE }
};

static const CaseFoldMapping1_16 case_fold1_16_022[] = {
    { 0x0214, 0x0215 },
    { 0x0412, 0x0432 },
    { 0x1E08, 0x1E09 },
    { 0x1F09, 0x1F01 },
    { 0xA7B1, 0x0287 },
    { 0xABBD, 0x13ED }
};

static const CaseFoldMapping1_16 case_fold1_16_023[] = {
    { 0x0116, 0x0117 },
    { 0x0413, 0x0433 },
    { 0x0512, 0x0513 },
    { 0x1F08, 0x1F00 },
    { 0xA7B0, 0x029E },
    { 0xABBC, 0x13EC }
};

static const CaseFoldMapping1_16 case_fold1_16_024[] = {
    { 0x021A, 0x021B },
    { 0x041C, 0x043C },
    { 0x1E06, 0x1E07 },
    { 0xABB3, 0x13E3 }
};

static const CaseFoldMapping1_16 case_fold1_16_025[] = {
    { 0x0118, 0x0119 },
    { 0x041D, 0x043D },
    { 0x051C, 0x051D },
    { 0xABB2, 0x13E2 }
};

static const CaseFoldMapping1_16 case_fold1_16_026[] = {
    { 0x0218, 0x0219 },
    { 0x041E, 0x043E },
    { 0x1E04, 0x1E05 },
    { 0xABB1, 0x13E1 }
};

static const CaseFoldMapping1_16 case_fold1_16_027[] = {
    { 0x011A, 0x011B },
    { 0x041F, 0x043F },
    { 0x051E, 0x051F },
    { 0xABB0, 0x13E0 }
};

static const CaseFoldMapping1_16 case_fold1_16_028[] = {
    { 0x021E, 0x021F },
    { 0x0418, 0x0438 },
    { 0x1E02, 0x1E03 },
    { 0xABB7, 0x13E7 }
};

static const CaseFoldMapping1_16 case_fold1_16_029[] = {
    { 0x011C, 0x011D },
    { 0x0419, 0x0439 },
    { 0x0518, 0x0519 },
    { 0xABB6, 0x13E6 }
};

static const CaseFoldMapping1_16 case_fold1_16_030[] = {
    { 0x021C, 0x021D },
    { 0x041A, 0x043A },
    { 0x1E00, 0x1E01 },
    { 0xABB5, 0x13E5 }
};

static const CaseFoldMapping1_16 case_fold1_16_031[] = {
    { 0x011E, 0x011F },
    { 0x041B, 0x043B },
    { 0x051A, 0x051B },
    { 0xABB4, 0x13E4 }
};

static const CaseFoldMapping1_16 case_fold1_16_032[] = {
    { 0x0222, 0x0223 },
    { 0x0424, 0x0444 },
    { 0x1E3E, 0x1E3F },
    { 0x1F3F, 0x1F37 },
    { 0x2C0C, 0x2C3C },
    { 0xA686, 0xA687 },
    { 0xAB8B, 0x13BB }
};

static const CaseFoldMapping1_16 case_fold1_16_033[] = {
    { 0x0120, 0x0121 },
    { 0x0425, 0x0445 },
    { 0x0524, 0x0525 },
    { 0x1F3E, 0x1F36 },
    { 0x2C0D, 0x2C3D },
    { 0xA786, 0xA787 },
    { 0xAB8A, 0x13BA }
};

static const CaseFoldMapping1_16 case_fold1_16_034[] = {
    { 0x0220, 0x019E },
    { 0x0426, 0x0446 },
    { 0x1E3C, 0x1E3D },
    { 0x1F3D, 0x1F35 },
    { 0x2C0E, 0x2C3E },
    { 0xA684, 0xA685 },
    { 0xAB89, 0x13B9 }
};

static const CaseFoldMapping1_16 case_fold1_16_035[] = {
    { 0x0122, 0x0123 },
    { 0x0427, 0x0447 },
    { 0x0526, 0x0527 },
    { 0x1F3C, 0x1F34 },
    { 0x2C0F, 0x2C3F },
    { 0xA784, 0xA785 },
    { 0xAB88, 0x13B8 }
};

static const CaseFoldMapping1_16 case_fold1_16_036[] = {
    { 0x0226, 0x0227 },
    { 0x0420, 0x0440 },
    { 0x1E3A, 0x1E3B },
    { 0x1F3B, 0x1F33 },
    { 0x2C08, 0x2C38 },
    { 0xA682, 0xA683 },
    { 0xAB8F, 0x13BF }
};

static const CaseFoldMapping1_16 case_fold1_16_037[] = {
    { 0x0124, 0x0125 },
    { 0x0421, 0x0441 },
    { 0x0520, 0x0521 },
    { 0x1F3A, 0x1F32 },
    { 0x2C09, 0x2C39 },
    { 0xA782, 0xA783 },
    { 0xAB8E, 0x13BE }
};

static const CaseFoldMapping1_16 case_fold1_16_038[] = {
    { 0x0224, 0x0225 },
    { 0x0422, 0x0442 },
    { 0x1E38, 0x1E39 },
    { 0x1F39, 0x1F31 },
    { 0x2C0A, 0x2C3A },
    { 0xA680, 0xA681 },
    { 0xAB8D, 0x13BD }
};

static const CaseFoldMapping1_16 case_fold1_16_039[] = {
    { 0x0126, 0x0127 },
    { 0x0423, 0x0443 },
    { 0x0522, 0x0523 },
    { 0x1F38, 0x1F30 },
    { 0x2C0B, 0x2C3B },
    { 0xA780, 0xA781 },
    { 0xAB8C, 0x13BC }
};

static const CaseFoldMapping1_16 case_fold1_16_040[] = {
    { 0x022A, 0x022B },
    { 0x042C, 0x044C },
    { 0x1E36, 0x1E37 },
    { 0x2C04, 0x2C34 },
    { 0xA68E, 0xA68F },
    { 0xAB83, 0x13B3 }
};

static const CaseFoldMapping1_16 case_fold1_16_041[] = {
    { 0x0128, 0x0129 },
    { 0x042D, 0x044D },
    { 0x052C, 0x052D },
    { 0x2C05, 0x2C35 },
    { 0xAB82, 0x13B2 }
};

static const CaseFoldMapping1_16 case_fold1_16_042[] = {
    { 0x0228, 0x0229 },
    { 0x042E, 0x044E },
    { 0x1E34, 0x1E35 },
    { 0x2C06, 0x2C36 },
    { 0xA68C, 0xA68D },
    { 0xA78D, 0x0265 },
    { 0xAB81, 0x13B1 }
};

static const CaseFoldMapping1_16 case_fold1_16_043[] = {
    { 0x012A, 0x012B },
    { 0x042F, 0x044F },
    { 0x052E, 0x052F },
    { 0x2C07, 0x2C37 },
    { 0xAB80, 0x13B0 }
};

static const CaseFoldMapping1_16 case_fold1_16_044[] = {
    { 0x022E, 0x022F },
    { 0x0428, 0x0448 },
    { 0x1E32, 0x1E33 },
    { 0x2C00, 0x2C30 },
    { 0xA68A, 0xA68B },
    { 0xA78B, 0xA78C },
    { 0xAB87, 0x13B7 }
};

static const CaseFoldMapping1_16 case_fold1_16_045[] = {
    { 0x012C, 0x012D },
    { 0x0429, 0x0449 },
    { 0x0528, 0x0529 },
    { 0x2C01, 0x2C31 },
    { 0xAB86, 0x13B6 }
};

static const CaseFoldMapping1_16 case_fold1_16_046[] = {
    { 0x022C, 0x022D },
    { 0x042A, 0x044A },
    { 0x1E30, 0x1E31 },
    { 0x2C02, 0x2C32 },
    { 0xA688, 0xA689 },
    { 0xAB85, 0x13B5 }
};

static const CaseFoldMapping1_16 case_fold1_16_047[] = {
    { 0x012E, 0x012F },
    { 0x042B, 0x044B },
    { 0x052A, 0x052B },
    { 0x2C03, 0x2C33 },
    { 0xAB84, 0x13B4 }
};

static const CaseFoldMapping1_16 case_fold1_16_048[] = {
    { 0x0232, 0x0233 },
    { 0x0535, 0x0565 },
    { 0x1E2E, 0x1E2F },
    { 0x1F2F, 0x1F27 },
    { 0x2C1C, 0x2C4C },
    { 0xA696, 0xA697 },
    { 0xAB9B, 0x13CB }
};

static const CaseFoldMapping1_16 case_fold1_16_049[] = {
    { 0x0534, 0x0564 },
    { 0x1F2E, 0x1F26 },
    { 0x2C1D, 0x2C4D },
    { 0xA796, 0xA797 },
    { 0xAB9A, 0x13CA }
};

static const CaseFoldMapping1_16 case_fold1_16_050[] = {
    { 0x0230, 0x0231 },
    { 0x0537, 0x0567 },
    { 0x1E2C, 0x1E2D },
    { 0x1F2D, 0x1F25 },
    { 0x2C1E, 0x2C4E },
    { 0xA694, 0xA695 },
    { 0xAB99, 0x13C9 }
};

static const CaseFoldMapping1_16 case_fold1_16_051[] = {
    { 0x0132, 0x0133 },
    { 0x0536, 0x0566 },
    { 0x1F2C, 0x1F24 },
    { 0x2C1F, 0x2C4F },
    { 0xAB98, 0x13C8 }
};

static const CaseFoldMapping1_16 case_fold1_16_052[] = {
    { 0x0531, 0x0561 },
    { 0x1E2A, 0x1E2B },
    { 0x1F2B, 0x1F23 },
    { 0x2C18, 0x2C48 },
    { 0xA692, 0xA693 },
    { 0xAB9F, 0x13CF }
};

static const CaseFoldMapping1_16 case_fold1_16_053[] = {
    { 0x0134, 0x0135 },
    { 0x1F2A, 0x1F22 },
    { 0x2C19, 0x2C49 },
    { 0xA792, 0xA793 },
    { 0xAB9E, 0x13CE }
};

static const CaseFoldMapping1_16 case_fold1_16_054[] = {
    { 0x0533, 0x0563 },
    { 0x1E28, 0x1E29 },
    { 0x1F29, 0x1F21 },
    { 0x2C1A, 0x2C4A },
    { 0xA690, 0xA691 },
    { 0xAB9D, 0x13CD }
};

static const CaseFoldMapping1_16 case_fold1_16_055[] = {
    { 0x0136, 0x0137 },
    { 0x0532, 0x0562 },
    { 0x1F28, 0x1F20 },
    { 0x2C1B, 0x2C4B },
    { 0xA790, 0xA791 },
    { 0xAB9C, 0x13CC }
};

static const CaseFoldMapping1_16 case_fold1_16_056[] = {
    { 0x0139, 0x013A },
    { 0x023A, 0x2C65 },
    { 0x053D, 0x056D },
    { 0x1E26, 0x1E27 },
    { 0x2C14, 0x2C44 },
    { 0xAB93, 0x13C3 }
};

static const CaseFoldMapping1_16 case_fold1_16_057[] = {
    { 0x023B, 0x023C },
    { 0x053C, 0x056C },
    { 0x2C15, 0x2C45 },
    { 0xA79E, 0xA79F },
    { 0xAB92, 0x13C2 }
};

static const CaseFoldMapping1_16 case_fold1_16_058[] = {
    { 0x013B, 0x013C },
    { 0x053F, 0x056F },
    { 0x1E24, 0x1E25 },
    { 0x2C16, 0x2C46 },
    { 0xAB91, 0x13C1 }
};

static const CaseFoldMapping1_16 case_fold1_16_059[] = {
    { 0x053E, 0x056E },
    { 0x2C17, 0x2C47 },
    { 0xA79C, 0xA79D },
    { 0xAB90, 0x13C0 }
};

static const CaseFoldMapping1_16 case_fold1_16_060[] = {
    { 0x013D, 0x013E },
    { 0x023E, 0x2C66 },
    { 0x0539, 0x0569 },
    { 0x1E22, 0x1E23 },
    { 0x2C10, 0x2C40 },
    { 0xA69A, 0xA69B },
    { 0xAB97, 0x13C7 }
};

static const CaseFoldMapping1_16 case_fold1_16_061[] = {
    { 0x0538, 0x0568 },
    { 0x2C11, 0x2C41 },
    { 0xA79A, 0xA79B },
    { 0xAB96, 0x13C6 }
};

static const CaseFoldMapping1_16 case_fold1_16_062[] = {
    { 0x013F, 0x0140 },
    { 0x053B, 0x056B },
    { 0x1E20, 0x1E21 },
    { 0x2C12, 0x2C42 },
    { 0xA698, 0xA699 },
    { 0xAB95, 0x13C5 }
};

static const CaseFoldMapping1_16 case_fold1_16_063[] = {
    { 0x023D, 0x019A },
    { 0x053A, 0x056A },
    { 0x2C13, 0x2C43 },
    { 0xA798, 0xA799 },
    { 0xAB94, 0x13C4 }
};

static const CaseFoldMapping1_16 case_fold1_16_064[] = {
    { 0x0141, 0x0142 },
    { 0x0545, 0x0575 },
    { 0x1E5E, 0x1E5F },
    { 0x1F5F, 0x1F57 },
    { 0x2161, 0x2171 }
};

static const CaseFoldMapping1_16 case_fold1_16_065[] = {
    { 0x0041, 0x0061 },
    { 0x0243, 0x0180 },
    { 0x0544, 0x0574 },
    { 0x2160, 0x2170 },
    { 0x2C6D, 0x0251 }
};

static const CaseFoldMapping1_16 case_fold1_16_066[] = {
    { 0x0042, 0x0062 },
    { 0x0143, 0x0144 },
    { 0x0547, 0x0577 },
    { 0x1E5C, 0x1E5D },
    { 0x1F5D, 0x1F55 },
    { 0x2163, 0x2173 },
    { 0x2C6E, 0x0271 }
};

static const CaseFoldMapping1_16 case_fold1_16_067[] = {
    { 0x0043, 0x0063 },
    { 0x0241, 0x0242 },
    { 0x0546, 0x0576 },
    { 0x2162, 0x2172 },
    { 0x2C6F, 0x0250 }
};

static const CaseFoldMapping1_16 case_fold1_16_068[] = {
    { 0x0044, 0x0064 },
    { 0x0145, 0x0146 },
    { 0x0246, 0x0247 },
    { 0x0541, 0x0571 },
    { 0x1E5A, 0x1E5B },
    { 0x1F5B, 0x1F53 },
    { 0x2165, 0x2175 }
};

static const CaseFoldMapping1_16 case_fold1_16_069[] = {
    { 0x0045, 0x0065 },
    { 0x0540, 0x0570 },
    { 0x2164, 0x2174 },
    { 0x2C69, 0x2C6A }
};

static const CaseFoldMapping1_16 case_fold1_16_070[] = {
    { 0x0046, 0x0066 },
    { 0x0147, 0x0148 },
    { 0x0244, 0x0289 },
    { 0x0345, 0x03B9 },
    { 0x0543, 0x0573 },
    { 0x1E58, 0x1E59 },
    { 0x1F59, 0x1F51 },
    { 0x2167, 0x2177 }
};

static const CaseFoldMapping1_16 case_fold1_16_071[] = {
    { 0x0047, 0x0067 },
    { 0x0245, 0x028C },
    { 0x0542, 0x0572 },
    { 0x2166, 0x2176 },
    { 0x2C6B, 0x2C6C }
};

static const CaseFoldMapping1_16 case_fold1_16_072[] = {
    { 0x0048, 0x0068 },
    { 0x024A, 0x024B },
    { 0x054D, 0x057D },
    { 0x1E56, 0x1E57 },
    { 0x2169, 0x2179 },
    { 0x2C64, 0x027D }
};

static const CaseFoldMapping1_16 case_fold1_16_073[] = {
    { 0x0049, 0x0069 },
    { 0x054C, 0x057C },
    { 0x2168, 0x2178 }
};

static const CaseFoldMapping1_16 case_fold1_16_074[] = {
    { 0x004A, 0x006A },
    { 0x0248, 0x0249 },
    { 0x054F, 0x057F },
    { 0x1E54, 0x1E55 },
    { 0x216B, 0x217B }
};

static const CaseFoldMapping1_16 case_fold1_16_075[] = {
    { 0x004B, 0x006B },
    { 0x014A, 0x014B },
    { 0x054E, 0x057E },
    { 0x216A, 0x217A },
    { 0x2C67, 0x2C68 }
};

static const CaseFoldMapping1_16 case_fold1_16_076[] = {
    { 0x004C, 0x006C },
    { 0x024E, 0x024F },
    { 0x0549, 0x0579 },
    { 0x1E52, 0x1E53 },
    { 0x216D, 0x217D },
    { 0x2C60, 0x2C61 }
};

static const CaseFoldMapping1_16 case_fold1_16_077[] = {
    { 0x004D, 0x006D },
    { 0x014C, 0x014D },
    { 0x0548, 0x0578 },
    { 0x216C, 0x217C }
};

static const CaseFoldMapping1_16 case_fold1_16_078[] = {
    { 0x004E, 0x006E },
    { 0x024C, 0x024D },
    { 0x054B, 0x057B },
    { 0x1E50, 0x1E51 },
    { 0x216F, 0x217F },
    { 0x2C62, 0x026B }
};

static const CaseFoldMapping1_16 case_fold1_16_079[] = {
    { 0x004F, 0x006F },
    { 0x014E, 0x014F },
    { 0x054A, 0x057A },
    { 0x216E, 0x217E },
    { 0x2C63, 0x1D7D }
};

static const CaseFoldMapping1_16 case_fold1_16_080[] = {
    { 0x0050, 0x0070 },
    { 0x0555, 0x0585 },
    { 0x1E4E, 0x1E4F }
};

static const CaseFoldMapping1_16 case_fold1_16_081[] = {
    { 0x0051, 0x0071 },
    { 0x0150, 0x0151 },
    { 0x0554, 0x0584 }
};

static const CaseFoldMapping1_16 case_fold1_16_082[] = {
    { 0x0052, 0x0072 },
    { 0x1E4C, 0x1E4D },
    { 0x1F4D, 0x1F45 },
    { 0x2C7E, 0x023F }
};

static const CaseFoldMapping1_16 case_fold1_16_083[] = {
    { 0x0053, 0x0073 },
    { 0x0152, 0x0153 },
    { 0x0556, 0x0586 },
    { 0x1F4C, 0x1F44 },
    { 0x2C7F, 0x0240 }
};

static const CaseFoldMapping1_16 case_fold1_16_084[] = {
    { 0x0054, 0x0074 },
    { 0x0551, 0x0581 },
    { 0x1E4A, 0x1E4B },
    { 0x1F4B, 0x1F43 }
};

static const CaseFoldMapping1_16 case_fold1_16_085[] = {
    { 0x0055, 0x0075 },
    { 0x0154, 0x0155 },
    { 0x0550, 0x0580 },
    { 0x1F4A, 0x1F42 }
};

static const CaseFoldMapping1_16 case_fold1_16_086[] = {
    { 0x0056, 0x0076 },
    { 0x0553, 0x0583 },
    { 0x1E48, 0x1E49 },
    { 0x1F49, 0x1F41 }
};

static const CaseFoldMapping1_16 case_fold1_16_087[] = {
    { 0x0057, 0x0077 },
    { 0x0156, 0x0157 },
    { 0x0552, 0x0582 },
    { 0x1F48, 0x1F40 }
};

static const CaseFoldMapping1_16 case_fold1_16_088[] = {
    { 0x0058, 0x0078 },
    { 0x1E46, 0x1E47 }
};

static const CaseFoldMapping1_16 case_fold1_16_089[] = {
    { 0x0059, 0x0079 },
    { 0x0158, 0x0159 },
    { 0x2C75, 0x2C76 }
};

static const CaseFoldMapping1_16 case_fold1_16_090[] = {
    { 0x005A, 0x007A },
    { 0x1E44, 0x1E45 }
};

static const CaseFoldMapping1_16 case_fold1_16_091[] = {
    { 0x015A, 0x015B }
};

static const CaseFoldMapping1_16 case_fold1_16_092[] = {
    { 0x1E42, 0x1E43 },
    { 0x2C70, 0x0252 }
};

static const CaseFoldMapping1_16 case_fold1_16_093[] = {
    { 0x015C, 0x015D }
};

static const CaseFoldMapping1_16 case_fold1_16_094[] = {
    { 0x1E40, 0x1E41 },
    { 0x2C72, 0x2C73 }
};

static const CaseFoldMapping1_16 case_fold1_16_095[] = {
    { 0x015E, 0x015F }
};

static const CaseFoldMapping1_16 case_fold1_16_096[] = {
    { 0x0464, 0x0465 },
    { 0x1E7E, 0x1E7F }
};

static const CaseFoldMapping1_16 case_fold1_16_097[] = {
    { 0x0160, 0x0161 }
};

static const CaseFoldMapping1_16 case_fold1_16_098[] = {
    { 0x0466, 0x0467 },
    { 0x1E7C, 0x1E7D }
};

static const CaseFoldMapping1_16 case_fold1_16_099[] = {
    { 0x0162, 0x0163 }
};

static const CaseFoldMapping1_16 case_fold1_16_100[] = {
    { 0x0460, 0x0461 },
    { 0x1E7A, 0x1E7B }
};

static const CaseFoldMapping1_16 case_fold1_16_101[] = {
    { 0x0164, 0x0165 }
};

static const CaseFoldMapping1_16 case_fold1_16_102[] = {
    { 0x0462, 0x0463 },
    { 0x1E78, 0x1E79 }
};

static const CaseFoldMapping1_16 case_fold1_16_103[] = {
    { 0x0166, 0x0167 }
};

static const CaseFoldMapping1_16 case_fold1_16_104[] = {
    { 0x046C, 0x046D },
    { 0x1E76, 0x1E77 }
};

static const CaseFoldMapping1_16 case_fold1_16_105[] = {
    { 0x0168, 0x0169 }
};

static const CaseFoldMapping1_16 case_fold1_16_106[] = {
    { 0x046E, 0x046F },
    { 0x1E74, 0x1E75 }
};

static const CaseFoldMapping1_16 case_fold1_16_107[] = {
    { 0x016A, 0x016B }
};

static const CaseFoldMapping1_16 case_fold1_16_108[] = {
    { 0x0468, 0x0469 },
    { 0x1E72, 0x1E73 }
};

static const CaseFoldMapping1_16 case_fold1_16_109[] = {
    { 0x016C, 0x016D }
};

static const CaseFoldMapping1_16 case_fold1_16_110[] = {
    { 0x046A, 0x046B },
    { 0x1E70, 0x1E71 }
};

static const CaseFoldMapping1_16 case_fold1_16_111[] = {
    { 0x016E, 0x016F }
};

static const CaseFoldMapping1_16 case_fold1_16_112[] = {
    { 0x0474, 0x0475 },
    { 0x1E6E, 0x1E6F },
    { 0x1F6F, 0x1F67 }
};

static const CaseFoldMapping1_16 case_fold1_16_113[] = {
    { 0x0170, 0x0171 },
    { 0x0372, 0x0373 },
    { 0x1F6E, 0x1F66 }
};

static const CaseFoldMapping1_16 case_fold1_16_114[] = {
    { 0x0476, 0x0477 },
    { 0x1E6C, 0x1E6D },
    { 0x1F6D, 0x1F65 }
};

static const CaseFoldMapping1_16 case_fold1_16_115[] = {
    { 0x0172, 0x0173 },
    { 0x0370, 0x0371 },
    { 0x1F6C, 0x1F64 }
};

static const CaseFoldMapping1_16 case_fold1_16_116[] = {
    { 0x0470, 0x0471 },
    { 0x1E6A, 0x1E6B },
    { 0x1F6B, 0x1F63 }
};

static const CaseFoldMapping1_16 case_fold1_16_117[] = {
    { 0x0174, 0x0175 },
    { 0x0376, 0x0377 },
    { 0x1F6A, 0x1F62 }
};

static const CaseFoldMapping1_16 case_fold1_16_118[] = {
    { 0x0472, 0x0473 },
    { 0x1E68, 0x1E69 },
    { 0x1F69, 0x1F61 }
};

static const CaseFoldMapping1_16 case_fold1_16_119[] = {
    { 0x0176, 0x0177 },
    { 0x1F68, 0x1F60 }
};

static const CaseFoldMapping1_16 case_fold1_16_120[] = {
    { 0x0179, 0x017A },
    { 0x047C, 0x047D },
    { 0x1E66, 0x1E67 }
};

static const CaseFoldMapping1_16 case_fold1_16_121[] = {
    { 0x0178, 0x00FF }
};

static const CaseFoldMapping1_16 case_fold1_16_122[] = {
    { 0x017B, 0x017C },
    { 0x047E, 0x047F },
    { 0x1E64, 0x1E65 }
};

static const CaseFoldMapping1_16 case_fold1_16_124[] = {
    { 0x017D, 0x017E },
    { 0x037F, 0x03F3 },
    { 0x0478, 0x0479 },
    { 0x1E62, 0x1E63 }
};

static const CaseFoldMapping1_16 case_fold1_16_126[] = {
    { 0x017F, 0x0073 },
    { 0x047A, 0x047B },
    { 0x1E60, 0x1E61 }
};

static const CaseFoldMapping1_16 case_fold1_16_128[] = {
    { 0x0181, 0x0253 },
    { 0x2CAC, 0x2CAD }
};

static const CaseFoldMapping1_16 case_fold1_16_129[] = {
    { 0xA726, 0xA727 }
};

static const CaseFoldMapping1_16 case_fold1_16_130[] = {
    { 0x2CAE, 0x2CAF }
};

static const CaseFoldMapping1_16 case_fold1_16_131[] = {
    { 0x0182, 0x0183 },
    { 0xA724, 0xA725 }
};

static const CaseFoldMapping1_16 case_fold1_16_132[] = {
    { 0x0480, 0x0481 },
    { 0x2CA8, 0x2CA9 }
};

static const CaseFoldMapping1_16 case_fold1_16_133[] = {
    { 0x0184, 0x0185 },
    { 0x0386, 0x03AC },
    { 0x1E9B, 0x1E61 },
    { 0xA722, 0xA723 }
};

static const CaseFoldMapping1_16 case_fold1_16_134[] = {
    { 0x0187, 0x0188 },
    { 0x2CAA, 0x2CAB }
};

static const CaseFoldMapping1_16 case_fold1_16_135[] = {
    { 0x0186, 0x0254 }
};

static const CaseFoldMapping1_16 case_fold1_16_136[] = {
    { 0x0189, 0x0256 },
    { 0x048C, 0x048D },
    { 0x2CA4, 0x2CA5 }
};

static const CaseFoldMapping1_16 case_fold1_16_137[] = {
    { 0x038A, 0x03AF },
    { 0xA72E, 0xA72F }
};

static const CaseFoldMapping1_16 case_fold1_16_138[] = {
    { 0x018B, 0x018C },
    { 0x0389, 0x03AE },
    { 0x048E, 0x048F },
    { 0x1E94, 0x1E95 },
    { 0x2CA6, 0x2CA7 }
};

static const CaseFoldMapping1_16 case_fold1_16_139[] = {
    { 0x018A, 0x0257 },
    { 0x0388, 0x03AD },
    { 0xA72C, 0xA72D }
};

static const CaseFoldMapping1_16 case_fold1_16_140[] = {
    { 0x038F, 0x03CE },
    { 0x1E92, 0x1E93 },
    { 0x2CA0, 0x2CA1 }
};

static const CaseFoldMapping1_16 case_fold1_16_141[] = {
    { 0x038E, 0x03CD },
    { 0xA72A, 0xA72B }
};

static const CaseFoldMapping1_16 case_fold1_16_142[] = {
    { 0x018F, 0x0259 },
    { 0x048A, 0x048B },
    { 0x1E90, 0x1E91 },
    { 0x2CA2, 0x2CA3 }
};

static const CaseFoldMapping1_16 case_fold1_16_143[] = {
    { 0x018E, 0x01DD },
    { 0x038C, 0x03CC },
    { 0xA728, 0xA729 }
};

static const CaseFoldMapping1_16 case_fold1_16_144[] = {
    { 0x0191, 0x0192 },
    { 0x0393, 0x03B3 },
    { 0x0494, 0x0495 },
    { 0x1E8E, 0x1E8F },
    { 0x2CBC, 0x2CBD }
};

static const CaseFoldMapping1_16 case_fold1_16_145[] = {
    { 0x0190, 0x025B },
    { 0x0392, 0x03B2 },
    { 0xA736, 0xA737 }
};

static const CaseFoldMapping1_16 case_fold1_16_146[] = {
    { 0x0193, 0x0260 },
    { 0x0391, 0x03B1 },
    { 0x0496, 0x0497 },
    { 0x1E8C, 0x1E8D },
    { 0x24B6, 0x24D0 },
    { 0x2CBE, 0x2CBF }
};

static const CaseFoldMapping1_16 case_fold1_16_147[] = {
    { 0x24B7, 0x24D1 },
    { 0xA734, 0xA735 }
};

static const CaseFoldMapping1_16 case_fold1_16_148[] = {
    { 0x0397, 0x03B7 },
    { 0x0490, 0x0491 },
    { 0x1E8A, 0x1E8B },
    { 0x2CB8, 0x2CB9 }
};

static const CaseFoldMapping1_16 case_fold1_16_149[] = {
    { 0x0194, 0x0263 },
    { 0x0396, 0x03B6 },
    { 0xA732, 0xA733 }
};

static const CaseFoldMapping1_16 case_fold1_16_150[] = {
    { 0x0197, 0x0268 },
    { 0x0395, 0x03B5 },
    { 0x0492, 0x0493 },
    { 0x1E88, 0x1E89 },
    { 0x2CBA, 0x2CBB }
};

static const CaseFoldMapping1_16 case_fold1_16_151[] = {
    { 0x0196, 0x0269 },
    { 0x0394, 0x03B4 }
};

static const CaseFoldMapping1_16 case_fold1_16_152[] = {
    { 0x039B, 0x03BB },
    { 0x049C, 0x049D },
    { 0x1E86, 0x1E87 },
    { 0x24BC, 0x24D6 },
    { 0x2CB4, 0x2CB5 }
};

static const CaseFoldMapping1_16 case_fold1_16_153[] = {
    { 0x0198, 0x0199 },
    { 0x039A, 0x03BA },
    { 0x24BD, 0x24D7 },
    { 0xA73E, 0xA73F }
};

static const CaseFoldMapping1_16 case_fold1_16_154[] = {
    { 0x0399, 0x03B9 },
    { 0x049E, 0x049F },
    { 0x1E84, 0x1E85 },
    { 0x24BE, 0x24D8 },
    { 0x2CB6, 0x2CB7 }
};

static const CaseFoldMapping1_16 case_fold1_16_155[] = {
    { 0x0398, 0x03B8 },
    { 0x24BF, 0x24D9 },
    { 0xA73C, 0xA73D }
};

static const CaseFoldMapping1_16 case_fold1_16_156[] = {
    { 0x019D, 0x0272 },
    { 0x039F, 0x03BF },
    { 0x0498, 0x0499 },
    { 0x1E82, 0x1E83 },
    { 0x24B8, 0x24D2 },
    { 0x2CB0, 0x2CB1 }
};

static const CaseFoldMapping1_16 case_fold1_16_157[] = {
    { 0x019C, 0x026F },
    { 0x039E, 0x03BE },
    { 0x24B9, 0x24D3 },
    { 0xA73A, 0xA73B }
};

static const CaseFoldMapping1_16 case_fold1_16_158[] = {
    { 0x019F, 0x0275 },
    { 0x039D, 0x03BD },
    { 0x049A, 0x049B },
    { 0x1E80, 0x1E81 },
    { 0x24BA, 0x24D4 },
    { 0x2CB2, 0x2CB3 }
};

static const CaseFoldMapping1_16 case_fold1_16_159[] = {
    { 0x039C, 0x03BC },
    { 0x24BB, 0x24D5 },
    { 0xA738, 0xA739 }
};

static const CaseFoldMapping1_16 case_fold1_16_160[] = {
    { 0x03A3, 0x03C3 },
    { 0x04A4, 0x04A5 },
    { 0x10B0, 0x2D10 },
    { 0x1EBE, 0x1EBF },
    { 0x2C8C, 0x2C8D }
};

static const CaseFoldMapping1_16 case_fold1_16_161[] = {
    { 0x01A0, 0x01A1 },
    { 0x10B1, 0x2D11 },
    { 0x1FBE, 0x03B9 }
};

static const CaseFoldMapping1_16 case_fold1_16_162[] = {
    { 0x03A1, 0x03C1 },
    { 0x04A6, 0x04A7 },
    { 0x10B2, 0x2D12 },
    { 0x1EBC, 0x1EBD },
    { 0x2183, 0x2184 },
    { 0x2C8E, 0x2C8F }
};

static const CaseFoldMapping1_16 case_fold1_16_163[] = {
    { 0x01A2, 0x01A3 },
    { 0x03A0, 0x03C0 },
    { 0x10B3, 0x2D13 }
};

static const CaseFoldMapping1_16 case_fold1_16_164[] = {
    { 0x03A7, 0x03C7 },
    { 0x04A0, 0x04A1 },
    { 0x10B4, 0x2D14 },
    { 0x1EBA, 0x1EBB },
    { 0x1FBB, 0x1F71 },
    { 0x2C88, 0x2C89 }
};

static const CaseFoldMapping1_16 case_fold1_16_165[] = {
    { 0x01A4, 0x01A5 },
    { 0x03A6, 0x03C6 },
    { 0x10B5, 0x2D15 },
    { 0x1FBA, 0x1F70 }
};

static const CaseFoldMapping1_16 case_fold1_16_166[] = {
    { 0x01A7, 0x01A8 },
    { 0x03A5, 0x03C5 },
    { 0x04A2, 0x04A3 },
    { 0x10B6, 0x2D16 },
    { 0x1EB8, 0x1EB9 },
    { 0x1FB9, 0x1FB1 },
    { 0x2C8A, 0x2C8B }
};

static const CaseFoldMapping1_16 case_fold1_16_167[] = {
    { 0x01A6, 0x0280 },
    { 0x03A4, 0x03C4 },
    { 0x10B7, 0x2D17 },
    { 0x1FB8, 0x1FB0 }
};

static const CaseFoldMapping1_16 case_fold1_16_168[] = {
    { 0x01A9, 0x0283 },
    { 0x03AB, 0x03CB },
    { 0x04AC, 0x04AD },
    { 0x10B8, 0x2D18 },
    { 0x1EB6, 0x1EB7 },
    { 0x2C84, 0x2C85 }
};

static const CaseFoldMapping1_16 case_fold1_16_169[] = {
    { 0x03AA, 0x03CA },
    { 0x10B9, 0x2D19 }
};

static const CaseFoldMapping1_16 case_fold1_16_170[] = {
    { 0x03A9, 0x03C9 },
    { 0x04AE, 0x04AF },
    { 0x10BA, 0x2D1A },
    { 0x1EB4, 0x1EB5 },
    { 0x2C86, 0x2C87 }
};

static const CaseFoldMapping1_16 case_fold1_16_171[] = {
    { 0x03A8, 0x03C8 },
    { 0x10BB, 0x2D1B }
};

static const CaseFoldMapping1_16 case_fold1_16_172[] = {
    { 0x04A8, 0x04A9 },
    { 0x10BC, 0x2D1C },
    { 0x1EB2, 0x1EB3 },
    { 0x2C80, 0x2C81 }
};

static const CaseFoldMapping1_16 case_fold1_16_173[] = {
    { 0x01AC, 0x01AD },
    { 0x10BD, 0x2D1D }
};

static const CaseFoldMapping1_16 case_fold1_16_174[] = {
    { 0x01AF, 0x01B0 },
    { 0x04AA, 0x04AB },
    { 0x10BE, 0x2D1E },
    { 0x1EB0, 0x1EB1 },
    { 0x2C82, 0x2C83 }
};

static const CaseFoldMapping1_16 case_fold1_16_175[] = {
    { 0x01AE, 0x0288 },
    { 0x10BF, 0x2D1F }
};

static const CaseFoldMapping1_16 case_fold1_16_176[] = {
    { 0x01B1, 0x028A },
    { 0x04B4, 0x04B5 },
    { 0x10A0, 0x2D00 },
    { 0x1EAE, 0x1EAF },
    { 0x2C9C, 0x2C9D }
};

static const CaseFoldMapping1_16 case_fold1_16_177[] = {
    { 0x10A1, 0x2D01 }
};

static const CaseFoldMapping1_16 case_fold1_16_178[] = {
    { 0x01B3, 0x01B4 },
    { 0x04B6, 0x04B7 },
    { 0x10A2, 0x2D02 },
    { 0x1EAC, 0x1EAD },
    { 0x2C9E, 0x2C9F }
};

static const CaseFoldMapping1_16 case_fold1_16_179[] = {
    { 0x01B2, 0x028B },
    { 0x10A3, 0x2D03 }
};

static const CaseFoldMapping1_16 case_fold1_16_180[] = {
    { 0x01B5, 0x01B6 },
    { 0x04B0, 0x04B1 },
    { 0x10A4, 0x2D04 },
    { 0x1EAA, 0x1EAB },
    { 0x2C98, 0x2C99 }
};

static const CaseFoldMapping1_16 case_fold1_16_181[] = {
    { 0x00B5, 0x03BC },
    { 0x10A5, 0x2D05 }
};

static const CaseFoldMapping1_16 case_fold1_16_182[] = {
    { 0x01B7, 0x0292 },
    { 0x04B2, 0x04B3 },
    { 0x10A6, 0x2D06 },
    { 0x1EA8, 0x1EA9 },
    { 0x2C9A, 0x2C9B }
};

static const CaseFoldMapping1_16 case_fold1_16_183[] = {
    { 0x10A7, 0x2D07 }
};

static const CaseFoldMapping1_16 case_fold1_16_184[] = {
    { 0x04BC, 0x04BD },
    { 0x10A8, 0x2D08 },
    { 0x1EA6, 0x1EA7 },
    { 0x2C94, 0x2C95 }
};

static const CaseFoldMapping1_16 case_fold1_16_185[] = {
    { 0x01B8, 0x01B9 },
    { 0x10A9, 0x2D09 }
};

static const CaseFoldMapping1_16 case_fold1_16_186[] = {
    { 0x04BE, 0x04BF },
    { 0x10AA, 0x2D0A },
    { 0x1EA4, 0x1EA5 },
    { 0x2C96, 0x2C97 }
};

static const CaseFoldMapping1_16 case_fold1_16_187[] = {
    { 0x10AB, 0x2D0B }
};

static const CaseFoldMapping1_16 case_fold1_16_188[] = {
    { 0x04B8, 0x04B9 },
    { 0x10AC, 0x2D0C },
    { 0x1EA2, 0x1EA3 },
    { 0x2C90, 0x2C91 }
};

static const CaseFoldMapping1_16 case_fold1_16_189[] = {
    { 0x01BC, 0x01BD },
    { 0x10AD, 0x2D0D }
};

static const CaseFoldMapping1_16 case_fold1_16_190[] = {
    { 0x04BA, 0x04BB },
    { 0x10AE, 0x2D0E },
    { 0x1EA0, 0x1EA1 },
    { 0x2C92, 0x2C93 }
};

static const CaseFoldMapping1_16 case_fold1_16_191[] = {
    { 0x10AF, 0x2D0F }
};

static const CaseFoldMapping1_16 case_fold1_16_192[] = {
    { 0x00C0, 0x00E0 },
    { 0x1EDE, 0x1EDF },
    { 0xA666, 0xA667 }
};

static const CaseFoldMapping1_16 case_fold1_16_193[] = {
    { 0x00C1, 0x00E1 },
    { 0x03C2, 0x03C3 },
    { 0x04C5, 0x04C6 },
    { 0x2CED, 0x2CEE },
    { 0xA766, 0xA767 }
};

static const CaseFoldMapping1_16 case_fold1_16_194[] = {
    { 0x00C2, 0x00E2 },
    { 0x1EDC, 0x1EDD },
    { 0xA664, 0xA665 }
};

static const CaseFoldMapping1_16 case_fold1_16_195[] = {
    { 0x00C3, 0x00E3 },
    { 0x04C7, 0x04C8 },
    { 0xA764, 0xA765 }
};

static const CaseFoldMapping1_16 case_fold1_16_196[] = {
    { 0x00C4, 0x00E4 },
    { 0x01C5, 0x01C6 },
    { 0x04C0, 0x04CF },
    { 0x1EDA, 0x1EDB },
    { 0x1FDB, 0x1F77 },
    { 0xA662, 0xA663 }
};

static const CaseFoldMapping1_16 case_fold1_16_197[] = {
    { 0x00C5, 0x00E5 },
    { 0x01C4, 0x01C6 },
    { 0x04C1, 0x04C2 },
    { 0x1FDA, 0x1F76 },
    { 0xA762, 0xA763 },
    { 0xFF3A, 0xFF5A }
};

static const CaseFoldMapping1_16 case_fold1_16_198[] = {
    { 0x00C6, 0x00E6 },
    { 0x01C7, 0x01C9 },
    { 0x1ED8, 0x1ED9 },
    { 0x1FD9, 0x1FD1 },
    { 0xA660, 0xA661 },
    { 0xFF39, 0xFF59 }
};

static const CaseFoldMapping1_16 case_fold1_16_199[] = {
    { 0x00C7, 0x00E7 },
    { 0x04C3, 0x04C4 },
    { 0x1FD8, 0x1FD0 },
    { 0x2CEB, 0x2CEC },
    { 0xA760, 0xA761 },
    { 0xFF38, 0xFF58 }
};

static const CaseFoldMapping1_16 case_fold1_16_200[] = {
    { 0x00C8, 0x00E8 },
    { 0x1ED6, 0x1ED7 },
    { 0xFF37, 0xFF57 }
};

static const CaseFoldMapping1_16 case_fold1_16_201[] = {
    { 0x00C9, 0x00E9 },
    { 0x01C8, 0x01C9 },
    { 0x04CD, 0x04CE },
    { 0xA76E, 0xA76F },
    { 0xFF36, 0xFF56 }
};

static const CaseFoldMapping1_16 case_fold1_16_202[] = {
    { 0x00CA, 0x00EA },
    { 0x01CB, 0x01CC },
    { 0x1ED4, 0x1ED5 },
    { 0xA66C, 0xA66D },
    { 0xFF35, 0xFF55 }
};

static const CaseFoldMapping1_16 case_fold1_16_203[] = {
    { 0x00CB, 0x00EB },
    { 0x01CA, 0x01CC },
    { 0xA76C, 0xA76D },
    { 0xFF34, 0xFF54 }
};

static const CaseFoldMapping1_16 case_fold1_16_204[] = {
    { 0x00CC, 0x00EC },
    { 0x01CD, 0x01CE },
    { 0x03CF, 0x03D7 },
    { 0x1ED2, 0x1ED3 },
    { 0x2CE0, 0x2CE1 },
    { 0xA66A, 0xA66B },
    { 0xFF33, 0xFF53 }
};

static const CaseFoldMapping1_16 case_fold1_16_205[] = {
    { 0x00CD, 0x00ED },
    { 0x04C9, 0x04CA },
    { 0xA76A, 0xA76B },
    { 0xFF32, 0xFF52 }
};

static const CaseFoldMapping1_16 case_fold1_16_206[] = {
    { 0x00CE, 0x00EE },
    { 0x01CF, 0x01D0 },
    { 0x1ED0, 0x1ED1 },
    { 0x2CE2, 0x2CE3 },
    { 0xA668, 0xA669 },
    { 0xFF31, 0xFF51 }
};

static const CaseFoldMapping1_16 case_fold1_16_207[] = {
    { 0x00CF, 0x00EF },
    { 0x04CB, 0x04CC },
    { 0xA768, 0xA769 },
    { 0xFF30, 0xFF50 }
};

static const CaseFoldMapping1_16 case_fold1_16_208[] = {
    { 0x00D0, 0x00F0 },
    { 0x01D1, 0x01D2 },
    { 0x04D4, 0x04D5 },
    { 0x10C0, 0x2D20 },
    { 0x1ECE, 0x1ECF },
    { 0xAB7B, 0x13AB },
    { 0xFF2F, 0xFF4F }
};

static const CaseFoldMapping1_16 case_fold1_16_209[] = {
    { 0x00D1, 0x00F1 },
    { 0x10C1, 0x2D21 },
    { 0xAB7A, 0x13AA },
    { 0xFF2E, 0xFF4E }
};

static const CaseFoldMapping1_16 case_fold1_16_210[] = {
    { 0x00D2, 0x00F2 },
    { 0x01D3, 0x01D4 },
    { 0x03D1, 0x03B8 },
    { 0x04D6, 0x04D7 },
    { 0x10C2, 0x2D22 },
    { 0x1ECC, 0x1ECD },
    { 0xAB79, 0x13A9 },
    { 0xFF2D, 0xFF4D }
};

static const CaseFoldMapping1_16 case_fold1_16_211[] = {
    { 0x00D3, 0x00F3 },
    { 0x03D0, 0x03B2 },
    { 0x10C3, 0x2D23 },
    { 0xAB78, 0x13A8 },
    { 0xFF2C, 0xFF4C }
};

static const CaseFoldMapping1_16 case_fold1_16_212[] = {
    { 0x00D4, 0x00F4 },
    { 0x01D5, 0x01D6 },
    { 0x04D0, 0x04D1 },
    { 0x10C4, 0x2D24 },
    { 0x1ECA, 0x1ECB },
    { 0x1FCB, 0x1F75 },
    { 0xAB7F, 0x13AF },
    { 0xFF2B, 0xFF4B }
};

static const CaseFoldMapping1_16 case_fold1_16_213[] = {
    { 0x00D5, 0x00F5 },
    { 0x03D6, 0x03C0 },
    { 0x10C5, 0x2D25 },
    { 0x1FCA, 0x1F74 },
    { 0xAB7E, 0x13AE },
    { 0xFF2A, 0xFF4A }
};

static const CaseFoldMapping1_16 case_fold1_16_214[] = {
    { 0x00D6, 0x00F6 },
    { 0x01D7, 0x01D8 },
    { 0x03D5, 0x03C6 },
    { 0x04D2, 0x04D3 },
    { 0x1EC8, 0x1EC9 },
    { 0x1FC9, 0x1F73 },
    { 0xAB7D, 0x13AD },
    { 0xFF29, 0xFF49 }
};

static const CaseFoldMapping1_16 case_fold1_16_215[] = {
    { 0x10C7, 0x2D27 },
    { 0x1FC8, 0x1F72 },
    { 0xAB7C, 0x13AC },
    { 0xFF28, 0xFF48 }
};

static const CaseFoldMapping1_16 case_fold1_16_216[] = {
    { 0x00D8, 0x00F8 },
    { 0x01D9, 0x01DA },
    { 0x04DC, 0x04DD },
    { 0x1EC6, 0x1EC7 },
    { 0xAB73, 0x13A3 },
    { 0xFF27, 0xFF47 }
};

static const CaseFoldMapping1_16 case_fold1_16_217[] = {
    { 0x00D9, 0x00F9 },
    { 0x03DA, 0x03DB },
    { 0xA77E, 0xA77F },
    { 0xAB72, 0x13A2 },
    { 0xFF26, 0xFF46 }
};

static const CaseFoldMapping1_16 case_fold1_16_218[] = {
    { 0x00DA, 0x00FA },
    { 0x01DB, 0x01DC },
    { 0x04DE, 0x04DF },
    { 0x1EC4, 0x1EC5 },
    { 0xA77D, 0x1D79 },
    { 0xAB71, 0x13A1 },
    { 0xFF25, 0xFF45 }
};

static const CaseFoldMapping1_16 case_fold1_16_219[] = {
    { 0x00DB, 0x00FB },
    { 0x03D8, 0x03D9 },
    { 0xAB70, 0x13A0 },
    { 0xFF24, 0xFF44 }
};

static const CaseFoldMapping1_16 case_fold1_16_220[] = {
    { 0x00DC, 0x00FC },
    { 0x04D8, 0x04D9 },
    { 0x1EC2, 0x1EC3 },
    { 0xA77B, 0xA77C },
    { 0xAB77, 0x13A7 },
    { 0xFF23, 0xFF43 }
};

static const CaseFoldMapping1_16 case_fold1_16_221[] = {
    { 0x00DD, 0x00FD },
    { 0x03DE, 0x03DF },
    { 0x10CD, 0x2D2D },
    { 0xAB76, 0x13A6 },
    { 0xFF22, 0xFF42 }
};

static const CaseFoldMapping1_16 case_fold1_16_222[] = {
    { 0x00DE, 0x00FE },
    { 0x04DA, 0x04DB },
    { 0x1EC0, 0x1EC1 },
    { 0x2CF2, 0x2CF3 },
    { 0xA779, 0xA77A },
    { 0xAB75, 0x13A5 },
    { 0xFF21, 0xFF41 }
};

static const CaseFoldMapping1_16 case_fold1_16_223[] = {
    { 0x01DE, 0x01DF },
    { 0x03DC, 0x03DD },
    { 0xAB74, 0x13A4 }
};

static const CaseFoldMapping1_16 case_fold1_16_224[] = {
    { 0x04E4, 0x04E5 },
    { 0x1EFE, 0x1EFF },
    { 0x24C4, 0x24DE },
    { 0x2CCC, 0x2CCD },
    { 0xA646, 0xA647 }
};

static const CaseFoldMapping1_16 case_fold1_16_225[] = {
    { 0x01E0, 0x01E1 },
    { 0x03E2, 0x03E3 },
    { 0x24C5, 0x24DF },
    { 0xA746, 0xA747 }
};

static const CaseFoldMapping1_16 case_fold1_16_226[] = {
    { 0x04E6, 0x04E7 },
    { 0x1EFC, 0x1EFD },
    { 0x24C6, 0x24E0 },
    { 0x2CCE, 0x2CCF },
    { 0xA644, 0xA645 }
};

static const CaseFoldMapping1_16 case_fold1_16_227[] = {
    { 0x01E2, 0x01E3 },
    { 0x03E0, 0x03E1 },
    { 0x24C7, 0x24E1 },
    { 0xA744, 0xA745 }
};

static const CaseFoldMapping1_16 case_fold1_16_228[] = {
    { 0x04E0, 0x04E1 },
    { 0x1EFA, 0x1EFB },
    { 0x1FFB, 0x1F7D },
    { 0x24C0, 0x24DA },
    { 0x2CC8, 0x2CC9 },
    { 0xA642, 0xA643 }
};

static const CaseFoldMapping1_16 case_fold1_16_229[] = {
    { 0x01E4, 0x01E5 },
    { 0x03E6, 0x03E7 },
    { 0x1FFA, 0x1F7C },
    { 0x24C1, 0x24DB },
    { 0xA742, 0xA743 }
};

static const CaseFoldMapping1_16 case_fold1_16_230[] = {
    { 0x04E2, 0x04E3 },
    { 0x1EF8, 0x1EF9 },
    { 0x1FF9, 0x1F79 },
    { 0x24C2, 0x24DC },
    { 0x2CCA, 0x2CCB },
    { 0xA640, 0xA641 }
};

static const CaseFoldMapping1_16 case_fold1_16_231[] = {
    { 0x01E6, 0x01E7 },
    { 0x03E4, 0x03E5 },
    { 0x1FF8, 0x1F78 },
    { 0x24C3, 0x24DD },
    { 0xA740, 0xA741 }
};

static const CaseFoldMapping1_16 case_fold1_16_232[] = {
    { 0x04EC, 0x04ED },
    { 0x13FB, 0x13F3 },
    { 0x1EF6, 0x1EF7 },
    { 0x24CC, 0x24E6 },
    { 0x2CC4, 0x2CC5 },
    { 0xA64E, 0xA64F }
};

static const CaseFoldMapping1_16 case_fold1_16_233[] = {
    { 0x01E8, 0x01E9 },
    { 0x03EA, 0x03EB },
    { 0x13FA, 0x13F2 },
    { 0x24CD, 0x24E7 },
    { 0xA74E, 0xA74F }
};

static const CaseFoldMapping1_16 case_fold1_16_234[] = {
    { 0x04EE, 0x04EF },
    { 0x13F9, 0x13F1 },
    { 0x1EF4, 0x1EF5 },
    { 0x24CE, 0x24E8 },
    { 0x2CC6, 0x2CC7 },
    { 0xA64C, 0xA64D }
};

static const CaseFoldMapping1_16 case_fold1_16_235[] = {
    { 0x01EA, 0x01EB },
    { 0x03E8, 0x03E9 },
    { 0x13F8, 0x13F0 },
    { 0x24CF, 0x24E9 },
    { 0xA74C, 0xA74D }
};

static const CaseFoldMapping1_16 case_fold1_16_236[] = {
    { 0x04E8, 0x04E9 },
    { 0x1EF2, 0x1EF3 },
    { 0x24C8, 0x24E2 },
    { 0x2CC0, 0x2CC1 },
    { 0xA64A, 0xA64B }
};

static const CaseFoldMapping1_16 case_fold1_16_237[] = {
    { 0x01EC, 0x01ED },
    { 0x03EE, 0x03EF },
    { 0x24C9, 0x24E3 },
    { 0xA74A, 0xA74B }
};

static const CaseFoldMapping1_16 case_fold1_16_238[] = {
    { 0x04EA, 0x04EB },
    { 0x13FD, 0x13F5 },
    { 0x1EF0, 0x1EF1 },
    { 0x24CA, 0x24E4 },
    { 0x2CC2, 0x2CC3 },
    { 0xA648, 0xA649 }
};

static const CaseFoldMapping1_16 case_fold1_16_239[] = {
    { 0x01EE, 0x01EF },
    { 0x03EC, 0x03ED },
    { 0x13FC, 0x13F4 },
    { 0x24CB, 0x24E5 },
    { 0xA748, 0xA749 }
};

static const CaseFoldMapping1_16 case_fold1_16_240[] = {
    { 0x01F1, 0x01F3 },
    { 0x04F4, 0x04F5 },
    { 0x1EEE, 0x1EEF },
    { 0x2CDC, 0x2CDD },
    { 0xA656, 0xA657 }
};

static const CaseFoldMapping1_16 case_fold1_16_241[] = {
    { 0xA756, 0xA757 }
};

static const CaseFoldMapping1_16 case_fold1_16_242[] = {
    { 0x03F1, 0x03C1 },
    { 0x04F6, 0x04F7 },
    { 0x1EEC, 0x1EED },
    { 0x2CDE, 0x2CDF },
    { 0xA654, 0xA655 }
};

static const CaseFoldMapping1_16 case_fold1_16_243[] = {
    { 0x01F2, 0x01F3 },
    { 0x03F0, 0x03BA },
    { 0x1FEC, 0x1FE5 },
    { 0xA754, 0xA755 }
};

static const CaseFoldMapping1_16 case_fold1_16_244[] = {
    { 0x03F7, 0x03F8 },
    { 0x04F0, 0x04F1 },
    { 0x1EEA, 0x1EEB },
    { 0x1FEB, 0x1F7B },
    { 0x2CD8, 0x2CD9 },
    { 0xA652, 0xA653 }
};

static const CaseFoldMapping1_16 case_fold1_16_245[] = {
    { 0x01F4, 0x01F5 },
    { 0x1FEA, 0x1F7A },
    { 0xA752, 0xA753 }
};

static const CaseFoldMapping1_16 case_fold1_16_246[] = {
    { 0x01F7, 0x01BF },
    { 0x03F5, 0x03B5 },
    { 0x04F2, 0x04F3 },
    { 0x1EE8, 0x1EE9 },
    { 0x1FE9, 0x1FE1 },
    { 0x2CDA, 0x2CDB },
    { 0xA650, 0xA651 }
};

static const CaseFoldMapping1_16 case_fold1_16_247[] = {
    { 0x01F6, 0x0195 },
    { 0x03F4, 0x03B8 },
    { 0x1FE8, 0x1FE0 },
    { 0xA750, 0xA751 }
};

static const CaseFoldMapping1_16 case_fold1_16_248[] = {
    { 0x04FC, 0x04FD },
    { 0x1EE6, 0x1EE7 },
    { 0x2CD4, 0x2CD5 },
    { 0xA65E, 0xA65F }
};

static const CaseFoldMapping1_16 case_fold1_16_249[] = {
    { 0x01F8, 0x01F9 },
    { 0x03FA, 0x03FB },
    { 0xA75E, 0xA75F }
};

static const CaseFoldMapping1_16 case_fold1_16_250[] = {
    { 0x03F9, 0x03F2 },
    { 0x04FE, 0x04FF },
    { 0x1EE4, 0x1EE5 },
    { 0x2CD6, 0x2CD7 },
    { 0xA65C, 0xA65D }
};

static const CaseFoldMapping1_16 case_fold1_16_251[] = {
    { 0x01FA, 0x01FB },
    { 0xA75C, 0xA75D }
};

static const CaseFoldMapping1_16 case_fold1_16_252[] = {
    { 0x03FF, 0x037D },
    { 0x04F8, 0x04F9 },
    { 0x1EE2, 0x1EE3 },
    { 0x2CD0, 0x2CD1 },
    { 0xA65A, 0xA65B }
};

static const CaseFoldMapping1_16 case_fold1_16_253[] = {
    { 0x01FC, 0x01FD },
    { 0x03FE, 0x037C },
    { 0xA75A, 0xA75B }
};

static const CaseFoldMapping1_16 case_fold1_16_254[] = {
    { 0x03FD, 0x037B },
    { 0x04FA, 0x04FB },
    { 0x1EE0, 0x1EE1 },
    { 0x2CD2, 0x2CD3 },
    { 0xA658, 0xA659 }
};

static const CaseFoldMapping1_16 case_fold1_16_255[] = {
    { 0x01FE, 0x01FF },
    { 0xA758, 0xA759 }
};

static const CaseFoldMapping1_32 case_fold1_32_000[] = {
    { 0x10404, 0x1042C },
    { 0x10414, 0x1043C },
    { 0x10424, 0x1044C },
    { 0x10C8C, 0x10CCC },
    { 0x10C9C, 0x10CDC },
    { 0x10CAC, 0x10CEC },
    { 0x118A8, 0x118C8 },
    { 0x118B8, 0x118D8 }
};

static const CaseFoldMapping1_32 case_fold1_32_001[] = {
    { 0x10405, 0x1042D },
    { 0x10415, 0x1043D },
    { 0x10425, 0x1044D },
    { 0x10C8D, 0x10CCD },
    { 0x10C9D, 0x10CDD },
    { 0x10CAD, 0x10CED },
    { 0x118A9, 0x118C9 },
    { 0x118B9, 0x118D9 }
};

static const CaseFoldMapping1_32 case_fold1_32_002[] = {
    { 0x10406, 0x1042E },
    { 0x10416, 0x1043E },
    { 0x10426, 0x1044E },
    { 0x10C8E, 0x10CCE },
    { 0x10C9E, 0x10CDE },
    { 0x10CAE, 0x10CEE },
    { 0x118AA, 0x118CA },
    { 0x118BA, 0x118DA }
};

static const CaseFoldMapping1_32 case_fold1_32_003[] = {
    { 0x10407, 0x1042F },
    { 0x10417, 0x1043F },
    { 0x10427, 0x1044F },
    { 0x10C8F, 0x10CCF },
    { 0x10C9F, 0x10CDF },
    { 0x10CAF, 0x10CEF },
    { 0x118AB, 0x118CB },
    { 0x118BB, 0x118DB }
};

static const CaseFoldMapping1_32 case_fold1_32_004[] = {
    { 0x10400, 0x10428 },
    { 0x10410, 0x10438 },
    { 0x10420, 0x10448 },
    { 0x10C88, 0x10CC8 },
    { 0x10C98, 0x10CD8 },
    { 0x10CA8, 0x10CE8 },
    { 0x118AC, 0x118CC },
    { 0x118BC, 0x118DC }
};

static const CaseFoldMapping1_32 case_fold1_32_005[] = {
    { 0x10401, 0x10429 },
    { 0x10411, 0x10439 },
    { 0x10421, 0x10449 },
    { 0x10C89, 0x10CC9 },
    { 0x10C99, 0x10CD9 },
    { 0x10CA9, 0x10CE9 },
    { 0x118AD, 0x118CD },
    { 0x118BD, 0x118DD }
};

static const CaseFoldMapping1_32 case_fold1_32_006[] = {
    { 0x10402, 0x1042A },
    { 0x10412, 0x1043A },
    { 0x10422, 0x1044A },
    { 0x10C8A, 0x10CCA },
    { 0x10C9A, 0x10CDA },
    { 0x10CAA, 0x10CEA },
    { 0x118AE, 0x118CE },
    { 0x118BE, 0x118DE }
};

static const CaseFoldMapping1_32 case_fold1_32_007[] = {
    { 0x10403, 0x1042B },
    { 0x10413, 0x1043B },
    { 0x10423, 0x1044B },
    { 0x10C8B, 0x10CCB },
    { 0x10C9B, 0x10CDB },
    { 0x10CAB, 0x10CEB },
    { 0x118AF, 0x118CF },
    { 0x118BF, 0x118DF }
};

static const CaseFoldMapping1_32 case_fold1_32_008[] = {
    { 0x1040C, 0x10434 },
    { 0x1041C, 0x10444 },
    { 0x10C84, 0x10CC4 },
    { 0x10C94, 0x10CD4 },
    { 0x10CA4, 0x10CE4 },
    { 0x118A0, 0x118C0 },
    { 0x118B0, 0x118D0 }
};

static const CaseFoldMapping1_32 case_fold1_32_009[] = {
    { 0x1040D, 0x10435 },
    { 0x1041D, 0x10445 },
    { 0x10C85, 0x10CC5 },
    { 0x10C95, 0x10CD5 },
    { 0x10CA5, 0x10CE5 },
    { 0x118A1, 0x118C1 },
    { 0x118B1, 0x118D1 }
};

static const CaseFoldMapping1_32 case_fold1_32_010[] = {
    { 0x1040E, 0x10436 },
    { 0x1041E, 0x10446 },
    { 0x10C86, 0x10CC6 },
    { 0x10C96, 0x10CD6 },
    { 0x10CA6, 0x10CE6 },
    { 0x118A2, 0x118C2 },
    { 0x118B2, 0x118D2 }
};

static const CaseFoldMapping1_32 case_fold1_32_011[] = {
    { 0x1040F, 0x10437 },
    { 0x1041F, 0x10447 },
    { 0x10C87, 0x10CC7 },
    { 0x10C97, 0x10CD7 },
    { 0x10CA7, 0x10CE7 },
    { 0x118A3, 0x118C3 },
    { 0x118B3, 0x118D3 }
};

static const CaseFoldMapping1_32 case_fold1_32_012[] = {
    { 0x10408, 0x10430 },
    { 0x10418, 0x10440 },
    { 0x10C80, 0x10CC0 },
    { 0x10C90, 0x10CD0 },
    { 0x10CA0, 0x10CE0 },
    { 0x10CB0, 0x10CF0 },
    { 0x118A4, 0x118C4 },
    { 0x118B4, 0x118D4 }
};

static const CaseFoldMapping1_32 case_fold1_32_013[] = {
    { 0x10409, 0x10431 },
    { 0x10419, 0x10441 },
    { 0x10C81, 0x10CC1 },
    { 0x10C91, 0x10CD1 },
    { 0x10CA1, 0x10CE1 },
    { 0x10CB1, 0x10CF1 },
    { 0x118A5, 0x118C5 },
    { 0x118B5, 0x118D5 }
};

static const CaseFoldMapping1_32 case_fold1_32_014[] = {
    { 0x1040A, 0x10432 },
    { 0x1041A, 0x10442 },
    { 0x10C82, 0x10CC2 },
    { 0x10C92, 0x10CD2 },
    { 0x10CA2, 0x10CE2 },
    { 0x10CB2, 0x10CF2 },
    { 0x118A6, 0x118C6 },
    { 0x118B6, 0x118D6 }
};

static const CaseFoldMapping1_32 case_fold1_32_015[] = {
    { 0x1040B, 0x10433 },
    { 0x1041B, 0x10443 },
    { 0x10C83, 0x10CC3 },
    { 0x10C93, 0x10CD3 },
    { 0x10CA3, 0x10CE3 },
    { 0x118A7, 0x118C7 },
    { 0x118B7, 0x118D7 }
};

static const CaseFoldMapping2_16 case_fold2_16_000[] = {
    { 0x1E9E, 0x0073, 0x0073 },
    { 0x1F8F, 0x1F07, 0x03B9 },
    { 0x1F9F, 0x1F27, 0x03B9 },
    { 0x1FAF, 0x1F67, 0x03B9 }
};

static const CaseFoldMapping2_16 case_fold2_16_001[] = {
    { 0x0130, 0x0069, 0x0307 },
    { 0x01F0, 0x006A, 0x030C },
    { 0x1F8E, 0x1F06, 0x03B9 },
    { 0x1F9E, 0x1F26, 0x03B9 },
    { 0x1FAE, 0x1F66, 0x03B9 }
};

static const CaseFoldMapping2_16 case_fold2_16_002[] = {
    { 0x0587, 0x0565, 0x0582 },
    { 0x1F8D, 0x1F05, 0x03B9 },
    { 0x1F9D, 0x1F25, 0x03B9 },
    { 0x1FAD, 0x1F65, 0x03B9 }
};

static const CaseFoldMapping2_16 case_fold2_16_003[] = {
    { 0x1F8C, 0x1F04, 0x03B9 },
    { 0x1F9C, 0x1F24, 0x03B9 },
    { 0x1FAC, 0x1F64, 0x03B9 },
    { 0x1FBC, 0x03B1, 0x03B9 },
    { 0x1FCC, 0x03B7, 0x03B9 },
    { 0x1FFC, 0x03C9, 0x03B9 }
};

static const CaseFoldMapping2_16 case_fold2_16_004[] = {
    { 0x1E9A, 0x0061, 0x02BE },
    { 0x1F8B, 0x1F03, 0x03B9 },
    { 0x1F9B, 0x1F23, 0x03B9 },
    { 0x1FAB, 0x1F63, 0x03B9 }
};

static const CaseFoldMapping2_16 case_fold2_16_005[] = {
    { 0x1F8A, 0x1F02, 0x03B9 },
    { 0x1F9A, 0x1F22, 0x03B9 },
    { 0x1FAA, 0x1F62, 0x03B9 }
};

static const CaseFoldMapping2_16 case_fold2_16_006[] = {
    { 0x1E98, 0x0077, 0x030A },
    { 0x1F89, 0x1F01, 0x03B9 },
    { 0x1F99, 0x1F21, 0x03B9 },
    { 0x1FA9, 0x1F61, 0x03B9 }
};

static const CaseFoldMapping2_16 case_fold2_16_007[] = {
    { 0x1E99, 0x0079, 0x030A },
    { 0x1F88, 0x1F00, 0x03B9 },
    { 0x1F98, 0x1F20, 0x03B9 },
    { 0x1FA8, 0x1F60, 0x03B9 }
};

static const CaseFoldMapping2_16 case_fold2_16_008[] = {
    { 0x0149, 0x02BC, 0x006E },
    { 0x1E96, 0x0068, 0x0331 },
    { 0x1F87, 0x1F07, 0x03B9 },
    { 0x1F97, 0x1F27, 0x03B9 },
    { 0x1FA7, 0x1F67, 0x03B9 },
    { 0xFB13, 0x0574, 0x0576 }
};

static const CaseFoldMapping2_16 case_fold2_16_009[] = {
    { 0x1E97, 0x0074, 0x0308 },
    { 0x1F86, 0x1F06, 0x03B9 },
    { 0x1F96, 0x1F26, 0x03B9 },
    { 0x1FA6, 0x1F66, 0x03B9 },
    { 0x1FB6, 0x03B1, 0x0342 },
    { 0x1FC6, 0x03B7, 0x0342 },
    { 0x1FD6, 0x03B9, 0x0342 },
    { 0x1FE6, 0x03C5, 0x0342 },
    { 0x1FF6, 0x03C9, 0x0342 },
    { 0xFB02, 0x0066, 0x006C }
};

static const CaseFoldMapping2_16 case_fold2_16_010[] = {
    { 0x1F85, 0x1F05, 0x03B9 },
    { 0x1F95, 0x1F25, 0x03B9 },
    { 0x1FA5, 0x1F65, 0x03B9 },
    { 0xFB01, 0x0066, 0x0069 }
};

static const CaseFoldMapping2_16 case_fold2_16_011[] = {
    { 0x1F84, 0x1F04, 0x03B9 },
    { 0x1F94, 0x1F24, 0x03B9 },
    { 0x1FA4, 0x1F64, 0x03B9 },
    { 0x1FB4, 0x03AC, 0x03B9 },
    { 0x1FC4, 0x03AE, 0x03B9 },
    { 0x1FE4, 0x03C1, 0x0313 },
    { 0x1FF4, 0x03CE, 0x03B9 },
    { 0xFB00, 0x0066, 0x0066 }
};

static const CaseFoldMapping2_16 case_fold2_16_012[] = {
    { 0x1F83, 0x1F03, 0x03B9 },
    { 0x1F93, 0x1F23, 0x03B9 },
    { 0x1FA3, 0x1F63, 0x03B9 },
    { 0x1FB3, 0x03B1, 0x03B9 },
    { 0x1FC3, 0x03B7, 0x03B9 },
    { 0x1FF3, 0x03C9, 0x03B9 },
    { 0xFB17, 0x0574, 0x056D }
};

static const CaseFoldMapping2_16 case_fold2_16_013[] = {
    { 0x1F82, 0x1F02, 0x03B9 },
    { 0x1F92, 0x1F22, 0x03B9 },
    { 0x1FA2, 0x1F62, 0x03B9 },
    { 0x1FB2, 0x1F70, 0x03B9 },
    { 0x1FC2, 0x1F74, 0x03B9 },
    { 0x1FF2, 0x1F7C, 0x03B9 },
    { 0xFB06, 0x0073, 0x0074 },
    { 0xFB16, 0x057E, 0x0576 }
};

static const CaseFoldMapping2_16 case_fold2_16_014[] = {
    { 0x1F81, 0x1F01, 0x03B9 },
    { 0x1F91, 0x1F21, 0x03B9 },
    { 0x1FA1, 0x1F61, 0x03B9 },
    { 0xFB05, 0x0073, 0x0074 },
    { 0xFB15, 0x0574, 0x056B }
};

static const CaseFoldMapping2_16 case_fold2_16_015[] = {
    { 0x00DF, 0x0073, 0x0073 },
    { 0x1F50, 0x03C5, 0x0313 },
    { 0x1F80, 0x1F00, 0x03B9 },
    { 0x1F90, 0x1F20, 0x03B9 },
    { 0x1FA0, 0x1F60, 0x03B9 },
    { 0xFB14, 0x0574, 0x0565 }
};

static const CaseFoldMapping3_16 case_fold3_16_000[] = {
    { 0x1FB7, 0x03B1, 0x0342, 0x03B9 },
    { 0x1FC7, 0x03B7, 0x0342, 0x03B9 },
    { 0x1FD3, 0x03B9, 0x0308, 0x0301 },
    { 0x1FD7, 0x03B9, 0x0308, 0x0342 },
    { 0x1FE3, 0x03C5, 0x0308, 0x0301 },
    { 0x1FE7, 0x03C5, 0x0308, 0x0342 },
    { 0x1FF7, 0x03C9, 0x0342, 0x03B9 },
    { 0xFB03, 0x0066, 0x0066, 0x0069 }
};

static const CaseFoldMapping3_16 case_fold3_16_001[] = {
    { 0x1F52, 0x03C5, 0x0313, 0x0300 },
    { 0x1F56, 0x03C5, 0x0313, 0x0342 },
    { 0x1FD2, 0x03B9, 0x0308, 0x0300 },
    { 0x1FE2, 0x03C5, 0x0308, 0x0300 }
};

static const CaseFoldMapping3_16 case_fold3_16_003[] = {
    { 0x0390, 0x03B9, 0x0308, 0x0301 },
    { 0x03B0, 0x03C5, 0x0308, 0x0301 },
    { 0x1F54, 0x03C5, 0x0313, 0x0301 },
    { 0xFB04, 0x0066, 0x0066, 0x006C }
};

static const CaseFoldHashBucket1_16 case_fold_hash1_16[] = {
    { case_fold1_16_000, __PHYSFS_ARRAYLEN(case_fold1_16_000) },
    { case_fold1_16_001, __PHYSFS_ARRAYLEN(case_fold1_16_001) },
    { case_fold1_16_002, __PHYSFS_ARRAYLEN(case_fold1_16_002) },
    { case_fold1_16_003, __PHYSFS_ARRAYLEN(case_fold1_16_003) },
    { case_fold1_16_004, __PHYSFS_ARRAYLEN(case_fold1_16_004) },
    { case_fold1_16_005, __PHYSFS_ARRAYLEN(case_fold1_16_005) },
    { case_fold1_16_006, __PHYSFS_ARRAYLEN(case_fold1_16_006) },
    { case_fold1_16_007, __PHYSFS_ARRAYLEN(case_fold1_16_007) },
    { case_fold1_16_008, __PHYSFS_ARRAYLEN(case_fold1_16_008) },
    { case_fold1_16_009, __PHYSFS_ARRAYLEN(case_fold1_16_009) },
    { case_fold1_16_010, __PHYSFS_ARRAYLEN(case_fold1_16_010) },
    { case_fold1_16_011, __PHYSFS_ARRAYLEN(case_fold1_16_011) },
    { case_fold1_16_012, __PHYSFS_ARRAYLEN(case_fold1_16_012) },
    { case_fold1_16_013, __PHYSFS_ARRAYLEN(case_fold1_16_013) },
    { case_fold1_16_014, __PHYSFS_ARRAYLEN(case_fold1_16_014) },
    { case_fold1_16_015, __PHYSFS_ARRAYLEN(case_fold1_16_015) },
    { case_fold1_16_016, __PHYSFS_ARRAYLEN(case_fold1_16_016) },
    { case_fold1_16_017, __PHYSFS_ARRAYLEN(case_fold1_16_017) },
    { case_fold1_16_018, __PHYSFS_ARRAYLEN(case_fold1_16_018) },
    { case_fold1_16_019, __PHYSFS_ARRAYLEN(case_fold1_16_019) },
    { case_fold1_16_020, __PHYSFS_ARRAYLEN(case_fold1_16_020) },
    { case_fold1_16_021, __PHYSFS_ARRAYLEN(case_fold1_16_021) },
    { case_fold1_16_022, __PHYSFS_ARRAYLEN(case_fold1_16_022) },
    { case_fold1_16_023, __PHYSFS_ARRAYLEN(case_fold1_16_023) },
    { case_fold1_16_024, __PHYSFS_ARRAYLEN(case_fold1_16_024) },
    { case_fold1_16_025, __PHYSFS_ARRAYLEN(case_fold1_16_025) },
    { case_fold1_16_026, __PHYSFS_ARRAYLEN(case_fold1_16_026) },
    { case_fold1_16_027, __PHYSFS_ARRAYLEN(case_fold1_16_027) },
    { case_fold1_16_028, __PHYSFS_ARRAYLEN(case_fold1_16_028) },
    { case_fold1_16_029, __PHYSFS_ARRAYLEN(case_fold1_16_029) },
    { case_fold1_16_030, __PHYSFS_ARRAYLEN(case_fold1_16_030) },
    { case_fold1_16_031, __PHYSFS_ARRAYLEN(case_fold1_16_031) },
    { case_fold1_16_032, __PHYSFS_ARRAYLEN(case_fold1_16_032) },
    { case_fold1_16_033, __PHYSFS_ARRAYLEN(case_fold1_16_033) },
    { case_fold1_16_034, __PHYSFS_ARRAYLEN(case_fold1_16_034) },
    { case_fold1_16_035, __PHYSFS_ARRAYLEN(case_fold1_16_035) },
    { case_fold1_16_036, __PHYSFS_ARRAYLEN(case_fold1_16_036) },
    { case_fold1_16_037, __PHYSFS_ARRAYLEN(case_fold1_16_037) },
    { case_fold1_16_038, __PHYSFS_ARRAYLEN(case_fold1_16_038) },
    { case_fold1_16_039, __PHYSFS_ARRAYLEN(case_fold1_16_039) },
    { case_fold1_16_040, __PHYSFS_ARRAYLEN(case_fold1_16_040) },
    { case_fold1_16_041, __PHYSFS_ARRAYLEN(case_fold1_16_041) },
    { case_fold1_16_042, __PHYSFS_ARRAYLEN(case_fold1_16_042) },
    { case_fold1_16_043, __PHYSFS_ARRAYLEN(case_fold1_16_043) },
    { case_fold1_16_044, __PHYSFS_ARRAYLEN(case_fold1_16_044) },
    { case_fold1_16_045, __PHYSFS_ARRAYLEN(case_fold1_16_045) },
    { case_fold1_16_046, __PHYSFS_ARRAYLEN(case_fold1_16_046) },
    { case_fold1_16_047, __PHYSFS_ARRAYLEN(case_fold1_16_047) },
    { case_fold1_16_048, __PHYSFS_ARRAYLEN(case_fold1_16_048) },
    { case_fold1_16_049, __PHYSFS_ARRAYLEN(case_fold1_16_049) },
    { case_fold1_16_050, __PHYSFS_ARRAYLEN(case_fold1_16_050) },
    { case_fold1_16_051, __PHYSFS_ARRAYLEN(case_fold1_16_051) },
    { case_fold1_16_052, __PHYSFS_ARRAYLEN(case_fold1_16_052) },
    { case_fold1_16_053, __PHYSFS_ARRAYLEN(case_fold1_16_053) },
    { case_fold1_16_054, __PHYSFS_ARRAYLEN(case_fold1_16_054) },
    { case_fold1_16_055, __PHYSFS_ARRAYLEN(case_fold1_16_055) },
    { case_fold1_16_056, __PHYSFS_ARRAYLEN(case_fold1_16_056) },
    { case_fold1_16_057, __PHYSFS_ARRAYLEN(case_fold1_16_057) },
    { case_fold1_16_058, __PHYSFS_ARRAYLEN(case_fold1_16_058) },
    { case_fold1_16_059, __PHYSFS_ARRAYLEN(case_fold1_16_059) },
    { case_fold1_16_060, __PHYSFS_ARRAYLEN(case_fold1_16_060) },
    { case_fold1_16_061, __PHYSFS_ARRAYLEN(case_fold1_16_061) },
    { case_fold1_16_062, __PHYSFS_ARRAYLEN(case_fold1_16_062) },
    { case_fold1_16_063, __PHYSFS_ARRAYLEN(case_fold1_16_063) },
    { case_fold1_16_064, __PHYSFS_ARRAYLEN(case_fold1_16_064) },
    { case_fold1_16_065, __PHYSFS_ARRAYLEN(case_fold1_16_065) },
    { case_fold1_16_066, __PHYSFS_ARRAYLEN(case_fold1_16_066) },
    { case_fold1_16_067, __PHYSFS_ARRAYLEN(case_fold1_16_067) },
    { case_fold1_16_068, __PHYSFS_ARRAYLEN(case_fold1_16_068) },
    { case_fold1_16_069, __PHYSFS_ARRAYLEN(case_fold1_16_069) },
    { case_fold1_16_070, __PHYSFS_ARRAYLEN(case_fold1_16_070) },
    { case_fold1_16_071, __PHYSFS_ARRAYLEN(case_fold1_16_071) },
    { case_fold1_16_072, __PHYSFS_ARRAYLEN(case_fold1_16_072) },
    { case_fold1_16_073, __PHYSFS_ARRAYLEN(case_fold1_16_073) },
    { case_fold1_16_074, __PHYSFS_ARRAYLEN(case_fold1_16_074) },
    { case_fold1_16_075, __PHYSFS_ARRAYLEN(case_fold1_16_075) },
    { case_fold1_16_076, __PHYSFS_ARRAYLEN(case_fold1_16_076) },
    { case_fold1_16_077, __PHYSFS_ARRAYLEN(case_fold1_16_077) },
    { case_fold1_16_078, __PHYSFS_ARRAYLEN(case_fold1_16_078) },
    { case_fold1_16_079, __PHYSFS_ARRAYLEN(case_fold1_16_079) },
    { case_fold1_16_080, __PHYSFS_ARRAYLEN(case_fold1_16_080) },
    { case_fold1_16_081, __PHYSFS_ARRAYLEN(case_fold1_16_081) },
    { case_fold1_16_082, __PHYSFS_ARRAYLEN(case_fold1_16_082) },
    { case_fold1_16_083, __PHYSFS_ARRAYLEN(case_fold1_16_083) },
    { case_fold1_16_084, __PHYSFS_ARRAYLEN(case_fold1_16_084) },
    { case_fold1_16_085, __PHYSFS_ARRAYLEN(case_fold1_16_085) },
    { case_fold1_16_086, __PHYSFS_ARRAYLEN(case_fold1_16_086) },
    { case_fold1_16_087, __PHYSFS_ARRAYLEN(case_fold1_16_087) },
    { case_fold1_16_088, __PHYSFS_ARRAYLEN(case_fold1_16_088) },
    { case_fold1_16_089, __PHYSFS_ARRAYLEN(case_fold1_16_089) },
    { case_fold1_16_090, __PHYSFS_ARRAYLEN(case_fold1_16_090) },
    { case_fold1_16_091, __PHYSFS_ARRAYLEN(case_fold1_16_091) },
    { case_fold1_16_092, __PHYSFS_ARRAYLEN(case_fold1_16_092) },
    { case_fold1_16_093, __PHYSFS_ARRAYLEN(case_fold1_16_093) },
    { case_fold1_16_094, __PHYSFS_ARRAYLEN(case_fold1_16_094) },
    { case_fold1_16_095, __PHYSFS_ARRAYLEN(case_fold1_16_095) },
    { case_fold1_16_096, __PHYSFS_ARRAYLEN(case_fold1_16_096) },
    { case_fold1_16_097, __PHYSFS_ARRAYLEN(case_fold1_16_097) },
    { case_fold1_16_098, __PHYSFS_ARRAYLEN(case_fold1_16_098) },
    { case_fold1_16_099, __PHYSFS_ARRAYLEN(case_fold1_16_099) },
    { case_fold1_16_100, __PHYSFS_ARRAYLEN(case_fold1_16_100) },
    { case_fold1_16_101, __PHYSFS_ARRAYLEN(case_fold1_16_101) },
    { case_fold1_16_102, __PHYSFS_ARRAYLEN(case_fold1_16_102) },
    { case_fold1_16_103, __PHYSFS_ARRAYLEN(case_fold1_16_103) },
    { case_fold1_16_104, __PHYSFS_ARRAYLEN(case_fold1_16_104) },
    { case_fold1_16_105, __PHYSFS_ARRAYLEN(case_fold1_16_105) },
    { case_fold1_16_106, __PHYSFS_ARRAYLEN(case_fold1_16_106) },
    { case_fold1_16_107, __PHYSFS_ARRAYLEN(case_fold1_16_107) },
    { case_fold1_16_108, __PHYSFS_ARRAYLEN(case_fold1_16_108) },
    { case_fold1_16_109, __PHYSFS_ARRAYLEN(case_fold1_16_109) },
    { case_fold1_16_110, __PHYSFS_ARRAYLEN(case_fold1_16_110) },
    { case_fold1_16_111, __PHYSFS_ARRAYLEN(case_fold1_16_111) },
    { case_fold1_16_112, __PHYSFS_ARRAYLEN(case_fold1_16_112) },
    { case_fold1_16_113, __PHYSFS_ARRAYLEN(case_fold1_16_113) },
    { case_fold1_16_114, __PHYSFS_ARRAYLEN(case_fold1_16_114) },
    { case_fold1_16_115, __PHYSFS_ARRAYLEN(case_fold1_16_115) },
    { case_fold1_16_116, __PHYSFS_ARRAYLEN(case_fold1_16_116) },
    { case_fold1_16_117, __PHYSFS_ARRAYLEN(case_fold1_16_117) },
    { case_fold1_16_118, __PHYSFS_ARRAYLEN(case_fold1_16_118) },
    { case_fold1_16_119, __PHYSFS_ARRAYLEN(case_fold1_16_119) },
    { case_fold1_16_120, __PHYSFS_ARRAYLEN(case_fold1_16_120) },
    { case_fold1_16_121, __PHYSFS_ARRAYLEN(case_fold1_16_121) },
    { case_fold1_16_122, __PHYSFS_ARRAYLEN(case_fold1_16_122) },
    { NULL, 0 },
    { case_fold1_16_124, __PHYSFS_ARRAYLEN(case_fold1_16_124) },
    { NULL, 0 },
    { case_fold1_16_126, __PHYSFS_ARRAYLEN(case_fold1_16_126) },
    { NULL, 0 },
    { case_fold1_16_128, __PHYSFS_ARRAYLEN(case_fold1_16_128) },
    { case_fold1_16_129, __PHYSFS_ARRAYLEN(case_fold1_16_129) },
    { case_fold1_16_130, __PHYSFS_ARRAYLEN(case_fold1_16_130) },
    { case_fold1_16_131, __PHYSFS_ARRAYLEN(case_fold1_16_131) },
    { case_fold1_16_132, __PHYSFS_ARRAYLEN(case_fold1_16_132) },
    { case_fold1_16_133, __PHYSFS_ARRAYLEN(case_fold1_16_133) },
    { case_fold1_16_134, __PHYSFS_ARRAYLEN(case_fold1_16_134) },
    { case_fold1_16_135, __PHYSFS_ARRAYLEN(case_fold1_16_135) },
    { case_fold1_16_136, __PHYSFS_ARRAYLEN(case_fold1_16_136) },
    { case_fold1_16_137, __PHYSFS_ARRAYLEN(case_fold1_16_137) },
    { case_fold1_16_138, __PHYSFS_ARRAYLEN(case_fold1_16_138) },
    { case_fold1_16_139, __PHYSFS_ARRAYLEN(case_fold1_16_139) },
    { case_fold1_16_140, __PHYSFS_ARRAYLEN(case_fold1_16_140) },
    { case_fold1_16_141, __PHYSFS_ARRAYLEN(case_fold1_16_141) },
    { case_fold1_16_142, __PHYSFS_ARRAYLEN(case_fold1_16_142) },
    { case_fold1_16_143, __PHYSFS_ARRAYLEN(case_fold1_16_143) },
    { case_fold1_16_144, __PHYSFS_ARRAYLEN(case_fold1_16_144) },
    { case_fold1_16_145, __PHYSFS_ARRAYLEN(case_fold1_16_145) },
    { case_fold1_16_146, __PHYSFS_ARRAYLEN(case_fold1_16_146) },
    { case_fold1_16_147, __PHYSFS_ARRAYLEN(case_fold1_16_147) },
    { case_fold1_16_148, __PHYSFS_ARRAYLEN(case_fold1_16_148) },
    { case_fold1_16_149, __PHYSFS_ARRAYLEN(case_fold1_16_149) },
    { case_fold1_16_150, __PHYSFS_ARRAYLEN(case_fold1_16_150) },
    { case_fold1_16_151, __PHYSFS_ARRAYLEN(case_fold1_16_151) },
    { case_fold1_16_152, __PHYSFS_ARRAYLEN(case_fold1_16_152) },
    { case_fold1_16_153, __PHYSFS_ARRAYLEN(case_fold1_16_153) },
    { case_fold1_16_154, __PHYSFS_ARRAYLEN(case_fold1_16_154) },
    { case_fold1_16_155, __PHYSFS_ARRAYLEN(case_fold1_16_155) },
    { case_fold1_16_156, __PHYSFS_ARRAYLEN(case_fold1_16_156) },
    { case_fold1_16_157, __PHYSFS_ARRAYLEN(case_fold1_16_157) },
    { case_fold1_16_158, __PHYSFS_ARRAYLEN(case_fold1_16_158) },
    { case_fold1_16_159, __PHYSFS_ARRAYLEN(case_fold1_16_159) },
    { case_fold1_16_160, __PHYSFS_ARRAYLEN(case_fold1_16_160) },
    { case_fold1_16_161, __PHYSFS_ARRAYLEN(case_fold1_16_161) },
    { case_fold1_16_162, __PHYSFS_ARRAYLEN(case_fold1_16_162) },
    { case_fold1_16_163, __PHYSFS_ARRAYLEN(case_fold1_16_163) },
    { case_fold1_16_164, __PHYSFS_ARRAYLEN(case_fold1_16_164) },
    { case_fold1_16_165, __PHYSFS_ARRAYLEN(case_fold1_16_165) },
    { case_fold1_16_166, __PHYSFS_ARRAYLEN(case_fold1_16_166) },
    { case_fold1_16_167, __PHYSFS_ARRAYLEN(case_fold1_16_167) },
    { case_fold1_16_168, __PHYSFS_ARRAYLEN(case_fold1_16_168) },
    { case_fold1_16_169, __PHYSFS_ARRAYLEN(case_fold1_16_169) },
    { case_fold1_16_170, __PHYSFS_ARRAYLEN(case_fold1_16_170) },
    { case_fold1_16_171, __PHYSFS_ARRAYLEN(case_fold1_16_171) },
    { case_fold1_16_172, __PHYSFS_ARRAYLEN(case_fold1_16_172) },
    { case_fold1_16_173, __PHYSFS_ARRAYLEN(case_fold1_16_173) },
    { case_fold1_16_174, __PHYSFS_ARRAYLEN(case_fold1_16_174) },
    { case_fold1_16_175, __PHYSFS_ARRAYLEN(case_fold1_16_175) },
    { case_fold1_16_176, __PHYSFS_ARRAYLEN(case_fold1_16_176) },
    { case_fold1_16_177, __PHYSFS_ARRAYLEN(case_fold1_16_177) },
    { case_fold1_16_178, __PHYSFS_ARRAYLEN(case_fold1_16_178) },
    { case_fold1_16_179, __PHYSFS_ARRAYLEN(case_fold1_16_179) },
    { case_fold1_16_180, __PHYSFS_ARRAYLEN(case_fold1_16_180) },
    { case_fold1_16_181, __PHYSFS_ARRAYLEN(case_fold1_16_181) },
    { case_fold1_16_182, __PHYSFS_ARRAYLEN(case_fold1_16_182) },
    { case_fold1_16_183, __PHYSFS_ARRAYLEN(case_fold1_16_183) },
    { case_fold1_16_184, __PHYSFS_ARRAYLEN(case_fold1_16_184) },
    { case_fold1_16_185, __PHYSFS_ARRAYLEN(case_fold1_16_185) },
    { case_fold1_16_186, __PHYSFS_ARRAYLEN(case_fold1_16_186) },
    { case_fold1_16_187, __PHYSFS_ARRAYLEN(case_fold1_16_187) },
    { case_fold1_16_188, __PHYSFS_ARRAYLEN(case_fold1_16_188) },
    { case_fold1_16_189, __PHYSFS_ARRAYLEN(case_fold1_16_189) },
    { case_fold1_16_190, __PHYSFS_ARRAYLEN(case_fold1_16_190) },
    { case_fold1_16_191, __PHYSFS_ARRAYLEN(case_fold1_16_191) },
    { case_fold1_16_192, __PHYSFS_ARRAYLEN(case_fold1_16_192) },
    { case_fold1_16_193, __PHYSFS_ARRAYLEN(case_fold1_16_193) },
    { case_fold1_16_194, __PHYSFS_ARRAYLEN(case_fold1_16_194) },
    { case_fold1_16_195, __PHYSFS_ARRAYLEN(case_fold1_16_195) },
    { case_fold1_16_196, __PHYSFS_ARRAYLEN(case_fold1_16_196) },
    { case_fold1_16_197, __PHYSFS_ARRAYLEN(case_fold1_16_197) },
    { case_fold1_16_198, __PHYSFS_ARRAYLEN(case_fold1_16_198) },
    { case_fold1_16_199, __PHYSFS_ARRAYLEN(case_fold1_16_199) },
    { case_fold1_16_200, __PHYSFS_ARRAYLEN(case_fold1_16_200) },
    { case_fold1_16_201, __PHYSFS_ARRAYLEN(case_fold1_16_201) },
    { case_fold1_16_202, __PHYSFS_ARRAYLEN(case_fold1_16_202) },
    { case_fold1_16_203, __PHYSFS_ARRAYLEN(case_fold1_16_203) },
    { case_fold1_16_204, __PHYSFS_ARRAYLEN(case_fold1_16_204) },
    { case_fold1_16_205, __PHYSFS_ARRAYLEN(case_fold1_16_205) },
    { case_fold1_16_206, __PHYSFS_ARRAYLEN(case_fold1_16_206) },
    { case_fold1_16_207, __PHYSFS_ARRAYLEN(case_fold1_16_207) },
    { case_fold1_16_208, __PHYSFS_ARRAYLEN(case_fold1_16_208) },
    { case_fold1_16_209, __PHYSFS_ARRAYLEN(case_fold1_16_209) },
    { case_fold1_16_210, __PHYSFS_ARRAYLEN(case_fold1_16_210) },
    { case_fold1_16_211, __PHYSFS_ARRAYLEN(case_fold1_16_211) },
    { case_fold1_16_212, __PHYSFS_ARRAYLEN(case_fold1_16_212) },
    { case_fold1_16_213, __PHYSFS_ARRAYLEN(case_fold1_16_213) },
    { case_fold1_16_214, __PHYSFS_ARRAYLEN(case_fold1_16_214) },
    { case_fold1_16_215, __PHYSFS_ARRAYLEN(case_fold1_16_215) },
    { case_fold1_16_216, __PHYSFS_ARRAYLEN(case_fold1_16_216) },
    { case_fold1_16_217, __PHYSFS_ARRAYLEN(case_fold1_16_217) },
    { case_fold1_16_218, __PHYSFS_ARRAYLEN(case_fold1_16_218) },
    { case_fold1_16_219, __PHYSFS_ARRAYLEN(case_fold1_16_219) },
    { case_fold1_16_220, __PHYSFS_ARRAYLEN(case_fold1_16_220) },
    { case_fold1_16_221, __PHYSFS_ARRAYLEN(case_fold1_16_221) },
    { case_fold1_16_222, __PHYSFS_ARRAYLEN(case_fold1_16_222) },
    { case_fold1_16_223, __PHYSFS_ARRAYLEN(case_fold1_16_223) },
    { case_fold1_16_224, __PHYSFS_ARRAYLEN(case_fold1_16_224) },
    { case_fold1_16_225, __PHYSFS_ARRAYLEN(case_fold1_16_225) },
    { case_fold1_16_226, __PHYSFS_ARRAYLEN(case_fold1_16_226) },
    { case_fold1_16_227, __PHYSFS_ARRAYLEN(case_fold1_16_227) },
    { case_fold1_16_228, __PHYSFS_ARRAYLEN(case_fold1_16_228) },
    { case_fold1_16_229, __PHYSFS_ARRAYLEN(case_fold1_16_229) },
    { case_fold1_16_230, __PHYSFS_ARRAYLEN(case_fold1_16_230) },
    { case_fold1_16_231, __PHYSFS_ARRAYLEN(case_fold1_16_231) },
    { case_fold1_16_232, __PHYSFS_ARRAYLEN(case_fold1_16_232) },
    { case_fold1_16_233, __PHYSFS_ARRAYLEN(case_fold1_16_233) },
    { case_fold1_16_234, __PHYSFS_ARRAYLEN(case_fold1_16_234) },
    { case_fold1_16_235, __PHYSFS_ARRAYLEN(case_fold1_16_235) },
    { case_fold1_16_236, __PHYSFS_ARRAYLEN(case_fold1_16_236) },
    { case_fold1_16_237, __PHYSFS_ARRAYLEN(case_fold1_16_237) },
    { case_fold1_16_238, __PHYSFS_ARRAYLEN(case_fold1_16_238) },
    { case_fold1_16_239, __PHYSFS_ARRAYLEN(case_fold1_16_239) },
    { case_fold1_16_240, __PHYSFS_ARRAYLEN(case_fold1_16_240) },
    { case_fold1_16_241, __PHYSFS_ARRAYLEN(case_fold1_16_241) },
    { case_fold1_16_242, __PHYSFS_ARRAYLEN(case_fold1_16_242) },
    { case_fold1_16_243, __PHYSFS_ARRAYLEN(case_fold1_16_243) },
    { case_fold1_16_244, __PHYSFS_ARRAYLEN(case_fold1_16_244) },
    { case_fold1_16_245, __PHYSFS_ARRAYLEN(case_fold1_16_245) },
    { case_fold1_16_246, __PHYSFS_ARRAYLEN(case_fold1_16_246) },
    { case_fold1_16_247, __PHYSFS_ARRAYLEN(case_fold1_16_247) },
    { case_fold1_16_248, __PHYSFS_ARRAYLEN(case_fold1_16_248) },
    { case_fold1_16_249, __PHYSFS_ARRAYLEN(case_fold1_16_249) },
    { case_fold1_16_250, __PHYSFS_ARRAYLEN(case_fold1_16_250) },
    { case_fold1_16_251, __PHYSFS_ARRAYLEN(case_fold1_16_251) },
    { case_fold1_16_252, __PHYSFS_ARRAYLEN(case_fold1_16_252) },
    { case_fold1_16_253, __PHYSFS_ARRAYLEN(case_fold1_16_253) },
    { case_fold1_16_254, __PHYSFS_ARRAYLEN(case_fold1_16_254) },
    { case_fold1_16_255, __PHYSFS_ARRAYLEN(case_fold1_16_255) },
};

static const CaseFoldHashBucket1_32 case_fold_hash1_32[] = {
    { case_fold1_32_000, __PHYSFS_ARRAYLEN(case_fold1_32_000) },
    { case_fold1_32_001, __PHYSFS_ARRAYLEN(case_fold1_32_001) },
    { case_fold1_32_002, __PHYSFS_ARRAYLEN(case_fold1_32_002) },
    { case_fold1_32_003, __PHYSFS_ARRAYLEN(case_fold1_32_003) },
    { case_fold1_32_004, __PHYSFS_ARRAYLEN(case_fold1_32_004) },
    { case_fold1_32_005, __PHYSFS_ARRAYLEN(case_fold1_32_005) },
    { case_fold1_32_006, __PHYSFS_ARRAYLEN(case_fold1_32_006) },
    { case_fold1_32_007, __PHYSFS_ARRAYLEN(case_fold1_32_007) },
    { case_fold1_32_008, __PHYSFS_ARRAYLEN(case_fold1_32_008) },
    { case_fold1_32_009, __PHYSFS_ARRAYLEN(case_fold1_32_009) },
    { case_fold1_32_010, __PHYSFS_ARRAYLEN(case_fold1_32_010) },
    { case_fold1_32_011, __PHYSFS_ARRAYLEN(case_fold1_32_011) },
    { case_fold1_32_012, __PHYSFS_ARRAYLEN(case_fold1_32_012) },
    { case_fold1_32_013, __PHYSFS_ARRAYLEN(case_fold1_32_013) },
    { case_fold1_32_014, __PHYSFS_ARRAYLEN(case_fold1_32_014) },
    { case_fold1_32_015, __PHYSFS_ARRAYLEN(case_fold1_32_015) },
};

static const CaseFoldHashBucket2_16 case_fold_hash2_16[] = {
    { case_fold2_16_000, __PHYSFS_ARRAYLEN(case_fold2_16_000) },
    { case_fold2_16_001, __PHYSFS_ARRAYLEN(case_fold2_16_001) },
    { case_fold2_16_002, __PHYSFS_ARRAYLEN(case_fold2_16_002) },
    { case_fold2_16_003, __PHYSFS_ARRAYLEN(case_fold2_16_003) },
    { case_fold2_16_004, __PHYSFS_ARRAYLEN(case_fold2_16_004) },
    { case_fold2_16_005, __PHYSFS_ARRAYLEN(case_fold2_16_005) },
    { case_fold2_16_006, __PHYSFS_ARRAYLEN(case_fold2_16_006) },
    { case_fold2_16_007, __PHYSFS_ARRAYLEN(case_fold2_16_007) },
    { case_fold2_16_008, __PHYSFS_ARRAYLEN(case_fold2_16_008) },
    { case_fold2_16_009, __PHYSFS_ARRAYLEN(case_fold2_16_009) },
    { case_fold2_16_010, __PHYSFS_ARRAYLEN(case_fold2_16_010) },
    { case_fold2_16_011, __PHYSFS_ARRAYLEN(case_fold2_16_011) },
    { case_fold2_16_012, __PHYSFS_ARRAYLEN(case_fold2_16_012) },
    { case_fold2_16_013, __PHYSFS_ARRAYLEN(case_fold2_16_013) },
    { case_fold2_16_014, __PHYSFS_ARRAYLEN(case_fold2_16_014) },
    { case_fold2_16_015, __PHYSFS_ARRAYLEN(case_fold2_16_015) },
};

static const CaseFoldHashBucket3_16 case_fold_hash3_16[] = {
    { case_fold3_16_000, __PHYSFS_ARRAYLEN(case_fold3_16_000) },
    { case_fold3_16_001, __PHYSFS_ARRAYLEN(case_fold3_16_001) },
    { NULL, 0 },
    { case_fold3_16_003, __PHYSFS_ARRAYLEN(case_fold3_16_003) },
};


#endif  /* _INCLUDE_PHYSFS_CASEFOLDING_H_ */

/* end of physfs_casefolding.h ... */


/*
 * From rfc3629, the UTF-8 spec:
 *  https://www.ietf.org/rfc/rfc3629.txt
 *
 *   Char. number range  |        UTF-8 octet sequence
 *      (hexadecimal)    |              (binary)
 *   --------------------+---------------------------------------------
 *   0000 0000-0000 007F | 0xxxxxxx
 *   0000 0080-0000 07FF | 110xxxxx 10xxxxxx
 *   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
 *   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 */


/*
 * This may not be the best value, but it's one that isn't represented
 *  in Unicode (0x10FFFF is the largest codepoint value). We return this
 *  value from utf8codepoint() if there's bogus bits in the
 *  stream. utf8codepoint() will turn this value into something
 *  reasonable (like a question mark), for text that wants to try to recover,
 *  whereas utf8valid() will use the value to determine if a string has bad
 *  bits.
 */
#define UNICODE_BOGUS_CHAR_VALUE 0xFFFFFFFF

/*
 * This is the codepoint we currently return when there was bogus bits in a
 *  UTF-8 string. May not fly in Asian locales?
 */
#define UNICODE_BOGUS_CHAR_CODEPOINT '?'

static PHYSFS_uint32 utf8codepoint(const char **_str)
{
    const char *str = *_str;
    PHYSFS_uint32 retval = 0;
    PHYSFS_uint32 octet = (PHYSFS_uint32) ((PHYSFS_uint8) *str);
    PHYSFS_uint32 octet2, octet3, octet4;

    if (octet == 0)  /* null terminator, end of string. */
        return 0;

    else if (octet < 128)  /* one octet char: 0 to 127 */
    {
        (*_str)++;  /* skip to next possible start of codepoint. */
        return octet;
    } /* else if */

    else if ((octet > 127) && (octet < 192))  /* bad (starts with 10xxxxxx). */
    {
        /*
         * Apparently each of these is supposed to be flagged as a bogus
         *  char, instead of just resyncing to the next valid codepoint.
         */
        (*_str)++;  /* skip to next possible start of codepoint. */
        return UNICODE_BOGUS_CHAR_VALUE;
    } /* else if */

    else if (octet < 224)  /* two octets */
    {
        (*_str)++;  /* advance at least one byte in case of an error */
        octet -= (128+64);
        octet2 = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet2 & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        *_str += 1;  /* skip to next possible start of codepoint. */
        retval = ((octet << 6) | (octet2 - 128));
        if ((retval >= 0x80) && (retval <= 0x7FF))
            return retval;
    } /* else if */

    else if (octet < 240)  /* three octets */
    {
        (*_str)++;  /* advance at least one byte in case of an error */
        octet -= (128+64+32);
        octet2 = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet2 & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet3 = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet3 & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        *_str += 2;  /* skip to next possible start of codepoint. */
        retval = ( ((octet << 12)) | ((octet2-128) << 6) | ((octet3-128)) );

        /* There are seven "UTF-16 surrogates" that are illegal in UTF-8. */
        switch (retval)
        {
            case 0xD800:
            case 0xDB7F:
            case 0xDB80:
            case 0xDBFF:
            case 0xDC00:
            case 0xDF80:
            case 0xDFFF:
                return UNICODE_BOGUS_CHAR_VALUE;
        } /* switch */

        /* 0xFFFE and 0xFFFF are illegal, too, so we check them at the edge. */
        if ((retval >= 0x800) && (retval <= 0xFFFD))
            return retval;
    } /* else if */

    else if (octet < 248)  /* four octets */
    {
        (*_str)++;  /* advance at least one byte in case of an error */
        octet -= (128+64+32+16);
        octet2 = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet2 & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet3 = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet3 & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet4 = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet4 & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        *_str += 3;  /* skip to next possible start of codepoint. */
        retval = ( ((octet << 18)) | ((octet2 - 128) << 12) |
                   ((octet3 - 128) << 6) | ((octet4 - 128)) );
        if ((retval >= 0x10000) && (retval <= 0x10FFFF))
            return retval;
    } /* else if */

    /*
     * Five and six octet sequences became illegal in rfc3629.
     *  We throw the codepoint away, but parse them to make sure we move
     *  ahead the right number of bytes and don't overflow the buffer.
     */

    else if (octet < 252)  /* five octets */
    {
        (*_str)++;  /* advance at least one byte in case of an error */
        octet = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        *_str += 4;  /* skip to next possible start of codepoint. */
        return UNICODE_BOGUS_CHAR_VALUE;
    } /* else if */

    else  /* six octets */
    {
        (*_str)++;  /* advance at least one byte in case of an error */
        octet = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        octet = (PHYSFS_uint32) ((PHYSFS_uint8) *(++str));
        if ((octet & (128+64)) != 128)  /* Format isn't 10xxxxxx? */
            return UNICODE_BOGUS_CHAR_VALUE;

        *_str += 6;  /* skip to next possible start of codepoint. */
        return UNICODE_BOGUS_CHAR_VALUE;
    } /* else if */

    return UNICODE_BOGUS_CHAR_VALUE;
} /* utf8codepoint */

static PHYSFS_uint32 utf16codepoint(const PHYSFS_uint16 **_str)
{
    const PHYSFS_uint16 *src = *_str;
    PHYSFS_uint32 cp = (PHYSFS_uint32) *(src++);

    if (cp == 0)  /* null terminator, end of string. */
        return 0;
    /* Orphaned second half of surrogate pair? */
    else if ((cp >= 0xDC00) && (cp <= 0xDFFF))
        cp = UNICODE_BOGUS_CHAR_CODEPOINT;
    else if ((cp >= 0xD800) && (cp <= 0xDBFF))  /* start surrogate pair! */
    {
        const PHYSFS_uint32 pair = (PHYSFS_uint32) *src;
        if (pair == 0)
            cp = UNICODE_BOGUS_CHAR_CODEPOINT;
        else if ((pair < 0xDC00) || (pair > 0xDFFF))
            cp = UNICODE_BOGUS_CHAR_CODEPOINT;
        else
        {
            src++;  /* eat the other surrogate. */
            cp = (((cp - 0xD800) << 10) | (pair - 0xDC00));
        } /* else */
    } /* else if */

    *_str = src;
    return cp;
} /* utf16codepoint */

static PHYSFS_uint32 utf32codepoint(const PHYSFS_uint32 **_str)
{
    const PHYSFS_uint32 *src = *_str;
    PHYSFS_uint32 cp = *(src++);

    if (cp == 0)  /* null terminator, end of string. */
        return 0;
    else if (cp > 0x10FFF)
        cp = UNICODE_BOGUS_CHAR_CODEPOINT;

    *_str = src;
    return cp;
} /* utf32codepoint */


void PHYSFS_utf8ToUcs4(const char *src, PHYSFS_uint32 *dst, PHYSFS_uint64 len)
{
    len -= sizeof (PHYSFS_uint32);   /* save room for null char. */
    while (len >= sizeof (PHYSFS_uint32))
    {
        PHYSFS_uint32 cp = utf8codepoint(&src);
        if (cp == 0)
            break;
        else if (cp == UNICODE_BOGUS_CHAR_VALUE)
            cp = UNICODE_BOGUS_CHAR_CODEPOINT;
        *(dst++) = cp;
        len -= sizeof (PHYSFS_uint32);
    } /* while */

    *dst = 0;
} /* PHYSFS_utf8ToUcs4 */


void PHYSFS_utf8ToUcs2(const char *src, PHYSFS_uint16 *dst, PHYSFS_uint64 len)
{
    len -= sizeof (PHYSFS_uint16);   /* save room for null char. */
    while (len >= sizeof (PHYSFS_uint16))
    {
        PHYSFS_uint32 cp = utf8codepoint(&src);
        if (cp == 0)
            break;
        else if (cp == UNICODE_BOGUS_CHAR_VALUE)
            cp = UNICODE_BOGUS_CHAR_CODEPOINT;

        if (cp > 0xFFFF)  /* UTF-16 surrogates (bogus chars in UCS-2) */
            cp = UNICODE_BOGUS_CHAR_CODEPOINT;

        *(dst++) = cp;
        len -= sizeof (PHYSFS_uint16);
    } /* while */

    *dst = 0;
} /* PHYSFS_utf8ToUcs2 */


void PHYSFS_utf8ToUtf16(const char *src, PHYSFS_uint16 *dst, PHYSFS_uint64 len)
{
    len -= sizeof (PHYSFS_uint16);   /* save room for null char. */
    while (len >= sizeof (PHYSFS_uint16))
    {
        PHYSFS_uint32 cp = utf8codepoint(&src);
        if (cp == 0)
            break;
        else if (cp == UNICODE_BOGUS_CHAR_VALUE)
            cp = UNICODE_BOGUS_CHAR_CODEPOINT;

        if (cp > 0xFFFF)  /* encode as surrogate pair */
        {
            if (len < (sizeof (PHYSFS_uint16) * 2))
                break;  /* not enough room for the pair, stop now. */

            cp -= 0x10000;  /* Make this a 20-bit value */

            *(dst++) = 0xD800 + ((cp >> 10) & 0x3FF);
            len -= sizeof (PHYSFS_uint16);

            cp = 0xDC00 + (cp & 0x3FF);
        } /* if */

        *(dst++) = cp;
        len -= sizeof (PHYSFS_uint16);
    } /* while */

    *dst = 0;
} /* PHYSFS_utf8ToUtf16 */

static void utf8fromcodepoint(PHYSFS_uint32 cp, char **_dst, PHYSFS_uint64 *_len)
{
    char *dst = *_dst;
    PHYSFS_uint64 len = *_len;

    if (len == 0)
        return;

    if (cp > 0x10FFFF)
        cp = UNICODE_BOGUS_CHAR_CODEPOINT;
    else if ((cp == 0xFFFE) || (cp == 0xFFFF))  /* illegal values. */
        cp = UNICODE_BOGUS_CHAR_CODEPOINT;
    else
    {
        /* There are seven "UTF-16 surrogates" that are illegal in UTF-8. */
        switch (cp)
        {
            case 0xD800:
            case 0xDB7F:
            case 0xDB80:
            case 0xDBFF:
            case 0xDC00:
            case 0xDF80:
            case 0xDFFF:
                cp = UNICODE_BOGUS_CHAR_CODEPOINT;
        } /* switch */
    } /* else */

    /* Do the encoding... */
    if (cp < 0x80)
    {
        *(dst++) = (char) cp;
        len--;
    } /* if */

    else if (cp < 0x800)
    {
        if (len < 2)
            len = 0;
        else
        {
            *(dst++) = (char) ((cp >> 6) | 128 | 64);
            *(dst++) = (char) (cp & 0x3F) | 128;
            len -= 2;
        } /* else */
    } /* else if */

    else if (cp < 0x10000)
    {
        if (len < 3)
            len = 0;
        else
        {
            *(dst++) = (char) ((cp >> 12) | 128 | 64 | 32);
            *(dst++) = (char) ((cp >> 6) & 0x3F) | 128;
            *(dst++) = (char) (cp & 0x3F) | 128;
            len -= 3;
        } /* else */
    } /* else if */

    else
    {
        if (len < 4)
            len = 0;
        else
        {
            *(dst++) = (char) ((cp >> 18) | 128 | 64 | 32 | 16);
            *(dst++) = (char) ((cp >> 12) & 0x3F) | 128;
            *(dst++) = (char) ((cp >> 6) & 0x3F) | 128;
            *(dst++) = (char) (cp & 0x3F) | 128;
            len -= 4;
        } /* else if */
    } /* else */

    *_dst = dst;
    *_len = len;
} /* utf8fromcodepoint */

#define UTF8FROMTYPE(typ, src, dst, len) \
    if (len == 0) return; \
    len--;  \
    while (len) \
    { \
        const PHYSFS_uint32 cp = (PHYSFS_uint32) ((typ) (*(src++))); \
        if (cp == 0) break; \
        utf8fromcodepoint(cp, &dst, &len); \
    } \
    *dst = '\0'; \

void PHYSFS_utf8FromUcs4(const PHYSFS_uint32 *src, char *dst, PHYSFS_uint64 len)
{
    UTF8FROMTYPE(PHYSFS_uint32, src, dst, len);
} /* PHYSFS_utf8FromUcs4 */

void PHYSFS_utf8FromUcs2(const PHYSFS_uint16 *src, char *dst, PHYSFS_uint64 len)
{
    UTF8FROMTYPE(PHYSFS_uint64, src, dst, len);
} /* PHYSFS_utf8FromUcs2 */

/* latin1 maps to unicode codepoints directly, we just utf-8 encode it. */
void PHYSFS_utf8FromLatin1(const char *src, char *dst, PHYSFS_uint64 len)
{
    UTF8FROMTYPE(PHYSFS_uint8, src, dst, len);
} /* PHYSFS_utf8FromLatin1 */

#undef UTF8FROMTYPE


void PHYSFS_utf8FromUtf16(const PHYSFS_uint16 *src, char *dst, PHYSFS_uint64 len)
{
    if (len == 0)
        return;

    len--;
    while (len)
    {
        const PHYSFS_uint32 cp = utf16codepoint(&src);
        if (!cp)
            break;
        utf8fromcodepoint(cp, &dst, &len);
    } /* while */

    *dst = '\0';
} /* PHYSFS_utf8FromUtf16 */


int PHYSFS_caseFold(const PHYSFS_uint32 from, PHYSFS_uint32 *to)
{
    int i;

    if (from < 128)  /* low-ASCII, easy! */
    {
        if ((from >= 'A') && (from <= 'Z'))
            *to = from - ('A' - 'a');
        else
            *to = from;
        return 1;
    } /* if */

    else if (from <= 0xFFFF)
    {
        const PHYSFS_uint8 hash = ((from ^ (from >> 8)) & 0xFF);
        const PHYSFS_uint16 from16 = (PHYSFS_uint16) from;

        {
            const CaseFoldHashBucket1_16 *bucket = &case_fold_hash1_16[hash];
            const int count = (int) bucket->count;
            for (i = 0; i < count; i++)
            {
                const CaseFoldMapping1_16 *mapping = &bucket->list[i];
                if (mapping->from == from16)
                {
                    *to = mapping->to0;
                    return 1;
                } /* if */
            } /* for */
        }

        {
            const CaseFoldHashBucket2_16 *bucket = &case_fold_hash2_16[hash & 15];
            const int count = (int) bucket->count;
            for (i = 0; i < count; i++)
            {
                const CaseFoldMapping2_16 *mapping = &bucket->list[i];
                if (mapping->from == from16)
                {
                    to[0] = mapping->to0;
                    to[1] = mapping->to1;
                    return 2;
                } /* if */
            } /* for */
        }

        {
            const CaseFoldHashBucket3_16 *bucket = &case_fold_hash3_16[hash & 3];
            const int count = (int) bucket->count;
            for (i = 0; i < count; i++)
            {
                const CaseFoldMapping3_16 *mapping = &bucket->list[i];
                if (mapping->from == from16)
                {
                    to[0] = mapping->to0;
                    to[1] = mapping->to1;
                    to[2] = mapping->to2;
                    return 3;
                } /* if */
            } /* for */
        }
    } /* else if */

    else  /* codepoint that doesn't fit in 16 bits. */
    {
        const PHYSFS_uint8 hash = ((from ^ (from >> 8)) & 0xFF);
        const CaseFoldHashBucket1_32 *bucket = &case_fold_hash1_32[hash & 15];
        const int count = (int) bucket->count;
        for (i = 0; i < count; i++)
        {
            const CaseFoldMapping1_32 *mapping = &bucket->list[i];
            if (mapping->from == from)
            {
                *to = mapping->to0;
                return 1;
            } /* if */
        } /* for */
    } /* else */


    /* Not found...there's no remapping for this codepoint. */
    *to = from;
    return 1;
} /* PHYSFS_caseFold */


#define UTFSTRICMP(bits) \
    PHYSFS_uint32 folded1[3], folded2[3]; \
    int head1 = 0, tail1 = 0, head2 = 0, tail2 = 0; \
    while (1) { \
        PHYSFS_uint32 cp1, cp2; \
        if (head1 != tail1) { \
            cp1 = folded1[tail1++]; \
        } else { \
            head1 = PHYSFS_caseFold(utf##bits##codepoint(&str1), folded1); \
            cp1 = folded1[0]; \
            tail1 = 1; \
        } \
        if (head2 != tail2) { \
            cp2 = folded2[tail2++]; \
        } else { \
            head2 = PHYSFS_caseFold(utf##bits##codepoint(&str2), folded2); \
            cp2 = folded2[0]; \
            tail2 = 1; \
        } \
        if (cp1 < cp2) { \
            return -1; \
        } else if (cp1 > cp2) { \
            return 1; \
        } else if (cp1 == 0) { \
            break;  /* complete match. */ \
        } \
    } \
    return 0

int PHYSFS_utf8stricmp(const char *str1, const char *str2)
{
    UTFSTRICMP(8);
} /* PHYSFS_utf8stricmp */

int PHYSFS_utf16stricmp(const PHYSFS_uint16 *str1, const PHYSFS_uint16 *str2)
{
    UTFSTRICMP(16);
} /* PHYSFS_utf16stricmp */

int PHYSFS_ucs4stricmp(const PHYSFS_uint32 *str1, const PHYSFS_uint32 *str2)
{
    UTFSTRICMP(32);
} /* PHYSFS_ucs4stricmp */

#undef UTFSTRICMP

/* end of physfs_unicode.c ... */


/*
 * Read an unsigned 64-bit int and swap to native byte order.
 */
static int readui64(PHYSFS_Io *io, PHYSFS_uint64 *val)
{
    PHYSFS_uint64 v;
    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &v, sizeof (v)), 0);
    *val = PHYSFS_swapULE64(v);
    return 1;
} /* readui64 */

/*
 * Read an unsigned 32-bit int and swap to native byte order.
 */
static int readui32(PHYSFS_Io *io, PHYSFS_uint32 *val)
{
    PHYSFS_uint32 v;
    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &v, sizeof (v)), 0);
    *val = PHYSFS_swapULE32(v);
    return 1;
} /* readui32 */


/*
 * 7zip support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file was written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_7Z

/*#include "physfs_lzmasdk.h"*/
#ifndef _INCLUDE_PHYSFS_LZMASDK_H_
#define _INCLUDE_PHYSFS_LZMASDK_H_

/* This is just a bunch of the LZMA SDK mushed together into one header.
This code is all public domain, and mostly (if not entirely) written by
Igor Pavlov. http://www.7-zip.org/sdk.html
--ryan. */



/* 7zTypes.h -- Basic types
2013-11-12 : Igor Pavlov : Public domain */

#ifndef __7Z_TYPES_H
#define __7Z_TYPES_H

#ifdef _WIN32
/* #include <windows.h> */
#endif

#include <stddef.h>

#ifndef EXTERN_C_BEGIN
#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif
#endif

EXTERN_C_BEGIN

#define SZ_OK 0

#define SZ_ERROR_DATA 1
#define SZ_ERROR_MEM 2
#define SZ_ERROR_CRC 3
#define SZ_ERROR_UNSUPPORTED 4
#define SZ_ERROR_PARAM 5
#define SZ_ERROR_INPUT_EOF 6
#define SZ_ERROR_OUTPUT_EOF 7
#define SZ_ERROR_READ 8
#define SZ_ERROR_WRITE 9
#define SZ_ERROR_PROGRESS 10
#define SZ_ERROR_FAIL 11
#define SZ_ERROR_THREAD 12

#define SZ_ERROR_ARCHIVE 16
#define SZ_ERROR_NO_ARCHIVE 17

typedef int SRes;

#ifdef _WIN32
/* typedef DWORD WRes; */
typedef unsigned WRes;
#else
typedef int WRes;
#endif

#ifndef RINOK
#define RINOK(x) { int __result__ = (x); if (__result__ != 0) return __result__; }
#endif

typedef unsigned char Byte;
typedef short Int16;
typedef unsigned short UInt16;

#ifdef _LZMA_UINT32_IS_ULONG
typedef long Int32;
typedef unsigned long UInt32;
#else
typedef int Int32;
typedef unsigned int UInt32;
#endif

#ifdef _SZ_NO_INT_64

/* define _SZ_NO_INT_64, if your compiler doesn't support 64-bit integers.
   NOTES: Some code will work incorrectly in that case! */

typedef long Int64;
typedef unsigned long UInt64;

#else

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#define UINT64_CONST(n) n ## ui64
#else
typedef long long int Int64;
typedef unsigned long long int UInt64;
#define UINT64_CONST(n) n ## ULL
#endif

#endif

#ifdef _LZMA_NO_SYSTEM_SIZE_T
typedef UInt32 SizeT;
#else
typedef size_t SizeT;
#endif

typedef int Bool;
#define True 1
#define False 0


#ifdef _WIN32
#define MY_STD_CALL __stdcall
#else
#define MY_STD_CALL
#endif

#ifdef _MSC_VER

#if _MSC_VER >= 1300
#define MY_NO_INLINE __declspec(noinline)
#else
#define MY_NO_INLINE
#endif

#define MY_CDECL __cdecl
#define MY_FAST_CALL __fastcall

#else

#define MY_NO_INLINE
#define MY_CDECL
#define MY_FAST_CALL

#endif


/* The following interfaces use first parameter as pointer to structure */

typedef struct
{
  Byte (*Read)(void *p); /* reads one byte, returns 0 in case of EOF or error */
} IByteIn;

typedef struct
{
  void (*Write)(void *p, Byte b);
} IByteOut;

typedef struct
{
  SRes (*Read)(void *p, void *buf, size_t *size);
    /* if (input(*size) != 0 && output(*size) == 0) means end_of_stream.
       (output(*size) < input(*size)) is allowed */
} ISeqInStream;

typedef struct
{
  size_t (*Write)(void *p, const void *buf, size_t size);
    /* Returns: result - the number of actually written bytes.
       (result < size) means error */
} ISeqOutStream;

typedef enum
{
  SZ_SEEK_SET = 0,
  SZ_SEEK_CUR = 1,
  SZ_SEEK_END = 2
} ESzSeek;

typedef struct
{
  SRes (*Read)(void *p, void *buf, size_t *size);  /* same as ISeqInStream::Read */
  SRes (*Seek)(void *p, Int64 *pos, ESzSeek origin);
} ISeekInStream;

typedef struct
{
  SRes (*Look)(void *p, const void **buf, size_t *size);
    /* if (input(*size) != 0 && output(*size) == 0) means end_of_stream.
       (output(*size) > input(*size)) is not allowed
       (output(*size) < input(*size)) is allowed */
  SRes (*Skip)(void *p, size_t offset);
    /* offset must be <= output(*size) of Look */

  SRes (*Read)(void *p, void *buf, size_t *size);
    /* reads directly (without buffer). It's same as ISeqInStream::Read */
  SRes (*Seek)(void *p, Int64 *pos, ESzSeek origin);
} ILookInStream;

static SRes LookInStream_SeekTo(ILookInStream *stream, UInt64 offset);

/* reads via ILookInStream::Read */
static SRes LookInStream_Read2(ILookInStream *stream, void *buf, size_t size, SRes errorType);
static SRes LookInStream_Read(ILookInStream *stream, void *buf, size_t size);

#define LookToRead_BUF_SIZE (1 << 14)

typedef struct
{
  ILookInStream s;
  ISeekInStream *realStream;
  size_t pos;
  size_t size;
  Byte buf[LookToRead_BUF_SIZE];
} CLookToRead;

static void LookToRead_CreateVTable(CLookToRead *p, int lookahead);
static void LookToRead_Init(CLookToRead *p);

typedef struct
{
  ISeqInStream s;
  ILookInStream *realStream;
} CSecToLook;

typedef struct
{
  ISeqInStream s;
  ILookInStream *realStream;
} CSecToRead;

typedef struct
{
  SRes (*Progress)(void *p, UInt64 inSize, UInt64 outSize);
    /* Returns: result. (result != SZ_OK) means break.
       Value (UInt64)(Int64)-1 for size means unknown value. */
} ICompressProgress;

typedef struct
{
  void *(*Alloc)(void *p, size_t size);
  void (*Free)(void *p, void *address); /* address can be 0 */
} ISzAlloc;

#define IAlloc_Alloc(p, size) (p)->Alloc((p), size)
#define IAlloc_Free(p, a) (p)->Free((p), a)

#ifdef _WIN32

#define CHAR_PATH_SEPARATOR '\\'
#define WCHAR_PATH_SEPARATOR L'\\'
#define STRING_PATH_SEPARATOR "\\"
#define WSTRING_PATH_SEPARATOR L"\\"

#else

#define CHAR_PATH_SEPARATOR '/'
#define WCHAR_PATH_SEPARATOR L'/'
#define STRING_PATH_SEPARATOR "/"
#define WSTRING_PATH_SEPARATOR L"/"

#endif

EXTERN_C_END

#endif

/* 7z.h -- 7z interface
2015-11-18 : Igor Pavlov : Public domain */

#ifndef __7Z_H
#define __7Z_H

/*#include "7zTypes.h"*/

EXTERN_C_BEGIN

#define k7zStartHeaderSize 0x20
#define k7zSignatureSize 6

static const Byte k7zSignature[k7zSignatureSize] = {'7', 'z', 0xBC, 0xAF, 0x27, 0x1C};

typedef struct
{
  const Byte *Data;
  size_t Size;
} CSzData;

/* CSzCoderInfo & CSzFolder support only default methods */

typedef struct
{
  size_t PropsOffset;
  UInt32 MethodID;
  Byte NumStreams;
  Byte PropsSize;
} CSzCoderInfo;

typedef struct
{
  UInt32 InIndex;
  UInt32 OutIndex;
} CSzBond;

#define SZ_NUM_CODERS_IN_FOLDER_MAX 4
#define SZ_NUM_BONDS_IN_FOLDER_MAX 3
#define SZ_NUM_PACK_STREAMS_IN_FOLDER_MAX 4

typedef struct
{
  UInt32 NumCoders;
  UInt32 NumBonds;
  UInt32 NumPackStreams;
  UInt32 UnpackStream;
  UInt32 PackStreams[SZ_NUM_PACK_STREAMS_IN_FOLDER_MAX];
  CSzBond Bonds[SZ_NUM_BONDS_IN_FOLDER_MAX];
  CSzCoderInfo Coders[SZ_NUM_CODERS_IN_FOLDER_MAX];
} CSzFolder;


static SRes SzGetNextFolderItem(CSzFolder *f, CSzData *sd);

typedef struct
{
  UInt32 Low;
  UInt32 High;
} CNtfsFileTime;

typedef struct
{
  Byte *Defs; /* MSB 0 bit numbering */
  UInt32 *Vals;
} CSzBitUi32s;

typedef struct
{
  Byte *Defs; /* MSB 0 bit numbering */
  /* UInt64 *Vals; */
  CNtfsFileTime *Vals;
} CSzBitUi64s;

#define SzBitArray_Check(p, i) (((p)[(i) >> 3] & (0x80 >> ((i) & 7))) != 0)

#define SzBitWithVals_Check(p, i) ((p)->Defs && ((p)->Defs[(i) >> 3] & (0x80 >> ((i) & 7))) != 0)

typedef struct
{
  UInt32 NumPackStreams;
  UInt32 NumFolders;

  UInt64 *PackPositions;          /* NumPackStreams + 1 */
  CSzBitUi32s FolderCRCs;         /* NumFolders */

  size_t *FoCodersOffsets;        /* NumFolders + 1 */
  UInt32 *FoStartPackStreamIndex; /* NumFolders + 1 */
  UInt32 *FoToCoderUnpackSizes;   /* NumFolders + 1 */
  Byte *FoToMainUnpackSizeIndex;  /* NumFolders */
  UInt64 *CoderUnpackSizes;       /* for all coders in all folders */

  Byte *CodersData;
} CSzAr;

static UInt64 SzAr_GetFolderUnpackSize(const CSzAr *p, UInt32 folderIndex);

static SRes SzAr_DecodeFolder(const CSzAr *p, UInt32 folderIndex,
    ILookInStream *stream, UInt64 startPos,
    Byte *outBuffer, size_t outSize,
    ISzAlloc *allocMain);

typedef struct
{
  CSzAr db;

  UInt64 startPosAfterHeader;
  UInt64 dataPos;

  UInt32 NumFiles;

  UInt64 *UnpackPositions;  /* NumFiles + 1 */
  /* Byte *IsEmptyFiles; */
  Byte *IsDirs;
  CSzBitUi32s CRCs;

  CSzBitUi32s Attribs;
  /* CSzBitUi32s Parents; */
  CSzBitUi64s MTime;
  CSzBitUi64s CTime;

  UInt32 *FolderToFile;   /* NumFolders + 1 */
  UInt32 *FileToFolder;   /* NumFiles */

  size_t *FileNameOffsets; /* in 2-byte steps */
  Byte *FileNames;  /* UTF-16-LE */
} CSzArEx;

#define SzArEx_IsDir(p, i) (SzBitArray_Check((p)->IsDirs, i))

#define SzArEx_GetFileSize(p, i) ((p)->UnpackPositions[(i) + 1] - (p)->UnpackPositions[i])

static void SzArEx_Init(CSzArEx *p);
static void SzArEx_Free(CSzArEx *p, ISzAlloc *alloc);

/*
if dest == NULL, the return value specifies the required size of the buffer,
  in 16-bit characters, including the null-terminating character.
if dest != NULL, the return value specifies the number of 16-bit characters that
  are written to the dest, including the null-terminating character. */

static size_t SzArEx_GetFileNameUtf16(const CSzArEx *p, size_t fileIndex, UInt16 *dest);

/*
size_t SzArEx_GetFullNameLen(const CSzArEx *p, size_t fileIndex);
UInt16 *SzArEx_GetFullNameUtf16_Back(const CSzArEx *p, size_t fileIndex, UInt16 *dest);
*/



/*
  SzArEx_Extract extracts file from archive

  *outBuffer must be 0 before first call for each new archive.

  Extracting cache:
    If you need to decompress more than one file, you can send
    these values from previous call:
      *blockIndex,
      *outBuffer,
      *outBufferSize
    You can consider "*outBuffer" as cache of solid block. If your archive is solid,
    it will increase decompression speed.

    If you use external function, you can declare these 3 cache variables
    (blockIndex, outBuffer, outBufferSize) as static in that external function.

    Free *outBuffer and set *outBuffer to 0, if you want to flush cache.
*/

static SRes SzArEx_Extract(
    const CSzArEx *db,
    ILookInStream *inStream,
    UInt32 fileIndex,         /* index of file */
    UInt32 *blockIndex,       /* index of solid block */
    Byte **outBuffer,         /* pointer to pointer to output buffer (allocated with allocMain) */
    size_t *outBufferSize,    /* buffer size for output buffer */
    size_t *offset,           /* offset of stream for required file in *outBuffer */
    size_t *outSizeProcessed, /* size of file in *outBuffer */
    ISzAlloc *allocMain,
    ISzAlloc *allocTemp);


/*
SzArEx_Open Errors:
SZ_ERROR_NO_ARCHIVE
SZ_ERROR_ARCHIVE
SZ_ERROR_UNSUPPORTED
SZ_ERROR_MEM
SZ_ERROR_CRC
SZ_ERROR_INPUT_EOF
SZ_ERROR_FAIL
*/

static SRes SzArEx_Open(CSzArEx *p, ILookInStream *inStream,
    ISzAlloc *allocMain, ISzAlloc *allocTemp);

EXTERN_C_END

#endif

/* 7zCrc.h -- CRC32 calculation
2013-01-18 : Igor Pavlov : Public domain */

#ifndef __7Z_CRC_H
#define __7Z_CRC_H

/*#include "7zTypes.h" */

EXTERN_C_BEGIN

/* Call CrcGenerateTable one time before other CRC functions */
static void MY_FAST_CALL CrcGenerateTable(void);

#define CRC_INIT_VAL 0xFFFFFFFF
#define CRC_GET_DIGEST(crc) ((crc) ^ CRC_INIT_VAL)
#define CRC_UPDATE_BYTE(crc, b) (g_CrcTable[((crc) ^ (b)) & 0xFF] ^ ((crc) >> 8))

static UInt32 MY_FAST_CALL CrcCalc(const void *data, size_t size);

EXTERN_C_END

#endif

/* CpuArch.h -- CPU specific code
2016-06-09: Igor Pavlov : Public domain */

#ifndef __CPU_ARCH_H
#define __CPU_ARCH_H

/*#include "7zTypes.h"*/

EXTERN_C_BEGIN

/*
MY_CPU_LE means that CPU is LITTLE ENDIAN.
MY_CPU_BE means that CPU is BIG ENDIAN.
If MY_CPU_LE and MY_CPU_BE are not defined, we don't know about ENDIANNESS of platform.

MY_CPU_LE_UNALIGN means that CPU is LITTLE ENDIAN and CPU supports unaligned memory accesses.
*/

#if defined(_M_X64) \
   || defined(_M_AMD64) \
   || defined(__x86_64__) \
   || defined(__AMD64__) \
   || defined(__amd64__)
  #define MY_CPU_AMD64
#endif

#if defined(MY_CPU_AMD64) \
    || defined(_M_ARM64) \
    || defined(_M_IA64) \
    || defined(__AARCH64EL__) \
    || defined(__AARCH64EB__)
  #define MY_CPU_64BIT
#endif

#if defined(_M_IX86) || defined(__i386__)
#define MY_CPU_X86
#endif

#if defined(MY_CPU_X86) || defined(MY_CPU_AMD64)
#define MY_CPU_X86_OR_AMD64
#endif

#if defined(MY_CPU_X86) \
    || defined(_M_ARM) \
    || defined(__ARMEL__) \
    || defined(__THUMBEL__) \
    || defined(__ARMEB__) \
    || defined(__THUMBEB__)
  #define MY_CPU_32BIT
#endif

#if defined(_WIN32) && defined(_M_ARM)
#define MY_CPU_ARM_LE
#elif defined(_WIN64) && defined(_M_ARM64)
#define MY_CPU_ARM_LE
#endif

#if defined(_WIN32) && defined(_M_IA64)
#define MY_CPU_IA64_LE
#endif

#if defined(MY_CPU_X86_OR_AMD64) \
    || defined(MY_CPU_ARM_LE) \
    || defined(MY_CPU_IA64_LE) \
    || defined(__LITTLE_ENDIAN__) \
    || defined(__ARMEL__) \
    || defined(__THUMBEL__) \
    || defined(__AARCH64EL__) \
    || defined(__MIPSEL__) \
    || defined(__MIPSEL) \
    || defined(_MIPSEL) \
    || defined(__BFIN__) \
    || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
  #define MY_CPU_LE
#endif

#if defined(__BIG_ENDIAN__) \
    || defined(__ARMEB__) \
    || defined(__THUMBEB__) \
    || defined(__AARCH64EB__) \
    || defined(__MIPSEB__) \
    || defined(__MIPSEB) \
    || defined(_MIPSEB) \
    || defined(__m68k__) \
    || defined(__s390__) \
    || defined(__s390x__) \
    || defined(__zarch__) \
    || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
  #define MY_CPU_BE
#endif

#if defined(MY_CPU_LE) && defined(MY_CPU_BE)
Stop_Compiling_Bad_Endian
#endif


#ifdef MY_CPU_LE
  #if defined(MY_CPU_X86_OR_AMD64) \
      /* || defined(__AARCH64EL__) */
    /*#define MY_CPU_LE_UNALIGN*/
  #endif
#endif


#ifdef MY_CPU_LE_UNALIGN

#define GetUi16(p) (*(const UInt16 *)(const void *)(p))
#define GetUi32(p) (*(const UInt32 *)(const void *)(p))
#define GetUi64(p) (*(const UInt64 *)(const void *)(p))

#define SetUi16(p, v) { *(UInt16 *)(p) = (v); }
#define SetUi32(p, v) { *(UInt32 *)(p) = (v); }
#define SetUi64(p, v) { *(UInt64 *)(p) = (v); }

#else

#define GetUi16(p) ( (UInt16) ( \
             ((const Byte *)(p))[0] | \
    ((UInt16)((const Byte *)(p))[1] << 8) ))

#define GetUi32(p) ( \
             ((const Byte *)(p))[0]        | \
    ((UInt32)((const Byte *)(p))[1] <<  8) | \
    ((UInt32)((const Byte *)(p))[2] << 16) | \
    ((UInt32)((const Byte *)(p))[3] << 24))

#define GetUi64(p) (GetUi32(p) | ((UInt64)GetUi32(((const Byte *)(p)) + 4) << 32))

#define SetUi16(p, v) { Byte *_ppp_ = (Byte *)(p); UInt32 _vvv_ = (v); \
    _ppp_[0] = (Byte)_vvv_; \
    _ppp_[1] = (Byte)(_vvv_ >> 8); }

#define SetUi32(p, v) { Byte *_ppp_ = (Byte *)(p); UInt32 _vvv_ = (v); \
    _ppp_[0] = (Byte)_vvv_; \
    _ppp_[1] = (Byte)(_vvv_ >> 8); \
    _ppp_[2] = (Byte)(_vvv_ >> 16); \
    _ppp_[3] = (Byte)(_vvv_ >> 24); }

#define SetUi64(p, v) { Byte *_ppp2_ = (Byte *)(p); UInt64 _vvv2_ = (v); \
    SetUi32(_ppp2_    , (UInt32)_vvv2_); \
    SetUi32(_ppp2_ + 4, (UInt32)(_vvv2_ >> 32)); }

#endif


#if defined(MY_CPU_LE_UNALIGN) && /* defined(_WIN64) && */ (_MSC_VER >= 1300)

/* Note: we use bswap instruction, that is unsupported in 386 cpu */

#include <stdlib.h>

#pragma intrinsic(_byteswap_ulong)
#pragma intrinsic(_byteswap_uint64)
#define GetBe32(p) _byteswap_ulong(*(const UInt32 *)(const Byte *)(p))
#define GetBe64(p) _byteswap_uint64(*(const UInt64 *)(const Byte *)(p))

#define SetBe32(p, v) (*(UInt32 *)(void *)(p)) = _byteswap_ulong(v)

#elif defined(MY_CPU_LE_UNALIGN) && defined (__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))

#define GetBe32(p) __builtin_bswap32(*(const UInt32 *)(const Byte *)(p))
#define GetBe64(p) __builtin_bswap64(*(const UInt64 *)(const Byte *)(p))

#define SetBe32(p, v) (*(UInt32 *)(void *)(p)) = __builtin_bswap32(v)

#else

#define GetBe32(p) ( \
    ((UInt32)((const Byte *)(p))[0] << 24) | \
    ((UInt32)((const Byte *)(p))[1] << 16) | \
    ((UInt32)((const Byte *)(p))[2] <<  8) | \
             ((const Byte *)(p))[3] )

#define GetBe64(p) (((UInt64)GetBe32(p) << 32) | GetBe32(((const Byte *)(p)) + 4))

#define SetBe32(p, v) { Byte *_ppp_ = (Byte *)(p); UInt32 _vvv_ = (v); \
    _ppp_[0] = (Byte)(_vvv_ >> 24); \
    _ppp_[1] = (Byte)(_vvv_ >> 16); \
    _ppp_[2] = (Byte)(_vvv_ >> 8); \
    _ppp_[3] = (Byte)_vvv_; }

#endif


#define GetBe16(p) ( (UInt16) ( \
    ((UInt16)((const Byte *)(p))[0] << 8) | \
             ((const Byte *)(p))[1] ))



#ifdef MY_CPU_X86_OR_AMD64

typedef struct
{
  UInt32 maxFunc;
  UInt32 vendor[3];
  UInt32 ver;
  UInt32 b;
  UInt32 c;
  UInt32 d;
} Cx86cpuid;

enum
{
  CPU_FIRM_INTEL,
  CPU_FIRM_AMD,
  CPU_FIRM_VIA
};

static void MyCPUID(UInt32 function, UInt32 *a, UInt32 *b, UInt32 *c, UInt32 *d);

static Bool x86cpuid_CheckAndRead(Cx86cpuid *p);
static int x86cpuid_GetFirm(const Cx86cpuid *p);

#define x86cpuid_GetFamily(ver) (((ver >> 16) & 0xFF0) | ((ver >> 8) & 0xF))
#define x86cpuid_GetModel(ver)  (((ver >> 12) &  0xF0) | ((ver >> 4) & 0xF))
#define x86cpuid_GetStepping(ver) (ver & 0xF)

static Bool CPU_Is_InOrder();

#endif

EXTERN_C_END

#endif

/* 7zBuf.h -- Byte Buffer
2013-01-18 : Igor Pavlov : Public domain */

#ifndef __7Z_BUF_H
#define __7Z_BUF_H

/*#include "7zTypes.h" */

EXTERN_C_BEGIN

typedef struct
{
  Byte *data;
  size_t size;
} CBuf;

static void Buf_Init(CBuf *p);
static int Buf_Create(CBuf *p, size_t size, ISzAlloc *alloc);
static void Buf_Free(CBuf *p, ISzAlloc *alloc);

EXTERN_C_END

#endif


/* Bcj2.h -- BCJ2 Converter for x86 code
2014-11-10 : Igor Pavlov : Public domain */

#ifndef __BCJ2_H
#define __BCJ2_H

/*#include "7zTypes.h" */

EXTERN_C_BEGIN

#define BCJ2_NUM_STREAMS 4

enum
{
  BCJ2_STREAM_MAIN,
  BCJ2_STREAM_CALL,
  BCJ2_STREAM_JUMP,
  BCJ2_STREAM_RC
};

enum
{
  BCJ2_DEC_STATE_ORIG_0 = BCJ2_NUM_STREAMS,
  BCJ2_DEC_STATE_ORIG_1,
  BCJ2_DEC_STATE_ORIG_2,
  BCJ2_DEC_STATE_ORIG_3,

  BCJ2_DEC_STATE_ORIG,
  BCJ2_DEC_STATE_OK
};

enum
{
  BCJ2_ENC_STATE_ORIG = BCJ2_NUM_STREAMS,
  BCJ2_ENC_STATE_OK
};


#define BCJ2_IS_32BIT_STREAM(s) ((s) == BCJ2_STREAM_CALL || (s) == BCJ2_STREAM_JUMP)

/*
CBcj2Dec / CBcj2Enc
bufs sizes:
  BUF_SIZE(n) = lims[n] - bufs[n]
bufs sizes for BCJ2_STREAM_CALL and BCJ2_STREAM_JUMP must be mutliply of 4:
    (BUF_SIZE(BCJ2_STREAM_CALL) & 3) == 0
    (BUF_SIZE(BCJ2_STREAM_JUMP) & 3) == 0
*/

/*
CBcj2Dec:
dest is allowed to overlap with bufs[BCJ2_STREAM_MAIN], with the following conditions:
  bufs[BCJ2_STREAM_MAIN] >= dest &&
  bufs[BCJ2_STREAM_MAIN] - dest >= tempReserv +
        BUF_SIZE(BCJ2_STREAM_CALL) +
        BUF_SIZE(BCJ2_STREAM_JUMP)
     tempReserv = 0 : for first call of Bcj2Dec_Decode
     tempReserv = 4 : for any other calls of Bcj2Dec_Decode
  overlap with offset = 1 is not allowed
*/

typedef struct
{
  const Byte *bufs[BCJ2_NUM_STREAMS];
  const Byte *lims[BCJ2_NUM_STREAMS];
  Byte *dest;
  const Byte *destLim;

  unsigned state; /* BCJ2_STREAM_MAIN has more priority than BCJ2_STATE_ORIG */

  UInt32 ip;
  Byte temp[4];
  UInt32 range;
  UInt32 code;
  UInt16 probs[2 + 256];
} CBcj2Dec;

static void Bcj2Dec_Init(CBcj2Dec *p);

/* Returns: SZ_OK or SZ_ERROR_DATA */
static SRes Bcj2Dec_Decode(CBcj2Dec *p);

#define Bcj2Dec_IsFinished(_p_) ((_p_)->code == 0)

#define BCJ2_RELAT_LIMIT_NUM_BITS 26
#define BCJ2_RELAT_LIMIT ((UInt32)1 << BCJ2_RELAT_LIMIT_NUM_BITS)

/* limit for CBcj2Enc::fileSize variable */
#define BCJ2_FileSize_MAX ((UInt32)1 << 31)

EXTERN_C_END

#endif

/* Bra.h -- Branch converters for executables
2013-01-18 : Igor Pavlov : Public domain */

#ifndef __BRA_H
#define __BRA_H

/*#include "7zTypes.h"*/

EXTERN_C_BEGIN

/*
These functions convert relative addresses to absolute addresses
in CALL instructions to increase the compression ratio.

  In:
    data     - data buffer
    size     - size of data
    ip       - current virtual Instruction Pinter (IP) value
    state    - state variable for x86 converter
    encoding - 0 (for decoding), 1 (for encoding)

  Out:
    state    - state variable for x86 converter

  Returns:
    The number of processed bytes. If you call these functions with multiple calls,
    you must start next call with first byte after block of processed bytes.

  Type   Endian  Alignment  LookAhead

  x86    little      1          4
  ARMT   little      2          2
  ARM    little      4          0
  PPC     big        4          0
  SPARC   big        4          0
  IA64   little     16          0

  size must be >= Alignment + LookAhead, if it's not last block.
  If (size < Alignment + LookAhead), converter returns 0.

  Example:

    UInt32 ip = 0;
    for ()
    {
      ; size must be >= Alignment + LookAhead, if it's not last block
      SizeT processed = Convert(data, size, ip, 1);
      data += processed;
      size -= processed;
      ip += processed;
    }
*/

#define x86_Convert_Init(state) { state = 0; }
static SizeT x86_Convert(Byte *data, SizeT size, UInt32 ip, UInt32 *state, int encoding);
static SizeT ARM_Convert(Byte *data, SizeT size, UInt32 ip, int encoding);
static SizeT ARMT_Convert(Byte *data, SizeT size, UInt32 ip, int encoding);
static SizeT PPC_Convert(Byte *data, SizeT size, UInt32 ip, int encoding);
static SizeT SPARC_Convert(Byte *data, SizeT size, UInt32 ip, int encoding);
static SizeT IA64_Convert(Byte *data, SizeT size, UInt32 ip, int encoding);

EXTERN_C_END

#endif

/* Delta.h -- Delta converter
2013-01-18 : Igor Pavlov : Public domain */

#ifndef __DELTA_H
#define __DELTA_H

/*#include "7zTypes.h" */

EXTERN_C_BEGIN

#define DELTA_STATE_SIZE 256

static void Delta_Init(Byte *state);
static void Delta_Decode(Byte *state, unsigned delta, Byte *data, SizeT size);

EXTERN_C_END

#endif

/* LzmaDec.h -- LZMA Decoder
2013-01-18 : Igor Pavlov : Public domain */

#ifndef __LZMA_DEC_H
#define __LZMA_DEC_H

/*#include "7zTypes.h"*/

EXTERN_C_BEGIN

/* #define _LZMA_PROB32 */
/* _LZMA_PROB32 can increase the speed on some CPUs,
   but memory usage for CLzmaDec::probs will be doubled in that case */

#ifdef _LZMA_PROB32
#define CLzmaProb UInt32
#else
#define CLzmaProb UInt16
#endif


/* ---------- LZMA Properties ---------- */

#define LZMA_PROPS_SIZE 5

typedef struct _CLzmaProps
{
  unsigned lc, lp, pb;
  UInt32 dicSize;
} CLzmaProps;

/* LzmaProps_Decode - decodes properties
Returns:
  SZ_OK
  SZ_ERROR_UNSUPPORTED - Unsupported properties
*/

static SRes LzmaProps_Decode(CLzmaProps *p, const Byte *data, unsigned size);


/* ---------- LZMA Decoder state ---------- */

/* LZMA_REQUIRED_INPUT_MAX = number of required input bytes for worst case.
   Num bits = log2((2^11 / 31) ^ 22) + 26 < 134 + 26 = 160; */

#define LZMA_REQUIRED_INPUT_MAX 20

typedef struct
{
  CLzmaProps prop;
  CLzmaProb *probs;
  Byte *dic;
  const Byte *buf;
  UInt32 range, code;
  SizeT dicPos;
  SizeT dicBufSize;
  UInt32 processedPos;
  UInt32 checkDicSize;
  unsigned state;
  UInt32 reps[4];
  unsigned remainLen;
  int needFlush;
  int needInitState;
  UInt32 numProbs;
  unsigned tempBufSize;
  Byte tempBuf[LZMA_REQUIRED_INPUT_MAX];
} CLzmaDec;

#define LzmaDec_Construct(p) { (p)->dic = 0; (p)->probs = 0; }

static void LzmaDec_Init(CLzmaDec *p);

/* There are two types of LZMA streams:
     0) Stream with end mark. That end mark adds about 6 bytes to compressed size.
     1) Stream without end mark. You must know exact uncompressed size to decompress such stream. */

typedef enum
{
  LZMA_FINISH_ANY,   /* finish at any point */
  LZMA_FINISH_END    /* block must be finished at the end */
} ELzmaFinishMode;

/* ELzmaFinishMode has meaning only if the decoding reaches output limit !!!

   You must use LZMA_FINISH_END, when you know that current output buffer
   covers last bytes of block. In other cases you must use LZMA_FINISH_ANY.

   If LZMA decoder sees end marker before reaching output limit, it returns SZ_OK,
   and output value of destLen will be less than output buffer size limit.
   You can check status result also.

   You can use multiple checks to test data integrity after full decompression:
     1) Check Result and "status" variable.
     2) Check that output(destLen) = uncompressedSize, if you know real uncompressedSize.
     3) Check that output(srcLen) = compressedSize, if you know real compressedSize.
        You must use correct finish mode in that case. */

typedef enum
{
  LZMA_STATUS_NOT_SPECIFIED,               /* use main error code instead */
  LZMA_STATUS_FINISHED_WITH_MARK,          /* stream was finished with end mark. */
  LZMA_STATUS_NOT_FINISHED,                /* stream was not finished */
  LZMA_STATUS_NEEDS_MORE_INPUT,            /* you must provide more input bytes */
  LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK  /* there is probability that stream was finished without end mark */
} ELzmaStatus;

/* ELzmaStatus is used only as output value for function call */


/* ---------- Interfaces ---------- */

/* There are 3 levels of interfaces:
     1) Dictionary Interface
     2) Buffer Interface
     3) One Call Interface
   You can select any of these interfaces, but don't mix functions from different
   groups for same object. */


/* There are two variants to allocate state for Dictionary Interface:
     1) LzmaDec_Allocate / LzmaDec_Free
     2) LzmaDec_AllocateProbs / LzmaDec_FreeProbs
   You can use variant 2, if you set dictionary buffer manually.
   For Buffer Interface you must always use variant 1.

LzmaDec_Allocate* can return:
  SZ_OK
  SZ_ERROR_MEM         - Memory allocation error
  SZ_ERROR_UNSUPPORTED - Unsupported properties
*/

static SRes LzmaDec_AllocateProbs(CLzmaDec *p, const Byte *props, unsigned propsSize, ISzAlloc *alloc);
static void LzmaDec_FreeProbs(CLzmaDec *p, ISzAlloc *alloc);

/* ---------- Dictionary Interface ---------- */

/* You can use it, if you want to eliminate the overhead for data copying from
   dictionary to some other external buffer.
   You must work with CLzmaDec variables directly in this interface.

   STEPS:
     LzmaDec_Constr()
     LzmaDec_Allocate()
     for (each new stream)
     {
       LzmaDec_Init()
       while (it needs more decompression)
       {
         LzmaDec_DecodeToDic()
         use data from CLzmaDec::dic and update CLzmaDec::dicPos
       }
     }
     LzmaDec_Free()
*/

/* LzmaDec_DecodeToDic

   The decoding to internal dictionary buffer (CLzmaDec::dic).
   You must manually update CLzmaDec::dicPos, if it reaches CLzmaDec::dicBufSize !!!

finishMode:
  It has meaning only if the decoding reaches output limit (dicLimit).
  LZMA_FINISH_ANY - Decode just dicLimit bytes.
  LZMA_FINISH_END - Stream must be finished after dicLimit.

Returns:
  SZ_OK
    status:
      LZMA_STATUS_FINISHED_WITH_MARK
      LZMA_STATUS_NOT_FINISHED
      LZMA_STATUS_NEEDS_MORE_INPUT
      LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK
  SZ_ERROR_DATA - Data error
*/

static SRes LzmaDec_DecodeToDic(CLzmaDec *p, SizeT dicLimit,
    const Byte *src, SizeT *srcLen, ELzmaFinishMode finishMode, ELzmaStatus *status);

EXTERN_C_END

#endif

/* Lzma2Dec.h -- LZMA2 Decoder
2015-05-13 : Igor Pavlov : Public domain */

#ifndef __LZMA2_DEC_H
#define __LZMA2_DEC_H

/*#include "LzmaDec.h"*/

EXTERN_C_BEGIN

/* ---------- State Interface ---------- */

typedef struct
{
  CLzmaDec decoder;
  UInt32 packSize;
  UInt32 unpackSize;
  unsigned state;
  Byte control;
  Bool needInitDic;
  Bool needInitState;
  Bool needInitProp;
} CLzma2Dec;

#define Lzma2Dec_Construct(p) LzmaDec_Construct(&(p)->decoder)
#define Lzma2Dec_FreeProbs(p, alloc) LzmaDec_FreeProbs(&(p)->decoder, alloc);
#define Lzma2Dec_Free(p, alloc) LzmaDec_Free(&(p)->decoder, alloc);

static SRes Lzma2Dec_AllocateProbs(CLzma2Dec *p, Byte prop, ISzAlloc *alloc);
static void Lzma2Dec_Init(CLzma2Dec *p);


/*
finishMode:
  It has meaning only if the decoding reaches output limit (*destLen or dicLimit).
  LZMA_FINISH_ANY - use smallest number of input bytes
  LZMA_FINISH_END - read EndOfStream marker after decoding

Returns:
  SZ_OK
    status:
      LZMA_STATUS_FINISHED_WITH_MARK
      LZMA_STATUS_NOT_FINISHED
      LZMA_STATUS_NEEDS_MORE_INPUT
  SZ_ERROR_DATA - Data error
*/

static SRes Lzma2Dec_DecodeToDic(CLzma2Dec *p, SizeT dicLimit,
    const Byte *src, SizeT *srcLen, ELzmaFinishMode finishMode, ELzmaStatus *status);


EXTERN_C_END

#endif


/* END HEADERS */


/* 7zCrc.c -- CRC32 init
2015-03-10 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "7zCrc.h"
#include "CpuArch.h"
*/
#define UNUSED_VAR(x) (void)x;

#define kCrcPoly 0xEDB88320

#ifdef MY_CPU_LE
  #define CRC_NUM_TABLES 8
#else
  #define CRC_NUM_TABLES 9

  #define CRC_UINT32_SWAP(v) ((v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24))

  static UInt32 MY_FAST_CALL CrcUpdateT1_BeT4(UInt32 v, const void *data, size_t size, const UInt32 *table);
  static UInt32 MY_FAST_CALL CrcUpdateT1_BeT8(UInt32 v, const void *data, size_t size, const UInt32 *table);
#endif

#ifndef MY_CPU_BE
  static UInt32 MY_FAST_CALL CrcUpdateT4(UInt32 v, const void *data, size_t size, const UInt32 *table);
  static UInt32 MY_FAST_CALL CrcUpdateT8(UInt32 v, const void *data, size_t size, const UInt32 *table);
#endif

typedef UInt32 (MY_FAST_CALL *CRC_FUNC)(UInt32 v, const void *data, size_t size, const UInt32 *table);

static CRC_FUNC g_CrcUpdateT4;
static CRC_FUNC g_CrcUpdateT8;
static CRC_FUNC g_CrcUpdate;

static UInt32 g_CrcTable[256 * CRC_NUM_TABLES];

static UInt32 MY_FAST_CALL CrcCalc(const void *data, size_t size)
{
  return g_CrcUpdate(CRC_INIT_VAL, data, size, g_CrcTable) ^ CRC_INIT_VAL;
}

#define CRC_UPDATE_BYTE_2(crc, b) (table[((crc) ^ (b)) & 0xFF] ^ ((crc) >> 8))

#if CRC_NUM_TABLES < 4
static UInt32 MY_FAST_CALL CrcUpdateT1(UInt32 v, const void *data, size_t size, const UInt32 *table)
{
  const Byte *p = (const Byte *)data;
  const Byte *pEnd = p + size;
  for (; p != pEnd; p++)
    v = CRC_UPDATE_BYTE_2(v, *p);
  return v;
}
#endif

static void MY_FAST_CALL CrcGenerateTable()
{
  UInt32 i;
  for (i = 0; i < 256; i++)
  {
    UInt32 r = i;
    unsigned j;
    for (j = 0; j < 8; j++)
      r = (r >> 1) ^ (kCrcPoly & ~((r & 1) - 1));
    g_CrcTable[i] = r;
  }
  for (; i < 256 * CRC_NUM_TABLES; i++)
  {
    UInt32 r = g_CrcTable[i - 256];
    g_CrcTable[i] = g_CrcTable[r & 0xFF] ^ (r >> 8);
  }

  #if CRC_NUM_TABLES < 4

  g_CrcUpdate = CrcUpdateT1;

  #else

  #ifdef MY_CPU_LE

    g_CrcUpdateT4 = CrcUpdateT4;
    g_CrcUpdate = CrcUpdateT4;

    #if CRC_NUM_TABLES >= 8
      g_CrcUpdateT8 = CrcUpdateT8;

      #ifdef MY_CPU_X86_OR_AMD64
      if (!CPU_Is_InOrder())
        g_CrcUpdate = CrcUpdateT8;
      #endif
    #endif

  #else
  {
    #ifndef MY_CPU_BE
    UInt32 k = 0x01020304;
    const Byte *p = (const Byte *)&k;
    if (p[0] == 4 && p[1] == 3)
    {
      g_CrcUpdateT4 = CrcUpdateT4;
      g_CrcUpdate = CrcUpdateT4;
      #if CRC_NUM_TABLES >= 8
      g_CrcUpdateT8 = CrcUpdateT8;
      /* g_CrcUpdate = CrcUpdateT8; */
      #endif
    }
    else if (p[0] != 1 || p[1] != 2)
      g_CrcUpdate = CrcUpdateT1;
    else
    #endif
    {
      for (i = 256 * CRC_NUM_TABLES - 1; i >= 256; i--)
      {
        UInt32 x = g_CrcTable[i - 256];
        g_CrcTable[i] = CRC_UINT32_SWAP(x);
      }
      g_CrcUpdateT4 = CrcUpdateT1_BeT4;
      g_CrcUpdate = CrcUpdateT1_BeT4;
      #if CRC_NUM_TABLES >= 8
      g_CrcUpdateT8 = CrcUpdateT1_BeT8;
      /* g_CrcUpdate = CrcUpdateT1_BeT8; */
      #endif
    }
  }
  #endif

  #endif
}

/* 7zCrcOpt.c -- CRC32 calculation
2015-03-01 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "CpuArch.h"
*/

#ifndef MY_CPU_BE

#define CRC_UPDATE_BYTE_2(crc, b) (table[((crc) ^ (b)) & 0xFF] ^ ((crc) >> 8))

static UInt32 MY_FAST_CALL CrcUpdateT4(UInt32 v, const void *data, size_t size, const UInt32 *table)
{
  const Byte *p = (const Byte *)data;
  for (; size > 0 && ((unsigned)(ptrdiff_t)p & 3) != 0; size--, p++)
    v = CRC_UPDATE_BYTE_2(v, *p);
  for (; size >= 4; size -= 4, p += 4)
  {
    v ^= *(const UInt32 *)p;
    v =
          table[0x300 + ((v      ) & 0xFF)]
        ^ table[0x200 + ((v >>  8) & 0xFF)]
        ^ table[0x100 + ((v >> 16) & 0xFF)]
        ^ table[0x000 + ((v >> 24))];
  }
  for (; size > 0; size--, p++)
    v = CRC_UPDATE_BYTE_2(v, *p);
  return v;
}

static UInt32 MY_FAST_CALL CrcUpdateT8(UInt32 v, const void *data, size_t size, const UInt32 *table)
{
  const Byte *p = (const Byte *)data;
  for (; size > 0 && ((unsigned)(ptrdiff_t)p & 7) != 0; size--, p++)
    v = CRC_UPDATE_BYTE_2(v, *p);
  for (; size >= 8; size -= 8, p += 8)
  {
    UInt32 d;
    v ^= *(const UInt32 *)p;
    v =
          table[0x700 + ((v      ) & 0xFF)]
        ^ table[0x600 + ((v >>  8) & 0xFF)]
        ^ table[0x500 + ((v >> 16) & 0xFF)]
        ^ table[0x400 + ((v >> 24))];
    d = *((const UInt32 *)p + 1);
    v ^=
          table[0x300 + ((d      ) & 0xFF)]
        ^ table[0x200 + ((d >>  8) & 0xFF)]
        ^ table[0x100 + ((d >> 16) & 0xFF)]
        ^ table[0x000 + ((d >> 24))];
  }
  for (; size > 0; size--, p++)
    v = CRC_UPDATE_BYTE_2(v, *p);
  return v;
}

#endif


#ifndef MY_CPU_LE

#define CRC_UINT32_SWAP(v) ((v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24))

#define CRC_UPDATE_BYTE_2_BE(crc, b) (table[(((crc) >> 24) ^ (b))] ^ ((crc) << 8))

static UInt32 MY_FAST_CALL CrcUpdateT1_BeT4(UInt32 v, const void *data, size_t size, const UInt32 *table)
{
  const Byte *p = (const Byte *)data;
  table += 0x100;
  v = CRC_UINT32_SWAP(v);
  for (; size > 0 && ((unsigned)(ptrdiff_t)p & 3) != 0; size--, p++)
    v = CRC_UPDATE_BYTE_2_BE(v, *p);
  for (; size >= 4; size -= 4, p += 4)
  {
    v ^= *(const UInt32 *)p;
    v =
          table[0x000 + ((v      ) & 0xFF)]
        ^ table[0x100 + ((v >>  8) & 0xFF)]
        ^ table[0x200 + ((v >> 16) & 0xFF)]
        ^ table[0x300 + ((v >> 24))];
  }
  for (; size > 0; size--, p++)
    v = CRC_UPDATE_BYTE_2_BE(v, *p);
  return CRC_UINT32_SWAP(v);
}

static UInt32 MY_FAST_CALL CrcUpdateT1_BeT8(UInt32 v, const void *data, size_t size, const UInt32 *table)
{
  const Byte *p = (const Byte *)data;
  table += 0x100;
  v = CRC_UINT32_SWAP(v);
  for (; size > 0 && ((unsigned)(ptrdiff_t)p & 7) != 0; size--, p++)
    v = CRC_UPDATE_BYTE_2_BE(v, *p);
  for (; size >= 8; size -= 8, p += 8)
  {
    UInt32 d;
    v ^= *(const UInt32 *)p;
    v =
          table[0x400 + ((v      ) & 0xFF)]
        ^ table[0x500 + ((v >>  8) & 0xFF)]
        ^ table[0x600 + ((v >> 16) & 0xFF)]
        ^ table[0x700 + ((v >> 24))];
    d = *((const UInt32 *)p + 1);
    v ^=
          table[0x000 + ((d      ) & 0xFF)]
        ^ table[0x100 + ((d >>  8) & 0xFF)]
        ^ table[0x200 + ((d >> 16) & 0xFF)]
        ^ table[0x300 + ((d >> 24))];
  }
  for (; size > 0; size--, p++)
    v = CRC_UPDATE_BYTE_2_BE(v, *p);
  return CRC_UINT32_SWAP(v);
}

#endif

/* CpuArch.c -- CPU specific code
2016-02-25: Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "CpuArch.h"
*/

#ifdef MY_CPU_X86_OR_AMD64

#if (defined(_MSC_VER) && !defined(MY_CPU_AMD64)) || defined(__GNUC__) || defined(__TINYC__)
#define USE_ASM
#endif

#if !defined(USE_ASM) && _MSC_VER >= 1500
#include <intrin.h>
#endif

#if defined(USE_ASM) && !defined(MY_CPU_AMD64)
static UInt32 CheckFlag(UInt32 flag)
{
  #ifdef _MSC_VER
  __asm pushfd;
  __asm pop EAX;
  __asm mov EDX, EAX;
  __asm xor EAX, flag;
  __asm push EAX;
  __asm popfd;
  __asm pushfd;
  __asm pop EAX;
  __asm xor EAX, EDX;
  __asm push EDX;
  __asm popfd;
  __asm and flag, EAX;
  #else
  __asm__ __volatile__ (
    "pushf\n\t"
    "pop  %%EAX\n\t"
    "movl %%EAX,%%EDX\n\t"
    "xorl %0,%%EAX\n\t"
    "push %%EAX\n\t"
    "popf\n\t"
    "pushf\n\t"
    "pop  %%EAX\n\t"
    "xorl %%EDX,%%EAX\n\t"
    "push %%EDX\n\t"
    "popf\n\t"
    "andl %%EAX, %0\n\t":
    "=c" (flag) : "c" (flag) :
    "%eax", "%edx");
  #endif
  return flag;
}
#define CHECK_CPUID_IS_SUPPORTED if (CheckFlag(1 << 18) == 0 || CheckFlag(1 << 21) == 0) return False;
#else
#define CHECK_CPUID_IS_SUPPORTED
#endif

#if defined(__WATCOMC__)
static void __cpuid(int *cpuinfo, const UInt32 infotype);
#pragma aux __cpuid =     \
    ".586"                \
    "cpuid"               \
    "mov  [esi+0],eax"    \
    "mov  [esi+4],ebx"    \
    "mov  [esi+8],ecx"    \
    "mov  [esi+12],edx"   \
    parm [esi] [eax] modify [ebx ecx edx];
#endif


static void MyCPUID(UInt32 function, UInt32 *a, UInt32 *b, UInt32 *c, UInt32 *d)
{
  #ifdef USE_ASM

  #ifdef _MSC_VER

  UInt32 a2, b2, c2, d2;
  __asm xor EBX, EBX;
  __asm xor ECX, ECX;
  __asm xor EDX, EDX;
  __asm mov EAX, function;
  __asm cpuid;
  __asm mov a2, EAX;
  __asm mov b2, EBX;
  __asm mov c2, ECX;
  __asm mov d2, EDX;

  *a = a2;
  *b = b2;
  *c = c2;
  *d = d2;

  #else

  __asm__ __volatile__ (
  #if defined(MY_CPU_AMD64) && defined(__PIC__)
    "mov %%rbx, %%rdi;"
    "cpuid;"
    "xchg %%rbx, %%rdi;"
    : "=a" (*a) ,
      "=D" (*b) ,
  #elif defined(MY_CPU_X86) && defined(__PIC__)
    "mov %%ebx, %%edi;"
    "cpuid;"
    "xchgl %%ebx, %%edi;"
    : "=a" (*a) ,
      "=D" (*b) ,
  #else
    "cpuid"
    : "=a" (*a) ,
      "=b" (*b) ,
  #endif
      "=c" (*c) ,
      "=d" (*d)
    : "0" (function)) ;

  #endif

  #else

  int CPUInfo[4];
  __cpuid(CPUInfo, function);
  *a = CPUInfo[0];
  *b = CPUInfo[1];
  *c = CPUInfo[2];
  *d = CPUInfo[3];

  #endif
}

static Bool x86cpuid_CheckAndRead(Cx86cpuid *p)
{
  CHECK_CPUID_IS_SUPPORTED
  MyCPUID(0, &p->maxFunc, &p->vendor[0], &p->vendor[2], &p->vendor[1]);
  MyCPUID(1, &p->ver, &p->b, &p->c, &p->d);
  return True;
}

static const UInt32 kVendors[][3] =
{
  { 0x756E6547, 0x49656E69, 0x6C65746E},
  { 0x68747541, 0x69746E65, 0x444D4163},
  { 0x746E6543, 0x48727561, 0x736C7561}
};

static int x86cpuid_GetFirm(const Cx86cpuid *p)
{
  unsigned i;
  for (i = 0; i < sizeof(kVendors) / sizeof(kVendors[i]); i++)
  {
    const UInt32 *v = kVendors[i];
    if (v[0] == p->vendor[0] &&
        v[1] == p->vendor[1] &&
        v[2] == p->vendor[2])
      return (int)i;
  }
  return -1;
}

static Bool CPU_Is_InOrder()
{
  Cx86cpuid p;
  int firm;
  UInt32 family, model;
  if (!x86cpuid_CheckAndRead(&p))
    return True;

  family = x86cpuid_GetFamily(p.ver);
  model = x86cpuid_GetModel(p.ver);

  firm = x86cpuid_GetFirm(&p);

  switch (firm)
  {
    case CPU_FIRM_INTEL: return (family < 6 || (family == 6 && (
        /* In-Order Atom CPU */
           model == 0x1C  /* 45 nm, N4xx, D4xx, N5xx, D5xx, 230, 330 */
        || model == 0x26  /* 45 nm, Z6xx */
        || model == 0x27  /* 32 nm, Z2460 */
        || model == 0x35  /* 32 nm, Z2760 */
        || model == 0x36  /* 32 nm, N2xxx, D2xxx */
        )));
    case CPU_FIRM_AMD: return (family < 5 || (family == 5 && (model < 6 || model == 0xA)));
    case CPU_FIRM_VIA: return (family < 6 || (family == 6 && model < 0xF));
  }
  return True;
}

#endif

/* 7zStream.c -- 7z Stream functions
2013-11-12 : Igor Pavlov : Public domain */

/*#include "Precomp.h"*/

#include <string.h>

/*#include "7zTypes.h"*/

static SRes LookInStream_SeekTo(ILookInStream *stream, UInt64 offset)
{
  Int64 t = offset;
  return stream->Seek(stream, &t, SZ_SEEK_SET);
}

static SRes LookInStream_Read2(ILookInStream *stream, void *buf, size_t size, SRes errorType)
{
  while (size != 0)
  {
    size_t processed = size;
    RINOK(stream->Read(stream, buf, &processed));
    if (processed == 0)
      return errorType;
    buf = (void *)((Byte *)buf + processed);
    size -= processed;
  }
  return SZ_OK;
}

static SRes LookInStream_Read(ILookInStream *stream, void *buf, size_t size)
{
  return LookInStream_Read2(stream, buf, size, SZ_ERROR_INPUT_EOF);
}

static SRes LookToRead_Look_Lookahead(void *pp, const void **buf, size_t *size)
{
  SRes res = SZ_OK;
  CLookToRead *p = (CLookToRead *)pp;
  size_t size2 = p->size - p->pos;
  if (size2 == 0 && *size > 0)
  {
    p->pos = 0;
    size2 = LookToRead_BUF_SIZE;
    res = p->realStream->Read(p->realStream, p->buf, &size2);
    p->size = size2;
  }
  if (size2 < *size)
    *size = size2;
  *buf = p->buf + p->pos;
  return res;
}

static SRes LookToRead_Look_Exact(void *pp, const void **buf, size_t *size)
{
  SRes res = SZ_OK;
  CLookToRead *p = (CLookToRead *)pp;
  size_t size2 = p->size - p->pos;
  if (size2 == 0 && *size > 0)
  {
    p->pos = 0;
    if (*size > LookToRead_BUF_SIZE)
      *size = LookToRead_BUF_SIZE;
    res = p->realStream->Read(p->realStream, p->buf, size);
    size2 = p->size = *size;
  }
  if (size2 < *size)
    *size = size2;
  *buf = p->buf + p->pos;
  return res;
}

static SRes LookToRead_Skip(void *pp, size_t offset)
{
  CLookToRead *p = (CLookToRead *)pp;
  p->pos += offset;
  return SZ_OK;
}

static SRes LookToRead_Read(void *pp, void *buf, size_t *size)
{
  CLookToRead *p = (CLookToRead *)pp;
  size_t rem = p->size - p->pos;
  if (rem == 0)
    return p->realStream->Read(p->realStream, buf, size);
  if (rem > *size)
    rem = *size;
  memcpy(buf, p->buf + p->pos, rem);
  p->pos += rem;
  *size = rem;
  return SZ_OK;
}

static SRes LookToRead_Seek(void *pp, Int64 *pos, ESzSeek origin)
{
  CLookToRead *p = (CLookToRead *)pp;
  p->pos = p->size = 0;
  return p->realStream->Seek(p->realStream, pos, origin);
}

static void LookToRead_CreateVTable(CLookToRead *p, int lookahead)
{
  p->s.Look = lookahead ?
      LookToRead_Look_Lookahead :
      LookToRead_Look_Exact;
  p->s.Skip = LookToRead_Skip;
  p->s.Read = LookToRead_Read;
  p->s.Seek = LookToRead_Seek;
}

static void LookToRead_Init(CLookToRead *p)
{
  p->pos = p->size = 0;
}


/* 7zArcIn.c -- 7z Input functions
2016-05-16 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include <string.h>

#include "7z.h"
#include "7zBuf.h"
#include "7zCrc.h"
#include "CpuArch.h"
*/

#define MY_ALLOC(T, p, size, alloc) { \
  if ((p = (T *)IAlloc_Alloc(alloc, (size) * sizeof(T))) == NULL) return SZ_ERROR_MEM; }

#define MY_ALLOC_ZE(T, p, size, alloc) { if ((size) == 0) p = NULL; else MY_ALLOC(T, p, size, alloc) }

#define MY_ALLOC_AND_CPY(to, size, from, alloc) \
  { MY_ALLOC(Byte, to, size, alloc); memcpy(to, from, size); }

#define MY_ALLOC_ZE_AND_CPY(to, size, from, alloc) \
  { if ((size) == 0) p = NULL; else { MY_ALLOC_AND_CPY(to, size, from, alloc) } }

#define k7zMajorVersion 0

enum EIdEnum
{
  k7zIdEnd,
  k7zIdHeader,
  k7zIdArchiveProperties,
  k7zIdAdditionalStreamsInfo,
  k7zIdMainStreamsInfo,
  k7zIdFilesInfo,
  k7zIdPackInfo,
  k7zIdUnpackInfo,
  k7zIdSubStreamsInfo,
  k7zIdSize,
  k7zIdCRC,
  k7zIdFolder,
  k7zIdCodersUnpackSize,
  k7zIdNumUnpackStream,
  k7zIdEmptyStream,
  k7zIdEmptyFile,
  k7zIdAnti,
  k7zIdName,
  k7zIdCTime,
  k7zIdATime,
  k7zIdMTime,
  k7zIdWinAttrib,
  k7zIdComment,
  k7zIdEncodedHeader,
  k7zIdStartPos,
  k7zIdDummy
  /* k7zNtSecure, */
  /* k7zParent, */
  /* k7zIsReal */
};

#define SzBitUi32s_Init(p) { (p)->Defs = NULL; (p)->Vals = NULL; }

static SRes SzBitUi32s_Alloc(CSzBitUi32s *p, size_t num, ISzAlloc *alloc)
{
  if (num == 0)
  {
    p->Defs = NULL;
    p->Vals = NULL;
  }
  else
  {
    MY_ALLOC(Byte, p->Defs, (num + 7) >> 3, alloc);
    MY_ALLOC(UInt32, p->Vals, num, alloc);
  }
  return SZ_OK;
}

static void SzBitUi32s_Free(CSzBitUi32s *p, ISzAlloc *alloc)
{
  IAlloc_Free(alloc, p->Defs); p->Defs = NULL;
  IAlloc_Free(alloc, p->Vals); p->Vals = NULL;
}

#define SzBitUi64s_Init(p) { (p)->Defs = NULL; (p)->Vals = NULL; }

static void SzBitUi64s_Free(CSzBitUi64s *p, ISzAlloc *alloc)
{
  IAlloc_Free(alloc, p->Defs); p->Defs = NULL;
  IAlloc_Free(alloc, p->Vals); p->Vals = NULL;
}


static void SzAr_Init(CSzAr *p)
{
  p->NumPackStreams = 0;
  p->NumFolders = 0;

  p->PackPositions = NULL;
  SzBitUi32s_Init(&p->FolderCRCs);

  p->FoCodersOffsets = NULL;
  p->FoStartPackStreamIndex = NULL;
  p->FoToCoderUnpackSizes = NULL;
  p->FoToMainUnpackSizeIndex = NULL;
  p->CoderUnpackSizes = NULL;

  p->CodersData = NULL;
}

static void SzAr_Free(CSzAr *p, ISzAlloc *alloc)
{
  IAlloc_Free(alloc, p->PackPositions);
  SzBitUi32s_Free(&p->FolderCRCs, alloc);

  IAlloc_Free(alloc, p->FoCodersOffsets);
  IAlloc_Free(alloc, p->FoStartPackStreamIndex);
  IAlloc_Free(alloc, p->FoToCoderUnpackSizes);
  IAlloc_Free(alloc, p->FoToMainUnpackSizeIndex);
  IAlloc_Free(alloc, p->CoderUnpackSizes);

  IAlloc_Free(alloc, p->CodersData);

  SzAr_Init(p);
}


static void SzArEx_Init(CSzArEx *p)
{
  SzAr_Init(&p->db);

  p->NumFiles = 0;
  p->dataPos = 0;

  p->UnpackPositions = NULL;
  p->IsDirs = NULL;

  p->FolderToFile = NULL;
  p->FileToFolder = NULL;

  p->FileNameOffsets = NULL;
  p->FileNames = NULL;

  SzBitUi32s_Init(&p->CRCs);
  SzBitUi32s_Init(&p->Attribs);
  /* SzBitUi32s_Init(&p->Parents); */
  SzBitUi64s_Init(&p->MTime);
  SzBitUi64s_Init(&p->CTime);
}

static void SzArEx_Free(CSzArEx *p, ISzAlloc *alloc)
{
  IAlloc_Free(alloc, p->UnpackPositions);
  IAlloc_Free(alloc, p->IsDirs);

  IAlloc_Free(alloc, p->FolderToFile);
  IAlloc_Free(alloc, p->FileToFolder);

  IAlloc_Free(alloc, p->FileNameOffsets);
  IAlloc_Free(alloc, p->FileNames);

  SzBitUi32s_Free(&p->CRCs, alloc);
  SzBitUi32s_Free(&p->Attribs, alloc);
  /* SzBitUi32s_Free(&p->Parents, alloc); */
  SzBitUi64s_Free(&p->MTime, alloc);
  SzBitUi64s_Free(&p->CTime, alloc);

  SzAr_Free(&p->db, alloc);
  SzArEx_Init(p);
}


static int TestSignatureCandidate(const Byte *testBytes)
{
  unsigned i;
  for (i = 0; i < k7zSignatureSize; i++)
    if (testBytes[i] != k7zSignature[i])
      return 0;
  return 1;
}

#define SzData_Clear(p) { (p)->Data = NULL; (p)->Size = 0; }

#define SZ_READ_BYTE_SD(_sd_, dest) if ((_sd_)->Size == 0) return SZ_ERROR_ARCHIVE; (_sd_)->Size--; dest = *(_sd_)->Data++;
#define SZ_READ_BYTE(dest) SZ_READ_BYTE_SD(sd, dest)
#define SZ_READ_BYTE_2(dest) if (sd.Size == 0) return SZ_ERROR_ARCHIVE; sd.Size--; dest = *sd.Data++;

#define SKIP_DATA(sd, size) { sd->Size -= (size_t)(size); sd->Data += (size_t)(size); }
#define SKIP_DATA2(sd, size) { sd.Size -= (size_t)(size); sd.Data += (size_t)(size); }

#define SZ_READ_32(dest) if (sd.Size < 4) return SZ_ERROR_ARCHIVE; \
   dest = GetUi32(sd.Data); SKIP_DATA2(sd, 4);

static MY_NO_INLINE SRes ReadNumber(CSzData *sd, UInt64 *value)
{
  Byte firstByte, mask;
  unsigned i;
  UInt32 v;

  SZ_READ_BYTE(firstByte);
  if ((firstByte & 0x80) == 0)
  {
    *value = firstByte;
    return SZ_OK;
  }
  SZ_READ_BYTE(v);
  if ((firstByte & 0x40) == 0)
  {
    *value = (((UInt32)firstByte & 0x3F) << 8) | v;
    return SZ_OK;
  }
  SZ_READ_BYTE(mask);
  *value = v | ((UInt32)mask << 8);
  mask = 0x20;
  for (i = 2; i < 8; i++)
  {
    Byte b;
    if ((firstByte & mask) == 0)
    {
      UInt64 highPart = (unsigned)firstByte & (unsigned)(mask - 1);
      *value |= (highPart << (8 * i));
      return SZ_OK;
    }
    SZ_READ_BYTE(b);
    *value |= ((UInt64)b << (8 * i));
    mask >>= 1;
  }
  return SZ_OK;
}


static MY_NO_INLINE SRes SzReadNumber32(CSzData *sd, UInt32 *value)
{
  Byte firstByte;
  UInt64 value64;
  if (sd->Size == 0)
    return SZ_ERROR_ARCHIVE;
  firstByte = *sd->Data;
  if ((firstByte & 0x80) == 0)
  {
    *value = firstByte;
    sd->Data++;
    sd->Size--;
    return SZ_OK;
  }
  RINOK(ReadNumber(sd, &value64));
  if (value64 >= (UInt32)0x80000000 - 1)
    return SZ_ERROR_UNSUPPORTED;
  if (value64 >= ((UInt64)(1) << ((sizeof(size_t) - 1) * 8 + 4)))
    return SZ_ERROR_UNSUPPORTED;
  *value = (UInt32)value64;
  return SZ_OK;
}

#define ReadID(sd, value) ReadNumber(sd, value)

static SRes SkipData(CSzData *sd)
{
  UInt64 size;
  RINOK(ReadNumber(sd, &size));
  if (size > sd->Size)
    return SZ_ERROR_ARCHIVE;
  SKIP_DATA(sd, size);
  return SZ_OK;
}

static SRes WaitId(CSzData *sd, UInt32 id)
{
  for (;;)
  {
    UInt64 type;
    RINOK(ReadID(sd, &type));
    if (type == id)
      return SZ_OK;
    if (type == k7zIdEnd)
      return SZ_ERROR_ARCHIVE;
    RINOK(SkipData(sd));
  }
}

static SRes RememberBitVector(CSzData *sd, UInt32 numItems, const Byte **v)
{
  UInt32 numBytes = (numItems + 7) >> 3;
  if (numBytes > sd->Size)
    return SZ_ERROR_ARCHIVE;
  *v = sd->Data;
  SKIP_DATA(sd, numBytes);
  return SZ_OK;
}

static UInt32 CountDefinedBits(const Byte *bits, UInt32 numItems)
{
  Byte b = 0;
  unsigned m = 0;
  UInt32 sum = 0;
  for (; numItems != 0; numItems--)
  {
    if (m == 0)
    {
      b = *bits++;
      m = 8;
    }
    m--;
    sum += ((b >> m) & 1);
  }
  return sum;
}

static MY_NO_INLINE SRes ReadBitVector(CSzData *sd, UInt32 numItems, Byte **v, ISzAlloc *alloc)
{
  Byte allAreDefined;
  Byte *v2;
  UInt32 numBytes = (numItems + 7) >> 3;
  *v = NULL;
  SZ_READ_BYTE(allAreDefined);
  if (numBytes == 0)
    return SZ_OK;
  if (allAreDefined == 0)
  {
    if (numBytes > sd->Size)
      return SZ_ERROR_ARCHIVE;
    MY_ALLOC_AND_CPY(*v, numBytes, sd->Data, alloc);
    SKIP_DATA(sd, numBytes);
    return SZ_OK;
  }
  MY_ALLOC(Byte, *v, numBytes, alloc);
  v2 = *v;
  memset(v2, 0xFF, (size_t)numBytes);
  {
    unsigned numBits = (unsigned)numItems & 7;
    if (numBits != 0)
      v2[numBytes - 1] = (Byte)((((UInt32)1 << numBits) - 1) << (8 - numBits));
  }
  return SZ_OK;
}

static MY_NO_INLINE SRes ReadUi32s(CSzData *sd2, UInt32 numItems, CSzBitUi32s *crcs, ISzAlloc *alloc)
{
  UInt32 i;
  CSzData sd;
  UInt32 *vals;
  const Byte *defs;
  MY_ALLOC_ZE(UInt32, crcs->Vals, numItems, alloc);
  sd = *sd2;
  defs = crcs->Defs;
  vals = crcs->Vals;
  for (i = 0; i < numItems; i++)
    if (SzBitArray_Check(defs, i))
    {
      SZ_READ_32(vals[i]);
    }
    else
      vals[i] = 0;
  *sd2 = sd;
  return SZ_OK;
}

static SRes ReadBitUi32s(CSzData *sd, UInt32 numItems, CSzBitUi32s *crcs, ISzAlloc *alloc)
{
  SzBitUi32s_Free(crcs, alloc);
  RINOK(ReadBitVector(sd, numItems, &crcs->Defs, alloc));
  return ReadUi32s(sd, numItems, crcs, alloc);
}

static SRes SkipBitUi32s(CSzData *sd, UInt32 numItems)
{
  Byte allAreDefined;
  UInt32 numDefined = numItems;
  SZ_READ_BYTE(allAreDefined);
  if (!allAreDefined)
  {
    size_t numBytes = (numItems + 7) >> 3;
    if (numBytes > sd->Size)
      return SZ_ERROR_ARCHIVE;
    numDefined = CountDefinedBits(sd->Data, numItems);
    SKIP_DATA(sd, numBytes);
  }
  if (numDefined > (sd->Size >> 2))
    return SZ_ERROR_ARCHIVE;
  SKIP_DATA(sd, (size_t)numDefined * 4);
  return SZ_OK;
}

static SRes ReadPackInfo(CSzAr *p, CSzData *sd, ISzAlloc *alloc)
{
  RINOK(SzReadNumber32(sd, &p->NumPackStreams));

  RINOK(WaitId(sd, k7zIdSize));
  MY_ALLOC(UInt64, p->PackPositions, (size_t)p->NumPackStreams + 1, alloc);
  {
    UInt64 sum = 0;
    UInt32 i;
    UInt32 numPackStreams = p->NumPackStreams;
    for (i = 0; i < numPackStreams; i++)
    {
      UInt64 packSize;
      p->PackPositions[i] = sum;
      RINOK(ReadNumber(sd, &packSize));
      sum += packSize;
      if (sum < packSize)
        return SZ_ERROR_ARCHIVE;
    }
    p->PackPositions[i] = sum;
  }

  for (;;)
  {
    UInt64 type;
    RINOK(ReadID(sd, &type));
    if (type == k7zIdEnd)
      return SZ_OK;
    if (type == k7zIdCRC)
    {
      /* CRC of packed streams is unused now */
      RINOK(SkipBitUi32s(sd, p->NumPackStreams));
      continue;
    }
    RINOK(SkipData(sd));
  }
}

/*
static SRes SzReadSwitch(CSzData *sd)
{
  Byte external;
  RINOK(SzReadByte(sd, &external));
  return (external == 0) ? SZ_OK: SZ_ERROR_UNSUPPORTED;
}
*/

#define k_NumCodersStreams_in_Folder_MAX (SZ_NUM_BONDS_IN_FOLDER_MAX + SZ_NUM_PACK_STREAMS_IN_FOLDER_MAX)

static SRes SzGetNextFolderItem(CSzFolder *f, CSzData *sd)
{
  UInt32 numCoders, i;
  UInt32 numInStreams = 0;
  const Byte *dataStart = sd->Data;

  f->NumCoders = 0;
  f->NumBonds = 0;
  f->NumPackStreams = 0;
  f->UnpackStream = 0;

  RINOK(SzReadNumber32(sd, &numCoders));
  if (numCoders == 0 || numCoders > SZ_NUM_CODERS_IN_FOLDER_MAX)
    return SZ_ERROR_UNSUPPORTED;

  for (i = 0; i < numCoders; i++)
  {
    Byte mainByte;
    CSzCoderInfo *coder = f->Coders + i;
    unsigned idSize, j;
    UInt64 id;

    SZ_READ_BYTE(mainByte);
    if ((mainByte & 0xC0) != 0)
      return SZ_ERROR_UNSUPPORTED;

    idSize = (unsigned)(mainByte & 0xF);
    if (idSize > sizeof(id))
      return SZ_ERROR_UNSUPPORTED;
    if (idSize > sd->Size)
      return SZ_ERROR_ARCHIVE;
    id = 0;
    for (j = 0; j < idSize; j++)
    {
      id = ((id << 8) | *sd->Data);
      sd->Data++;
      sd->Size--;
    }
    if (id > UINT64_CONST(0xFFFFFFFF))
      return SZ_ERROR_UNSUPPORTED;
    coder->MethodID = (UInt32)id;

    coder->NumStreams = 1;
    coder->PropsOffset = 0;
    coder->PropsSize = 0;

    if ((mainByte & 0x10) != 0)
    {
      UInt32 numStreams;

      RINOK(SzReadNumber32(sd, &numStreams));
      if (numStreams > k_NumCodersStreams_in_Folder_MAX)
        return SZ_ERROR_UNSUPPORTED;
      coder->NumStreams = (Byte)numStreams;

      RINOK(SzReadNumber32(sd, &numStreams));
      if (numStreams != 1)
        return SZ_ERROR_UNSUPPORTED;
    }

    numInStreams += coder->NumStreams;

    if (numInStreams > k_NumCodersStreams_in_Folder_MAX)
      return SZ_ERROR_UNSUPPORTED;

    if ((mainByte & 0x20) != 0)
    {
      UInt32 propsSize = 0;
      RINOK(SzReadNumber32(sd, &propsSize));
      if (propsSize > sd->Size)
        return SZ_ERROR_ARCHIVE;
      if (propsSize >= 0x80)
        return SZ_ERROR_UNSUPPORTED;
      coder->PropsOffset = sd->Data - dataStart;
      coder->PropsSize = (Byte)propsSize;
      sd->Data += (size_t)propsSize;
      sd->Size -= (size_t)propsSize;
    }
  }

  /*
  if (numInStreams == 1 && numCoders == 1)
  {
    f->NumPackStreams = 1;
    f->PackStreams[0] = 0;
  }
  else
  */
  {
    Byte streamUsed[k_NumCodersStreams_in_Folder_MAX];
    UInt32 numBonds, numPackStreams;

    numBonds = numCoders - 1;
    if (numInStreams < numBonds)
      return SZ_ERROR_ARCHIVE;
    if (numBonds > SZ_NUM_BONDS_IN_FOLDER_MAX)
      return SZ_ERROR_UNSUPPORTED;
    f->NumBonds = numBonds;

    numPackStreams = numInStreams - numBonds;
    if (numPackStreams > SZ_NUM_PACK_STREAMS_IN_FOLDER_MAX)
      return SZ_ERROR_UNSUPPORTED;
    f->NumPackStreams = numPackStreams;

    for (i = 0; i < numInStreams; i++)
      streamUsed[i] = False;

    if (numBonds != 0)
    {
      Byte coderUsed[SZ_NUM_CODERS_IN_FOLDER_MAX];

      for (i = 0; i < numCoders; i++)
        coderUsed[i] = False;

      for (i = 0; i < numBonds; i++)
      {
        CSzBond *bp = f->Bonds + i;

        RINOK(SzReadNumber32(sd, &bp->InIndex));
        if (bp->InIndex >= numInStreams || streamUsed[bp->InIndex])
          return SZ_ERROR_ARCHIVE;
        streamUsed[bp->InIndex] = True;

        RINOK(SzReadNumber32(sd, &bp->OutIndex));
        if (bp->OutIndex >= numCoders || coderUsed[bp->OutIndex])
          return SZ_ERROR_ARCHIVE;
        coderUsed[bp->OutIndex] = True;
      }

      for (i = 0; i < numCoders; i++)
        if (!coderUsed[i])
        {
          f->UnpackStream = i;
          break;
        }

      if (i == numCoders)
        return SZ_ERROR_ARCHIVE;
    }

    if (numPackStreams == 1)
    {
      for (i = 0; i < numInStreams; i++)
        if (!streamUsed[i])
          break;
      if (i == numInStreams)
        return SZ_ERROR_ARCHIVE;
      f->PackStreams[0] = i;
    }
    else
      for (i = 0; i < numPackStreams; i++)
      {
        UInt32 index;
        RINOK(SzReadNumber32(sd, &index));
        if (index >= numInStreams || streamUsed[index])
          return SZ_ERROR_ARCHIVE;
        streamUsed[index] = True;
        f->PackStreams[i] = index;
      }
  }

  f->NumCoders = numCoders;

  return SZ_OK;
}


static MY_NO_INLINE SRes SkipNumbers(CSzData *sd2, UInt32 num)
{
  CSzData sd;
  sd = *sd2;
  for (; num != 0; num--)
  {
    Byte firstByte, mask;
    unsigned i;
    SZ_READ_BYTE_2(firstByte);
    if ((firstByte & 0x80) == 0)
      continue;
    if ((firstByte & 0x40) == 0)
    {
      if (sd.Size == 0)
        return SZ_ERROR_ARCHIVE;
      sd.Size--;
      sd.Data++;
      continue;
    }
    mask = 0x20;
    for (i = 2; i < 8 && (firstByte & mask) != 0; i++)
      mask >>= 1;
    if (i > sd.Size)
      return SZ_ERROR_ARCHIVE;
    SKIP_DATA2(sd, i);
  }
  *sd2 = sd;
  return SZ_OK;
}


#define k_Scan_NumCoders_MAX 64
#define k_Scan_NumCodersStreams_in_Folder_MAX 64


static SRes ReadUnpackInfo(CSzAr *p,
    CSzData *sd2,
    UInt32 numFoldersMax,
    const CBuf *tempBufs, UInt32 numTempBufs,
    ISzAlloc *alloc)
{
  CSzData sd;

  UInt32 fo, numFolders, numCodersOutStreams, packStreamIndex;
  const Byte *startBufPtr;
  Byte external;

  RINOK(WaitId(sd2, k7zIdFolder));

  RINOK(SzReadNumber32(sd2, &numFolders));
  if (numFolders > numFoldersMax)
    return SZ_ERROR_UNSUPPORTED;
  p->NumFolders = numFolders;

  SZ_READ_BYTE_SD(sd2, external);
  if (external == 0)
    sd = *sd2;
  else
  {
    UInt32 index;
    RINOK(SzReadNumber32(sd2, &index));
    if (index >= numTempBufs)
      return SZ_ERROR_ARCHIVE;
    sd.Data = tempBufs[index].data;
    sd.Size = tempBufs[index].size;
  }

  MY_ALLOC(size_t, p->FoCodersOffsets, (size_t)numFolders + 1, alloc);
  MY_ALLOC(UInt32, p->FoStartPackStreamIndex, (size_t)numFolders + 1, alloc);
  MY_ALLOC(UInt32, p->FoToCoderUnpackSizes, (size_t)numFolders + 1, alloc);
  MY_ALLOC(Byte, p->FoToMainUnpackSizeIndex, (size_t)numFolders, alloc);

  startBufPtr = sd.Data;

  packStreamIndex = 0;
  numCodersOutStreams = 0;

  for (fo = 0; fo < numFolders; fo++)
  {
    UInt32 numCoders, ci, numInStreams = 0;

    p->FoCodersOffsets[fo] = sd.Data - startBufPtr;

    RINOK(SzReadNumber32(&sd, &numCoders));
    if (numCoders == 0 || numCoders > k_Scan_NumCoders_MAX)
      return SZ_ERROR_UNSUPPORTED;

    for (ci = 0; ci < numCoders; ci++)
    {
      Byte mainByte;
      unsigned idSize;
      UInt32 coderInStreams;

      SZ_READ_BYTE_2(mainByte);
      if ((mainByte & 0xC0) != 0)
        return SZ_ERROR_UNSUPPORTED;
      idSize = (mainByte & 0xF);
      if (idSize > 8)
        return SZ_ERROR_UNSUPPORTED;
      if (idSize > sd.Size)
        return SZ_ERROR_ARCHIVE;
      SKIP_DATA2(sd, idSize);

      coderInStreams = 1;

      if ((mainByte & 0x10) != 0)
      {
        UInt32 coderOutStreams;
        RINOK(SzReadNumber32(&sd, &coderInStreams));
        RINOK(SzReadNumber32(&sd, &coderOutStreams));
        if (coderInStreams > k_Scan_NumCodersStreams_in_Folder_MAX || coderOutStreams != 1)
          return SZ_ERROR_UNSUPPORTED;
      }

      numInStreams += coderInStreams;

      if ((mainByte & 0x20) != 0)
      {
        UInt32 propsSize;
        RINOK(SzReadNumber32(&sd, &propsSize));
        if (propsSize > sd.Size)
          return SZ_ERROR_ARCHIVE;
        SKIP_DATA2(sd, propsSize);
      }
    }

    {
      UInt32 indexOfMainStream = 0;
      UInt32 numPackStreams = 1;

      if (numCoders != 1 || numInStreams != 1)
      {
        Byte streamUsed[k_Scan_NumCodersStreams_in_Folder_MAX];
        Byte coderUsed[k_Scan_NumCoders_MAX];

        UInt32 i;
        UInt32 numBonds = numCoders - 1;
        if (numInStreams < numBonds)
          return SZ_ERROR_ARCHIVE;

        if (numInStreams > k_Scan_NumCodersStreams_in_Folder_MAX)
          return SZ_ERROR_UNSUPPORTED;

        for (i = 0; i < numInStreams; i++)
          streamUsed[i] = False;
        for (i = 0; i < numCoders; i++)
          coderUsed[i] = False;

        for (i = 0; i < numBonds; i++)
        {
          UInt32 index;

          RINOK(SzReadNumber32(&sd, &index));
          if (index >= numInStreams || streamUsed[index])
            return SZ_ERROR_ARCHIVE;
          streamUsed[index] = True;

          RINOK(SzReadNumber32(&sd, &index));
          if (index >= numCoders || coderUsed[index])
            return SZ_ERROR_ARCHIVE;
          coderUsed[index] = True;
        }

        numPackStreams = numInStreams - numBonds;

        if (numPackStreams != 1)
          for (i = 0; i < numPackStreams; i++)
          {
            UInt32 index;
            RINOK(SzReadNumber32(&sd, &index));
            if (index >= numInStreams || streamUsed[index])
              return SZ_ERROR_ARCHIVE;
            streamUsed[index] = True;
          }

        for (i = 0; i < numCoders; i++)
          if (!coderUsed[i])
          {
            indexOfMainStream = i;
            break;
          }

        if (i == numCoders)
          return SZ_ERROR_ARCHIVE;
      }

      p->FoStartPackStreamIndex[fo] = packStreamIndex;
      p->FoToCoderUnpackSizes[fo] = numCodersOutStreams;
      p->FoToMainUnpackSizeIndex[fo] = (Byte)indexOfMainStream;
      numCodersOutStreams += numCoders;
      if (numCodersOutStreams < numCoders)
        return SZ_ERROR_UNSUPPORTED;
      if (numPackStreams > p->NumPackStreams - packStreamIndex)
        return SZ_ERROR_ARCHIVE;
      packStreamIndex += numPackStreams;
    }
  }

  p->FoToCoderUnpackSizes[fo] = numCodersOutStreams;

  {
    size_t dataSize = sd.Data - startBufPtr;
    p->FoStartPackStreamIndex[fo] = packStreamIndex;
    p->FoCodersOffsets[fo] = dataSize;
    MY_ALLOC_ZE_AND_CPY(p->CodersData, dataSize, startBufPtr, alloc);
  }

  if (external != 0)
  {
    if (sd.Size != 0)
      return SZ_ERROR_ARCHIVE;
    sd = *sd2;
  }

  RINOK(WaitId(&sd, k7zIdCodersUnpackSize));

  MY_ALLOC_ZE(UInt64, p->CoderUnpackSizes, (size_t)numCodersOutStreams, alloc);
  {
    UInt32 i;
    for (i = 0; i < numCodersOutStreams; i++)
    {
      RINOK(ReadNumber(&sd, p->CoderUnpackSizes + i));
    }
  }

  for (;;)
  {
    UInt64 type;
    RINOK(ReadID(&sd, &type));
    if (type == k7zIdEnd)
    {
      *sd2 = sd;
      return SZ_OK;
    }
    if (type == k7zIdCRC)
    {
      RINOK(ReadBitUi32s(&sd, numFolders, &p->FolderCRCs, alloc));
      continue;
    }
    RINOK(SkipData(&sd));
  }
}


static UInt64 SzAr_GetFolderUnpackSize(const CSzAr *p, UInt32 folderIndex)
{
  return p->CoderUnpackSizes[p->FoToCoderUnpackSizes[folderIndex] + p->FoToMainUnpackSizeIndex[folderIndex]];
}


typedef struct
{
  UInt32 NumTotalSubStreams;
  UInt32 NumSubDigests;
  CSzData sdNumSubStreams;
  CSzData sdSizes;
  CSzData sdCRCs;
} CSubStreamInfo;


static SRes ReadSubStreamsInfo(CSzAr *p, CSzData *sd, CSubStreamInfo *ssi)
{
  UInt64 type = 0;
  UInt32 numSubDigests = 0;
  UInt32 numFolders = p->NumFolders;
  UInt32 numUnpackStreams = numFolders;
  UInt32 numUnpackSizesInData = 0;

  for (;;)
  {
    RINOK(ReadID(sd, &type));
    if (type == k7zIdNumUnpackStream)
    {
      UInt32 i;
      ssi->sdNumSubStreams.Data = sd->Data;
      numUnpackStreams = 0;
      numSubDigests = 0;
      for (i = 0; i < numFolders; i++)
      {
        UInt32 numStreams;
        RINOK(SzReadNumber32(sd, &numStreams));
        if (numUnpackStreams > numUnpackStreams + numStreams)
          return SZ_ERROR_UNSUPPORTED;
        numUnpackStreams += numStreams;
        if (numStreams != 0)
          numUnpackSizesInData += (numStreams - 1);
        if (numStreams != 1 || !SzBitWithVals_Check(&p->FolderCRCs, i))
          numSubDigests += numStreams;
      }
      ssi->sdNumSubStreams.Size = sd->Data - ssi->sdNumSubStreams.Data;
      continue;
    }
    if (type == k7zIdCRC || type == k7zIdSize || type == k7zIdEnd)
      break;
    RINOK(SkipData(sd));
  }

  if (!ssi->sdNumSubStreams.Data)
  {
    numSubDigests = numFolders;
    if (p->FolderCRCs.Defs)
      numSubDigests = numFolders - CountDefinedBits(p->FolderCRCs.Defs, numFolders);
  }

  ssi->NumTotalSubStreams = numUnpackStreams;
  ssi->NumSubDigests = numSubDigests;

  if (type == k7zIdSize)
  {
    ssi->sdSizes.Data = sd->Data;
    RINOK(SkipNumbers(sd, numUnpackSizesInData));
    ssi->sdSizes.Size = sd->Data - ssi->sdSizes.Data;
    RINOK(ReadID(sd, &type));
  }

  for (;;)
  {
    if (type == k7zIdEnd)
      return SZ_OK;
    if (type == k7zIdCRC)
    {
      ssi->sdCRCs.Data = sd->Data;
      RINOK(SkipBitUi32s(sd, numSubDigests));
      ssi->sdCRCs.Size = sd->Data - ssi->sdCRCs.Data;
    }
    else
    {
      RINOK(SkipData(sd));
    }
    RINOK(ReadID(sd, &type));
  }
}

static SRes SzReadStreamsInfo(CSzAr *p,
    CSzData *sd,
    UInt32 numFoldersMax, const CBuf *tempBufs, UInt32 numTempBufs,
    UInt64 *dataOffset,
    CSubStreamInfo *ssi,
    ISzAlloc *alloc)
{
  UInt64 type;

  SzData_Clear(&ssi->sdSizes);
  SzData_Clear(&ssi->sdCRCs);
  SzData_Clear(&ssi->sdNumSubStreams);

  *dataOffset = 0;
  RINOK(ReadID(sd, &type));
  if (type == k7zIdPackInfo)
  {
    RINOK(ReadNumber(sd, dataOffset));
    RINOK(ReadPackInfo(p, sd, alloc));
    RINOK(ReadID(sd, &type));
  }
  if (type == k7zIdUnpackInfo)
  {
    RINOK(ReadUnpackInfo(p, sd, numFoldersMax, tempBufs, numTempBufs, alloc));
    RINOK(ReadID(sd, &type));
  }
  if (type == k7zIdSubStreamsInfo)
  {
    RINOK(ReadSubStreamsInfo(p, sd, ssi));
    RINOK(ReadID(sd, &type));
  }
  else
  {
    ssi->NumTotalSubStreams = p->NumFolders;
    /* ssi->NumSubDigests = 0; */
  }

  return (type == k7zIdEnd ? SZ_OK : SZ_ERROR_UNSUPPORTED);
}

static SRes SzReadAndDecodePackedStreams(
    ILookInStream *inStream,
    CSzData *sd,
    CBuf *tempBufs,
    UInt32 numFoldersMax,
    UInt64 baseOffset,
    CSzAr *p,
    ISzAlloc *allocTemp)
{
  UInt64 dataStartPos = 0;
  UInt32 fo;
  CSubStreamInfo ssi;
  UInt32 numFolders;

  RINOK(SzReadStreamsInfo(p, sd, numFoldersMax, NULL, 0, &dataStartPos, &ssi, allocTemp));

  numFolders = p->NumFolders;
  if (numFolders == 0)
    return SZ_ERROR_ARCHIVE;
  else if (numFolders > numFoldersMax)
    return SZ_ERROR_UNSUPPORTED;

  dataStartPos += baseOffset;

  for (fo = 0; fo < numFolders; fo++)
    Buf_Init(tempBufs + fo);

  for (fo = 0; fo < numFolders; fo++)
  {
    CBuf *tempBuf = tempBufs + fo;
    UInt64 unpackSize = SzAr_GetFolderUnpackSize(p, fo);
    if ((size_t)unpackSize != unpackSize)
      return SZ_ERROR_MEM;
    if (!Buf_Create(tempBuf, (size_t)unpackSize, allocTemp))
      return SZ_ERROR_MEM;
  }

  for (fo = 0; fo < numFolders; fo++)
  {
    const CBuf *tempBuf = tempBufs + fo;
    RINOK(LookInStream_SeekTo(inStream, dataStartPos));
    RINOK(SzAr_DecodeFolder(p, fo, inStream, dataStartPos, tempBuf->data, tempBuf->size, allocTemp));
  }

  return SZ_OK;
}

static SRes SzReadFileNames(const Byte *data, size_t size, UInt32 numFiles, size_t *offsets)
{
  size_t pos = 0;
  *offsets++ = 0;
  if (numFiles == 0)
    return (size == 0) ? SZ_OK : SZ_ERROR_ARCHIVE;
  if (size < 2)
    return SZ_ERROR_ARCHIVE;
  if (data[size - 2] != 0 || data[size - 1] != 0)
    return SZ_ERROR_ARCHIVE;
  do
  {
    const Byte *p;
    if (pos == size)
      return SZ_ERROR_ARCHIVE;
    for (p = data + pos;
      #ifdef _WIN32
      *(const UInt16 *)p != 0
      #else
      p[0] != 0 || p[1] != 0
      #endif
      ; p += 2);
    pos = p - data + 2;
    *offsets++ = (pos >> 1);
  }
  while (--numFiles);
  return (pos == size) ? SZ_OK : SZ_ERROR_ARCHIVE;
}

static MY_NO_INLINE SRes ReadTime(CSzBitUi64s *p, UInt32 num,
    CSzData *sd2,
    const CBuf *tempBufs, UInt32 numTempBufs,
    ISzAlloc *alloc)
{
  CSzData sd;
  UInt32 i;
  CNtfsFileTime *vals;
  Byte *defs;
  Byte external;

  RINOK(ReadBitVector(sd2, num, &p->Defs, alloc));

  SZ_READ_BYTE_SD(sd2, external);
  if (external == 0)
    sd = *sd2;
  else
  {
    UInt32 index;
    RINOK(SzReadNumber32(sd2, &index));
    if (index >= numTempBufs)
      return SZ_ERROR_ARCHIVE;
    sd.Data = tempBufs[index].data;
    sd.Size = tempBufs[index].size;
  }

  MY_ALLOC_ZE(CNtfsFileTime, p->Vals, num, alloc);
  vals = p->Vals;
  defs = p->Defs;
  for (i = 0; i < num; i++)
    if (SzBitArray_Check(defs, i))
    {
      if (sd.Size < 8)
        return SZ_ERROR_ARCHIVE;
      vals[i].Low = GetUi32(sd.Data);
      vals[i].High = GetUi32(sd.Data + 4);
      SKIP_DATA2(sd, 8);
    }
    else
      vals[i].High = vals[i].Low = 0;

  if (external == 0)
    *sd2 = sd;

  return SZ_OK;
}


#define NUM_ADDITIONAL_STREAMS_MAX 8


static SRes SzReadHeader2(
    CSzArEx *p,   /* allocMain */
    CSzData *sd,
    ILookInStream *inStream,
    CBuf *tempBufs, UInt32 *numTempBufs,
    ISzAlloc *allocMain,
    ISzAlloc *allocTemp
    )
{
  CSubStreamInfo ssi;

{
  UInt64 type;

  SzData_Clear(&ssi.sdSizes);
  SzData_Clear(&ssi.sdCRCs);
  SzData_Clear(&ssi.sdNumSubStreams);

  ssi.NumSubDigests = 0;
  ssi.NumTotalSubStreams = 0;

  RINOK(ReadID(sd, &type));

  if (type == k7zIdArchiveProperties)
  {
    for (;;)
    {
      UInt64 type2;
      RINOK(ReadID(sd, &type2));
      if (type2 == k7zIdEnd)
        break;
      RINOK(SkipData(sd));
    }
    RINOK(ReadID(sd, &type));
  }

  if (type == k7zIdAdditionalStreamsInfo)
  {
    CSzAr tempAr;
    SRes res;

    SzAr_Init(&tempAr);
    res = SzReadAndDecodePackedStreams(inStream, sd, tempBufs, NUM_ADDITIONAL_STREAMS_MAX,
        p->startPosAfterHeader, &tempAr, allocTemp);
    *numTempBufs = tempAr.NumFolders;
    SzAr_Free(&tempAr, allocTemp);

    if (res != SZ_OK)
      return res;
    RINOK(ReadID(sd, &type));
  }

  if (type == k7zIdMainStreamsInfo)
  {
    RINOK(SzReadStreamsInfo(&p->db, sd, (UInt32)1 << 30, tempBufs, *numTempBufs,
        &p->dataPos, &ssi, allocMain));
    p->dataPos += p->startPosAfterHeader;
    RINOK(ReadID(sd, &type));
  }

  if (type == k7zIdEnd)
  {
    return SZ_OK;
  }

  if (type != k7zIdFilesInfo)
    return SZ_ERROR_ARCHIVE;
}

{
  UInt32 numFiles = 0;
  UInt32 numEmptyStreams = 0;
  const Byte *emptyStreams = NULL;
  const Byte *emptyFiles = NULL;

  RINOK(SzReadNumber32(sd, &numFiles));
  p->NumFiles = numFiles;

  for (;;)
  {
    UInt64 type;
    UInt64 size;
    RINOK(ReadID(sd, &type));
    if (type == k7zIdEnd)
      break;
    RINOK(ReadNumber(sd, &size));
    if (size > sd->Size)
      return SZ_ERROR_ARCHIVE;

    if (type >= ((UInt32)1 << 8))
    {
      SKIP_DATA(sd, size);
    }
    else switch ((unsigned)type)
    {
      case k7zIdName:
      {
        size_t namesSize;
        const Byte *namesData;
        Byte external;

        SZ_READ_BYTE(external);
        if (external == 0)
        {
          namesSize = (size_t)size - 1;
          namesData = sd->Data;
        }
        else
        {
          UInt32 index;
          RINOK(SzReadNumber32(sd, &index));
          if (index >= *numTempBufs)
            return SZ_ERROR_ARCHIVE;
          namesData = (tempBufs)[index].data;
          namesSize = (tempBufs)[index].size;
        }

        if ((namesSize & 1) != 0)
          return SZ_ERROR_ARCHIVE;
        MY_ALLOC(size_t, p->FileNameOffsets, numFiles + 1, allocMain);
        MY_ALLOC_ZE_AND_CPY(p->FileNames, namesSize, namesData, allocMain);
        RINOK(SzReadFileNames(p->FileNames, namesSize, numFiles, p->FileNameOffsets))
        if (external == 0)
        {
          SKIP_DATA(sd, namesSize);
        }
        break;
      }
      case k7zIdEmptyStream:
      {
        RINOK(RememberBitVector(sd, numFiles, &emptyStreams));
        numEmptyStreams = CountDefinedBits(emptyStreams, numFiles);
        emptyFiles = NULL;
        break;
      }
      case k7zIdEmptyFile:
      {
        RINOK(RememberBitVector(sd, numEmptyStreams, &emptyFiles));
        break;
      }
      case k7zIdWinAttrib:
      {
        Byte external;
        CSzData sdSwitch;
        CSzData *sdPtr;
        SzBitUi32s_Free(&p->Attribs, allocMain);
        RINOK(ReadBitVector(sd, numFiles, &p->Attribs.Defs, allocMain));

        SZ_READ_BYTE(external);
        if (external == 0)
          sdPtr = sd;
        else
        {
          UInt32 index;
          RINOK(SzReadNumber32(sd, &index));
          if (index >= *numTempBufs)
            return SZ_ERROR_ARCHIVE;
          sdSwitch.Data = (tempBufs)[index].data;
          sdSwitch.Size = (tempBufs)[index].size;
          sdPtr = &sdSwitch;
        }
        RINOK(ReadUi32s(sdPtr, numFiles, &p->Attribs, allocMain));
        break;
      }
      /*
      case k7zParent:
      {
        SzBitUi32s_Free(&p->Parents, allocMain);
        RINOK(ReadBitVector(sd, numFiles, &p->Parents.Defs, allocMain));
        RINOK(SzReadSwitch(sd));
        RINOK(ReadUi32s(sd, numFiles, &p->Parents, allocMain));
        break;
      }
      */
      case k7zIdMTime: RINOK(ReadTime(&p->MTime, numFiles, sd, tempBufs, *numTempBufs, allocMain)); break;
      case k7zIdCTime: RINOK(ReadTime(&p->CTime, numFiles, sd, tempBufs, *numTempBufs, allocMain)); break;
      default:
      {
        SKIP_DATA(sd, size);
      }
    }
  }

  if (numFiles - numEmptyStreams != ssi.NumTotalSubStreams)
    return SZ_ERROR_ARCHIVE;

  for (;;)
  {
    UInt64 type;
    RINOK(ReadID(sd, &type));
    if (type == k7zIdEnd)
      break;
    RINOK(SkipData(sd));
  }

  {
    UInt32 i;
    UInt32 emptyFileIndex = 0;
    UInt32 folderIndex = 0;
    UInt32 remSubStreams = 0;
    UInt32 numSubStreams = 0;
    UInt64 unpackPos = 0;
    const Byte *digestsDefs = NULL;
    const Byte *digestsVals = NULL;
    UInt32 digestsValsIndex = 0;
    UInt32 digestIndex;
    Byte allDigestsDefined = 0;
    Byte isDirMask = 0;
    Byte crcMask = 0;
    Byte mask = 0x80;

    MY_ALLOC(UInt32, p->FolderToFile, p->db.NumFolders + 1, allocMain);
    MY_ALLOC_ZE(UInt32, p->FileToFolder, p->NumFiles, allocMain);
    MY_ALLOC(UInt64, p->UnpackPositions, p->NumFiles + 1, allocMain);
    MY_ALLOC_ZE(Byte, p->IsDirs, (p->NumFiles + 7) >> 3, allocMain);

    RINOK(SzBitUi32s_Alloc(&p->CRCs, p->NumFiles, allocMain));

    if (ssi.sdCRCs.Size != 0)
    {
      SZ_READ_BYTE_SD(&ssi.sdCRCs, allDigestsDefined);
      if (allDigestsDefined)
        digestsVals = ssi.sdCRCs.Data;
      else
      {
        size_t numBytes = (ssi.NumSubDigests + 7) >> 3;
        digestsDefs = ssi.sdCRCs.Data;
        digestsVals = digestsDefs + numBytes;
      }
    }

    digestIndex = 0;

    for (i = 0; i < numFiles; i++, mask >>= 1)
    {
      if (mask == 0)
      {
        UInt32 byteIndex = (i - 1) >> 3;
        p->IsDirs[byteIndex] = isDirMask;
        p->CRCs.Defs[byteIndex] = crcMask;
        isDirMask = 0;
        crcMask = 0;
        mask = 0x80;
      }

      p->UnpackPositions[i] = unpackPos;
      p->CRCs.Vals[i] = 0;

      if (emptyStreams && SzBitArray_Check(emptyStreams, i))
      {
        if (emptyFiles)
        {
          if (!SzBitArray_Check(emptyFiles, emptyFileIndex))
            isDirMask |= mask;
          emptyFileIndex++;
        }
        else
          isDirMask |= mask;
        if (remSubStreams == 0)
        {
          p->FileToFolder[i] = (UInt32)-1;
          continue;
        }
      }

      if (remSubStreams == 0)
      {
        for (;;)
        {
          if (folderIndex >= p->db.NumFolders)
            return SZ_ERROR_ARCHIVE;
          p->FolderToFile[folderIndex] = i;
          numSubStreams = 1;
          if (ssi.sdNumSubStreams.Data)
          {
            RINOK(SzReadNumber32(&ssi.sdNumSubStreams, &numSubStreams));
          }
          remSubStreams = numSubStreams;
          if (numSubStreams != 0)
            break;
          {
            UInt64 folderUnpackSize = SzAr_GetFolderUnpackSize(&p->db, folderIndex);
            unpackPos += folderUnpackSize;
            if (unpackPos < folderUnpackSize)
              return SZ_ERROR_ARCHIVE;
          }

          folderIndex++;
        }
      }

      p->FileToFolder[i] = folderIndex;

      if (emptyStreams && SzBitArray_Check(emptyStreams, i))
        continue;

      if (--remSubStreams == 0)
      {
        UInt64 folderUnpackSize = SzAr_GetFolderUnpackSize(&p->db, folderIndex);
        UInt64 startFolderUnpackPos = p->UnpackPositions[p->FolderToFile[folderIndex]];
        if (folderUnpackSize < unpackPos - startFolderUnpackPos)
          return SZ_ERROR_ARCHIVE;
        unpackPos = startFolderUnpackPos + folderUnpackSize;
        if (unpackPos < folderUnpackSize)
          return SZ_ERROR_ARCHIVE;

        if (numSubStreams == 1 && SzBitWithVals_Check(&p->db.FolderCRCs, i))
        {
          p->CRCs.Vals[i] = p->db.FolderCRCs.Vals[folderIndex];
          crcMask |= mask;
        }
        else if (allDigestsDefined || (digestsDefs && SzBitArray_Check(digestsDefs, digestIndex)))
        {
          p->CRCs.Vals[i] = GetUi32(digestsVals + (size_t)digestsValsIndex * 4);
          digestsValsIndex++;
          crcMask |= mask;
        }

        folderIndex++;
      }
      else
      {
        UInt64 v;
        RINOK(ReadNumber(&ssi.sdSizes, &v));
        unpackPos += v;
        if (unpackPos < v)
          return SZ_ERROR_ARCHIVE;
        if (allDigestsDefined || (digestsDefs && SzBitArray_Check(digestsDefs, digestIndex)))
        {
          p->CRCs.Vals[i] = GetUi32(digestsVals + (size_t)digestsValsIndex * 4);
          digestsValsIndex++;
          crcMask |= mask;
        }
      }
    }

    if (mask != 0x80)
    {
      UInt32 byteIndex = (i - 1) >> 3;
      p->IsDirs[byteIndex] = isDirMask;
      p->CRCs.Defs[byteIndex] = crcMask;
    }

    p->UnpackPositions[i] = unpackPos;

    if (remSubStreams != 0)
      return SZ_ERROR_ARCHIVE;

    for (;;)
    {
      p->FolderToFile[folderIndex] = i;
      if (folderIndex >= p->db.NumFolders)
        break;
      if (!ssi.sdNumSubStreams.Data)
        return SZ_ERROR_ARCHIVE;
      RINOK(SzReadNumber32(&ssi.sdNumSubStreams, &numSubStreams));
      if (numSubStreams != 0)
        return SZ_ERROR_ARCHIVE;
      /*
      {
        UInt64 folderUnpackSize = SzAr_GetFolderUnpackSize(&p->db, folderIndex);
        unpackPos += folderUnpackSize;
        if (unpackPos < folderUnpackSize)
          return SZ_ERROR_ARCHIVE;
      }
      */
      folderIndex++;
    }

    if (ssi.sdNumSubStreams.Data && ssi.sdNumSubStreams.Size != 0)
      return SZ_ERROR_ARCHIVE;
  }
}
  return SZ_OK;
}


static SRes SzReadHeader(
    CSzArEx *p,
    CSzData *sd,
    ILookInStream *inStream,
    ISzAlloc *allocMain,
    ISzAlloc *allocTemp)
{
  UInt32 i;
  UInt32 numTempBufs = 0;
  SRes res;
  CBuf tempBufs[NUM_ADDITIONAL_STREAMS_MAX];

  for (i = 0; i < NUM_ADDITIONAL_STREAMS_MAX; i++)
    Buf_Init(tempBufs + i);

  res = SzReadHeader2(p, sd, inStream,
      tempBufs, &numTempBufs,
      allocMain, allocTemp);

  for (i = 0; i < NUM_ADDITIONAL_STREAMS_MAX; i++)
    Buf_Free(tempBufs + i, allocTemp);

  RINOK(res);

  if (sd->Size != 0)
    return SZ_ERROR_FAIL;

  return res;
}

static SRes SzArEx_Open2(
    CSzArEx *p,
    ILookInStream *inStream,
    ISzAlloc *allocMain,
    ISzAlloc *allocTemp)
{
  Byte header[k7zStartHeaderSize];
  Int64 startArcPos;
  UInt64 nextHeaderOffset, nextHeaderSize;
  size_t nextHeaderSizeT;
  UInt32 nextHeaderCRC;
  CBuf buf;
  SRes res;

  startArcPos = 0;
  RINOK(inStream->Seek(inStream, &startArcPos, SZ_SEEK_CUR));

  RINOK(LookInStream_Read2(inStream, header, k7zStartHeaderSize, SZ_ERROR_NO_ARCHIVE));

  if (!TestSignatureCandidate(header))
    return SZ_ERROR_NO_ARCHIVE;
  if (header[6] != k7zMajorVersion)
    return SZ_ERROR_UNSUPPORTED;

  nextHeaderOffset = GetUi64(header + 12);
  nextHeaderSize = GetUi64(header + 20);
  nextHeaderCRC = GetUi32(header + 28);

  p->startPosAfterHeader = startArcPos + k7zStartHeaderSize;

  if (CrcCalc(header + 12, 20) != GetUi32(header + 8))
    return SZ_ERROR_CRC;

  nextHeaderSizeT = (size_t)nextHeaderSize;
  if (nextHeaderSizeT != nextHeaderSize)
    return SZ_ERROR_MEM;
  if (nextHeaderSizeT == 0)
    return SZ_OK;
  if (nextHeaderOffset > nextHeaderOffset + nextHeaderSize ||
      nextHeaderOffset > nextHeaderOffset + nextHeaderSize + k7zStartHeaderSize)
    return SZ_ERROR_NO_ARCHIVE;

  {
    Int64 pos = 0;
    RINOK(inStream->Seek(inStream, &pos, SZ_SEEK_END));
    if ((UInt64)pos < startArcPos + nextHeaderOffset ||
        (UInt64)pos < startArcPos + k7zStartHeaderSize + nextHeaderOffset ||
        (UInt64)pos < startArcPos + k7zStartHeaderSize + nextHeaderOffset + nextHeaderSize)
      return SZ_ERROR_INPUT_EOF;
  }

  RINOK(LookInStream_SeekTo(inStream, startArcPos + k7zStartHeaderSize + nextHeaderOffset));

  if (!Buf_Create(&buf, nextHeaderSizeT, allocTemp))
    return SZ_ERROR_MEM;

  res = LookInStream_Read(inStream, buf.data, nextHeaderSizeT);

  if (res == SZ_OK)
  {
    res = SZ_ERROR_ARCHIVE;
    if (CrcCalc(buf.data, nextHeaderSizeT) == nextHeaderCRC)
    {
      CSzData sd;
      UInt64 type;
      sd.Data = buf.data;
      sd.Size = buf.size;

      res = ReadID(&sd, &type);

      if (res == SZ_OK && type == k7zIdEncodedHeader)
      {
        CSzAr tempAr;
        CBuf tempBuf;
        Buf_Init(&tempBuf);

        SzAr_Init(&tempAr);
        res = SzReadAndDecodePackedStreams(inStream, &sd, &tempBuf, 1, p->startPosAfterHeader, &tempAr, allocTemp);
        SzAr_Free(&tempAr, allocTemp);

        if (res != SZ_OK)
        {
          Buf_Free(&tempBuf, allocTemp);
        }
        else
        {
          Buf_Free(&buf, allocTemp);
          buf.data = tempBuf.data;
          buf.size = tempBuf.size;
          sd.Data = buf.data;
          sd.Size = buf.size;
          res = ReadID(&sd, &type);
        }
      }

      if (res == SZ_OK)
      {
        if (type == k7zIdHeader)
        {
          /*
          CSzData sd2;
          unsigned ttt;
          for (ttt = 0; ttt < 40000; ttt++)
          {
            SzArEx_Free(p, allocMain);
            sd2 = sd;
            res = SzReadHeader(p, &sd2, inStream, allocMain, allocTemp);
            if (res != SZ_OK)
              break;
          }
          */
          res = SzReadHeader(p, &sd, inStream, allocMain, allocTemp);
        }
        else
          res = SZ_ERROR_UNSUPPORTED;
      }
    }
  }

  Buf_Free(&buf, allocTemp);
  return res;
}


static SRes SzArEx_Open(CSzArEx *p, ILookInStream *inStream,
    ISzAlloc *allocMain, ISzAlloc *allocTemp)
{
  SRes res = SzArEx_Open2(p, inStream, allocMain, allocTemp);
  if (res != SZ_OK)
    SzArEx_Free(p, allocMain);
  return res;
}


static SRes SzArEx_Extract(
    const CSzArEx *p,
    ILookInStream *inStream,
    UInt32 fileIndex,
    UInt32 *blockIndex,
    Byte **tempBuf,
    size_t *outBufferSize,
    size_t *offset,
    size_t *outSizeProcessed,
    ISzAlloc *allocMain,
    ISzAlloc *allocTemp)
{
  UInt32 folderIndex = p->FileToFolder[fileIndex];
  SRes res = SZ_OK;

  *offset = 0;
  *outSizeProcessed = 0;

  if (folderIndex == (UInt32)-1)
  {
    IAlloc_Free(allocMain, *tempBuf);
    *blockIndex = folderIndex;
    *tempBuf = NULL;
    *outBufferSize = 0;
    return SZ_OK;
  }

  if (*tempBuf == NULL || *blockIndex != folderIndex)
  {
    UInt64 unpackSizeSpec = SzAr_GetFolderUnpackSize(&p->db, folderIndex);
    /*
    UInt64 unpackSizeSpec =
        p->UnpackPositions[p->FolderToFile[folderIndex + 1]] -
        p->UnpackPositions[p->FolderToFile[folderIndex]];
    */
    size_t unpackSize = (size_t)unpackSizeSpec;

    if (unpackSize != unpackSizeSpec)
      return SZ_ERROR_MEM;
    *blockIndex = folderIndex;
    IAlloc_Free(allocMain, *tempBuf);
    *tempBuf = NULL;

    if (res == SZ_OK)
    {
      *outBufferSize = unpackSize;
      if (unpackSize != 0)
      {
        *tempBuf = (Byte *)IAlloc_Alloc(allocMain, unpackSize);
        if (*tempBuf == NULL)
          res = SZ_ERROR_MEM;
      }

      if (res == SZ_OK)
      {
        res = SzAr_DecodeFolder(&p->db, folderIndex,
            inStream, p->dataPos, *tempBuf, unpackSize, allocTemp);
      }
    }
  }

  if (res == SZ_OK)
  {
    UInt64 unpackPos = p->UnpackPositions[fileIndex];
    *offset = (size_t)(unpackPos - p->UnpackPositions[p->FolderToFile[folderIndex]]);
    *outSizeProcessed = (size_t)(p->UnpackPositions[fileIndex + 1] - unpackPos);
    if (*offset + *outSizeProcessed > *outBufferSize)
      return SZ_ERROR_FAIL;
    if (SzBitWithVals_Check(&p->CRCs, fileIndex))
      if (CrcCalc(*tempBuf + *offset, *outSizeProcessed) != p->CRCs.Vals[fileIndex])
        res = SZ_ERROR_CRC;
  }

  return res;
}


static size_t SzArEx_GetFileNameUtf16(const CSzArEx *p, size_t fileIndex, UInt16 *dest)
{
  size_t offs = p->FileNameOffsets[fileIndex];
  size_t len = p->FileNameOffsets[fileIndex + 1] - offs;
  if (dest != 0)
  {
    size_t i;
    const Byte *src = p->FileNames + offs * 2;
    for (i = 0; i < len; i++)
      dest[i] = GetUi16(src + i * 2);
  }
  return len;
}

/*
static size_t SzArEx_GetFullNameLen(const CSzArEx *p, size_t fileIndex)
{
  size_t len;
  if (!p->FileNameOffsets)
    return 1;
  len = 0;
  for (;;)
  {
    UInt32 parent = (UInt32)(Int32)-1;
    len += p->FileNameOffsets[fileIndex + 1] - p->FileNameOffsets[fileIndex];
    if SzBitWithVals_Check(&p->Parents, fileIndex)
      parent = p->Parents.Vals[fileIndex];
    if (parent == (UInt32)(Int32)-1)
      return len;
    fileIndex = parent;
  }
}

static UInt16 *SzArEx_GetFullNameUtf16_Back(const CSzArEx *p, size_t fileIndex, UInt16 *dest)
{
  Bool needSlash;
  if (!p->FileNameOffsets)
  {
    *(--dest) = 0;
    return dest;
  }
  needSlash = False;
  for (;;)
  {
    UInt32 parent = (UInt32)(Int32)-1;
    size_t curLen = p->FileNameOffsets[fileIndex + 1] - p->FileNameOffsets[fileIndex];
    SzArEx_GetFileNameUtf16(p, fileIndex, dest - curLen);
    if (needSlash)
      *(dest - 1) = '/';
    needSlash = True;
    dest -= curLen;

    if SzBitWithVals_Check(&p->Parents, fileIndex)
      parent = p->Parents.Vals[fileIndex];
    if (parent == (UInt32)(Int32)-1)
      return dest;
    fileIndex = parent;
  }
}
*/

/* 7zBuf.c -- Byte Buffer
2013-01-21 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "7zBuf.h"
*/

static void Buf_Init(CBuf *p)
{
  p->data = 0;
  p->size = 0;
}

static int Buf_Create(CBuf *p, size_t size, ISzAlloc *alloc)
{
  p->size = 0;
  if (size == 0)
  {
    p->data = 0;
    return 1;
  }
  p->data = (Byte *)alloc->Alloc(alloc, size);
  if (p->data != 0)
  {
    p->size = size;
    return 1;
  }
  return 0;
}

static void Buf_Free(CBuf *p, ISzAlloc *alloc)
{
  alloc->Free(alloc, p->data);
  p->data = 0;
  p->size = 0;
}

/* 7zDec.c -- Decoding from 7z folder
2015-11-18 : Igor Pavlov : Public domain */

/* #define _7ZIP_PPMD_SUPPPORT */

/*
#include "Precomp.h"

#include <string.h>

#include "7z.h"
#include "7zCrc.h"

#include "Bcj2.h"
#include "Bra.h"
#include "CpuArch.h"
#include "Delta.h"
#include "LzmaDec.h"
#include "Lzma2Dec.h"
#ifdef _7ZIP_PPMD_SUPPPORT
#include "Ppmd7.h"
#endif
*/

#define k_Copy 0
#define k_Delta 3
#define k_LZMA2 0x21
#define k_LZMA  0x30101
#define k_BCJ   0x3030103
#define k_BCJ2  0x303011B
#define k_PPC   0x3030205
#define k_IA64  0x3030401
#define k_ARM   0x3030501
#define k_ARMT  0x3030701
#define k_SPARC 0x3030805


#ifdef _7ZIP_PPMD_SUPPPORT

#define k_PPMD 0x30401

typedef struct
{
  IByteIn p;
  const Byte *cur;
  const Byte *end;
  const Byte *begin;
  UInt64 processed;
  Bool extra;
  SRes res;
  ILookInStream *inStream;
} CByteInToLook;

static Byte ReadByte(void *pp)
{
  CByteInToLook *p = (CByteInToLook *)pp;
  if (p->cur != p->end)
    return *p->cur++;
  if (p->res == SZ_OK)
  {
    size_t size = p->cur - p->begin;
    p->processed += size;
    p->res = p->inStream->Skip(p->inStream, size);
    size = (1 << 25);
    p->res = p->inStream->Look(p->inStream, (const void **)&p->begin, &size);
    p->cur = p->begin;
    p->end = p->begin + size;
    if (size != 0)
      return *p->cur++;;
  }
  p->extra = True;
  return 0;
}

static SRes SzDecodePpmd(const Byte *props, unsigned propsSize, UInt64 inSize, ILookInStream *inStream,
    Byte *outBuffer, SizeT outSize, ISzAlloc *allocMain)
{
  CPpmd7 ppmd;
  CByteInToLook s;
  SRes res = SZ_OK;

  s.p.Read = ReadByte;
  s.inStream = inStream;
  s.begin = s.end = s.cur = NULL;
  s.extra = False;
  s.res = SZ_OK;
  s.processed = 0;

  if (propsSize != 5)
    return SZ_ERROR_UNSUPPORTED;

  {
    unsigned order = props[0];
    UInt32 memSize = GetUi32(props + 1);
    if (order < PPMD7_MIN_ORDER ||
        order > PPMD7_MAX_ORDER ||
        memSize < PPMD7_MIN_MEM_SIZE ||
        memSize > PPMD7_MAX_MEM_SIZE)
      return SZ_ERROR_UNSUPPORTED;
    Ppmd7_Construct(&ppmd);
    if (!Ppmd7_Alloc(&ppmd, memSize, allocMain))
      return SZ_ERROR_MEM;
    Ppmd7_Init(&ppmd, order);
  }
  {
    CPpmd7z_RangeDec rc;
    Ppmd7z_RangeDec_CreateVTable(&rc);
    rc.Stream = &s.p;
    if (!Ppmd7z_RangeDec_Init(&rc))
      res = SZ_ERROR_DATA;
    else if (s.extra)
      res = (s.res != SZ_OK ? s.res : SZ_ERROR_DATA);
    else
    {
      SizeT i;
      for (i = 0; i < outSize; i++)
      {
        int sym = Ppmd7_DecodeSymbol(&ppmd, &rc.p);
        if (s.extra || sym < 0)
          break;
        outBuffer[i] = (Byte)sym;
      }
      if (i != outSize)
        res = (s.res != SZ_OK ? s.res : SZ_ERROR_DATA);
      else if (s.processed + (s.cur - s.begin) != inSize || !Ppmd7z_RangeDec_IsFinishedOK(&rc))
        res = SZ_ERROR_DATA;
    }
  }
  Ppmd7_Free(&ppmd, allocMain);
  return res;
}

#endif


static SRes SzDecodeLzma(const Byte *props, unsigned propsSize, UInt64 inSize, ILookInStream *inStream,
    Byte *outBuffer, SizeT outSize, ISzAlloc *allocMain)
{
  CLzmaDec state;
  SRes res = SZ_OK;

  LzmaDec_Construct(&state);
  RINOK(LzmaDec_AllocateProbs(&state, props, propsSize, allocMain));
  state.dic = outBuffer;
  state.dicBufSize = outSize;
  LzmaDec_Init(&state);

  for (;;)
  {
    const void *inBuf = NULL;
    size_t lookahead = (1 << 18);
    if (lookahead > inSize)
      lookahead = (size_t)inSize;
    res = inStream->Look(inStream, &inBuf, &lookahead);
    if (res != SZ_OK)
      break;

    {
      SizeT inProcessed = (SizeT)lookahead, dicPos = state.dicPos;
      ELzmaStatus status;
      res = LzmaDec_DecodeToDic(&state, outSize, (const Byte*)inBuf, &inProcessed, LZMA_FINISH_END, &status);
      lookahead -= inProcessed;
      inSize -= inProcessed;
      if (res != SZ_OK)
        break;

      if (status == LZMA_STATUS_FINISHED_WITH_MARK)
      {
        if (outSize != state.dicPos || inSize != 0)
          res = SZ_ERROR_DATA;
        break;
      }

      if (outSize == state.dicPos && inSize == 0 && status == LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK)
        break;

      if (inProcessed == 0 && dicPos == state.dicPos)
      {
        res = SZ_ERROR_DATA;
        break;
      }

      res = inStream->Skip((void *)inStream, inProcessed);
      if (res != SZ_OK)
        break;
    }
  }

  LzmaDec_FreeProbs(&state, allocMain);
  return res;
}


#ifndef _7Z_NO_METHOD_LZMA2

static SRes SzDecodeLzma2(const Byte *props, unsigned propsSize, UInt64 inSize, ILookInStream *inStream,
    Byte *outBuffer, SizeT outSize, ISzAlloc *allocMain)
{
  CLzma2Dec state;
  SRes res = SZ_OK;

  Lzma2Dec_Construct(&state);
  if (propsSize != 1)
    return SZ_ERROR_DATA;
  RINOK(Lzma2Dec_AllocateProbs(&state, props[0], allocMain));
  state.decoder.dic = outBuffer;
  state.decoder.dicBufSize = outSize;
  Lzma2Dec_Init(&state);

  for (;;)
  {
    const void *inBuf = NULL;
    size_t lookahead = (1 << 18);
    if (lookahead > inSize)
      lookahead = (size_t)inSize;
    res = inStream->Look(inStream, &inBuf, &lookahead);
    if (res != SZ_OK)
      break;

    {
      SizeT inProcessed = (SizeT)lookahead, dicPos = state.decoder.dicPos;
      ELzmaStatus status;
      res = Lzma2Dec_DecodeToDic(&state, outSize, (const Byte*)inBuf, &inProcessed, LZMA_FINISH_END, &status);
      lookahead -= inProcessed;
      inSize -= inProcessed;
      if (res != SZ_OK)
        break;

      if (status == LZMA_STATUS_FINISHED_WITH_MARK)
      {
        if (outSize != state.decoder.dicPos || inSize != 0)
          res = SZ_ERROR_DATA;
        break;
      }

      if (inProcessed == 0 && dicPos == state.decoder.dicPos)
      {
        res = SZ_ERROR_DATA;
        break;
      }

      res = inStream->Skip((void *)inStream, inProcessed);
      if (res != SZ_OK)
        break;
    }
  }

  Lzma2Dec_FreeProbs(&state, allocMain);
  return res;
}

#endif


static SRes SzDecodeCopy(UInt64 inSize, ILookInStream *inStream, Byte *outBuffer)
{
  while (inSize > 0)
  {
    const void *inBuf;
    size_t curSize = (1 << 18);
    if (curSize > inSize)
      curSize = (size_t)inSize;
    RINOK(inStream->Look(inStream, &inBuf, &curSize));
    if (curSize == 0)
      return SZ_ERROR_INPUT_EOF;
    memcpy(outBuffer, inBuf, curSize);
    outBuffer += curSize;
    inSize -= curSize;
    RINOK(inStream->Skip((void *)inStream, curSize));
  }
  return SZ_OK;
}

static Bool IS_MAIN_METHOD(UInt32 m)
{
  switch (m)
  {
    case k_Copy:
    case k_LZMA:
    #ifndef _7Z_NO_METHOD_LZMA2
    case k_LZMA2:
    #endif
    #ifdef _7ZIP_PPMD_SUPPPORT
    case k_PPMD:
    #endif
      return True;
  }
  return False;
}

static Bool IS_SUPPORTED_CODER(const CSzCoderInfo *c)
{
  return
      c->NumStreams == 1
      /* && c->MethodID <= (UInt32)0xFFFFFFFF */
      && IS_MAIN_METHOD((UInt32)c->MethodID);
}

#define IS_BCJ2(c) ((c)->MethodID == k_BCJ2 && (c)->NumStreams == 4)

static SRes CheckSupportedFolder(const CSzFolder *f)
{
  if (f->NumCoders < 1 || f->NumCoders > 4)
    return SZ_ERROR_UNSUPPORTED;
  if (!IS_SUPPORTED_CODER(&f->Coders[0]))
    return SZ_ERROR_UNSUPPORTED;
  if (f->NumCoders == 1)
  {
    if (f->NumPackStreams != 1 || f->PackStreams[0] != 0 || f->NumBonds != 0)
      return SZ_ERROR_UNSUPPORTED;
    return SZ_OK;
  }


  #ifndef _7Z_NO_METHODS_FILTERS

  if (f->NumCoders == 2)
  {
    const CSzCoderInfo *c = &f->Coders[1];
    if (
        /* c->MethodID > (UInt32)0xFFFFFFFF || */
        c->NumStreams != 1
        || f->NumPackStreams != 1
        || f->PackStreams[0] != 0
        || f->NumBonds != 1
        || f->Bonds[0].InIndex != 1
        || f->Bonds[0].OutIndex != 0)
      return SZ_ERROR_UNSUPPORTED;
    switch ((UInt32)c->MethodID)
    {
      case k_Delta:
      case k_BCJ:
      case k_PPC:
      case k_IA64:
      case k_SPARC:
      case k_ARM:
      case k_ARMT:
        break;
      default:
        return SZ_ERROR_UNSUPPORTED;
    }
    return SZ_OK;
  }

  #endif


  if (f->NumCoders == 4)
  {
    if (!IS_SUPPORTED_CODER(&f->Coders[1])
        || !IS_SUPPORTED_CODER(&f->Coders[2])
        || !IS_BCJ2(&f->Coders[3]))
      return SZ_ERROR_UNSUPPORTED;
    if (f->NumPackStreams != 4
        || f->PackStreams[0] != 2
        || f->PackStreams[1] != 6
        || f->PackStreams[2] != 1
        || f->PackStreams[3] != 0
        || f->NumBonds != 3
        || f->Bonds[0].InIndex != 5 || f->Bonds[0].OutIndex != 0
        || f->Bonds[1].InIndex != 4 || f->Bonds[1].OutIndex != 1
        || f->Bonds[2].InIndex != 3 || f->Bonds[2].OutIndex != 2)
      return SZ_ERROR_UNSUPPORTED;
    return SZ_OK;
  }

  return SZ_ERROR_UNSUPPORTED;
}

#define CASE_BRA_CONV(isa) case k_ ## isa: isa ## _Convert(outBuffer, outSize, 0, 0); break;

static SRes SzFolder_Decode2(const CSzFolder *folder,
    const Byte *propsData,
    const UInt64 *unpackSizes,
    const UInt64 *packPositions,
    ILookInStream *inStream, UInt64 startPos,
    Byte *outBuffer, SizeT outSize, ISzAlloc *allocMain,
    Byte *tempBuf[])
{
  UInt32 ci;
  SizeT tempSizes[3] = { 0, 0, 0};
  SizeT tempSize3 = 0;
  Byte *tempBuf3 = 0;

  RINOK(CheckSupportedFolder(folder));

  for (ci = 0; ci < folder->NumCoders; ci++)
  {
    const CSzCoderInfo *coder = &folder->Coders[ci];

    if (IS_MAIN_METHOD((UInt32)coder->MethodID))
    {
      UInt32 si = 0;
      UInt64 offset;
      UInt64 inSize;
      Byte *outBufCur = outBuffer;
      SizeT outSizeCur = outSize;
      if (folder->NumCoders == 4)
      {
        UInt32 indices[] = { 3, 2, 0 };
        UInt64 unpackSize = unpackSizes[ci];
        si = indices[ci];
        if (ci < 2)
        {
          Byte *temp;
          outSizeCur = (SizeT)unpackSize;
          if (outSizeCur != unpackSize)
            return SZ_ERROR_MEM;
          temp = (Byte *)IAlloc_Alloc(allocMain, outSizeCur);
          if (!temp && outSizeCur != 0)
            return SZ_ERROR_MEM;
          outBufCur = tempBuf[1 - ci] = temp;
          tempSizes[1 - ci] = outSizeCur;
        }
        else if (ci == 2)
        {
          if (unpackSize > outSize) /* check it */
            return SZ_ERROR_PARAM;
          tempBuf3 = outBufCur = outBuffer + (outSize - (size_t)unpackSize);
          tempSize3 = outSizeCur = (SizeT)unpackSize;
        }
        else
          return SZ_ERROR_UNSUPPORTED;
      }
      offset = packPositions[si];
      inSize = packPositions[si + 1] - offset;
      RINOK(LookInStream_SeekTo(inStream, startPos + offset));

      if (coder->MethodID == k_Copy)
      {
        if (inSize != outSizeCur) /* check it */
          return SZ_ERROR_DATA;
        RINOK(SzDecodeCopy(inSize, inStream, outBufCur));
      }
      else if (coder->MethodID == k_LZMA)
      {
        RINOK(SzDecodeLzma(propsData + coder->PropsOffset, coder->PropsSize, inSize, inStream, outBufCur, outSizeCur, allocMain));
      }
      #ifndef _7Z_NO_METHOD_LZMA2
      else if (coder->MethodID == k_LZMA2)
      {
        RINOK(SzDecodeLzma2(propsData + coder->PropsOffset, coder->PropsSize, inSize, inStream, outBufCur, outSizeCur, allocMain));
      }
      #endif
      #ifdef _7ZIP_PPMD_SUPPPORT
      else if (coder->MethodID == k_PPMD)
      {
        RINOK(SzDecodePpmd(propsData + coder->PropsOffset, coder->PropsSize, inSize, inStream, outBufCur, outSizeCur, allocMain));
      }
      #endif
      else
        return SZ_ERROR_UNSUPPORTED;
    }
    else if (coder->MethodID == k_BCJ2)
    {
      UInt64 offset = packPositions[1];
      UInt64 s3Size = packPositions[2] - offset;

      if (ci != 3)
        return SZ_ERROR_UNSUPPORTED;

      tempSizes[2] = (SizeT)s3Size;
      if (tempSizes[2] != s3Size)
        return SZ_ERROR_MEM;
      tempBuf[2] = (Byte *)IAlloc_Alloc(allocMain, tempSizes[2]);
      if (!tempBuf[2] && tempSizes[2] != 0)
        return SZ_ERROR_MEM;

      RINOK(LookInStream_SeekTo(inStream, startPos + offset));
      RINOK(SzDecodeCopy(s3Size, inStream, tempBuf[2]));

      if ((tempSizes[0] & 3) != 0 ||
          (tempSizes[1] & 3) != 0 ||
          tempSize3 + tempSizes[0] + tempSizes[1] != outSize)
        return SZ_ERROR_DATA;

      {
        CBcj2Dec p;

        p.bufs[0] = tempBuf3;   p.lims[0] = tempBuf3 + tempSize3;
        p.bufs[1] = tempBuf[0]; p.lims[1] = tempBuf[0] + tempSizes[0];
        p.bufs[2] = tempBuf[1]; p.lims[2] = tempBuf[1] + tempSizes[1];
        p.bufs[3] = tempBuf[2]; p.lims[3] = tempBuf[2] + tempSizes[2];

        p.dest = outBuffer;
        p.destLim = outBuffer + outSize;

        Bcj2Dec_Init(&p);
        RINOK(Bcj2Dec_Decode(&p));

        {
          unsigned i;
          for (i = 0; i < 4; i++)
            if (p.bufs[i] != p.lims[i])
              return SZ_ERROR_DATA;

          if (!Bcj2Dec_IsFinished(&p))
            return SZ_ERROR_DATA;

          if (p.dest != p.destLim
             || p.state != BCJ2_STREAM_MAIN)
            return SZ_ERROR_DATA;
        }
      }
    }
    #ifndef _7Z_NO_METHODS_FILTERS
    else if (ci == 1)
    {
      if (coder->MethodID == k_Delta)
      {
        if (coder->PropsSize != 1)
          return SZ_ERROR_UNSUPPORTED;
        {
          Byte state[DELTA_STATE_SIZE];
          Delta_Init(state);
          Delta_Decode(state, (unsigned)(propsData[coder->PropsOffset]) + 1, outBuffer, outSize);
        }
      }
      else
      {
        if (coder->PropsSize != 0)
          return SZ_ERROR_UNSUPPORTED;
        switch (coder->MethodID)
        {
          case k_BCJ:
          {
            UInt32 state;
            x86_Convert_Init(state);
            x86_Convert(outBuffer, outSize, 0, &state, 0);
            break;
          }
          CASE_BRA_CONV(PPC)
          CASE_BRA_CONV(IA64)
          CASE_BRA_CONV(SPARC)
          CASE_BRA_CONV(ARM)
          CASE_BRA_CONV(ARMT)
          default:
            return SZ_ERROR_UNSUPPORTED;
        }
      }
    }
    #endif
    else
      return SZ_ERROR_UNSUPPORTED;
  }

  return SZ_OK;
}


static SRes SzAr_DecodeFolder(const CSzAr *p, UInt32 folderIndex,
    ILookInStream *inStream, UInt64 startPos,
    Byte *outBuffer, size_t outSize,
    ISzAlloc *allocMain)
{
  SRes res;
  CSzFolder folder;
  CSzData sd;

  const Byte *data = p->CodersData + p->FoCodersOffsets[folderIndex];
  sd.Data = data;
  sd.Size = p->FoCodersOffsets[folderIndex + 1] - p->FoCodersOffsets[folderIndex];

  res = SzGetNextFolderItem(&folder, &sd);

  if (res != SZ_OK)
    return res;

  if (sd.Size != 0
      || folder.UnpackStream != p->FoToMainUnpackSizeIndex[folderIndex]
      || outSize != SzAr_GetFolderUnpackSize(p, folderIndex))
    return SZ_ERROR_FAIL;
  {
    unsigned i;
    Byte *tempBuf[3] = { 0, 0, 0};

    res = SzFolder_Decode2(&folder, data,
        &p->CoderUnpackSizes[p->FoToCoderUnpackSizes[folderIndex]],
        p->PackPositions + p->FoStartPackStreamIndex[folderIndex],
        inStream, startPos,
        outBuffer, (SizeT)outSize, allocMain, tempBuf);

    for (i = 0; i < 3; i++)
      IAlloc_Free(allocMain, tempBuf[i]);

    if (res == SZ_OK)
      if (SzBitWithVals_Check(&p->FolderCRCs, folderIndex))
        if (CrcCalc(outBuffer, outSize) != p->FolderCRCs.Vals[folderIndex])
          res = SZ_ERROR_CRC;

    return res;
  }
}

/* Bcj2.c -- BCJ2 Decoder (Converter for x86 code)
2015-08-01 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "Bcj2.h"
#include "CpuArch.h"
*/

#define CProb UInt16

#define kTopValue ((UInt32)1 << 24)
#define kNumModelBits 11
#define kBitModelTotal (1 << kNumModelBits)
#define kNumMoveBits 5

#define _IF_BIT_0 ttt = *prob; bound = (p->range >> kNumModelBits) * ttt; if (p->code < bound)
#define _UPDATE_0 p->range = bound; *prob = (CProb)(ttt + ((kBitModelTotal - ttt) >> kNumMoveBits));
#define _UPDATE_1 p->range -= bound; p->code -= bound; *prob = (CProb)(ttt - (ttt >> kNumMoveBits));

static void Bcj2Dec_Init(CBcj2Dec *p)
{
  unsigned i;

  p->state = BCJ2_DEC_STATE_OK;
  p->ip = 0;
  p->temp[3] = 0;
  p->range = 0;
  p->code = 0;
  for (i = 0; i < sizeof(p->probs) / sizeof(p->probs[0]); i++)
    p->probs[i] = kBitModelTotal >> 1;
}

static SRes Bcj2Dec_Decode(CBcj2Dec *p)
{
  if (p->range <= 5)
  {
    p->state = BCJ2_DEC_STATE_OK;
    for (; p->range != 5; p->range++)
    {
      if (p->range == 1 && p->code != 0)
        return SZ_ERROR_DATA;

      if (p->bufs[BCJ2_STREAM_RC] == p->lims[BCJ2_STREAM_RC])
      {
        p->state = BCJ2_STREAM_RC;
        return SZ_OK;
      }

      p->code = (p->code << 8) | *(p->bufs[BCJ2_STREAM_RC])++;
    }

    if (p->code == 0xFFFFFFFF)
      return SZ_ERROR_DATA;

    p->range = 0xFFFFFFFF;
  }
  else if (p->state >= BCJ2_DEC_STATE_ORIG_0)
  {
    while (p->state <= BCJ2_DEC_STATE_ORIG_3)
    {
      Byte *dest = p->dest;
      if (dest == p->destLim)
        return SZ_OK;
      *dest = p->temp[p->state++ - BCJ2_DEC_STATE_ORIG_0];
      p->dest = dest + 1;
    }
  }

  /*
  if (BCJ2_IS_32BIT_STREAM(p->state))
  {
    const Byte *cur = p->bufs[p->state];
    if (cur == p->lims[p->state])
      return SZ_OK;
    p->bufs[p->state] = cur + 4;

    {
      UInt32 val;
      Byte *dest;
      SizeT rem;

      p->ip += 4;
      val = GetBe32(cur) - p->ip;
      dest = p->dest;
      rem = p->destLim - dest;
      if (rem < 4)
      {
        SizeT i;
        SetUi32(p->temp, val);
        for (i = 0; i < rem; i++)
          dest[i] = p->temp[i];
        p->dest = dest + rem;
        p->state = BCJ2_DEC_STATE_ORIG_0 + (unsigned)rem;
        return SZ_OK;
      }
      SetUi32(dest, val);
      p->temp[3] = (Byte)(val >> 24);
      p->dest = dest + 4;
      p->state = BCJ2_DEC_STATE_OK;
    }
  }
  */

  for (;;)
  {
    if (BCJ2_IS_32BIT_STREAM(p->state))
      p->state = BCJ2_DEC_STATE_OK;
    else
    {
      if (p->range < kTopValue)
      {
        if (p->bufs[BCJ2_STREAM_RC] == p->lims[BCJ2_STREAM_RC])
        {
          p->state = BCJ2_STREAM_RC;
          return SZ_OK;
        }
        p->range <<= 8;
        p->code = (p->code << 8) | *(p->bufs[BCJ2_STREAM_RC])++;
      }

      {
        const Byte *src = p->bufs[BCJ2_STREAM_MAIN];
        const Byte *srcLim;
        Byte *dest;
        SizeT num = p->lims[BCJ2_STREAM_MAIN] - src;

        if (num == 0)
        {
          p->state = BCJ2_STREAM_MAIN;
          return SZ_OK;
        }

        dest = p->dest;
        if (num > (SizeT)(p->destLim - dest))
        {
          num = p->destLim - dest;
          if (num == 0)
          {
            p->state = BCJ2_DEC_STATE_ORIG;
            return SZ_OK;
          }
        }

        srcLim = src + num;

        if (p->temp[3] == 0x0F && (src[0] & 0xF0) == 0x80)
          *dest = src[0];
        else for (;;)
        {
          Byte b = *src;
          *dest = b;
          if (b != 0x0F)
          {
            if ((b & 0xFE) == 0xE8)
              break;
            dest++;
            if (++src != srcLim)
              continue;
            break;
          }
          dest++;
          if (++src == srcLim)
            break;
          if ((*src & 0xF0) != 0x80)
            continue;
          *dest = *src;
          break;
        }

        num = src - p->bufs[BCJ2_STREAM_MAIN];

        if (src == srcLim)
        {
          p->temp[3] = src[-1];
          p->bufs[BCJ2_STREAM_MAIN] = src;
          p->ip += (UInt32)num;
          p->dest += num;
          p->state =
            p->bufs[BCJ2_STREAM_MAIN] ==
            p->lims[BCJ2_STREAM_MAIN] ?
              (unsigned)BCJ2_STREAM_MAIN :
              (unsigned)BCJ2_DEC_STATE_ORIG;
          return SZ_OK;
        }

        {
          UInt32 bound, ttt;
          CProb *prob;
          Byte b = src[0];
          Byte prev = (Byte)(num == 0 ? p->temp[3] : src[-1]);

          p->temp[3] = b;
          p->bufs[BCJ2_STREAM_MAIN] = src + 1;
          num++;
          p->ip += (UInt32)num;
          p->dest += num;

          prob = p->probs + (unsigned)(b == 0xE8 ? 2 + (unsigned)prev : (b == 0xE9 ? 1 : 0));

          _IF_BIT_0
          {
            _UPDATE_0
            continue;
          }
          _UPDATE_1

        }
      }
    }

    {
      UInt32 val;
      unsigned cj = (p->temp[3] == 0xE8) ? BCJ2_STREAM_CALL : BCJ2_STREAM_JUMP;
      const Byte *cur = p->bufs[cj];
      Byte *dest;
      SizeT rem;

      if (cur == p->lims[cj])
      {
        p->state = cj;
        break;
      }

      val = GetBe32(cur);
      p->bufs[cj] = cur + 4;

      p->ip += 4;
      val -= p->ip;
      dest = p->dest;
      rem = p->destLim - dest;

      if (rem < 4)
      {
        SizeT i;
        SetUi32(p->temp, val);
        for (i = 0; i < rem; i++)
          dest[i] = p->temp[i];
        p->dest = dest + rem;
        p->state = BCJ2_DEC_STATE_ORIG_0 + (unsigned)rem;
        break;
      }

      SetUi32(dest, val);
      p->temp[3] = (Byte)(val >> 24);
      p->dest = dest + 4;
    }
  }

  if (p->range < kTopValue && p->bufs[BCJ2_STREAM_RC] != p->lims[BCJ2_STREAM_RC])
  {
    p->range <<= 8;
    p->code = (p->code << 8) | *(p->bufs[BCJ2_STREAM_RC])++;
  }

  return SZ_OK;
}

#undef kTopValue  /* reused later. --ryan. */
#undef kBitModelTotal  /* reused later. --ryan. */


/* Bra.c -- Converters for RISC code
2010-04-16 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "Bra.h"
*/

static SizeT ARM_Convert(Byte *data, SizeT size, UInt32 ip, int encoding)
{
  SizeT i;
  if (size < 4)
    return 0;
  size -= 4;
  ip += 8;
  for (i = 0; i <= size; i += 4)
  {
    if (data[i + 3] == 0xEB)
    {
      UInt32 dest;
      UInt32 src = ((UInt32)data[i + 2] << 16) | ((UInt32)data[i + 1] << 8) | (data[i + 0]);
      src <<= 2;
      if (encoding)
        dest = ip + (UInt32)i + src;
      else
        dest = src - (ip + (UInt32)i);
      dest >>= 2;
      data[i + 2] = (Byte)(dest >> 16);
      data[i + 1] = (Byte)(dest >> 8);
      data[i + 0] = (Byte)dest;
    }
  }
  return i;
}

static SizeT ARMT_Convert(Byte *data, SizeT size, UInt32 ip, int encoding)
{
  SizeT i;
  if (size < 4)
    return 0;
  size -= 4;
  ip += 4;
  for (i = 0; i <= size; i += 2)
  {
    if ((data[i + 1] & 0xF8) == 0xF0 &&
        (data[i + 3] & 0xF8) == 0xF8)
    {
      UInt32 dest;
      UInt32 src =
        (((UInt32)data[i + 1] & 0x7) << 19) |
        ((UInt32)data[i + 0] << 11) |
        (((UInt32)data[i + 3] & 0x7) << 8) |
        (data[i + 2]);

      src <<= 1;
      if (encoding)
        dest = ip + (UInt32)i + src;
      else
        dest = src - (ip + (UInt32)i);
      dest >>= 1;

      data[i + 1] = (Byte)(0xF0 | ((dest >> 19) & 0x7));
      data[i + 0] = (Byte)(dest >> 11);
      data[i + 3] = (Byte)(0xF8 | ((dest >> 8) & 0x7));
      data[i + 2] = (Byte)dest;
      i += 2;
    }
  }
  return i;
}

static SizeT PPC_Convert(Byte *data, SizeT size, UInt32 ip, int encoding)
{
  SizeT i;
  if (size < 4)
    return 0;
  size -= 4;
  for (i = 0; i <= size; i += 4)
  {
    if ((data[i] >> 2) == 0x12 && (data[i + 3] & 3) == 1)
    {
      UInt32 src = ((UInt32)(data[i + 0] & 3) << 24) |
        ((UInt32)data[i + 1] << 16) |
        ((UInt32)data[i + 2] << 8) |
        ((UInt32)data[i + 3] & (~3));

      UInt32 dest;
      if (encoding)
        dest = ip + (UInt32)i + src;
      else
        dest = src - (ip + (UInt32)i);
      data[i + 0] = (Byte)(0x48 | ((dest >> 24) &  0x3));
      data[i + 1] = (Byte)(dest >> 16);
      data[i + 2] = (Byte)(dest >> 8);
      data[i + 3] &= 0x3;
      data[i + 3] |= dest;
    }
  }
  return i;
}

static SizeT SPARC_Convert(Byte *data, SizeT size, UInt32 ip, int encoding)
{
  UInt32 i;
  if (size < 4)
    return 0;
  size -= 4;
  for (i = 0; i <= size; i += 4)
  {
    if ((data[i] == 0x40 && (data[i + 1] & 0xC0) == 0x00) ||
        (data[i] == 0x7F && (data[i + 1] & 0xC0) == 0xC0))
    {
      UInt32 src =
        ((UInt32)data[i + 0] << 24) |
        ((UInt32)data[i + 1] << 16) |
        ((UInt32)data[i + 2] << 8) |
        ((UInt32)data[i + 3]);
      UInt32 dest;

      src <<= 2;
      if (encoding)
        dest = ip + i + src;
      else
        dest = src - (ip + i);
      dest >>= 2;

      dest = (((0 - ((dest >> 22) & 1)) << 22) & 0x3FFFFFFF) | (dest & 0x3FFFFF) | 0x40000000;

      data[i + 0] = (Byte)(dest >> 24);
      data[i + 1] = (Byte)(dest >> 16);
      data[i + 2] = (Byte)(dest >> 8);
      data[i + 3] = (Byte)dest;
    }
  }
  return i;
}

/* Bra86.c -- Converter for x86 code (BCJ)
2013-11-12 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "Bra.h"
*/

#define Test86MSByte(b) ((((b) + 1) & 0xFE) == 0)

static SizeT x86_Convert(Byte *data, SizeT size, UInt32 ip, UInt32 *state, int encoding)
{
  SizeT pos = 0;
  UInt32 mask = *state & 7;
  if (size < 5)
    return 0;
  size -= 4;
  ip += 5;

  for (;;)
  {
    Byte *p = data + pos;
    const Byte *limit = data + size;
    for (; p < limit; p++)
      if ((*p & 0xFE) == 0xE8)
        break;

    {
      SizeT d = (SizeT)(p - data - pos);
      pos = (SizeT)(p - data);
      if (p >= limit)
      {
        *state = (d > 2 ? 0 : mask >> (unsigned)d);
        return pos;
      }
      if (d > 2)
        mask = 0;
      else
      {
        mask >>= (unsigned)d;
        if (mask != 0 && (mask > 4 || mask == 3 || Test86MSByte(p[(mask >> 1) + 1])))
        {
          mask = (mask >> 1) | 4;
          pos++;
          continue;
        }
      }
    }

    if (Test86MSByte(p[4]))
    {
      UInt32 v = ((UInt32)p[4] << 24) | ((UInt32)p[3] << 16) | ((UInt32)p[2] << 8) | ((UInt32)p[1]);
      UInt32 cur = ip + (UInt32)pos;
      pos += 5;
      if (encoding)
        v += cur;
      else
        v -= cur;
      if (mask != 0)
      {
        unsigned sh = (mask & 6) << 2;
        if (Test86MSByte((Byte)(v >> sh)))
        {
          v ^= (((UInt32)0x100 << sh) - 1);
          if (encoding)
            v += cur;
          else
            v -= cur;
        }
        mask = 0;
      }
      p[1] = (Byte)v;
      p[2] = (Byte)(v >> 8);
      p[3] = (Byte)(v >> 16);
      p[4] = (Byte)(0 - ((v >> 24) & 1));
    }
    else
    {
      mask = (mask >> 1) | 4;
      pos++;
    }
  }
}


/* BraIA64.c -- Converter for IA-64 code
2013-11-12 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "Bra.h"
*/
static const Byte kBranchTable[32] =
{
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  4, 4, 6, 6, 0, 0, 7, 7,
  4, 4, 0, 0, 4, 4, 0, 0
};

static SizeT IA64_Convert(Byte *data, SizeT size, UInt32 ip, int encoding)
{
  SizeT i;
  if (size < 16)
    return 0;
  size -= 16;
  for (i = 0; i <= size; i += 16)
  {
    UInt32 instrTemplate = data[i] & 0x1F;
    UInt32 mask = kBranchTable[instrTemplate];
    UInt32 bitPos = 5;
    int slot;
    for (slot = 0; slot < 3; slot++, bitPos += 41)
    {
      UInt32 bytePos, bitRes;
      UInt64 instruction, instNorm;
      int j;
      if (((mask >> slot) & 1) == 0)
        continue;
      bytePos = (bitPos >> 3);
      bitRes = bitPos & 0x7;
      instruction = 0;
      for (j = 0; j < 6; j++)
        instruction += (UInt64)data[i + j + bytePos] << (8 * j);

      instNorm = instruction >> bitRes;
      if (((instNorm >> 37) & 0xF) == 0x5 && ((instNorm >> 9) & 0x7) == 0)
      {
        UInt32 src = (UInt32)((instNorm >> 13) & 0xFFFFF);
        UInt32 dest;
        src |= ((UInt32)(instNorm >> 36) & 1) << 20;

        src <<= 4;

        if (encoding)
          dest = ip + (UInt32)i + src;
        else
          dest = src - (ip + (UInt32)i);

        dest >>= 4;

        instNorm &= ~((UInt64)(0x8FFFFF) << 13);
        instNorm |= ((UInt64)(dest & 0xFFFFF) << 13);
        instNorm |= ((UInt64)(dest & 0x100000) << (36 - 20));

        instruction &= (1 << bitRes) - 1;
        instruction |= (instNorm << bitRes);
        for (j = 0; j < 6; j++)
          data[i + j + bytePos] = (Byte)(instruction >> (8 * j));
      }
    }
  }
  return i;
}


/* Delta.c -- Delta converter
2009-05-26 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "Delta.h"
*/

static void Delta_Init(Byte *state)
{
  unsigned i;
  for (i = 0; i < DELTA_STATE_SIZE; i++)
    state[i] = 0;
}

static void MyMemCpy(Byte *dest, const Byte *src, unsigned size)
{
  unsigned i;
  for (i = 0; i < size; i++)
    dest[i] = src[i];
}

static void Delta_Decode(Byte *state, unsigned delta, Byte *data, SizeT size)
{
  Byte buf[DELTA_STATE_SIZE];
  unsigned j = 0;
  MyMemCpy(buf, state, delta);
  {
    SizeT i;
    for (i = 0; i < size;)
    {
      for (j = 0; j < delta && i < size; i++, j++)
      {
        buf[j] = data[i] = (Byte)(buf[j] + data[i]);
      }
    }
  }
  if (j == delta)
    j = 0;
  MyMemCpy(state, buf + j, delta - j);
  MyMemCpy(state + delta - j, buf, j);
}

/* LzmaDec.c -- LZMA Decoder
2016-05-16 : Igor Pavlov : Public domain */

/*
#include "Precomp.h"

#include "LzmaDec.h"

#include <string.h>
*/

#define kNumTopBits 24
#define kTopValue ((UInt32)1 << kNumTopBits)

#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5

#define RC_INIT_SIZE 5

#define NORMALIZE if (range < kTopValue) { range <<= 8; code = (code << 8) | (*buf++); }

#define IF_BIT_0(p) ttt = *(p); NORMALIZE; bound = (range >> kNumBitModelTotalBits) * ttt; if (code < bound)
#define UPDATE_0(p) range = bound; *(p) = (CLzmaProb)(ttt + ((kBitModelTotal - ttt) >> kNumMoveBits));
#define UPDATE_1(p) range -= bound; code -= bound; *(p) = (CLzmaProb)(ttt - (ttt >> kNumMoveBits));
#define GET_BIT2(p, i, A0, A1) IF_BIT_0(p) \
  { UPDATE_0(p); i = (i + i); A0; } else \
  { UPDATE_1(p); i = (i + i) + 1; A1; }
#define GET_BIT(p, i) GET_BIT2(p, i, ; , ;)

#define TREE_GET_BIT(probs, i) { GET_BIT((probs + i), i); }
#define TREE_DECODE(probs, limit, i) \
  { i = 1; do { TREE_GET_BIT(probs, i); } while (i < limit); i -= limit; }

/* #define _LZMA_SIZE_OPT */

#ifdef _LZMA_SIZE_OPT
#define TREE_6_DECODE(probs, i) TREE_DECODE(probs, (1 << 6), i)
#else
#define TREE_6_DECODE(probs, i) \
  { i = 1; \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  i -= 0x40; }
#endif

#define NORMAL_LITER_DEC GET_BIT(prob + symbol, symbol)
#define MATCHED_LITER_DEC \
  matchByte <<= 1; \
  bit = (matchByte & offs); \
  probLit = prob + offs + bit + symbol; \
  GET_BIT2(probLit, symbol, offs &= ~bit, offs &= bit)

#define NORMALIZE_CHECK if (range < kTopValue) { if (buf >= bufLimit) return DUMMY_ERROR; range <<= 8; code = (code << 8) | (*buf++); }

#define IF_BIT_0_CHECK(p) ttt = *(p); NORMALIZE_CHECK; bound = (range >> kNumBitModelTotalBits) * ttt; if (code < bound)
#define UPDATE_0_CHECK range = bound;
#define UPDATE_1_CHECK range -= bound; code -= bound;
#define GET_BIT2_CHECK(p, i, A0, A1) IF_BIT_0_CHECK(p) \
  { UPDATE_0_CHECK; i = (i + i); A0; } else \
  { UPDATE_1_CHECK; i = (i + i) + 1; A1; }
#define GET_BIT_CHECK(p, i) GET_BIT2_CHECK(p, i, ; , ;)
#define TREE_DECODE_CHECK(probs, limit, i) \
  { i = 1; do { GET_BIT_CHECK(probs + i, i) } while (i < limit); i -= limit; }


#define kNumPosBitsMax 4
#define kNumPosStatesMax (1 << kNumPosBitsMax)

#define kLenNumLowBits 3
#define kLenNumLowSymbols (1 << kLenNumLowBits)
#define kLenNumMidBits 3
#define kLenNumMidSymbols (1 << kLenNumMidBits)
#define kLenNumHighBits 8
#define kLenNumHighSymbols (1 << kLenNumHighBits)

#define LenChoice 0
#define LenChoice2 (LenChoice + 1)
#define LenLow (LenChoice2 + 1)
#define LenMid (LenLow + (kNumPosStatesMax << kLenNumLowBits))
#define LenHigh (LenMid + (kNumPosStatesMax << kLenNumMidBits))
#define kNumLenProbs (LenHigh + kLenNumHighSymbols)


#define kNumStates 12
#define kNumLitStates 7

#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))

#define kNumPosSlotBits 6
#define kNumLenToPosStates 4

#define kNumAlignBits 4
#define kAlignTableSize (1 << kNumAlignBits)

#define kMatchMinLen 2
#define kMatchSpecLenStart (kMatchMinLen + kLenNumLowSymbols + kLenNumMidSymbols + kLenNumHighSymbols)

#define IsMatch 0
#define IsRep (IsMatch + (kNumStates << kNumPosBitsMax))
#define IsRepG0 (IsRep + kNumStates)
#define IsRepG1 (IsRepG0 + kNumStates)
#define IsRepG2 (IsRepG1 + kNumStates)
#define IsRep0Long (IsRepG2 + kNumStates)
#define PosSlot (IsRep0Long + (kNumStates << kNumPosBitsMax))
#define SpecPos (PosSlot + (kNumLenToPosStates << kNumPosSlotBits))
#define Align (SpecPos + kNumFullDistances - kEndPosModelIndex)
#define LenCoder (Align + kAlignTableSize)
#define RepLenCoder (LenCoder + kNumLenProbs)
#define Literal (RepLenCoder + kNumLenProbs)

#define LZMA_BASE_SIZE 1846
#define LZMA_LIT_SIZE 0x300

#if Literal != LZMA_BASE_SIZE
StopCompilingDueBUG
#endif

#define LzmaProps_GetNumProbs(p) (Literal + ((UInt32)LZMA_LIT_SIZE << ((p)->lc + (p)->lp)))

#define LZMA_DIC_MIN (1 << 12)

/* First LZMA-symbol is always decoded.
And it decodes new LZMA-symbols while (buf < bufLimit), but "buf" is without last normalization
Out:
  Result:
    SZ_OK - OK
    SZ_ERROR_DATA - Error
  p->remainLen:
    < kMatchSpecLenStart : normal remain
    = kMatchSpecLenStart : finished
    = kMatchSpecLenStart + 1 : Flush marker (unused now)
    = kMatchSpecLenStart + 2 : State Init Marker (unused now)
*/

static int MY_FAST_CALL LzmaDec_DecodeReal(CLzmaDec *p, SizeT limit, const Byte *bufLimit)
{
  CLzmaProb *probs = p->probs;

  unsigned state = p->state;
  UInt32 rep0 = p->reps[0], rep1 = p->reps[1], rep2 = p->reps[2], rep3 = p->reps[3];
  unsigned pbMask = ((unsigned)1 << (p->prop.pb)) - 1;
  unsigned lpMask = ((unsigned)1 << (p->prop.lp)) - 1;
  unsigned lc = p->prop.lc;

  Byte *dic = p->dic;
  SizeT dicBufSize = p->dicBufSize;
  SizeT dicPos = p->dicPos;

  UInt32 processedPos = p->processedPos;
  UInt32 checkDicSize = p->checkDicSize;
  unsigned len = 0;

  const Byte *buf = p->buf;
  UInt32 range = p->range;
  UInt32 code = p->code;

  do
  {
    CLzmaProb *prob;
    UInt32 bound;
    unsigned ttt;
    unsigned posState = processedPos & pbMask;

    prob = probs + IsMatch + (state << kNumPosBitsMax) + posState;
    IF_BIT_0(prob)
    {
      unsigned symbol;
      UPDATE_0(prob);
      prob = probs + Literal;
      if (processedPos != 0 || checkDicSize != 0)
        prob += ((UInt32)LZMA_LIT_SIZE * (((processedPos & lpMask) << lc) +
            (dic[(dicPos == 0 ? dicBufSize : dicPos) - 1] >> (8 - lc))));
      processedPos++;

      if (state < kNumLitStates)
      {
        state -= (state < 4) ? state : 3;
        symbol = 1;
        #ifdef _LZMA_SIZE_OPT
        do { NORMAL_LITER_DEC } while (symbol < 0x100);
        #else
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        #endif
      }
      else
      {
        unsigned matchByte = dic[dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0)];
        unsigned offs = 0x100;
        state -= (state < 10) ? 3 : 6;
        symbol = 1;
        #ifdef _LZMA_SIZE_OPT
        do
        {
          unsigned bit;
          CLzmaProb *probLit;
          MATCHED_LITER_DEC
        }
        while (symbol < 0x100);
        #else
        {
          unsigned bit;
          CLzmaProb *probLit;
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
        }
        #endif
      }

      dic[dicPos++] = (Byte)symbol;
      continue;
    }

    {
      UPDATE_1(prob);
      prob = probs + IsRep + state;
      IF_BIT_0(prob)
      {
        UPDATE_0(prob);
        state += kNumStates;
        prob = probs + LenCoder;
      }
      else
      {
        UPDATE_1(prob);
        if (checkDicSize == 0 && processedPos == 0)
          return SZ_ERROR_DATA;
        prob = probs + IsRepG0 + state;
        IF_BIT_0(prob)
        {
          UPDATE_0(prob);
          prob = probs + IsRep0Long + (state << kNumPosBitsMax) + posState;
          IF_BIT_0(prob)
          {
            UPDATE_0(prob);
            dic[dicPos] = dic[dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0)];
            dicPos++;
            processedPos++;
            state = state < kNumLitStates ? 9 : 11;
            continue;
          }
          UPDATE_1(prob);
        }
        else
        {
          UInt32 distance;
          UPDATE_1(prob);
          prob = probs + IsRepG1 + state;
          IF_BIT_0(prob)
          {
            UPDATE_0(prob);
            distance = rep1;
          }
          else
          {
            UPDATE_1(prob);
            prob = probs + IsRepG2 + state;
            IF_BIT_0(prob)
            {
              UPDATE_0(prob);
              distance = rep2;
            }
            else
            {
              UPDATE_1(prob);
              distance = rep3;
              rep3 = rep2;
            }
            rep2 = rep1;
          }
          rep1 = rep0;
          rep0 = distance;
        }
        state = state < kNumLitStates ? 8 : 11;
        prob = probs + RepLenCoder;
      }

      #ifdef _LZMA_SIZE_OPT
      {
        unsigned lim, offset;
        CLzmaProb *probLen = prob + LenChoice;
        IF_BIT_0(probLen)
        {
          UPDATE_0(probLen);
          probLen = prob + LenLow + (posState << kLenNumLowBits);
          offset = 0;
          lim = (1 << kLenNumLowBits);
        }
        else
        {
          UPDATE_1(probLen);
          probLen = prob + LenChoice2;
          IF_BIT_0(probLen)
          {
            UPDATE_0(probLen);
            probLen = prob + LenMid + (posState << kLenNumMidBits);
            offset = kLenNumLowSymbols;
            lim = (1 << kLenNumMidBits);
          }
          else
          {
            UPDATE_1(probLen);
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols + kLenNumMidSymbols;
            lim = (1 << kLenNumHighBits);
          }
        }
        TREE_DECODE(probLen, lim, len);
        len += offset;
      }
      #else
      {
        CLzmaProb *probLen = prob + LenChoice;
        IF_BIT_0(probLen)
        {
          UPDATE_0(probLen);
          probLen = prob + LenLow + (posState << kLenNumLowBits);
          len = 1;
          TREE_GET_BIT(probLen, len);
          TREE_GET_BIT(probLen, len);
          TREE_GET_BIT(probLen, len);
          len -= 8;
        }
        else
        {
          UPDATE_1(probLen);
          probLen = prob + LenChoice2;
          IF_BIT_0(probLen)
          {
            UPDATE_0(probLen);
            probLen = prob + LenMid + (posState << kLenNumMidBits);
            len = 1;
            TREE_GET_BIT(probLen, len);
            TREE_GET_BIT(probLen, len);
            TREE_GET_BIT(probLen, len);
          }
          else
          {
            UPDATE_1(probLen);
            probLen = prob + LenHigh;
            TREE_DECODE(probLen, (1 << kLenNumHighBits), len);
            len += kLenNumLowSymbols + kLenNumMidSymbols;
          }
        }
      }
      #endif

      if (state >= kNumStates)
      {
        UInt32 distance;
        prob = probs + PosSlot +
            ((len < kNumLenToPosStates ? len : kNumLenToPosStates - 1) << kNumPosSlotBits);
        TREE_6_DECODE(prob, distance);
        if (distance >= kStartPosModelIndex)
        {
          unsigned posSlot = (unsigned)distance;
          unsigned numDirectBits = (unsigned)(((distance >> 1) - 1));
          distance = (2 | (distance & 1));
          if (posSlot < kEndPosModelIndex)
          {
            distance <<= numDirectBits;
            prob = probs + SpecPos + distance - posSlot - 1;
            {
              UInt32 mask = 1;
              unsigned i = 1;
              do
              {
                GET_BIT2(prob + i, i, ; , distance |= mask);
                mask <<= 1;
              }
              while (--numDirectBits != 0);
            }
          }
          else
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              NORMALIZE
              range >>= 1;

              {
                UInt32 t;
                code -= range;
                t = (0 - ((UInt32)code >> 31)); /* (UInt32)((Int32)code >> 31) */
                distance = (distance << 1) + (t + 1);
                code += range & t;
              }
              /*
              distance <<= 1;
              if (code >= range)
              {
                code -= range;
                distance |= 1;
              }
              */
            }
            while (--numDirectBits != 0);
            prob = probs + Align;
            distance <<= kNumAlignBits;
            {
              unsigned i = 1;
              GET_BIT2(prob + i, i, ; , distance |= 1);
              GET_BIT2(prob + i, i, ; , distance |= 2);
              GET_BIT2(prob + i, i, ; , distance |= 4);
              GET_BIT2(prob + i, i, ; , distance |= 8);
            }
            if (distance == (UInt32)0xFFFFFFFF)
            {
              len += kMatchSpecLenStart;
              state -= kNumStates;
              break;
            }
          }
        }

        rep3 = rep2;
        rep2 = rep1;
        rep1 = rep0;
        rep0 = distance + 1;
        if (checkDicSize == 0)
        {
          if (distance >= processedPos)
          {
            p->dicPos = dicPos;
            return SZ_ERROR_DATA;
          }
        }
        else if (distance >= checkDicSize)
        {
          p->dicPos = dicPos;
          return SZ_ERROR_DATA;
        }
        state = (state < kNumStates + kNumLitStates) ? kNumLitStates : kNumLitStates + 3;
      }

      len += kMatchMinLen;

      {
        SizeT rem;
        unsigned curLen;
        SizeT pos;

        if ((rem = limit - dicPos) == 0)
        {
          p->dicPos = dicPos;
          return SZ_ERROR_DATA;
        }

        curLen = ((rem < len) ? (unsigned)rem : len);
        pos = dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0);

        processedPos += curLen;

        len -= curLen;
        if (curLen <= dicBufSize - pos)
        {
          Byte *dest = dic + dicPos;
          ptrdiff_t src = (ptrdiff_t)pos - (ptrdiff_t)dicPos;
          const Byte *lim = dest + curLen;
          dicPos += curLen;
          do
            *(dest) = (Byte)*(dest + src);
          while (++dest != lim);
        }
        else
        {
          do
          {
            dic[dicPos++] = dic[pos];
            if (++pos == dicBufSize)
              pos = 0;
          }
          while (--curLen != 0);
        }
      }
    }
  }
  while (dicPos < limit && buf < bufLimit);

  NORMALIZE;

  p->buf = buf;
  p->range = range;
  p->code = code;
  p->remainLen = len;
  p->dicPos = dicPos;
  p->processedPos = processedPos;
  p->reps[0] = rep0;
  p->reps[1] = rep1;
  p->reps[2] = rep2;
  p->reps[3] = rep3;
  p->state = state;

  return SZ_OK;
}

static void MY_FAST_CALL LzmaDec_WriteRem(CLzmaDec *p, SizeT limit)
{
  if (p->remainLen != 0 && p->remainLen < kMatchSpecLenStart)
  {
    Byte *dic = p->dic;
    SizeT dicPos = p->dicPos;
    SizeT dicBufSize = p->dicBufSize;
    unsigned len = p->remainLen;
    SizeT rep0 = p->reps[0]; /* we use SizeT to avoid the BUG of VC14 for AMD64 */
    SizeT rem = limit - dicPos;
    if (rem < len)
      len = (unsigned)(rem);

    if (p->checkDicSize == 0 && p->prop.dicSize - p->processedPos <= len)
      p->checkDicSize = p->prop.dicSize;

    p->processedPos += len;
    p->remainLen -= len;
    while (len != 0)
    {
      len--;
      dic[dicPos] = dic[dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0)];
      dicPos++;
    }
    p->dicPos = dicPos;
  }
}

static int MY_FAST_CALL LzmaDec_DecodeReal2(CLzmaDec *p, SizeT limit, const Byte *bufLimit)
{
  do
  {
    SizeT limit2 = limit;
    if (p->checkDicSize == 0)
    {
      UInt32 rem = p->prop.dicSize - p->processedPos;
      if (limit - p->dicPos > rem)
        limit2 = p->dicPos + rem;
    }

    RINOK(LzmaDec_DecodeReal(p, limit2, bufLimit));

    if (p->checkDicSize == 0 && p->processedPos >= p->prop.dicSize)
      p->checkDicSize = p->prop.dicSize;

    LzmaDec_WriteRem(p, limit);
  }
  while (p->dicPos < limit && p->buf < bufLimit && p->remainLen < kMatchSpecLenStart);

  if (p->remainLen > kMatchSpecLenStart)
    p->remainLen = kMatchSpecLenStart;

  return 0;
}

typedef enum
{
  DUMMY_ERROR, /* unexpected end of input stream */
  DUMMY_LIT,
  DUMMY_MATCH,
  DUMMY_REP
} ELzmaDummy;

static ELzmaDummy LzmaDec_TryDummy(const CLzmaDec *p, const Byte *buf, SizeT inSize)
{
  UInt32 range = p->range;
  UInt32 code = p->code;
  const Byte *bufLimit = buf + inSize;
  const CLzmaProb *probs = p->probs;
  unsigned state = p->state;
  ELzmaDummy res;

  {
    const CLzmaProb *prob;
    UInt32 bound;
    unsigned ttt;
    unsigned posState = (p->processedPos) & ((1 << p->prop.pb) - 1);

    prob = probs + IsMatch + (state << kNumPosBitsMax) + posState;
    IF_BIT_0_CHECK(prob)
    {
      UPDATE_0_CHECK

      /* if (bufLimit - buf >= 7) return DUMMY_LIT; */

      prob = probs + Literal;
      if (p->checkDicSize != 0 || p->processedPos != 0)
        prob += ((UInt32)LZMA_LIT_SIZE *
            ((((p->processedPos) & ((1 << (p->prop.lp)) - 1)) << p->prop.lc) +
            (p->dic[(p->dicPos == 0 ? p->dicBufSize : p->dicPos) - 1] >> (8 - p->prop.lc))));

      if (state < kNumLitStates)
      {
        unsigned symbol = 1;
        do { GET_BIT_CHECK(prob + symbol, symbol) } while (symbol < 0x100);
      }
      else
      {
        unsigned matchByte = p->dic[p->dicPos - p->reps[0] +
            (p->dicPos < p->reps[0] ? p->dicBufSize : 0)];
        unsigned offs = 0x100;
        unsigned symbol = 1;
        do
        {
          unsigned bit;
          const CLzmaProb *probLit;
          matchByte <<= 1;
          bit = (matchByte & offs);
          probLit = prob + offs + bit + symbol;
          GET_BIT2_CHECK(probLit, symbol, offs &= ~bit, offs &= bit)
        }
        while (symbol < 0x100);
      }
      res = DUMMY_LIT;
    }
    else
    {
      unsigned len;
      UPDATE_1_CHECK;

      prob = probs + IsRep + state;
      IF_BIT_0_CHECK(prob)
      {
        UPDATE_0_CHECK;
        state = 0;
        prob = probs + LenCoder;
        res = DUMMY_MATCH;
      }
      else
      {
        UPDATE_1_CHECK;
        res = DUMMY_REP;
        prob = probs + IsRepG0 + state;
        IF_BIT_0_CHECK(prob)
        {
          UPDATE_0_CHECK;
          prob = probs + IsRep0Long + (state << kNumPosBitsMax) + posState;
          IF_BIT_0_CHECK(prob)
          {
            UPDATE_0_CHECK;
            NORMALIZE_CHECK;
            return DUMMY_REP;
          }
          else
          {
            UPDATE_1_CHECK;
          }
        }
        else
        {
          UPDATE_1_CHECK;
          prob = probs + IsRepG1 + state;
          IF_BIT_0_CHECK(prob)
          {
            UPDATE_0_CHECK;
          }
          else
          {
            UPDATE_1_CHECK;
            prob = probs + IsRepG2 + state;
            IF_BIT_0_CHECK(prob)
            {
              UPDATE_0_CHECK;
            }
            else
            {
              UPDATE_1_CHECK;
            }
          }
        }
        state = kNumStates;
        prob = probs + RepLenCoder;
      }
      {
        unsigned limit, offset;
        const CLzmaProb *probLen = prob + LenChoice;
        IF_BIT_0_CHECK(probLen)
        {
          UPDATE_0_CHECK;
          probLen = prob + LenLow + (posState << kLenNumLowBits);
          offset = 0;
          limit = 1 << kLenNumLowBits;
        }
        else
        {
          UPDATE_1_CHECK;
          probLen = prob + LenChoice2;
          IF_BIT_0_CHECK(probLen)
          {
            UPDATE_0_CHECK;
            probLen = prob + LenMid + (posState << kLenNumMidBits);
            offset = kLenNumLowSymbols;
            limit = 1 << kLenNumMidBits;
          }
          else
          {
            UPDATE_1_CHECK;
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols + kLenNumMidSymbols;
            limit = 1 << kLenNumHighBits;
          }
        }
        TREE_DECODE_CHECK(probLen, limit, len);
        len += offset;
      }

      if (state < 4)
      {
        unsigned posSlot;
        prob = probs + PosSlot +
            ((len < kNumLenToPosStates ? len : kNumLenToPosStates - 1) <<
            kNumPosSlotBits);
        TREE_DECODE_CHECK(prob, 1 << kNumPosSlotBits, posSlot);
        if (posSlot >= kStartPosModelIndex)
        {
          unsigned numDirectBits = ((posSlot >> 1) - 1);

          /* if (bufLimit - buf >= 8) return DUMMY_MATCH; */

          if (posSlot < kEndPosModelIndex)
          {
            prob = probs + SpecPos + ((2 | (posSlot & 1)) << numDirectBits) - posSlot - 1;
          }
          else
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              NORMALIZE_CHECK
              range >>= 1;
              code -= range & (((code - range) >> 31) - 1);
              /* if (code >= range) code -= range; */
            }
            while (--numDirectBits != 0);
            prob = probs + Align;
            numDirectBits = kNumAlignBits;
          }
          {
            unsigned i = 1;
            do
            {
              GET_BIT_CHECK(prob + i, i);
            }
            while (--numDirectBits != 0);
          }
        }
      }
    }
  }
  NORMALIZE_CHECK;
  return res;
}


static void LzmaDec_InitDicAndState(CLzmaDec *p, Bool initDic, Bool initState)
{
  p->needFlush = 1;
  p->remainLen = 0;
  p->tempBufSize = 0;

  if (initDic)
  {
    p->processedPos = 0;
    p->checkDicSize = 0;
    p->needInitState = 1;
  }
  if (initState)
    p->needInitState = 1;
}

static void LzmaDec_Init(CLzmaDec *p)
{
  p->dicPos = 0;
  LzmaDec_InitDicAndState(p, True, True);
}

static void LzmaDec_InitStateReal(CLzmaDec *p)
{
  SizeT numProbs = LzmaProps_GetNumProbs(&p->prop);
  SizeT i;
  CLzmaProb *probs = p->probs;
  for (i = 0; i < numProbs; i++)
    probs[i] = kBitModelTotal >> 1;
  p->reps[0] = p->reps[1] = p->reps[2] = p->reps[3] = 1;
  p->state = 0;
  p->needInitState = 0;
}

static SRes LzmaDec_DecodeToDic(CLzmaDec *p, SizeT dicLimit, const Byte *src, SizeT *srcLen,
    ELzmaFinishMode finishMode, ELzmaStatus *status)
{
  SizeT inSize = *srcLen;
  (*srcLen) = 0;
  LzmaDec_WriteRem(p, dicLimit);

  *status = LZMA_STATUS_NOT_SPECIFIED;

  while (p->remainLen != kMatchSpecLenStart)
  {
      int checkEndMarkNow;

      if (p->needFlush)
      {
        for (; inSize > 0 && p->tempBufSize < RC_INIT_SIZE; (*srcLen)++, inSize--)
          p->tempBuf[p->tempBufSize++] = *src++;
        if (p->tempBufSize < RC_INIT_SIZE)
        {
          *status = LZMA_STATUS_NEEDS_MORE_INPUT;
          return SZ_OK;
        }
        if (p->tempBuf[0] != 0)
          return SZ_ERROR_DATA;
        p->code =
              ((UInt32)p->tempBuf[1] << 24)
            | ((UInt32)p->tempBuf[2] << 16)
            | ((UInt32)p->tempBuf[3] << 8)
            | ((UInt32)p->tempBuf[4]);
        p->range = 0xFFFFFFFF;
        p->needFlush = 0;
        p->tempBufSize = 0;
      }

      checkEndMarkNow = 0;
      if (p->dicPos >= dicLimit)
      {
        if (p->remainLen == 0 && p->code == 0)
        {
          *status = LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK;
          return SZ_OK;
        }
        if (finishMode == LZMA_FINISH_ANY)
        {
          *status = LZMA_STATUS_NOT_FINISHED;
          return SZ_OK;
        }
        if (p->remainLen != 0)
        {
          *status = LZMA_STATUS_NOT_FINISHED;
          return SZ_ERROR_DATA;
        }
        checkEndMarkNow = 1;
      }

      if (p->needInitState)
        LzmaDec_InitStateReal(p);

      if (p->tempBufSize == 0)
      {
        SizeT processed;
        const Byte *bufLimit;
        if (inSize < LZMA_REQUIRED_INPUT_MAX || checkEndMarkNow)
        {
          int dummyRes = LzmaDec_TryDummy(p, src, inSize);
          if (dummyRes == DUMMY_ERROR)
          {
            memcpy(p->tempBuf, src, inSize);
            p->tempBufSize = (unsigned)inSize;
            (*srcLen) += inSize;
            *status = LZMA_STATUS_NEEDS_MORE_INPUT;
            return SZ_OK;
          }
          if (checkEndMarkNow && dummyRes != DUMMY_MATCH)
          {
            *status = LZMA_STATUS_NOT_FINISHED;
            return SZ_ERROR_DATA;
          }
          bufLimit = src;
        }
        else
          bufLimit = src + inSize - LZMA_REQUIRED_INPUT_MAX;
        p->buf = src;
        if (LzmaDec_DecodeReal2(p, dicLimit, bufLimit) != 0)
          return SZ_ERROR_DATA;
        processed = (SizeT)(p->buf - src);
        (*srcLen) += processed;
        src += processed;
        inSize -= processed;
      }
      else
      {
        unsigned rem = p->tempBufSize, lookAhead = 0;
        while (rem < LZMA_REQUIRED_INPUT_MAX && lookAhead < inSize)
          p->tempBuf[rem++] = src[lookAhead++];
        p->tempBufSize = rem;
        if (rem < LZMA_REQUIRED_INPUT_MAX || checkEndMarkNow)
        {
          int dummyRes = LzmaDec_TryDummy(p, p->tempBuf, rem);
          if (dummyRes == DUMMY_ERROR)
          {
            (*srcLen) += lookAhead;
            *status = LZMA_STATUS_NEEDS_MORE_INPUT;
            return SZ_OK;
          }
          if (checkEndMarkNow && dummyRes != DUMMY_MATCH)
          {
            *status = LZMA_STATUS_NOT_FINISHED;
            return SZ_ERROR_DATA;
          }
        }
        p->buf = p->tempBuf;
        if (LzmaDec_DecodeReal2(p, dicLimit, p->buf) != 0)
          return SZ_ERROR_DATA;

        {
          unsigned kkk = (unsigned)(p->buf - p->tempBuf);
          if (rem < kkk)
            return SZ_ERROR_FAIL; /* some internal error */
          rem -= kkk;
          if (lookAhead < rem)
            return SZ_ERROR_FAIL; /* some internal error */
          lookAhead -= rem;
        }
        (*srcLen) += lookAhead;
        src += lookAhead;
        inSize -= lookAhead;
        p->tempBufSize = 0;
      }
  }
  if (p->code == 0)
    *status = LZMA_STATUS_FINISHED_WITH_MARK;
  return (p->code == 0) ? SZ_OK : SZ_ERROR_DATA;
}

static void LzmaDec_FreeProbs(CLzmaDec *p, ISzAlloc *alloc)
{
  alloc->Free(alloc, p->probs);
  p->probs = NULL;
}

static SRes LzmaProps_Decode(CLzmaProps *p, const Byte *data, unsigned size)
{
  UInt32 dicSize;
  Byte d;

  if (size < LZMA_PROPS_SIZE)
    return SZ_ERROR_UNSUPPORTED;
  else
    dicSize = data[1] | ((UInt32)data[2] << 8) | ((UInt32)data[3] << 16) | ((UInt32)data[4] << 24);

  if (dicSize < LZMA_DIC_MIN)
    dicSize = LZMA_DIC_MIN;
  p->dicSize = dicSize;

  d = data[0];
  if (d >= (9 * 5 * 5))
    return SZ_ERROR_UNSUPPORTED;

  p->lc = d % 9;
  d /= 9;
  p->pb = d / 5;
  p->lp = d % 5;

  return SZ_OK;
}

static SRes LzmaDec_AllocateProbs2(CLzmaDec *p, const CLzmaProps *propNew, ISzAlloc *alloc)
{
  UInt32 numProbs = LzmaProps_GetNumProbs(propNew);
  if (!p->probs || numProbs != p->numProbs)
  {
    LzmaDec_FreeProbs(p, alloc);
    p->probs = (CLzmaProb *)alloc->Alloc(alloc, numProbs * sizeof(CLzmaProb));
    p->numProbs = numProbs;
    if (!p->probs)
      return SZ_ERROR_MEM;
  }
  return SZ_OK;
}

static SRes LzmaDec_AllocateProbs(CLzmaDec *p, const Byte *props, unsigned propsSize, ISzAlloc *alloc)
{
  CLzmaProps propNew;
  RINOK(LzmaProps_Decode(&propNew, props, propsSize));
  RINOK(LzmaDec_AllocateProbs2(p, &propNew, alloc));
  p->prop = propNew;
  return SZ_OK;
}

/* Lzma2Dec.c -- LZMA2 Decoder
2015-11-09 : Igor Pavlov : Public domain */

/* #define SHOW_DEBUG_INFO */

/*
#include "Precomp.h"

#ifdef SHOW_DEBUG_INFO
#include <stdio.h>
#endif

#include <string.h>

#include "Lzma2Dec.h"
*/

/*
00000000  -  EOS
00000001 U U  -  Uncompressed Reset Dic
00000010 U U  -  Uncompressed No Reset
100uuuuu U U P P  -  LZMA no reset
101uuuuu U U P P  -  LZMA reset state
110uuuuu U U P P S  -  LZMA reset state + new prop
111uuuuu U U P P S  -  LZMA reset state + new prop + reset dic

  u, U - Unpack Size
  P - Pack Size
  S - Props
*/

#define LZMA2_CONTROL_LZMA (1 << 7)
#define LZMA2_CONTROL_COPY_NO_RESET 2
#define LZMA2_CONTROL_COPY_RESET_DIC 1
#define LZMA2_CONTROL_EOF 0

#define LZMA2_IS_UNCOMPRESSED_STATE(p) (((p)->control & LZMA2_CONTROL_LZMA) == 0)

#define LZMA2_GET_LZMA_MODE(p) (((p)->control >> 5) & 3)
#define LZMA2_IS_THERE_PROP(mode) ((mode) >= 2)

#define LZMA2_LCLP_MAX 4
#define LZMA2_DIC_SIZE_FROM_PROP(p) (((UInt32)2 | ((p) & 1)) << ((p) / 2 + 11))

#ifdef SHOW_DEBUG_INFO
#define PRF(x) x
#else
#define PRF(x)
#endif

typedef enum
{
  LZMA2_STATE_CONTROL,
  LZMA2_STATE_UNPACK0,
  LZMA2_STATE_UNPACK1,
  LZMA2_STATE_PACK0,
  LZMA2_STATE_PACK1,
  LZMA2_STATE_PROP,
  LZMA2_STATE_DATA,
  LZMA2_STATE_DATA_CONT,
  LZMA2_STATE_FINISHED,
  LZMA2_STATE_ERROR
} ELzma2State;

static SRes Lzma2Dec_GetOldProps(Byte prop, Byte *props)
{
  UInt32 dicSize;
  if (prop > 40)
    return SZ_ERROR_UNSUPPORTED;
  dicSize = (prop == 40) ? 0xFFFFFFFF : LZMA2_DIC_SIZE_FROM_PROP(prop);
  props[0] = (Byte)LZMA2_LCLP_MAX;
  props[1] = (Byte)(dicSize);
  props[2] = (Byte)(dicSize >> 8);
  props[3] = (Byte)(dicSize >> 16);
  props[4] = (Byte)(dicSize >> 24);
  return SZ_OK;
}

static SRes Lzma2Dec_AllocateProbs(CLzma2Dec *p, Byte prop, ISzAlloc *alloc)
{
  Byte props[LZMA_PROPS_SIZE];
  RINOK(Lzma2Dec_GetOldProps(prop, props));
  return LzmaDec_AllocateProbs(&p->decoder, props, LZMA_PROPS_SIZE, alloc);
}

static void Lzma2Dec_Init(CLzma2Dec *p)
{
  p->state = LZMA2_STATE_CONTROL;
  p->needInitDic = True;
  p->needInitState = True;
  p->needInitProp = True;
  LzmaDec_Init(&p->decoder);
}

static ELzma2State Lzma2Dec_UpdateState(CLzma2Dec *p, Byte b)
{
  switch (p->state)
  {
    case LZMA2_STATE_CONTROL:
      p->control = b;
      PRF(printf("\n %4X ", (unsigned)p->decoder.dicPos));
      PRF(printf(" %2X", (unsigned)b));
      if (p->control == 0)
        return LZMA2_STATE_FINISHED;
      if (LZMA2_IS_UNCOMPRESSED_STATE(p))
      {
        if ((p->control & 0x7F) > 2)
          return LZMA2_STATE_ERROR;
        p->unpackSize = 0;
      }
      else
        p->unpackSize = (UInt32)(p->control & 0x1F) << 16;
      return LZMA2_STATE_UNPACK0;

    case LZMA2_STATE_UNPACK0:
      p->unpackSize |= (UInt32)b << 8;
      return LZMA2_STATE_UNPACK1;

    case LZMA2_STATE_UNPACK1:
      p->unpackSize |= (UInt32)b;
      p->unpackSize++;
      PRF(printf(" %8u", (unsigned)p->unpackSize));
      return (LZMA2_IS_UNCOMPRESSED_STATE(p)) ? LZMA2_STATE_DATA : LZMA2_STATE_PACK0;

    case LZMA2_STATE_PACK0:
      p->packSize = (UInt32)b << 8;
      return LZMA2_STATE_PACK1;

    case LZMA2_STATE_PACK1:
      p->packSize |= (UInt32)b;
      p->packSize++;
      PRF(printf(" %8u", (unsigned)p->packSize));
      return LZMA2_IS_THERE_PROP(LZMA2_GET_LZMA_MODE(p)) ? LZMA2_STATE_PROP:
        (p->needInitProp ? LZMA2_STATE_ERROR : LZMA2_STATE_DATA);

    case LZMA2_STATE_PROP:
    {
      unsigned lc, lp;
      if (b >= (9 * 5 * 5))
        return LZMA2_STATE_ERROR;
      lc = b % 9;
      b /= 9;
      p->decoder.prop.pb = b / 5;
      lp = b % 5;
      if (lc + lp > LZMA2_LCLP_MAX)
        return LZMA2_STATE_ERROR;
      p->decoder.prop.lc = lc;
      p->decoder.prop.lp = lp;
      p->needInitProp = False;
      return LZMA2_STATE_DATA;
    }
  }
  return LZMA2_STATE_ERROR;
}

static void LzmaDec_UpdateWithUncompressed(CLzmaDec *p, const Byte *src, SizeT size)
{
  memcpy(p->dic + p->dicPos, src, size);
  p->dicPos += size;
  if (p->checkDicSize == 0 && p->prop.dicSize - p->processedPos <= size)
    p->checkDicSize = p->prop.dicSize;
  p->processedPos += (UInt32)size;
}

static void LzmaDec_InitDicAndState(CLzmaDec *p, Bool initDic, Bool initState);

static SRes Lzma2Dec_DecodeToDic(CLzma2Dec *p, SizeT dicLimit,
    const Byte *src, SizeT *srcLen, ELzmaFinishMode finishMode, ELzmaStatus *status)
{
  SizeT inSize = *srcLen;
  *srcLen = 0;
  *status = LZMA_STATUS_NOT_SPECIFIED;

  while (p->state != LZMA2_STATE_FINISHED)
  {
    SizeT dicPos = p->decoder.dicPos;

    if (p->state == LZMA2_STATE_ERROR)
      return SZ_ERROR_DATA;

    if (dicPos == dicLimit && finishMode == LZMA_FINISH_ANY)
    {
      *status = LZMA_STATUS_NOT_FINISHED;
      return SZ_OK;
    }

    if (p->state != LZMA2_STATE_DATA && p->state != LZMA2_STATE_DATA_CONT)
    {
      if (*srcLen == inSize)
      {
        *status = LZMA_STATUS_NEEDS_MORE_INPUT;
        return SZ_OK;
      }
      (*srcLen)++;
      p->state = Lzma2Dec_UpdateState(p, *src++);

      if (dicPos == dicLimit && p->state != LZMA2_STATE_FINISHED)
      {
        p->state = LZMA2_STATE_ERROR;
        return SZ_ERROR_DATA;
      }
      continue;
    }

    {
      SizeT destSizeCur = dicLimit - dicPos;
      SizeT srcSizeCur = inSize - *srcLen;
      ELzmaFinishMode curFinishMode = LZMA_FINISH_ANY;

      if (p->unpackSize <= destSizeCur)
      {
        destSizeCur = (SizeT)p->unpackSize;
        curFinishMode = LZMA_FINISH_END;
      }

      if (LZMA2_IS_UNCOMPRESSED_STATE(p))
      {
        if (*srcLen == inSize)
        {
          *status = LZMA_STATUS_NEEDS_MORE_INPUT;
          return SZ_OK;
        }

        if (p->state == LZMA2_STATE_DATA)
        {
          Bool initDic = (p->control == LZMA2_CONTROL_COPY_RESET_DIC);
          if (initDic)
            p->needInitProp = p->needInitState = True;
          else if (p->needInitDic)
          {
            p->state = LZMA2_STATE_ERROR;
            return SZ_ERROR_DATA;
          }
          p->needInitDic = False;
          LzmaDec_InitDicAndState(&p->decoder, initDic, False);
        }

        if (srcSizeCur > destSizeCur)
          srcSizeCur = destSizeCur;

        if (srcSizeCur == 0)
        {
          p->state = LZMA2_STATE_ERROR;
          return SZ_ERROR_DATA;
        }

        LzmaDec_UpdateWithUncompressed(&p->decoder, src, srcSizeCur);

        src += srcSizeCur;
        *srcLen += srcSizeCur;
        p->unpackSize -= (UInt32)srcSizeCur;
        p->state = (p->unpackSize == 0) ? LZMA2_STATE_CONTROL : LZMA2_STATE_DATA_CONT;
      }
      else
      {
        SizeT outSizeProcessed;
        SRes res;

        if (p->state == LZMA2_STATE_DATA)
        {
          unsigned mode = LZMA2_GET_LZMA_MODE(p);
          Bool initDic = (mode == 3);
          Bool initState = (mode != 0);
          if ((!initDic && p->needInitDic) || (!initState && p->needInitState))
          {
            p->state = LZMA2_STATE_ERROR;
            return SZ_ERROR_DATA;
          }

          LzmaDec_InitDicAndState(&p->decoder, initDic, initState);
          p->needInitDic = False;
          p->needInitState = False;
          p->state = LZMA2_STATE_DATA_CONT;
        }

        if (srcSizeCur > p->packSize)
          srcSizeCur = (SizeT)p->packSize;

        res = LzmaDec_DecodeToDic(&p->decoder, dicPos + destSizeCur, src, &srcSizeCur, curFinishMode, status);

        src += srcSizeCur;
        *srcLen += srcSizeCur;
        p->packSize -= (UInt32)srcSizeCur;

        outSizeProcessed = p->decoder.dicPos - dicPos;
        p->unpackSize -= (UInt32)outSizeProcessed;

        RINOK(res);
        if (*status == LZMA_STATUS_NEEDS_MORE_INPUT)
          return res;

        if (srcSizeCur == 0 && outSizeProcessed == 0)
        {
          if (*status != LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK
              || p->unpackSize != 0
              || p->packSize != 0)
          {
            p->state = LZMA2_STATE_ERROR;
            return SZ_ERROR_DATA;
          }
          p->state = LZMA2_STATE_CONTROL;
        }

        if (*status == LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK)
          *status = LZMA_STATUS_NOT_FINISHED;
      }
    }
  }

  *status = LZMA_STATUS_FINISHED_WITH_MARK;
  return SZ_OK;
}

#endif  /* _INCLUDE_PHYSFS_LZMASDK_H_ */

/* end of physfs_lzmasdk.h ... */


typedef struct
{
    ISeekInStream seekStream; /* lzma sdk i/o interface (lower level).  */
    PHYSFS_Io *io;            /* physfs i/o interface for this archive. */
    CLookToRead lookStream;   /* lzma sdk i/o interface (higher level). */
} SZIPLookToRead;

/* One SZIPentry is kept for each file in an open 7zip archive. */
typedef struct
{
    __PHYSFS_DirTreeEntry tree;   /* manages directory tree         */
    PHYSFS_uint32 dbidx;          /* index into lzma sdk database   */
} SZIPentry;

/* One SZIPinfo is kept for each open 7zip archive. */
typedef struct
{
    __PHYSFS_DirTree tree;    /* manages directory tree.           */
    PHYSFS_Io *io;            /* physfs i/o interface for this archive. */
    CSzArEx db;               /* lzma sdk archive database object. */
} SZIPinfo;


static PHYSFS_ErrorCode szipErrorCode(const SRes rc)
{
    switch (rc)
    {
        case SZ_OK: return PHYSFS_ERR_OK;
        case SZ_ERROR_DATA: return PHYSFS_ERR_CORRUPT;
        case SZ_ERROR_MEM: return PHYSFS_ERR_OUT_OF_MEMORY;
        case SZ_ERROR_CRC: return PHYSFS_ERR_CORRUPT;
        case SZ_ERROR_UNSUPPORTED: return PHYSFS_ERR_UNSUPPORTED;
        case SZ_ERROR_INPUT_EOF: return PHYSFS_ERR_CORRUPT;
        case SZ_ERROR_OUTPUT_EOF: return PHYSFS_ERR_IO;
        case SZ_ERROR_READ: return PHYSFS_ERR_IO;
        case SZ_ERROR_WRITE: return PHYSFS_ERR_IO;
        case SZ_ERROR_ARCHIVE: return PHYSFS_ERR_CORRUPT;
        case SZ_ERROR_NO_ARCHIVE: return PHYSFS_ERR_UNSUPPORTED;
        default: break;
    } /* switch */

    return PHYSFS_ERR_OTHER_ERROR;
} /* szipErrorCode */


/* LZMA SDK's ISzAlloc interface ... */

static void *SZIP_ISzAlloc_Alloc(void *p, size_t size)
{
    return allocator.Malloc(size ? size : 1);
} /* SZIP_ISzAlloc_Alloc */

static void SZIP_ISzAlloc_Free(void *p, void *address)
{
    if (address)
        allocator.Free(address);
} /* SZIP_ISzAlloc_Free */

static ISzAlloc SZIP_SzAlloc = {
    SZIP_ISzAlloc_Alloc, SZIP_ISzAlloc_Free
};


/* we implement ISeekInStream, and then wrap that in LZMA SDK's CLookToRead,
   which implements the higher-level ILookInStream on top of that, handling
   buffering and such for us. */

/* LZMA SDK's ISeekInStream interface ... */

static SRes SZIP_ISeekInStream_Read(void *p, void *buf, size_t *size)
{
    SZIPLookToRead *stream = (SZIPLookToRead *) p;
    PHYSFS_Io *io = stream->io;
    const PHYSFS_uint64 len = (PHYSFS_uint64) *size;
    const PHYSFS_sint64 rc = (len == 0) ? 0 : io->read(io, buf, len);

    if (rc < 0)
    {
        *size = 0;
        return SZ_ERROR_READ;
    } /* if */

    *size = (size_t) rc;
    return SZ_OK;
} /* SZIP_ISeekInStream_Read */

static SRes SZIP_ISeekInStream_Seek(void *p, Int64 *pos, ESzSeek origin)
{
    SZIPLookToRead *stream = (SZIPLookToRead *) p;
    PHYSFS_Io *io = stream->io;
    PHYSFS_sint64 base;
    PHYSFS_uint64 newpos;

    switch (origin)
    {
        case SZ_SEEK_SET:
            base = 0;
            break;

        case SZ_SEEK_CUR:
            base = io->tell(io);
            break;

        case SZ_SEEK_END:
            base = io->length(io);
            break;

        default:
            return SZ_ERROR_FAIL;
    } /* switch */

    if (base < 0)
        return SZ_ERROR_FAIL;
    else if ((*pos < 0) && (((Int64) base) < -*pos))
        return SZ_ERROR_FAIL;

    newpos = (PHYSFS_uint64) (((Int64) base) + *pos);
    if (!io->seek(io, newpos))
        return SZ_ERROR_FAIL;

    *pos = (Int64) newpos;
    return SZ_OK;
} /* SZIP_ISeekInStream_Seek */


static void szipInitStream(SZIPLookToRead *stream, PHYSFS_Io *io)
{
    stream->seekStream.Read = SZIP_ISeekInStream_Read;
    stream->seekStream.Seek = SZIP_ISeekInStream_Seek;

    stream->io = io;

    /* !!! FIXME: can we use lookahead? Is there value to it? */
    LookToRead_Init(&stream->lookStream);
    LookToRead_CreateVTable(&stream->lookStream, False);
    stream->lookStream.realStream = &stream->seekStream;
} /* szipInitStream */


/* Do this in a separate function so we can smallAlloc without looping. */
static int szipLoadEntry(SZIPinfo *info, const PHYSFS_uint32 idx)
{
    const size_t utf16len = SzArEx_GetFileNameUtf16(&info->db, idx, NULL);
    const size_t utf16buflen = utf16len * 2;
    PHYSFS_uint16 *utf16 = (PHYSFS_uint16 *) __PHYSFS_smallAlloc(utf16buflen);
    const size_t utf8buflen = utf16len * 4;
    char *utf8 = (char *) __PHYSFS_smallAlloc(utf8buflen);
    int retval = 0;

    if (utf16 && utf8)
    {
        const int isdir = SzArEx_IsDir(&info->db, idx) != 0;
        SZIPentry *entry;
        SzArEx_GetFileNameUtf16(&info->db, idx, (UInt16 *) utf16);
        PHYSFS_utf8FromUtf16(utf16, utf8, utf8buflen);
        entry = (SZIPentry*) __PHYSFS_DirTreeAdd(&info->tree, utf8, isdir);
        retval = (entry != NULL);
        if (retval)
            entry->dbidx = idx;
    } /* if */

    __PHYSFS_smallFree(utf8);
    __PHYSFS_smallFree(utf16);

    return retval;
} /* szipLoadEntry */


static int szipLoadEntries(SZIPinfo *info)
{
    int retval = 0;

    if (__PHYSFS_DirTreeInit(&info->tree, sizeof (SZIPentry)))
    {
        const PHYSFS_uint32 count = info->db.NumFiles;
        PHYSFS_uint32 i;
        for (i = 0; i < count; i++)
            BAIL_IF_ERRPASS(!szipLoadEntry(info, i), 0);
        retval = 1;
    } /* if */

    return retval;
} /* szipLoadEntries */


static void SZIP_closeArchive(void *opaque)
{
    SZIPinfo *info = (SZIPinfo *) opaque;
    if (info)
    {
        if (info->io)
            info->io->destroy(info->io);
        SzArEx_Free(&info->db, &SZIP_SzAlloc);
        __PHYSFS_DirTreeDeinit(&info->tree);
        allocator.Free(info);
    } /* if */
} /* SZIP_closeArchive */


static void *SZIP_openArchive(PHYSFS_Io *io, const char *name,
                              int forWriting, int *claimed)
{
    static const PHYSFS_uint8 wantedsig[] = { '7','z',0xBC,0xAF,0x27,0x1C };
    SZIPLookToRead stream;
    ISzAlloc *alloc = &SZIP_SzAlloc;
    SZIPinfo *info = NULL;
    SRes rc;
    PHYSFS_uint8 sig[6];
    PHYSFS_sint64 pos;

    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);
    pos = io->tell(io);
    BAIL_IF_ERRPASS(pos == -1, NULL);
    BAIL_IF_ERRPASS(io->read(io, sig, 6) != 6, NULL);
    *claimed = (memcmp(sig, wantedsig, 6) == 0);
    BAIL_IF_ERRPASS(!io->seek(io, pos), NULL);

    info = (SZIPinfo *) allocator.Malloc(sizeof (SZIPinfo));
    BAIL_IF(!info, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    memset(info, '\0', sizeof (*info));

    SzArEx_Init(&info->db);

    info->io = io;

    szipInitStream(&stream, io);
    rc = SzArEx_Open(&info->db, &stream.lookStream.s, alloc, alloc);
    GOTO_IF(rc != SZ_OK, szipErrorCode(rc), failed);

    GOTO_IF_ERRPASS(!szipLoadEntries(info), failed);

    return info;

failed:
    info->io = NULL;  /* don't let cleanup destroy the PHYSFS_Io. */
    SZIP_closeArchive(info);
    return NULL;
} /* SZIP_openArchive */


static PHYSFS_Io *SZIP_openRead(void *opaque, const char *path)
{
    /* !!! FIXME: the current lzma sdk C API only allows you to decompress
       !!! FIXME:  the entire file at once, which isn't ideal. Fix this in the
       !!! FIXME:  SDK and then convert this all to a streaming interface. */

    SZIPinfo *info = (SZIPinfo *) opaque;
    SZIPentry *entry = (SZIPentry *) __PHYSFS_DirTreeFind(&info->tree, path);
    ISzAlloc *alloc = &SZIP_SzAlloc;
    SZIPLookToRead stream;
    PHYSFS_Io *retval = NULL;
    PHYSFS_Io *io = NULL;
    UInt32 blockIndex = 0xFFFFFFFF;
    Byte *outBuffer = NULL;
    size_t outBufferSize = 0;
    size_t offset = 0;
    size_t outSizeProcessed = 0;
    void *buf = NULL;
    SRes rc;

    BAIL_IF_ERRPASS(!entry, NULL);
    BAIL_IF(entry->tree.isdir, PHYSFS_ERR_NOT_A_FILE, NULL);

    io = info->io->duplicate(info->io);
    GOTO_IF_ERRPASS(!io, SZIP_openRead_failed);

    szipInitStream(&stream, io);

    rc = SzArEx_Extract(&info->db, &stream.lookStream.s, entry->dbidx,
                        &blockIndex, &outBuffer, &outBufferSize, &offset,
                        &outSizeProcessed, alloc, alloc);
    GOTO_IF(rc != SZ_OK, szipErrorCode(rc), SZIP_openRead_failed);
    GOTO_IF(outBuffer == NULL, PHYSFS_ERR_OUT_OF_MEMORY, SZIP_openRead_failed);

    io->destroy(io);
    io = NULL;

    buf = allocator.Malloc(outSizeProcessed ? outSizeProcessed : 1);
    GOTO_IF(buf == NULL, PHYSFS_ERR_OUT_OF_MEMORY, SZIP_openRead_failed);

    if (outSizeProcessed > 0)
        memcpy(buf, outBuffer + offset, outSizeProcessed);

    alloc->Free(alloc, outBuffer);
    outBuffer = NULL;

    retval = __PHYSFS_createMemoryIo(buf, outSizeProcessed, allocator.Free);
    GOTO_IF_ERRPASS(!retval, SZIP_openRead_failed);

    return retval;

SZIP_openRead_failed:
    if (io != NULL)
        io->destroy(io);

    if (buf)
        allocator.Free(buf);

    if (outBuffer)
        alloc->Free(alloc, outBuffer);

    return NULL;
} /* SZIP_openRead */


static PHYSFS_Io *SZIP_openWrite(void *opaque, const char *filename)
{
    BAIL(PHYSFS_ERR_READ_ONLY, NULL);
} /* SZIP_openWrite */


static PHYSFS_Io *SZIP_openAppend(void *opaque, const char *filename)
{
    BAIL(PHYSFS_ERR_READ_ONLY, NULL);
} /* SZIP_openAppend */


static int SZIP_remove(void *opaque, const char *name)
{
    BAIL(PHYSFS_ERR_READ_ONLY, 0);
} /* SZIP_remove */


static int SZIP_mkdir(void *opaque, const char *name)
{
    BAIL(PHYSFS_ERR_READ_ONLY, 0);
} /* SZIP_mkdir */


static inline PHYSFS_uint64 lzmasdkTimeToPhysfsTime(const CNtfsFileTime *t)
{
    const PHYSFS_uint64 winEpochToUnixEpoch = __PHYSFS_UI64(0x019DB1DED53E8000);
    const PHYSFS_uint64 nanosecToMillisec = __PHYSFS_UI64(10000000);
    const PHYSFS_uint64 quad = (((PHYSFS_uint64) t->High) << 32) | t->Low;
    return (quad - winEpochToUnixEpoch) / nanosecToMillisec;
} /* lzmasdkTimeToPhysfsTime */


static int SZIP_stat(void *opaque, const char *path, PHYSFS_Stat *stat)
{
    SZIPinfo *info = (SZIPinfo *) opaque;
    SZIPentry *entry;
    PHYSFS_uint32 idx;

    entry = (SZIPentry *) __PHYSFS_DirTreeFind(&info->tree, path);
    BAIL_IF_ERRPASS(!entry, 0);
    idx = entry->dbidx;

    if (entry->tree.isdir)
    {
        stat->filesize = -1;
        stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
    } /* if */
    else
    {
        stat->filesize = (PHYSFS_sint64) SzArEx_GetFileSize(&info->db, idx);
        stat->filetype = PHYSFS_FILETYPE_REGULAR;
    } /* else */

    if (info->db.MTime.Vals != NULL)
        stat->modtime = lzmasdkTimeToPhysfsTime(&info->db.MTime.Vals[idx]);
    else if (info->db.CTime.Vals != NULL)
        stat->modtime = lzmasdkTimeToPhysfsTime(&info->db.CTime.Vals[idx]);
    else
        stat->modtime = -1;

    if (info->db.CTime.Vals != NULL)
        stat->createtime = lzmasdkTimeToPhysfsTime(&info->db.CTime.Vals[idx]);
    else if (info->db.MTime.Vals != NULL)
        stat->createtime = lzmasdkTimeToPhysfsTime(&info->db.MTime.Vals[idx]);
    else
        stat->createtime = -1;

    stat->accesstime = -1;
    stat->readonly = 1;

    return 1;
} /* SZIP_stat */


void SZIP_global_init(void)
{
    /* this just needs to calculate some things, so it only ever
       has to run once, even after a deinit. */
    static int generatedTable = 0;
    if (!generatedTable)
    {
        generatedTable = 1;
        CrcGenerateTable();
    } /* if */
} /* SZIP_global_init */


const PHYSFS_Archiver __PHYSFS_Archiver_7Z =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "7Z",
        "7zip archives",
        "Ryan C. Gordon <icculus@icculus.org>",
        "https://icculus.org/physfs/",
        0,  /* supportsSymlinks */
    },
    SZIP_openArchive,
    __PHYSFS_DirTreeEnumerate,
    SZIP_openRead,
    SZIP_openWrite,
    SZIP_openAppend,
    SZIP_remove,
    SZIP_mkdir,
    SZIP_stat,
    SZIP_closeArchive
};

#endif  /* defined PHYSFS_SUPPORTS_7Z */

/* end of physfs_archiver_7z.c ... */

/*
 * Standard directory I/O support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
/* There's no PHYSFS_Io interface here. Use __PHYSFS_createNativeIo(). */



static char *cvtToDependent(const char *prepend, const char *path,
                            char *buf, const size_t buflen)
{
    BAIL_IF(buf == NULL, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    snprintf(buf, buflen, "%s%s", prepend ? prepend : "", path);

    #if !__PHYSFS_STANDARD_DIRSEP
    assert(__PHYSFS_platformDirSeparator != '/');
    {
        char *p;
        for (p = strchr(buf, '/'); p != NULL; p = strchr(p + 1, '/'))
            *p = __PHYSFS_platformDirSeparator;
    } /* if */
    #endif

    return buf;
} /* cvtToDependent */


#define CVT_TO_DEPENDENT(buf, pre, dir) { \
    const size_t len = ((pre) ? strlen((char *) pre) : 0) + strlen(dir) + 1; \
    buf = cvtToDependent((char*)pre,dir,(char*)__PHYSFS_smallAlloc(len),len); \
}



static void *DIR_openArchive(PHYSFS_Io *io, const char *name,
                             int forWriting, int *claimed)
{
    PHYSFS_Stat st;
    const char dirsep = __PHYSFS_platformDirSeparator;
    char *retval = NULL;
    const size_t namelen = strlen(name);
    const size_t seplen = 1;

    assert(io == NULL);  /* shouldn't create an Io for these. */
    BAIL_IF_ERRPASS(!__PHYSFS_platformStat(name, &st, 1), NULL);

    if (st.filetype != PHYSFS_FILETYPE_DIRECTORY)
        BAIL(PHYSFS_ERR_UNSUPPORTED, NULL);

    *claimed = 1;
    retval = (char*)allocator.Malloc(namelen + seplen + 1);
    BAIL_IF(retval == NULL, PHYSFS_ERR_OUT_OF_MEMORY, NULL);

    strcpy(retval, name);

    /* make sure there's a dir separator at the end of the string */
    if (retval[namelen - 1] != dirsep)
    {
        retval[namelen] = dirsep;
        retval[namelen + 1] = '\0';
    } /* if */

    return retval;
} /* DIR_openArchive */


static PHYSFS_EnumerateCallbackResult DIR_enumerate(void *opaque,
                         const char *dname, PHYSFS_EnumerateCallback cb,
                         const char *origdir, void *callbackdata)
{
    char *d;
    PHYSFS_EnumerateCallbackResult retval;
    CVT_TO_DEPENDENT(d, opaque, dname);
    BAIL_IF_ERRPASS(!d, PHYSFS_ENUM_ERROR);
    retval = __PHYSFS_platformEnumerate(d, cb, origdir, callbackdata);
    __PHYSFS_smallFree(d);
    return retval;
} /* DIR_enumerate */


static PHYSFS_Io *doOpen(void *opaque, const char *name, const int mode)
{
    PHYSFS_Io *io = NULL;
    char *f = NULL;

    CVT_TO_DEPENDENT(f, opaque, name);
    BAIL_IF_ERRPASS(!f, NULL);

    io = __PHYSFS_createNativeIo(f, mode);
    if (io == NULL)
    {
        const PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
        PHYSFS_Stat statbuf;
        __PHYSFS_platformStat(f, &statbuf, 0);  /* !!! FIXME: why are we stating here? */
        PHYSFS_setErrorCode(err);
    } /* if */

    __PHYSFS_smallFree(f);

    return io;
} /* doOpen */


static PHYSFS_Io *DIR_openRead(void *opaque, const char *filename)
{
    return doOpen(opaque, filename, 'r');
} /* DIR_openRead */


static PHYSFS_Io *DIR_openWrite(void *opaque, const char *filename)
{
    return doOpen(opaque, filename, 'w');
} /* DIR_openWrite */


static PHYSFS_Io *DIR_openAppend(void *opaque, const char *filename)
{
    return doOpen(opaque, filename, 'a');
} /* DIR_openAppend */


static int DIR_remove(void *opaque, const char *name)
{
    int retval;
    char *f;

    CVT_TO_DEPENDENT(f, opaque, name);
    BAIL_IF_ERRPASS(!f, 0);
    retval = __PHYSFS_platformDelete(f);
    __PHYSFS_smallFree(f);
    return retval;
} /* DIR_remove */


static int DIR_mkdir(void *opaque, const char *name)
{
    int retval;
    char *f;

    CVT_TO_DEPENDENT(f, opaque, name);
    BAIL_IF_ERRPASS(!f, 0);
    retval = __PHYSFS_platformMkDir(f);
    __PHYSFS_smallFree(f);
    return retval;
} /* DIR_mkdir */


static void DIR_closeArchive(void *opaque)
{
    allocator.Free(opaque);
} /* DIR_closeArchive */


static int DIR_stat(void *opaque, const char *name, PHYSFS_Stat *stat)
{
    int retval = 0;
    char *d;

    CVT_TO_DEPENDENT(d, opaque, name);
    BAIL_IF_ERRPASS(!d, 0);
    retval = __PHYSFS_platformStat(d, stat, 0);
    __PHYSFS_smallFree(d);
    return retval;
} /* DIR_stat */


const PHYSFS_Archiver __PHYSFS_Archiver_DIR =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "",
        "Non-archive, direct filesystem I/O",
        "Ryan C. Gordon <icculus@icculus.org>",
        "https://icculus.org/physfs/",
        1,  /* supportsSymlinks */
    },
    DIR_openArchive,
    DIR_enumerate,
    DIR_openRead,
    DIR_openWrite,
    DIR_openAppend,
    DIR_remove,
    DIR_mkdir,
    DIR_stat,
    DIR_closeArchive
};

/* end of physfs_archiver_dir.c ... */

/*
 * GRP support routines for PhysicsFS.
 *
 * This driver handles BUILD engine archives ("groupfiles"). This format
 *  (but not this driver) was put together by Ken Silverman.
 *
 * The format is simple enough. In Ken's words:
 *
 *    What's the .GRP file format?
 *
 *     The ".grp" file format is just a collection of a lot of files stored
 *     into 1 big one. I tried to make the format as simple as possible: The
 *     first 12 bytes contains my name, "KenSilverman". The next 4 bytes is
 *     the number of files that were compacted into the group file. Then for
 *     each file, there is a 16 byte structure, where the first 12 bytes are
 *     the filename, and the last 4 bytes are the file's size. The rest of
 *     the group file is just the raw data packed one after the other in the
 *     same order as the list of files.
 *
 * (That info is from http://www.advsys.net/ken/build.htm ...)
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_GRP

static int grpLoadEntries(PHYSFS_Io *io, const PHYSFS_uint32 count, void *arc)
{
    PHYSFS_uint32 pos = 16 + (16 * count);  /* past sig+metadata. */
    PHYSFS_uint32 i;

    for (i = 0; i < count; i++)
    {
        char *ptr;
        char name[13];
        PHYSFS_uint32 size;
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, name, 12), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &size, 4), 0);

        name[12] = '\0';  /* name isn't null-terminated in file. */
        if ((ptr = strchr(name, ' ')) != NULL)
            *ptr = '\0';  /* trim extra spaces. */

        size = PHYSFS_swapULE32(size);
        BAIL_IF_ERRPASS(!UNPK_addEntry(arc, name, 0, -1, -1, pos, size), 0);

        pos += size;
    } /* for */

    return 1;
} /* grpLoadEntries */


static void *GRP_openArchive(PHYSFS_Io *io, const char *name,
                             int forWriting, int *claimed)
{
    PHYSFS_uint8 buf[12];
    PHYSFS_uint32 count = 0;
    void *unpkarc = NULL;

    assert(io != NULL);  /* shouldn't ever happen. */

    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, buf, sizeof (buf)), NULL);
    if (memcmp(buf, "KenSilverman", sizeof (buf)) != 0)
        BAIL(PHYSFS_ERR_UNSUPPORTED, NULL);

    *claimed = 1;

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &count, sizeof(count)), NULL);
    count = PHYSFS_swapULE32(count);

    unpkarc = UNPK_openArchive(io);
    BAIL_IF_ERRPASS(!unpkarc, NULL);

    if (!grpLoadEntries(io, count, unpkarc))
    {
        UNPK_abandonArchive(unpkarc);
        return NULL;
    } /* if */

    return unpkarc;
} /* GRP_openArchive */


const PHYSFS_Archiver __PHYSFS_Archiver_GRP =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "GRP",
        "Build engine Groupfile format",
        "Ryan C. Gordon <icculus@icculus.org>",
        "https://icculus.org/physfs/",
        0,  /* supportsSymlinks */
    },
    GRP_openArchive,
    UNPK_enumerate,
    UNPK_openRead,
    UNPK_openWrite,
    UNPK_openAppend,
    UNPK_remove,
    UNPK_mkdir,
    UNPK_stat,
    UNPK_closeArchive
};

#endif  /* defined PHYSFS_SUPPORTS_GRP */

/* end of physfs_archiver_grp.c ... */

/*
 * HOG support routines for PhysicsFS.
 *
 * This driver handles Descent I/II/III HOG archives.
 *
 * The Descent I/II format is very simple:
 *
 *   The file always starts with the 3-byte signature "DHF" (Descent
 *   HOG file). After that the files of a HOG are just attached after
 *   another, divided by a 17 bytes header, which specifies the name
 *   and length (in bytes) of the forthcoming file! So you just read
 *   the header with its information of how big the following file is,
 *   and then skip exact that number of bytes to get to the next file
 *   in that HOG.
 *
 *    char sig[3] = {'D', 'H', 'F'}; // "DHF"=Descent HOG File
 *
 *    struct {
 *     char file_name[13]; // Filename, padded to 13 bytes with 0s
 *     int file_size; // filesize in bytes
 *     char data[file_size]; // The file data
 *    } FILE_STRUCT; // Repeated until the end of the file.
 *
 * (That info is from http://www.descent2.com/ddn/specs/hog/)
 *
 * Descent 3 moved to HOG2 format, which starts with the chars "HOG2",
 *  then 32-bits for the number of contained files, 32 bits for the offset
 *  to the first file's data, then 56 bytes of 0xFF (reserved?). Then for
 *  each file, there's 36 bytes for filename (null-terminated, rest of bytes
 *  are garbage), 32-bits unknown/reserved (always zero?), 32-bits of length
 *  of file data, 32-bits of time since Unix epoch. Then immediately following,
 *  for each file is their uncompressed content, you can find its offset
 *  by starting at the initial data offset and adding the filesize of each
 *  prior file.
 *
 * This information was found at:
 *  https://web.archive.org/web/20020213004051/http://descent-3.com/ddn/specs/hog/
 *
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 * This file written by Bradley Bell and Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_HOG

static int hog1LoadEntries(PHYSFS_Io *io, void *arc)
{
    const PHYSFS_uint64 iolen = io->length(io);
    PHYSFS_uint32 pos = 3;

    while (pos < iolen)
    {
        PHYSFS_uint32 size;
        char name[13];

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, name, 13), 0);
        BAIL_IF_ERRPASS(!readui32(io, &size), 0);
        name[12] = '\0';  /* just in case. */
        pos += 13 + 4;

        BAIL_IF_ERRPASS(!UNPK_addEntry(arc, name, 0, -1, -1, pos, size), 0);
        pos += size;

        /* skip over entry */
        BAIL_IF_ERRPASS(!io->seek(io, pos), 0);
    } /* while */

    return 1;
} /* hogLoadEntries */

static int hog2LoadEntries(PHYSFS_Io *io, void *arc)
{
    PHYSFS_uint32 numfiles;
    PHYSFS_uint32 pos;
    PHYSFS_uint32 i;

    BAIL_IF_ERRPASS(!readui32(io, &numfiles), 0);
    BAIL_IF_ERRPASS(!readui32(io, &pos), 0);
    BAIL_IF_ERRPASS(!io->seek(io, 68), 0);  /* skip to end of header. */

    for (i = 0; i < numfiles; i++) {
        char name[37];
        PHYSFS_uint32 reserved;
        PHYSFS_uint32 size;
        PHYSFS_uint32 mtime;
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, name, 36), 0);
        BAIL_IF_ERRPASS(!readui32(io, &reserved), 0);
        BAIL_IF_ERRPASS(!readui32(io, &size), 0);
        BAIL_IF_ERRPASS(!readui32(io, &mtime), 0);
        name[36] = '\0';  /* just in case */
        BAIL_IF_ERRPASS(!UNPK_addEntry(arc, name, 0, mtime, mtime, pos, size), 0);
        pos += size;
    }

    return 1;
} /* hog2LoadEntries */


static void *HOG_openArchive(PHYSFS_Io *io, const char *name,
                             int forWriting, int *claimed)
{
    PHYSFS_uint8 buf[3];
    void *unpkarc = NULL;
    int hog1 = 0;

    assert(io != NULL);  /* shouldn't ever happen. */
    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);
    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, buf, 3), NULL);

    if (memcmp(buf, "DHF", 3) == 0)
        hog1 = 1;  /* original HOG (Descent 1 and 2) archive */
    else
    {
        BAIL_IF(memcmp(buf, "HOG", 3) != 0, PHYSFS_ERR_UNSUPPORTED, NULL); /* Not HOG2 */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, buf, 1), NULL);
        BAIL_IF(buf[0] != '2', PHYSFS_ERR_UNSUPPORTED, NULL); /* Not HOG2 */
    } /* else */

    *claimed = 1;

    unpkarc = UNPK_openArchive(io);
    BAIL_IF_ERRPASS(!unpkarc, NULL);

    if (!(hog1 ? hog1LoadEntries(io, unpkarc) : hog2LoadEntries(io, unpkarc)))
    {
        UNPK_abandonArchive(unpkarc);
        return NULL;
    } /* if */

    return unpkarc;
} /* HOG_openArchive */


const PHYSFS_Archiver __PHYSFS_Archiver_HOG =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "HOG",
        "Descent I/II/III HOG file format",
        "Bradley Bell <btb@icculus.org>",
        "https://icculus.org/physfs/",
        0,  /* supportsSymlinks */
    },
    HOG_openArchive,
    UNPK_enumerate,
    UNPK_openRead,
    UNPK_openWrite,
    UNPK_openAppend,
    UNPK_remove,
    UNPK_mkdir,
    UNPK_stat,
    UNPK_closeArchive
};

#endif  /* defined PHYSFS_SUPPORTS_HOG */

/* end of physfs_archiver_hog.c ... */

/*
 * ISO9660 support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file originally written by Christoph Nelles, but was largely
 *  rewritten by Ryan C. Gordon (so please harass Ryan about bugs and not
 *  Christoph).
 */

/*
 * Handles CD-ROM disk images (and raw CD-ROM devices).
 *
 * Not supported:
 * - Rock Ridge (needed for sparse files, device nodes and symlinks, etc).
 * - Non 2048 Sectors
 * - TRANS.TBL (maps 8.3 filenames on old discs to long filenames).
 * - Multiextents (4gb max file size without it).
 * - UDF
 *
 * Deviations from the standard
 * - Ignores mandatory sort order
 * - Allows various invalid file names
 *
 * Problems
 * - Ambiguities in the standard
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_ISO9660

#include <time.h>

/* ISO9660 often stores values in both big and little endian formats: little
   first, followed by big. While technically there might be different values
   in each, we just always use the littleendian ones and swap ourselves. The
   fields aren't aligned anyhow, so you have to serialize them in any case
   to avoid crashes on many CPU archs in any case. */

static int iso9660LoadEntries(PHYSFS_Io *io, const int joliet,
                              const char *base, const PHYSFS_uint64 dirstart,
                              const PHYSFS_uint64 dirend, void *unpkarc);

static int iso9660AddEntry(PHYSFS_Io *io, const int joliet, const int isdir,
                           const char *base, PHYSFS_uint8 *fname,
                           const int fnamelen, const PHYSFS_sint64 ts,
                           const PHYSFS_uint64 pos, const PHYSFS_uint64 len,
                           void *unpkarc)
{
    char *fullpath;
    char *fnamecpy;
    size_t baselen;
    size_t fullpathlen;
    void *entry;
    int i;

    if (fnamelen == 1 && ((fname[0] == 0) || (fname[0] == 1)))
        return 1;  /* Magic that represents "." and "..", ignore */

    BAIL_IF(fnamelen == 0, PHYSFS_ERR_CORRUPT, 0);
    assert(fnamelen > 0);
    assert(fnamelen <= 255);
    BAIL_IF(joliet && (fnamelen % 2), PHYSFS_ERR_CORRUPT, 0);

    /* Joliet is UCS-2, so at most UTF-8 will double the byte size */
    baselen = strlen(base);
    fullpathlen = baselen + (fnamelen * (joliet ? 2 : 1)) + 2;
    fullpath = (char *) __PHYSFS_smallAlloc(fullpathlen);
    BAIL_IF(!fullpath, PHYSFS_ERR_OUT_OF_MEMORY, 0);
    fnamecpy = fullpath;
    if (baselen > 0)
    {
        snprintf(fullpath, fullpathlen, "%s/", base);
        fnamecpy += baselen + 1;
        fullpathlen -= baselen - 1;
    } /* if */

    if (joliet)
    {
        PHYSFS_uint16 *ucs2 = (PHYSFS_uint16 *) fname;
        int total = fnamelen / 2;
        for (i = 0; i < total; i++)
            ucs2[i] = PHYSFS_swapUBE16(ucs2[i]);
        ucs2[total] = '\0';
        PHYSFS_utf8FromUcs2(ucs2, fnamecpy, fullpathlen);
    } /* if */
    else
    {
        for (i = 0; i < fnamelen; i++)
        {
            /* We assume the filenames are low-ASCII; consider the archive
               corrupt if we see something above 127, since we don't know the
               encoding. (We can change this later if we find out these exist
               and are intended to be, say, latin-1 or UTF-8 encoding). */
            BAIL_IF(fname[i] > 127, PHYSFS_ERR_CORRUPT, 0);
            fnamecpy[i] = fname[i];
        } /* for */
        fnamecpy[fnamelen] = '\0';

        if (!isdir)
        {
            /* find last SEPARATOR2 */
            char *ptr = strrchr(fnamecpy, ';');
            if (ptr && (ptr != fnamecpy))
                *(ptr--) = '\0';
            else
                ptr = fnamecpy + (fnamelen - 1);

            /* chop out any trailing '.', as done in all implementations */
            if (*ptr == '.')
                *ptr = '\0';
        } /* if */
    } /* else */

    entry = UNPK_addEntry(unpkarc, fullpath, isdir, ts, ts, pos, len);
    if ((entry) && (isdir))
    {
        if (!iso9660LoadEntries(io, joliet, fullpath, pos, pos + len, unpkarc))
            entry = NULL;  /* so we report a failure later. */
    } /* if */

    __PHYSFS_smallFree(fullpath);
    return entry != NULL;
} /* iso9660AddEntry */

static int iso9660LoadEntries(PHYSFS_Io *io, const int joliet,
                              const char *base, const PHYSFS_uint64 dirstart,
                              const PHYSFS_uint64 dirend, void *unpkarc)
{
    PHYSFS_uint64 readpos = dirstart;

    while (1)
    {
        PHYSFS_uint8 recordlen;
        PHYSFS_uint8 extattrlen;
        PHYSFS_uint32 extent;
        PHYSFS_uint32 datalen;
        PHYSFS_uint8 ignore[4];
        PHYSFS_uint8 year, month, day, hour, minute, second, offset;
        PHYSFS_uint8 flags;
        PHYSFS_uint8 fnamelen;
        PHYSFS_uint8 fname[256];
        PHYSFS_sint64 timestamp;
        struct tm t;
        int isdir;
        int multiextent;

        BAIL_IF_ERRPASS(!io->seek(io, readpos), 0);

        /* recordlen = 0 -> no more entries or fill entry */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &recordlen, 1), 0);
        if (recordlen > 0)
            readpos += recordlen;  /* ready to seek to next record. */
        else
        {
            PHYSFS_uint64 nextpos;

            /* if we are in the last sector of the directory & it's 0 -> end */
            if ((dirend - 2048) <= (readpos - 1))
                break; /* finished */

            /* else skip to the next sector & continue; */
            nextpos = (((readpos - 1) / 2048) + 1) * 2048;

            /* whoops, can't make forward progress! */
            BAIL_IF(nextpos == readpos, PHYSFS_ERR_CORRUPT, 0);

            readpos = nextpos;
            continue;  /* start back at upper loop. */
        } /* else */

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &extattrlen, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &extent, 4), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* extent be */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &datalen, 4), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* datalen be */

        /* record timestamp */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &year, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &month, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &day, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &hour, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &minute, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &second, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &offset, 1), 0);

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &flags, 1), 0);
        isdir = (flags & (1 << 1)) != 0;
        multiextent = (flags & (1 << 7)) != 0;
        BAIL_IF(multiextent, PHYSFS_ERR_UNSUPPORTED, 0);  /* !!! FIXME */

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 1), 0); /* unit size */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 1), 0); /* interleave gap */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 2), 0); /* seqnum le */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 2), 0); /* seqnum be */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &fnamelen, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, fname, fnamelen), 0);

        t.tm_sec = second;
        t.tm_min = minute;
        t.tm_hour = hour;
        t.tm_mday = day;
        t.tm_mon = month - 1;
        t.tm_year = year;
        t.tm_wday = 0;
        t.tm_yday = 0;
        t.tm_isdst = -1;
        timestamp = (PHYSFS_sint64) mktime(&t);

        extent += extattrlen;  /* skip extended attribute record. */

        /* infinite loop, corrupt file? */
        BAIL_IF((extent * 2048) == dirstart, PHYSFS_ERR_CORRUPT, 0);

        if (!iso9660AddEntry(io, joliet, isdir, base, fname, fnamelen,
                             timestamp, extent * 2048, datalen, unpkarc))
        {
            return 0;
        } /* if */
    } /* while */

    return 1;
} /* iso9660LoadEntries */


static int parseVolumeDescriptor(PHYSFS_Io *io, PHYSFS_uint64 *_rootpos,
                                 PHYSFS_uint64 *_rootlen, int *_joliet,
                                 int *_claimed)
{
    PHYSFS_uint64 pos = 32768; /* start at the Primary Volume Descriptor */
    int found = 0;
    int done = 0;

    *_joliet = 0;

    while (!done)
    {
        PHYSFS_uint8 type;
        PHYSFS_uint8 identifier[5];
        PHYSFS_uint8 version;
        PHYSFS_uint8 flags;
        PHYSFS_uint8 escapeseqs[32];
        PHYSFS_uint8 ignore[32];
        PHYSFS_uint16 blocksize;
        PHYSFS_uint32 extent;
        PHYSFS_uint32 datalen;

        BAIL_IF_ERRPASS(!io->seek(io, pos), 0);
        pos += 2048;  /* each volume descriptor is 2048 bytes */

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &type, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &identifier, 5), 0);

        if (memcmp(identifier, "CD001", 5) != 0)  /* maybe not an iso? */
        {
            BAIL_IF(!*_claimed, PHYSFS_ERR_UNSUPPORTED, 0);
            continue;  /* just skip this one */
        } /* if */

        *_claimed = 1; /* okay, this is probably an iso. */

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &version, 1), 0);  /* version */
        BAIL_IF(version != 1, PHYSFS_ERR_UNSUPPORTED, 0);

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &flags, 1), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 32), 0);  /* system id */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 32), 0);  /* volume id */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 8), 0);  /* reserved */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0);  /* space le */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0);  /* space be */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, escapeseqs, 32), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 2), 0);  /* setsize le */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 2), 0);  /* setsize be */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 2), 0);  /* seq num le */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 2), 0);  /* seq num be */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &blocksize, 2), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 2), 0); /* blocklen be */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* pthtablen le */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* pthtablen be */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* pthtabpos le */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* optpthtabpos le */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* pthtabpos be */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* optpthtabpos be */

        /* root directory record... */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 1), 0); /* len */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 1), 0); /* attr len */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &extent, 4), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* extent be */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &datalen, 4), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), 0); /* datalen be */

        /* !!! FIXME: deal with this properly. */
        blocksize = PHYSFS_swapULE32(blocksize);
        BAIL_IF(blocksize && (blocksize != 2048), PHYSFS_ERR_UNSUPPORTED, 0);

        switch (type)
        {
            case 1:  /* Primary Volume Descriptor */
            case 2:  /* Supplementary Volume Descriptor */
                if (found < type)
                {
                    *_rootpos = PHYSFS_swapULE32(extent) * 2048;
                    *_rootlen = PHYSFS_swapULE32(datalen);
                    found = type;

                    if (found == 2)  /* possible Joliet volume */
                    {
                        const PHYSFS_uint8 *s = escapeseqs;
                        *_joliet = !(flags & 1) &&
                            (s[0] == 0x25) && (s[1] == 0x2F) &&
                            ((s[2] == 0x40) || (s[2] == 0x43) || (s[2] == 0x45));
                    } /* if */
                } /* if */
                break;

            case 255: /* type 255 terminates the volume descriptor list */
                done = 1;
                break;

            default:
                break;  /* skip unknown types. */
        } /* switch */
    } /* while */

    BAIL_IF(!found, PHYSFS_ERR_CORRUPT, 0);

    return 1;
} /* parseVolumeDescriptor */


static void *ISO9660_openArchive(PHYSFS_Io *io, const char *filename,
                                 int forWriting, int *claimed)
{
    PHYSFS_uint64 rootpos = 0;
    PHYSFS_uint64 len = 0;
    int joliet = 0;
    void *unpkarc = NULL;

    assert(io != NULL);  /* shouldn't ever happen. */

    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);

    if (!parseVolumeDescriptor(io, &rootpos, &len, &joliet, claimed))
        return NULL;

    unpkarc = UNPK_openArchive(io);
    BAIL_IF_ERRPASS(!unpkarc, NULL);

    if (!iso9660LoadEntries(io, joliet, "", rootpos, rootpos + len, unpkarc))
    {
        UNPK_abandonArchive(unpkarc);
        return NULL;
    } /* if */

    return unpkarc;
} /* ISO9660_openArchive */


const PHYSFS_Archiver __PHYSFS_Archiver_ISO9660 =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "ISO",
        "ISO9660 image file",
        "Ryan C. Gordon <icculus@icculus.org>",
        "https://icculus.org/physfs/",
        0,  /* supportsSymlinks */
    },
    ISO9660_openArchive,
    UNPK_enumerate,
    UNPK_openRead,
    UNPK_openWrite,
    UNPK_openAppend,
    UNPK_remove,
    UNPK_mkdir,
    UNPK_stat,
    UNPK_closeArchive
};

#endif  /* defined PHYSFS_SUPPORTS_ISO9660 */

/* end of physfs_archiver_iso9660.c ... */

/*
 * MVL support routines for PhysicsFS.
 *
 * This driver handles Descent II Movielib archives.
 *
 * The file format of MVL is quite easy...
 *
 *   //MVL File format - Written by Heiko Herrmann
 *   char sig[4] = {'D','M', 'V', 'L'}; // "DMVL"=Descent MoVie Library
 *
 *   int num_files; // the number of files in this MVL
 *
 *   struct {
 *    char file_name[13]; // Filename, padded to 13 bytes with 0s
 *    int file_size; // filesize in bytes
 *   }DIR_STRUCT[num_files];
 *
 *   struct {
 *    char data[file_size]; // The file data
 *   }FILE_STRUCT[num_files];
 *
 * (That info is from http://www.descent2.com/ddn/specs/mvl/)
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Bradley Bell.
 *  Based on grp.c by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_MVL

static int mvlLoadEntries(PHYSFS_Io *io, const PHYSFS_uint32 count, void *arc)
{
    PHYSFS_uint32 pos = 8 + (17 * count);   /* past sig+metadata. */
    PHYSFS_uint32 i;

    for (i = 0; i < count; i++)
    {
        PHYSFS_uint32 size;
        char name[13];
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, name, 13), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &size, 4), 0);
        name[12] = '\0';  /* just in case. */
        size = PHYSFS_swapULE32(size);
        BAIL_IF_ERRPASS(!UNPK_addEntry(arc, name, 0, -1, -1, pos, size), 0);
        pos += size;
    } /* for */

    return 1;
} /* mvlLoadEntries */


static void *MVL_openArchive(PHYSFS_Io *io, const char *name,
                             int forWriting, int *claimed)
{
    PHYSFS_uint8 buf[4];
    PHYSFS_uint32 count = 0;
    void *unpkarc;

    assert(io != NULL);  /* shouldn't ever happen. */
    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);
    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, buf, 4), NULL);
    BAIL_IF(memcmp(buf, "DMVL", 4) != 0, PHYSFS_ERR_UNSUPPORTED, NULL);

    *claimed = 1;

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &count, sizeof(count)), NULL);
    count = PHYSFS_swapULE32(count);

    unpkarc = UNPK_openArchive(io);
    BAIL_IF_ERRPASS(!unpkarc, NULL);

    if (!mvlLoadEntries(io, count, unpkarc))
    {
        UNPK_abandonArchive(unpkarc);
        return NULL;
    } /* if */

    return unpkarc;
} /* MVL_openArchive */


const PHYSFS_Archiver __PHYSFS_Archiver_MVL =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "MVL",
        "Descent II Movielib format",
        "Bradley Bell <btb@icculus.org>",
        "https://icculus.org/physfs/",
        0,  /* supportsSymlinks */
    },
    MVL_openArchive,
    UNPK_enumerate,
    UNPK_openRead,
    UNPK_openWrite,
    UNPK_openAppend,
    UNPK_remove,
    UNPK_mkdir,
    UNPK_stat,
    UNPK_closeArchive
};

#endif  /* defined PHYSFS_SUPPORTS_MVL */

/* end of physfs_archiver_mvl.c ... */

/*
 * QPAK support routines for PhysicsFS.
 *
 *  This archiver handles the archive format utilized by Quake 1 and 2.
 *  Quake3-based games use the PkZip/Info-Zip format (which our
 *  physfs_archiver_zip.c handles).
 *
 *  ========================================================================
 *
 *  This format info (in more detail) comes from:
 *     https://web.archive.org/web/20040209101748/http://debian.fmi.uni-sofia.bg/~sergei/cgsr/docs/pak.txt
 *
 *  Quake PAK Format
 *
 *  Header
 *   (4 bytes)  signature = 'PACK'
 *   (4 bytes)  directory offset
 *   (4 bytes)  directory length
 *
 *  Directory
 *   (56 bytes) file name
 *   (4 bytes)  file position
 *   (4 bytes)  file length
 *
 *  ========================================================================
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_QPAK

#define QPAK_SIG 0x4B434150   /* "PACK" in ASCII. */

static int qpakLoadEntries(PHYSFS_Io *io, const PHYSFS_uint32 count, void *arc)
{
    PHYSFS_uint32 i;
    for (i = 0; i < count; i++)
    {
        PHYSFS_uint32 size;
        PHYSFS_uint32 pos;
        char name[56];
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, name, 56), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &pos, 4), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &size, 4), 0);
        size = PHYSFS_swapULE32(size);
        pos = PHYSFS_swapULE32(pos);
        BAIL_IF_ERRPASS(!UNPK_addEntry(arc, name, 0, -1, -1, pos, size), 0);
    } /* for */

    return 1;
} /* qpakLoadEntries */


static void *QPAK_openArchive(PHYSFS_Io *io, const char *name,
                              int forWriting, int *claimed)
{
    PHYSFS_uint32 val = 0;
    PHYSFS_uint32 pos = 0;
    PHYSFS_uint32 count = 0;
    void *unpkarc;

    assert(io != NULL);  /* shouldn't ever happen. */

    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &val, 4), NULL);
    if (PHYSFS_swapULE32(val) != QPAK_SIG)
        BAIL(PHYSFS_ERR_UNSUPPORTED, NULL);

    *claimed = 1;

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &val, 4), NULL);
    pos = PHYSFS_swapULE32(val);  /* directory table offset. */

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &val, 4), NULL);
    count = PHYSFS_swapULE32(val);

    /* corrupted archive? */
    BAIL_IF((count % 64) != 0, PHYSFS_ERR_CORRUPT, NULL);
    count /= 64;

    BAIL_IF_ERRPASS(!io->seek(io, pos), NULL);

    unpkarc = UNPK_openArchive(io);
    BAIL_IF_ERRPASS(!unpkarc, NULL);

    if (!qpakLoadEntries(io, count, unpkarc))
    {
        UNPK_abandonArchive(unpkarc);
        return NULL;
    } /* if */

    return unpkarc;
} /* QPAK_openArchive */


const PHYSFS_Archiver __PHYSFS_Archiver_QPAK =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "PAK",
        "Quake I/II format",
        "Ryan C. Gordon <icculus@icculus.org>",
        "https://icculus.org/physfs/",
        0,  /* supportsSymlinks */
    },
    QPAK_openArchive,
    UNPK_enumerate,
    UNPK_openRead,
    UNPK_openWrite,
    UNPK_openAppend,
    UNPK_remove,
    UNPK_mkdir,
    UNPK_stat,
    UNPK_closeArchive
};

#endif  /* defined PHYSFS_SUPPORTS_QPAK */

/* end of physfs_archiver_qpak.c ... */

/*
 * SLB support routines for PhysicsFS.
 *
 * This driver handles SLB archives ("slab files"). This uncompressed format
 * is used in I-War / Independence War and Independence War: Defiance.
 *
 * The format begins with four zero bytes (version?), the file count and the
 * location of the table of contents. Each ToC entry contains a 64-byte buffer
 * containing a zero-terminated filename, the offset of the data, and its size.
 * All the filenames begin with the separator character '\'.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 * This file written by Aleksi Nurmi, based on the GRP archiver by
 * Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_SLB

static int slbLoadEntries(PHYSFS_Io *io, const PHYSFS_uint32 count, void *arc)
{
    PHYSFS_uint32 i;
    for (i = 0; i < count; i++)
    {
        PHYSFS_uint32 pos;
        PHYSFS_uint32 size;
        char name[64];
        char backslash;
        char *ptr;

        /* don't include the '\' in the beginning */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &backslash, 1), 0);
        BAIL_IF(backslash != '\\', PHYSFS_ERR_CORRUPT, 0);

        /* read the rest of the buffer, 63 bytes */
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &name, 63), 0);
        name[63] = '\0'; /* in case the name lacks the null terminator */

        /* convert backslashes */
        for (ptr = name; *ptr; ptr++)
        {
            if (*ptr == '\\')
                *ptr = '/';
        } /* for */

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &pos, 4), 0);
        pos = PHYSFS_swapULE32(pos);

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &size, 4), 0);
        size = PHYSFS_swapULE32(size);

        BAIL_IF_ERRPASS(!UNPK_addEntry(arc, name, 0, -1, -1, pos, size), 0);
    } /* for */

    return 1;
} /* slbLoadEntries */


static void *SLB_openArchive(PHYSFS_Io *io, const char *name,
                             int forWriting, int *claimed)
{
    PHYSFS_uint32 version;
    PHYSFS_uint32 count;
    PHYSFS_uint32 tocPos;
    void *unpkarc;

    /* There's no identifier on an SLB file, so we assume it's _not_ if the
       file count or tocPos is zero. Beyond that, we'll assume it's
       bogus/corrupt if the entries' filenames don't start with '\' or the
       tocPos is past the end of the file (seek will fail). This probably
       covers all meaningful cases where we would accidentally accept a non-SLB
       file with this archiver. */

    assert(io != NULL);  /* shouldn't ever happen. */

    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &version, sizeof (version)), NULL);
    version = PHYSFS_swapULE32(version);
    BAIL_IF(version != 0, PHYSFS_ERR_UNSUPPORTED, NULL);

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &count, sizeof (count)), NULL);
    count = PHYSFS_swapULE32(count);
    BAIL_IF(!count, PHYSFS_ERR_UNSUPPORTED, NULL);

    /* offset of the table of contents */
    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &tocPos, sizeof (tocPos)), NULL);
    tocPos = PHYSFS_swapULE32(tocPos);
    BAIL_IF(!tocPos, PHYSFS_ERR_UNSUPPORTED, NULL);

    /* seek to the table of contents */
    BAIL_IF_ERRPASS(!io->seek(io, tocPos), NULL);

    unpkarc = UNPK_openArchive(io);
    BAIL_IF_ERRPASS(!unpkarc, NULL);

    if (!slbLoadEntries(io, count, unpkarc))
    {
        UNPK_abandonArchive(unpkarc);
        return NULL;
    } /* if */

    *claimed = 1;  /* oh well. */

    return unpkarc;
} /* SLB_openArchive */


const PHYSFS_Archiver __PHYSFS_Archiver_SLB =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "SLB",
        "I-War / Independence War Slab file",
        "Aleksi Nurmi <aleksi.nurmi@gmail.com>",
        "https://bitbucket.org/ahnurmi/",
        0,  /* supportsSymlinks */
    },
    SLB_openArchive,
    UNPK_enumerate,
    UNPK_openRead,
    UNPK_openWrite,
    UNPK_openAppend,
    UNPK_remove,
    UNPK_mkdir,
    UNPK_stat,
    UNPK_closeArchive
};

#endif  /* defined PHYSFS_SUPPORTS_SLB */

/* end of physfs_archiver_slb.c ... */
/*
 * High-level PhysicsFS archiver for simple unpacked file formats.
 *
 * This is a framework that basic archivers build on top of. It's for simple
 *  formats that can just hand back a list of files and the offsets of their
 *  uncompressed data. There are an alarming number of formats like this.
 *
 * RULES: Archive entries must be uncompressed. Dirs and files allowed, but no
 *  symlinks, etc. We can relax some of these rules as necessary.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
typedef struct
{
    __PHYSFS_DirTree tree;
    PHYSFS_Io *io;
} UNPKinfo;

typedef struct
{
    __PHYSFS_DirTreeEntry tree;
    PHYSFS_uint64 startPos;
    PHYSFS_uint64 size;
    PHYSFS_sint64 ctime;
    PHYSFS_sint64 mtime;
} UNPKentry;

typedef struct
{
    PHYSFS_Io *io;
    UNPKentry *entry;
    PHYSFS_uint32 curPos;
} UNPKfileinfo;


void UNPK_closeArchive(void *opaque)
{
    UNPKinfo *info = ((UNPKinfo *) opaque);
    if (info)
    {
        __PHYSFS_DirTreeDeinit(&info->tree);

        if (info->io)
            info->io->destroy(info->io);

        allocator.Free(info);
    } /* if */
} /* UNPK_closeArchive */

void UNPK_abandonArchive(void *opaque)
{
    UNPKinfo *info = ((UNPKinfo *) opaque);
    if (info)
    {
        info->io = NULL;
        UNPK_closeArchive(info);
    } /* if */
} /* UNPK_abandonArchive */

static PHYSFS_sint64 UNPK_read(PHYSFS_Io *io, void *buffer, PHYSFS_uint64 len)
{
    UNPKfileinfo *finfo = (UNPKfileinfo *) io->opaque;
    const UNPKentry *entry = finfo->entry;
    const PHYSFS_uint64 bytesLeft = (PHYSFS_uint64)(entry->size-finfo->curPos);
    PHYSFS_sint64 rc;

    if (bytesLeft < len)
        len = bytesLeft;

    rc = finfo->io->read(finfo->io, buffer, len);
    if (rc > 0)
        finfo->curPos += (PHYSFS_uint32) rc;

    return rc;
} /* UNPK_read */


static PHYSFS_sint64 UNPK_write(PHYSFS_Io *io, const void *b, PHYSFS_uint64 len)
{
    BAIL(PHYSFS_ERR_READ_ONLY, -1);
} /* UNPK_write */


static PHYSFS_sint64 UNPK_tell(PHYSFS_Io *io)
{
    return ((UNPKfileinfo *) io->opaque)->curPos;
} /* UNPK_tell */


static int UNPK_seek(PHYSFS_Io *io, PHYSFS_uint64 offset)
{
    UNPKfileinfo *finfo = (UNPKfileinfo *) io->opaque;
    const UNPKentry *entry = finfo->entry;
    int rc;

    BAIL_IF(offset >= entry->size, PHYSFS_ERR_PAST_EOF, 0);
    rc = finfo->io->seek(finfo->io, entry->startPos + offset);
    if (rc)
        finfo->curPos = (PHYSFS_uint32) offset;

    return rc;
} /* UNPK_seek */


static PHYSFS_sint64 UNPK_length(PHYSFS_Io *io)
{
    const UNPKfileinfo *finfo = (UNPKfileinfo *) io->opaque;
    return ((PHYSFS_sint64) finfo->entry->size);
} /* UNPK_length */


static PHYSFS_Io *UNPK_duplicate(PHYSFS_Io *_io)
{
    UNPKfileinfo *origfinfo = (UNPKfileinfo *) _io->opaque;
    PHYSFS_Io *io = NULL;
    PHYSFS_Io *retval = (PHYSFS_Io *) allocator.Malloc(sizeof (PHYSFS_Io));
    UNPKfileinfo *finfo = (UNPKfileinfo *) allocator.Malloc(sizeof (UNPKfileinfo));
    GOTO_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, UNPK_duplicate_failed);
    GOTO_IF(!finfo, PHYSFS_ERR_OUT_OF_MEMORY, UNPK_duplicate_failed);

    io = origfinfo->io->duplicate(origfinfo->io);
    if (!io) goto UNPK_duplicate_failed;
    finfo->io = io;
    finfo->entry = origfinfo->entry;
    finfo->curPos = 0;
    memcpy(retval, _io, sizeof (PHYSFS_Io));
    retval->opaque = finfo;
    return retval;

UNPK_duplicate_failed:
    if (finfo != NULL) allocator.Free(finfo);
    if (retval != NULL) allocator.Free(retval);
    if (io != NULL) io->destroy(io);
    return NULL;
} /* UNPK_duplicate */

static int UNPK_flush(PHYSFS_Io *io) { return 1;  /* no write support. */ }

static void UNPK_destroy(PHYSFS_Io *io)
{
    UNPKfileinfo *finfo = (UNPKfileinfo *) io->opaque;
    finfo->io->destroy(finfo->io);
    allocator.Free(finfo);
    allocator.Free(io);
} /* UNPK_destroy */


static const PHYSFS_Io UNPK_Io =
{
    CURRENT_PHYSFS_IO_API_VERSION, NULL,
    UNPK_read,
    UNPK_write,
    UNPK_seek,
    UNPK_tell,
    UNPK_length,
    UNPK_duplicate,
    UNPK_flush,
    UNPK_destroy
};


static inline UNPKentry *findEntry(UNPKinfo *info, const char *path)
{
    return (UNPKentry *) __PHYSFS_DirTreeFind(&info->tree, path);
} /* findEntry */


PHYSFS_Io *UNPK_openRead(void *opaque, const char *name)
{
    PHYSFS_Io *retval = NULL;
    UNPKinfo *info = (UNPKinfo *) opaque;
    UNPKfileinfo *finfo = NULL;
    UNPKentry *entry = findEntry(info, name);

    BAIL_IF_ERRPASS(!entry, NULL);
    BAIL_IF(entry->tree.isdir, PHYSFS_ERR_NOT_A_FILE, NULL);

    retval = (PHYSFS_Io *) allocator.Malloc(sizeof (PHYSFS_Io));
    GOTO_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, UNPK_openRead_failed);

    finfo = (UNPKfileinfo *) allocator.Malloc(sizeof (UNPKfileinfo));
    GOTO_IF(!finfo, PHYSFS_ERR_OUT_OF_MEMORY, UNPK_openRead_failed);

    finfo->io = info->io->duplicate(info->io);
    GOTO_IF_ERRPASS(!finfo->io, UNPK_openRead_failed);

    if (!finfo->io->seek(finfo->io, entry->startPos))
        goto UNPK_openRead_failed;

    finfo->curPos = 0;
    finfo->entry = entry;

    memcpy(retval, &UNPK_Io, sizeof (*retval));
    retval->opaque = finfo;
    return retval;

UNPK_openRead_failed:
    if (finfo != NULL)
    {
        if (finfo->io != NULL)
            finfo->io->destroy(finfo->io);
        allocator.Free(finfo);
    } /* if */

    if (retval != NULL)
        allocator.Free(retval);

    return NULL;
} /* UNPK_openRead */


PHYSFS_Io *UNPK_openWrite(void *opaque, const char *name)
{
    BAIL(PHYSFS_ERR_READ_ONLY, NULL);
} /* UNPK_openWrite */


PHYSFS_Io *UNPK_openAppend(void *opaque, const char *name)
{
    BAIL(PHYSFS_ERR_READ_ONLY, NULL);
} /* UNPK_openAppend */


int UNPK_remove(void *opaque, const char *name)
{
    BAIL(PHYSFS_ERR_READ_ONLY, 0);
} /* UNPK_remove */


int UNPK_mkdir(void *opaque, const char *name)
{
    BAIL(PHYSFS_ERR_READ_ONLY, 0);
} /* UNPK_mkdir */


int UNPK_stat(void *opaque, const char *path, PHYSFS_Stat *stat)
{
    UNPKinfo *info = (UNPKinfo *) opaque;
    const UNPKentry *entry = findEntry(info, path);

    BAIL_IF_ERRPASS(!entry, 0);

    if (entry->tree.isdir)
    {
        stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
        stat->filesize = 0;
    } /* if */
    else
    {
        stat->filetype = PHYSFS_FILETYPE_REGULAR;
        stat->filesize = entry->size;
    } /* else */

    stat->modtime = entry->mtime;
    stat->createtime = entry->ctime;
    stat->accesstime = -1;
    stat->readonly = 1;

    return 1;
} /* UNPK_stat */


void *UNPK_addEntry(void *opaque, char *name, const int isdir,
                    const PHYSFS_sint64 ctime, const PHYSFS_sint64 mtime,
                    const PHYSFS_uint64 pos, const PHYSFS_uint64 len)
{
    UNPKinfo *info = (UNPKinfo *) opaque;
    UNPKentry *entry;

    entry = (UNPKentry *) __PHYSFS_DirTreeAdd(&info->tree, name, isdir);
    BAIL_IF_ERRPASS(!entry, NULL);

    entry->startPos = isdir ? 0 : pos;
    entry->size = isdir ? 0 : len;
    entry->ctime = ctime;
    entry->mtime = mtime;

    return entry;
} /* UNPK_addEntry */


void *UNPK_openArchive(PHYSFS_Io *io)
{
    UNPKinfo *info = (UNPKinfo *) allocator.Malloc(sizeof (UNPKinfo));
    BAIL_IF(!info, PHYSFS_ERR_OUT_OF_MEMORY, NULL);

    if (!__PHYSFS_DirTreeInit(&info->tree, sizeof (UNPKentry)))
    {
        allocator.Free(info);
        return NULL;
    } /* if */

    info->io = io;

    return info;
} /* UNPK_openArchive */

/* end of physfs_archiver_unpacked.c ... */

/*
 * VDF support routines for PhysicsFS.
 *
 * This driver handles Gothic I/II VDF archives.
 * This format (but not this driver) was designed by Piranha Bytes for
 *  use wih the ZenGin engine.
 *
 * This file was written by Francesco Bertolaccini, based on the UNPK archiver
 *  by Ryan C. Gordon and the works of degenerated1123 and Nico Bendlin.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_VDF

#include <time.h>

#define VDF_COMMENT_LENGTH 256
#define VDF_SIGNATURE_LENGTH 16
#define VDF_ENTRY_NAME_LENGTH 64
#define VDF_ENTRY_DIR 0x80000000

static const char* VDF_SIGNATURE_G1 = "PSVDSC_V2.00\r\n\r\n";
static const char* VDF_SIGNATURE_G2 = "PSVDSC_V2.00\n\r\n\r";


static PHYSFS_sint64 vdfDosTimeToEpoch(const PHYSFS_uint32 dostime)
{
    /* VDF stores timestamps as 32bit DOS dates: the seconds are counted in
       2-seconds intervals and the years are counted since 1 Jan. 1980 */
    struct tm t;
    memset(&t, '\0', sizeof (t));
    t.tm_year = ((int) ((dostime >> 25) & 0x7F)) + 80; /* 1980 to 1900 */
    t.tm_mon = ((int) ((dostime >> 21) & 0xF)) - 1;  /* 1-12 to 0-11 */
    t.tm_mday = (int) ((dostime >> 16) & 0x1F);
    t.tm_hour = (int) ((dostime >> 11) & 0x1F);
    t.tm_min = (int) ((dostime >> 5) & 0x3F);
    t.tm_sec = ((int) ((dostime >> 0) & 0x1F)) * 2;  /* 2 seconds to 1. */
    return (PHYSFS_sint64) mktime(&t);
} /* vdfDosTimeToEpoch */


static int vdfLoadEntries(PHYSFS_Io *io, const PHYSFS_uint32 count,
                          const PHYSFS_sint64 ts, void *arc)
{
    PHYSFS_uint32 i;

    for (i = 0; i < count; i++)
    {
        char name[VDF_ENTRY_NAME_LENGTH + 1];
        int namei;
        PHYSFS_uint32 jump, size, type, attr;

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, name, sizeof (name) - 1), 0);
        BAIL_IF_ERRPASS(!readui32(io, &jump), 0);
        BAIL_IF_ERRPASS(!readui32(io, &size), 0);
        BAIL_IF_ERRPASS(!readui32(io, &type), 0);
        BAIL_IF_ERRPASS(!readui32(io, &attr), 0);

        /* Trim whitespace off the end of the filename */
        name[VDF_ENTRY_NAME_LENGTH] = '\0';  /* always null-terminated. */
        for (namei = VDF_ENTRY_NAME_LENGTH - 1; namei >= 0; namei--)
        {
            /* We assume the filenames are low-ASCII; consider the archive
               corrupt if we see something above 127, since we don't know the
               encoding. (We can change this later if we find out these exist
               and are intended to be, say, latin-1 or UTF-8 encoding). */
            BAIL_IF(((PHYSFS_uint8) name[namei]) > 127, PHYSFS_ERR_CORRUPT, 0);

            if (name[namei] == ' ')
                name[namei] = '\0';
            else
                break;
        } /* for */

        BAIL_IF(!name[0], PHYSFS_ERR_CORRUPT, 0);
        if (!(type & VDF_ENTRY_DIR)) {
            BAIL_IF_ERRPASS(!UNPK_addEntry(arc, name, 0, ts, ts, jump, size), 0);
        }
    } /* for */

    return 1;
} /* vdfLoadEntries */


static void *VDF_openArchive(PHYSFS_Io *io, const char *name,
                             int forWriting, int *claimed)
{
    PHYSFS_uint8 ignore[16];
    PHYSFS_uint8 sig[VDF_SIGNATURE_LENGTH];
    PHYSFS_uint32 count, timestamp, version, dataSize, rootCatOffset;
    void *unpkarc;

    assert(io != NULL); /* shouldn't ever happen. */

    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);

    /* skip the 256-byte comment field. */
    BAIL_IF_ERRPASS(!io->seek(io, VDF_COMMENT_LENGTH), NULL);

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, sig, sizeof (sig)), NULL);

    if ((memcmp(sig, VDF_SIGNATURE_G1, VDF_SIGNATURE_LENGTH) != 0) &&
        (memcmp(sig, VDF_SIGNATURE_G2, VDF_SIGNATURE_LENGTH) != 0))
    {
        BAIL(PHYSFS_ERR_UNSUPPORTED, NULL);
    } /* if */

    *claimed = 1;

    BAIL_IF_ERRPASS(!readui32(io, &count), NULL);
    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, ignore, 4), NULL);  /* numFiles */
    BAIL_IF_ERRPASS(!readui32(io, &timestamp), NULL);
    BAIL_IF_ERRPASS(!readui32(io, &dataSize), NULL);  /* dataSize */
    BAIL_IF_ERRPASS(!readui32(io, &rootCatOffset), NULL);  /* rootCatOff */
    BAIL_IF_ERRPASS(!readui32(io, &version), NULL);

    BAIL_IF(version != 0x50, PHYSFS_ERR_UNSUPPORTED, NULL);

    BAIL_IF_ERRPASS(!io->seek(io, rootCatOffset), NULL);

    unpkarc = UNPK_openArchive(io);
    BAIL_IF_ERRPASS(!unpkarc, NULL);

    if (!vdfLoadEntries(io, count, vdfDosTimeToEpoch(timestamp), unpkarc))
    {
        UNPK_abandonArchive(unpkarc);
        return NULL;
    } /* if */

    return unpkarc;
} /* VDF_openArchive */


const PHYSFS_Archiver __PHYSFS_Archiver_VDF =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "VDF",
        "Gothic I/II engine format",
        "Francesco Bertolaccini <bertolaccinifrancesco@gmail.com>",
        "https://github.com/frabert",
        0,  /* supportsSymlinks */
    },
    VDF_openArchive,
    UNPK_enumerate,
    UNPK_openRead,
    UNPK_openWrite,
    UNPK_openAppend,
    UNPK_remove,
    UNPK_mkdir,
    UNPK_stat,
    UNPK_closeArchive
};

#endif /* defined PHYSFS_SUPPORTS_VDF */

/* end of physfs_archiver_vdf.c ... */
/*
 * WAD support routines for PhysicsFS.
 *
 * This driver handles DOOM engine archives ("wads").
 * This format (but not this driver) was designed by id Software for use
 *  with the DOOM engine.
 * The specs of the format are from the unofficial doom specs v1.666
 * found here: http://www.gamers.org/dhs/helpdocs/dmsp1666.html
 * The format of the archive: (from the specs)
 *
 *  A WAD file has three parts:
 *  (1) a twelve-byte header
 *  (2) one or more "lumps"
 *  (3) a directory or "info table" that contains the names, offsets, and
 *      sizes of all the lumps in the WAD
 *
 *  The header consists of three four-byte parts:
 *    (a) an ASCII string which must be either "IWAD" or "PWAD"
 *    (b) a uint32 which is the number of lumps in the wad
 *    (c) a uint32 which is the file offset to the start of
 *    the directory
 *
 *  The directory has one 16-byte entry for every lump. Each entry consists
 *  of three parts:
 *
 *    (a) a uint32, the file offset to the start of the lump
 *    (b) a uint32, the size of the lump in bytes
 *    (c) an 8-byte ASCII string, the name of the lump, padded with zeros.
 *        For example, the "DEMO1" entry in hexadecimal would be
 *        (44 45 4D 4F 31 00 00 00)
 *
 * Note that there is no way to tell if an opened WAD archive is a
 *  IWAD or PWAD with this archiver.
 * I couldn't think of a way to provide that information, without being too
 *  hacky.
 * I don't think it's really that important though.
 *
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 * This file written by Travis Wells, based on the GRP archiver by
 *  Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_WAD

static int wadLoadEntries(PHYSFS_Io *io, const PHYSFS_uint32 count, void *arc)
{
    PHYSFS_uint32 i;
    for (i = 0; i < count; i++)
    {
        PHYSFS_uint32 pos;
        PHYSFS_uint32 size;
        char name[9];

        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &pos, 4), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &size, 4), 0);
        BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, name, 8), 0);

        name[8] = '\0'; /* name might not be null-terminated in file. */
        size = PHYSFS_swapULE32(size);
        pos = PHYSFS_swapULE32(pos);
        BAIL_IF_ERRPASS(!UNPK_addEntry(arc, name, 0, -1, -1, pos, size), 0);
    } /* for */

    return 1;
} /* wadLoadEntries */


static void *WAD_openArchive(PHYSFS_Io *io, const char *name,
                             int forWriting, int *claimed)
{
    PHYSFS_uint8 buf[4];
    PHYSFS_uint32 count;
    PHYSFS_uint32 directoryOffset;
    void *unpkarc;

    assert(io != NULL);  /* shouldn't ever happen. */

    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);
    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, buf, sizeof (buf)), NULL);
    if ((memcmp(buf, "IWAD", 4) != 0) && (memcmp(buf, "PWAD", 4) != 0))
        BAIL(PHYSFS_ERR_UNSUPPORTED, NULL);

    *claimed = 1;

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &count, sizeof (count)), NULL);
    count = PHYSFS_swapULE32(count);

    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &directoryOffset, 4), 0);
    directoryOffset = PHYSFS_swapULE32(directoryOffset);

    BAIL_IF_ERRPASS(!io->seek(io, directoryOffset), 0);

    unpkarc = UNPK_openArchive(io);
    BAIL_IF_ERRPASS(!unpkarc, NULL);

    if (!wadLoadEntries(io, count, unpkarc))
    {
        UNPK_abandonArchive(unpkarc);
        return NULL;
    } /* if */

    return unpkarc;
} /* WAD_openArchive */


const PHYSFS_Archiver __PHYSFS_Archiver_WAD =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "WAD",
        "DOOM engine format",
        "Travis Wells <traviswells@mchsi.com>",
        "http://www.3dmm2.com/doom/",
        0,  /* supportsSymlinks */
    },
    WAD_openArchive,
    UNPK_enumerate,
    UNPK_openRead,
    UNPK_openWrite,
    UNPK_openAppend,
    UNPK_remove,
    UNPK_mkdir,
    UNPK_stat,
    UNPK_closeArchive
};

#endif  /* defined PHYSFS_SUPPORTS_WAD */

/* end of physfs_archiver_wad.c ... */

/*
 * ZIP support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon, with some peeking at "unzip.c"
 *   by Gilles Vollant.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"
*/
#if PHYSFS_SUPPORTS_ZIP

#include <errno.h>
#include <time.h>

/*#include "physfs_miniz.h"*/
/* tinfl.c v1.11 - public domain inflate with zlib header parsing/adler32 checking (inflate-only subset of miniz.c)
   See "unlicense" statement at the end of this file.
   Rich Geldreich <richgel99@gmail.com>, last updated May 20, 2011
   Implements RFC 1950: https://www.ietf.org/rfc/rfc1950.txt and RFC 1951: https://www.ietf.org/rfc/rfc1951.txt

   The entire decompressor coroutine is implemented in tinfl_decompress(). The other functions are optional high-level helpers.
*/
#ifndef TINFL_HEADER_INCLUDED
#define TINFL_HEADER_INCLUDED

typedef PHYSFS_uint8 mz_uint8;
typedef PHYSFS_sint16 mz_int16;
typedef PHYSFS_uint16 mz_uint16;
typedef PHYSFS_uint32 mz_uint32;
typedef unsigned int mz_uint;
typedef PHYSFS_uint64 mz_uint64;

/* For more compatibility with zlib, miniz.c uses unsigned long for some parameters/struct members. */
typedef unsigned long mz_ulong;

/* Heap allocation callbacks. */
typedef void *(*mz_alloc_func)(void *opaque, unsigned int items, unsigned int size);
typedef void (*mz_free_func)(void *opaque, void *address);

#if defined(_M_IX86) || defined(_M_X64)
/* Set MINIZ_USE_UNALIGNED_LOADS_AND_STORES to 1 if integer loads and stores to unaligned addresses are acceptable on the target platform (slightly faster). */
#define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 1
/* Set MINIZ_LITTLE_ENDIAN to 1 if the processor is little endian. */
#define MINIZ_LITTLE_ENDIAN 1
#endif

#if defined(_WIN64) || defined(__MINGW64__) || defined(_LP64) || defined(__LP64__)
/* Set MINIZ_HAS_64BIT_REGISTERS to 1 if the processor has 64-bit general purpose registers (enables 64-bit bitbuffer in inflator) */
#define MINIZ_HAS_64BIT_REGISTERS 1
#endif

/* Works around MSVC's spammy "warning C4127: conditional expression is constant" message. */
#ifdef _MSC_VER
#define MZ_MACRO_END while (0, 0)
#else
#define MZ_MACRO_END while (0)
#endif

/* Decompression flags. */
enum
{
  TINFL_FLAG_PARSE_ZLIB_HEADER = 1,
  TINFL_FLAG_HAS_MORE_INPUT = 2,
  TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4,
  TINFL_FLAG_COMPUTE_ADLER32 = 8
};

struct tinfl_decompressor_tag; typedef struct tinfl_decompressor_tag tinfl_decompressor;

/* Max size of LZ dictionary. */
#define TINFL_LZ_DICT_SIZE 32768

/* Return status. */
typedef enum
{
  TINFL_STATUS_BAD_PARAM = -3,
  TINFL_STATUS_ADLER32_MISMATCH = -2,
  TINFL_STATUS_FAILED = -1,
  TINFL_STATUS_DONE = 0,
  TINFL_STATUS_NEEDS_MORE_INPUT = 1,
  TINFL_STATUS_HAS_MORE_OUTPUT = 2
} tinfl_status;

/* Initializes the decompressor to its initial state. */
#define tinfl_init(r) do { (r)->m_state = 0; } MZ_MACRO_END
#define tinfl_get_adler32(r) (r)->m_check_adler32

/* Main low-level decompressor coroutine function. This is the only function actually needed for decompression. All the other functions are just high-level helpers for improved usability. */
/* This is a universal API, i.e. it can be used as a building block to build any desired higher level decompression API. In the limit case, it can be called once per every byte input or output. */
static tinfl_status tinfl_decompress(tinfl_decompressor *r, const mz_uint8 *pIn_buf_next, size_t *pIn_buf_size, mz_uint8 *pOut_buf_start, mz_uint8 *pOut_buf_next, size_t *pOut_buf_size, const mz_uint32 decomp_flags);

/* Internal/private bits follow. */
enum
{
  TINFL_MAX_HUFF_TABLES = 3, TINFL_MAX_HUFF_SYMBOLS_0 = 288, TINFL_MAX_HUFF_SYMBOLS_1 = 32, TINFL_MAX_HUFF_SYMBOLS_2 = 19,
  TINFL_FAST_LOOKUP_BITS = 10, TINFL_FAST_LOOKUP_SIZE = 1 << TINFL_FAST_LOOKUP_BITS
};

typedef struct
{
  mz_uint8 m_code_size[TINFL_MAX_HUFF_SYMBOLS_0];
  mz_int16 m_look_up[TINFL_FAST_LOOKUP_SIZE], m_tree[TINFL_MAX_HUFF_SYMBOLS_0 * 2];
} tinfl_huff_table;

#if MINIZ_HAS_64BIT_REGISTERS
  #define TINFL_USE_64BIT_BITBUF 1
#endif

#if TINFL_USE_64BIT_BITBUF
  typedef mz_uint64 tinfl_bit_buf_t;
  #define TINFL_BITBUF_SIZE (64)
#else
  typedef mz_uint32 tinfl_bit_buf_t;
  #define TINFL_BITBUF_SIZE (32)
#endif

struct tinfl_decompressor_tag
{
  mz_uint32 m_state, m_num_bits, m_zhdr0, m_zhdr1, m_z_adler32, m_final, m_type, m_check_adler32, m_dist, m_counter, m_num_extra, m_table_sizes[TINFL_MAX_HUFF_TABLES];
  tinfl_bit_buf_t m_bit_buf;
  size_t m_dist_from_out_buf_start;
  tinfl_huff_table m_tables[TINFL_MAX_HUFF_TABLES];
  mz_uint8 m_raw_header[4], m_len_codes[TINFL_MAX_HUFF_SYMBOLS_0 + TINFL_MAX_HUFF_SYMBOLS_1 + 137];
};

#endif /* #ifdef TINFL_HEADER_INCLUDED */

/* ------------------- End of Header: Implementation follows. (If you only want the header, define MINIZ_HEADER_FILE_ONLY.) */

#ifndef TINFL_HEADER_FILE_ONLY

#define MZ_MAX(a,b) (((a)>(b))?(a):(b))
#define MZ_MIN(a,b) (((a)<(b))?(a):(b))
#define MZ_CLEAR_OBJ(obj) memset(&(obj), 0, sizeof(obj))

#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN
  #define MZ_READ_LE16(p) *((const mz_uint16 *)(p))
  #define MZ_READ_LE32(p) *((const mz_uint32 *)(p))
#else
  #define MZ_READ_LE16(p) ((mz_uint32)(((const mz_uint8 *)(p))[0]) | ((mz_uint32)(((const mz_uint8 *)(p))[1]) << 8U))
  #define MZ_READ_LE32(p) ((mz_uint32)(((const mz_uint8 *)(p))[0]) | ((mz_uint32)(((const mz_uint8 *)(p))[1]) << 8U) | ((mz_uint32)(((const mz_uint8 *)(p))[2]) << 16U) | ((mz_uint32)(((const mz_uint8 *)(p))[3]) << 24U))
#endif

#define TINFL_MEMCPY(d, s, l) memcpy(d, s, l)
#define TINFL_MEMSET(p, c, l) memset(p, c, l)

#define TINFL_CR_BEGIN switch(r->m_state) { case 0:
#define TINFL_CR_RETURN(state_index, result) do { status = result; r->m_state = state_index; goto common_exit; case state_index:; } MZ_MACRO_END
#define TINFL_CR_RETURN_FOREVER(state_index, result) do { for ( ; ; ) { TINFL_CR_RETURN(state_index, result); } } MZ_MACRO_END
#define TINFL_CR_FINISH }

/* TODO: If the caller has indicated that there's no more input, and we attempt to read beyond the input buf, then something is wrong with the input because the inflator never */
/* reads ahead more than it needs to. Currently TINFL_GET_BYTE() pads the end of the stream with 0's in this scenario. */
#define TINFL_GET_BYTE(state_index, c) do { \
  if (pIn_buf_cur >= pIn_buf_end) { \
    for ( ; ; ) { \
      if (decomp_flags & TINFL_FLAG_HAS_MORE_INPUT) { \
        TINFL_CR_RETURN(state_index, TINFL_STATUS_NEEDS_MORE_INPUT); \
        if (pIn_buf_cur < pIn_buf_end) { \
          c = *pIn_buf_cur++; \
          break; \
        } \
      } else { \
        c = 0; \
        break; \
      } \
    } \
  } else c = *pIn_buf_cur++; } MZ_MACRO_END

#define TINFL_NEED_BITS(state_index, n) do { mz_uint c; TINFL_GET_BYTE(state_index, c); bit_buf |= (((tinfl_bit_buf_t)c) << num_bits); num_bits += 8; } while (num_bits < (mz_uint)(n))
#define TINFL_SKIP_BITS(state_index, n) do { if (num_bits < (mz_uint)(n)) { TINFL_NEED_BITS(state_index, n); } bit_buf >>= (n); num_bits -= (n); } MZ_MACRO_END
#define TINFL_GET_BITS(state_index, b, n) do { if (num_bits < (mz_uint)(n)) { TINFL_NEED_BITS(state_index, n); } b = bit_buf & ((1 << (n)) - 1); bit_buf >>= (n); num_bits -= (n); } MZ_MACRO_END

/* TINFL_HUFF_BITBUF_FILL() is only used rarely, when the number of bytes remaining in the input buffer falls below 2. */
/* It reads just enough bytes from the input stream that are needed to decode the next Huffman code (and absolutely no more). It works by trying to fully decode a */
/* Huffman code by using whatever bits are currently present in the bit buffer. If this fails, it reads another byte, and tries again until it succeeds or until the */
/* bit buffer contains >=15 bits (deflate's max. Huffman code size). */
#define TINFL_HUFF_BITBUF_FILL(state_index, pHuff) \
  do { \
    temp = (pHuff)->m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]; \
    if (temp >= 0) { \
      code_len = temp >> 9; \
      if ((code_len) && (num_bits >= code_len)) \
      break; \
    } else if (num_bits > TINFL_FAST_LOOKUP_BITS) { \
       code_len = TINFL_FAST_LOOKUP_BITS; \
       do { \
          temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; \
       } while ((temp < 0) && (num_bits >= (code_len + 1))); if (temp >= 0) break; \
    } TINFL_GET_BYTE(state_index, c); bit_buf |= (((tinfl_bit_buf_t)c) << num_bits); num_bits += 8; \
  } while (num_bits < 15);

/* TINFL_HUFF_DECODE() decodes the next Huffman coded symbol. It's more complex than you would initially expect because the zlib API expects the decompressor to never read */
/* beyond the final byte of the deflate stream. (In other words, when this macro wants to read another byte from the input, it REALLY needs another byte in order to fully */
/* decode the next Huffman code.) Handling this properly is particularly important on raw deflate (non-zlib) streams, which aren't followed by a byte aligned adler-32. */
/* The slow path is only executed at the very end of the input buffer. */
#define TINFL_HUFF_DECODE(state_index, sym, pHuff) do { \
  int temp; mz_uint code_len, c; \
  if (num_bits < 15) { \
    if ((pIn_buf_end - pIn_buf_cur) < 2) { \
       TINFL_HUFF_BITBUF_FILL(state_index, pHuff); \
    } else { \
       bit_buf |= (((tinfl_bit_buf_t)pIn_buf_cur[0]) << num_bits) | (((tinfl_bit_buf_t)pIn_buf_cur[1]) << (num_bits + 8)); pIn_buf_cur += 2; num_bits += 16; \
    } \
  } \
  if ((temp = (pHuff)->m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0) \
    code_len = temp >> 9, temp &= 511; \
  else { \
    code_len = TINFL_FAST_LOOKUP_BITS; do { temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; } while (temp < 0); \
  } sym = temp; bit_buf >>= code_len; num_bits -= code_len; } MZ_MACRO_END

static tinfl_status tinfl_decompress(tinfl_decompressor *r, const mz_uint8 *pIn_buf_next, size_t *pIn_buf_size, mz_uint8 *pOut_buf_start, mz_uint8 *pOut_buf_next, size_t *pOut_buf_size, const mz_uint32 decomp_flags)
{
  static const int s_length_base[31] = { 3,4,5,6,7,8,9,10,11,13, 15,17,19,23,27,31,35,43,51,59, 67,83,99,115,131,163,195,227,258,0,0 };
  static const int s_length_extra[31]= { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0 };
  static const int s_dist_base[32] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193, 257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0};
  static const int s_dist_extra[32] = { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
  static const mz_uint8 s_length_dezigzag[19] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
  static const int s_min_table_sizes[3] = { 257, 1, 4 };

  tinfl_status status = TINFL_STATUS_FAILED; mz_uint32 num_bits, dist, counter, num_extra; tinfl_bit_buf_t bit_buf;
  const mz_uint8 *pIn_buf_cur = pIn_buf_next, *const pIn_buf_end = pIn_buf_next + *pIn_buf_size;
  mz_uint8 *pOut_buf_cur = pOut_buf_next, *const pOut_buf_end = pOut_buf_next + *pOut_buf_size;
  size_t out_buf_size_mask = (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF) ? (size_t)-1 : ((pOut_buf_next - pOut_buf_start) + *pOut_buf_size) - 1, dist_from_out_buf_start;

  /* Ensure the output buffer's size is a power of 2, unless the output buffer is large enough to hold the entire output file (in which case it doesn't matter). */
  if (((out_buf_size_mask + 1) & out_buf_size_mask) || (pOut_buf_next < pOut_buf_start)) { *pIn_buf_size = *pOut_buf_size = 0; return TINFL_STATUS_BAD_PARAM; }

  num_bits = r->m_num_bits; bit_buf = r->m_bit_buf; dist = r->m_dist; counter = r->m_counter; num_extra = r->m_num_extra; dist_from_out_buf_start = r->m_dist_from_out_buf_start;
  TINFL_CR_BEGIN

  bit_buf = num_bits = dist = counter = num_extra = r->m_zhdr0 = r->m_zhdr1 = 0; r->m_z_adler32 = r->m_check_adler32 = 1;
  if (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER)
  {
    TINFL_GET_BYTE(1, r->m_zhdr0); TINFL_GET_BYTE(2, r->m_zhdr1);
    counter = (((r->m_zhdr0 * 256 + r->m_zhdr1) % 31 != 0) || (r->m_zhdr1 & 32) || ((r->m_zhdr0 & 15) != 8));
    if (!(decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)) counter |= (((1U << (8U + (r->m_zhdr0 >> 4))) > 32768U) || ((out_buf_size_mask + 1) < (size_t)(1U << (8U + (r->m_zhdr0 >> 4)))));
    if (counter) { TINFL_CR_RETURN_FOREVER(36, TINFL_STATUS_FAILED); }
  }

  do
  {
    TINFL_GET_BITS(3, r->m_final, 3); r->m_type = r->m_final >> 1;
    if (r->m_type == 0)
    {
      TINFL_SKIP_BITS(5, num_bits & 7);
      for (counter = 0; counter < 4; ++counter) { if (num_bits) TINFL_GET_BITS(6, r->m_raw_header[counter], 8); else TINFL_GET_BYTE(7, r->m_raw_header[counter]); }
      if ((counter = (r->m_raw_header[0] | (r->m_raw_header[1] << 8))) != (mz_uint)(0xFFFF ^ (r->m_raw_header[2] | (r->m_raw_header[3] << 8)))) { TINFL_CR_RETURN_FOREVER(39, TINFL_STATUS_FAILED); }
      while ((counter) && (num_bits))
      {
        TINFL_GET_BITS(51, dist, 8);
        while (pOut_buf_cur >= pOut_buf_end) { TINFL_CR_RETURN(52, TINFL_STATUS_HAS_MORE_OUTPUT); }
        *pOut_buf_cur++ = (mz_uint8)dist;
        counter--;
      }
      while (counter)
      {
        size_t n; while (pOut_buf_cur >= pOut_buf_end) { TINFL_CR_RETURN(9, TINFL_STATUS_HAS_MORE_OUTPUT); }
        while (pIn_buf_cur >= pIn_buf_end)
        {
          if (decomp_flags & TINFL_FLAG_HAS_MORE_INPUT)
          {
            TINFL_CR_RETURN(38, TINFL_STATUS_NEEDS_MORE_INPUT);
          }
          else
          {
            TINFL_CR_RETURN_FOREVER(40, TINFL_STATUS_FAILED);
          }
        }
        n = MZ_MIN(MZ_MIN((size_t)(pOut_buf_end - pOut_buf_cur), (size_t)(pIn_buf_end - pIn_buf_cur)), counter);
        TINFL_MEMCPY(pOut_buf_cur, pIn_buf_cur, n); pIn_buf_cur += n; pOut_buf_cur += n; counter -= (mz_uint)n;
      }
    }
    else if (r->m_type == 3)
    {
      TINFL_CR_RETURN_FOREVER(10, TINFL_STATUS_FAILED);
    }
    else
    {
      if (r->m_type == 1)
      {
        mz_uint8 *p = r->m_tables[0].m_code_size; mz_uint i;
        r->m_table_sizes[0] = 288; r->m_table_sizes[1] = 32; TINFL_MEMSET(r->m_tables[1].m_code_size, 5, 32);
        for ( i = 0; i <= 143; ++i) *p++ = 8;
        for ( ; i <= 255; ++i) *p++ = 9;
        for ( ; i <= 279; ++i) *p++ = 7;
        for ( ; i <= 287; ++i) *p++ = 8;
      }
      else
      {
        for (counter = 0; counter < 3; counter++) { TINFL_GET_BITS(11, r->m_table_sizes[counter], "\05\05\04"[counter]); r->m_table_sizes[counter] += s_min_table_sizes[counter]; }
        MZ_CLEAR_OBJ(r->m_tables[2].m_code_size); for (counter = 0; counter < r->m_table_sizes[2]; counter++) { mz_uint s; TINFL_GET_BITS(14, s, 3); r->m_tables[2].m_code_size[s_length_dezigzag[counter]] = (mz_uint8)s; }
        r->m_table_sizes[2] = 19;
      }
      for ( ; (int)r->m_type >= 0; r->m_type--)
      {
        int tree_next, tree_cur; tinfl_huff_table *pTable;
        mz_uint i, j, used_syms, total, sym_index, next_code[17], total_syms[16]; pTable = &r->m_tables[r->m_type]; MZ_CLEAR_OBJ(total_syms); MZ_CLEAR_OBJ(pTable->m_look_up); MZ_CLEAR_OBJ(pTable->m_tree);
        for (i = 0; i < r->m_table_sizes[r->m_type]; ++i) total_syms[pTable->m_code_size[i]]++;
        used_syms = 0, total = 0; next_code[0] = next_code[1] = 0;
        for (i = 1; i <= 15; ++i) { used_syms += total_syms[i]; next_code[i + 1] = (total = ((total + total_syms[i]) << 1)); }
        if ((65536 != total) && (used_syms > 1))
        {
          TINFL_CR_RETURN_FOREVER(35, TINFL_STATUS_FAILED);
        }
        for (tree_next = -1, sym_index = 0; sym_index < r->m_table_sizes[r->m_type]; ++sym_index)
        {
          mz_uint rev_code = 0, l, cur_code, code_size = pTable->m_code_size[sym_index]; if (!code_size) continue;
          cur_code = next_code[code_size]++; for (l = code_size; l > 0; l--, cur_code >>= 1) rev_code = (rev_code << 1) | (cur_code & 1);
          if (code_size <= TINFL_FAST_LOOKUP_BITS) { mz_int16 k = (mz_int16)((code_size << 9) | sym_index); while (rev_code < TINFL_FAST_LOOKUP_SIZE) { pTable->m_look_up[rev_code] = k; rev_code += (1 << code_size); } continue; }
          if (0 == (tree_cur = pTable->m_look_up[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)])) { pTable->m_look_up[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)] = (mz_int16)tree_next; tree_cur = tree_next; tree_next -= 2; }
          rev_code >>= (TINFL_FAST_LOOKUP_BITS - 1);
          for (j = code_size; j > (TINFL_FAST_LOOKUP_BITS + 1); j--)
          {
            tree_cur -= ((rev_code >>= 1) & 1);
            if (!pTable->m_tree[-tree_cur - 1]) { pTable->m_tree[-tree_cur - 1] = (mz_int16)tree_next; tree_cur = tree_next; tree_next -= 2; } else tree_cur = pTable->m_tree[-tree_cur - 1];
          }
          tree_cur -= ((rev_code >>= 1) & 1); pTable->m_tree[-tree_cur - 1] = (mz_int16)sym_index;
        }
        if (r->m_type == 2)
        {
          for (counter = 0; counter < (r->m_table_sizes[0] + r->m_table_sizes[1]); )
          {
            mz_uint s; TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]); if (dist < 16) { r->m_len_codes[counter++] = (mz_uint8)dist; continue; }
            if ((dist == 16) && (!counter))
            {
              TINFL_CR_RETURN_FOREVER(17, TINFL_STATUS_FAILED);
            }
            num_extra = "\02\03\07"[dist - 16]; TINFL_GET_BITS(18, s, num_extra); s += "\03\03\013"[dist - 16];
            TINFL_MEMSET(r->m_len_codes + counter, (dist == 16) ? r->m_len_codes[counter - 1] : 0, s); counter += s;
          }
          if ((r->m_table_sizes[0] + r->m_table_sizes[1]) != counter)
          {
            TINFL_CR_RETURN_FOREVER(21, TINFL_STATUS_FAILED);
          }
          TINFL_MEMCPY(r->m_tables[0].m_code_size, r->m_len_codes, r->m_table_sizes[0]); TINFL_MEMCPY(r->m_tables[1].m_code_size, r->m_len_codes + r->m_table_sizes[0], r->m_table_sizes[1]);
        }
      }
      for ( ; ; )
      {
        mz_uint8 *pSrc;
        for ( ; ; )
        {
          if (((pIn_buf_end - pIn_buf_cur) < 4) || ((pOut_buf_end - pOut_buf_cur) < 2))
          {
            TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
            if (counter >= 256)
              break;
            while (pOut_buf_cur >= pOut_buf_end) { TINFL_CR_RETURN(24, TINFL_STATUS_HAS_MORE_OUTPUT); }
            *pOut_buf_cur++ = (mz_uint8)counter;
          }
          else
          {
            int sym2; mz_uint code_len;
#if TINFL_USE_64BIT_BITBUF
            if (num_bits < 30) { bit_buf |= (((tinfl_bit_buf_t)MZ_READ_LE32(pIn_buf_cur)) << num_bits); pIn_buf_cur += 4; num_bits += 32; }
#else
            if (num_bits < 15) { bit_buf |= (((tinfl_bit_buf_t)MZ_READ_LE16(pIn_buf_cur)) << num_bits); pIn_buf_cur += 2; num_bits += 16; }
#endif
            if ((sym2 = r->m_tables[0].m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)
              code_len = sym2 >> 9;
            else
            {
              code_len = TINFL_FAST_LOOKUP_BITS; do { sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)]; } while (sym2 < 0);
            }
            counter = sym2; bit_buf >>= code_len; num_bits -= code_len;
            if (counter & 256)
              break;

#if !TINFL_USE_64BIT_BITBUF
            if (num_bits < 15) { bit_buf |= (((tinfl_bit_buf_t)MZ_READ_LE16(pIn_buf_cur)) << num_bits); pIn_buf_cur += 2; num_bits += 16; }
#endif
            if ((sym2 = r->m_tables[0].m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)
              code_len = sym2 >> 9;
            else
            {
              code_len = TINFL_FAST_LOOKUP_BITS; do { sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)]; } while (sym2 < 0);
            }
            bit_buf >>= code_len; num_bits -= code_len;

            pOut_buf_cur[0] = (mz_uint8)counter;
            if (sym2 & 256)
            {
              pOut_buf_cur++;
              counter = sym2;
              break;
            }
            pOut_buf_cur[1] = (mz_uint8)sym2;
            pOut_buf_cur += 2;
          }
        }
        if ((counter &= 511) == 256) break;

        num_extra = s_length_extra[counter - 257]; counter = s_length_base[counter - 257];
        if (num_extra) { mz_uint extra_bits; TINFL_GET_BITS(25, extra_bits, num_extra); counter += extra_bits; }

        TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
        num_extra = s_dist_extra[dist]; dist = s_dist_base[dist];
        if (num_extra) { mz_uint extra_bits; TINFL_GET_BITS(27, extra_bits, num_extra); dist += extra_bits; }

        dist_from_out_buf_start = pOut_buf_cur - pOut_buf_start;
        if ((dist > dist_from_out_buf_start) && (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF))
        {
          TINFL_CR_RETURN_FOREVER(37, TINFL_STATUS_FAILED);
        }

        pSrc = pOut_buf_start + ((dist_from_out_buf_start - dist) & out_buf_size_mask);

        if ((MZ_MAX(pOut_buf_cur, pSrc) + counter) > pOut_buf_end)
        {
          while (counter--)
          {
            while (pOut_buf_cur >= pOut_buf_end) { TINFL_CR_RETURN(53, TINFL_STATUS_HAS_MORE_OUTPUT); }
            *pOut_buf_cur++ = pOut_buf_start[(dist_from_out_buf_start++ - dist) & out_buf_size_mask];
          }
          continue;
        }
#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES
        else if ((counter >= 9) && (counter <= dist))
        {
          const mz_uint8 *pSrc_end = pSrc + (counter & ~7);
          do
          {
            ((mz_uint32 *)pOut_buf_cur)[0] = ((const mz_uint32 *)pSrc)[0];
            ((mz_uint32 *)pOut_buf_cur)[1] = ((const mz_uint32 *)pSrc)[1];
            pOut_buf_cur += 8;
          } while ((pSrc += 8) < pSrc_end);
          if ((counter &= 7) < 3)
          {
            if (counter)
            {
              pOut_buf_cur[0] = pSrc[0];
              if (counter > 1)
                pOut_buf_cur[1] = pSrc[1];
              pOut_buf_cur += counter;
            }
            continue;
          }
        }
#endif
        do
        {
          pOut_buf_cur[0] = pSrc[0];
          pOut_buf_cur[1] = pSrc[1];
          pOut_buf_cur[2] = pSrc[2];
          pOut_buf_cur += 3; pSrc += 3;
        } while ((int)(counter -= 3) > 2);
        if ((int)counter > 0)
        {
          pOut_buf_cur[0] = pSrc[0];
          if ((int)counter > 1)
            pOut_buf_cur[1] = pSrc[1];
          pOut_buf_cur += counter;
        }
      }
    }
  } while (!(r->m_final & 1));
  if (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER)
  {
    TINFL_SKIP_BITS(32, num_bits & 7); for (counter = 0; counter < 4; ++counter) { mz_uint s; if (num_bits) TINFL_GET_BITS(41, s, 8); else TINFL_GET_BYTE(42, s); r->m_z_adler32 = (r->m_z_adler32 << 8) | s; }
  }
  TINFL_CR_RETURN_FOREVER(34, TINFL_STATUS_DONE);
  TINFL_CR_FINISH

common_exit:
  r->m_num_bits = num_bits; r->m_bit_buf = bit_buf; r->m_dist = dist; r->m_counter = counter; r->m_num_extra = num_extra; r->m_dist_from_out_buf_start = dist_from_out_buf_start;
  *pIn_buf_size = pIn_buf_cur - pIn_buf_next; *pOut_buf_size = pOut_buf_cur - pOut_buf_next;
  if ((decomp_flags & (TINFL_FLAG_PARSE_ZLIB_HEADER | TINFL_FLAG_COMPUTE_ADLER32)) && (status >= 0))
  {
    const mz_uint8 *ptr = pOut_buf_next; size_t buf_len = *pOut_buf_size;
    mz_uint32 i, s1 = r->m_check_adler32 & 0xffff, s2 = r->m_check_adler32 >> 16; size_t block_len = buf_len % 5552;
    while (buf_len)
    {
      for (i = 0; i + 7 < block_len; i += 8, ptr += 8)
      {
        s1 += ptr[0], s2 += s1; s1 += ptr[1], s2 += s1; s1 += ptr[2], s2 += s1; s1 += ptr[3], s2 += s1;
        s1 += ptr[4], s2 += s1; s1 += ptr[5], s2 += s1; s1 += ptr[6], s2 += s1; s1 += ptr[7], s2 += s1;
      }
      for ( ; i < block_len; ++i) s1 += *ptr++, s2 += s1;
      s1 %= 65521U, s2 %= 65521U; buf_len -= block_len; block_len = 5552;
    }
    r->m_check_adler32 = (s2 << 16) + s1; if ((status == TINFL_STATUS_DONE) && (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER) && (r->m_check_adler32 != r->m_z_adler32)) status = TINFL_STATUS_ADLER32_MISMATCH;
  }
  return status;
}

/* Flush values. For typical usage you only need MZ_NO_FLUSH and MZ_FINISH. The other stuff is for advanced use. */
enum { MZ_NO_FLUSH = 0, MZ_PARTIAL_FLUSH = 1, MZ_SYNC_FLUSH = 2, MZ_FULL_FLUSH = 3, MZ_FINISH = 4, MZ_BLOCK = 5 };

/* Return status codes. MZ_PARAM_ERROR is non-standard. */
enum { MZ_OK = 0, MZ_STREAM_END = 1, MZ_NEED_DICT = 2, MZ_ERRNO = -1, MZ_STREAM_ERROR = -2, MZ_DATA_ERROR = -3, MZ_MEM_ERROR = -4, MZ_BUF_ERROR = -5, MZ_VERSION_ERROR = -6, MZ_PARAM_ERROR = -10000 };

/* Compression levels. */
enum { MZ_NO_COMPRESSION = 0, MZ_BEST_SPEED = 1, MZ_BEST_COMPRESSION = 9, MZ_DEFAULT_COMPRESSION = -1 };

/* Window bits */
#define MZ_DEFAULT_WINDOW_BITS 15

struct mz_internal_state;

/* Compression/decompression stream struct. */
typedef struct mz_stream_s
{
  const unsigned char *next_in;     /* pointer to next byte to read */
  unsigned int avail_in;            /* number of bytes available at next_in */
  mz_ulong total_in;                /* total number of bytes consumed so far */

  unsigned char *next_out;          /* pointer to next byte to write */
  unsigned int avail_out;           /* number of bytes that can be written to next_out */
  mz_ulong total_out;               /* total number of bytes produced so far */

  char *msg;                        /* error msg (unused) */
  struct mz_internal_state *state;  /* internal state, allocated by zalloc/zfree */

  mz_alloc_func zalloc;             /* optional heap allocation function (defaults to malloc) */
  mz_free_func zfree;               /* optional heap free function (defaults to free) */
  void *opaque;                     /* heap alloc function user pointer */

  int data_type;                    /* data_type (unused) */
  mz_ulong adler;                   /* adler32 of the source or uncompressed data */
  mz_ulong reserved;                /* not used */
} mz_stream;

typedef mz_stream *mz_streamp;


typedef struct
{
  tinfl_decompressor m_decomp;
  mz_uint m_dict_ofs, m_dict_avail, m_first_call, m_has_flushed; int m_window_bits;
  mz_uint8 m_dict[TINFL_LZ_DICT_SIZE];
  tinfl_status m_last_status;
} inflate_state;

static int mz_inflateInit2(mz_streamp pStream, int window_bits)
{
  inflate_state *pDecomp;
  if (!pStream) return MZ_STREAM_ERROR;
  if ((window_bits != MZ_DEFAULT_WINDOW_BITS) && (-window_bits != MZ_DEFAULT_WINDOW_BITS)) return MZ_PARAM_ERROR;

  pStream->data_type = 0;
  pStream->adler = 0;
  pStream->msg = NULL;
  pStream->total_in = 0;
  pStream->total_out = 0;
  pStream->reserved = 0;
  /* if (!pStream->zalloc) pStream->zalloc = def_alloc_func; */
  /* if (!pStream->zfree) pStream->zfree = def_free_func; */

  pDecomp = (inflate_state*)pStream->zalloc(pStream->opaque, 1, sizeof(inflate_state));
  if (!pDecomp) return MZ_MEM_ERROR;

  pStream->state = (struct mz_internal_state *)pDecomp;

  tinfl_init(&pDecomp->m_decomp);
  pDecomp->m_dict_ofs = 0;
  pDecomp->m_dict_avail = 0;
  pDecomp->m_last_status = TINFL_STATUS_NEEDS_MORE_INPUT;
  pDecomp->m_first_call = 1;
  pDecomp->m_has_flushed = 0;
  pDecomp->m_window_bits = window_bits;

  return MZ_OK;
}

static int mz_inflate(mz_streamp pStream, int flush)
{
  inflate_state* pState;
  mz_uint n, first_call, decomp_flags = TINFL_FLAG_COMPUTE_ADLER32;
  size_t in_bytes, out_bytes, orig_avail_in;
  tinfl_status status;

  if ((!pStream) || (!pStream->state)) return MZ_STREAM_ERROR;
  if (flush == MZ_PARTIAL_FLUSH) flush = MZ_SYNC_FLUSH;
  if ((flush) && (flush != MZ_SYNC_FLUSH) && (flush != MZ_FINISH)) return MZ_STREAM_ERROR;

  pState = (inflate_state*)pStream->state;
  if (pState->m_window_bits > 0) decomp_flags |= TINFL_FLAG_PARSE_ZLIB_HEADER;
  orig_avail_in = pStream->avail_in;

  first_call = pState->m_first_call; pState->m_first_call = 0;
  if (pState->m_last_status < 0) return MZ_DATA_ERROR;

  if (pState->m_has_flushed && (flush != MZ_FINISH)) return MZ_STREAM_ERROR;
  pState->m_has_flushed |= (flush == MZ_FINISH);

  if ((flush == MZ_FINISH) && (first_call))
  {
    /* MZ_FINISH on the first call implies that the input and output buffers are large enough to hold the entire compressed/decompressed file. */
    decomp_flags |= TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF;
    in_bytes = pStream->avail_in; out_bytes = pStream->avail_out;
    status = tinfl_decompress(&pState->m_decomp, pStream->next_in, &in_bytes, pStream->next_out, pStream->next_out, &out_bytes, decomp_flags);
    pState->m_last_status = status;
    pStream->next_in += (mz_uint)in_bytes; pStream->avail_in -= (mz_uint)in_bytes; pStream->total_in += (mz_uint)in_bytes;
    pStream->adler = tinfl_get_adler32(&pState->m_decomp);
    pStream->next_out += (mz_uint)out_bytes; pStream->avail_out -= (mz_uint)out_bytes; pStream->total_out += (mz_uint)out_bytes;

    if (status < 0)
      return MZ_DATA_ERROR;
    else if (status != TINFL_STATUS_DONE)
    {
      pState->m_last_status = TINFL_STATUS_FAILED;
      return MZ_BUF_ERROR;
    }
    return MZ_STREAM_END;
  }
  /* flush != MZ_FINISH then we must assume there's more input. */
  if (flush != MZ_FINISH) decomp_flags |= TINFL_FLAG_HAS_MORE_INPUT;

  if (pState->m_dict_avail)
  {
    n = MZ_MIN(pState->m_dict_avail, pStream->avail_out);
    memcpy(pStream->next_out, pState->m_dict + pState->m_dict_ofs, n);
    pStream->next_out += n; pStream->avail_out -= n; pStream->total_out += n;
    pState->m_dict_avail -= n; pState->m_dict_ofs = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);
    return ((pState->m_last_status == TINFL_STATUS_DONE) && (!pState->m_dict_avail)) ? MZ_STREAM_END : MZ_OK;
  }

  for ( ; ; )
  {
    in_bytes = pStream->avail_in;
    out_bytes = TINFL_LZ_DICT_SIZE - pState->m_dict_ofs;

    status = tinfl_decompress(&pState->m_decomp, pStream->next_in, &in_bytes, pState->m_dict, pState->m_dict + pState->m_dict_ofs, &out_bytes, decomp_flags);
    pState->m_last_status = status;

    pStream->next_in += (mz_uint)in_bytes; pStream->avail_in -= (mz_uint)in_bytes;
    pStream->total_in += (mz_uint)in_bytes; pStream->adler = tinfl_get_adler32(&pState->m_decomp);

    pState->m_dict_avail = (mz_uint)out_bytes;

    n = MZ_MIN(pState->m_dict_avail, pStream->avail_out);
    memcpy(pStream->next_out, pState->m_dict + pState->m_dict_ofs, n);
    pStream->next_out += n; pStream->avail_out -= n; pStream->total_out += n;
    pState->m_dict_avail -= n; pState->m_dict_ofs = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);

    if (status < 0)
       return MZ_DATA_ERROR; /* Stream is corrupted (there could be some uncompressed data left in the output dictionary - oh well). */
    else if ((status == TINFL_STATUS_NEEDS_MORE_INPUT) && (!orig_avail_in))
      return MZ_BUF_ERROR; /* Signal caller that we can't make forward progress without supplying more input or by setting flush to MZ_FINISH. */
    else if (flush == MZ_FINISH)
    {
       /* The output buffer MUST be large to hold the remaining uncompressed data when flush==MZ_FINISH. */
       if (status == TINFL_STATUS_DONE)
          return pState->m_dict_avail ? MZ_BUF_ERROR : MZ_STREAM_END;
       /* status here must be TINFL_STATUS_HAS_MORE_OUTPUT, which means there's at least 1 more byte on the way. If there's no more room left in the output buffer then something is wrong. */
       else if (!pStream->avail_out)
          return MZ_BUF_ERROR;
    }
    else if ((status == TINFL_STATUS_DONE) || (!pStream->avail_in) || (!pStream->avail_out) || (pState->m_dict_avail))
      break;
  }

  return ((status == TINFL_STATUS_DONE) && (!pState->m_dict_avail)) ? MZ_STREAM_END : MZ_OK;
}

static int mz_inflateEnd(mz_streamp pStream)
{
  if (!pStream)
    return MZ_STREAM_ERROR;
  if (pStream->state)
  {
    pStream->zfree(pStream->opaque, pStream->state);
    pStream->state = NULL;
  }
  return MZ_OK;
}

/* make this a drop-in replacement for zlib... */
  #define voidpf void*
  #define uInt unsigned int
  #define z_stream              mz_stream
  #define inflateInit2          mz_inflateInit2
  #define inflate               mz_inflate
  #define inflateEnd            mz_inflateEnd
  #define Z_SYNC_FLUSH          MZ_SYNC_FLUSH
  #define Z_FINISH              MZ_FINISH
  #define Z_OK                  MZ_OK
  #define Z_STREAM_END          MZ_STREAM_END
  #define Z_NEED_DICT           MZ_NEED_DICT
  #define Z_ERRNO               MZ_ERRNO
  #define Z_STREAM_ERROR        MZ_STREAM_ERROR
  #define Z_DATA_ERROR          MZ_DATA_ERROR
  #define Z_MEM_ERROR           MZ_MEM_ERROR
  #define Z_BUF_ERROR           MZ_BUF_ERROR
  #define Z_VERSION_ERROR       MZ_VERSION_ERROR
  #define MAX_WBITS             15

#endif /* #ifndef TINFL_HEADER_FILE_ONLY */

/*
  This is free and unencumbered software released into the public domain.

  Anyone is free to copy, modify, publish, use, compile, sell, or
  distribute this software, either in source code form or as a compiled
  binary, for any purpose, commercial or non-commercial, and by any
  means.

  In jurisdictions that recognize copyright laws, the author or authors
  of this software dedicate any and all copyright interest in the
  software to the public domain. We make this dedication for the benefit
  of the public at large and to the detriment of our heirs and
  successors. We intend this dedication to be an overt act of
  relinquishment in perpetuity of all present and future rights to this
  software under copyright law.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  For more information, please refer to <https://unlicense.org/>
*/

/* end of physfs_miniz.h ... */

/*
 * A buffer of ZIP_READBUFSIZE is allocated for each compressed file opened,
 *  and is freed when you close the file; compressed data is read into
 *  this buffer, and then is decompressed into the buffer passed to
 *  PHYSFS_read().
 *
 * Uncompressed entries in a zipfile do not allocate this buffer; they just
 *  read data directly into the buffer passed to PHYSFS_read().
 *
 * Depending on your speed and memory requirements, you should tweak this
 *  value.
 */
#define ZIP_READBUFSIZE   (16 * 1024)


/*
 * Entries are "unresolved" until they are first opened. At that time,
 *  local file headers parsed/validated, data offsets will be updated to look
 *  at the actual file data instead of the header, and symlinks will be
 *  followed and optimized. This means that we don't seek and read around the
 *  archive until forced to do so, and after the first time, we had to do
 *  less reading and parsing, which is very CD-ROM friendly.
 */
typedef enum
{
    ZIP_UNRESOLVED_FILE,
    ZIP_UNRESOLVED_SYMLINK,
    ZIP_RESOLVING,
    ZIP_RESOLVED,
    ZIP_DIRECTORY,
    ZIP_BROKEN_FILE,
    ZIP_BROKEN_SYMLINK
} ZipResolveType;

#if PHYSFS_HAVE_PRAGMA_VISIBILITY
#pragma GCC visibility push(hidden)
#endif

/*
 * One ZIPentry is kept for each file in an open ZIP archive.
 */
typedef struct _ZIPentry
{
    __PHYSFS_DirTreeEntry tree;         /* manages directory tree         */
    struct _ZIPentry *symlink;          /* NULL or file we symlink to     */
    ZipResolveType resolved;            /* Have we resolved file/symlink? */
    PHYSFS_uint64 offset;               /* offset of data in archive      */
    PHYSFS_uint16 version;              /* version made by                */
    PHYSFS_uint16 version_needed;       /* version needed to extract      */
    PHYSFS_uint16 general_bits;         /* general purpose bits           */
    PHYSFS_uint16 compression_method;   /* compression method             */
    PHYSFS_uint32 crc;                  /* crc-32                         */
    PHYSFS_uint64 compressed_size;      /* compressed size                */
    PHYSFS_uint64 uncompressed_size;    /* uncompressed size              */
    PHYSFS_sint64 last_mod_time;        /* last file mod time             */
    PHYSFS_uint32 dos_mod_time;         /* original MS-DOS style mod time */
} ZIPentry;

/*
 * One ZIPinfo is kept for each open ZIP archive.
 */
typedef struct
{
    __PHYSFS_DirTree tree;    /* manages directory tree.                */
    PHYSFS_Io *io;            /* the i/o interface for this archive.    */
    int zip64;                /* non-zero if this is a Zip64 archive.   */
    int has_crypto;           /* non-zero if any entry uses encryption. */
} ZIPinfo;

/*
 * One ZIPfileinfo is kept for each open file in a ZIP archive.
 */
typedef struct
{
    ZIPentry *entry;                      /* Info on file.              */
    PHYSFS_Io *io;                        /* physical file handle.      */
    PHYSFS_uint32 compressed_position;    /* offset in compressed data. */
    PHYSFS_uint32 uncompressed_position;  /* tell() position.           */
    PHYSFS_uint8 *buffer;                 /* decompression buffer.      */
    PHYSFS_uint32 crypto_keys[3];         /* for "traditional" crypto.  */
    PHYSFS_uint32 initial_crypto_keys[3]; /* for "traditional" crypto.  */
    z_stream stream;                      /* zlib stream state.         */
} ZIPfileinfo;

#if PHYSFS_HAVE_PRAGMA_VISIBILITY
#pragma GCC visibility pop
#endif

/* Magic numbers... */
#define ZIP_LOCAL_FILE_SIG                          0x04034b50
#define ZIP_CENTRAL_DIR_SIG                         0x02014b50
#define ZIP_END_OF_CENTRAL_DIR_SIG                  0x06054b50
#define ZIP64_END_OF_CENTRAL_DIR_SIG                0x06064b50
#define ZIP64_END_OF_CENTRAL_DIRECTORY_LOCATOR_SIG  0x07064b50
#define ZIP64_EXTENDED_INFO_EXTRA_FIELD_SIG         0x0001

/* compression methods... */
#define COMPMETH_NONE 0
/* ...and others... */


#define UNIX_FILETYPE_MASK    0170000
#define UNIX_FILETYPE_SYMLINK 0120000

#define ZIP_GENERAL_BITS_TRADITIONAL_CRYPTO   (1 << 0)
#define ZIP_GENERAL_BITS_IGNORE_LOCAL_HEADER  (1 << 3)

/* support for "traditional" PKWARE encryption. */
static int zip_entry_is_tradional_crypto(const ZIPentry *entry)
{
    return (entry->general_bits & ZIP_GENERAL_BITS_TRADITIONAL_CRYPTO) != 0;
} /* zip_entry_is_traditional_crypto */

static int zip_entry_ignore_local_header(const ZIPentry *entry)
{
    return (entry->general_bits & ZIP_GENERAL_BITS_IGNORE_LOCAL_HEADER) != 0;
} /* zip_entry_is_traditional_crypto */

static PHYSFS_uint32 zip_crypto_crc32(const PHYSFS_uint32 crc, const PHYSFS_uint8 val)
{
    int i;
    PHYSFS_uint32 xorval = (crc ^ ((PHYSFS_uint32) val)) & 0xFF;
    for (i = 0; i < 8; i++)
        xorval = ((xorval & 1) ? (0xEDB88320 ^ (xorval >> 1)) : (xorval >> 1));
    return xorval ^ (crc >> 8);
} /* zip_crc32 */

static void zip_update_crypto_keys(PHYSFS_uint32 *keys, const PHYSFS_uint8 val)
{
    keys[0] = zip_crypto_crc32(keys[0], val);
    keys[1] = keys[1] + (keys[0] & 0x000000FF);
    keys[1] = (keys[1] * 134775813) + 1;
    keys[2] = zip_crypto_crc32(keys[2], (PHYSFS_uint8) ((keys[1] >> 24) & 0xFF));
} /* zip_update_crypto_keys */

static PHYSFS_uint8 zip_decrypt_byte(const PHYSFS_uint32 *keys)
{
    const PHYSFS_uint16 tmp = keys[2] | 2;
    return (PHYSFS_uint8) ((tmp * (tmp ^ 1)) >> 8);
} /* zip_decrypt_byte */

static PHYSFS_sint64 zip_read_decrypt(ZIPfileinfo *finfo, void *buf, PHYSFS_uint64 len)
{
    PHYSFS_Io *io = finfo->io;
    const PHYSFS_sint64 br = io->read(io, buf, len);

    /* Decompression the new data if necessary. */
    if (zip_entry_is_tradional_crypto(finfo->entry) && (br > 0))
    {
        PHYSFS_uint32 *keys = finfo->crypto_keys;
        PHYSFS_uint8 *ptr = (PHYSFS_uint8 *) buf;
        PHYSFS_sint64 i;
        for (i = 0; i < br; i++, ptr++)
        {
            const PHYSFS_uint8 ch = *ptr ^ zip_decrypt_byte(keys);
            zip_update_crypto_keys(keys, ch);
            *ptr = ch;
        } /* for */
    } /* if  */

    return br;
} /* zip_read_decrypt */

static int zip_prep_crypto_keys(ZIPfileinfo *finfo, const PHYSFS_uint8 *crypto_header, const PHYSFS_uint8 *password)
{
    /* It doesn't appear to be documented in PKWare's APPNOTE.TXT, but you
       need to use a different byte in the header to verify the password
       if general purpose bit 3 is set. Discovered this from Info-Zip.
       That's what the (verifier) value is doing, below. */

    PHYSFS_uint32 *keys = finfo->crypto_keys;
    const ZIPentry *entry = finfo->entry;
    const int usedate = zip_entry_ignore_local_header(entry);
    const PHYSFS_uint8 verifier = (PHYSFS_uint8) ((usedate ? (entry->dos_mod_time >> 8) : (entry->crc >> 24)) & 0xFF);
    PHYSFS_uint8 finalbyte = 0;
    int i = 0;

    /* initialize vector with defaults, then password, then header. */
    keys[0] = 305419896;
    keys[1] = 591751049;
    keys[2] = 878082192;

    while (*password)
        zip_update_crypto_keys(keys, *(password++));

    for (i = 0; i < 12; i++)
    {
        const PHYSFS_uint8 c = crypto_header[i] ^ zip_decrypt_byte(keys);
        zip_update_crypto_keys(keys, c);
        finalbyte = c;
    } /* for */

    /* you have a 1/256 chance of passing this test incorrectly. :/ */
    if (finalbyte != verifier)
        BAIL(PHYSFS_ERR_BAD_PASSWORD, 0);

    /* save the initial vector for seeking purposes. Not secure!! */
    memcpy(finfo->initial_crypto_keys, finfo->crypto_keys, 12);
    return 1;
} /* zip_prep_crypto_keys */


/*
 * Bridge physfs allocation functions to zlib's format...
 */
static voidpf zlibPhysfsAlloc(voidpf opaque, uInt items, uInt size)
{
    return ((PHYSFS_Allocator *) opaque)->Malloc(items * size);
} /* zlibPhysfsAlloc */

/*
 * Bridge physfs allocation functions to zlib's format...
 */
static void zlibPhysfsFree(voidpf opaque, voidpf address)
{
    ((PHYSFS_Allocator *) opaque)->Free(address);
} /* zlibPhysfsFree */


/*
 * Construct a new z_stream to a sane state.
 */
static void initializeZStream(z_stream *pstr)
{
    memset(pstr, '\0', sizeof (z_stream));
    pstr->zalloc = zlibPhysfsAlloc;
    pstr->zfree = zlibPhysfsFree;
    pstr->opaque = &allocator;
} /* initializeZStream */


static PHYSFS_ErrorCode zlib_error_code(int rc)
{
    switch (rc)
    {
        case Z_OK: return PHYSFS_ERR_OK;  /* not an error. */
        case Z_STREAM_END: return PHYSFS_ERR_OK; /* not an error. */
        case Z_ERRNO: return PHYSFS_ERR_IO;
        case Z_MEM_ERROR: return PHYSFS_ERR_OUT_OF_MEMORY;
        default: return PHYSFS_ERR_CORRUPT;
    } /* switch */
} /* zlib_error_string */


/*
 * Wrap all zlib calls in this, so the physfs error state is set appropriately.
 */
static int zlib_err(const int rc)
{
    PHYSFS_setErrorCode(zlib_error_code(rc));
    return rc;
} /* zlib_err */


/*
 * Read an unsigned 16-bit int and swap to native byte order.
 */
static int readui16(PHYSFS_Io *io, PHYSFS_uint16 *val)
{
    PHYSFS_uint16 v;
    BAIL_IF_ERRPASS(!__PHYSFS_readAll(io, &v, sizeof (v)), 0);
    *val = PHYSFS_swapULE16(v);
    return 1;
} /* readui16 */


static PHYSFS_sint64 ZIP_read(PHYSFS_Io *_io, void *buf, PHYSFS_uint64 len)
{
    ZIPfileinfo *finfo = (ZIPfileinfo *) _io->opaque;
    ZIPentry *entry = finfo->entry;
    PHYSFS_sint64 retval = 0;
    PHYSFS_sint64 maxread = (PHYSFS_sint64) len;
    PHYSFS_sint64 avail = entry->uncompressed_size -
                          finfo->uncompressed_position;

    if (avail < maxread)
        maxread = avail;

    BAIL_IF_ERRPASS(maxread == 0, 0);    /* quick rejection. */

    if (entry->compression_method == COMPMETH_NONE)
        retval = zip_read_decrypt(finfo, buf, maxread);
    else
    {
        finfo->stream.next_out = (unsigned char*)buf;
        finfo->stream.avail_out = (uInt) maxread;

        while (retval < maxread)
        {
            const PHYSFS_uint32 before = (PHYSFS_uint32) finfo->stream.total_out;
            int rc;

            if (finfo->stream.avail_in == 0)
            {
                PHYSFS_sint64 br;

                br = entry->compressed_size - finfo->compressed_position;
                if (br > 0)
                {
                    if (br > ZIP_READBUFSIZE)
                        br = ZIP_READBUFSIZE;

                    br = zip_read_decrypt(finfo, finfo->buffer, (PHYSFS_uint64) br);
                    if (br <= 0)
                        break;

                    finfo->compressed_position += (PHYSFS_uint32) br;
                    finfo->stream.next_in = finfo->buffer;
                    finfo->stream.avail_in = (unsigned int) br;
                } /* if */
            } /* if */

            rc = zlib_err(inflate(&finfo->stream, Z_SYNC_FLUSH));
            retval += (finfo->stream.total_out - before);

            if (rc != Z_OK)
                break;
        } /* while */
    } /* else */

    if (retval > 0)
        finfo->uncompressed_position += (PHYSFS_uint32) retval;

    return retval;
} /* ZIP_read */


static PHYSFS_sint64 ZIP_write(PHYSFS_Io *io, const void *b, PHYSFS_uint64 len)
{
    BAIL(PHYSFS_ERR_READ_ONLY, -1);
} /* ZIP_write */


static PHYSFS_sint64 ZIP_tell(PHYSFS_Io *io)
{
    return ((ZIPfileinfo *) io->opaque)->uncompressed_position;
} /* ZIP_tell */


static int ZIP_seek(PHYSFS_Io *_io, PHYSFS_uint64 offset)
{
    ZIPfileinfo *finfo = (ZIPfileinfo *) _io->opaque;
    ZIPentry *entry = finfo->entry;
    PHYSFS_Io *io = finfo->io;
    const int encrypted = zip_entry_is_tradional_crypto(entry);

    BAIL_IF(offset > entry->uncompressed_size, PHYSFS_ERR_PAST_EOF, 0);

    if (!encrypted && (entry->compression_method == COMPMETH_NONE))
    {
        PHYSFS_sint64 newpos = offset + entry->offset;
        BAIL_IF_ERRPASS(!io->seek(io, newpos), 0);
        finfo->uncompressed_position = (PHYSFS_uint32) offset;
    } /* if */

    else
    {
        /*
         * If seeking backwards, we need to redecode the file
         *  from the start and throw away the compressed bits until we hit
         *  the offset we need. If seeking forward, we still need to
         *  decode, but we don't rewind first.
         */
        if (offset < finfo->uncompressed_position)
        {
            /* we do a copy so state is sane if inflateInit2() fails. */
            z_stream str;
            initializeZStream(&str);
            if (zlib_err(inflateInit2(&str, -MAX_WBITS)) != Z_OK)
                return 0;

            if (!io->seek(io, entry->offset + (encrypted ? 12 : 0)))
                return 0;

            inflateEnd(&finfo->stream);
            memcpy(&finfo->stream, &str, sizeof (z_stream));
            finfo->uncompressed_position = finfo->compressed_position = 0;

            if (encrypted)
                memcpy(finfo->crypto_keys, finfo->initial_crypto_keys, 12);
        } /* if */

        while (finfo->uncompressed_position != offset)
        {
            PHYSFS_uint8 buf[512];
            PHYSFS_uint32 maxread;

            maxread = (PHYSFS_uint32) (offset - finfo->uncompressed_position);
            if (maxread > sizeof (buf))
                maxread = sizeof (buf);

            if (ZIP_read(_io, buf, maxread) != maxread)
                return 0;
        } /* while */
    } /* else */

    return 1;
} /* ZIP_seek */


static PHYSFS_sint64 ZIP_length(PHYSFS_Io *io)
{
    const ZIPfileinfo *finfo = (ZIPfileinfo *) io->opaque;
    return (PHYSFS_sint64) finfo->entry->uncompressed_size;
} /* ZIP_length */


static PHYSFS_Io *zip_get_io(PHYSFS_Io *io, ZIPinfo *inf, ZIPentry *entry);

static PHYSFS_Io *ZIP_duplicate(PHYSFS_Io *io)
{
    ZIPfileinfo *origfinfo = (ZIPfileinfo *) io->opaque;
    PHYSFS_Io *retval = (PHYSFS_Io *) allocator.Malloc(sizeof (PHYSFS_Io));
    ZIPfileinfo *finfo = (ZIPfileinfo *) allocator.Malloc(sizeof (ZIPfileinfo));
    GOTO_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, failed);
    GOTO_IF(!finfo, PHYSFS_ERR_OUT_OF_MEMORY, failed);
    memset(finfo, '\0', sizeof (*finfo));

    finfo->entry = origfinfo->entry;
    finfo->io = zip_get_io(origfinfo->io, NULL, finfo->entry);
    GOTO_IF_ERRPASS(!finfo->io, failed);

    initializeZStream(&finfo->stream);
    if (finfo->entry->compression_method != COMPMETH_NONE)
    {
        finfo->buffer = (PHYSFS_uint8 *) allocator.Malloc(ZIP_READBUFSIZE);
        GOTO_IF(!finfo->buffer, PHYSFS_ERR_OUT_OF_MEMORY, failed);
        if (zlib_err(inflateInit2(&finfo->stream, -MAX_WBITS)) != Z_OK)
            goto failed;
    } /* if */

    memcpy(retval, io, sizeof (PHYSFS_Io));
    retval->opaque = finfo;
    return retval;

failed:
    if (finfo != NULL)
    {
        if (finfo->io != NULL)
            finfo->io->destroy(finfo->io);

        if (finfo->buffer != NULL)
        {
            allocator.Free(finfo->buffer);
            inflateEnd(&finfo->stream);
        } /* if */

        allocator.Free(finfo);
    } /* if */

    if (retval != NULL)
        allocator.Free(retval);

    return NULL;
} /* ZIP_duplicate */

static int ZIP_flush(PHYSFS_Io *io) { return 1;  /* no write support. */ }

static void ZIP_destroy(PHYSFS_Io *io)
{
    ZIPfileinfo *finfo = (ZIPfileinfo *) io->opaque;
    finfo->io->destroy(finfo->io);

    if (finfo->entry->compression_method != COMPMETH_NONE)
        inflateEnd(&finfo->stream);

    if (finfo->buffer != NULL)
        allocator.Free(finfo->buffer);

    allocator.Free(finfo);
    allocator.Free(io);
} /* ZIP_destroy */


static const PHYSFS_Io ZIP_Io =
{
    CURRENT_PHYSFS_IO_API_VERSION, NULL,
    ZIP_read,
    ZIP_write,
    ZIP_seek,
    ZIP_tell,
    ZIP_length,
    ZIP_duplicate,
    ZIP_flush,
    ZIP_destroy
};



static PHYSFS_sint64 zip_find_end_of_central_dir(PHYSFS_Io *io, PHYSFS_sint64 *len)
{
    PHYSFS_uint8 buf[256];
    PHYSFS_uint8 extra[4] = { 0, 0, 0, 0 };
    PHYSFS_sint32 i = 0;
    PHYSFS_sint64 filelen;
    PHYSFS_sint64 filepos;
    PHYSFS_sint32 maxread;
    PHYSFS_sint32 totalread = 0;
    int found = 0;

    filelen = io->length(io);
    BAIL_IF_ERRPASS(filelen == -1, -1);

    /*
     * Jump to the end of the file and start reading backwards.
     *  The last thing in the file is the zipfile comment, which is variable
     *  length, and the field that specifies its size is before it in the
     *  file (argh!)...this means that we need to scan backwards until we
     *  hit the end-of-central-dir signature. We can then sanity check that
     *  the comment was as big as it should be to make sure we're in the
     *  right place. The comment length field is 16 bits, so we can stop
     *  searching for that signature after a little more than 64k at most,
     *  and call it a corrupted zipfile.
     */

    if ((PHYSFS_sint64)sizeof (buf) < filelen)
    {
        filepos = filelen - sizeof (buf);
        maxread = sizeof (buf);
    } /* if */
    else
    {
        filepos = 0;
        maxread = (PHYSFS_uint32) filelen;
    } /* else */

    while ((totalread < filelen) && (totalread < 65557))
    {
        BAIL_IF_ERRPASS(!io->seek(io, filepos), -1);

        /* make sure we catch a signature between buffers. */
        if (totalread != 0)
        {
            if (!__PHYSFS_readAll(io, buf, maxread - 4))
                return -1;
            memcpy(&buf[maxread - 4], &extra, sizeof (extra));
            totalread += maxread - 4;
        } /* if */
        else
        {
            if (!__PHYSFS_readAll(io, buf, maxread))
                return -1;
            totalread += maxread;
        } /* else */

        memcpy(&extra, buf, sizeof (extra));

        for (i = maxread - 4; i > 0; i--)
        {
            if ((buf[i + 0] == 0x50) &&
                (buf[i + 1] == 0x4B) &&
                (buf[i + 2] == 0x05) &&
                (buf[i + 3] == 0x06) )
            {
                found = 1;  /* that's the signature! */
                break;
            } /* if */
        } /* for */

        if (found)
            break;

        filepos -= (maxread - 4);
        if (filepos < 0)
            filepos = 0;
    } /* while */

    BAIL_IF(!found, PHYSFS_ERR_UNSUPPORTED, -1);

    if (len != NULL)
        *len = filelen;

    return (filepos + i);
} /* zip_find_end_of_central_dir */


static int isZip(PHYSFS_Io *io)
{
    PHYSFS_uint32 sig = 0;
    int retval = 0;

    /*
     * The first thing in a zip file might be the signature of the
     *  first local file record, so it makes for a quick determination.
     */
    if (readui32(io, &sig))
    {
        retval = (sig == ZIP_LOCAL_FILE_SIG);
        if (!retval)
        {
            /*
             * No sig...might be a ZIP with data at the start
             *  (a self-extracting executable, etc), so we'll have to do
             *  it the hard way...
             */
            retval = (zip_find_end_of_central_dir(io, NULL) != -1);
        } /* if */
    } /* if */

    return retval;
} /* isZip */


/* Convert paths from old, buggy DOS zippers... */
static void zip_convert_dos_path(const PHYSFS_uint16 entryversion, char *path)
{
    const PHYSFS_uint8 hosttype = (PHYSFS_uint8) ((entryversion >> 8) & 0xFF);
    if (hosttype == 0)  /* FS_FAT_ */
    {
        while (*path)
        {
            if (*path == '\\')
                *path = '/';
            path++;
        } /* while */
    } /* if */
} /* zip_convert_dos_path */


static void zip_expand_symlink_path(char *path)
{
    char *ptr = path;
    char *prevptr = path;

    while (1)
    {
        ptr = strchr(ptr, '/');
        if (ptr == NULL)
            break;

        if (*(ptr + 1) == '.')
        {
            if (*(ptr + 2) == '/')
            {
                /* current dir in middle of string: ditch it. */
                memmove(ptr, ptr + 2, strlen(ptr + 2) + 1);
            } /* else if */

            else if (*(ptr + 2) == '\0')
            {
                /* current dir at end of string: ditch it. */
                *ptr = '\0';
            } /* else if */

            else if (*(ptr + 2) == '.')
            {
                if (*(ptr + 3) == '/')
                {
                    /* parent dir in middle: move back one, if possible. */
                    memmove(prevptr, ptr + 4, strlen(ptr + 4) + 1);
                    ptr = prevptr;
                    while (prevptr != path)
                    {
                        prevptr--;
                        if (*prevptr == '/')
                        {
                            prevptr++;
                            break;
                        } /* if */
                    } /* while */
                } /* if */

                if (*(ptr + 3) == '\0')
                {
                    /* parent dir at end: move back one, if possible. */
                    *prevptr = '\0';
                } /* if */
            } /* if */
        } /* if */
        else
        {
            prevptr = ptr;
            ptr++;
        } /* else */
    } /* while */
} /* zip_expand_symlink_path */


static inline ZIPentry *zip_find_entry(ZIPinfo *info, const char *path)
{
    return (ZIPentry *) __PHYSFS_DirTreeFind(&info->tree, path);
} /* zip_find_entry */

/* (forward reference: zip_follow_symlink and zip_resolve call each other.) */
static int zip_resolve(PHYSFS_Io *io, ZIPinfo *info, ZIPentry *entry);

/*
 * Look for the entry named by (path). If it exists, resolve it, and return
 *  a pointer to that entry. If it's another symlink, keep resolving until you
 *  hit a real file and then return a pointer to the final non-symlink entry.
 *  If there's a problem, return NULL.
 */
static ZIPentry *zip_follow_symlink(PHYSFS_Io *io, ZIPinfo *info, char *path)
{
    ZIPentry *entry;

    zip_expand_symlink_path(path);
    entry = zip_find_entry(info, path);
    if (entry != NULL)
    {
        if (!zip_resolve(io, info, entry))  /* recursive! */
            entry = NULL;
        else
        {
            if (entry->symlink != NULL)
                entry = entry->symlink;
        } /* else */
    } /* if */

    return entry;
} /* zip_follow_symlink */


static int zip_resolve_symlink(PHYSFS_Io *io, ZIPinfo *info, ZIPentry *entry)
{
    const size_t size = (size_t) entry->uncompressed_size;
    char *path = NULL;
    int rc = 0;

    /*
     * We've already parsed the local file header of the symlink at this
     *  point. Now we need to read the actual link from the file data and
     *  follow it.
     */

    BAIL_IF_ERRPASS(!io->seek(io, entry->offset), 0);

    path = (char *) __PHYSFS_smallAlloc(size + 1);
    BAIL_IF(!path, PHYSFS_ERR_OUT_OF_MEMORY, 0);

    if (entry->compression_method == COMPMETH_NONE)
        rc = __PHYSFS_readAll(io, path, size);

    else  /* symlink target path is compressed... */
    {
        z_stream stream;
        const size_t complen = (size_t) entry->compressed_size;
        PHYSFS_uint8 *compressed = (PHYSFS_uint8*) __PHYSFS_smallAlloc(complen);
        if (compressed != NULL)
        {
            if (__PHYSFS_readAll(io, compressed, complen))
            {
                initializeZStream(&stream);
                stream.next_in = compressed;
                stream.avail_in = (unsigned int) complen;
                stream.next_out = (unsigned char *) path;
                stream.avail_out = (unsigned int) size;
                if (zlib_err(inflateInit2(&stream, -MAX_WBITS)) == Z_OK)
                {
                    rc = zlib_err(inflate(&stream, Z_FINISH));
                    inflateEnd(&stream);

                    /* both are acceptable outcomes... */
                    rc = ((rc == Z_OK) || (rc == Z_STREAM_END));
                } /* if */
            } /* if */
            __PHYSFS_smallFree(compressed);
        } /* if */
    } /* else */

    if (rc)
    {
        path[entry->uncompressed_size] = '\0';    /* null-terminate it. */
        zip_convert_dos_path(entry->version, path);
        entry->symlink = zip_follow_symlink(io, info, path);
    } /* else */

    __PHYSFS_smallFree(path);

    return (entry->symlink != NULL);
} /* zip_resolve_symlink */


/*
 * Parse the local file header of an entry, and update entry->offset.
 */
static int zip_parse_local(PHYSFS_Io *io, ZIPentry *entry)
{
    PHYSFS_uint32 ui32;
    PHYSFS_uint16 ui16;
    PHYSFS_uint16 fnamelen;
    PHYSFS_uint16 extralen;

    /*
     * crc and (un)compressed_size are always zero if this is a "JAR"
     *  archive created with Sun's Java tools, apparently. We only
     *  consider this archive corrupted if those entries don't match and
     *  aren't zero. That seems to work well.
     * We also ignore a mismatch if the value is 0xFFFFFFFF here, since it's
     *  possible that's a Zip64 thing.
     */

    /* !!! FIXME: apparently these are zero if general purpose bit 3 is set,
       !!! FIXME:  which is probably true for Jar files, fwiw, but we don't
       !!! FIXME:  care about these values anyhow. */

    BAIL_IF_ERRPASS(!io->seek(io, entry->offset), 0);
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 != ZIP_LOCAL_FILE_SIG, PHYSFS_ERR_CORRUPT, 0);
    BAIL_IF_ERRPASS(!readui16(io, &ui16), 0);
    BAIL_IF(ui16 != entry->version_needed, PHYSFS_ERR_CORRUPT, 0);
    BAIL_IF_ERRPASS(!readui16(io, &ui16), 0);  /* general bits. */
    BAIL_IF_ERRPASS(!readui16(io, &ui16), 0);
    BAIL_IF(ui16 != entry->compression_method, PHYSFS_ERR_CORRUPT, 0);
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);  /* date/time */
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 && (ui32 != entry->crc), PHYSFS_ERR_CORRUPT, 0);

    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 && (ui32 != 0xFFFFFFFF) &&
                  (ui32 != entry->compressed_size), PHYSFS_ERR_CORRUPT, 0);

    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 && (ui32 != 0xFFFFFFFF) &&
                 (ui32 != entry->uncompressed_size), PHYSFS_ERR_CORRUPT, 0);

    BAIL_IF_ERRPASS(!readui16(io, &fnamelen), 0);
    BAIL_IF_ERRPASS(!readui16(io, &extralen), 0);

    entry->offset += fnamelen + extralen + 30;
    return 1;
} /* zip_parse_local */


static int zip_resolve(PHYSFS_Io *io, ZIPinfo *info, ZIPentry *entry)
{
    int retval = 1;
    const ZipResolveType resolve_type = entry->resolved;

    if (resolve_type == ZIP_DIRECTORY)
        return 1;   /* we're good. */

    /* Don't bother if we've failed to resolve this entry before. */
    BAIL_IF(resolve_type == ZIP_BROKEN_FILE, PHYSFS_ERR_CORRUPT, 0);
    BAIL_IF(resolve_type == ZIP_BROKEN_SYMLINK, PHYSFS_ERR_CORRUPT, 0);

    /* uhoh...infinite symlink loop! */
    BAIL_IF(resolve_type == ZIP_RESOLVING, PHYSFS_ERR_SYMLINK_LOOP, 0);

    /*
     * We fix up the offset to point to the actual data on the
     *  first open, since we don't want to seek across the whole file on
     *  archive open (can be SLOW on large, CD-stored files), but we
     *  need to check the local file header...not just for corruption,
     *  but since it stores offset info the central directory does not.
     */
    if (resolve_type != ZIP_RESOLVED)
    {
        if (entry->tree.isdir)  /* an ancestor dir that DirTree filled in? */
        {
            entry->resolved = ZIP_DIRECTORY;
            return 1;
        } /* if */

        retval = zip_parse_local(io, entry);
        if (retval)
        {
            /*
             * If it's a symlink, find the original file. This will cause
             *  resolution of other entries (other symlinks and, eventually,
             *  the real file) if all goes well.
             */
            if (resolve_type == ZIP_UNRESOLVED_SYMLINK)
                retval = zip_resolve_symlink(io, info, entry);
        } /* if */

        if (resolve_type == ZIP_UNRESOLVED_SYMLINK)
            entry->resolved = ((retval) ? ZIP_RESOLVED : ZIP_BROKEN_SYMLINK);
        else if (resolve_type == ZIP_UNRESOLVED_FILE)
            entry->resolved = ((retval) ? ZIP_RESOLVED : ZIP_BROKEN_FILE);
    } /* if */

    return retval;
} /* zip_resolve */


static int zip_entry_is_symlink(const ZIPentry *entry)
{
    return ((entry->resolved == ZIP_UNRESOLVED_SYMLINK) ||
            (entry->resolved == ZIP_BROKEN_SYMLINK) ||
            (entry->symlink));
} /* zip_entry_is_symlink */


static int zip_version_does_symlinks(PHYSFS_uint32 version)
{
    int retval = 0;
    PHYSFS_uint8 hosttype = (PHYSFS_uint8) ((version >> 8) & 0xFF);

    switch (hosttype)
    {
            /*
             * These are the platforms that can NOT build an archive with
             *  symlinks, according to the Info-ZIP project.
             */
        case 0:  /* FS_FAT_  */
        case 1:  /* AMIGA_   */
        case 2:  /* VMS_     */
        case 4:  /* VM_CSM_  */
        case 6:  /* FS_HPFS_ */
        case 11: /* FS_NTFS_ */
        case 14: /* FS_VFAT_ */
        case 13: /* ACORN_   */
        case 15: /* MVS_     */
        case 18: /* THEOS_   */
            break;  /* do nothing. */

        default:  /* assume the rest to be unix-like. */
            retval = 1;
            break;
    } /* switch */

    return retval;
} /* zip_version_does_symlinks */


static inline int zip_has_symlink_attr(const ZIPentry *entry,
                                       const PHYSFS_uint32 extern_attr)
{
    PHYSFS_uint16 xattr = ((extern_attr >> 16) & 0xFFFF);
    return ( (zip_version_does_symlinks(entry->version)) &&
             (entry->uncompressed_size > 0) &&
             ((xattr & UNIX_FILETYPE_MASK) == UNIX_FILETYPE_SYMLINK) );
} /* zip_has_symlink_attr */


static PHYSFS_sint64 zip_dos_time_to_physfs_time(PHYSFS_uint32 dostime)
{
    PHYSFS_uint32 dosdate;
    struct tm unixtime;
    memset(&unixtime, '\0', sizeof (unixtime));

    dosdate = (PHYSFS_uint32) ((dostime >> 16) & 0xFFFF);
    dostime &= 0xFFFF;

    /* dissect date */
    unixtime.tm_year = ((dosdate >> 9) & 0x7F) + 80;
    unixtime.tm_mon  = ((dosdate >> 5) & 0x0F) - 1;
    unixtime.tm_mday = ((dosdate     ) & 0x1F);

    /* dissect time */
    unixtime.tm_hour = ((dostime >> 11) & 0x1F);
    unixtime.tm_min  = ((dostime >>  5) & 0x3F);
    unixtime.tm_sec  = ((dostime <<  1) & 0x3E);

    /* let mktime calculate daylight savings time. */
    unixtime.tm_isdst = -1;

    return ((PHYSFS_sint64) mktime(&unixtime));
} /* zip_dos_time_to_physfs_time */


static ZIPentry *zip_load_entry(ZIPinfo *info, const int zip64,
                                const PHYSFS_uint64 ofs_fixup)
{
    PHYSFS_Io *io = info->io;
    ZIPentry entry;
    ZIPentry *retval = NULL;
    PHYSFS_uint16 fnamelen, extralen, commentlen;
    PHYSFS_uint32 external_attr;
    PHYSFS_uint32 starting_disk;
    PHYSFS_uint64 offset;
    PHYSFS_uint16 ui16;
    PHYSFS_uint32 ui32;
    PHYSFS_sint64 si64;
    char *name = NULL;
    int isdir = 0;

    /* sanity check with central directory signature... */
    BAIL_IF_ERRPASS(!readui32(io, &ui32), NULL);
    BAIL_IF(ui32 != ZIP_CENTRAL_DIR_SIG, PHYSFS_ERR_CORRUPT, NULL);

    memset(&entry, '\0', sizeof (entry));

    /* Get the pertinent parts of the record... */
    BAIL_IF_ERRPASS(!readui16(io, &entry.version), NULL);
    BAIL_IF_ERRPASS(!readui16(io, &entry.version_needed), NULL);
    BAIL_IF_ERRPASS(!readui16(io, &entry.general_bits), NULL);  /* general bits */
    BAIL_IF_ERRPASS(!readui16(io, &entry.compression_method), NULL);
    BAIL_IF_ERRPASS(!readui32(io, &entry.dos_mod_time), NULL);
    entry.last_mod_time = zip_dos_time_to_physfs_time(entry.dos_mod_time);
    BAIL_IF_ERRPASS(!readui32(io, &entry.crc), NULL);
    BAIL_IF_ERRPASS(!readui32(io, &ui32), NULL);
    entry.compressed_size = (PHYSFS_uint64) ui32;
    BAIL_IF_ERRPASS(!readui32(io, &ui32), NULL);
    entry.uncompressed_size = (PHYSFS_uint64) ui32;
    BAIL_IF_ERRPASS(!readui16(io, &fnamelen), NULL);
    BAIL_IF_ERRPASS(!readui16(io, &extralen), NULL);
    BAIL_IF_ERRPASS(!readui16(io, &commentlen), NULL);
    BAIL_IF_ERRPASS(!readui16(io, &ui16), NULL);
    starting_disk = (PHYSFS_uint32) ui16;
    BAIL_IF_ERRPASS(!readui16(io, &ui16), NULL);  /* internal file attribs */
    BAIL_IF_ERRPASS(!readui32(io, &external_attr), NULL);
    BAIL_IF_ERRPASS(!readui32(io, &ui32), NULL);
    offset = (PHYSFS_uint64) ui32;

    name = (char *) __PHYSFS_smallAlloc(fnamelen + 1);
    BAIL_IF(!name, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    if (!__PHYSFS_readAll(io, name, fnamelen))
    {
        __PHYSFS_smallFree(name);
        return NULL;
    } /* if */

    if (name[fnamelen - 1] == '/')
    {
        name[fnamelen - 1] = '\0';
        isdir = 1;
    } /* if */
    name[fnamelen] = '\0';  /* null-terminate the filename. */

    zip_convert_dos_path(entry.version, name);

    retval = (ZIPentry *) __PHYSFS_DirTreeAdd(&info->tree, name, isdir);
    __PHYSFS_smallFree(name);

    BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);

    /* It's okay to BAIL without freeing retval, because it's stored in the
       __PHYSFS_DirTree and will be freed later anyhow. */
    BAIL_IF(retval->last_mod_time != 0, PHYSFS_ERR_CORRUPT, NULL); /* dupe? */

    /* Move the data we already read into place in the official object. */
    memcpy(((PHYSFS_uint8 *) retval) + sizeof (__PHYSFS_DirTreeEntry),
           ((PHYSFS_uint8 *) &entry) + sizeof (__PHYSFS_DirTreeEntry),
           sizeof (*retval) - sizeof (__PHYSFS_DirTreeEntry));

    retval->symlink = NULL;  /* will be resolved later, if necessary. */

    if (isdir)
        retval->resolved = ZIP_DIRECTORY;
    else
    {
        retval->resolved = (zip_has_symlink_attr(retval, external_attr)) ?
                                ZIP_UNRESOLVED_SYMLINK : ZIP_UNRESOLVED_FILE;
    } /* else */

    si64 = io->tell(io);
    BAIL_IF_ERRPASS(si64 == -1, NULL);

    /* If the actual sizes didn't fit in 32-bits, look for the Zip64
        extended information extra field... */
    if ( (zip64) &&
         ((offset == 0xFFFFFFFF) ||
          (starting_disk == 0xFFFFFFFF) ||
          (retval->compressed_size == 0xFFFFFFFF) ||
          (retval->uncompressed_size == 0xFFFFFFFF)) )
    {
        int found = 0;
        PHYSFS_uint16 sig = 0;
        PHYSFS_uint16 len = 0;
        while (extralen > 4)
        {
            BAIL_IF_ERRPASS(!readui16(io, &sig), NULL);
            BAIL_IF_ERRPASS(!readui16(io, &len), NULL);

            si64 += 4 + len;
            extralen -= 4 + len;
            if (sig != ZIP64_EXTENDED_INFO_EXTRA_FIELD_SIG)
            {
                BAIL_IF_ERRPASS(!io->seek(io, si64), NULL);
                continue;
            } /* if */

            found = 1;
            break;
        } /* while */

        BAIL_IF(!found, PHYSFS_ERR_CORRUPT, NULL);

        if (retval->uncompressed_size == 0xFFFFFFFF)
        {
            BAIL_IF(len < 8, PHYSFS_ERR_CORRUPT, NULL);
            BAIL_IF_ERRPASS(!readui64(io, &retval->uncompressed_size), NULL);
            len -= 8;
        } /* if */

        if (retval->compressed_size == 0xFFFFFFFF)
        {
            BAIL_IF(len < 8, PHYSFS_ERR_CORRUPT, NULL);
            BAIL_IF_ERRPASS(!readui64(io, &retval->compressed_size), NULL);
            len -= 8;
        } /* if */

        if (offset == 0xFFFFFFFF)
        {
            BAIL_IF(len < 8, PHYSFS_ERR_CORRUPT, NULL);
            BAIL_IF_ERRPASS(!readui64(io, &offset), NULL);
            len -= 8;
        } /* if */

        if (starting_disk == 0xFFFFFFFF)
        {
            BAIL_IF(len < 8, PHYSFS_ERR_CORRUPT, NULL);
            BAIL_IF_ERRPASS(!readui32(io, &starting_disk), NULL);
            len -= 4;
        } /* if */

        BAIL_IF(len != 0, PHYSFS_ERR_CORRUPT, NULL);
    } /* if */

    BAIL_IF(starting_disk != 0, PHYSFS_ERR_CORRUPT, NULL);

    retval->offset = offset + ofs_fixup;

    /* seek to the start of the next entry in the central directory... */
    BAIL_IF_ERRPASS(!io->seek(io, si64 + extralen + commentlen), NULL);

    return retval;  /* success. */
} /* zip_load_entry */


/* This leaves things allocated on error; the caller will clean up the mess. */
static int zip_load_entries(ZIPinfo *info,
                            const PHYSFS_uint64 data_ofs,
                            const PHYSFS_uint64 central_ofs,
                            const PHYSFS_uint64 entry_count)
{
    PHYSFS_Io *io = info->io;
    const int zip64 = info->zip64;
    PHYSFS_uint64 i;

    BAIL_IF_ERRPASS(!io->seek(io, central_ofs), 0);

    for (i = 0; i < entry_count; i++)
    {
        ZIPentry *entry = zip_load_entry(info, zip64, data_ofs);
        BAIL_IF_ERRPASS(!entry, 0);
        if (zip_entry_is_tradional_crypto(entry))
            info->has_crypto = 1;
    } /* for */

    return 1;
} /* zip_load_entries */


static PHYSFS_sint64 zip64_find_end_of_central_dir(PHYSFS_Io *io,
                                                   PHYSFS_sint64 _pos,
                                                   PHYSFS_uint64 offset)
{
    /*
     * Naturally, the offset is useless to us; it is the offset from the
     *  start of file, which is meaningless if we've appended this .zip to
     *  a self-extracting .exe. We need to find this on our own. It should
     *  be directly before the locator record, but the record in question,
     *  like the original end-of-central-directory record, ends with a
     *  variable-length field. Unlike the original, which has to store the
     *  size of that variable-length field in a 16-bit int and thus has to be
     *  within 64k, the new one gets 64-bits.
     *
     * Fortunately, the only currently-specified record for that variable
     *  length block is some weird proprietary thing that deals with EBCDIC
     *  and tape backups or something. So we don't seek far.
     */

    PHYSFS_uint32 ui32;
    const PHYSFS_uint64 pos = (PHYSFS_uint64) _pos;

    assert(_pos > 0);

    /* Try offset specified in the Zip64 end of central directory locator. */
    /* This works if the entire PHYSFS_Io is the zip file. */
    BAIL_IF_ERRPASS(!io->seek(io, offset), -1);
    BAIL_IF_ERRPASS(!readui32(io, &ui32), -1);
    if (ui32 == ZIP64_END_OF_CENTRAL_DIR_SIG)
        return offset;

    /* Try 56 bytes before the Zip64 end of central directory locator. */
    /* This works if the record isn't variable length and is version 1. */
    if (pos > 56)
    {
        BAIL_IF_ERRPASS(!io->seek(io, pos-56), -1);
        BAIL_IF_ERRPASS(!readui32(io, &ui32), -1);
        if (ui32 == ZIP64_END_OF_CENTRAL_DIR_SIG)
            return pos-56;
    } /* if */

    /* Try 84 bytes before the Zip64 end of central directory locator. */
    /* This works if the record isn't variable length and is version 2. */
    if (pos > 84)
    {
        BAIL_IF_ERRPASS(!io->seek(io, pos-84), -1);
        BAIL_IF_ERRPASS(!readui32(io, &ui32), -1);
        if (ui32 == ZIP64_END_OF_CENTRAL_DIR_SIG)
            return pos-84;
    } /* if */

    /* Ok, brute force: we know it's between (offset) and (pos) somewhere. */
    /*  Just try moving back at most 256k. Oh well. */
    if ((offset < pos) && (pos > 4))
    {
        const size_t maxbuflen = 256 * 1024;
        size_t len = (size_t) (pos - offset);
        PHYSFS_uint8 *buf = NULL;
        PHYSFS_sint32 i;

        if (len > maxbuflen)
            len = maxbuflen;

        buf = (PHYSFS_uint8 *) __PHYSFS_smallAlloc(len);
        BAIL_IF(!buf, PHYSFS_ERR_OUT_OF_MEMORY, -1);

        if (!io->seek(io, pos - len) || !__PHYSFS_readAll(io, buf, len))
        {
            __PHYSFS_smallFree(buf);
            return -1;  /* error was set elsewhere. */
        } /* if */

        for (i = (PHYSFS_sint32) (len - 4); i >= 0; i--)
        {
            if ( (buf[i] == 0x50) && (buf[i+1] == 0x4b) &&
                 (buf[i+2] == 0x06) && (buf[i+3] == 0x06) )
            {
                __PHYSFS_smallFree(buf);
                return pos - ((PHYSFS_sint64) (len - i));
            } /* if */
        } /* for */

        __PHYSFS_smallFree(buf);
    } /* if */

    BAIL(PHYSFS_ERR_CORRUPT, -1);  /* didn't find it. */
} /* zip64_find_end_of_central_dir */


static int zip64_parse_end_of_central_dir(ZIPinfo *info,
                                          PHYSFS_uint64 *data_start,
                                          PHYSFS_uint64 *dir_ofs,
                                          PHYSFS_uint64 *entry_count,
                                          PHYSFS_sint64 pos)
{
    PHYSFS_Io *io = info->io;
    PHYSFS_uint64 ui64;
    PHYSFS_uint32 ui32;
    PHYSFS_uint16 ui16;

    /* We should be positioned right past the locator signature. */

    if ((pos < 0) || (!io->seek(io, pos)))
        return 0;

    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    if (ui32 != ZIP64_END_OF_CENTRAL_DIRECTORY_LOCATOR_SIG)
        return -1;  /* it's not a Zip64 archive. Not an error, though! */

    info->zip64 = 1;

    /* number of the disk with the start of the central directory. */
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 != 0, PHYSFS_ERR_CORRUPT, 0);

    /* offset of Zip64 end of central directory record. */
    BAIL_IF_ERRPASS(!readui64(io, &ui64), 0);

    /* total number of disks */
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 != 1, PHYSFS_ERR_CORRUPT, 0);

    pos = zip64_find_end_of_central_dir(io, pos, ui64);
    if (pos < 0)
        return 0;  /* oh well. */

    /*
     * For self-extracting archives, etc, there's crapola in the file
     *  before the zipfile records; we calculate how much data there is
     *  prepended by determining how far the zip64-end-of-central-directory
     *  offset is from where it is supposed to be...the difference in bytes
     *  is how much arbitrary data is at the start of the physical file.
     */
    assert(((PHYSFS_uint64) pos) >= ui64);
    *data_start = ((PHYSFS_uint64) pos) - ui64;

    BAIL_IF_ERRPASS(!io->seek(io, pos), 0);

    /* check signature again, just in case. */
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 != ZIP64_END_OF_CENTRAL_DIR_SIG, PHYSFS_ERR_CORRUPT, 0);

    /* size of Zip64 end of central directory record. */
    BAIL_IF_ERRPASS(!readui64(io, &ui64), 0);

    /* version made by. */
    BAIL_IF_ERRPASS(!readui16(io, &ui16), 0);

    /* version needed to extract. */
    BAIL_IF_ERRPASS(!readui16(io, &ui16), 0);

    /* number of this disk. */
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 != 0, PHYSFS_ERR_CORRUPT, 0);

    /* number of disk with start of central directory record. */
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 != 0, PHYSFS_ERR_CORRUPT, 0);

    /* total number of entries in the central dir on this disk */
    BAIL_IF_ERRPASS(!readui64(io, &ui64), 0);

    /* total number of entries in the central dir */
    BAIL_IF_ERRPASS(!readui64(io, entry_count), 0);
    BAIL_IF(ui64 != *entry_count, PHYSFS_ERR_CORRUPT, 0);

    /* size of the central directory */
    BAIL_IF_ERRPASS(!readui64(io, &ui64), 0);

    /* offset of central directory */
    BAIL_IF_ERRPASS(!readui64(io, dir_ofs), 0);

    /* Since we know the difference, fix up the central dir offset... */
    *dir_ofs += *data_start;

    /*
     * There are more fields here, for encryption and feature-specific things,
     *  but we don't care about any of them at the moment.
     */

    return 1;  /* made it. */
} /* zip64_parse_end_of_central_dir */


static int zip_parse_end_of_central_dir(ZIPinfo *info,
                                        PHYSFS_uint64 *data_start,
                                        PHYSFS_uint64 *dir_ofs,
                                        PHYSFS_uint64 *entry_count)
{
    PHYSFS_Io *io = info->io;
    PHYSFS_uint16 entryCount16;
    PHYSFS_uint32 offset32;
    PHYSFS_uint32 ui32;
    PHYSFS_uint16 ui16;
    PHYSFS_sint64 len;
    PHYSFS_sint64 pos;
    int rc;

    /* find the end-of-central-dir record, and seek to it. */
    pos = zip_find_end_of_central_dir(io, &len);
    BAIL_IF_ERRPASS(pos == -1, 0);
    BAIL_IF_ERRPASS(!io->seek(io, pos), 0);

    /* check signature again, just in case. */
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);
    BAIL_IF(ui32 != ZIP_END_OF_CENTRAL_DIR_SIG, PHYSFS_ERR_CORRUPT, 0);

    /* Seek back to see if "Zip64 end of central directory locator" exists. */
    /* this record is 20 bytes before end-of-central-dir */
    rc = zip64_parse_end_of_central_dir(info, data_start, dir_ofs,
                                        entry_count, pos - 20);

    /* Error or success? Bounce out of here. Keep going if not zip64. */
    if ((rc == 0) || (rc == 1))
        return rc;

    assert(rc == -1);  /* no error, just not a Zip64 archive. */

    /* Not Zip64? Seek back to where we were and keep processing. */
    BAIL_IF_ERRPASS(!io->seek(io, pos + 4), 0);

    /* number of this disk */
    BAIL_IF_ERRPASS(!readui16(io, &ui16), 0);
    BAIL_IF(ui16 != 0, PHYSFS_ERR_CORRUPT, 0);

    /* number of the disk with the start of the central directory */
    BAIL_IF_ERRPASS(!readui16(io, &ui16), 0);
    BAIL_IF(ui16 != 0, PHYSFS_ERR_CORRUPT, 0);

    /* total number of entries in the central dir on this disk */
    BAIL_IF_ERRPASS(!readui16(io, &ui16), 0);

    /* total number of entries in the central dir */
    BAIL_IF_ERRPASS(!readui16(io, &entryCount16), 0);
    BAIL_IF(ui16 != entryCount16, PHYSFS_ERR_CORRUPT, 0);

    *entry_count = entryCount16;

    /* size of the central directory */
    BAIL_IF_ERRPASS(!readui32(io, &ui32), 0);

    /* offset of central directory */
    BAIL_IF_ERRPASS(!readui32(io, &offset32), 0);
    *dir_ofs = (PHYSFS_uint64) offset32;
    BAIL_IF(((PHYSFS_uint64) pos) < (*dir_ofs + ui32), PHYSFS_ERR_CORRUPT, 0);

    /*
     * For self-extracting archives, etc, there's crapola in the file
     *  before the zipfile records; we calculate how much data there is
     *  prepended by determining how far the central directory offset is
     *  from where it is supposed to be (start of end-of-central-dir minus
     *  sizeof central dir)...the difference in bytes is how much arbitrary
     *  data is at the start of the physical file.
     */
    *data_start = (PHYSFS_uint64) (pos - (*dir_ofs + ui32));

    /* Now that we know the difference, fix up the central dir offset... */
    *dir_ofs += *data_start;

    /* zipfile comment length */
    BAIL_IF_ERRPASS(!readui16(io, &ui16), 0);

    /*
     * Make sure that the comment length matches to the end of file...
     *  If it doesn't, we're either in the wrong part of the file, or the
     *  file is corrupted, but we give up either way.
     */
    BAIL_IF((pos + 22 + ui16) != len, PHYSFS_ERR_CORRUPT, 0);

    return 1;  /* made it. */
} /* zip_parse_end_of_central_dir */


static void ZIP_closeArchive(void *opaque)
{
    ZIPinfo *info = (ZIPinfo *) (opaque);

    if (!info)
        return;

    if (info->io)
        info->io->destroy(info->io);

    __PHYSFS_DirTreeDeinit(&info->tree);

    allocator.Free(info);
} /* ZIP_closeArchive */


static void *ZIP_openArchive(PHYSFS_Io *io, const char *name,
                             int forWriting, int *claimed)
{
    ZIPinfo *info = NULL;
    ZIPentry *root = NULL;
    PHYSFS_uint64 dstart = 0;  /* data start */
    PHYSFS_uint64 cdir_ofs;  /* central dir offset */
    PHYSFS_uint64 count;

    assert(io != NULL);  /* shouldn't ever happen. */

    BAIL_IF(forWriting, PHYSFS_ERR_READ_ONLY, NULL);
    BAIL_IF_ERRPASS(!isZip(io), NULL);

    *claimed = 1;

    info = (ZIPinfo *) allocator.Malloc(sizeof (ZIPinfo));
    BAIL_IF(!info, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    memset(info, '\0', sizeof (ZIPinfo));

    info->io = io;

    if (!zip_parse_end_of_central_dir(info, &dstart, &cdir_ofs, &count))
        goto ZIP_openarchive_failed;
    else if (!__PHYSFS_DirTreeInit(&info->tree, sizeof (ZIPentry)))
        goto ZIP_openarchive_failed;

    root = (ZIPentry *) info->tree.root;
    root->resolved = ZIP_DIRECTORY;

    if (!zip_load_entries(info, dstart, cdir_ofs, count))
        goto ZIP_openarchive_failed;

    assert(info->tree.root->sibling == NULL);
    return info;

ZIP_openarchive_failed:
    info->io = NULL;  /* don't let ZIP_closeArchive destroy (io). */
    ZIP_closeArchive(info);
    return NULL;
} /* ZIP_openArchive */


static PHYSFS_Io *zip_get_io(PHYSFS_Io *io, ZIPinfo *inf, ZIPentry *entry)
{
    int success;
    PHYSFS_Io *retval = io->duplicate(io);
    BAIL_IF_ERRPASS(!retval, NULL);

    assert(!entry->tree.isdir); /* should have been checked before calling. */

    /* (inf) can be NULL if we already resolved. */
    success = (inf == NULL) || zip_resolve(retval, inf, entry);
    if (success)
    {
        PHYSFS_sint64 offset;
        offset = ((entry->symlink) ? entry->symlink->offset : entry->offset);
        success = retval->seek(retval, offset);
    } /* if */

    if (!success)
    {
        retval->destroy(retval);
        retval = NULL;
    } /* if */

    return retval;
} /* zip_get_io */


static PHYSFS_Io *ZIP_openRead(void *opaque, const char *filename)
{
    PHYSFS_Io *retval = NULL;
    ZIPinfo *info = (ZIPinfo *) opaque;
    ZIPentry *entry = zip_find_entry(info, filename);
    ZIPfileinfo *finfo = NULL;
    PHYSFS_Io *io = NULL;
    PHYSFS_uint8 *password = NULL;

    /* if not found, see if maybe "$PASSWORD" is appended. */
    if ((!entry) && (info->has_crypto))
    {
        const char *ptr = strrchr(filename, '$');
        if (ptr != NULL)
        {
            const size_t len = (size_t) (ptr - filename);
            char *str = (char *) __PHYSFS_smallAlloc(len + 1);
            BAIL_IF(!str, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
            memcpy(str, filename, len);
            str[len] = '\0';
            entry = zip_find_entry(info, str);
            __PHYSFS_smallFree(str);
            password = (PHYSFS_uint8 *) (ptr + 1);
        } /* if */
    } /* if */

    BAIL_IF_ERRPASS(!entry, NULL);

    BAIL_IF_ERRPASS(!zip_resolve(info->io, info, entry), NULL);

    BAIL_IF(entry->tree.isdir, PHYSFS_ERR_NOT_A_FILE, NULL);

    retval = (PHYSFS_Io *) allocator.Malloc(sizeof (PHYSFS_Io));
    GOTO_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, ZIP_openRead_failed);

    finfo = (ZIPfileinfo *) allocator.Malloc(sizeof (ZIPfileinfo));
    GOTO_IF(!finfo, PHYSFS_ERR_OUT_OF_MEMORY, ZIP_openRead_failed);
    memset(finfo, '\0', sizeof (ZIPfileinfo));

    io = zip_get_io(info->io, info, entry);
    GOTO_IF_ERRPASS(!io, ZIP_openRead_failed);
    finfo->io = io;
    finfo->entry = ((entry->symlink != NULL) ? entry->symlink : entry);
    initializeZStream(&finfo->stream);

    if (finfo->entry->compression_method != COMPMETH_NONE)
    {
        finfo->buffer = (PHYSFS_uint8 *) allocator.Malloc(ZIP_READBUFSIZE);
        if (!finfo->buffer)
            GOTO(PHYSFS_ERR_OUT_OF_MEMORY, ZIP_openRead_failed);
        else if (zlib_err(inflateInit2(&finfo->stream, -MAX_WBITS)) != Z_OK)
            goto ZIP_openRead_failed;
    } /* if */

    if (!zip_entry_is_tradional_crypto(entry))
        GOTO_IF(password != NULL, PHYSFS_ERR_BAD_PASSWORD, ZIP_openRead_failed);
    else
    {
        PHYSFS_uint8 crypto_header[12];
        GOTO_IF(password == NULL, PHYSFS_ERR_BAD_PASSWORD, ZIP_openRead_failed);
        if (io->read(io, crypto_header, 12) != 12)
            goto ZIP_openRead_failed;
        else if (!zip_prep_crypto_keys(finfo, crypto_header, password))
            goto ZIP_openRead_failed;
    } /* if */

    memcpy(retval, &ZIP_Io, sizeof (PHYSFS_Io));
    retval->opaque = finfo;

    return retval;

ZIP_openRead_failed:
    if (finfo != NULL)
    {
        if (finfo->io != NULL)
            finfo->io->destroy(finfo->io);

        if (finfo->buffer != NULL)
        {
            allocator.Free(finfo->buffer);
            inflateEnd(&finfo->stream);
        } /* if */

        allocator.Free(finfo);
    } /* if */

    if (retval != NULL)
        allocator.Free(retval);

    return NULL;
} /* ZIP_openRead */


static PHYSFS_Io *ZIP_openWrite(void *opaque, const char *filename)
{
    BAIL(PHYSFS_ERR_READ_ONLY, NULL);
} /* ZIP_openWrite */


static PHYSFS_Io *ZIP_openAppend(void *opaque, const char *filename)
{
    BAIL(PHYSFS_ERR_READ_ONLY, NULL);
} /* ZIP_openAppend */


static int ZIP_remove(void *opaque, const char *name)
{
    BAIL(PHYSFS_ERR_READ_ONLY, 0);
} /* ZIP_remove */


static int ZIP_mkdir(void *opaque, const char *name)
{
    BAIL(PHYSFS_ERR_READ_ONLY, 0);
} /* ZIP_mkdir */


static int ZIP_stat(void *opaque, const char *filename, PHYSFS_Stat *stat)
{
    ZIPinfo *info = (ZIPinfo *) opaque;
    ZIPentry *entry = zip_find_entry(info, filename);

    if (entry == NULL)
        return 0;

    else if (!zip_resolve(info->io, info, entry))
        return 0;

    else if (entry->resolved == ZIP_DIRECTORY)
    {
        stat->filesize = 0;
        stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
    } /* if */

    else if (zip_entry_is_symlink(entry))
    {
        stat->filesize = 0;
        stat->filetype = PHYSFS_FILETYPE_SYMLINK;
    } /* else if */

    else
    {
        stat->filesize = (PHYSFS_sint64) entry->uncompressed_size;
        stat->filetype = PHYSFS_FILETYPE_REGULAR;
    } /* else */

    stat->modtime = ((entry) ? entry->last_mod_time : 0);
    stat->createtime = stat->modtime;
    stat->accesstime = -1;
    stat->readonly = 1; /* .zip files are always read only */

    return 1;
} /* ZIP_stat */


const PHYSFS_Archiver __PHYSFS_Archiver_ZIP =
{
    CURRENT_PHYSFS_ARCHIVER_API_VERSION,
    {
        "ZIP",
        "PkZip/WinZip/Info-Zip compatible",
        "Ryan C. Gordon <icculus@icculus.org>",
        "https://icculus.org/physfs/",
        1,  /* supportsSymlinks */
    },
    ZIP_openArchive,
    __PHYSFS_DirTreeEnumerate,
    ZIP_openRead,
    ZIP_openWrite,
    ZIP_openAppend,
    ZIP_remove,
    ZIP_mkdir,
    ZIP_stat,
    ZIP_closeArchive
};

#endif  /* defined PHYSFS_SUPPORTS_ZIP */

/* end of physfs_archiver_zip.c ... */

#ifdef __cplusplus
}
#endif

#endif /* PHYSFS_IMPL */


#ifdef PHYSFS_PLATFORM_IMPL

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Android support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_platforms.h"
*/
#ifdef PHYSFS_PLATFORM_ANDROID

#include <jni.h>
#include <android/log.h>
/*#include "physfs_internal.h"*/

static char *prefpath = NULL;


int __PHYSFS_platformInit(void)
{
    return 1;  /* always succeed. */
} /* __PHYSFS_platformInit */


void __PHYSFS_platformDeinit(void)
{
    if (prefpath)
    {
        allocator.Free(prefpath);
        prefpath = NULL;
    } /* if */
} /* __PHYSFS_platformDeinit */


void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data)
{
    /* no-op. */
} /* __PHYSFS_platformDetectAvailableCDs */


char *__PHYSFS_platformCalcBaseDir(const char *argv0)
{
    /* as a cheat, we expect argv0 to be a PHYSFS_AndroidInit* on Android. */
    PHYSFS_AndroidInit *ainit = (PHYSFS_AndroidInit *) argv0;
    char *retval = NULL;
    JNIEnv *jenv = NULL;
    jobject jcontext;

    if (ainit == NULL)
        return __PHYSFS_strdup("/");  /* oh well. */

    jenv = (JNIEnv *) ainit->jnienv;
    jcontext = (jobject) ainit->context;

    if ((*jenv)->PushLocalFrame(jenv, 16) >= 0)
    {
        jobject jfileobj = 0;
        jmethodID jmeth = 0;
        jthrowable jexception = 0;
        jstring jstr = 0;

        jmeth = (*jenv)->GetMethodID(jenv, (*jenv)->GetObjectClass(jenv, jcontext), "getPackageResourcePath", "()Ljava/lang/String;");
        jstr = (jstring)(*jenv)->CallObjectMethod(jenv, jcontext, jmeth);
        jexception = (*jenv)->ExceptionOccurred(jenv);  /* this can't throw an exception, right? Just in case. */
        if (jexception != NULL)
            (*jenv)->ExceptionClear(jenv);
        else
        {
            const char *path = (*jenv)->GetStringUTFChars(jenv, jstr, NULL);
            retval = __PHYSFS_strdup(path);
            (*jenv)->ReleaseStringUTFChars(jenv, jstr, path);
        } /* else */

        /* We only can rely on the Activity being valid during this function call,
           so go ahead and grab the prefpath too. */
        jmeth = (*jenv)->GetMethodID(jenv, (*jenv)->GetObjectClass(jenv, jcontext), "getFilesDir", "()Ljava/io/File;");
        jfileobj = (*jenv)->CallObjectMethod(jenv, jcontext, jmeth);
        if (jfileobj)
        {
            jmeth = (*jenv)->GetMethodID(jenv, (*jenv)->GetObjectClass(jenv, jfileobj), "getCanonicalPath", "()Ljava/lang/String;");
            jstr = (jstring)(*jenv)->CallObjectMethod(jenv, jfileobj, jmeth);
            jexception = (*jenv)->ExceptionOccurred(jenv);
            if (jexception != NULL)
                (*jenv)->ExceptionClear(jenv);
            else
            {
                const char *path = (*jenv)->GetStringUTFChars(jenv, jstr, NULL);
                const size_t len = strlen(path) + 2;
                prefpath = (char*)allocator.Malloc(len);
                if (prefpath)
                    snprintf(prefpath, len, "%s/", path);
                (*jenv)->ReleaseStringUTFChars(jenv, jstr, path);
            } /* else */
        } /* if */

        (*jenv)->PopLocalFrame(jenv, NULL);
    } /* if */

    /* we can't return NULL because then PhysicsFS will treat argv0 as a string, but it's a non-NULL jobject! */
    if (retval == NULL)
        retval = __PHYSFS_strdup("/");   /* we pray this works. */

    return retval;
} /* __PHYSFS_platformCalcBaseDir */


char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app)
{
    return __PHYSFS_strdup(prefpath ? prefpath : "/");
} /* __PHYSFS_platformCalcPrefDir */

#endif /* PHYSFS_PLATFORM_ANDROID */

/* end of physfs_platform_android.c ... */

/*
 * OS/2 support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_platforms.h"
*/
#ifdef PHYSFS_PLATFORM_OS2

#define INCL_DOSMODULEMGR
#define INCL_DOSSEMAPHORES
#define INCL_DOSDATETIME
#define INCL_DOSFILEMGR
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS
#define INCL_DOSPROCESS
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSMISC
#include <os2.h>
#include <uconv.h>

#include <errno.h>
#include <time.h>
#include <ctype.h>

/*#include "physfs_internal.h"*/

static HMODULE uconvdll = 0;
static UconvObject uconv = 0;
static int (_System *pUniCreateUconvObject)(UniChar *, UconvObject *) = NULL;
static int (_System *pUniFreeUconvObject)(UconvObject *) = NULL;
static int (_System *pUniUconvToUcs)(UconvObject,void **,size_t *, UniChar**, size_t *, size_t *) = NULL;
static int (_System *pUniUconvFromUcs)(UconvObject,UniChar **,size_t *,void **,size_t *,size_t *) = NULL;

static PHYSFS_ErrorCode errcodeFromAPIRET(const APIRET rc)
{
    switch (rc)
    {
        case NO_ERROR: return PHYSFS_ERR_OK;  /* not an error. */
        case ERROR_INTERRUPT: return PHYSFS_ERR_OK;  /* not an error. */
        case ERROR_TIMEOUT: return PHYSFS_ERR_OK;  /* not an error. */
        case ERROR_NOT_ENOUGH_MEMORY: return PHYSFS_ERR_OUT_OF_MEMORY;
        case ERROR_FILE_NOT_FOUND: return PHYSFS_ERR_NOT_FOUND;
        case ERROR_PATH_NOT_FOUND: return PHYSFS_ERR_NOT_FOUND;
        case ERROR_ACCESS_DENIED: return PHYSFS_ERR_PERMISSION;
        case ERROR_NOT_DOS_DISK: return PHYSFS_ERR_NOT_FOUND;
        case ERROR_SHARING_VIOLATION: return PHYSFS_ERR_PERMISSION;
        case ERROR_CANNOT_MAKE: return PHYSFS_ERR_IO;  /* maybe this is wrong? */
        case ERROR_DEVICE_IN_USE: return PHYSFS_ERR_BUSY;
        case ERROR_OPEN_FAILED: return PHYSFS_ERR_IO;  /* maybe this is wrong? */
        case ERROR_DISK_FULL: return PHYSFS_ERR_NO_SPACE;
        case ERROR_PIPE_BUSY: return PHYSFS_ERR_BUSY;
        case ERROR_SHARING_BUFFER_EXCEEDED: return PHYSFS_ERR_IO;
        case ERROR_FILENAME_EXCED_RANGE: return PHYSFS_ERR_BAD_FILENAME;
        case ERROR_META_EXPANSION_TOO_LONG: return PHYSFS_ERR_BAD_FILENAME;
        case ERROR_TOO_MANY_HANDLES: return PHYSFS_ERR_IO;
        case ERROR_TOO_MANY_OPEN_FILES: return PHYSFS_ERR_IO;
        case ERROR_NO_MORE_SEARCH_HANDLES: return PHYSFS_ERR_IO;
        case ERROR_SEEK_ON_DEVICE: return PHYSFS_ERR_IO;
        case ERROR_NEGATIVE_SEEK: return PHYSFS_ERR_INVALID_ARGUMENT;
        case ERROR_WRITE_PROTECT: return PHYSFS_ERR_PERMISSION;
        case ERROR_WRITE_FAULT: return PHYSFS_ERR_IO;
        case ERROR_UNCERTAIN_MEDIA: return PHYSFS_ERR_IO;
        case ERROR_PROTECTION_VIOLATION: return PHYSFS_ERR_IO;
        case ERROR_BROKEN_PIPE: return PHYSFS_ERR_IO;

        /* !!! FIXME: some of these might be PHYSFS_ERR_BAD_FILENAME, etc */
        case ERROR_LOCK_VIOLATION:
        case ERROR_GEN_FAILURE:
        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_NAME:
        case ERROR_INVALID_DRIVE:
        case ERROR_INVALID_HANDLE:
        case ERROR_INVALID_FUNCTION:
        case ERROR_INVALID_LEVEL:
        case ERROR_INVALID_CATEGORY:
        case ERROR_DUPLICATE_NAME:
        case ERROR_BUFFER_OVERFLOW:
        case ERROR_BAD_LENGTH:
        case ERROR_BAD_DRIVER_LEVEL:
        case ERROR_DIRECT_ACCESS_HANDLE:
        case ERROR_NOT_OWNER:
            return PHYSFS_ERR_OS_ERROR;

        default: break;
    } /* switch */

    return PHYSFS_ERR_OTHER_ERROR;
} /* errcodeFromAPIRET */

static char *cvtUtf8ToCodepage(const char *utf8str)
{
    const size_t len = strlen(utf8str) + 1;
    const size_t uc2buflen = len * sizeof (UniChar);
    UniChar *uc2ptr = (UniChar *) __PHYSFS_smallAlloc(uc2buflen);
    UniChar *uc2str = uc2ptr;
    char *cpptr = NULL;
    char *cpstr = NULL;
    size_t subs = 0;
    size_t unilen;

    BAIL_IF(!uc2str, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    PHYSFS_utf8ToUcs2(utf8str, (PHYSFS_uint16 *) uc2str, uc2buflen);
    for (unilen = 0; uc2str[unilen]; unilen++) { /* spin */ }
    unilen++;  /* null terminator. */

    if (!uconvdll)
    {
        /* There's really not much we can do on older OS/2s except pray this
           is latin1-compatible. */
        size_t i;
        cpptr = (char *) allocator.Malloc(unilen);
        cpstr = cpptr;
        GOTO_IF(!cpptr, PHYSFS_ERR_OUT_OF_MEMORY, failed);
        for (i = 0; i < unilen; i++)
        {
            const UniChar ch = uc2str[i];
            GOTO_IF(ch > 0xFF, PHYSFS_ERR_BAD_FILENAME, failed);
            cpptr[i] = (char) ((unsigned char) ch);
        } /* for */

        __PHYSFS_smallFree(uc2ptr);
        return cpstr;
    } /* if */
    else
    {
        int rc;
        size_t cplen = unilen * 4; /* overallocate, just in case. */
        cpptr = (char *) allocator.Malloc(cplen);
        GOTO_IF(!cpptr, PHYSFS_ERR_OUT_OF_MEMORY, failed);
        cpstr = cpptr;

        rc = pUniUconvFromUcs(uconv, &uc2str, &unilen, (void **) &cpstr, &cplen, &subs);
        GOTO_IF(rc != ULS_SUCCESS, PHYSFS_ERR_BAD_FILENAME, failed);
        GOTO_IF(subs > 0, PHYSFS_ERR_BAD_FILENAME, failed);
        assert(unilen == 0);

        __PHYSFS_smallFree(uc2ptr);
        return cpptr;
    } /* else */

failed:
    __PHYSFS_smallFree(uc2ptr);
    allocator.Free(cpptr);

    return NULL;
} /* cvtUtf8ToCodepage */

static char *cvtCodepageToUtf8(const char *cpstr)
{
    const size_t len = strlen(cpstr) + 1;
    char *retvalbuf = (char *) allocator.Malloc(len * 4);
    char *retval = NULL;

    BAIL_IF(!retvalbuf, PHYSFS_ERR_OUT_OF_MEMORY, NULL);

    if (!uconvdll)
    {
        /* There's really not much we can do on older OS/2s except pray this
           is latin1-compatible. */
        retval = retvalbuf;
        PHYSFS_utf8FromLatin1(cpstr, retval, len * 4);
    } /* if */
    else
    {
        int rc;
        size_t cplen = len;
        size_t unilen = len;
        size_t subs = 0;
        UniChar *uc2ptr = __PHYSFS_smallAlloc(len * sizeof (UniChar));
        UniChar *uc2str = uc2ptr;

        BAIL_IF(!uc2ptr, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        rc = pUniUconvToUcs(uconv, (void **) &cpstr, &cplen, &uc2str, &unilen, &subs);
        GOTO_IF(rc != ULS_SUCCESS, PHYSFS_ERR_BAD_FILENAME, done);
        GOTO_IF(subs > 0, PHYSFS_ERR_BAD_FILENAME, done);
        assert(cplen == 0);
        retval = retvalbuf;
        PHYSFS_utf8FromUcs2((const PHYSFS_uint16 *) uc2ptr, retval, len * 4);
        done:
        __PHYSFS_smallFree(uc2ptr);
    } /* else */

    return retval;
} /* cvtCodepageToUtf8 */


/* (be gentle, this function isn't very robust.) */
static char *cvtPathToCorrectCase(char *buf)
{
    char *retval = buf;
    char *fname = buf + 3;            /* point to first element. */
    char *ptr = strchr(fname, '\\');  /* find end of first element. */

    buf[0] = toupper(buf[0]);  /* capitalize drive letter. */

    /*
     * Go through each path element, and enumerate its parent dir until
     *  a case-insensitive match is found. If one is (and it SHOULD be)
     *  then overwrite the original element with the correct case.
     * If there's an error, or the path has vanished for some reason, it
     *  won't hurt to have the original case, so we just keep going.
     */
    while ((fname != NULL) && (*fname != '\0'))
    {
        char spec[CCHMAXPATH];
        FILEFINDBUF3 fb;
        HDIR hdir = HDIR_CREATE;
        ULONG count = 1;
        APIRET rc;

        *(fname - 1) = '\0';  /* isolate parent dir string. */

        strcpy(spec, buf);      /* copy isolated parent dir... */
        strcat(spec, "\\*.*");  /*  ...and add wildcard search spec. */

        if (ptr != NULL)  /* isolate element to find (fname is the start). */
            *ptr = '\0';

        rc = DosFindFirst((unsigned char *) spec, &hdir, FILE_DIRECTORY,
                          &fb, sizeof (fb), &count, FIL_STANDARD);
        if (rc == NO_ERROR)
        {
            while (count == 1)  /* while still entries to enumerate... */
            {
                int cmp;
                char *utf8 = cvtCodepageToUtf8(fb.achName);
                if (!utf8) /* ugh, maybe we'll get lucky with the C runtime. */
                    cmp = stricmp(fb.achName, fname);
                else
                {
                    cmp = PHYSFS_utf8stricmp(utf8, fname);
                    allocator.Free(utf8);
                } /* else */

                if (cmp == 0)
                {
                    strcpy(fname, fb.achName);
                    break;  /* there it is. Overwrite and stop searching. */
                } /* if */

                DosFindNext(hdir, &fb, sizeof (fb), &count);
            } /* while */
            DosFindClose(hdir);
        } /* if */

        *(fname - 1) = '\\';   /* unisolate parent dir. */
        fname = ptr;           /* point to next element. */
        if (ptr != NULL)
        {
            *ptr = '\\';       /* unisolate element. */
            ptr = strchr(++fname, '\\');  /* find next element. */
        } /* if */
    } /* while */

    return retval;
} /* cvtPathToCorrectCase */

static void prepUnicodeSupport(void)
{
    /* really old OS/2 might not have Unicode support _at all_, so load
       the system library and do without if it doesn't exist. */
    int ok = 0;
    char buf[CCHMAXPATH];
    UniChar defstr[] = { 0 };
    if (DosLoadModule(buf, sizeof (buf) - 1, "uconv", &uconvdll) == NO_ERROR)
    {
        #define LOAD(x) (DosQueryProcAddr(uconvdll,0,#x,(PFN*)&p##x)==NO_ERROR)
        ok = LOAD(UniCreateUconvObject) &&
             LOAD(UniFreeUconvObject) &&
             LOAD(UniUconvToUcs) &&
             LOAD(UniUconvFromUcs);
        #undef LOAD
    } /* else */

    if (!ok || (pUniCreateUconvObject(defstr, &uconv) != ULS_SUCCESS))
    {
        /* oh well, live without it. */
        if (uconvdll)
        {
            if (uconv)
                pUniFreeUconvObject(uconv);
            DosFreeModule(uconvdll);
            uconvdll = 0;
        } /* if */
    } /* if */
} /* prepUnicodeSupport */


int __PHYSFS_platformInit(void)
{
    prepUnicodeSupport();
    return 1;  /* ready to go! */
} /* __PHYSFS_platformInit */


void __PHYSFS_platformDeinit(void)
{
    if (uconvdll)
    {
        pUniFreeUconvObject(uconv);
        uconv = 0;
        DosFreeModule(uconvdll);
        uconvdll = 0;
    } /* if */
} /* __PHYSFS_platformDeinit */


static int discIsInserted(ULONG drive)
{
    int rc;
    char buf[20];
    DosError(FERR_DISABLEHARDERR | FERR_DISABLEEXCEPTION);
    rc = DosQueryFSInfo(drive + 1, FSIL_VOLSER, buf, sizeof (buf));
    DosError(FERR_ENABLEHARDERR | FERR_ENABLEEXCEPTION);
    return (rc == NO_ERROR);
} /* is_cdrom_inserted */


/* looks like "CD01" in ASCII (littleendian)...used for an ioctl. */
#define CD01 0x31304443

static int isCdRomDrive(ULONG drive)
{
    PHYSFS_uint32 param, data;
    ULONG ul1, ul2;
    APIRET rc;
    HFILE hfile = NULLHANDLE;
    unsigned char drivename[3] = { 0, ':', '\0' };

    drivename[0] = 'A' + drive;

    rc = DosOpen(drivename, &hfile, &ul1, 0, 0,
                 OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
                 OPEN_FLAGS_DASD | OPEN_FLAGS_FAIL_ON_ERROR |
                 OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYNONE, NULL);
    if (rc != NO_ERROR)
        return 0;

    data = 0;
    param = PHYSFS_swapULE32(CD01);
    ul1 = ul2 = sizeof (PHYSFS_uint32);
    rc = DosDevIOCtl(hfile, IOCTL_CDROMDISK, CDROMDISK_GETDRIVER,
                     &param, sizeof (param), &ul1, &data, sizeof (data), &ul2);

    DosClose(hfile);
    return ((rc == NO_ERROR) && (PHYSFS_swapULE32(data) == CD01));
} /* isCdRomDrive */


void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data)
{
    ULONG dummy = 0;
    ULONG drivemap = 0;
    ULONG i, bit;
    const APIRET rc = DosQueryCurrentDisk(&dummy, &drivemap);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc),);

    for (i = 0, bit = 1; i < 26; i++, bit <<= 1)
    {
        if (drivemap & bit)  /* this logical drive exists. */
        {
            if ((isCdRomDrive(i)) && (discIsInserted(i)))
            {
                char drive[4] = "x:\\";
                drive[0] = ('A' + i);
                cb(data, drive);
            } /* if */
        } /* if */
    } /* for */
} /* __PHYSFS_platformDetectAvailableCDs */


char *__PHYSFS_platformCalcBaseDir(const char *argv0)
{
    char *retval = NULL;
    char buf[CCHMAXPATH];
    APIRET rc;
    PTIB ptib;
    PPIB ppib;
    PHYSFS_sint32 len;

    rc = DosGetInfoBlocks(&ptib, &ppib);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), 0);
    rc = DosQueryModuleName(ppib->pib_hmte, sizeof (buf), (PCHAR) buf);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), 0);
    retval = cvtCodepageToUtf8(buf);
    BAIL_IF_ERRPASS(!retval, NULL);

    /* chop off filename, leave path. */
    for (len = strlen(retval) - 1; len >= 0; len--)
    {
        if (retval[len] == '\\')
        {
            retval[len + 1] = '\0';
            break;
        } /* if */
    } /* for */

    assert(len > 0);  /* should have been a "x:\\" on the front on string. */

    /* The string is capitalized! Figure out the REAL case... */
    return cvtPathToCorrectCase(retval);
} /* __PHYSFS_platformCalcBaseDir */

char *__PHYSFS_platformCalcUserDir(void)
{
    return __PHYSFS_platformCalcBaseDir(NULL);  /* !!! FIXME: ? */
} /* __PHYSFS_platformCalcUserDir */

char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app)
{
    return __PHYSFS_platformCalcBaseDir(NULL);  /* !!! FIXME: ? */
} /* __PHYSFS_platformCalcPrefDir */

PHYSFS_EnumerateCallbackResult __PHYSFS_platformEnumerate(const char *dirname,
                               PHYSFS_EnumerateCallback callback,
                               const char *origdir, void *callbackdata)
{
    PHYSFS_EnumerateCallbackResult retval = PHYSFS_ENUM_OK;
    size_t utf8len = strlen(dirname);
    char *utf8 = (char *) __PHYSFS_smallAlloc(utf8len + 5);
    char *cpspec = NULL;
    FILEFINDBUF3 fb;
    HDIR hdir = HDIR_CREATE;
    ULONG count = 1;
    APIRET rc;

    BAIL_IF(!utf8, PHYSFS_ERR_OUT_OF_MEMORY, PHYSFS_ENUM_ERROR);

    strcpy(utf8, dirname);
    if (utf8[utf8len - 1] != '\\')
        strcpy(utf8 + utf8len, "\\*.*");
    else
        strcpy(utf8 + utf8len, "*.*");

    cpspec = cvtUtf8ToCodepage(utf8);
    __PHYSFS_smallFree(utf8);
    BAIL_IF_ERRPASS(!cpspec, PHYSFS_ENUM_ERROR);

    rc = DosFindFirst((unsigned char *) cpspec, &hdir,
                      FILE_DIRECTORY | FILE_ARCHIVED |
                      FILE_READONLY | FILE_HIDDEN | FILE_SYSTEM,
                      &fb, sizeof (fb), &count, FIL_STANDARD);
    allocator.Free(cpspec);

    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), PHYSFS_ENUM_ERROR);

    while (count == 1)
    {
        if ((strcmp(fb.achName, ".") != 0) && (strcmp(fb.achName, "..") != 0))
        {
            utf8 = cvtCodepageToUtf8(fb.achName);
            if (!utf8)
                retval = PHYSFS_ENUM_ERROR;
            else
            {
                retval = callback(callbackdata, origdir, utf8);
                allocator.Free(utf8);
                if (retval == PHYSFS_ENUM_ERROR)
                    PHYSFS_setErrorCode(PHYSFS_ERR_APP_CALLBACK);
            } /* else */
        } /* if */

        if (retval != PHYSFS_ENUM_OK)
            break;

        DosFindNext(hdir, &fb, sizeof (fb), &count);
    } /* while */

    DosFindClose(hdir);

    return retval;
} /* __PHYSFS_platformEnumerate */


char *__PHYSFS_platformCurrentDir(void)
{
    char *retval;
    char *cpstr;
    char *utf8;
    ULONG currentDisk;
    ULONG dummy;
    ULONG pathSize = 0;
    APIRET rc;
    BYTE byte;

    rc = DosQueryCurrentDisk(&currentDisk, &dummy);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), NULL);

    /* The first call just tells us how much space we need for the string. */
    rc = DosQueryCurrentDir(currentDisk, &byte, &pathSize);
    pathSize++; /* Add space for null terminator. */
    cpstr = (char *) __PHYSFS_smallAlloc(pathSize);
    BAIL_IF(cpstr == NULL, PHYSFS_ERR_OUT_OF_MEMORY, NULL);

    /* Actually get the string this time. */
    rc = DosQueryCurrentDir(currentDisk, (PBYTE) cpstr, &pathSize);
    if (rc != NO_ERROR)
    {
        __PHYSFS_smallFree(cpstr);
        BAIL(errcodeFromAPIRET(rc), NULL);
    } /* if */

    utf8 = cvtCodepageToUtf8(cpstr);
    __PHYSFS_smallFree(cpstr);
    BAIL_IF_ERRPASS(utf8 == NULL, NULL);

    /* +4 for "x:\\" drive selector and null terminator. */
    retval = (char *) allocator.Malloc(strlen(utf8) + 4);
    if (retval == NULL)
    {
        allocator.Free(utf8);
        BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    } /* if */

    retval[0] = ('A' + (currentDisk - 1));
    retval[1] = ':';
    retval[2] = '\\';
    strcpy(retval + 3, utf8);

    allocator.Free(utf8);

    return retval;
} /* __PHYSFS_platformCurrentDir */


int __PHYSFS_platformMkDir(const char *filename)
{
    APIRET rc;
    char *cpstr = cvtUtf8ToCodepage(filename);
    BAIL_IF_ERRPASS(!cpstr, 0);
    rc = DosCreateDir((unsigned char *) cpstr, NULL);
    allocator.Free(cpstr);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), 0);
    return 1;
} /* __PHYSFS_platformMkDir */


static HFILE openFile(const char *filename, const ULONG flags, const ULONG mode)
{
    char *cpfname = cvtUtf8ToCodepage(filename);
    ULONG action = 0;
    HFILE hfile = NULLHANDLE;
    APIRET rc;

    BAIL_IF_ERRPASS(!cpfname, 0);

    rc = DosOpen(cpfname, &hfile, &action, 0, FILE_NORMAL, flags, mode, NULL);
    allocator.Free(cpfname);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), 0);

    return hfile;
} /* openFile */

void *__PHYSFS_platformOpenRead(const char *filename)
{
    /*
     * File must be opened SHARE_DENYWRITE and ACCESS_READONLY, otherwise
     *  DosQueryFileInfo() will fail if we try to get a file length, etc.
     */
    return (void *) openFile(filename,
                        OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
                        OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_NO_LOCALITY |
                        OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYWRITE |
                        OPEN_ACCESS_READONLY);
} /* __PHYSFS_platformOpenRead */


void *__PHYSFS_platformOpenWrite(const char *filename)
{
    return (void *) openFile(filename,
                        OPEN_ACTION_REPLACE_IF_EXISTS |
                        OPEN_ACTION_CREATE_IF_NEW,
                        OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_NO_LOCALITY |
                        OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYWRITE);
} /* __PHYSFS_platformOpenWrite */


void *__PHYSFS_platformOpenAppend(const char *filename)
{
    APIRET rc;
    ULONG dummy = 0;
    HFILE hfile;

    /*
     * File must be opened SHARE_DENYWRITE and ACCESS_READWRITE, otherwise
     *  DosQueryFileInfo() will fail if we try to get a file length, etc.
     */
    hfile = openFile(filename,
                        OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW,
                        OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_NO_LOCALITY |
                        OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYWRITE |
                        OPEN_ACCESS_READWRITE);
    BAIL_IF_ERRPASS(!hfile, NULL);

    rc = DosSetFilePtr(hfile, 0, FILE_END, &dummy);
    if (rc != NO_ERROR)
    {
        DosClose(hfile);
        BAIL(errcodeFromAPIRET(rc), NULL);
    } /* if */

    return ((void *) hfile);
} /* __PHYSFS_platformOpenAppend */


PHYSFS_sint64 __PHYSFS_platformRead(void *opaque, void *buf, PHYSFS_uint64 len)
{
    ULONG br = 0;
    APIRET rc;
    BAIL_IF(!__PHYSFS_ui64FitsAddressSpace(len),PHYSFS_ERR_INVALID_ARGUMENT,-1);
    rc = DosRead((HFILE) opaque, buf, (ULONG) len, &br);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), (br > 0) ? ((PHYSFS_sint64) br) : -1);
    return (PHYSFS_sint64) br;
} /* __PHYSFS_platformRead */


PHYSFS_sint64 __PHYSFS_platformWrite(void *opaque, const void *buf,
                                     PHYSFS_uint64 len)
{
    ULONG bw = 0;
    APIRET rc;
    BAIL_IF(!__PHYSFS_ui64FitsAddressSpace(len),PHYSFS_ERR_INVALID_ARGUMENT,-1);
    rc = DosWrite((HFILE) opaque, (void *) buf, (ULONG) len, &bw);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), (bw > 0) ? ((PHYSFS_sint64) bw) : -1);
    return (PHYSFS_sint64) bw;
} /* __PHYSFS_platformWrite */


int __PHYSFS_platformSeek(void *opaque, PHYSFS_uint64 pos)
{
    ULONG dummy;
    HFILE hfile = (HFILE) opaque;
    LONG dist = (LONG) pos;
    APIRET rc;

    /* hooray for 32-bit filesystem limits!  :) */
    BAIL_IF((PHYSFS_uint64) dist != pos, PHYSFS_ERR_INVALID_ARGUMENT, 0);
    rc = DosSetFilePtr(hfile, dist, FILE_BEGIN, &dummy);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), 0);
    return 1;
} /* __PHYSFS_platformSeek */


PHYSFS_sint64 __PHYSFS_platformTell(void *opaque)
{
    ULONG pos;
    HFILE hfile = (HFILE) opaque;
    const APIRET rc = DosSetFilePtr(hfile, 0, FILE_CURRENT, &pos);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), -1);
    return ((PHYSFS_sint64) pos);
} /* __PHYSFS_platformTell */


PHYSFS_sint64 __PHYSFS_platformFileLength(void *opaque)
{
    FILESTATUS3 fs;
    HFILE hfile = (HFILE) opaque;
    const APIRET rc = DosQueryFileInfo(hfile, FIL_STANDARD, &fs, sizeof (fs));
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), -1);
    return ((PHYSFS_sint64) fs.cbFile);
} /* __PHYSFS_platformFileLength */


int __PHYSFS_platformFlush(void *opaque)
{
    const APIRET rc = DosResetBuffer((HFILE) opaque);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), 0);
    return 1;
} /* __PHYSFS_platformFlush */


void __PHYSFS_platformClose(void *opaque)
{
    DosClose((HFILE) opaque);  /* ignore errors. You should have flushed! */
} /* __PHYSFS_platformClose */


int __PHYSFS_platformDelete(const char *path)
{
    char *cppath = cvtUtf8ToCodepage(path);
    FILESTATUS3 fs;
    APIRET rc;
    int retval = 0;

    BAIL_IF_ERRPASS(!cppath, 0);
    rc = DosQueryPathInfo(cppath, FIL_STANDARD, &fs, sizeof (fs));
    GOTO_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), done);
    rc = (fs.attrFile & FILE_DIRECTORY) ? DosDeleteDir(path) : DosDelete(path);
    GOTO_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), done);
    retval = 1;  /* success */

done:
    allocator.Free(cppath);
    return retval;
} /* __PHYSFS_platformDelete */


/* Convert to a format PhysicsFS can grok... */
PHYSFS_sint64 os2TimeToUnixTime(const FDATE *date, const FTIME *time)
{
    struct tm tm;

    tm.tm_sec = ((PHYSFS_uint32) time->twosecs) * 2;
    tm.tm_min = time->minutes;
    tm.tm_hour = time->hours;
    tm.tm_mday = date->day;
    tm.tm_mon = date->month;
    tm.tm_year = ((PHYSFS_uint32) date->year) + 80;
    tm.tm_wday = -1 /*st_localtz.wDayOfWeek*/;
    tm.tm_yday = -1;
    tm.tm_isdst = -1;

    return (PHYSFS_sint64) mktime(&tm);
} /* os2TimeToUnixTime */


int __PHYSFS_platformStat(const char *filename, PHYSFS_Stat *stat, const int follow)
{
    char *cpfname = cvtUtf8ToCodepage(filename);
    FILESTATUS3 fs;
    int retval = 0;
    APIRET rc;

    BAIL_IF_ERRPASS(!cpfname, 0);

    rc = DosQueryPathInfo(cpfname, FIL_STANDARD, &fs, sizeof (fs));
    GOTO_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), done);

    if (fs.attrFile & FILE_DIRECTORY)
    {
        stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
        stat->filesize = 0;
    } /* if */
    else
    {
        stat->filetype = PHYSFS_FILETYPE_REGULAR;
        stat->filesize = fs.cbFile;
    } /* else */

    stat->modtime = os2TimeToUnixTime(&fs.fdateLastWrite, &fs.ftimeLastWrite);
    if (stat->modtime < 0)
        stat->modtime = 0;

    stat->accesstime = os2TimeToUnixTime(&fs.fdateLastAccess, &fs.ftimeLastAccess);
    if (stat->accesstime < 0)
        stat->accesstime = 0;

    stat->createtime = os2TimeToUnixTime(&fs.fdateCreation, &fs.ftimeCreation);
    if (stat->createtime < 0)
        stat->createtime = 0;

    stat->readonly = ((fs.attrFile & FILE_READONLY) == FILE_READONLY);
    return 1;  /* success */

done:
    allocator.Free(cpfname);
    return retval;
} /* __PHYSFS_platformStat */


void *__PHYSFS_platformGetThreadID(void)
{
    PTIB ptib;
    PPIB ppib;

    /*
     * Allegedly, this API never fails, but we'll punt and return a
     *  default value (zero might as well do) if it does.
     */
    const APIRET rc = DosGetInfoBlocks(&ptib, &ppib);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), 0);
    return ((void *) ptib->tib_ordinal);
} /* __PHYSFS_platformGetThreadID */


void *__PHYSFS_platformCreateMutex(void)
{
    HMTX hmtx = NULLHANDLE;
    const APIRET rc = DosCreateMutexSem(NULL, &hmtx, 0, 0);
    BAIL_IF(rc != NO_ERROR, errcodeFromAPIRET(rc), NULL);
    return ((void *) hmtx);
} /* __PHYSFS_platformCreateMutex */


void __PHYSFS_platformDestroyMutex(void *mutex)
{
    DosCloseMutexSem((HMTX) mutex);
} /* __PHYSFS_platformDestroyMutex */


int __PHYSFS_platformGrabMutex(void *mutex)
{
    /* Do _NOT_ set the physfs error message in here! */
    return (DosRequestMutexSem((HMTX) mutex, SEM_INDEFINITE_WAIT) == NO_ERROR);
} /* __PHYSFS_platformGrabMutex */


void __PHYSFS_platformReleaseMutex(void *mutex)
{
    DosReleaseMutexSem((HMTX) mutex);
} /* __PHYSFS_platformReleaseMutex */

#endif  /* PHYSFS_PLATFORM_OS2 */

/* end of physfs_platform_os2.c ... */
/*
 * Posix-esque support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

/* !!! FIXME: check for EINTR? */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_platforms.h"
*/
#ifdef PHYSFS_PLATFORM_POSIX

#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

/*#include "physfs_internal.h"*/


static PHYSFS_ErrorCode errcodeFromErrnoError(const int err)
{
    switch (err)
    {
        case 0: return PHYSFS_ERR_OK;
        case EACCES: return PHYSFS_ERR_PERMISSION;
        case EPERM: return PHYSFS_ERR_PERMISSION;
        case EDQUOT: return PHYSFS_ERR_NO_SPACE;
        case EIO: return PHYSFS_ERR_IO;
        case ELOOP: return PHYSFS_ERR_SYMLINK_LOOP;
        case EMLINK: return PHYSFS_ERR_NO_SPACE;
        case ENAMETOOLONG: return PHYSFS_ERR_BAD_FILENAME;
        case ENOENT: return PHYSFS_ERR_NOT_FOUND;
        case ENOSPC: return PHYSFS_ERR_NO_SPACE;
        case ENOTDIR: return PHYSFS_ERR_NOT_FOUND;
        case EISDIR: return PHYSFS_ERR_NOT_A_FILE;
        case EROFS: return PHYSFS_ERR_READ_ONLY;
        case ETXTBSY: return PHYSFS_ERR_BUSY;
        case EBUSY: return PHYSFS_ERR_BUSY;
        case ENOMEM: return PHYSFS_ERR_OUT_OF_MEMORY;
        case ENOTEMPTY: return PHYSFS_ERR_DIR_NOT_EMPTY;
        default: return PHYSFS_ERR_OS_ERROR;
    } /* switch */
} /* errcodeFromErrnoError */


static inline PHYSFS_ErrorCode errcodeFromErrno(void)
{
    return errcodeFromErrnoError(errno);
} /* errcodeFromErrno */


static char *getUserDirByUID(void)
{
    uid_t uid = getuid();
    struct passwd *pw;
    char *retval = NULL;

    pw = getpwuid(uid);
    if ((pw != NULL) && (pw->pw_dir != NULL) && (*pw->pw_dir != '\0'))
    {
        const size_t dlen = strlen(pw->pw_dir);
        const size_t add_dirsep = (pw->pw_dir[dlen-1] != '/') ? 1 : 0;
        retval = (char *) allocator.Malloc(dlen + 1 + add_dirsep);
        if (retval != NULL)
        {
            strcpy(retval, pw->pw_dir);
            if (add_dirsep)
            {
                retval[dlen] = '/';
                retval[dlen+1] = '\0';
            } /* if */
        } /* if */
    } /* if */

    return retval;
} /* getUserDirByUID */


char *__PHYSFS_platformCalcUserDir(void)
{
    char *retval = NULL;
    char *envr = getenv("HOME");

    /* if the environment variable was set, make sure it's really a dir. */
    if (envr != NULL)
    {
        struct stat statbuf;
        if ((stat(envr, &statbuf) != -1) && (S_ISDIR(statbuf.st_mode)))
        {
            const size_t envrlen = strlen(envr);
            const size_t add_dirsep = (envr[envrlen-1] != '/') ? 1 : 0;
            retval = (char*)allocator.Malloc(envrlen + 1 + add_dirsep);
            if (retval)
            {
                strcpy(retval, envr);
                if (add_dirsep)
                {
                    retval[envrlen] = '/';
                    retval[envrlen+1] = '\0';
                } /* if */
            } /* if */
        } /* if */
    } /* if */

    if (retval == NULL)
        retval = getUserDirByUID();

    return retval;
} /* __PHYSFS_platformCalcUserDir */


PHYSFS_EnumerateCallbackResult __PHYSFS_platformEnumerate(const char *dirname,
                               PHYSFS_EnumerateCallback callback,
                               const char *origdir, void *callbackdata)
{
    DIR *dir;
    struct dirent *ent;
    PHYSFS_EnumerateCallbackResult retval = PHYSFS_ENUM_OK;

    dir = opendir(dirname);
    BAIL_IF(dir == NULL, errcodeFromErrno(), PHYSFS_ENUM_ERROR);

    while ((retval == PHYSFS_ENUM_OK) && ((ent = readdir(dir)) != NULL))
    {
        const char *name = ent->d_name;
        if (name[0] == '.')  /* ignore "." and ".." */
        {
            if ((name[1] == '\0') || ((name[1] == '.') && (name[2] == '\0')))
                continue;
        } /* if */

        retval = callback(callbackdata, origdir, name);
        if (retval == PHYSFS_ENUM_ERROR)
            PHYSFS_setErrorCode(PHYSFS_ERR_APP_CALLBACK);
    } /* while */

    closedir(dir);

    return retval;
} /* __PHYSFS_platformEnumerate */


int __PHYSFS_platformMkDir(const char *path)
{
    const int rc = mkdir(path, S_IRWXU);
    BAIL_IF(rc == -1, errcodeFromErrno(), 0);
    return 1;
} /* __PHYSFS_platformMkDir */


static void *__doOpen(const char *filename, int mode)
{
    const int appending = (mode & O_APPEND);
    int fd;
    int *retval;
    errno = 0;

    /* O_APPEND doesn't actually behave as we'd like. */
    mode &= ~O_APPEND;

    fd = open(filename, mode, S_IRUSR | S_IWUSR);
    BAIL_IF(fd < 0, errcodeFromErrno(), NULL);

    if (appending)
    {
        if (lseek(fd, 0, SEEK_END) < 0)
        {
            const int err = errno;
            close(fd);
            BAIL(errcodeFromErrnoError(err), NULL);
        } /* if */
    } /* if */

    retval = (int *) allocator.Malloc(sizeof (int));
    if (!retval)
    {
        close(fd);
        BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    } /* if */

    *retval = fd;
    return ((void *) retval);
} /* __doOpen */


void *__PHYSFS_platformOpenRead(const char *filename)
{
    return __doOpen(filename, O_RDONLY);
} /* __PHYSFS_platformOpenRead */


void *__PHYSFS_platformOpenWrite(const char *filename)
{
    return __doOpen(filename, O_WRONLY | O_CREAT | O_TRUNC);
} /* __PHYSFS_platformOpenWrite */


void *__PHYSFS_platformOpenAppend(const char *filename)
{
    return __doOpen(filename, O_WRONLY | O_CREAT | O_APPEND);
} /* __PHYSFS_platformOpenAppend */


PHYSFS_sint64 __PHYSFS_platformRead(void *opaque, void *buffer,
                                    PHYSFS_uint64 len)
{
    const int fd = *((int *) opaque);
    ssize_t rc = 0;

    if (!__PHYSFS_ui64FitsAddressSpace(len))
        BAIL(PHYSFS_ERR_INVALID_ARGUMENT, -1);

    rc = read(fd, buffer, (size_t) len);
    BAIL_IF(rc == -1, errcodeFromErrno(), -1);
    assert(rc >= 0);
    assert((PHYSFS_uint64)rc <= len);
    return (PHYSFS_sint64) rc;
} /* __PHYSFS_platformRead */


PHYSFS_sint64 __PHYSFS_platformWrite(void *opaque, const void *buffer,
                                     PHYSFS_uint64 len)
{
    const int fd = *((int *) opaque);
    ssize_t rc = 0;

    if (!__PHYSFS_ui64FitsAddressSpace(len))
        BAIL(PHYSFS_ERR_INVALID_ARGUMENT, -1);

    rc = write(fd, (void *) buffer, (size_t) len);
    BAIL_IF(rc == -1, errcodeFromErrno(), rc);
    assert(rc >= 0);
    assert((PHYSFS_uint64)rc <= len);
    return (PHYSFS_sint64) rc;
} /* __PHYSFS_platformWrite */


int __PHYSFS_platformSeek(void *opaque, PHYSFS_uint64 pos)
{
    const int fd = *((int *) opaque);
    const off_t rc = lseek(fd, (off_t) pos, SEEK_SET);
    BAIL_IF(rc == -1, errcodeFromErrno(), 0);
    return 1;
} /* __PHYSFS_platformSeek */


PHYSFS_sint64 __PHYSFS_platformTell(void *opaque)
{
    const int fd = *((int *) opaque);
    PHYSFS_sint64 retval;
    retval = (PHYSFS_sint64) lseek(fd, 0, SEEK_CUR);
    BAIL_IF(retval == -1, errcodeFromErrno(), -1);
    return retval;
} /* __PHYSFS_platformTell */


PHYSFS_sint64 __PHYSFS_platformFileLength(void *opaque)
{
    const int fd = *((int *) opaque);
    struct stat statbuf;
    BAIL_IF(fstat(fd, &statbuf) == -1, errcodeFromErrno(), -1);
    return ((PHYSFS_sint64) statbuf.st_size);
} /* __PHYSFS_platformFileLength */


int __PHYSFS_platformFlush(void *opaque)
{
    const int fd = *((int *) opaque);
    if ((fcntl(fd, F_GETFL) & O_ACCMODE) != O_RDONLY)
        BAIL_IF(fsync(fd) == -1, errcodeFromErrno(), 0);
    return 1;
} /* __PHYSFS_platformFlush */


void __PHYSFS_platformClose(void *opaque)
{
    const int fd = *((int *) opaque);
    (void) close(fd);  /* we don't check this. You should have used flush! */
    allocator.Free(opaque);
} /* __PHYSFS_platformClose */


int __PHYSFS_platformDelete(const char *path)
{
    BAIL_IF(remove(path) == -1, errcodeFromErrno(), 0);
    return 1;
} /* __PHYSFS_platformDelete */


int __PHYSFS_platformStat(const char *fname, PHYSFS_Stat *st, const int follow)
{
    struct stat statbuf;
    const int rc = follow ? stat(fname, &statbuf) : lstat(fname, &statbuf);
    BAIL_IF(rc == -1, errcodeFromErrno(), 0);

    if (S_ISREG(statbuf.st_mode))
    {
        st->filetype = PHYSFS_FILETYPE_REGULAR;
        st->filesize = statbuf.st_size;
    } /* if */

    else if(S_ISDIR(statbuf.st_mode))
    {
        st->filetype = PHYSFS_FILETYPE_DIRECTORY;
        st->filesize = 0;
    } /* else if */

    else if(S_ISLNK(statbuf.st_mode))
    {
        st->filetype = PHYSFS_FILETYPE_SYMLINK;
        st->filesize = 0;
    } /* else if */

    else
    {
        st->filetype = PHYSFS_FILETYPE_OTHER;
        st->filesize = statbuf.st_size;
    } /* else */

    st->modtime = statbuf.st_mtime;
    st->createtime = statbuf.st_ctime;
    st->accesstime = statbuf.st_atime;

    st->readonly = (access(fname, W_OK) == -1);
    return 1;
} /* __PHYSFS_platformStat */


typedef struct
{
    pthread_mutex_t mutex;
    pthread_t owner;
    PHYSFS_uint32 count;
} PthreadMutex;


void *__PHYSFS_platformGetThreadID(void)
{
    return ( (void *) ((size_t) pthread_self()) );
} /* __PHYSFS_platformGetThreadID */


void *__PHYSFS_platformCreateMutex(void)
{
    int rc;
    PthreadMutex *m = (PthreadMutex *) allocator.Malloc(sizeof (PthreadMutex));
    BAIL_IF(!m, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    rc = pthread_mutex_init(&m->mutex, NULL);
    if (rc != 0)
    {
        allocator.Free(m);
        BAIL(PHYSFS_ERR_OS_ERROR, NULL);
    } /* if */

    m->count = 0;
    m->owner = (pthread_t) 0xDEADBEEF;
    return ((void *) m);
} /* __PHYSFS_platformCreateMutex */


void __PHYSFS_platformDestroyMutex(void *mutex)
{
    PthreadMutex *m = (PthreadMutex *) mutex;

    /* Destroying a locked mutex is a bug, but we'll try to be helpful. */
    if ((m->owner == pthread_self()) && (m->count > 0))
        pthread_mutex_unlock(&m->mutex);

    pthread_mutex_destroy(&m->mutex);
    allocator.Free(m);
} /* __PHYSFS_platformDestroyMutex */


int __PHYSFS_platformGrabMutex(void *mutex)
{
    PthreadMutex *m = (PthreadMutex *) mutex;
    pthread_t tid = pthread_self();
    if (m->owner != tid)
    {
        if (pthread_mutex_lock(&m->mutex) != 0)
            return 0;
        m->owner = tid;
    } /* if */

    m->count++;
    return 1;
} /* __PHYSFS_platformGrabMutex */


void __PHYSFS_platformReleaseMutex(void *mutex)
{
    PthreadMutex *m = (PthreadMutex *) mutex;
    assert(m->owner == pthread_self());  /* catch programming errors. */
    assert(m->count > 0);  /* catch programming errors. */
    if (m->owner == pthread_self())
    {
        if (--m->count == 0)
        {
            m->owner = (pthread_t) 0xDEADBEEF;
            pthread_mutex_unlock(&m->mutex);
        } /* if */
    } /* if */
} /* __PHYSFS_platformReleaseMutex */

#endif  /* PHYSFS_PLATFORM_POSIX */

/* end of physfs_platform_posix.c ... */

/*
 * QNX support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

/* This is tested against QNX 7 at the moment. */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_platforms.h"
*/
#ifdef PHYSFS_PLATFORM_QNX

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

/*#include "physfs_internal.h"*/

int __PHYSFS_platformInit(void)
{
    return 1;  /* always succeed. */
} /* __PHYSFS_platformInit */


void __PHYSFS_platformDeinit(void)
{
    /* no-op */
} /* __PHYSFS_platformDeinit */


char *__PHYSFS_platformCalcBaseDir(const char *argv0)
{
    char *retval = (char *) allocator.Malloc(PATH_MAX+1);
    if (retval == NULL)
        BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    else
    {
        const int fd = open("/proc/self/exefile", O_RDONLY);
        const ssize_t br = (fd == -1) ? -1 : read(fd, retval, PATH_MAX);
        char *ptr;

        if (fd != -1)
            close(fd);

        if ((br < 0) || (br > PATH_MAX))
        {
            allocator.Free(retval);
            BAIL(PHYSFS_ERR_OS_ERROR, NULL);
        } /* if */

        retval[br] = '\0';
        ptr = strrchr(retval, '/');
        if (ptr == NULL)  /* uhoh! */
        {
            allocator.Free(retval);
            BAIL(PHYSFS_ERR_OS_ERROR, NULL);
        } /* if */

        ptr[1] = '\0';  /* chop off filename, leave dirs and '/' */

        ptr = (char *) allocator.Realloc(retval, (ptr - retval) + 2);
        if (ptr != NULL)  /* just shrinking buffer; don't care if it failed. */
            retval = ptr;
    } /* else */

    return retval;
} /* __PHYSFS_platformCalcBaseDir */


char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app)
{
    /* !!! FIXME: this might be wrong; I don't know if there's a better method
        on QNX, or if it follows XDG specs, etc. */
    char *retval = NULL;
    const char *home = __PHYSFS_getUserDir();
    if (home)
    {
        const size_t len = strlen(home) + strlen(app) + 3;
        retval = (char *) allocator.Malloc(len);
        BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        snprintf(retval, len, "%s.%s/", home, app);
    } /* if */
    return retval;
} /* __PHYSFS_platformCalcPrefDir */


#if !PHYSFS_NO_CDROM_SUPPORT
#include <devctl.h>
#include <sys/dcmd_blk.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

static void checkPathForCD(const char *path, PHYSFS_StringCallback cb, void *d)
{
    struct stat statbuf;
    int fd;

    /* The devctl() thing is QNX-specific. In this case, we query what is
       probably the mountpoint for the device. statvfs() on that mountpoint
       will tell use its filesystem type. */

    if ( (stat(path, &statbuf) == 0) &&
         (S_ISBLK(statbuf.st_mode)) &&
         ((fd = open(path, O_RDONLY | O_NONBLOCK)) != -1) )
    {
        char mnt[256] = { 0 };
        const int rc = devctl(fd, DCMD_FSYS_MOUNTED_BY, mnt, sizeof (mnt), 0);
        close(fd);
        if ( (rc == EOK) && (mnt[0]) )
        {
            struct statvfs statvfsbuf;
            if (statvfs(mnt, &statvfsbuf) == 0)
            {
                /* I don't know if this is a complete or accurate list. */
                const char *fstype = statvfsbuf.f_basetype;
                const int iscd = ( (strcmp(fstype, "cd") == 0) ||
                                   (strcmp(fstype, "udf") == 0) );
                if (iscd)
                    cb(d, mnt);
            } /* if */
        } /* if */
    } /* if */
} /* checkPathForCD */

static void checkDevForCD(const char *dev, PHYSFS_StringCallback cb, void *d)
{
    size_t len;
    char *path;

    if (dev[0] == '.')  /* ignore "." and ".." */
    {
        if ((dev[1] == '\0') || ((dev[1] == '.') && (dev[2] == '\0')))
            return;
    } /* if */

    len = strlen(dev) + 6;
    path = (char *) __PHYSFS_smallAlloc(len);
    if (!path)
        return;  /* oh well. */

    snprintf(path, len, "/dev/%s", dev);
    checkPathForCD(path, cb, d);
    __PHYSFS_smallFree(path);
} /* checkDevForCD */
#endif /* !PHYSFS_NO_CDROM_SUPPORT */

void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data)
{
#if !PHYSFS_NO_CDROM_SUPPORT
    DIR *dirp = opendir("/dev");
    if (dirp)
    {
        struct dirent *dent;
        while ((dent = readdir(dirp)) != NULL)
            checkDevForCD(dent->d_name, cb, data);
        closedir(dirp);
    } /* if */
#endif
} /* __PHYSFS_platformDetectAvailableCDs */

#endif /* PHYSFS_PLATFORM_QNX */

/* end of physfs_platform_qnx.c ... */

/*
 * Unix support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_platforms.h"
*/
#ifdef PHYSFS_PLATFORM_UNIX

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#if PHYSFS_NO_CDROM_SUPPORT
#elif PHYSFS_PLATFORM_LINUX
#  define PHYSFS_HAVE_MNTENT_H 1
#elif defined __CYGWIN__
#  define PHYSFS_HAVE_MNTENT_H 1
#elif PHYSFS_PLATFORM_SOLARIS
#  define PHYSFS_HAVE_SYS_MNTTAB_H 1
#elif PHYSFS_PLATFORM_BSD
#  define PHYSFS_HAVE_SYS_UCRED_H 1
#else
#  warning No CD-ROM support included. Either define your platform here,
#  warning  or define PHYSFS_NO_CDROM_SUPPORT=1 to confirm this is intentional.
#endif

#ifdef PHYSFS_HAVE_SYS_UCRED_H
#  ifdef PHYSFS_HAVE_MNTENT_H
#    undef PHYSFS_HAVE_MNTENT_H /* don't do both... */
#  endif
#  include <sys/mount.h>
#  include <sys/ucred.h>
#endif

#ifdef PHYSFS_HAVE_MNTENT_H
#include <mntent.h>
#endif

#ifdef PHYSFS_HAVE_SYS_MNTTAB_H
#include <sys/mnttab.h>
#endif

#ifdef PHYSFS_PLATFORM_FREEBSD
#include <sys/sysctl.h>
#endif


/*#include "physfs_internal.h"*/

int __PHYSFS_platformInit(void)
{
    return 1;  /* always succeed. */
} /* __PHYSFS_platformInit */


void __PHYSFS_platformDeinit(void)
{
    /* no-op */
} /* __PHYSFS_platformDeinit */


void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data)
{
#if (defined PHYSFS_NO_CDROM_SUPPORT)
    /* no-op. */

#elif (defined PHYSFS_HAVE_SYS_UCRED_H)
    int i;
    struct statfs *mntbufp = NULL;
    int mounts = getmntinfo(&mntbufp, MNT_NOWAIT);

    for (i = 0; i < mounts; i++)
    {
        int add_it = 0;

        if (strcmp(mntbufp[i].f_fstypename, "iso9660") == 0)
            add_it = 1;
        else if (strcmp( mntbufp[i].f_fstypename, "cd9660") == 0)
            add_it = 1;

        /* add other mount types here */

        if (add_it)
            cb(data, mntbufp[i].f_mntonname);
    } /* for */

#elif (defined PHYSFS_HAVE_MNTENT_H)
    FILE *mounts = NULL;
    struct mntent *ent = NULL;

    mounts = setmntent("/etc/mtab", "r");
    BAIL_IF(mounts == NULL, PHYSFS_ERR_IO, /*return void*/);

    while ( (ent = getmntent(mounts)) != NULL )
    {
        int add_it = 0;
        if (strcmp(ent->mnt_type, "iso9660") == 0)
            add_it = 1;
        else if (strcmp(ent->mnt_type, "udf") == 0)
            add_it = 1;

        /* !!! FIXME: these might pick up floppy drives, right? */
        else if (strcmp(ent->mnt_type, "auto") == 0)
            add_it = 1;
        else if (strcmp(ent->mnt_type, "supermount") == 0)
            add_it = 1;

        /* add other mount types here */

        if (add_it)
            cb(data, ent->mnt_dir);
    } /* while */

    endmntent(mounts);

#elif (defined PHYSFS_HAVE_SYS_MNTTAB_H)
    FILE *mounts = fopen(MNTTAB, "r");
    struct mnttab ent;

    BAIL_IF(mounts == NULL, PHYSFS_ERR_IO, /*return void*/);
    while (getmntent(mounts, &ent) == 0)
    {
        int add_it = 0;
        if (strcmp(ent.mnt_fstype, "hsfs") == 0)
            add_it = 1;

        /* add other mount types here */

        if (add_it)
            cb(data, ent.mnt_mountp);
    } /* while */

    fclose(mounts);
#endif
} /* __PHYSFS_platformDetectAvailableCDs */


/*
 * See where program (bin) resides in the $PATH specified by (envr).
 *  returns a copy of the first element in envr that contains it, or NULL
 *  if it doesn't exist or there were other problems. PHYSFS_SetError() is
 *  called if we have a problem.
 *
 * (envr) will be scribbled over, and you are expected to allocator.Free() the
 *  return value when you're done with it.
 */
static char *findBinaryInPath(const char *bin, char *envr)
{
    size_t alloc_size = 0;
    char *exe = NULL;
    char *start = envr;
    char *ptr;

    assert(bin != NULL);
    assert(envr != NULL);

    do
    {
        size_t size;
        size_t binlen;

        ptr = strchr(start, ':');  /* find next $PATH separator. */
        if (ptr)
            *ptr = '\0';

        binlen = strlen(bin);
        size = strlen(start) + binlen + 2;
        if (size >= alloc_size)
        {
            char *x = (char *) allocator.Realloc(exe, size);
            if (!x)
            {
                if (exe != NULL)
                    allocator.Free(exe);
                BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
            } /* if */

            alloc_size = size;
            exe = x;
        } /* if */

        /* build full binary path... */
        strcpy(exe, start);
        if ((exe[0] == '\0') || (exe[strlen(exe) - 1] != '/'))
            strcat(exe, "/");
        strcat(exe, bin);

        if (access(exe, X_OK) == 0)  /* Exists as executable? We're done. */
        {
            exe[(size - binlen) - 1] = '\0'; /* chop off filename, leave '/' */
            return exe;
        } /* if */

        start = ptr + 1;  /* start points to beginning of next element. */
    } while (ptr != NULL);

    if (exe != NULL)
        allocator.Free(exe);

    return NULL;  /* doesn't exist in path. */
} /* findBinaryInPath */


static char *readSymLink(const char *path)
{
    ssize_t len = 64;
    ssize_t rc = -1;
    char *retval = NULL;

    while (1)
    {
         char *ptr = (char *) allocator.Realloc(retval, (size_t) len);
         if (ptr == NULL)
             break;   /* out of memory. */
         retval = ptr;

         rc = readlink(path, retval, len);
         if (rc == -1)
             break;  /* not a symlink, i/o error, etc. */

         else if (rc < len)
         {
             retval[rc] = '\0';  /* readlink doesn't null-terminate. */
             return retval;  /* we're good to go. */
         } /* else if */

         len *= 2;  /* grow buffer, try again. */
    } /* while */

    if (retval != NULL)
        allocator.Free(retval);
    return NULL;
} /* readSymLink */


char *__PHYSFS_platformCalcBaseDir(const char *argv0)
{
    char *retval = NULL;
    const char *envr = NULL;

    /* Try to avoid using argv0 unless forced to. Try system-specific stuff. */

    #if defined(PHYSFS_PLATFORM_FREEBSD)
    {
        char fullpath[PATH_MAX];
        size_t buflen = sizeof (fullpath);
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
        if (sysctl(mib, 4, fullpath, &buflen, NULL, 0) != -1)
            retval = __PHYSFS_strdup(fullpath);
    }
    #elif defined(PHYSFS_PLATFORM_SOLARIS)
    {
        const char *path = getexecname();
        if ((path != NULL) && (path[0] == '/'))  /* must be absolute path... */
            retval = __PHYSFS_strdup(path);
    }
    #endif

    /* If there's a Linux-like /proc filesystem, you can get the full path to
     *  the current process from a symlink in there.
     */

    if (!retval && (access("/proc", F_OK) == 0))
    {
        retval = readSymLink("/proc/self/exe");
        if (!retval) retval = readSymLink("/proc/curproc/file");
        if (!retval) retval = readSymLink("/proc/curproc/exe");
        if (retval == NULL)
        {
            /* older kernels don't have /proc/self ... try PID version... */
            const unsigned long long pid = (unsigned long long) getpid();
            char path[64];
            const int rc = (int) snprintf(path,sizeof(path),"/proc/%llu/exe",pid);
            if ( (rc > 0) && (rc < (int)sizeof(path)) )
                retval = readSymLink(path);
        } /* if */
    } /* if */

    if (retval != NULL)  /* chop off filename. */
    {
        char *ptr = strrchr(retval, '/');
        if (ptr != NULL)
            *(ptr+1) = '\0';
        else  /* shouldn't happen, but just in case... */
        {
            allocator.Free(retval);
            retval = NULL;
        } /* else */
    } /* if */

    /* No /proc/self/exe, etc, but we have an argv[0] we can parse? */
    if ((retval == NULL) && (argv0 != NULL))
    {
        /* fast path: default behaviour can handle this. */
        if (strchr(argv0, '/') != NULL)
            return NULL;  /* higher level parses out real path from argv0. */

        /* If there's no dirsep on argv0, then look through $PATH for it. */
        envr = getenv("PATH");
        if (envr != NULL)
        {
            char *path = (char *) __PHYSFS_smallAlloc(strlen(envr) + 1);
            BAIL_IF(!path, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
            strcpy(path, envr);
            retval = findBinaryInPath(argv0, path);
            __PHYSFS_smallFree(path);
        } /* if */
    } /* if */

    if (retval != NULL)
    {
        /* try to shrink buffer... */
        char *ptr = (char *) allocator.Realloc(retval, strlen(retval) + 1);
        if (ptr != NULL)
            retval = ptr;  /* oh well if it failed. */
    } /* if */

    return retval;
} /* __PHYSFS_platformCalcBaseDir */


char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app)
{
    /*
     * We use XDG's base directory spec, even if you're not on Linux.
     *  This isn't strictly correct, but the results are relatively sane
     *  in any case.
     *
     * https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
     */
    const char *envr = getenv("XDG_DATA_HOME");
    const char *append = "/";
    char *retval = NULL;
    size_t len = 0;

    if (!envr)
    {
        /* You end up with "$HOME/.local/share/Game Name 2" */
        envr = __PHYSFS_getUserDir();
        BAIL_IF_ERRPASS(!envr, NULL);  /* oh well. */
        append = ".local/share/";
    } /* if */

    len = strlen(envr) + strlen(append) + strlen(app) + 2;
    retval = (char *) allocator.Malloc(len);
    BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    snprintf(retval, len, "%s%s%s/", envr, append, app);
    return retval;
} /* __PHYSFS_platformCalcPrefDir */

#endif /* PHYSFS_PLATFORM_UNIX */

/* end of physfs_platform_unix.c ... */

/*
 * Windows support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon, and made sane by Gregory S. Read.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_platforms.h"
*/
#ifdef PHYSFS_PLATFORM_WINDOWS

#undef allocator

/* Forcibly disable UNICODE macro, since we manage this ourselves. */
#ifdef UNICODE
#undef UNICODE
#endif

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#ifndef PHYSFS_PLATFORM_WINRT
#include <userenv.h>
#include <shlobj.h>
#endif

#if !defined(PHYSFS_NO_CDROM_SUPPORT)
#include <dbt.h>
#endif

#include <errno.h>
#include <ctype.h>
#include <time.h>

#ifdef allocator  /* apparently Windows 10 SDK conflicts here. */
#undef allocator
#endif

/*#include "physfs_internal.h"*/

/*
 * Users without the platform SDK don't have this defined.  The original docs
 *  for SetFilePointer() just said to compare with 0xFFFFFFFF, so this should
 *  work as desired.
 */
#define PHYSFS_INVALID_SET_FILE_POINTER  0xFFFFFFFF

/* just in case... */
#define PHYSFS_INVALID_FILE_ATTRIBUTES   0xFFFFFFFF

/* Not defined before the Vista SDK. */
#define PHYSFS_FILE_ATTRIBUTE_REPARSE_POINT 0x400
#define PHYSFS_IO_REPARSE_TAG_SYMLINK    0xA000000C


#define UTF8_TO_UNICODE_STACK(w_assignto, str) { \
    if (str == NULL) \
        w_assignto = NULL; \
    else { \
        const size_t len = (PHYSFS_uint64) ((strlen(str) + 1) * 2); \
        w_assignto = (WCHAR *) __PHYSFS_smallAlloc(len); \
        if (w_assignto != NULL) \
            PHYSFS_utf8ToUtf16(str, (PHYSFS_uint16 *) w_assignto, len); \
    } \
} \

/* Note this counts WCHARs, not codepoints! */
static PHYSFS_uint64 wStrLen(const WCHAR *wstr)
{
    PHYSFS_uint64 len = 0;
    while (*(wstr++))
        len++;
    return len;
} /* wStrLen */

static char *unicodeToUtf8Heap(const WCHAR *w_str)
{
    char *retval = NULL;
    if (w_str != NULL)
    {
        void *ptr = NULL;
        const PHYSFS_uint64 len = (wStrLen(w_str) * 4) + 1;
        retval = (char*)physfs_allocator.Malloc(len);
        BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        PHYSFS_utf8FromUtf16((const PHYSFS_uint16 *) w_str, retval, len);
        ptr = physfs_allocator.Realloc(retval, strlen(retval) + 1); /* shrink. */
        if (ptr != NULL)
            retval = (char *) ptr;
    } /* if */
    return retval;
} /* unicodeToUtf8Heap */


/* Some older APIs aren't in WinRT (only the "Ex" version, etc).
   Since non-WinRT might not have the "Ex" version, we tapdance to use
   the perfectly-fine-and-available-even-on-Win95 API on non-WinRT targets. */

static inline HANDLE winFindFirstFileW(const WCHAR *path, LPWIN32_FIND_DATAW d)
{
    #ifdef PHYSFS_PLATFORM_WINRT
    return FindFirstFileExW(path, FindExInfoStandard, d,
                            FindExSearchNameMatch, NULL, 0);
    #else
    return FindFirstFileW(path, d);
    #endif
} /* winFindFirstFileW */

static inline BOOL winInitializeCriticalSection(LPCRITICAL_SECTION lpcs)
{
    #ifdef PHYSFS_PLATFORM_WINRT
    return InitializeCriticalSectionEx(lpcs, 2000, 0);
    #else
    InitializeCriticalSection(lpcs);
    return TRUE;
    #endif
} /* winInitializeCriticalSection */

static inline HANDLE winCreateFileW(const WCHAR *wfname, const DWORD mode,
                                    const DWORD creation)
{
    const DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
    #ifdef PHYSFS_PLATFORM_WINRT
    return CreateFile2(wfname, mode, share, creation, NULL);
    #else
    return CreateFileW(wfname, mode, share, NULL, creation,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    #endif
} /* winCreateFileW */

static BOOL winSetFilePointer(HANDLE h, const PHYSFS_sint64 pos,
                              PHYSFS_sint64 *_newpos, const DWORD whence)
{
    #ifdef PHYSFS_PLATFORM_WINRT
    LARGE_INTEGER lipos;
    LARGE_INTEGER linewpos;
    BOOL rc;
    lipos.QuadPart = (LONGLONG) pos;
    rc = SetFilePointerEx(h, lipos, &linewpos, whence);
    if (_newpos)
        *_newpos = (PHYSFS_sint64) linewpos.QuadPart;
    return rc;
    #else
    const LONG low = (LONG) (pos & 0xFFFFFFFF);
    LONG high = (LONG) ((pos >> 32) & 0xFFFFFFFF);
    const DWORD rc = SetFilePointer(h, low, &high, whence);
    /* 0xFFFFFFFF could be valid, so you have to check GetLastError too! */
    if (_newpos)
        *_newpos = ((PHYSFS_sint64) rc) | (((PHYSFS_sint64) high) << 32);
    if ((rc == PHYSFS_INVALID_SET_FILE_POINTER) && (GetLastError() != NO_ERROR))
        return FALSE;
    return TRUE;
    #endif
} /* winSetFilePointer */

static PHYSFS_sint64 winGetFileSize(HANDLE h)
{
    #ifdef PHYSFS_PLATFORM_WINRT
    FILE_STANDARD_INFO info;
    const BOOL rc = GetFileInformationByHandleEx(h, FileStandardInfo,
                                                 &info, sizeof (info));
    return rc ? (PHYSFS_sint64) info.EndOfFile.QuadPart : -1;
    #else
    DWORD high = 0;
    const DWORD rc = GetFileSize(h, &high);
    if ((rc == PHYSFS_INVALID_SET_FILE_POINTER) && (GetLastError() != NO_ERROR))
        return -1;
    return (PHYSFS_sint64) ((((PHYSFS_uint64) high) << 32) | rc);
    #endif
} /* winGetFileSize */


static PHYSFS_ErrorCode errcodeFromWinApiError(const DWORD err)
{
    /*
     * win32 error codes are sort of a tricky thing; Microsoft intentionally
     *  doesn't list which ones a given API might trigger, there are several
     *  with overlapping and unclear meanings...and there's 16 thousand of
     *  them in Windows 7. It looks like the ones we care about are in the
     *  first 500, but I can't say this list is perfect; we might miss
     *  important values or misinterpret others.
     *
     * Don't treat this list as anything other than a work in progress.
     */
    switch (err)
    {
        case ERROR_SUCCESS: return PHYSFS_ERR_OK;
        case ERROR_ACCESS_DENIED: return PHYSFS_ERR_PERMISSION;
        case ERROR_NETWORK_ACCESS_DENIED: return PHYSFS_ERR_PERMISSION;
        case ERROR_NOT_READY: return PHYSFS_ERR_IO;
        case ERROR_CRC: return PHYSFS_ERR_IO;
        case ERROR_SEEK: return PHYSFS_ERR_IO;
        case ERROR_SECTOR_NOT_FOUND: return PHYSFS_ERR_IO;
        case ERROR_NOT_DOS_DISK: return PHYSFS_ERR_IO;
        case ERROR_WRITE_FAULT: return PHYSFS_ERR_IO;
        case ERROR_READ_FAULT: return PHYSFS_ERR_IO;
        case ERROR_DEV_NOT_EXIST: return PHYSFS_ERR_IO;
        case ERROR_BUFFER_OVERFLOW: return PHYSFS_ERR_BAD_FILENAME;
        case ERROR_INVALID_NAME: return PHYSFS_ERR_BAD_FILENAME;
        case ERROR_BAD_PATHNAME: return PHYSFS_ERR_BAD_FILENAME;
        case ERROR_DIRECTORY: return PHYSFS_ERR_BAD_FILENAME;
        case ERROR_FILE_NOT_FOUND: return PHYSFS_ERR_NOT_FOUND;
        case ERROR_PATH_NOT_FOUND: return PHYSFS_ERR_NOT_FOUND;
        case ERROR_DELETE_PENDING: return PHYSFS_ERR_NOT_FOUND;
        case ERROR_INVALID_DRIVE: return PHYSFS_ERR_NOT_FOUND;
        case ERROR_HANDLE_DISK_FULL: return PHYSFS_ERR_NO_SPACE;
        case ERROR_DISK_FULL: return PHYSFS_ERR_NO_SPACE;
        case ERROR_WRITE_PROTECT: return PHYSFS_ERR_READ_ONLY;
        case ERROR_LOCK_VIOLATION: return PHYSFS_ERR_BUSY;
        case ERROR_SHARING_VIOLATION: return PHYSFS_ERR_BUSY;
        case ERROR_CURRENT_DIRECTORY: return PHYSFS_ERR_BUSY;
        case ERROR_DRIVE_LOCKED: return PHYSFS_ERR_BUSY;
        case ERROR_PATH_BUSY: return PHYSFS_ERR_BUSY;
        case ERROR_BUSY: return PHYSFS_ERR_BUSY;
        case ERROR_NOT_ENOUGH_MEMORY: return PHYSFS_ERR_OUT_OF_MEMORY;
        case ERROR_OUTOFMEMORY: return PHYSFS_ERR_OUT_OF_MEMORY;
        case ERROR_DIR_NOT_EMPTY: return PHYSFS_ERR_DIR_NOT_EMPTY;
        default: return PHYSFS_ERR_OS_ERROR;
    } /* switch */
} /* errcodeFromWinApiError */

static inline PHYSFS_ErrorCode errcodeFromWinApi(void)
{
    return errcodeFromWinApiError(GetLastError());
} /* errcodeFromWinApi */


#if defined(PHYSFS_NO_CDROM_SUPPORT)
#define detectAvailableCDs(cb, data)
#define deinitCDThread()
#else
static HANDLE detectCDThreadHandle = NULL;
static HWND detectCDHwnd = NULL;
static volatile DWORD drivesWithMediaBitmap = 0;

typedef BOOL (WINAPI *fnSTEM)(DWORD, LPDWORD b);

static DWORD pollDiscDrives(void)
{
    /* Try to use SetThreadErrorMode(), which showed up in Windows 7. */
    HMODULE lib = LoadLibraryA("kernel32.dll");
    fnSTEM stem = NULL;
    char drive[4] = { 'x', ':', '\\', '\0' };
    DWORD oldErrorMode = 0;
    DWORD drives = 0;
    DWORD i;

    if (lib)
        stem = (fnSTEM) GetProcAddress(lib, "SetThreadErrorMode");

    if (stem)
        stem(SEM_FAILCRITICALERRORS, &oldErrorMode);
    else
        oldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);

    /* Do detection. This may block if a disc is spinning up. */
    for (i = 'A'; i <= 'Z'; i++)
    {
        DWORD tmp = 0;
        drive[0] = (char) i;
        if (GetDriveTypeA(drive) != DRIVE_CDROM)
            continue;

        /* If this function succeeds, there's media in the drive */
        if (GetVolumeInformationA(drive, NULL, 0, NULL, NULL, &tmp, NULL, 0))
            drives |= (1 << (i - 'A'));
    } /* for */

    if (stem)
        stem(oldErrorMode, NULL);
    else
        SetErrorMode(oldErrorMode);

    if (lib)
        FreeLibrary(lib);

    return drives;
} /* pollDiscDrives */


static LRESULT CALLBACK detectCDWndProc(HWND hwnd, UINT msg,
                                        WPARAM wp, LPARAM lparam)
{
    PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR) lparam;
    PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME) lparam;
    const int removed = (wp == DBT_DEVICEREMOVECOMPLETE);

    if (msg == WM_DESTROY)
        return 0;
    else if ((msg != WM_DEVICECHANGE) ||
             ((wp != DBT_DEVICEARRIVAL) && (wp != DBT_DEVICEREMOVECOMPLETE)) ||
             (lpdb->dbch_devicetype != DBT_DEVTYP_VOLUME) ||
             ((lpdbv->dbcv_flags & DBTF_MEDIA) == 0))
    {
        return DefWindowProcW(hwnd, msg, wp, lparam);
    } /* else if */

    if (removed)
        drivesWithMediaBitmap &= ~lpdbv->dbcv_unitmask;
    else
        drivesWithMediaBitmap |= lpdbv->dbcv_unitmask;

    return TRUE;
} /* detectCDWndProc */


static DWORD WINAPI detectCDThread(LPVOID arg)
{
    HANDLE initialDiscDetectionComplete = *((HANDLE *) arg);
    const char *classname = "PhysicsFSDetectCDCatcher";
    const char *winname = "PhysicsFSDetectCDMsgWindow";
    HINSTANCE hInstance = GetModuleHandleW(NULL);
    ATOM class_atom = 0;
    WNDCLASSEXA wce;
    MSG msg;

    memset(&wce, '\0', sizeof (wce));
    wce.cbSize = sizeof (wce);
    wce.lpfnWndProc = detectCDWndProc;
    wce.lpszClassName = classname;
    wce.hInstance = hInstance;
    class_atom = RegisterClassExA(&wce);
    if (class_atom == 0)
    {
        SetEvent(initialDiscDetectionComplete);  /* let main thread go on. */
        return 0;
    } /* if */

    detectCDHwnd = CreateWindowExA(0, classname, winname, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, HWND_DESKTOP, NULL, hInstance, NULL);

    if (detectCDHwnd == NULL)
    {
        SetEvent(initialDiscDetectionComplete);  /* let main thread go on. */
        UnregisterClassA(classname, hInstance);
        return 0;
    } /* if */

    /* We'll get events when discs come and go from now on. */

    /* Do initial detection, possibly blocking awhile... */
    drivesWithMediaBitmap = pollDiscDrives();

    SetEvent(initialDiscDetectionComplete);  /* let main thread go on. */

    do
    {
        const BOOL rc = GetMessageW(&msg, detectCDHwnd, 0, 0);
        if ((rc == 0) || (rc == -1))
            break;  /* don't care if WM_QUIT or error break this loop. */
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    } while (1);

    /* we've been asked to quit. */
    DestroyWindow(detectCDHwnd);
    UnregisterClassA(classname, hInstance);
    return 0;
} /* detectCDThread */

static void detectAvailableCDs(PHYSFS_StringCallback cb, void *data)
{
    char drive_str[4] = { 'x', ':', '\\', '\0' };
    DWORD drives = 0;
    DWORD i;

    /*
     * If you poll a drive while a user is inserting a disc, the OS will
     *  block this thread until the drive has spun up. So we swallow the risk
     *  once for initial detection, and spin a thread that will get device
     *  events thereafter, for apps that use this interface to poll for
     *  disc insertion.
     */
    if (!detectCDThreadHandle)
    {
        HANDLE initialDetectDone = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (!initialDetectDone)
            return;  /* oh well. */

        detectCDThreadHandle = CreateThread(NULL, 0, detectCDThread,
                                            &initialDetectDone, 0, NULL);
        if (detectCDThreadHandle)
            WaitForSingleObject(initialDetectDone, INFINITE);
        CloseHandle(initialDetectDone);

        if (!detectCDThreadHandle)
            return;  /* oh well. */
    } /* if */

    drives = drivesWithMediaBitmap; /* whatever the thread has seen, we take. */
    for (i = 'A'; i <= 'Z'; i++)
    {
        if (drives & (1 << (i - 'A')))
        {
            drive_str[0] = (char) i;
            cb(data, drive_str);
        } /* if */
    } /* for */
} /* detectAvailableCDs */

static void deinitCDThread(void)
{
    if (detectCDThreadHandle)
    {
        if (detectCDHwnd)
            PostMessageW(detectCDHwnd, WM_QUIT, 0, 0);
        CloseHandle(detectCDThreadHandle);
        detectCDThreadHandle = NULL;
        drivesWithMediaBitmap = 0;
    } /* if */
} /* deinitCDThread */
#endif


void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data)
{
    detectAvailableCDs(cb, data);
} /* __PHYSFS_platformDetectAvailableCDs */

#ifdef PHYSFS_PLATFORM_WINRT
static char *calcDirAppendSep(const WCHAR *wdir)
{
    size_t len;
    void *ptr;
    char *retval;
    BAIL_IF(!wdir, errcodeFromWinApi(), NULL);
    retval = unicodeToUtf8Heap(wdir);
    BAIL_IF_ERRPASS(!retval, NULL);
    len = strlen(retval);
    ptr = physfs_allocator.Realloc(retval, len + 2);
    if (!ptr)
    {
        physfs_allocator.Free(retval);
        BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    } /* if */
    retval = (char *) ptr;
    retval[len] = '\\';
    retval[len+1] = '\0';
    return retval;
} /* calcDirAppendSep */
#endif

char *__PHYSFS_platformCalcBaseDir(const char *argv0)
{
#ifdef PHYSFS_PLATFORM_WINRT
    return calcDirAppendSep((const WCHAR *) __PHYSFS_winrtCalcBaseDir());
#else
    char *retval = NULL;
    DWORD buflen = 64;
    LPWSTR modpath = NULL;

    while (1)
    {
        DWORD rc;
        void *ptr;

        if ( (ptr = physfs_allocator.Realloc(modpath, buflen*sizeof(WCHAR))) == NULL )
        {
            physfs_allocator.Free(modpath);
            BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        } /* if */
        modpath = (LPWSTR) ptr;

        rc = GetModuleFileNameW(NULL, modpath, buflen);
        if (rc == 0)
        {
            physfs_allocator.Free(modpath);
            BAIL(errcodeFromWinApi(), NULL);
        } /* if */

        if (rc < buflen)
        {
            buflen = rc;
            break;
        } /* if */

        buflen *= 2;
    } /* while */

    if (buflen > 0)  /* just in case... */
    {
        WCHAR *ptr = (modpath + buflen) - 1;
        while (ptr != modpath)
        {
            if (*ptr == '\\')
                break;
            ptr--;
        } /* while */

        if ((ptr == modpath) && (*ptr != '\\'))
            PHYSFS_setErrorCode(PHYSFS_ERR_OTHER_ERROR);  /* oh well. */
        else
        {
            *(ptr+1) = '\0';  /* chop off filename. */
            retval = unicodeToUtf8Heap(modpath);
        } /* else */
    } /* else */
    physfs_allocator.Free(modpath);

    return retval;   /* w00t. */
#endif
} /* __PHYSFS_platformCalcBaseDir */


char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app)
{
#ifdef PHYSFS_PLATFORM_WINRT
    return calcDirAppendSep((const WCHAR *) __PHYSFS_winrtCalcPrefDir());
#else
    /*
     * Vista and later has a new API for this, but SHGetFolderPath works there,
     *  and apparently just wraps the new API. This is the new way to do it:
     *
     *     SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE,
     *                          NULL, &wszPath);
     */

    WCHAR path[MAX_PATH];
    char *utf8 = NULL;
    size_t len = 0;
    char *retval = NULL;

    if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE,
                                   NULL, 0, path)))
        BAIL(PHYSFS_ERR_OS_ERROR, NULL);

    utf8 = unicodeToUtf8Heap(path);
    BAIL_IF_ERRPASS(!utf8, NULL);
    len = strlen(utf8) + strlen(org) + strlen(app) + 4;
    retval = (char*)physfs_allocator.Malloc(len);
    if (!retval)
    {
        physfs_allocator.Free(utf8);
        BAIL(PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    } /* if */

    snprintf(retval, len, "%s\\%s\\%s\\", utf8, org, app);
    physfs_allocator.Free(utf8);
    return retval;
#endif
} /* __PHYSFS_platformCalcPrefDir */


char *__PHYSFS_platformCalcUserDir(void)
{
#ifdef PHYSFS_PLATFORM_WINRT
    return calcDirAppendSep((const WCHAR *) __PHYSFS_winrtCalcPrefDir());
#else
    typedef BOOL (WINAPI *fnGetUserProfDirW)(HANDLE, LPWSTR, LPDWORD);
    fnGetUserProfDirW pGetDir = NULL;
    HMODULE lib = NULL;
    HANDLE accessToken = NULL;       /* Security handle to process */
    char *retval = NULL;

    lib = LoadLibraryA("userenv.dll");
    BAIL_IF(!lib, errcodeFromWinApi(), NULL);
    pGetDir=(fnGetUserProfDirW) GetProcAddress(lib,"GetUserProfileDirectoryW");
    GOTO_IF(!pGetDir, errcodeFromWinApi(), done);

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &accessToken))
        GOTO(errcodeFromWinApi(), done);
    else
    {
        DWORD psize = 0;
        LPWSTR wstr = NULL;
        BOOL rc = 0;

        /*
         * Should fail. Will write the size of the profile path in
         *  psize. Also note that the second parameter can't be
         *  NULL or the function fails on Windows XP, but has to be NULL on
         *  Windows 10 or it will fail.  :(
         */
        rc = pGetDir(accessToken, NULL, &psize);
        GOTO_IF(rc, PHYSFS_ERR_OS_ERROR, done);  /* should have failed! */

        if (psize == 0)  /* probably on Windows XP, try a different way. */
        {
            WCHAR x = 0;
            rc = pGetDir(accessToken, &x, &psize);
            GOTO_IF(rc, PHYSFS_ERR_OS_ERROR, done);  /* should have failed! */
            GOTO_IF(!psize, PHYSFS_ERR_OS_ERROR, done);  /* Uhoh... */
        } /* if */

        /* Allocate memory for the profile directory */
        wstr = (LPWSTR) __PHYSFS_smallAlloc((psize + 1) * sizeof (WCHAR));
        if (wstr != NULL)
        {
            if (pGetDir(accessToken, wstr, &psize))
            {
                /* Make sure it ends in a dirsep. We allocated +1 for this. */
                if (wstr[psize - 2] != '\\')
                {
                    wstr[psize - 1] = '\\';
                    wstr[psize - 0] = '\0';
                } /* if */
                retval = unicodeToUtf8Heap(wstr);
            } /* if */
            __PHYSFS_smallFree(wstr);
        } /* if */
    } /* if */

done:
    if (accessToken)
        CloseHandle(accessToken);
    FreeLibrary(lib);
    return retval;  /* We made it: hit the showers. */
#endif
} /* __PHYSFS_platformCalcUserDir */


int __PHYSFS_platformInit(void)
{
    return 1;  /* It's all good */
} /* __PHYSFS_platformInit */


void __PHYSFS_platformDeinit(void)
{
    deinitCDThread();
} /* __PHYSFS_platformDeinit */


void *__PHYSFS_platformGetThreadID(void)
{
    return ( (void *) ((size_t) GetCurrentThreadId()) );
} /* __PHYSFS_platformGetThreadID */


PHYSFS_EnumerateCallbackResult __PHYSFS_platformEnumerate(const char *dirname,
                               PHYSFS_EnumerateCallback callback,
                               const char *origdir, void *callbackdata)
{
    PHYSFS_EnumerateCallbackResult retval = PHYSFS_ENUM_OK;
    HANDLE dir = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW entw;
    size_t len = strlen(dirname);
    char *searchPath = NULL;
    WCHAR *wSearchPath = NULL;

    /* Allocate a new string for path, maybe '\\', "*", and NULL terminator */
    searchPath = (char *) __PHYSFS_smallAlloc(len + 3);
    BAIL_IF(!searchPath, PHYSFS_ERR_OUT_OF_MEMORY, PHYSFS_ENUM_ERROR);

    /* Copy current dirname */
    strcpy(searchPath, dirname);

    /* if there's no '\\' at the end of the path, stick one in there. */
    if (searchPath[len - 1] != '\\')
    {
        searchPath[len++] = '\\';
        searchPath[len] = '\0';
    } /* if */

    /* Append the "*" to the end of the string */
    strcat(searchPath, "*");

    UTF8_TO_UNICODE_STACK(wSearchPath, searchPath);
    __PHYSFS_smallFree(searchPath);
    BAIL_IF_ERRPASS(!wSearchPath, PHYSFS_ENUM_ERROR);

    dir = winFindFirstFileW(wSearchPath, &entw);
    __PHYSFS_smallFree(wSearchPath);
    BAIL_IF(dir==INVALID_HANDLE_VALUE, errcodeFromWinApi(), PHYSFS_ENUM_ERROR);

    do
    {
        const WCHAR *fn = entw.cFileName;
        char *utf8;

        if (fn[0] == '.')  /* ignore "." and ".." */
        {
            if ((fn[1] == '\0') || ((fn[1] == '.') && (fn[2] == '\0')))
                continue;
        } /* if */

        utf8 = unicodeToUtf8Heap(fn);
        if (utf8 == NULL)
            retval = (PHYSFS_EnumerateCallbackResult)-1;
        else
        {
            retval = callback(callbackdata, origdir, utf8);
            physfs_allocator.Free(utf8);
            if (retval == PHYSFS_ENUM_ERROR)
                PHYSFS_setErrorCode(PHYSFS_ERR_APP_CALLBACK);
        } /* else */
    } while ((retval == PHYSFS_ENUM_OK) && (FindNextFileW(dir, &entw) != 0));

    FindClose(dir);

    return retval;
} /* __PHYSFS_platformEnumerate */


int __PHYSFS_platformMkDir(const char *path)
{
    WCHAR *wpath;
    DWORD rc;
    UTF8_TO_UNICODE_STACK(wpath, path);
    rc = CreateDirectoryW(wpath, NULL);
    __PHYSFS_smallFree(wpath);
    BAIL_IF(rc == 0, errcodeFromWinApi(), 0);
    return 1;
} /* __PHYSFS_platformMkDir */


static HANDLE __doOpen(const char *fname, DWORD mode, DWORD creation)
{
    HANDLE fileh;
    WCHAR *wfname;

    UTF8_TO_UNICODE_STACK(wfname, fname);
    BAIL_IF(!wfname, PHYSFS_ERR_OUT_OF_MEMORY, NULL);

    fileh = winCreateFileW(wfname, mode, creation);
    __PHYSFS_smallFree(wfname);

    if (fileh == INVALID_HANDLE_VALUE)
        BAIL(errcodeFromWinApi(), INVALID_HANDLE_VALUE);

    return fileh;
} /* __doOpen */


void *__PHYSFS_platformOpenRead(const char *filename)
{
    HANDLE h = __doOpen(filename, GENERIC_READ, OPEN_EXISTING);
    return (h == INVALID_HANDLE_VALUE) ? NULL : (void *) h;
} /* __PHYSFS_platformOpenRead */


void *__PHYSFS_platformOpenWrite(const char *filename)
{
    HANDLE h = __doOpen(filename, GENERIC_WRITE, CREATE_ALWAYS);
    return (h == INVALID_HANDLE_VALUE) ? NULL : (void *) h;
} /* __PHYSFS_platformOpenWrite */


void *__PHYSFS_platformOpenAppend(const char *filename)
{
    HANDLE h = __doOpen(filename, GENERIC_WRITE, OPEN_ALWAYS);
    BAIL_IF_ERRPASS(h == INVALID_HANDLE_VALUE, NULL);

    if (!winSetFilePointer(h, 0, NULL, FILE_END))
    {
        const PHYSFS_ErrorCode err = errcodeFromWinApi();
        CloseHandle(h);
        BAIL(err, NULL);
    } /* if */

    return (void *) h;
} /* __PHYSFS_platformOpenAppend */


PHYSFS_sint64 __PHYSFS_platformRead(void *opaque, void *buf, PHYSFS_uint64 len)
{
    HANDLE h = (HANDLE) opaque;
    PHYSFS_sint64 totalRead = 0;

    if (!__PHYSFS_ui64FitsAddressSpace(len))
        BAIL(PHYSFS_ERR_INVALID_ARGUMENT, -1);

    while (len > 0)
    {
        const DWORD thislen = (len > 0xFFFFFFFF) ? 0xFFFFFFFF : (DWORD) len;
        DWORD numRead = 0;
        if (!ReadFile(h, buf, thislen, &numRead, NULL))
            BAIL(errcodeFromWinApi(), -1);
        len -= (PHYSFS_uint64) numRead;
        totalRead += (PHYSFS_sint64) numRead;
        if (numRead != thislen)
            break;
    } /* while */

    return totalRead;
} /* __PHYSFS_platformRead */


PHYSFS_sint64 __PHYSFS_platformWrite(void *opaque, const void *buffer,
                                     PHYSFS_uint64 len)
{
    HANDLE h = (HANDLE) opaque;
    PHYSFS_sint64 totalWritten = 0;

    if (!__PHYSFS_ui64FitsAddressSpace(len))
        BAIL(PHYSFS_ERR_INVALID_ARGUMENT, -1);

    while (len > 0)
    {
        const DWORD thislen = (len > 0xFFFFFFFF) ? 0xFFFFFFFF : (DWORD) len;
        DWORD numWritten = 0;
        if (!WriteFile(h, buffer, thislen, &numWritten, NULL))
            BAIL(errcodeFromWinApi(), -1);
        len -= (PHYSFS_uint64) numWritten;
        totalWritten += (PHYSFS_sint64) numWritten;
        if (numWritten != thislen)
            break;
    } /* while */

    return totalWritten;
} /* __PHYSFS_platformWrite */


int __PHYSFS_platformSeek(void *opaque, PHYSFS_uint64 pos)
{
    HANDLE h = (HANDLE) opaque;
    const PHYSFS_sint64 spos = (PHYSFS_sint64) pos;
    BAIL_IF(!winSetFilePointer(h,spos,NULL,FILE_BEGIN), errcodeFromWinApi(), 0);
    return 1;  /* No error occured */
} /* __PHYSFS_platformSeek */


PHYSFS_sint64 __PHYSFS_platformTell(void *opaque)
{
    HANDLE h = (HANDLE) opaque;
    PHYSFS_sint64 pos = 0;
    BAIL_IF(!winSetFilePointer(h,0,&pos,FILE_CURRENT), errcodeFromWinApi(), -1);
    return pos;
} /* __PHYSFS_platformTell */


PHYSFS_sint64 __PHYSFS_platformFileLength(void *opaque)
{
    HANDLE h = (HANDLE) opaque;
    const PHYSFS_sint64 retval = winGetFileSize(h);
    BAIL_IF(retval < 0, errcodeFromWinApi(), -1);
    return retval;
} /* __PHYSFS_platformFileLength */


int __PHYSFS_platformFlush(void *opaque)
{
    HANDLE h = (HANDLE) opaque;
    BAIL_IF(!FlushFileBuffers(h), errcodeFromWinApi(), 0);
    return 1;
} /* __PHYSFS_platformFlush */


void __PHYSFS_platformClose(void *opaque)
{
    HANDLE h = (HANDLE) opaque;
    (void) CloseHandle(h); /* ignore errors. You should have flushed! */
} /* __PHYSFS_platformClose */


static int doPlatformDelete(LPWSTR wpath)
{
    WIN32_FILE_ATTRIBUTE_DATA info;
    if (!GetFileAttributesExW(wpath, GetFileExInfoStandard, &info))
        BAIL(errcodeFromWinApi(), 0);
    else
    {
        const int isdir = (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        const BOOL rc = isdir ? RemoveDirectoryW(wpath) : DeleteFileW(wpath);
        BAIL_IF(!rc, errcodeFromWinApi(), 0);
    } /* else */
    return 1;   /* if you made it here, it worked. */
} /* doPlatformDelete */


int __PHYSFS_platformDelete(const char *path)
{
    int retval = 0;
    LPWSTR wpath = NULL;
    UTF8_TO_UNICODE_STACK(wpath, path);
    BAIL_IF(!wpath, PHYSFS_ERR_OUT_OF_MEMORY, 0);
    retval = doPlatformDelete(wpath);
    __PHYSFS_smallFree(wpath);
    return retval;
} /* __PHYSFS_platformDelete */


void *__PHYSFS_platformCreateMutex(void)
{
    LPCRITICAL_SECTION lpcs;
    lpcs = (LPCRITICAL_SECTION) physfs_allocator.Malloc(sizeof (CRITICAL_SECTION));
    BAIL_IF(!lpcs, PHYSFS_ERR_OUT_OF_MEMORY, NULL);

    if (!winInitializeCriticalSection(lpcs))
    {
        physfs_allocator.Free(lpcs);
        BAIL(errcodeFromWinApi(), NULL);
    } /* if */

    return lpcs;
} /* __PHYSFS_platformCreateMutex */


void __PHYSFS_platformDestroyMutex(void *mutex)
{
    DeleteCriticalSection((LPCRITICAL_SECTION) mutex);
    physfs_allocator.Free(mutex);
} /* __PHYSFS_platformDestroyMutex */


int __PHYSFS_platformGrabMutex(void *mutex)
{
    EnterCriticalSection((LPCRITICAL_SECTION) mutex);
    return 1;
} /* __PHYSFS_platformGrabMutex */


void __PHYSFS_platformReleaseMutex(void *mutex)
{
    LeaveCriticalSection((LPCRITICAL_SECTION) mutex);
} /* __PHYSFS_platformReleaseMutex */


static PHYSFS_sint64 FileTimeToPhysfsTime(const FILETIME *ft)
{
    SYSTEMTIME st_utc;
    SYSTEMTIME st_localtz;
    TIME_ZONE_INFORMATION tzi;
    DWORD tzid;
    PHYSFS_sint64 retval;
    struct tm tm;
    BOOL rc;

    BAIL_IF(!FileTimeToSystemTime(ft, &st_utc), errcodeFromWinApi(), -1);
    tzid = GetTimeZoneInformation(&tzi);
    BAIL_IF(tzid == TIME_ZONE_ID_INVALID, errcodeFromWinApi(), -1);
    rc = SystemTimeToTzSpecificLocalTime(&tzi, &st_utc, &st_localtz);
    BAIL_IF(!rc, errcodeFromWinApi(), -1);

    /* Convert to a format that mktime() can grok... */
    tm.tm_sec = st_localtz.wSecond;
    tm.tm_min = st_localtz.wMinute;
    tm.tm_hour = st_localtz.wHour;
    tm.tm_mday = st_localtz.wDay;
    tm.tm_mon = st_localtz.wMonth - 1;
    tm.tm_year = st_localtz.wYear - 1900;
    tm.tm_wday = -1 /*st_localtz.wDayOfWeek*/;
    tm.tm_yday = -1;
    tm.tm_isdst = -1;

    /* Convert to a format PhysicsFS can grok... */
    retval = (PHYSFS_sint64) mktime(&tm);
    BAIL_IF(retval == -1, PHYSFS_ERR_OS_ERROR, -1);
    return retval;
} /* FileTimeToPhysfsTime */


/* check for symlinks. These exist in NTFS 3.1 (WinXP), even though
   they aren't really available to userspace before Vista. I wonder
   what would happen if you put an NTFS disk with a symlink on it
   into an XP machine, though; would this flag get set?
   NTFS symlinks are a form of "reparse point" (junction, volume mount,
   etc), so if the REPARSE_POINT attribute is set, check for the symlink
   tag thereafter. This assumes you already read in the file attributes. */
static int isSymlink(const WCHAR *wpath, const DWORD attr)
{
    WIN32_FIND_DATAW w32dw;
    HANDLE h;

    if ((attr & PHYSFS_FILE_ATTRIBUTE_REPARSE_POINT) == 0)
        return 0;  /* not a reparse point? Definitely not a symlink. */

    h = winFindFirstFileW(wpath, &w32dw);
    if (h == INVALID_HANDLE_VALUE)
        return 0;  /* ...maybe the file just vanished...? */

    FindClose(h);
    return (w32dw.dwReserved0 == PHYSFS_IO_REPARSE_TAG_SYMLINK);
} /* isSymlink */


int __PHYSFS_platformStat(const char *filename, PHYSFS_Stat *st, const int follow)
{
    WIN32_FILE_ATTRIBUTE_DATA winstat;
    WCHAR *wstr = NULL;
    DWORD err = 0;
    BOOL rc = 0;
    int issymlink = 0;

    UTF8_TO_UNICODE_STACK(wstr, filename);
    BAIL_IF(!wstr, PHYSFS_ERR_OUT_OF_MEMORY, 0);
    rc = GetFileAttributesExW(wstr, GetFileExInfoStandard, &winstat);

    if (!rc)
        err = GetLastError();
    else  /* check for symlink while wstr is still available */
        issymlink = !follow && isSymlink(wstr, winstat.dwFileAttributes);

    __PHYSFS_smallFree(wstr);
    BAIL_IF(!rc, errcodeFromWinApiError(err), 0);

    st->modtime = FileTimeToPhysfsTime(&winstat.ftLastWriteTime);
    st->accesstime = FileTimeToPhysfsTime(&winstat.ftLastAccessTime);
    st->createtime = FileTimeToPhysfsTime(&winstat.ftCreationTime);

    if (issymlink)
    {
        st->filetype = PHYSFS_FILETYPE_SYMLINK;
        st->filesize = 0;
    } /* if */

    else if (winstat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        st->filetype = PHYSFS_FILETYPE_DIRECTORY;
        st->filesize = 0;
    } /* else if */

    else if (winstat.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_DEVICE))
    {
        st->filetype = PHYSFS_FILETYPE_OTHER;
        st->filesize = (((PHYSFS_uint64) winstat.nFileSizeHigh) << 32) | winstat.nFileSizeLow;
    } /* else if */

    else
    {
        st->filetype = PHYSFS_FILETYPE_REGULAR;
        st->filesize = (((PHYSFS_uint64) winstat.nFileSizeHigh) << 32) | winstat.nFileSizeLow;
    } /* else */

    st->readonly = ((winstat.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0);

    return 1;
} /* __PHYSFS_platformStat */

#endif  /* PHYSFS_PLATFORM_WINDOWS */

/* end of physfs_platform_windows.c ... */



#ifdef __cplusplus
}
#endif


/*
 * Apple platform (macOS, iOS, watchOS, etc) support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_platforms.h"
*/
#ifdef PHYSFS_PLATFORM_APPLE

#include <Foundation/Foundation.h>

/*#include "physfs_internal.h"*/

int __PHYSFS_platformInit(void)
{
    return 1;  /* success. */
} /* __PHYSFS_platformInit */


void __PHYSFS_platformDeinit(void)
{
    /* no-op */
} /* __PHYSFS_platformDeinit */


char *__PHYSFS_platformCalcBaseDir(const char *argv0)
{
    @autoreleasepool
    {
        NSString *path = [[NSBundle mainBundle] bundlePath];
        BAIL_IF(!path, PHYSFS_ERR_OS_ERROR, NULL);
        size_t len = [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        char *retval = (char *) allocator.Malloc(len + 2);
        BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        [path getCString:retval maxLength:len+1 encoding:NSUTF8StringEncoding];
        retval[len] = '/';
        retval[len+1] = '\0';
        return retval;  /* whew. */
    } /* @autoreleasepool */
} /* __PHYSFS_platformCalcBaseDir */


char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app)
{
    @autoreleasepool
    {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, TRUE);
        BAIL_IF(!paths, PHYSFS_ERR_OS_ERROR, NULL);
        NSString *path = (NSString *) [paths objectAtIndex:0];
        BAIL_IF(!path, PHYSFS_ERR_OS_ERROR, NULL);
        size_t len = [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        const size_t applen = strlen(app);
        char *retval = (char *) allocator.Malloc(len + applen + 3);
        BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        [path getCString:retval maxLength:len+1 encoding:NSUTF8StringEncoding];
        snprintf(retval + len, applen + 3, "/%s/", app);
        return retval;  /* whew. */
    } /* @autoreleasepool */
} /* __PHYSFS_platformCalcPrefDir */


/* CD-ROM detection code... */

/*
 * Code based on sample from Apple Developer Connection:
 *  https://developer.apple.com/samplecode/Sample_Code/Devices_and_Hardware/Disks/VolumeToBSDNode/VolumeToBSDNode.c.htm
 */

#if !defined(PHYSFS_NO_CDROM_SUPPORT)

#include <IOKit/IOKitLib.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IODVDMedia.h>
#include <sys/mount.h>

static int darwinIsWholeMedia(io_service_t service)
{
    int retval = 0;
    CFTypeRef wholeMedia;

    if (!IOObjectConformsTo(service, kIOMediaClass))
        return 0;

    wholeMedia = IORegistryEntryCreateCFProperty(service,
                                                 CFSTR(kIOMediaWholeKey),
                                                 NULL, 0);
    if (wholeMedia == NULL)
        return 0;

    retval = CFBooleanGetValue(wholeMedia);
    CFRelease(wholeMedia);

    return retval;
} /* darwinIsWholeMedia */


static int darwinIsMountedDisc(char *bsdName, mach_port_t masterPort)
{
    int retval = 0;
    CFMutableDictionaryRef matchingDict;
    kern_return_t rc;
    io_iterator_t iter;
    io_service_t service;

    if ((matchingDict = IOBSDNameMatching(masterPort, 0, bsdName)) == NULL)
        return 0;

    rc = IOServiceGetMatchingServices(masterPort, matchingDict, &iter);
    if ((rc != KERN_SUCCESS) || (!iter))
        return 0;

    service = IOIteratorNext(iter);
    IOObjectRelease(iter);
    if (!service)
        return 0;

    rc = IORegistryEntryCreateIterator(service, kIOServicePlane,
             kIORegistryIterateRecursively | kIORegistryIterateParents, &iter);

    if (!iter)
        return 0;

    if (rc != KERN_SUCCESS)
    {
        IOObjectRelease(iter);
        return 0;
    } /* if */

    IOObjectRetain(service);  /* add an extra object reference... */

    do
    {
        if (darwinIsWholeMedia(service))
        {
            if ( (IOObjectConformsTo(service, kIOCDMediaClass)) ||
                 (IOObjectConformsTo(service, kIODVDMediaClass)) )
            {
                retval = 1;
            } /* if */
        } /* if */
        IOObjectRelease(service);
    } while ((service = IOIteratorNext(iter)) && (!retval));

    IOObjectRelease(iter);
    IOObjectRelease(service);

    return retval;
} /* darwinIsMountedDisc */

#endif /* !defined(PHYSFS_NO_CDROM_SUPPORT) */


void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data)
{
#if !defined(PHYSFS_NO_CDROM_SUPPORT)
    const char *devPrefix = "/dev/";
    const int prefixLen = strlen(devPrefix);
    mach_port_t masterPort = 0;
    struct statfs *mntbufp;
    int i, mounts;

    if (IOMasterPort(MACH_PORT_NULL, &masterPort) != KERN_SUCCESS)
        BAIL(PHYSFS_ERR_OS_ERROR, ) /*return void*/;

    mounts = getmntinfo(&mntbufp, MNT_WAIT);  /* NOT THREAD SAFE! */
    for (i = 0; i < mounts; i++)
    {
        char *dev = mntbufp[i].f_mntfromname;
        char *mnt = mntbufp[i].f_mntonname;
        if (strncmp(dev, devPrefix, prefixLen) != 0)  /* a virtual device? */
            continue;

        dev += prefixLen;
        if (darwinIsMountedDisc(dev, masterPort))
            cb(data, mnt);
    } /* for */
#endif /* !defined(PHYSFS_NO_CDROM_SUPPORT) */
} /* __PHYSFS_platformDetectAvailableCDs */

#endif /* PHYSFS_PLATFORM_APPLE */

/* end of physfs_platform_apple.m ... */

/*
 * Haiku platform-dependent support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_platforms.h"
*/
#ifdef PHYSFS_PLATFORM_HAIKU

#include <os/kernel/OS.h>
#include <os/app/Roster.h>
#include <os/storage/Volume.h>
#include <os/storage/VolumeRoster.h>
#include <os/storage/Directory.h>
#include <os/storage/Entry.h>
#include <os/storage/Path.h>
#include <os/kernel/fs_info.h>
#include <os/device/scsi.h>

#include <errno.h>
#include <unistd.h>

/*#include "physfs_internal.h"*/

int __PHYSFS_platformInit(void)
{
    return 1;  /* always succeed. */
} /* __PHYSFS_platformInit */


void __PHYSFS_platformDeinit(void)
{
    /* no-op */
} /* __PHYSFS_platformDeinit */


static char *getMountPoint(const char *devname, char *buf, size_t bufsize)
{
    BVolumeRoster mounts;
    BVolume vol;

    mounts.Rewind();
    while (mounts.GetNextVolume(&vol) == B_NO_ERROR)
    {
        fs_info fsinfo;
        fs_stat_dev(vol.Device(), &fsinfo);
        if (strcmp(devname, fsinfo.device_name) == 0)
        {
            BDirectory directory;
            BEntry entry;
            BPath path;
            const char *str;

            if ( (vol.GetRootDirectory(&directory) < B_OK) ||
                 (directory.GetEntry(&entry) < B_OK) ||
                 (entry.GetPath(&path) < B_OK) ||
                 ( (str = path.Path()) == NULL) )
                return NULL;

            strncpy(buf, str, bufsize-1);
            buf[bufsize-1] = '\0';
            return buf;
        } /* if */
    } /* while */

    return NULL;
} /* getMountPoint */


    /*
     * This function is lifted from Simple Directmedia Layer (SDL):
     *  https://www.libsdl.org/  ... this is zlib-licensed code, too.
     */
static void tryDir(const char *d, PHYSFS_StringCallback callback, void *data)
{
    BDirectory dir;
    dir.SetTo(d);
    if (dir.InitCheck() != B_NO_ERROR)
        return;

    dir.Rewind();
    BEntry entry;
    while (dir.GetNextEntry(&entry) >= 0)
    {
        BPath path;
        const char *name;
        entry_ref e;

        if (entry.GetPath(&path) != B_NO_ERROR)
            continue;

        name = path.Path();

        if (entry.GetRef(&e) != B_NO_ERROR)
            continue;

        if (entry.IsDirectory())
        {
            if (strcmp(e.name, "floppy") != 0)
                tryDir(name, callback, data);
            continue;
        } /* if */

        const int devfd = open(name, O_RDONLY);
        if (devfd < 0)
            continue;

        device_geometry g;
        const int rc = ioctl(devfd, B_GET_GEOMETRY, &g, sizeof (g));
        close(devfd);
        if (rc < 0)
            continue;

        if (g.device_type != B_CD)
            continue;

        char mntpnt[B_FILE_NAME_LENGTH];
        if (getMountPoint(name, mntpnt, sizeof (mntpnt)))
            callback(data, mntpnt);
    } /* while */
} /* tryDir */


void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data)
{
    tryDir("/dev/disk", cb, data);
} /* __PHYSFS_platformDetectAvailableCDs */


static team_id getTeamID(void)
{
    thread_info info;
    thread_id tid = find_thread(NULL);
    get_thread_info(tid, &info);
    return info.team;
} /* getTeamID */


char *__PHYSFS_platformCalcBaseDir(const char *argv0)
{
    image_info info;
    int32 cookie = 0;

    while (get_next_image_info(0, &cookie, &info) == B_OK)
    {
        if (info.type == B_APP_IMAGE)
            break;
    } /* while */

    BEntry entry(info.name, true);
    BPath path;
    status_t rc = entry.GetPath(&path);  /* (path) now has binary's path. */
    assert(rc == B_OK);
    rc = path.GetParent(&path); /* chop filename, keep directory. */
    assert(rc == B_OK);
    const char *str = path.Path();
    assert(str != NULL);
    const size_t len = strlen(str);
    char *retval = (char *) allocator.Malloc(len + 2);
    BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    strcpy(retval, str);
    retval[len] = '/';
    retval[len+1] = '\0';
    return retval;
} /* __PHYSFS_platformCalcBaseDir */


char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app)
{
    const char *userdir = __PHYSFS_getUserDir();
    const char *append = "config/settings/";
    const size_t len = strlen(userdir) + strlen(append) + strlen(app) + 2;
    char *retval = (char *) allocator.Malloc(len);
    BAIL_IF(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    snprintf(retval, len, "%s%s%s/", userdir, append, app);
    return retval;
} /* __PHYSFS_platformCalcPrefDir */

#endif  /* PHYSFS_PLATFORM_HAIKU */

/* end of physfs_platform_haiku.cpp ... */

/*
 * Windows Runtime (WinRT) support routines for PhysicsFS.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file originally written by Martin "T-Bone" Ahrnbom, but was mostly
 *  merged into physfs_platform_windows.c by Ryan C. Gordon (so please harass
 *  Ryan about bugs and not Martin).
 */

/* (There used to be instructions on how to make a WinRT project, but at
   this point, either CMake will do it for you or you should just drop
   PhysicsFS's sources into your existing project. --ryan.) */
/*
#define __PHYSICSFS_INTERNAL__
#include "physfs_platforms.h"
*/
#ifdef PHYSFS_PLATFORM_WINRT

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#include <windows.h>

/*#include "physfs_internal.h"*/

const void *__PHYSFS_winrtCalcBaseDir(void)
{
    return Windows::ApplicationModel::Package::Current->InstalledLocation->Path->Data();
} /* __PHYSFS_winrtCalcBaseDir */

const void *__PHYSFS_winrtCalcPrefDir(void)
{
    return Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data();
} /* __PHYSFS_winrtCalcBaseDir */


#endif /* PHYSFS_PLATFORM_WINRT */

/* end of physfs_platform_winrt.cpp ... */

#endif /* PHYSFS_PLATFORM_IMPL */


#if defined(__PHYSICSFS_INTERNAL__)

/* cleanup macros defined for internal use */
#ifdef allocator
#undef allocator
#endif

#endif


/*
    License for PhysFS

    zlib license

    Copyright (c) 2001-2020 Ryan C. Gordon and others.

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from
    the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software in a
    product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

        Ryan C. Gordon <icculus@icculus.org>
*/

/*
    License for miniphyfs.h

    zlib license

    Copyright (c) 2020 Eduardo Bart <edub4rt@gmail.com>

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from
    the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software in a
    product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/
