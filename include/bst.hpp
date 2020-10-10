#pragma once

#include <iostream> 
#include <exception>
#include <utility>      // for std::pair
#include <string>       // for pretty printing
#include <sstream>      // ""
#include <functional>   // for std::less
#include <cmath>        // used in balancing


// forward declarations for friend operator<<
template< class K, class V, class cmp>
class Bst;

template< class K, class V, class cmp>
std::ostream& operator<<(std::ostream& , const Bst<K,V,cmp>&);

std::string& centered(std::string &s,std::size_t size,const char &fill_c=' '){
    if(size>s.length()){
        std::stringstream ss;
        std::string fill_l((size-s.length())/2 + (size-s.length())%2,fill_c);
        std::string fill_r((size-s.length())/2,fill_c);
        ss<<fill_l<<s<<fill_r;
        s = ss.str();
    }
    return s;
}

/// @brief Binary search tree data structure
/// 
/// This template class implements a Binary Search Tree
/// for storing key-value pairs ordered accordingly to a given
/// comparator (defaults to <).
/// 
/// @tparam K   Type of the keys used to order the nodes in the BST
/// @tparam V   Type of the values stored in the nodes
/// @tparam Cmp Comparator class (default: std::less<K>)
template< class K, class V, class cmp = std::less<K> >
class Bst{
    
  public:
    using kvpair = std::pair<const K,V>;

  private:

    /// @brief Tree nodes
    /// 
    /// These make up the actual memory store of the bst.
    /// Node allocation is managed by the enclosing bst class, hence
    /// CHILD DEALLOCATION MUST BE HANDLED MANUALLY by delete_subtree_rec()
    struct Node{
        kvpair kv;

        Node* parent{nullptr};
        Node* l_child{nullptr};
        Node* r_child{nullptr};

        Node(const kvpair& p_kv): kv{p_kv}{};
        Node(kvpair&& p_kv): kv{std::move(p_kv)}{};

        /// @brief Copy ctor
        Node(const Node& node);

        /// @brief Recursively deletes all node's descents
        void delete_subtree_rec();

        /// @brief Destroy the Node object and its descents
        /// 
        ~Node(){delete_subtree_rec();}

    };

    /// @brief Private helper function to go through tree nodes in cmp order.
    ///
    /// Note that cmp ordering is not checked explicitly! Node* n is supposed
    /// to belong to a bst which respects it!
    /// 
    /// @param n        Element of which we'd like to get next in ordering
    /// @return Node*   Next node (may be nullptr)
    static Node* select_next_node(Node *&n) noexcept{
    
        // 3. stay at end (nullptr) if there already
        if(n == nullptr){
            return nullptr;
        }

        Node* target{nullptr};

        // 1. leftmost of r_child subtree
        if(n->r_child){
            
            // go right
            target = n->r_child;
            // then left while possible
            while(target->l_child){
                target = target->l_child;
            }
        }
        //2. first ancestor whose l_child is ancestor
        else if(n->parent){

            // go up while possible and not l_child of parent
            target = n;
            while( (target->parent!=nullptr) &&
                (target != (target->parent->l_child))){
                target = target->parent;
            }
            // .. and up one step (eventually up to end())
            target = target->parent;
        }
        
        return target;
    }

    /// @brief Base template iterator class.
    /// 
    /// Used to define both iterator and const_iterator avoiding
    /// code duplication.
    /// @tparam KV Type returned by dereference op
    template<class KV>
    class _iterator{
        
        Node* current; ///< pointer to current node

      public:
        explicit _iterator(Node* n): current(n){};

        // comparison op: compare current

        bool operator==(const _iterator& rhs) const{return current == rhs.current;}
        bool operator!=(const _iterator& rhs) const{return !(current == rhs.current);}

        /// @brief pre-increment
        /// 
        /// @return _iterator& The incremented _iterator obj
        _iterator& operator++(){
            current = select_next_node(current);
            return *this;
        }
        
        /// @brief post-increment
        ///
        /// @return _iterator Copy of the _iterator before increment 
        _iterator operator++(int){
            _iterator cp{current};
            current = select_next_node(current);
            return cp;
        }

        /// @brief de-refernce op. Gets reference to current->kv
        /// 
        /// @return KV& 
        KV& operator*() const{
            if(current==nullptr){
                throw std::out_of_range("Bst iterator out of range!");
            }
            return current->kv;
        }
    };

