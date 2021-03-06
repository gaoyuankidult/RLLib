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
 * ProjectorTest.h
 *
 *  Created on: May 15, 2013
 *      Author: sam
 */

#ifndef PROJECTORTEST_H_
#define PROJECTORTEST_H_

#include "Test.h"
#include "Projector.h"

RLLIB_TEST(ProjectorTest)

class ProjectorTest: public ProjectorTestBase
{
  public:
    ProjectorTest() {}

    virtual ~ProjectorTest() {}
    void run();

  private:
    void testProjector();
    void testFourierBasis();
};

#endif /* PROJECTORTEST_H_ */
