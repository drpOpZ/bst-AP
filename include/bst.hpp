#pragma once

#include <iostream> 
#include <utility>   //for std::pair
#include <string>
#include <functional>


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

        const Node* parent;
        Node* l_child;
        Node* r_child;

        Node(const kvpair& p_kv): kv{p_kv}{};
        Node(kvpair&& p_kv): kv{std::move(p_kv)}{};

        /// @brief Recursively deletes all node's descents
        void delete_subtree_rec();

        ~Node();

        // copy/move semantics---------

        /// @brief Move ctor
        Node(Node&& node){

            kv = std::move(node.kv);

            if(node.l_child){
              l_child = node.l_child;
              l_child->parent = this;
            }
            if(node.r_child){
              r_child = node.r_child;
              r_child->parent = this;
            }

        }

        /// @brief Move assignment
        Node& operator=(Node&& rhs){
            delete_subtree_rec();
            kv = std::move(rhs.kv);

            if(rhs.l_child){
              l_child = rhs.l_child;
              l_child->parent = this;
            }
            if(rhs.r_child){
              r_child = rhs.r_child;
              r_child->parent = this;
            }
            return *this;
        }

        /// @brief Copy ctor
        Node(const Node& node);

        /// @brief Copy assignment
        Node& operator=(const Node& rhs);

    };

    Node* root;

    unsigned int size;
    int height;

  public:

    // ctors, dtors -----------------------------------------------------------
    Bst(): root{nullptr}, size{0}, height{-1}{};

    ~Bst();

    // copy/move semantics ----------------------------------------------------

    Bst(Bst&& bst);
    Bst& operator=(Bst&& rhs);

    Bst(const Bst& bst);
    Bst& operator=(const Bst& rhs);

    // Iterator interface -----------------------------------------------------

    class iterator{
        
        Node* current;

        /// @brief Advances current to the next accordingly to the tree order
        /// 
        /// Next node is either (checked in order):
        /// 1. leftmost node of right subtree (including r_child)
        /// 2. first ancestor whose l_child is node or an ancestor
        /// 3. nullptr = end() (stays there if it already is)
        void select_next();

      public:
        explicit iterator(Node* n): current(n){};

        bool operator==(const iterator rhs){return current == rhs->current;}
        bool operator!=(const iterator rhs){return !(current == rhs->current);}

        /// @brief pre-increment
        /// 
        /// @return iterator& The incremented iterator obj
        iterator& operator++(){ select_next(); }
        
        /// @brief post-increment
        ///
        /// @return iterator Copy of the iterator before increment 
        iterator operator++(int){ iterator cp{current}; select_next(); return cp;}

        kvpair operator*(){ return current->kv;}
    };

    class const_iterator{
        //TODO: basically duplicate iterator
    };

  /// @brief Returns the smallest element i.e. the leftmost
  /// 
  /// @return iterator iterator to leftmost element
  iterator begin();
  const const_iterator cbegin() const;

  /// @brief Returns one-past greatest element i.e. nullptr
  /// 
  /// @return iterator iterator to nullptr
  iterator end(){ return iterator(nullptr);}
  const const_iterator cend() const{ return const_iterator(nullptr);}


  //---------------
  // Node insertion
  //---------------

  /// @brief Inserts a new node in the tree by copying given key/value pair.
  ///
  /// If given key is already used the tree is left unchanged.
  /// 
  /// @param kv   key/value pair to copy
  /// @return std::pair<iterator, bool> iterator to element at given key + if insertion was successful
  std::pair<iterator, bool> insert(const kvpair& kv);

  /// @brief Inserts a new node in the tree by moving given key/value pair.
  ///
  /// If given key is already used the tree is left unchanged.
  /// 
  /// @param kv   key/value pair to move
  /// @return std::pair<iterator, bool> iterator to element at given key + if insertion was successful

  std::pair<iterator, bool> insert(kvpair&& kv);

  /// @brief Inserts a new node in the tree by creating it in place from given args.
  /// 
  /// If given key is already used the tree is left unchanged.
  /// 
  /// @tparam vctorargtypes   argument types of V ctor 
  /// @param key              key value to insert the element at (if not present)
  /// @param vctorargs        values forwarded to V ctor
  /// @return std::pair<iterator, bool> iterator to element at given key + if insertion was successful
  template< class... vctorargtypes >
  std::pair<iterator, bool> emplace(const K& key, vctorargtypes&&... vctorargs);

  //------------
  // Node access
  //------------

  /// @brief returns an iterator to given key (or to end() if none was found)
  /// 
  /// @param key        key to find
  /// @return iterator  iterator to value found (or end() if key is not present)
  iterator find(const K& key);
  const_iterator find(const K& key) const;

  /// @brief returns a r/w reference to value at given key (eventually initializing it).
  /// 
  /// @param key        key of the element to return
  /// @return V&        reference to the element at key (initializes it if not present already)
  V& operator[](const K& key);
  V& operator[](K&& key);

  /// @brief Remove the element at given key (if present) while preserving bst structure.
  /// 
  /// @param key Key of the element to remove
  void erase(const K& key);

  /// @brief Clears the content of the tree
  /// 
  void clear();

  //-------
  // Output
  //-------

  /// @brief Sends string representation of bst to ostream
  /// 
  /// @param os   output stream
  /// @param bst  current object
  /// @return std::ostream& the ostream, to allow chained call
  friend
  std::ostream& operator<< <>(std::ostream& os, const Bst& bst);

  //--------
  // Balance
  //--------

  /// @brief Balances the tree
  /// 
  void balance();
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
Bst<K,V,cmp>::Node::~Node(){
    delete_subtree_rec();
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

template< class K, class V, class cmp>
typename Bst<K,V,cmp>::Node& Bst<K,V,cmp>::Node::operator=(const Node& rhs){

    // Identity check
    if(&rhs != this){
      auto cpy{rhs};
      (*this) = std::move(cpy);
    }

    return *this;
}

//---------
// iterator
//---------


//----
// bst
//----

template< class K, class V, class cmp>
Bst<K,V,cmp>::~Bst(){
    if(root){
        delete root;
    }
}

template< class K, class V, class cmp>
std::pair<typename Bst< K, V, cmp> ::iterator, bool > Bst<K,V,cmp>::insert(const Bst::kvpair& kv){
    
    Node *target{root};
    Node *target_parent{nullptr};
    int new_height{0};

    // navigate the tree until insertion point is found 
    while(target){
        // <
        if( cmp()(kv.first,target->kv.first) && ! cmp()(target->kv.first, kv.first)){
            target_parent = target;
            target = target->l_child;
            if(target == nullptr){
                target_parent->l_child = new Node{kv};
                target_parent->l_child->parent = target_parent;
            }
        }
        // >
        else if( cmp()(target->kv.first, kv.first) && ! cmp()(kv.first,target->kv.first)){
            target_parent = target;
            target = target->r_child;
            if(target == nullptr){
                target_parent->r_child = new Node{kv};
                target_parent->r_child->parent = target_parent;
            }
        }
        // == --> no insertion
        else{
            return std::make_pair(Bst::iterator{target},false);
        }
        ++new_height;
    }
    
    // update size and height (if necessary)
    ++size;
    if(height<new_height){ height = new_height;}

    return std::make_pair(Bst::iterator{target},true);
}

template< class K, class V, class cmp>
std::pair<typename Bst< K, V, cmp> ::iterator, bool > Bst<K,V,cmp>::insert(Bst::kvpair&& kv){
    
    Node *target{root}, *target_parent{nullptr};
    int new_height{0};

    // navigate the tree until insertion point is found 
    while(target){
        // <
        if( cmp()(kv.first,target->kv.first) && ! cmp()(target->kv.first, kv.first)){
            target_parent = target;
            target = target->l_child;
            if(target == nullptr){
                target_parent->l_child = new Node{std::move(kv)};
                target_parent->l_child->parent = target_parent;
            }
        }
        // >
        else if( cmp()(target->kv.first, kv.first) && ! cmp()(kv.first,target->kv.first)){
            target_parent = target;
            target = target->r_child;
            if(target == nullptr){
                target_parent->r_child = new Node{std::move(kv)};
                target_parent->r_child->parent = target_parent;
            }
        }
        // == --> no insertion
        else{
            return std::make_pair(Bst::iterator{target},false);
        }
        ++new_height;
    }
    
    // update size and height (if necessary)
    ++size;
    if(height<new_height){ height = new_height;}

    return std::make_pair(Bst::iterator{target},true);
}