    Node* root; ///< holds the root node of the tree

    unsigned int size;
    int height;

    /// @brief Recursive function to compute the height of a subtree given its root pointer
    /// 
    /// @param n    subtree root
    /// @return int 
    int compute_height_rec(Node* n) noexcept{
        int hl{0},hr{0};
        if(n->l_child){hl=1+compute_height_rec(n->l_child);}
        if(n->r_child){hr=1+compute_height_rec(n->r_child);}
        return hl>hr?hl:hr;
    }

    /// @brief Recomputes the height of the tree. To be used when removing nodes.
    /// 
    void recompute_height() noexcept{
        if(size==0){
            height=-1;
            return;
        }
        height=compute_height_rec(root);
    }

  public:

    // ctors, dtors -----------------------------------------------------------
    Bst(): root{nullptr}, size{0}, height{-1}{};

    ~Bst(){ if(root){delete root;}}

    // copy/move semantics ----------------------------------------------------

    /// @brief Move ctor.
    /// 
    /// Basically steals root, leaving moved bst in a valid state.
    /// @param bst bst to steal
    Bst(Bst&& bst):
        root{bst.root},
        size{bst.size},
        height{bst.height}{
            if(root){
                if(root->l_child){root->l_child->parent = root;}
                if(root->r_child){root->r_child->parent = root;}
            }
            bst.root=nullptr;
            bst.size=0;
            bst.height=-1;
        }

    /// @brief Move assignment 
    /// 
    /// Steals root, leaving rhs in valid state.
    ///
    /// @param rhs   bst to steal
    /// @return Bst& *this after steal
    Bst& operator=(Bst&& rhs){

        // Self equality check before doing anything
        if(this != &rhs){

            // clear the tree 
            if(root){delete root;} //TODO: use clear()

            // Copy root and stats
            root = rhs.root;
            size = rhs.size;
            height = rhs.height;

            // steal their children 
            if(root){
                if(root->l_child){root->l_child->parent = root;}
                if(root->r_child){root->r_child->parent = root;}
            }

            // clean rhs
            rhs.root=nullptr;
            rhs.size=0;
            rhs.height=-1;
        }
        return *this;
    }

    /// @brief Deep-copy ctor
    /// 
    /// @param bst BST to copy
    Bst(const Bst& bst):
        root{bst.root?new Node{*bst.root}:nullptr},
        size{bst.size},
        height{bst.height}{};

    /// @brief Deep-copy assignment
    /// 
    /// @param rhs      BST to copy
    /// @return Bst&    *this after copy
    Bst& operator=(const Bst& rhs){
        if(this != &rhs){
            // clear  
            if(root){delete root;} //TODO: use clear

            // Perform the deep copy and also copy stats
            root = rhs.root?new Node{*rhs.root}:nullptr;
            size = rhs.size;
            height = rhs.height;
        }
        return *this;
    }

    // Iterator interface -----------------------------------------------------

    typedef _iterator<kvpair> iterator;
    typedef _iterator<const kvpair> const_iterator;

  private:
    /// @brief base iterator begin method
    /// 
    /// @tparam It - either iterator or const_iterator
    /// @return iterator to smallest (ie leftmost) element 
    template<class It>
    It _begin() const{
        Node* first{root};
        if(first){
            while(first->l_child){
                first = first->l_child;
            }
        }
        return It(first);
    }

    /// @brief base iterator end method
    /// 
    /// @tparam It - either iterator or const_iterator
    /// @return It iterator to nullptr
    template<class It>
    It _end() const{return It(nullptr);}

  public:
    
    inline iterator begin(){ return _begin<iterator>();}
    inline const_iterator begin() const{ return _begin<const_iterator>();}
    inline const_iterator cbegin() const{ return _begin<const_iterator>();}

    inline iterator end(){ return _end<iterator>();}
    inline const_iterator end() const{ return _end<const_iterator>();}
    inline const_iterator cend() const{ return _end<const_iterator>();}

    //---------------
    // Node insertion
    //---------------


    /// @brief Inserts a new node in the tree by moving given key/value pair.
    ///
    /// If given key is already used the tree is left unchanged.
    /// 
    /// @param kv   key/value pair to move
    /// @return std::pair<iterator, bool> iterator to element at given key + if insertion was successful
    std::pair<iterator, bool> insert(kvpair&& kv);

