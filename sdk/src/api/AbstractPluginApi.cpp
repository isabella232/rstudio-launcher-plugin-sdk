/*
 * AbstractPluginApi.cpp
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

#include <api/AbstractPluginApi.hpp>

#include <boost/algorithm/string/join.hpp>

#include <api/Constants.hpp>
#include <api/IJobSource.hpp>
#include <api/Response.hpp>
#include <options/Options.hpp>
#include <system/Asio.hpp>
#include <json/Json.hpp>

namespace rstudio {
namespace launcher_plugins {
namespace api {

typedef std::shared_ptr<AbstractPluginApi> SharedThis;
typedef std::weak_ptr<AbstractPluginApi> WeakThis;

struct AbstractPluginApi::Impl
{
   /**
    * @brief Constructor.
    *
    * @param in_launcherCommunicator    The communicator that will be used to send and receive messages from the RStudio
    *                                   Launcher.
    */
   explicit Impl(std::shared_ptr<comms::AbstractLauncherCommunicator> in_launcherCommunicator) :
      LauncherCommunicator(std::move(in_launcherCommunicator))
   {
   }

   /**
    * @brief Sends an error response to the Launcher.
    *
    * @param in_requestId   The ID of the request for which an error occurred.
    * @param in_type        The type of error which occurred.
    * @param in_error       The error which occurred.
    */
   void sendErrorResponse(uint64_t in_requestId, ErrorResponse::Type in_type, const std::string& in_errorMessage)
   {
      LauncherCommunicator->sendResponse(
         ErrorResponse(
            in_requestId,
            in_type,
            in_errorMessage));
   }

   /**
    * @brief Sends an error response to the Launcher.
    *
    * @param in_requestId   The ID of the request for which an error occurred.
    * @param in_type        The type of error which occurred.
    * @param in_error       The error which occurred.
    */
   void sendErrorResponse(uint64_t in_requestId, ErrorResponse::Type in_type, const Error& in_error)
   {
      sendErrorResponse(in_requestId, in_type, in_error.asString());
   }

   /**
    * @brief "Handles" a received heartbeat request by logging a debug message.
    */
   static void handleHeartbeat()
   {
      // There's really nothing to do here, since if the Launcher dies the plugin will die.
      logging::logDebugMessage("Received Heartbeat from Launcher.");
   }

   /**
    * @brief Handles bootstrap requests from the Launcher.
    *
    * @param in_bootstrapRequest    The bootstrap request to handle.
    */
   void handleBootstrap(const std::shared_ptr<BootstrapRequest>& in_bootstrapRequest)
   {
      if (in_bootstrapRequest->getMajorVersion() != API_VERSION_MAJOR)
      {
         return LauncherCommunicator->sendResponse(
            ErrorResponse(
               in_bootstrapRequest->getId(),
               ErrorResponse::Type::UNSUPPORTED_VERSION,
               "The plugin supports API version " +
               std::to_string(API_VERSION_MAJOR) +
               ".X.XXXX. The Launcher's API version is" +
               std::to_string(in_bootstrapRequest->getMajorVersion()) + "." +
               std::to_string(in_bootstrapRequest->getMinorVersion()) + "." +
               std::to_string(in_bootstrapRequest->getPatchNumber())));
      }

      Error error = JobSource->initialize();
      if (error)
         return sendErrorResponse(in_bootstrapRequest->getId(), ErrorResponse::Type::UNKNOWN, error);

      JobList jobs;
      error = JobSource->getJobs(jobs);
      if (error)
         return sendErrorResponse(in_bootstrapRequest->getId(), ErrorResponse::Type::UNKNOWN, error);

      for (const JobPtr& job: jobs)
         JobRepo->addJob(job);

      LauncherCommunicator->sendResponse(BootstrapResponse(in_bootstrapRequest->getId()));
   }

   void handleGetClusterInfo(const std::shared_ptr<UserRequest>& in_clusterInfoRequest)
   {
      const system::User& requestUser = in_clusterInfoRequest->getUser();
      uint64_t requestId = in_clusterInfoRequest->getId();

      JobSourceConfiguration caps;
      Error error = JobSource->getConfiguration(requestUser, caps);
      if (error)
         return sendErrorResponse(requestId, ErrorResponse::Type::UNKNOWN, error);

      return LauncherCommunicator->sendResponse(ClusterInfoResponse(in_clusterInfoRequest->getId(), caps));
   }

   void handleGetJobRequest(const std::shared_ptr<JobStateRequest>& in_getJobRequest)
   {
      const std::string& jobId = in_getJobRequest->getJobId();

      Optional<system::DateTime> startTime, endTime;
      Error error = in_getJobRequest->getStartTime(startTime);
      if (error)
         return sendErrorResponse(
            in_getJobRequest->getId(),
            ErrorResponse::Type::INVALID_REQUEST,
            "Invalid start time");

      error = in_getJobRequest->getEndTime(endTime);
      if (error)
         return sendErrorResponse(
            in_getJobRequest->getId(),
            ErrorResponse::Type::INVALID_REQUEST,
            "Invalid end time");

      const Optional<std::set<std::string> >& fields = in_getJobRequest->getFieldSet(),
                                            & tags = in_getJobRequest->getTagSet();

      Optional<std::set<Job::State> > statuses;
      error = in_getJobRequest->getStatusSet(statuses);
      if (error)
         return sendErrorResponse(
            in_getJobRequest->getId(),
            ErrorResponse::Type::INVALID_REQUEST,
            "Invalid status(es): " + error.getMessage());

      std::vector<std::string> statusesStrSet;
      std::string statusesStr = "none";
      if (statuses)
      {
         std::transform(
            statuses.getValueOr({}).begin(),
            statuses.getValueOr({}).end(),
            std::back_inserter(statusesStrSet),
            &Job::stateToString);
         statusesStr = boost::algorithm::join(statusesStrSet, ", ");
      }

      logging::logDebugMessage(
         "Received getJobState request for " + in_getJobRequest->getUser().getUsername() +
         ": jobID: " + jobId +
         " startTime: " + (startTime ? startTime.getValueOr(system::DateTime()).toString() : "none") +
         " endTime: " + (endTime ? endTime.getValueOr(system::DateTime()).toString() : "none") +
         " statuses: " + statusesStr);

      JobList jobs;
      if (jobId == "*")
      {
         jobs = JobRepo->getJobs(in_getJobRequest->getUser());

         // Filter the jobs based on the request.
         for (auto itr = jobs.begin(); itr != jobs.end();)
         {
            const JobPtr& job = *itr;

            // Skip the job if it wasn't submitted within the requested range of submission times...
            if ((startTime && (job->SubmissionTime < startTime.getValueOr(system::DateTime()))) ||
               (endTime && (job->SubmissionTime > endTime.getValueOr(system::DateTime()))) ||
               // ... or if it doesn't have all of the requested tags...
               (tags && !job->matchesTags(tags.getValueOr({}))) ||
               // ... or if it isn't in one of the requested states.
               (statuses && (statuses.getValueOr({}).find(job->Status) == statuses.getValueOr({}).end())))
               itr = jobs.erase(itr);
            else
               ++itr;
         }
      }
      else
      {
         // If a specific Job ID was requested, ignore the other filters.
         JobPtr job = JobRepo->getJob(jobId, in_getJobRequest->getUser());
         if (job == nullptr)
            return sendErrorResponse(
               in_getJobRequest->getId(),
               ErrorResponse::Type::JOB_NOT_FOUND,
               "Job " + jobId + " could not be found for user " + in_getJobRequest->getUser().getUsername());

         jobs.push_back(job);
      }

      return LauncherCommunicator->sendResponse(JobStateResponse(in_getJobRequest->getId(), jobs, fields));
   }

   /**
    * @brief Handles a request from the Launcher.
    *
    * @param in_type        The type of request handler which should be invoked.
    * @param in_request     The request to handle.
    */
   void handleRequest(Request::Type in_handlerType, const std::shared_ptr<Request>& in_request)
   {
      // This should be impossible. It would effectively be an internal server error.
      if (in_handlerType != in_request->getType())
         return sendErrorResponse(
            in_request->getId(),
            ErrorResponse::Type::UNKNOWN,
            "Internal Request Handling Error.");


      if (!JobSource)
      {
         logging::logErrorMessage("Request received before JobSource was initialized.", ERROR_LOCATION);
         return sendErrorResponse(
            in_request->getId(),
            ErrorResponse::Type::UNKNOWN,
            "Internal Request Handling Error.");
      }

      switch (in_handlerType)
      {
         case Request::Type::HEARTBEAT:
            return handleHeartbeat();
         case Request::Type::BOOTSTRAP:
            return handleBootstrap(std::static_pointer_cast<BootstrapRequest>(in_request));
         case Request::Type::GET_CLUSTER_INFO:
            return handleGetClusterInfo(std::static_pointer_cast<UserRequest>(in_request));
         case Request::Type::GET_JOB:
            return handleGetJobRequest(std::static_pointer_cast<JobStateRequest>(in_request));
         default:
            return sendErrorResponse(
               in_request->getId(),
               ErrorResponse::Type::UNKNOWN,
               "Internal Request Handling Error.");
      }
   }

   /** The job source which communicates with the job scheduling system. */
   std::shared_ptr<IJobSource> JobSource;

   /** The job repository */
   std::shared_ptr<jobs::JobRepository> JobRepo;

   /** The communicator that will be used to send and receive messages from the RStudio Launcher. */
   std::shared_ptr<comms::AbstractLauncherCommunicator> LauncherCommunicator;

   /** A timed event to send heartbeats on the configured time interval */
   system::AsyncTimedEvent SendHeartbeatEvent;
};

