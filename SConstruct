import os

game = 'Game'

shader_bld = Builder(action=Action('glslc $SHADERFLAGS -o $TARGET $SOURCE', '$SHADERCOMSTR'), suffix='.spv', src_suffix='.glsl', single_source=1)
shadersrc_bld = Builder(action=Action('glslc -E $SHADERFLAGS -Os -o $TARGET $SOURCE', '$SHADERCOMSTR'), suffix='.src', src_suffix='.glsl', single_source=1)

env = Environment(ENV=os.environ)
env.Append(BUILDERS = {'Shader' : shader_bld})
env.Append(BUILDERS = {'ShaderSrc' : shadersrc_bld})
env['SHADERFLAGS'] = []

cl_flags = ['/EHsc', '/std:c++17', '/FS']
lk_flags = ['/nologo', '/INCREMENTAL:NO', '/LTCG:NOSTATUS', '/MACHINE:X64']

shader_flags = ['-c', '--target-env=opengl', '-Werror']

dyn_libs = ['libs/glbinding/bin/glbinding.dll', 'libs/glbinding/bin/glbinding-aux.dll', 'libs/glfw/bin/glfw3.dll']

inc_path = ['#src', '#libs/glm/include', '#libs/gli/include', '#libs/glbinding/include', '#libs/glfw/include', '#libs/soloud/include', '#libs/rapidjson/include', 
            '#libs/libzip/include', '#libs/libzippp/include', '#libs/lua/include', '#libs/luastate/include', '#libs/tiny_gltf/include']
lib_path = ['libs/glbinding/lib', 'libs/glfw/lib', 'libs/soloud/lib', 'libs/libzip/lib', 'libs/lua/lib']
libs = ['user32', 'kernel32', 'gdi32', 'opengl32', 'glbinding', 'glbinding-aux', 'glfw3dll', 'soloud_static', 'ws2_32', 'advapi32', 'libz-static', 'libzip-static', 'lua5.3.0-static']

defines = ['ZIP_STATIC']

debug = ARGUMENTS.get('debug', 0)
verbose = ARGUMENTS.get('verbose', 0)
if int(debug):
    cl_flags.append(['/Zi', '/MTd'])
    lk_flags.append(['/DEBUG'])
    defines.append(['_DEBUG'])
    shader_flags.append(['-Os'])
else:
    cl_flags.append(['/O2', '/MT'])
    shader_flags.append(['-O'])

if not int(verbose):
    env['CXXCOMSTR'] = 'Compiling $TARGET'
    env['LINKCOMSTR'] = 'Linking $TARGET'
    env['RCCOMSTR'] = 'Resource $TARGET'
    env['SHADERCOMSTR'] = 'Shader $TARGET'
    env['INSTALLSTR'] = 'Copying $TARGET'

env['SHADERFLAGS'] = shader_flags
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

dyn_copy = env.Install('build/', dyn_libs)

shader_copy = env.Install('build/assets/shaders/', Glob("build/shader/*.spv"))
shadersrc_copy = env.Install('build/assets/shaders/', Glob("build/shader/*.src"))

env.Depends(client, shader_copy)
env.Depends(client, shadersrc_copy)
env.Depends(client, dyn_copy)
env.Default(client)

env.Precious(client)

cl = env.Alias('client', client)
sh = env.Alias('shader', shader_copy)

env.Depends(sh, [shader_copy, shadersrc_copy])

env.Alias('all', [cl])