    /// @brief Inserts a new node in the tree by copying given key/value pair.
    ///
    /// If given key is already used the tree is left unchanged.
    /// 
    /// @param kv   key/value pair to copy
    /// @return std::pair<iterator, bool> iterator to element at given key + if insertion was successful
    std::pair<iterator, bool> insert(const kvpair& kv){  
        kvpair kvcopy{kv};
        return insert(std::move(kv));
    }

    /// @brief Inserts a new node in the tree by creating it in place from given args.
    /// 
    /// If given key is already used the tree is left unchanged.
    /// 
    /// @tparam vctorargtypes   argument types of V ctor 
    /// @param key              key value to insert the element at (if not present)
    /// @param vctorargs        values forwarded to V ctor (these are moved!)
    /// @return std::pair<iterator, bool> iterator to element at given key + if insertion was successful
    template< class... vctorargtypes >
    std::pair<iterator, bool> emplace(const K& key, vctorargtypes&&... vctorargs){
        V value{vctorargs...};
        kvpair kv{std::make_pair(key, std::move(value))};
        return insert(std::move(kv));
    }

    //------------
    // Node access
    //------------

  private:
    template<class It>
    It _find(const K& key) const{
        Node* target{root};
        while(target){
            bool gt{cmp()(target->kv.first,key)}, lt{cmp()(key,target->kv.first)};
            if(gt==lt){ break;}
            target = lt? target->l_child : target->r_child;
        }
        return It(target);
    }
  public:

    /// @brief returns an iterator to given key (or to end() if none was found)
    /// 
    /// @param key        key to find
    /// @return iterator  iterator to value found (or end() if key is not present)
    inline iterator find(const K& key){ return _find<iterator>(key);}
    inline const_iterator find(const K& key) const{ return _find<const_iterator>(key);};

    /// @brief returns a r/w reference to value at given key (eventually initializing it).
    /// 
    /// @param key        key of the element to return
    /// @return V&        reference to the element at key (initializes it if not present already)
    V& operator[](K&& key){
        iterator it{find(std::move(key))};
        if(it==end()){
            it = insert(std::move(std::make_pair(key,V()))).first;
        }
        return (*it).second;
    }
    
    /// @brief returns a r/w reference to value at given key (eventually initializing it).
    /// 
    /// @param key        key of the element to return
    /// @return V&        reference to the element at key (initializes it if not present already)
    V& operator[](const K& key){
        auto cp{key};
        return (*this)[std::move(cp)];
    }

  private:
    
    /// @brief Helper method that performs node deletion recursively. BST structure is preserved.
    /// 
    /// @param n node to delete
    void erase_node(Node* n){

        // case 0: key not present. Skip
        if(n==nullptr){
            return;
        }

        Node** parent_child{&root};
        if(n->parent){
            parent_child= (n==n->parent->l_child)?
                           &(n->parent->l_child) : 
                           &(n->parent->r_child);
        }

        // case 1: no children
        if(n->l_child==nullptr && n->r_child == nullptr){
            
            // just update parent
            *parent_child = nullptr;
        }
        // case 2: both children -> recursion and break
        else if(n->l_child!=nullptr && n->r_child!=nullptr){
            Node* successor{select_next_node(n)};
            
            // substitute n->kv with successor's.
            // UGLY BIT HERE!
            
            // get n links
            Node *n_p{n->parent}, *n_l{n->l_child},*n_r{n->r_child};
            
            // rid of n (spare his children)
            n->l_child = nullptr;
            n->r_child = nullptr;
            delete n;

            // replace n and fix links in it...
            n = new Node{successor->kv};
            n->parent = n_p;
            n->l_child = n_l;
            n->r_child = n_r;
            
            // ... in parent ...
            *parent_child=n;
            
            // ... and in children. Note that both are there beause we checked.
            n_l->parent = n;
            n_r->parent = n;

            // recursively erase successor (which is a duplicate of n, except for family)
            erase_node(successor);

            return;
        }
        // case 3: one child -> link parent and child
        else{
            if(n->l_child){
                *parent_child = n->l_child;
                //else{root = n->l_child;}
                n->l_child->parent = n->parent;
                n->l_child = nullptr;
            }
            else{
                *parent_child = n->r_child;
                n->r_child->parent = n->parent;
                n->r_child = nullptr;
            }
        }

        // either case 1 or 2, hence delete node and update tree stats
        delete n;
        --size;
        //if(size==0){root=nullptr;}
        recompute_height();
        return;
    }
  public:

