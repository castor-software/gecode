/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2008
 *
 *  Last modified:
 *     $Date$ by $Author$
 *     $Revision$
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

namespace Gecode {

  /**
   * \defgroup FuncMemRegion Region memory management
   *
   * A region provides a handle to temporary memory owned by
   * a space. The memory will be managed in a stack fashion, that is,
   * the memory allocated through a region will be released
   * only after the region is deleted and all other regions
   * created later also have been deleted.
   *
   * In case a memory request cannot be fulfilled from a space's region, 
   * heap memory is allocated and returned to the operating system
   * as soon as the region is deleted.
   *
   * \ingroup FuncMem
   */
  //@{
  /// Handle to region
  class Region {
  private:
    /// Location to space
    Space& home;
    /// Free memory to reset
    size_t free_reset;
    /// Heap information data structure
    class HeapInfo {
    public:
      /// Number of allocated heap blocks
      unsigned int n;
      /// Limit of allocated heap blocks
      unsigned int size;
      /// Pointers to allocated heap blocks (more entries)
      void* blocks[1];
    };
    /**
     * \brief Heap allocation information
     *
     * If NULL, no heap memory has been allocated. If the pointer
     * is marked, it points to a single heap allocated block. Otherwise,
     * it points to a HeapInfo data structure.
     */
    void* hi;
    /// Allocate memory from heap
    GECODE_KERNEL_EXPORT void* heap_alloc(size_t s);
    /// Free memory previously allocated from heap
    GECODE_KERNEL_EXPORT void heap_free(void);
  public:
    /// Initialize region from space
    Region(const Space& home);
    /// \name Typed allocation routines
    //@{
    /** 
     * \brief Allocate block of \a n objects of type \a T from region
     *
     * Note that this function implements C++ semantics: the default
     * constructor of \a T is run for all \a n objects.
     */
    template <class T>
    T* alloc(unsigned int n);
    /** 
     * \brief Delete \a n objects allocated from the region starting at \a b
     *
     * Note that this function implements C++ semantics: the destructor
     * of \a T is run for all \a n objects.
     *
     * Note that the memory is not freed, the only effect is running the
     * destructors.
     */
    template <class T>
    void free(T* b, unsigned int n);
    /**
     * \brief Reallocate block of \a n objects starting at \a b to \a m objects of type \a T from the region
     *
     * Note that this function implements C++ semantics: the copy constructor
     * of \a T is run for all \f$\min(n,m)$\f objects, the default
     * constructor of \a T is run for all remaining 
     * \f$\max(n,m)-\min(n,m)$\f objects, and the destrucor of \a T is
     * run for all \a n objects in \a b.
     *
     * Returns the address of the new block.
     */
    template <class T>
    T* realloc(T* b, unsigned int n, unsigned int m);
    //@}
    /// \name Raw allocation routines
    //@{
    /// Allocate memory from region
    void* ralloc(size_t s);
    /**
     * \brief Free memory previously allocated
     *
     * Note that the memory is only guaranteed to be freed after the
     * region object itself gets deleted.
     */
    void rfree(void* p, size_t s);
    //@}
    /// Return memory
    ~Region(void);
  private:
    /// Allocate memory from heap (disabled)
    static void* operator new(size_t s) throw() { (void) s; return NULL; }
    /// Free memory allocated from heap (disabled)
    static void  operator delete(void* p) { (void) p; };
    /// Copy constructor (disabled)
    Region(const Region& r) : home(r.home) {}
    /// Assignment operator (disabled)
    const Region& operator=(const Region&) { return *this; }
  };
  //@}


  /*
   * Implementation
   *
   */
  forceinline
  Region::Region(const Space& h) 
    : home(const_cast<Space&>(h)), free_reset(home.sra->free), hi(0) {}

  forceinline void*
  Region::ralloc(size_t s) {
    void* p;
    if (home.sra->alloc(s,p))
      return p;
    return heap_alloc(s);
  }

  forceinline void
  Region::rfree(void*, size_t) {}

  forceinline
  Region::~Region(void) {
    home.sra->free = free_reset;
    if (hi != NULL)
      heap_free();
  }
  

  /*
   * Typed allocation routines
   *
   */
  template <class T>
  forceinline T*
  Region::alloc(unsigned int n) {
    T* p = static_cast<T*>(ralloc(sizeof(T)*n));
    for (unsigned int i=n; i--; )
      (void) new (p+i) T();
    return p;
  }

  template <class T>
  forceinline void
  Region::free(T* b, unsigned int n) {
    for (unsigned int i=n; i--; )
      b[i].~T();
    rfree(b,n*sizeof(T));
  }

  template <class T>
  forceinline T*
  Region::realloc(T* b, unsigned int n, unsigned int m) {
    if (n < m) {
      T* p = static_cast<T*>(ralloc(sizeof(T)*m));
      for (unsigned int i=n; i--; )
        (void) new (p+i) T(b[i]);
      for (unsigned int i=n; i<m; i++)
        (void) new (p+i) T();
      free<T>(b,n);
      return p;
    } else {
      free<T>(b+m,m-n);
      return b;
    }
  }

}

// STATISTICS: kernel-other