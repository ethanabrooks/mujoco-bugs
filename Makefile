src=src
build=build

all:
	mkdir -p $(build)
	gcc -fPIC $(src)/glfwbug.cpp -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -lmujoco150 -lglewosmesa -lOSMesa -lGL -l:libglfw.so.3 -o $(build)/glfw-bug
clean:
	rm -rf $(build)/
	rm "*.c" "*.so"


