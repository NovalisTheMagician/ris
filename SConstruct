import os

game = 'Game'

env = Environment(ENV=os.environ)

cl_flags = ['/EHsc', '/std:c++17', '/FS']
lk_flags = ['/nologo', '/INCREMENTAL:NO', '/LTCG:NOSTATUS', '/MACHINE:X64']

dyn_libs = []

inc_path = ['#src', os.environ['INC_PATH']]
lib_path = [os.environ['LIB_PATH']]

libs_rel = ['glfw3', 'soloud', 'libz', 'libzip', 'squirrel', 'sqstdlib', 'fmt', 'glad']
libs_deb = ['glfw3d', 'soloudd', 'libzd', 'libzipd', 'squirreld', 'sqstdlibd', 'fmtd', 'gladd']

libs = ['user32', 'kernel32', 'gdi32', 'shell32', 'opengl32', 'ws2_32', 'advapi32']

defines = ['ZIP_STATIC']

debug = ARGUMENTS.get('debug', 0)
verbose = ARGUMENTS.get('verbose', 0)
if int(debug):
    cl_flags.append(['/Zi', '/MTd'])
    lk_flags.append(['/DEBUG'])
    defines.append(['_DEBUG'])
    libs.append(libs_deb)
else:
    cl_flags.append(['/O2', '/MT'])
    defines.append(['NDEBUG'])
    libs.append(libs_rel)

if not int(verbose):
    env['CXXCOMSTR'] = 'Compiling $TARGET'
    env['LINKCOMSTR'] = 'Linking $TARGET'
    env['RCCOMSTR'] = 'Resource $TARGET'
    env['INSTALLSTR'] = 'Copying $TARGET'

env['CPPFLAGS'] = cl_flags
env['LINKFLAGS'] = lk_flags
env['CPPPATH'] = inc_path
env['CPPDEFINES'] = defines
env['LIBPATH'] = lib_path
env['LIBS'] = libs

Export('env')

objs = SConscript('src/SConscript', variant_dir='build', duplicate=0)

client = env.Program('build/' + game, objs)

env.Clean(client, 'build/' + game + '.ilk')
env.Clean(client, 'build/' + game + '.pdb')
env.Clean(client, 'build/' + game + '.exp')
env.Clean(client, 'build/' + game + '.lib')

client_install = env.Install('bin/', client)
dyn_copy = env.Install('bin/', dyn_libs)

env.Depends(client, [dyn_copy])
env.Default(client)

env.Precious(client)

cl = env.Alias('client', client_install)

env.Alias('all', [cl])
