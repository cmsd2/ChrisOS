AddOption('--sysroot',
    dest='sysroot',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='sysroot location')

AddOption('--arch',
    default='ia32',
    dest='arch',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='arch e.g. ia32')

arch = GetOption('arch')
env = Environment(SYSROOT = GetOption('sysroot'), ARCH = arch, tools = ['default', arch], toolpath = ['tools'])

RustBuilder = Builder(action = 'rustc --target $LLVM_TRIPLE --crate-type lib --emit obj -o $TARGET $SOURCE', src_suffix = 'rs', suffix = 'o', single_source = 1)
env['BUILDERS']['Rust'] = RustBuilder

Export('env')

SConscript('kernel/SConscript')

