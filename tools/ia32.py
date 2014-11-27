def generate(env, **kwargs):
    arch = env.get('ARCH')
    sysroot = env.get('SYSROOT')
    if sysroot is None:
        sysroot = ""
    else:
        sysroot = sysroot + "/bin/"
    if arch == 'ia32':
        env['LLVM_TRIPLE'] = llvm_triple = 'i686-linux-gnu'
        env['GCC_TRIPLE'] = gcc_triple = 'i686-pc-elf'
        target = gcc_triple + '-'
        env['LINKER_SCRIPT'] = env.Dir('#').abspath + '/kernel/ia32/pc/link.ld'
    else:
        raise Exception("unsupported arch")
    env['CC'] = sysroot + target + 'gcc'
    env['AR'] = sysroot + target + 'ar'
    env['OBJCOPY'] = sysroot + target + 'objcopy'
    env['RANLIB'] = sysroot + target + 'ranlib'
    env['CFLAGS'] = '-D__ChrisOS__ -std=gnu99 -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow'
    env['LINKFLAGS'] = ''

def exists(env):
    return 1
