#include "bst.hpp"
#include <iostream>

int main(){

    Bst< int, double> bst;
    std::cout<<"Hello Bst! s:"<<bst.get_size()<<" h:"<<bst.get_height()<<std::endl;


    for(int iii{9}; (iii%13)!=0; iii = (iii+7)%13){
        auto k = iii;
        auto v = 2.7*iii;
        std::cout<<"inserting "<<k<<" "<<v<<"..."<<std::flush;

        //test move-insert
        //auto out {bst.insert(std::make_pair(iii,2.7*iii))};
        auto out {bst.emplace(iii,2.7*iii)};
        

        std::cout<< (out.second?"done! ":"failed! ")<<
            (*out.first).first<<" "<<(*out.first).second<<
            " s:"<<bst.get_size()<<
            " h:"<<bst.get_height()<<std::endl;
    }

    //test copy insert and check repetition
    auto cpkv{std::make_pair(77,7.77)};
    cpkv.second+=0.01;
    std::cout<<"copy insert success:"<<bst.insert(cpkv).second<<std::endl;
    cpkv.second+=9.01;
    std::cout<<"duplcate insert success:"<<bst.insert(cpkv).second<<std::endl;

    std::cout<<"original\n"<<bst<<std::endl;

    auto cpy{bst};
    std::cout<<"copy\n"<<cpy<<std::endl;

    for(auto& kv:cpy){
        kv.second+=5;
    }
    std::cout<<"original\n"<<bst<<std::endl;
    std::cout<<"copy\n"<<cpy<<std::endl;

    return 0;
}