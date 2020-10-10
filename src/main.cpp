#include "bst.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>


typedef Bst<int,double> Testbst;

void test_interactive_tour(){
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

    //test copy ctor
    auto cpy{bst};
    cpy = bst;
    std::cout<<"copy\n"<<cpy<<std::endl;

    for(auto& kv:cpy){
        kv.second+=5;
    }
    std::cout<<"original\n"<<bst<<std::endl;
    std::cout<<"copy\n"<<cpy<<std::endl;

    //test mv ctor
    auto mv{std::move(cpy)};
    std::cout<<"moved copy\n"<<mv<<std::endl;
    std::cout<<"copy\n"<<cpy<<std::endl;

    //test find
    int key;
    std::cout<<"Chose key: ";
    std::cin>>key;
    auto f{mv.find(key)};
    if(f!=mv.end()){
        std::cout<<"Found V:"<<(*f).second<<", setting to 0"<<std::endl;
        (*f).second=0;
        std::cout<<"moved copy modified\n"<<mv<<std::endl;
    }
    else{
        std::cout<<"Not found!"<<std::endl;
    }

    //test operator[]
    double val;
    std::cout<<"Chose key and new value";
    std::cin>>key>>val;
    bst[key]=val;
    std::cout<<"original modified\n"<<bst<<std::endl;

    //test erase
    std::cout<<"Chose key to erase";
    std::cin>>key;
    bst.erase(key);
    std::cout<<"original modified\n"<<bst<<std::endl;

    //test balance
    bst.balance();
    std::cout<<"balanced\n"<<bst<<std::endl;
}

#define BEST_D_0 2e100

int* get_random_arr(unsigned int size){
    int* a{new int[size]};
    for(int iii{0};iii<(int)size;++iii){
        a[iii]=iii+1;
    }
    std::random_shuffle(a,&a[size]);
    return a;
}

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

    //test_interactive_tour();
    //test_performance(5,1<<4,1<<15);

    // int x;
    // std::string y{"y"};
    // std::cin>>x;
    // if(x>5){
    //     std::cin.clear();
    //     std::cin.ignore(0u - 1,'\n');
    //     std::cin>>y;
    // }
    // std::cout<<x<<" "<<y<<std::endl;

    //Testbst bst;
    //bst.emplace(1,5.7);
    Testbst bst;
    // int* a{get_random_arr(5)};
    // for(int iii{0};iii<5;++iii){
    //     bst.emplace(a[iii],(double)(a[iii]));
    // }
    // delete[] a;
    for(int iii{0};iii<5;++iii){
        bst.emplace(iii,(double)iii*iii*iii*iii);
    }

    bst.pretty_print(); std::cout<<bst<<std::endl;
    bst.balance();
    bst.pretty_print(); std::cout<<bst<<std::endl;


    return 0;
}