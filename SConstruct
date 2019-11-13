cl_flags = ['/EHsc', '/std:c++17']
lk_flags = ['/nologo']

client_inc_path = ['src', 'libs/glm/include', 'libs/gli/include', 'libs/glbinding/include', 'libs/glfw/include', 'libs/soloud/include']
client_lib_path = ['libs/glbinding/lib', 'libs/glfw/lib', 'libs/soloud/lib']
client_libs = ['user32', 'kernel32', 'gdi32', 'opengl32', 'glbinding', 'glfw3dll', 'soloud_static']

common_inc_path = ['src', 'libs/glm/include']

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

client_files = Glob('src/client/*.cpp')
server_files = Glob('src/server/*.cpp')
common_files = Glob('src/common/*.cpp')

common_objs = Object(common_files, CPPPATH=common_inc_path, CXXFLAGS=cl_flags, CPPDEFINES=defines)
client_objs = Object(client_files, CPPPATH=client_inc_path, CXXFLAGS=cl_flags, CPPDEFINES=defines)
server_objs = Object(server_files, CPPPATH=server_inc_path, CXXFLAGS=cl_flags, CPPDEFINES=defines)

client = Program('RIS', client_objs + common_objs, LIBS=client_libs, LIBPATH=client_lib_path, LINKFLAGS=lk_flags)
server = Program('RIS_server', server_objs + common_objs, LIBS=server_libs, LIBPATH=server_lib_path, LINKFLAGS=lk_flags)

Depends(client, server)
Default(client)
