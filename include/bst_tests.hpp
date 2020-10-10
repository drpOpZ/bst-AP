#pragma once

#include "bst.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>

typedef Bst<int,double> Testbst;


int* get_random_arr(unsigned int size){
    int* a{new int[size]};
    for(int iii{0};iii<(int)size;++iii){
        a[iii]=iii+1;
    }
    std::random_shuffle(a,&a[size]);
    return a;
}
/// @brief Runs an interactive sandbox test that features a simple command prompt to play with the BST.
/// 
void test_interactive(){

    // Handy lambdas
    auto rebuild_rnd_tree = [](Testbst& bst,std::size_t s, bool verbose){
        bst.clear();
        auto keys{get_random_arr(s)};

        for(auto iii{0};iii<(int)s;++iii){
            auto k{keys[iii]};
            auto v{7.77*iii*k};
            if(verbose){
                std::cout<<"inserting "<<k<<" "<<v<<"..."<<std::flush;
            }
            auto out{bst.emplace(k,v)};
            if(verbose){
                std::cout<< (out.second?"done! ":"failed! ")<<
                    (*out.first).first<<" "<<(*out.first).second<<
                    " s:"<<bst.get_size()<<
                    " h:"<<bst.get_height()<<std::endl;
            }
        }
        delete[] keys;

    };

    auto printmenu=[](){
        std::cout<<"--Interactive Demo Commands--\n"
            " p \t: "    "prints the tree\n"
            " b \t: "    "balances the tree\n"
            " f K\t: "   "finds node with key K:int and prints its value (if found)\n"
            " e K V\t: " "edits/inserts (K:int,V:double) into the tree\n"
            " x K\t: "   "erases node with key K:int\n"
            " c \t: "    "clears the tree\n"
            " r N\t: "   "dump the tree and generate a new random one of size N:unsiged int\n"
            " h \t: "    "prints this command list\n"
            " q \t: "    "exits the demo\n"
            <<std::endl;
    };
    auto printprompt=[](){
        std::cout<<"Command: "<<std::flush;
    };

    // Test start!

    std::cout<<"Welcome to the interactive demo!\n"
               "It is suggested to run this demo on a large console for proper printing.\n\n"
               "DISCLAIMER: This is a VERY basic interactive prompt! There's no input sanitizing and attempts to break\n"
               "it will easily succede. Use at your own risk!\n\n"
               "Please choose an initial bst size (suggested <8): "<<std::flush;


    // First build
    std::size_t usr_size;
    std::cin>>usr_size;

    Testbst bst;
    rebuild_rnd_tree(bst,usr_size,true);


    std::cout<<"Here's your randomly generated bst:"<<std::endl;
    bst.pretty_print();
    std::cout<<"Output of \"operator<<\":\n"<<bst<<std::endl;


    std::cout<<"Now, what do we do to it?"<<std::endl;
    printmenu();
    char cmd;
    do{
        printprompt();
        std::cin>>cmd;

        if(cmd=='p'){
            bst.pretty_print();
            std::cout<<bst<<std::endl;
        }
        else if(cmd=='b'){
            bst.balance();
            std::cout<<"Tree balanced!"<<std::endl;
        }
        else if(cmd=='f'){
            int k;
            std::cin>>k;
            auto it{bst.find(k)};
            if(it==bst.end()){
                std::cout<<"Key \""<<k<<"\" not found!"<<std::endl;
            }
            else{
                std::cout<<"Key found! "<<k<<":"<<(*it).second<<std::endl;
            }
        }
        else if(cmd=='e'){
            int k; double v;
            std::cin>>k>>v;
            bst[k]=v;
            std::cout<<k<<":"<<v<<" set."<<std::endl;
        }
        else if(cmd=='x'){
            int k;
            std::cin>>k;
            bst.erase(k);
            std::cout<<"Erase attempted"<<std::endl;
        }
        else if(cmd=='c'){
            bst.clear();
            std::cout<<"Bst cleared!"<<std::endl;
        }
        else if(cmd=='r'){
            std::cin>>usr_size;
            rebuild_rnd_tree(bst,usr_size,false);
            std::cout<<"Bst recreated!"<<std::endl;
        }
        else if(cmd=='q'){ /*pass*/ }
        else {
            if(cmd!='h'){
                std::cout<<"That's not a command! Try one of these:\n";
            }
            printmenu();
        }

        std::cin.clear();
        std::cin.ignore(0u - 1,'\n');

    }while(cmd!='q');
    std::cout<<"Thanks for running the demo :)\nBye bye!"<<std::endl;
}

