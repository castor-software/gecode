/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2015
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

#include <gecode/search/meta/sequential/pbs.hh>

namespace Gecode { namespace Search { namespace Meta { namespace Sequential {

  Stop* 
  stop(const Options& opt) {
    return new PortfolioStop(opt);
  }

  Engine* 
  engine(Engine** slaves, unsigned int n_slaves, 
         const Statistics& stat, Stop* stop, bool best) {
    if (best)
      return new PBS<true>(slaves,n_slaves,stat,
                           static_cast<PortfolioStop*>(stop));
    else
      return new PBS<false>(slaves,n_slaves,stat,
                            static_cast<PortfolioStop*>(stop));
  }

}}}}

#ifdef GECODE_HAS_THREADS

#include <gecode/search/meta/parallel/pbs.hh>

namespace Gecode { namespace Search { namespace Meta { namespace Parallel {

  Stop* 
  stop(Stop* so) {
    return new PortfolioStop(so);
  }

  Engine* 
  engine(Engine** slaves, Stop** stops, unsigned int n_slaves, 
         const Statistics& stat, bool best) {
    if (best)
      return new PBS<CollectBest>(slaves,stops,n_slaves,stat);
    else
      return new PBS<CollectAll>(slaves,stops,n_slaves,stat);
  }

}}}}

#endif

// STATISTICS: search-meta