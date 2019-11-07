src_files = Glob('src/*.cpp')

src_objs = Object(src_files, CPPPATH=['src', 'libs/glm/include', 'libs/gli/include', 'libs/glbinding/include', 'libs/glfw/include'], CXXFLAGS=['/EHsc'])

game = Program('RIS', src_objs, LIBS=['glbinding', 'glbinding-aux', 'glfw3dll', 'user32', 'opengl32'], LIBPATH=['libs/glbinding/lib', 'libs/glfw/lib'])

Default(game)
