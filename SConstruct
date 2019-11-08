
cl_flags = ['/EHsc', '/std:c++17']

debug = ARGUMENTS.get('debug', 0)
if int(debug):
    cl_flags.append('/Zi')

src_files = Glob('src/*.cpp')

src_objs = Object(src_files, CPPPATH=['src', 'libs/glm/include', 'libs/gli/include', 'libs/glbinding/include', 'libs/glfw/include'], CXXFLAGS=cl_flags)

game = Program('RIS', src_objs, LIBS=['user32', 'kernel32', 'gdi32', 'opengl32', 'glbinding', 'glfw3dll'], LIBPATH=['libs/glbinding/lib', 'libs/glfw/lib'])

Default(game)
