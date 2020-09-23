#installs dependencies
install:
	sudo apt install gcc;\
	sudo apt install libomp-dev;\

#build an execultable named main in folder build/ from main.c in folder src/
build:
	mkdir -p .build;\
	gcc src/main.c -o .build/main -fopenmp;

#cleans build folder
clear:
	rm -rf .build/

#runs main executable in folder build/
run:
	./.build/main