#define BEST_D_0 2e100

/// @brief  Runs a series of repeated tests and prints the timing results on std::out.
///         Each test is performed on three different BSTs:
///         - 1->N      Obtained by inserting numbers from 1 to N sequentially (a huge right arm)
///         - N->1      The opposite (A huge left arm)
///         - random    Randomly inserts nodes with keys 1...N
///         
///         A fresh bst is created and populated at each trial to limit memory reusage.
///
///         The tests preformed are the following:
///         1. Build            BST is filled with elements
///         2. Copy             BST is deep-copied using copy-ctor
///         3. Move             BST is move initialized
///         4. Balance          BST is balanced
///         5. Traversal        BST is traversed with iterator
///         6. Arbitrary access BST nodes are accessed 1..N with operator[]
///         7. Clear            BST is cleared
///         8. Arbitrary erase  All nodes are removed in a random order (same for all trees at each routine)
///
/// @param trials   Number of trials that each test will be repeated to compute averge score.
/// @param baseN    Starting size of the tested BSTs. Following routines duplicate it (e.g. 2->4->8...)
/// @param maxN     Maximum size (<=) of the tested bst.
void test_performance(int trials=5, int baseN=1<<4, int maxN=(1<<15)){

    auto start{std::chrono::steady_clock::now()},
         end{std::chrono::steady_clock::now()};
    std::chrono::duration<double> trial_secs;
    double avg{0}, worst{-1}, best{BEST_D_0}, acc{0};

    
    auto new_routine = [&](){
        avg=0; worst=-1; best=BEST_D_0; acc=0;
    };
    
    auto finalize_trial =[&](){
        trial_secs = end-start;
        if(trial_secs.count()<best){best=trial_secs.count();}
        if(trial_secs.count()>worst){worst=trial_secs.count();}
        acc+=trial_secs.count();
    };

    // save cout flags to restore them later
    std::ios_base::fmtflags defflags( std::cout.flags() );
    std::cout<<std::setprecision(15);

    
    //--------------------------------
    // Build test
    //--------------------------------
    std::cout<<"Build test"<<std::endl;
    std::cout<< std::left
             <<std::setw(16)<<"N"
             <<std::setw(16)<<"Tree"
             <<std::setw(16)<<"AVG"
             <<std::setw(16)<<"worst"
             <<std::setw(16)<<"best"
             <<std::endl;
    for(int N{baseN};N<maxN;N=(N<<1)){
        

        //1->N
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            
            start = std::chrono::steady_clock::now();
            for(int iii{1};iii<=N;++iii){
                bst.emplace(iii,(double)iii);
            }
            end = std::chrono::steady_clock::now();
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<N
                 <<std::setw(16)<<"1->N"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //N->1
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            
            start = std::chrono::steady_clock::now();
            for(int iii{N};iii>=1;--iii){
                bst.emplace(iii,(double)iii);
            }
            end = std::chrono::steady_clock::now();
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"N->1"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //random
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            int* a{get_random_arr(N)};
            
            start = std::chrono::steady_clock::now();
            for(int iii{0};iii<N;++iii){
                bst.emplace(a[iii],(double)(a[iii]));
            }
            end = std::chrono::steady_clock::now();
            delete[] a;
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"rnd"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
    }
 
    //--------------------------------
    // Copy test
    //--------------------------------
    std::cout<<"Copy test"<<std::endl;
    std::cout<< std::left
             <<std::setw(16)<<"N"
             <<std::setw(16)<<"Tree"
             <<std::setw(16)<<"AVG"
             <<std::setw(16)<<"worst"
             <<std::setw(16)<<"best"
             <<std::endl;
    for(int N{baseN};N<maxN;N=(N<<1)){
        
        //1->N
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{1};iii<=N;++iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            auto cp{bst};
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<N
                 <<std::setw(16)<<"1->N"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //N->1
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{N};iii>=1;--iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            auto cp{bst};
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"N->1"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //random
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            int* a{get_random_arr(N)};
            for(int iii{0};iii<N;++iii){
                bst.emplace(a[iii],(double)(a[iii]));
            }
            
            start = std::chrono::steady_clock::now();
            auto cp{bst};
            end = std::chrono::steady_clock::now();

            delete[] a;
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"rnd"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
    }
    
    //--------------------------------
    // Move test
    //--------------------------------
    std::cout<<"Move test"<<std::endl;
    std::cout<< std::left
             <<std::setw(16)<<"N"
             <<std::setw(16)<<"Tree"
             <<std::setw(16)<<"AVG"
             <<std::setw(16)<<"worst"
             <<std::setw(16)<<"best"
             <<std::endl;
    for(int N{baseN};N<maxN;N=(N<<1)){
        

        //1->N
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{1};iii<=N;++iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            auto mv{std::move(bst)};
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<N
                 <<std::setw(16)<<"1->N"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //N->1
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{N};iii>=1;--iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            auto mv{std::move(bst)};
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"N->1"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //random
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            int* a{get_random_arr(N)};
            for(int iii{0};iii<N;++iii){
                bst.emplace(a[iii],(double)(a[iii]));
            }
            
            start = std::chrono::steady_clock::now();
            auto mv{std::move(bst)};
            end = std::chrono::steady_clock::now();

            delete[] a;
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"rnd"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
    }

    //--------------------------------
    // Balance test
    //--------------------------------
    std::cout<<"Balance test"<<std::endl;
    std::cout<< std::left
             <<std::setw(16)<<"N"
             <<std::setw(16)<<"Tree"
             <<std::setw(16)<<"AVG"
             <<std::setw(16)<<"worst"
             <<std::setw(16)<<"best"
             <<std::endl;
    for(int N{baseN};N<maxN;N=(N<<1)){
        

        //1->N
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{1};iii<=N;++iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            bst.balance();
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<N
                 <<std::setw(16)<<"1->N"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //N->1
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{N};iii>=1;--iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            bst.balance();
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"N->1"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //random
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            int* a{get_random_arr(N)};
            for(int iii{0};iii<N;++iii){
                bst.emplace(a[iii],(double)(a[iii]));
            }
            
            start = std::chrono::steady_clock::now();
            bst.balance();
            end = std::chrono::steady_clock::now();

            delete[] a;
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"rnd"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
    }
    
    
    //--------------------------------
    // Traversal test
    //--------------------------------
    std::cout<<"Traversal test"<<std::endl;
    std::cout<< std::left
             <<std::setw(16)<<"N"
             <<std::setw(16)<<"Tree"
             <<std::setw(16)<<"AVG"
             <<std::setw(16)<<"worst"
             <<std::setw(16)<<"best"
             <<std::endl;
    for(int N{baseN};N<maxN;N=(N<<1)){
        

        //1->N
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{1};iii<=N;++iii){
                bst.emplace(iii,(double)iii);
            }

            auto it{bst.begin()};
            start = std::chrono::steady_clock::now();
            while(it!=bst.end()){++it;}
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<N
                 <<std::setw(16)<<"1->N"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //N->1
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{N};iii>=1;--iii){
                bst.emplace(iii,(double)iii);
            }

            auto it{bst.begin()};
            start = std::chrono::steady_clock::now();
            while(it!=bst.end()){++it;}
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"N->1"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //random
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            int* a{get_random_arr(N)};
            for(int iii{0};iii<N;++iii){
                bst.emplace(a[iii],(double)(a[iii]));
            }
                        
            auto it{bst.begin()};
            start = std::chrono::steady_clock::now();
            while(it!=bst.end()){++it;}
            end = std::chrono::steady_clock::now();

            delete[] a;
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"rnd"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
    }

    
    //--------------------------------
    // Arbitrary access test
    //--------------------------------
    std::cout<<"Arbitrary access test"<<std::endl;
    std::cout<< std::left
             <<std::setw(16)<<"N"
             <<std::setw(16)<<"Tree"
             <<std::setw(16)<<"AVG"
             <<std::setw(16)<<"worst"
             <<std::setw(16)<<"best"
             <<std::endl;
    for(int N{baseN};N<maxN;N=(N<<1)){
        

        //1->N
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{1};iii<=N;++iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            for(int iii{1};iii<=N;++iii){
                bst[iii];
            }
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<N
                 <<std::setw(16)<<"1->N"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //N->1
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{N};iii>=1;--iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            for(int iii{1};iii<=N;++iii){
                bst[iii];
            }
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"N->1"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //random
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            int* a{get_random_arr(N)};
            for(int iii{0};iii<N;++iii){
                bst.emplace(a[iii],(double)(a[iii]));
            }
            
            start = std::chrono::steady_clock::now();
            for(int iii{1};iii<=N;++iii){
                bst[iii];
            }
            end = std::chrono::steady_clock::now();

            delete[] a;
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"rnd"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
    }

    
    //--------------------------------
    // Clear
    //--------------------------------
    std::cout<<"Clear test"<<std::endl;
    std::cout<< std::left
             <<std::setw(16)<<"N"
             <<std::setw(16)<<"Tree"
             <<std::setw(16)<<"AVG"
             <<std::setw(16)<<"worst"
             <<std::setw(16)<<"best"
             <<std::endl;
    for(int N{baseN};N<maxN;N=(N<<1)){
        

        //1->N
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{1};iii<=N;++iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            bst.clear();
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<N
                 <<std::setw(16)<<"1->N"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //N->1
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{N};iii>=1;--iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            bst.clear();
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"N->1"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //random
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            int* a{get_random_arr(N)};
            for(int iii{0};iii<N;++iii){
                bst.emplace(a[iii],(double)(a[iii]));
            }
            
            start = std::chrono::steady_clock::now();
            bst.clear();
            end = std::chrono::steady_clock::now();

            delete[] a;
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"rnd"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
    }
    
    
    //--------------------------------
    // Arbitrary erase test
    //--------------------------------
    std::cout<<"Arbitrary erase test"<<std::endl;
    std::cout<< std::left
             <<std::setw(16)<<"N"
             <<std::setw(16)<<"Tree"
             <<std::setw(16)<<"AVG"
             <<std::setw(16)<<"worst"
             <<std::setw(16)<<"best"
             <<std::endl;
    for(int N{baseN};N<maxN;N=(N<<1)){

        int* erase_ord{get_random_arr(N)};
        
        //1->N
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{1};iii<=N;++iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            for(int iii{0};iii<N;++iii){
                bst.erase(erase_ord[iii]);
            }
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<N
                 <<std::setw(16)<<"1->N"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //N->1
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            for(int iii{N};iii>=1;--iii){
                bst.emplace(iii,(double)iii);
            }

            start = std::chrono::steady_clock::now();
            for(int iii{0};iii<N;++iii){
                bst.erase(erase_ord[iii]);
            }
            end = std::chrono::steady_clock::now();
            
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"N->1"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;
        
        //random
        new_routine();
        for(int ttt{0};ttt<trials;++ttt){

            Testbst bst;
            int* a{get_random_arr(N)};
            for(int iii{0};iii<N;++iii){
                bst.emplace(a[iii],(double)(a[iii]));
            }
            
            start = std::chrono::steady_clock::now();
            for(int iii{0};iii<N;++iii){
                bst.erase(erase_ord[iii]);
            }
            end = std::chrono::steady_clock::now();

            delete[] a;
            finalize_trial();
        }
        avg=acc/trials;
        std::cout<<std::setw(16)<<'"'
                 <<std::setw(16)<<"rnd"
                 <<std::setw(16)<<avg
                 <<std::setw(16)<<worst
                 <<std::setw(16)<<best
                 <<std::endl;


        delete[] erase_ord;
    }


    //--------------------------------
    //--------------------------------
    
    
    //reset cout flags
    std::cout.flags( defflags );
}