PRIVATE_IMPL_DELETER_IMPL(AbstractPluginApi)

Error AbstractPluginApi::initialize()
{
   // Create the job source.
   m_abstractPluginImpl->JobSource = createJobSource();

   // Create the job repository.
   m_abstractPluginImpl->JobRepo = createJobRepository();

   // Register all the request handlers.
   std::shared_ptr<comms::AbstractLauncherCommunicator>& comms = m_abstractPluginImpl->LauncherCommunicator;

   using namespace std::placeholders;
   comms->registerRequestHandler(
      Request::Type::BOOTSTRAP,
      std::bind(&Impl::handleRequest, m_abstractPluginImpl.get(), Request::Type::BOOTSTRAP, _1));
   comms->registerRequestHandler(
      Request::Type::HEARTBEAT,
      std::bind(&Impl::handleRequest, m_abstractPluginImpl.get(), Request::Type::HEARTBEAT, _1));
   comms->registerRequestHandler(
      Request::Type::GET_CLUSTER_INFO,
      std::bind(&Impl::handleRequest, m_abstractPluginImpl.get(), Request::Type::GET_CLUSTER_INFO, _1));
   comms->registerRequestHandler(
      Request::Type::GET_JOB,
      std::bind(&Impl::handleRequest, m_abstractPluginImpl.get(), Request::Type::GET_JOB, _1));

   // Make the heartbeat event.
   WeakThis weakThis = shared_from_this();
   auto onHeartbeatTimer = [weakThis]()
   {
      // If this doesn't exist just exit.
      SharedThis sharedThis = weakThis.lock();
      if (!sharedThis)
         return;

      sharedThis->m_abstractPluginImpl->LauncherCommunicator->sendResponse(HeartbeatResponse());
   };

   // Start the heartbeat timer.
   m_abstractPluginImpl->SendHeartbeatEvent.start(
      options::Options::getInstance().getHeartbeatIntervalSeconds(),
      onHeartbeatTimer);

   // Initialize the plugin-specific API components.
   return doInitialize();
}

AbstractPluginApi::AbstractPluginApi(std::shared_ptr<comms::AbstractLauncherCommunicator> in_launcherCommunicator) :
   m_abstractPluginImpl(new Impl(std::move(in_launcherCommunicator)))
{
}

std::shared_ptr<jobs::JobRepository> AbstractPluginApi::createJobRepository() const
{
   return std::make_shared<jobs::JobRepository>();
}

} // namespace api
} // namespace launcher_plugins
} // namespace rstudio