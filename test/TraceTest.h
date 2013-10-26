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
 * TraceTest.h
 *
 *  Created on: May 12, 2013
 *      Author: sam
 */

#ifndef TRACETEST_H_
#define TRACETEST_H_

#include "HeaderTest.h"

RLLIB_TEST(TraceTest)

class TraceTest: public TraceTestBase
{
  protected:
    vector<TraceDoubleType*> traces;
    SVecDoubleType* s01;
    SVecDoubleType* s02;

  public:
    TraceTest() :
        s01(0), s02(0)
    {
      s01 = new SVecDoubleType(100, 2);
      s02 = new SVecDoubleType(100, 2);

      s01->setEntry(1, 1.0);
      s01->setEntry(2, 1.0);
      s01->setEntry(3, 1.0);

      s02->setEntry(4, 1.0);
      s02->setEntry(5, 1.0);
      s02->setEntry(6, 1.0);
    }

    ~TraceTest()
    {
      delete s01;
      delete s02;

      for (vector<TraceDoubleType*>::iterator iter = traces.begin(); iter != traces.end(); ++iter)
        delete *iter;
      traces.clear();
    }

  private:

    TraceDoubleType* newTrace(const TraceEnum& type)
    {
      TraceDoubleType* trace = 0;
      switch (type)
      {
      case ATraceDouble:
      {
        trace = new ATraceDoubleType(100);
        break;
      }
      case RTraceDouble:
      {
        trace = new RTraceDoubleType(100);
        break;
      }
      case AMaxTraceDouble:
      {
        trace = new AMaxTraceDoubleType(100, 1e-8, 5.0);
        break;
      }
      default:
      {
        cerr << "ERROR!" << endl;
        exit(-1);
      }
      }
      traces.push_back(trace);
      return trace;
    }

    TraceDoubleType* newTrace(TraceDoubleType* trace, const int& size)
    {
      TraceDoubleType* wrapper = new MaxLengthTraceDoubleType(trace, size);
      traces.push_back(wrapper);
      return wrapper;
    }

    void testTraceWithDiscounting(const double& lambda, TraceDoubleType& trace,
        const double& expected)
    {
      trace.update(lambda, *s01);
      SVecDoubleType a(*s01);
      Assert::checkVectorEquals(trace.vect(), a);
      for (int i = 0; i < 1000; i++)
      {
        trace.update(lambda, *s02);
        Assert::pass(VectorsTestsUtils::checkConsistency(trace.vect()));
      }
      Assert::equals(s02->nbActiveEntries(), trace.vect().nbActiveEntries());
      SVecDoubleType expectedVec(trace.vect().dimension());
      expectedVec.set(*s02, expected);
      Assert::checkVectorEquals(trace.vect(), expectedVec, 0.00001);
    }

    void testTrace(const double& lambda, TraceDoubleType& trace, const double& expected)
    {
      testTraceWithDiscounting(lambda, trace, expected);
    }

  protected:
    void testATrace()
    {
      const double lambda = 0.9;
      testTrace(lambda, *newTrace(ATraceDouble), 1.0 / (1.0 - lambda));
    }

    void testRTrace()
    {
      testTrace(0.9, *newTrace(RTraceDouble), 1.0);
    }

    void testAMaxTrace()
    {
      testTrace(0.9, *newTrace(AMaxTraceDouble), 5.0);
    }

    template<class T, class O>
    class PerformanceVerifier: public Simulator<T, O>::Event
    {
      private:
        double minEpisodeReward;
      public:
        PerformanceVerifier() :
            minEpisodeReward(-350)
        {
        }

        void update() const
        {
          if (Simulator<T, O>::Event::nbEpisodeDone < 200)
            return;
          else
          {
            //std::cout << "[" << Simulator<T, O>::Event::episodeR << ","
            //    << Simulator<T, O>::Event::averageTimePerStep << "] ";
            Assert::fail(Simulator<T, O>::Event::episodeR < minEpisodeReward);
          }
        }
    };

    void runTestOnOnMountainCar(Projector<double, float>* projector, Trace<double>* trace);
    void testSarsaOnMountainCarSVectorTraces();
    void testSarsaOnMountainCarMaxLengthTraces();

  public:
    void run();
};

#endif /* TRACETEST_H_ */