    /// @brief Remove the element at given key (if present) while preserving bst structure.
    /// 
    /// @param key Key of the element to remove
    void erase(const K& key){

        // find node corresponding to key
        Node* n{root};
        while(n){
            bool gt{cmp()(n->kv.first,key)}, lt{cmp()(key,n->kv.first)};
            if(gt==lt){ break;}
            n = lt? n->l_child : n->r_child;
        }
        erase_node(n);
    }

    /// @brief Clears the content of the tree
    /// 
    void clear(){
        if(root){
            delete root;
            root = nullptr;
            size=0;
            height=-1;
        }
    }

    //-------
    // Output
    //-------

    /// @brief Getter for bst size
    /// 
    /// @return unsigned int bst's size
    unsigned int get_size() const noexcept{return size;}

    /// @brief Getter for bst height
    /// 
    /// @return int tree's height
    int get_height() const noexcept{return height;}
    
    //TODO: const this?
    /// @brief Sends string representation of bst to ostream
    /// 
    /// @param os               output stream
    /// @param bst              current object
    /// @return std::ostream&   the ostream, to allow chained call
    friend
    std::ostream& operator<< (std::ostream& os, const Bst& bst){
        os<<"size:"<<bst.size<<" height:"<<bst.height<<"\n";
        for (auto& kv:bst){
            os<<"("<<kv.first<<","<<kv.second<<") ";
        }
        return os;
    }
    
  private:

    std::string kv_to_str(kvpair &kv){
        std::stringstream s;
        s<<kv.first<<":"<<kv.second;
        return s.str();   
    }

    /// @brief Returns the string representation of a node in the form "[key]:[value]"
    /// 
    /// @param n            pointer to the node to print
    /// @param def          default string to use when input is nullptr
    /// @param key_only     if True, the output string is only "[key]"
    /// @return std::string "[key]:[value]" string representation of the node.
    std::string node_to_str(Node* n, std::string def = "X", bool key_only = false){
        if(n==nullptr){return def;}

        std::stringstream ss;
        if(key_only){
            ss<<n->kv.first;
        }
        else{
            ss<< kv_to_str(n->kv);
        }
        return ss.str();
    }

    void populate_nodes_at_depth(Node**& first,Node* n, const int& depth){
        
        if(depth<0){return;}

        if(depth==0){
            *first = n;
        }
        else if(n){
            Node** mid{&first[1<<(depth-1)]};
            populate_nodes_at_depth(first,n->l_child,depth-1);
            populate_nodes_at_depth(mid,n->r_child,depth-1);
        }
        else{
            Node** last{&first[1<<depth]};
            for(Node** ppp{first};ppp<last; ++ppp){
                *ppp = nullptr;
            }
        }
    }

    Node** nodes_at_depth(int depth){
        
        if(depth<0){return nullptr;}

        Node** out{new Node*[1<<depth]};
        populate_nodes_at_depth(out,root,depth);

        return out;
    }

  public:
    void pretty_print(std::ostream &os = std::cout, std::string empty="."){
        
        // single|no node case
        if(height<1){
            os<<node_to_str(root)<<std::endl;
            return;
        }

        std::size_t nrep_size{0};
        for(auto& kv: *this){
            auto nl{kv_to_str(kv).length()};
            if(nrep_size<nl){nrep_size=nl;}
        }
        nrep_size+=2;

        //puff nrep_size to root level size
        nrep_size= nrep_size<<height;
        for(int depth{0}; depth<=height; ++depth){

            auto n_d{nodes_at_depth(depth)};

            for(int iii{0}; iii< (1<<depth); ++iii){
                auto noderep{node_to_str(n_d[iii], empty)};
                os<<centered(noderep,nrep_size);
            }
            os<<std::endl<<std::endl;

            //halve nrep_size
            nrep_size/=2;

            delete[] n_d;
        }

        //
    }

    //--------
    // Balance
    //--------
  private:

