bst_test: src/main.cpp src/bst.cpp include/bst.hpp
	g++ -o bst_test src/main.cpp -I include -Wall -Wextra -std=c++14 
