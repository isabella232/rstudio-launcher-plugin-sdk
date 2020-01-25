/*
 * Response.hpp
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


#ifndef LAUNCHER_PLUGINS_RESPONSE_HPP
#define LAUNCHER_PLUGINS_RESPONSE_HPP

#include <cstdint>

#include <PImpl.hpp>

namespace rstudio {
namespace launcher_plugins {
namespace json {

class Object;

} // namespace json
} // namespace launcher_plugins
} // namespace rstudio

namespace rstudio {
namespace launcher_plugins {
namespace api {

/**
 * @brief Represents the common components of all responses which can be sent the RStudio Launcher.
 */
class Response
{
public:
   /**
    * @brief Virtual destructor to allow for inheritance.
    */
   virtual ~Response() = default;

   /**
    * @brief Converts this response to a JSON object.
    *
    * @return The JSON object which represents this response.
    */
   virtual json::Object asJson() const;

protected:
   /**
    * @enum Response::Type
    * @brief Enum which represents the type of a Response.
    *
    * Types are defined as described in the RStudio Launcher API Documentation. See
    * https://docs.rstudio.com/job-launcher/latest/creating-plugins.html#plugin-messages for more details.
    */
   enum class Type
   {
      /** Error response */
      ERROR                = -1,

      /** Heartbeat response */
      HEARTBEAT            = 0,

      /** Bootstrap response */
      BOOTSTRAP            = 1,

      /** Job State response */
      JOB_STATE            = 2,

      /** Job Status response */
      JOB_STATUS           = 3,

      /** Control Job response */
      CONTROL_JOB          = 4,

      /** Control Job output response */
      JOB_OUTPUT           = 5,

      /** Job Resource utilization response */
      JOB_RESOURCE_UTIL    = 6,

      /** Job Network information response */
      JOB_NETWORK          = 7,

      /** Cluster Info response */
      CLUSTER_INFO         = 8,
   };

   /**
    * @brief Constructor.
    *
    * @param in_responseType    The type of response to be constructed.
    * @param in_requestId       The ID of the request for which this response is being sent.
    */
   Response(Type in_responseType, uint64_t in_requestId);

private:
   // The private implementation of Response.
   PRIVATE_IMPL(m_responseImpl);
};

class BootstrapResponse : public Response
{
public:
   /**
    * @brief Constructor.
    *
    * @param in_requestId       The ID of the bootstrap request for which this bootstrap response is being sent.
    */
   explicit BootstrapResponse(uint64_t in_requestId);

   /**
    * @brief Converts this bootstrap response to a JSON object.
    *
    * @return The JSON object which represents this bootstrap response.
    */
   json::Object asJson() const override;
};

} // namespace api
} // namespace launcher_plugins
} // namespace rstudio

#endif
