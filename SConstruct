import os
import sys

game = 'Game'

env = Environment(ENV=os.environ)

cl_flags = []
lk_flags = []
dyn_libs = []
inc_path = []
lib_path = []
libs = []
defines = ['ZIP_STATIC']

debug = ARGUMENTS.get('debug', 0)
verbose = ARGUMENTS.get('verbose', 0)

if(sys.platform == 'linux'):
    cl_flags = ['-std=c++17', '-pthread']
    lk_flags = ['-pthread']
    inc_path = ['#src']
    libs = ['glfw', 'libz', 'libzip', 'fmt', 'glad', 'base64', 'boxer', 'soloud', 'dl', 'gtk-3', 'gobject-2.0', 'glib-2.0', 'asound'] 

    if int(debug):
        cl_flags.append(['-g', '-Wall', '-pedantic'])
        lk_flags.append(['-g'])
        defines.append(['_DEBUG'])
    else:
        cl_flags.append(['-O2'])
        defines.append(['NDEBUG'])

elif(sys.platform == 'win32'):
    cl_flags = ['/EHsc', '/std:c++17', '/FS']
    lk_flags = ['/nologo', '/INCREMENTAL:NO', '/MACHINE:X64'] # removed '/LTCG:NOSTATUS',
    
    dyn_libs = []
    
    inc_path = ['#src', os.environ['INC_PATH']]
    lib_path = [os.environ['LIB_PATH']]
    
    libs_rel = ['glfw3', 'soloud', 'libz', 'libzip', 'fmt', 'glad', 'boxer', 'base64', 'lua54']
    libs_deb = ['glfw3d', 'soloudd', 'libzd', 'libzipd', 'fmtd', 'gladd', 'boxerd', 'base64d', 'lua54d']
    
    libs = ['user32', 'kernel32', 'gdi32', 'shell32', 'opengl32', 'ws2_32', 'advapi32']

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
