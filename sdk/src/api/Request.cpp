/*
 * Request.cpp
 *
 * Copyright (C) 2019 by RStudio, Inc.
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

#include <api/Request.hpp>

#include <Error.hpp>
#include <system/User.hpp>

namespace rstudio {
namespace launcher_plugins {
namespace api {

struct Request::Impl
{
   Impl() :
      Id(0),
      RequestType(Type::HEARTBEAT),
      Valid(false)
   {
   }

   uint64_t Id;
   std::string RequestUsername;
   Type RequestType;
   system::User User;
   bool Valid;
};

PRIVATE_IMPL_DELETER_IMPL(Request)

Error Request::fromJson(const json::Object& in_requestJson, std::shared_ptr<Request>& out_request)
{
   return Success();
}

uint64_t Request::getId() const
{
   return m_impl->Id;
}

const std::string& Request::getRequestUsername() const
{
   return m_impl->RequestUsername;
}

Request::Type Request::getType() const
{
   return m_impl->RequestType;
}

const system::User& Request::getUser() const
{
   return m_impl->User;
}

Request::Request() :
   m_impl(new Impl())
{
}

} // namespace api
} // namespace launcher_plugins
} // namespace rstudio
