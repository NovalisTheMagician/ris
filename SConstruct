cl_flags = ['/EHsc', '/std:c++17']
lk_flags = ['/nologo']

dyn_libs = ['libs/glbinding/bin/glbinding.dll', 'libs/glbinding/bin/glbinding-aux.dll', 'libs/glfw/bin/glfw3.dll']

client_inc_path = ['src', 'libs/glm/include', 'libs/gli/include', 'libs/glbinding/include', 'libs/glfw/include', 'libs/soloud/include', 'libs/rapidjson/include']
client_lib_path = ['libs/glbinding/lib', 'libs/glfw/lib', 'libs/soloud/lib']
client_libs = ['user32', 'kernel32', 'gdi32', 'opengl32', 'glbinding', 'glfw3dll', 'soloud_static']

common_inc_path = ['src', 'libs/glm/include', 'libs/rapidjson/include']

server_inc_path = ['src']
server_lib_path = []
server_libs = ['user32', 'kernel32']

defines = []

debug = ARGUMENTS.get('debug', 0)
if int(debug):
    cl_flags.append(['/Zi', '/MTd'])
    lk_flags.append(['/DEBUG'])
    defines.append(['_DEBUG'])
else:
    cl_flags.append(['/O2', '/MT'])

barc_files = Glob('src/tools/barc/*cpp')
client_files = Glob('src/client/*.cpp')
server_files = Glob('src/server/*.cpp')
common_files = Glob('src/common/*.cpp')

rc_file = 'src/client/resource.rc'

barc_objs = Object(barc_files, CPPPATH=['src'], CXXFLAGS=cl_flags, CPPDEFINES=defines)
common_objs = Object(common_files, CPPPATH=common_inc_path, CXXFLAGS=cl_flags, CPPDEFINES=defines)
client_objs = Object(client_files, CPPPATH=client_inc_path, CXXFLAGS=cl_flags, CPPDEFINES=defines)
server_objs = Object(server_files, CPPPATH=server_inc_path, CXXFLAGS=cl_flags, CPPDEFINES=defines)

rc_obj = RES(rc_file)

barc = Program('bin/tools/barc', barc_objs, LINKFLAGS=lk_flags)
client = Program('bin/RIS', client_objs + common_objs + rc_obj, LIBS=client_libs, LIBPATH=client_lib_path, LINKFLAGS=lk_flags)
server = Program('bin/RIS_server', server_objs + common_objs, LIBS=server_libs, LIBPATH=server_lib_path, LINKFLAGS=lk_flags)

Clean(client, 'bin/RIS.ilk')
Clean(client, 'bin/RIS.pdb')

Clean(server, 'bin/RIS_server.ilk')
Clean(server, 'bin/RIS_server.pdb')

dyn_copy = Install('bin/', dyn_libs)
#config_copy = Install('bin/', 'src/client/config.txt')

Depends(client, dyn_copy)
#Depends(client, config_copy)
Default(client)

Precious(client)
Precious(server)

cl = Alias('client', client)
sv = Alias('server', server)
bc = Alias('barc', barc)
Alias('all', [cl, sv, bc])
