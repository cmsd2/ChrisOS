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
    env['CXX'] = sysroot + target + 'g++'
    env['AR'] = sysroot + target + 'ar'
    env['OBJCOPY'] = sysroot + target + 'objcopy'
    env['RANLIB'] = sysroot + target + 'ranlib'
    env['GCC_FLAGS'] = '-D__ChrisOS__ -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow'
    env['CFLAGS'] = env['GCC_FLAGS'] + ' -std=gnu99'
    env['CXXFLAGS'] = env['GCC_FLAGS'] + ' -std=c++11 -fno-rtti -fno-exceptions'
    env['LINKFLAGS'] = ''

def exists(env):
    return 1