    /// @brief Builds a balanced bst by recursive insertion of the middle point
    /// 
    /// @param kvs Sorted array of pointers to kvpairs
    /// @param out Empty bst to be orderly filled with kvpairs
    /// @param s   index of the first kvs element to consider
    /// @param f   index of the last kvs element to consider (length-1 at step 0)
    void balance_rec(kvpair**& kvs, Bst& out, int s, int f){
        
        // check that s<=f
        if(f<s){
            return;
        }

        // case 1: only one element, insert it
        if(s==f){
            out.insert(std::move(std::make_pair( kvs[s]->first, kvs[s]->second)));
        }
        // case 2: two elements insert both (s then f)
        else if(f-s==1){
            out.insert(std::move(std::make_pair(kvs[s]->first,kvs[s]->second)));
            out.insert(std::move(std::make_pair(kvs[f]->first,kvs[f]->second)));
        }
        //case 3: 3+ elements. Insert middlepoint and recursively call on l and right half (mid excluded)
        else{
            int mid{s + (f-s)/2};
            out.insert(std::move(std::make_pair(kvs[mid]->first,kvs[mid]->second)));
            balance_rec(kvs,out,s,mid-1);
            balance_rec(kvs,out,mid+1,f);
        }
    }
  
  public:
    /// @brief Balances the tree.
    /// 
    void balance() {

        // Exit if already balanced, ie if size<4 or height<=ceil(log_2(size))
        if(size<2 || std::log2(size+1)==height+1){return;}

        // Copy ordered addressed of kv pairs in the tree
        kvpair** kvs{new kvpair*[size]};
        int iii{0};
        for(auto& kv: *this){
            kvs[iii]=&kv;
            ++iii;
        }

        // build balanced copy recursively
        Bst balanced;
        balance_rec(kvs,balanced,0,size-1);

        // free resources
        delete[] kvs;

        // substitutes *this with balanced tree
        *this = std::move(balanced);
    }
};


//#############################################################################
//DEFINITIONS
//#############################################################################

//----
//Node
//----

template< class K, class V, class cmp>
void Bst<K,V,cmp>::Node::delete_subtree_rec(){
    if(l_child){
        delete l_child;
        l_child = nullptr;
    }
    if(r_child){
        delete r_child;
        r_child = nullptr;
    }
}

template< class K, class V, class cmp>
Bst<K,V,cmp>::Node::Node(const Node& node):
  kv{node.kv}{
    
    delete_subtree_rec();

    // clone descents (recursive call to copy ctor)
    if(node.l_child){
      l_child = new Node{*node.l_child};
      l_child->parent = this;
    }
    if(node.r_child){
      r_child = new Node{*node.r_child};
      r_child->parent = this;
    }
}


//---------
// iterator
//---------


//----
// bst
//----

template< class K, class V, class cmp>
std::pair<typename Bst< K, V, cmp> ::iterator, bool > Bst<K,V,cmp>::insert(Bst::kvpair&& kv){
    Node* target_parent{root};
    
    // root
    if(target_parent==nullptr){
        root = new Node{kv};
        size=1;
        height=0;
        return std::make_pair(Bst::iterator{root},true);
    }

    int new_height{1};
    Node *target{nullptr};
    while(target_parent){
        // <
        if(cmp()(kv.first,target_parent->kv.first) &&
           !cmp()(target_parent->kv.first, kv.first)){
            if(target_parent->l_child){
                target_parent = target_parent->l_child;
            }
            else{
                target_parent->l_child = new Node{std::move(kv)};
                target_parent->l_child->parent = target_parent;
                target = target_parent->l_child;
                break;
            }
        }
        // >
        else if(!cmp()(kv.first,target_parent->kv.first) &&
                cmp()(target_parent->kv.first, kv.first)){

            if(target_parent->r_child){
                target_parent = target_parent->r_child;
            }
            else{
                target_parent->r_child = new Node{std::move(kv)};
                target_parent->r_child->parent = target_parent;
                target = target_parent->r_child;
                break;
            }
        }
        //=
        else {
            return std::make_pair(Bst::iterator{target_parent},false);
        }
        ++new_height;
    }
    
    // update size and height (if necessary)
    ++size;
    if(height<new_height){ height = new_height;}

    return std::make_pair(Bst::iterator{target},true);
}