/*
 * AbstractMain.hpp
 *
 * Copyright (C) 2019 by RStudio, Inc.
 *
 * TODO: License
 *
 */

#ifndef LAUNCHER_PLUGINS_ABSTRACT_MAIN_HPP
#define LAUNCHER_PLUGINS_ABSTRACT_MAIN_HPP

#include "NonCopyable.hpp"

#include <memory>

#include "AbstractPluginApi.hpp"

namespace rstudio {
namespace launcher_plugins {

/**
 * @brief Base class for the Plugin Main class, which runs the plugin.
 */
class AbstractMain : public NonCopyable
{
public:
   /**
    * @brief Default Constructor.
    */
   AbstractMain() = default;

   /**
    * @brief Default destructor.
    */
   virtual ~AbstractMain() = default;

   /**
    * @brief Runs the plugin.
    *
    * @param in_argCount    The number of arguments in in_argList.
    * @param in_argList     The argument list to the program.
    *
    * @return 0 on a successful exit. A non-zero error code, otherwise.
    */
   int run(int in_argCount, char** in_argList);

private:
   /**
    * @brief Creates the Launcher Plugin API.
    *
    * @return The Plugin specific Launcher Plugin API.
    */
   virtual std::shared_ptr<AbstractPluginApi> createLauncherPluginApi() const = 0;
};

} // namespace launcher_plugins
} // namespace rstudio

#endif
