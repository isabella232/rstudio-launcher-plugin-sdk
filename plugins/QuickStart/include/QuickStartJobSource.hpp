/*
 * QuickStartJobSource.hpp
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

#ifndef LAUNCHER_PLUGINS_QUICK_START_JOB_SOURCE_HPP
#define LAUNCHER_PLUGINS_QUICK_START_JOB_SOURCE_HPP

#include <api/IJobSource.hpp>

namespace rstudio {
namespace launcher_plugins {
namespace quickstart {

/**
 * @brief Class which is responsible for running and retrieving information about jobs in the job scheduling system.
 */
class QuickStartJobSource : public api::IJobSource
{
public:
   /**
    * @brief Initializes the Job Source.
    *
    * This function should return an error if communication with the job source fails.
    *
    * @return Success if the job source could be initialized; Error otherwise.
    */
   Error initialize() override;

   /**
    * @brief Gets the capabilities of this Job Source for the specified user.
    *
    * This function controls the options that will be available to users when launching jobs.
    *
    * NOTE: Many of the values here should most likely be controllable by Launcher administrators when they configure
    *       the Launcher. For more details, see the RStudio Launcher Plugin SDK QuickStart Guide TODO #7.
    *
    * @param in_user                The user who made the request to see the capabilities of the Cluster. This may be
    *                               used to return different capabilities based on the configured user profiles. For
    *                               more information about user profiles, see the 'User Profiles' section of the
    *                               'Advanced Features' chapter of the RStudio Launcher Plugin SDK Developer's Guide.
    * @param out_capabilities       The capabilities of this Job Source, for the specified user.
    *
    * @return Success if the capabilities for this Job Source could be populated; Error otherwise.
    */
   Error getConfiguration(const system::User& in_user, api::JobSourceConfiguration& out_capabilities) const override;
};

} // namespace quickstart
} // namespace launcher_plugins
} // namespace rstudio

#endif
