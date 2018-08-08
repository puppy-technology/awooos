#!/usr/bin/python3

from pathlib import Path
from subprocess import check_output
from buildlib import *

CC      = "clang"
AS      = "nasm"
AR      = "ar"
RANLIB  = "ranlib"
LD      = "ld"

BOCHS   = "bochs"


NAME        = "awoo"
BUILD_TYPE  = env("BUILD_TYPE",  "debug")
NAME_SUFFIX = env("NAME_SUFFIX", "")
TARGET      = env("TARGET",      "i386")

iso_dir     = env("ISO_DIR", "./iso")
ISO_DIR     = Path(iso_dir)
iso_fname   = "{}{}-{}-{}.iso".format(NAME, NAME_SUFFIX, TARGET, BUILD_TYPE)
ISO_FILE    = str(ISO_DIR / iso_fname)

CFLAGS  =   env("CFLAGS", [])
CFLAGS  +=  """
            -std=c11 -pedantic-errors -gdwarf-2 -nostdinc
            -ffreestanding -fno-stack-protector -fno-builtin
            -fdiagnostics-show-option -Werror -Weverything
            -Wno-cast-qual -Wno-missing-prototypes -Wno-vla
            """.split()

LDFLAGS =   env("LDFLAGS", [])
LDFLAGS +=  "-nostdlib -g --whole-archive".split()

ASFLAGS =   env("ASFLAGS", [])
ASFLAGS +=  "".split()

QEMU    = env("QEMU", "qemu-system-{}".format(TARGET))

QEMU_FLAGS  = env("QEMU_FLAGS", [])
if BUILD_TYPE == "test":
    # 1. Don't reboot on exit.
    # 2. Add isa-debug-exit device to allow qemu to have a non-zero exit status.
    #
    # This combination allows us to do a clean shutdown and have qemu return a
    # zero status code, or do a non-clean shutdown (using isa-debug-exit) and
    # have qemu return a nonzero status code.
    QEMU_FLAGS += "-no-reboot -device isa-debug-exit,iobase=0xf4,iosize=0x04".split()

all_files = [str(x) for x in Path("src").glob("**/*")]
ALL_FILES = \
    fnfilter(all_files, "src/libraries/*/src/*")            + \
    fnfilter(all_files, "src/libraries/*/src/*/*")          + \
    fnfilter(all_files, "src/libraries/*/platform-{}/*".format(TARGET))   + \
    fnfilter(all_files, "src/libraries/*/platform-{}/*/*".format(TARGET)) + \
    fnfilter(all_files, "src/kernel/*")

C_INCLUDES = env("C_INCLUDES", [])
for x in fnfilter(ALL_FILES, "src/libraries/*/include"):
    C_INCLUDES += ["-I", x]

SRCFILES =  [*filter(is_filetype(".c"),   ALL_FILES),
             *filter(is_filetype(".asm"), ALL_FILES)]
OBJFILES = list(map(with_suffix(".o"), SRCFILES))

C_FILES = fnfilter(SRCFILES, "*.c")

build_info = check_output(["bash", "./bin/generate_build_info.sh", BUILD_TYPE, TARGET]).decode()
with open("include/awoo/build_info.h", "w") as f:
    f.write(build_info)

# Any directory directly under src/libraries/ is treated as a library.
LIBRARY_DIRS = [str(x) for x in
                    filter(Path.is_dir, Path("src/libraries").glob("*"))]
LIBRARIES = [x + ".a" for x in LIBRARY_DIRS]
LIBDIR_GLOBS = [x + "/*.*" for x in LIBRARY_DIRS]

KERNEL_LDFLAGS = []
for lib in LIBRARIES:
    KERNEL_LDFLAGS += ["-l", ":" + lib]

recipe("%.o", "%.c", [],
        [CC, *CFLAGS, *C_INCLUDES, "-c", "{match}", "-o", "{target}"])

recipe("%.o", "%.asm", [],
        [AS, *ASFLAGS, "-o", "{target}", "{match}"])

for (library, glob) in zip(LIBRARIES, LIBDIR_GLOBS):
    recipe(library, None, fnfilter(SRCFILES, glob),
            lambda target, match, deps: [AR, "rcs", target, *deps])

recipe("src/kernel.exe", None, LIBRARIES,
        [LD, "-o", "{target}", "-L", "src/libraries", *LDFLAGS,
            "-T", "src/link-{target}.ld", "src/kernel/start-{target}.o",
            "src/kernel/main.o", *KERNEL_LDFLAGS])

task("clean", [],
        ["rm", "-rf", "./isofs", str(ISO_DIR / "*.iso"), *OBJFILES,
            *LIBRARIES, "src/*.exe"])

task("test", ["lint"],
        "./build.py BUILD_TYPE=test qemu")

task("lint", [],
        ["clang-check", *C_FILES, "--", *C_INCLUDES])

# Fetch all submodules.
task("fetch-submodules", [],
        "git submodule update --recursive --init")

# Update all submodules to latest versions.
task("update-submodules", [],
        "git submodule update --recursive --remote --init")

task("qemu", ["iso"],
        [QEMU, *QEMU_FLAGS, "-vga", "std", "-serial", "stdio",
            "-cdrom", ISO_FILE])

task("qemu-monitor", ["iso"],
        [QEMU, *QEMU_FLAGS, "-monitor", "stdio", "-cdrom", ISO_FILE])

task("vbox", ["iso"],
        ["VirtualBox", "--startvm", NAME, "--debug-statistics",
            "--debug-command-line", "--start-running"])

default("src/kernel.exe")
build()

"""
all: src/kernel.exe

iso: ${ISO_FILE}
${ISO_FILE}: src/kernel.exe
	@cp -r assets/isofs/ ./
	@cp src/kernel.exe isofs/
	xorriso -report_about HINT -abort_on WARNING -as mkisofs -quiet -boot-info-table -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -input-charset utf-8 -o ${ISO_FILE} isofs

bochs: iso
	cd iso && ${BOCHS} -q -f bochsrc-${TARGET}.txt

# Generate a nightly build.
task("nightly", [],
        ["./build.py", "BUILD_TYPE=nightly",
        "NAME_SUFFIX="-$(shell date +'%Y-%m-%d')",
        "iso"])

# List all of the event_trigger() and event_watch() calls.
list-events:
	@grep -rEho '(event_trigger|event_watch)\(".*"' src | tr '("' '\t ' | sort
"""
