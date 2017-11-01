src=src
build=build

glfw-bug:
	mkdir -p $(build)
	#broke
	#gcc -fPIC $(src)/glfwbug.cpp -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -lmujoco150 -lglewosmesa -lOSMesa -lGL -l:libglfw.so.3 -o $(build)/glfw-bug

	#works
	#gcc -fPIC $(src)/glfwbug.cpp -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -lmujoco150 -lGL -lglew -lGL -l:libglfw.so.3 -o $(build)/glfw-bug

	#ref
	#g++ -O2 -I../include -L../bin -std=c++11 -mavx basic.cpp -lmujoco150 -lGL -lglew ../bin/libglfw.so.3 -o ../bin/basic

	#g++ -O2 -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -std=c++11 -mavx src/glfwbug.cpp -lmujoco150 -lGL -lglew ${HOME}/.mujoco/mjpro150/bin/libglfw.so.3 -o $(build)/glfw-bug
	#works
	g++ -O2 -std=c++11 -I/home/ethanbro/ogl/external/glfw-3.1.2/include/ -mavx src/glfwbug.cpp -lGL -lGLEW /usr/lib/x86_64-linux-gnu/libglfw.so.3 -o $(build)/glfw-bug

bad-access-bug:
	mkdir -p $(build)
	g++ -O2 -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -std=c++11 -mavx src/badaccessbug.cpp -lmujoco150 -lGL -lglew ${HOME}/.mujoco/mjpro150/bin/libglfw.so.3 -o $(build)/bad-access-bug

clean:
	rm -rf $(build)/
	rm "*.c" "*.so" "video.*" "*.out"

render:
	mkdir -p $(build)
	rm -f rgb.out video.mp4
	g++ -O2 -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -std=c++11 -mavx src/render.cpp -lmujoco150 -lGL -lglew ${HOME}/.mujoco/mjpro150/bin/libglfw.so.3 -o $(build)/render
	./build/render

mujoco:
	mkdir -p $(build)
	gcc -O2 -Ih/ -I/home/ethanbro/mujoco-py/mujoco_py/gl/ -I/home/ethanbro/.mujoco/mjpro150/include -L${HOME}/.mujoco/mjpro150/bin -std=c++11 -mavx src/eglshim.c  -lmujoco150 -lGL -lglew ${HOME}/.mujoco/mjpro150/bin/libglfw.so.3 
	#src/mujoco_py.cpp -lmujoco150 -lGL -lglew ${HOME}/.mujoco/mjpro150/bin/libglfw.so.3 -o $(build)/mujoco
	#./build/mujoco
