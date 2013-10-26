/*
 * Copyright 2013 Saminda Abeyruwan (saminda@cs.miami.edu)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * HeaderTest.h
 *
 *  Created on: Dec 18, 2012
 *      Author: sam
 */

#ifndef HEADERTEST_H_
#define HEADERTEST_H_

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstring>
#include <map>
#include <set>
#include <fstream>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <limits>

#include "Vector.h"
#include "Trace.h"
#include "Projector.h"
#include "ControlAlgorithm.h"
#include "Representation.h"
#include "MCar2D.h"
#include "Simulator.h"
#include "StateGraph.h"

using namespace std;
using namespace RLLib;

// Testing framework
class RLLibTestCase
{
  public:
    RLLibTestCase()
    {
    }
    virtual ~RLLibTestCase()
    {
    }
    virtual void run() =0;
    virtual const char* getName() const =0;
};

/** Test macro **/
#define RLLIB_TEST(NAME)                            \
class NAME;                                         \
class NAME##Base : public RLLibTestCase             \
{                                                   \
  public:                                           \
    NAME##Base() :  RLLibTestCase() {}              \
    virtual ~NAME##Base() {}                        \
    const char* getName() const { return #NAME ; }  \
};                                                  \

class RLLibTestRegistry
{
  protected:
    std::map<string, RLLibTestCase*> registry;

  public:

    typedef std::map<string, RLLibTestCase*>::iterator iterator;
    typedef std::map<string, RLLibTestCase*>::const_iterator const_iterator;

    iterator begin()
    {
      return registry.begin();
    }

    const_iterator begin() const
    {
      return registry.begin();
    }

    iterator end()
    {
      return registry.end();
    }

    const_iterator end() const
    {
      return registry.end();
    }

    iterator find(const string& key)
    {
      return registry.find(key);
    }

    const_iterator find(const string& key) const
    {
      return registry.find(key);
    }

    const unsigned int dimension() const
    {
      return registry.size();
    }

    static RLLibTestRegistry* getInstance();
    static void deleteInstance();
    static void registerInstance(RLLibTestCase* testCase);

  protected:
    RLLibTestRegistry();
    ~RLLibTestRegistry();
    RLLibTestRegistry(RLLibTestRegistry const&);
    RLLibTestRegistry& operator=(RLLibTestRegistry const&);

  private:
    static RLLibTestRegistry* instance;
};

template<class T>
class RLLibTestCaseLoader
{
  protected:
    T* theInstance;
  public:
    RLLibTestCaseLoader() :
        theInstance(new T())
    {
      RLLibTestRegistry::registerInstance(theInstance);
    }

    ~RLLibTestCaseLoader()
    {
      delete theInstance;
    }
};

/** Test make macro **/
#define RLLIB_TEST_MAKE(NAME) \
  RLLibTestCaseLoader<NAME> __theTestcaseLoader##NAME;

/** Test generic code **/
typedef SparseVector<double> SVecDoubleType;
typedef SparseVector<float> SVecFloatType;
typedef DenseVector<float> DVecFloatType;
typedef Trace<double> TraceDoubleType;
typedef ATrace<double> ATraceDoubleType;
typedef RTrace<double> RTraceDoubleType;
typedef AMaxTrace<double> AMaxTraceDoubleType;
typedef MaxLengthTrace<double> MaxLengthTraceDoubleType;

enum TraceEnum
{
  ATraceDouble = 0,
  RTraceDouble,
  AMaxTraceDouble,
  MaxLengthTraceDouble,
  TraceEnumDimension
};

// Common assertion tests

class VectorsTestsUtils
{
  private:
    static bool checkSparseVectorConsistency(const SVecDoubleType& v)
    {
      const int* indexesPositions = v.getIndexesPosition();
      const int* activeIndexes = v.getActiveIndexes();
      int nbActiveCounted = 0;
      bool positionChecked[v.nbActiveEntries()];
      std::fill(positionChecked, positionChecked + v.nbActiveEntries(), false);
      for (int index = 0; index < v.dimension(); index++)
      {
        const int position = indexesPositions[index];
        if (position == -1)
          continue;
        if (nbActiveCounted >= v.nbActiveEntries())
          return false;
        if (positionChecked[position])
          return false;
        if (activeIndexes[position] != index)
          return false;
        positionChecked[position] = true;
        ++nbActiveCounted;
      }
      return nbActiveCounted != v.nbActiveEntries() ? false : true;
    }

  public:
    static bool checkConsistency(const SVecDoubleType& v)
    {
      return checkSparseVectorConsistency(v);
    }

    static bool checkVectorEquals(const SVecDoubleType& a, const SVecDoubleType& b, double margin)
    {
      if (a.dimension() != b.dimension())
        return false;
      for (int i = 0; i < a.dimension(); i++)
      {
        double diff = fabs(a.getEntry(i) - b.getEntry(i));
        if (diff > margin)
          return false;
      }
      return true;
    }

    static bool checkValue(const SVecDoubleType& v)
    {
      const double* values = v.getValues();
      for (int position = 0; position < v.nbActiveEntries(); position++)
      {
        if (!Boundedness::checkValue(values[position]))
          return false;
      }
      return true;
    }

};

class Assert
{
  public:
    static void checkVectorEquals(const SVecDoubleType& a, const SVecDoubleType& b)
    {
      assert(VectorsTestsUtils::checkConsistency(a));
      assert(VectorsTestsUtils::checkConsistency(b));
      assert(VectorsTestsUtils::checkVectorEquals(a, b, numeric_limits<float>::epsilon()));
    }

    static void checkVectorEquals(const SVecDoubleType& a, const SVecDoubleType& b, double margin)
    {
      assert(VectorsTestsUtils::checkConsistency(a));
      assert(VectorsTestsUtils::checkConsistency(b));
      assert(VectorsTestsUtils::checkVectorEquals(a, b, margin));
    }

    static void checkConsistency(const SVecDoubleType& v)
    {
      assert(VectorsTestsUtils::checkConsistency(v));
    }

    static void checkValue(const SVecDoubleType& v)
    {
      assert(VectorsTestsUtils::checkValue(v));
    }

    static void pass(const bool& condition)
    {
      assert(condition);
    }

    static void fail(const bool& condition)
    {
      assert(!condition);
    }

    template<class T>
    static void equals(const T& a, const T& b)
    {
      assert(a == b);
    }
};

class Arrays
{
  public:
    template<class T, unsigned int N>
    static inline unsigned int length(T (&pX)[N])
    {
      return N;
    }
};

#endif /* HEADERTEST_H_ */
