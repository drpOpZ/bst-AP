#include "bst.hpp"
#include "bst_tests.hpp"

#include <iostream>


int main(){

    std::cout<<"\nNOTE:Exit the interactive demo to start performance test!\n"<<std::endl;
    test_interactive();
    test_performance(5,1<<4,1<<15);

    return 0;
}