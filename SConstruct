import os

shader_bld = Builder(action=Action('glslc $SHADERFLAGS -o $TARGET $SOURCE', '$SHADERCOMSTR'), suffix='.spv', src_suffix='.glsl', single_source=1)
shadersrc_bld = Builder(action=Action('glslc -E $SHADERFLAGS -Os -o $TARGET $SOURCE', '$SHADERCOMSTR'), suffix='.amd.src', src_suffix='.glsl', single_source=1)

env = Environment(ENV=os.environ)
env.Append(BUILDERS = {'Shader' : shader_bld})
env.Append(BUILDERS = {'ShaderSrc' : shadersrc_bld})
env['SHDAERFLAGS'] = []

cl_flags = ['/EHsc', '/std:c++17', '/FS']
lk_flags = ['/nologo']

shader_flags = ['-c', '--target-env=opengl', '-Werror']

dyn_libs = ['libs/glbinding/bin/glbinding.dll', 'libs/glbinding/bin/glbinding-aux.dll', 'libs/glfw/bin/glfw3.dll']

client_inc_path = ['src', 'libs/glm/include', 'libs/gli/include', 'libs/glbinding/include', 'libs/glfw/include', 'libs/soloud/include', 'libs/rapidjson/include']
client_lib_path = ['libs/glbinding/lib', 'libs/glfw/lib', 'libs/soloud/lib']
client_libs = ['user32', 'kernel32', 'gdi32', 'opengl32', 'glbinding', 'glbinding-aux', 'glfw3dll', 'soloud_static']

common_inc_path = ['src', 'libs/glm/include', 'libs/rapidjson/include']

server_inc_path = ['src']
server_lib_path = []
server_libs = ['user32', 'kernel32']

defines = []

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

shader_files = Glob('src/client/shader/*.glsl')
barc_files = Glob('src/tools/barc/*.cpp')
client_files = Glob('src/client/*.cpp')
server_files = Glob('src/server/*.cpp')
common_files = Glob('src/common/*.cpp')

test_files = Glob('src/test/*.cpp')

rc_file = 'src/client/resource.rc'

shader_objs = env.Shader(shader_files, SHADERFLAGS=shader_flags)
shadersrc_objs = env.ShaderSrc(shader_files, SHADERFLAGS=shader_flags)
barc_objs = env.Object(barc_files, CPPPATH=['src'], CPPFLAGS=cl_flags, CPPDEFINES=defines)
common_objs = env.Object(common_files, CPPPATH=common_inc_path, CPPFLAGS=cl_flags, CPPDEFINES=defines)
client_objs = env.Object(client_files, CPPPATH=client_inc_path, CPPFLAGS=cl_flags, CPPDEFINES=defines)
server_objs = env.Object(server_files, CPPPATH=server_inc_path, CPPFLAGS=cl_flags, CPPDEFINES=defines)

test_objs = env.Object(test_files, CPPPATH=client_inc_path, CPPFLAGS=cl_flags, CPPDEFINES=defines)

rc_obj = env.RES(rc_file)

barc = env.Program('bin/tools/barc', barc_objs, LINKFLAGS=lk_flags)
client = env.Program('bin/RIS', client_objs + common_objs + rc_obj, LIBS=client_libs, LIBPATH=client_lib_path, LINKFLAGS=lk_flags)
server = env.Program('bin/RIS_server', server_objs + common_objs, LIBS=server_libs, LIBPATH=server_lib_path, LINKFLAGS=lk_flags)

test = env.Program('bin/amd_test', test_objs, LIBS=client_libs, LIBPATH=client_lib_path, LINKFLAGS=lk_flags + ['/SUBSYSTEM:WINDOWS'])

env.Clean(client, 'bin/RIS.ilk')
env.Clean(client, 'bin/RIS.pdb')
env.Clean(client, 'bin/RIS.exp')
env.Clean(client, 'bin/RIS.lib')

env.Clean(server, 'bin/RIS_server.ilk')
env.Clean(server, 'bin/RIS_server.pdb')
env.Clean(server, 'bin/RIS_server.exp')
env.Clean(server, 'bin/RIS_server.lib')

dyn_copy = env.Install('bin/', dyn_libs)
shader_copy = env.Install('bin/assets/shaders/', shader_objs)
shadersrc_copy = env.Install('bin/assets/shaders/', shadersrc_objs)

#env.Depends(shader_copy, shader_objs)
#env.Depends(shadersrc_copy, shadersrc_objs)

env.Depends(client, shader_copy)
env.Depends(client, shadersrc_copy)
env.Depends(client, dyn_copy)
env.Default(client)

env.Precious(client)
env.Precious(server)

cl = env.Alias('client', client)
sv = env.Alias('server', server)
bc = env.Alias('barc', barc)
sh = env.Alias('shader', shader_copy)

env.Depends(sh, shadersrc_copy)

tst = env.Alias('amd_test', test)
env.Alias('all', [cl, sv, bc])
