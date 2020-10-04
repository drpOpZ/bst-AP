#pragma once

#include <iostream> 
#include <utility>   //for std::pair
#include <string>


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
    
    using kvpair = std::pair<const K,V>;

    struct Node{
        kvpair kv;

        Node* parent;
        Node* l_child;
        Node* r_child;
    };

    Node* root;

    unsigned int size;
    unsigned int height;

  public:

    // ctors, dtors -----------------------------------------------------------
    Bst(): root(nullptr){};

    ~Bst();


    // copy/move semantics ----------------------------------------------------

    Bst(const Bst& bst);
    Bst& operator=(const Bst* rhs);

    Bst(Bst&& bst);
    Bst& operator=(Bst&& rhs);

    // Iterator interface -----------------------------------------------------

    class iterator{
        
        Node* current;

        /// @brief Advances current to the next accordingly to the tree order
        /// 
        /// Next node is either (checked in order):
        /// 1. leftmost node of right subtree (including r_child)
        /// 2. parent
        /// 3. nullptr
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

    };

  iterator begin();
  iterator end();
  const const_iterator cbegin() const;
  const const_iterator cend() const;


  // Node insertion

  std::pair<iterator, bool> insert(const kvpair& kv);
  std::pair<iterator, bool> insert(kvpair&& kv);

  template< class... vctorargtypes >
  std::pair<iterator, bool> emplace(vctorargtypes&&... vctorargs);


  // Node access

  iterator find(const K& key);
  const_iterator find(const K& key) const;

  V& operator[](const K& key);
  V& operator[](K&& key);

  /// @brief Remove the element at given key (if present) while preserving bst structure.
  /// 
  /// @param key Key of the element to remove
  void erase(const K& key);

  /// @brief Clear the content of the tree
  /// 
  void clear();


  // Output

  /// @brief Sends string representation of bst to ostream
  /// 
  /// @param os   output stream
  /// @param bst  current object
  /// @return std::ostream& the ostream, to allow chained call
  friend
  std::ostream& operator<< <>(std::ostream& os, const Bst& bst);


  // Balance

  /// @brief Balances the tree
  /// 
  void balance();
};