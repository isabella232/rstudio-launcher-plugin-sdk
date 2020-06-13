/*
 * AbstractOutputStream.cpp
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

#include <api/AbstractOutputStream.hpp>

namespace rstudio {
namespace launcher_plugins {
namespace api {

struct AbstractOutputStream::Impl
{
   Impl(uint64_t in_requestId, comms::AbstractLauncherCommunicatorPtr&& in_launcherCommunicator) :
      LauncherCommunicator(in_launcherCommunicator),
      RequestId(in_requestId)
   {
   }

   comms::AbstractLauncherCommunicatorPtr LauncherCommunicator;

   uint64_t RequestId;
};

PRIVATE_IMPL_DELETER_IMPL(AbstractOutputStream)

AbstractOutputStream::AbstractOutputStream(
   uint64_t in_requestId,
   OutputType in_outputType,
   JobPtr in_job,
   comms::AbstractLauncherCommunicatorPtr in_launcherCommunicator) :
      m_outputType(in_outputType),
      m_job(std::move(in_job)),
      m_baseImpl(new Impl(in_requestId, std::move(in_launcherCommunicator)))
{
}

void AbstractOutputStream::reportData(const std::string& in_data, OutputType in_outputType)
{

}

void AbstractOutputStream::setStreamComplete()
{

}

} // namespace api
} // namespace launcher_plugins
} // namespace rstudio
