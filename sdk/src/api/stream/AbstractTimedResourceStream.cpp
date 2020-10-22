/*
 * AbstractTimedResourceStream.cpp
 *
 * Copyright (C) 2020 by RStudio, PBC
 *
 * Unless you have received this program directly from RStudio pursuant to the terms of a commercial license agreement
 * with RStudio, then this program is licensed to you under the following terms:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <api/stream/AbstractTimedResourceStream.hpp>

#include <system/Asio.hpp>

namespace rstudio {
namespace launcher_plugins {
namespace api {

typedef std::shared_ptr<AbstractTimedResourceStream> SharedThis;
typedef std::weak_ptr<AbstractTimedResourceStream> WeakThis;

struct AbstractTimedResourceStream::Impl
{
   explicit Impl(system::TimeDuration&& in_frequency) :
      Frequency(in_frequency)
   {
   }

   system::TimeDuration Frequency;

   system::AsyncTimedEvent TimedEvent;
};

PRIVATE_IMPL_DELETER_IMPL(AbstractTimedResourceStream);

AbstractTimedResourceStream::~AbstractTimedResourceStream()
{
   try
   {
      m_timedBaseImpl->TimedEvent.cancel();
   }
   catch (...)
   {
      // Swallow exceptions because destructors must not throw.
   }
}

AbstractTimedResourceStream::AbstractTimedResourceStream(
   system::TimeDuration in_frequency,
   const ConstJobPtr& in_job,
   comms::AbstractLauncherCommunicatorPtr in_launcherCommunicator) :
      AbstractResourceStream(in_job, in_launcherCommunicator),
      m_timedBaseImpl(new Impl(std::move(in_frequency)))
{
}

Error AbstractTimedResourceStream::initialize()
{
   WeakThis weakThis = weak_from_this();
   system::AsioFunction onTimer = [weakThis]()
   {
      if (SharedThis sharedThis = weakThis.lock())
      {
         ResourceUtilData data;
         Error error = sharedThis->pollResourceUtilData(data);
         if (error)
         {
            // No need to report the error twice, but cancel the timed event on error.
            sharedThis->reportError(error);
            sharedThis->m_timedBaseImpl->TimedEvent.cancel();
         }
         else
            sharedThis->reportData(data);
      }
   };

   m_timedBaseImpl->TimedEvent.start(m_timedBaseImpl->Frequency, onTimer);

   return Success();
}


} // namespace api 
} // namespace launcher_plugins 
} // namespace rstudio 
