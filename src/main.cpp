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

void test_interactive(){

    std::cout<<"Welcome to the interactive demo!\n"
               "Please choose an initial bst size (suggested <8): "<<std::flush;
    int usr_size;
    std::cin>>usr_size;

    auto keys{get_random_arr(usr_size)};

    Testbst bst;
    for(auto iii{0};iii<usr_size;++iii){
        auto k{keys[iii]};
        auto v{7.77*iii*k};
        std::cout<<"inserting "<<k<<" "<<v<<"..."<<std::flush;
        auto out{bst.emplace(k,v)};
        
        std::cout<< (out.second?"done! ":"failed! ")<<
            (*out.first).first<<" "<<(*out.first).second<<
            " s:"<<bst.get_size()<<
            " h:"<<bst.get_height()<<std::endl;
    }
    delete[] keys;

    std::cout<<"Here's your randomly generated bst:"<<std::endl;
    bst.pretty_print();
    std::cout<<"Output of \"operator<<\":\n"<<bst<<std::endl;

    auto printmenu=[](){
        std::cout<<"--Interactive Demo Commands--\n"
            " p \t: "    "prints the tree\n"
            " b \t: "    "balances the tree\n"
            " f K\t: "   "finds node with key K:int and prints its value (if found)\n"
            " e K V\t: " "edits/inserts (K:int,V:double) into the tree\n"
            " x K\t: "   "erases node with key K:int\n"
            " c \t: "    "clears the tree\n"
            " h \t: "    "prints this command list\n"
            " q \t: "    "exits the demo\n"
            <<std::endl;
    };
    auto printprompt=[](){
        std::cout<<"Command: "<<std::flush;
    };

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

void test_performance(int trials, int baseN=1, int maxN=(1<<10)){

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

int main(){

    std::cout<<"\nNOTE:Exit the interactive demo to start performance test!\n"<<std::endl;
    test_interactive();
    test_performance(5,1<<4,1<<15);

    return 0;
}