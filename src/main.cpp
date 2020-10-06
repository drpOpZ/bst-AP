#include "bst.hpp"
#include <iostream>

int main(){

    std::cout<<"Hello Bst!"<<std::endl;

    Bst< int, double> bst;
    bst.insert(std::make_pair(5,2.7));

    return 0;
}