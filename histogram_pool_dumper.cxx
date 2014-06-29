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
#include <datatools/exception.h>

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

  namespace po = boost::program_options;
  po::options_description opts ("Allowed options");

  try {
    // The dpp_driver parameters.
    hpd_driver_params dp;
    ui::build_opts(opts, dp);

    // Describe command line arguments :
    po::positional_options_description args;
    args.add ("input-file", -1);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts)
      .positional(args)
      .allow_unregistered()
      .run();
    po::store(parsed, vm);
    po::notify(vm);

    // Fetch general opts :
    logging = datatools::logger::get_priority(dp.logging_label);
    DT_THROW_IF(logging == datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "Invalid logging priority '" << dp.logging_label << "' !");

    if (dp.help) {
      ui::print_usage(std::cout);
    } else {
      hpd_driver drv;
      drv.setup(dp);
      drv.initialize();
      drv.run();
      drv.reset();
    }
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

void ui::print_usage(std::ostream & out_)
{
  out_ << APP_NAME << " -- A mygsl::histogram dumper program" << std::endl;
  out_ << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << std::endl;
  out_ << "  " << APP_NAME << " [OPTIONS] [ARGUMENTS] " << std::endl;
  out_ << std::endl;
  boost::program_options::options_description opts("Allowed options");
  hpd_driver_params dummy;
  build_opts(opts, dummy);
  out_ << opts << std::endl;
  return;
}

void ui::build_opts(boost::program_options::options_description & opts_,
                    hpd_driver_params & params_)
{
  namespace po = boost::program_options;
  opts_.add_options ()
    ("help,h", po::value<bool> (&params_.help)
     ->zero_tokens()
     ->default_value(false),
     "produce help message.")
    ("logging-priority,P",
     po::value<std::string>(&params_.logging_label)->default_value ("warning"),
     "set the logging priority.")
    ("input-file,i",
     po::value<std::vector<std::string> > (&params_.input_files),
     "set an input histogram Boost archive.")
    ("output-type,t",
     po::value<std::string>(&params_.output_type_label),
     "set the output format.")
    ;
  return;
}
