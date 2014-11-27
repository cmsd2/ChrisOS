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

vars = Variables('vars.py')
vars.Add(BoolVariable('RELEASE', 'Set to build for release', False))
vars.Add(BoolVariable('ACPI_DEBUG_OUTPUT', 'Set to tell libacpica to print debug info', False))

arch = GetOption('arch')
project_root = Dir('#').abspath
build_root = Dir('.').abspath
env = Environment(variables = vars, BUILD_ROOT = build_root, PROJECT_ROOT = project_root, SYSROOT = GetOption('sysroot'), ARCH = arch, tools = ['default', arch], toolpath = ['tools'])

RustBuilder = Builder(action = 'rustc --target $LLVM_TRIPLE --crate-type lib --emit obj -o $TARGET $SOURCE', src_suffix = 'rs', suffix = 'o', single_source = 1)
env['BUILDERS']['Rust'] = RustBuilder

Export('env')

SConscript('kernel/SConscript')

