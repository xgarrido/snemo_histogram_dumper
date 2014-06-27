/* histogram_pool_dumper.cxx
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2014-06-27
 * Last modified : 2014-06-27
 *
 * Copyright (C) 2014 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 * Description:
 *
 *  A generic program that dumps mygsl::histogram from an histogram_pool into
 *  different output format : pure ascii, org-mode file.
 *
 */

// Third party
// - Boost
#include <boost/program_options.hpp>

// - Bayeux:
// - datatools
#include <datatools/logger.h>

// This project
#include <hpd_driver.h>

struct ui {

  /// Print usage
  static void print_usage(std::ostream &);

  /// Build options
  static void build_opts(boost::program_options::options_description &,
                         hpd_driver_params &);

  /// Application name
  static const std::string APP_NAME;

};

const std::string ui::APP_NAME = "histogram_pool_dumper";

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  try {
  }
  catch (std::exception & x) {
    DT_LOG_FATAL(logging, ui::APP_NAME << ": " << x.what ());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, ui::APP_NAME << ": " << "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
