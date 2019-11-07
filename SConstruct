src_files = Glob('src/*.cpp')

src_objs = Object(src_files, CPPPATH=['src', 'libs/glm/include', 'libs/gli/include', 'libs/glbinding/include', 'libs/glfw/include'], CXXFLAGS=['/EHsc', '/std:c++17'])

game = Program('RIS', src_objs, LIBS=['user32', 'kernel32', 'gdi32', 'opengl32', 'glbinding', 'glbinding-aux', 'glfw3dll'], LIBPATH=['libs/glbinding/lib', 'libs/glfw/lib'])
Default(game)
