src=src
build=build

all:
	mkdir -p $(build)
	#broke
	#gcc -fPIC $(src)/glfwbug.cpp -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -lmujoco150 -lglewosmesa -lOSMesa -lGL -l:libglfw.so.3 -o $(build)/glfw-bug

	gcc -fPIC $(src)/glfwbug.cpp -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -lmujoco150 -lGL -lglew -lGL -l:libglfw.so.3 -o $(build)/glfw-bug

	#ref
	#g++ -O2 -I../include -L../bin -std=c++11 -mavx basic.cpp -lmujoco150 -lGL -lglew ../bin/libglfw.so.3 -o ../bin/basic

	#works
	#g++ -O2 -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -std=c++11 -mavx src/glfwbug.cpp -lmujoco150 -lGL -lglew ${HOME}/.mujoco/mjpro150/bin/libglfw.so.3 -o $(build)/glfw-bug

clean:

	rm -rf $(build)/
	rm "*.c" "*.so"


