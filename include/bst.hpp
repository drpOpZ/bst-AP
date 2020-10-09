#pragma once

#include <iostream> 
#include <utility>   //for std::pair
#include <string>
#include <functional>
#include <exception>


// forward declarations for friend operator<<
template< class K, class V, class cmp>
class Bst;

template< class K, class V, class cmp>
std::ostream& operator<<(std::ostream& , const Bst<K,V,cmp>&);


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

    static Node* select_next_node(Node *&n){
    
        // 3. stay at end if there
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

    /// @brief Base template iterator class
    /// 
    /// Used to define both iterator and const_iterator avoiding
    /// duplication.
    /// @tparam KV Type returned by dereference op
    template<class KV>
    class _iterator{
        
        Node* current;

      public:
        explicit _iterator(Node* n): current(n){};

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

        KV& operator*() const{
            if(current==nullptr){
                throw std::out_of_range("Bst iterator out of range!");
            }
            return current->kv;
        }
    };

    Node* root;

    unsigned int size;
    int height;

    int compute_height_rec(Node* n){
        int hl{0},hr{0};
        if(n->l_child){hl=1+compute_height_rec(n->l_child);}
        if(n->r_child){hr=1+compute_height_rec(n->r_child);}
        return hl>hr?hl:hr;
    }

    void recompute_height(){
        if(size==0){
            height=-1;
        }
        height=compute_height_rec(root);
    }

  public:

    // ctors, dtors -----------------------------------------------------------
    Bst(): root{nullptr}, size{0}, height{-1}{};

    ~Bst(){ if(root){delete root;}}

    // copy/move semantics ----------------------------------------------------

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

    Bst& operator=(Bst&& rhs){
        if(this != &rhs){
            if(root){delete root;}

            root = rhs.root;
            size = rhs.size;
            height = rhs.height;
            if(root){
                if(root->l_child){root->l_child->parent = root;}
                if(root->r_child){root->r_child->parent = root;}
            }
            rhs.root=nullptr;
            rhs.size=0;
            rhs.height=-1;
        }
        return *this;
    }

    Bst(const Bst& bst):
        root{bst.root?new Node{*bst.root}:nullptr},
        size{bst.size},
        height{bst.height}{};

    Bst& operator=(const Bst& rhs){
        if(this != &rhs){
            if(root){delete root;}
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
    /// @param vctorargs        values forwarded to V ctor
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

    V& operator[](const K& key){
        auto cp{key};
        return (*this)[std::move(cp)];
    }

  private:
    void erase_node(Node* n){

        // case 0: key not present
        if(n==nullptr){
            return;
        }

        Node** parent_child{nullptr};
        if(n->parent){
            parent_child= (n==n->parent->l_child)?
                           &(n->parent->l_child) : 
                           &(n->parent->r_child);
        }

        // case 1: no children
        if(n->l_child==nullptr && n->r_child == nullptr){
            *parent_child = nullptr;
        }
        // case 2: both children
        else if(n->l_child!=nullptr && n->r_child!=nullptr){
            Node* successor{select_next_node(n)};
            
            //replace n with successor
            Node *n_p{n->parent}, *n_l{n->l_child},*n_r{n->r_child};
            n->l_child = nullptr;
            n->r_child = nullptr;
            delete n;
            n = new Node{successor->kv};
            n->parent = n_p;
            n->l_child = n_l;
            n->r_child = n_r;
            if(parent_child){*parent_child=n;}
            n_l->parent = n;
            n_r->parent = n;

            erase_node(successor);
            return;
        }
        // case 3: one child
        else{
            if(n->l_child){
                if(parent_child){*parent_child = n->l_child;}
                n->l_child->parent = n->parent;
                n->l_child = nullptr;
            }
            else{
                if(parent_child){*parent_child = n->r_child;}
                n->r_child->parent = n->parent;
                n->r_child = nullptr;
            }
        }
        delete n;
        --size;
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

    unsigned int get_size() const {return size;}
    int get_height() const{return height;}
    
    /// @brief Sends string representation of bst to ostream
    /// 
    /// @param os   output stream
    /// @param bst  current object
    /// @return std::ostream& the ostream, to allow chained call
    friend
    std::ostream& operator<< (std::ostream& os, const Bst& bst){
        os<<"size:"<<bst.size<<" height:"<<bst.height<<"\n";
        for (auto& kv:bst){
            os<<"("<<kv.first<<","<<kv.second<<") ";
        }
        return os;
    }

    //--------
    // Balance
    //--------
  private:
    void balance_rec(kvpair**& kvs, Bst& out, int s, int f){
        
        if(f<s){
            return;
        }

        if(s==f){
            out.insert(std::move(std::make_pair( kvs[s]->first, kvs[s]->second)));
        }
        else if(f-s==1){
            out.insert(std::move(std::make_pair(kvs[s]->first,kvs[s]->second)));
            out.insert(std::move(std::make_pair(kvs[f]->first,kvs[f]->second)));
        }
        else{
            int mid= s + (f-s)/2;
            out.insert(std::move(std::make_pair(kvs[mid]->first,kvs[mid]->second)));
            balance_rec(kvs,out,s,mid-1);
            balance_rec(kvs,out,mid+1,f);
        }
    }
  
  public:
    /// @brief Balances the tree
    /// 
    void balance(){

        //TODO: h<=log_2(size) -> return
        if(size<3){return;}

        kvpair** kvs{new kvpair*[size]};
        
        int iii{0};
        for(auto& kv: *this){
            kvs[iii]=&kv;
            ++iii;
        }

        Bst balanced;
        balance_rec(kvs,balanced,0,size-1);
        delete[] kvs;

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