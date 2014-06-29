// hpd_driver.cc

// Ourselves
#include <hpd_driver.h>

// Third party
// - Boost
#include <boost/foreach.hpp>

// - Bayeux:
// - datatools
#include <datatools/exception.h>
#include <datatools/i_tree_dump.h>
// - mygsl
#include <dpp/histogram_service.h>

hpd_driver_params::hpd_driver_params()
{
  reset();
  return;
}

void hpd_driver_params::reset()
{
  help = false;
  logging_label = "warning";
  input_files.clear();
  output_type_label = "clog";
  return;
}

void hpd_driver_params::dump(std::ostream & out_)
{
  out_ << "hpd_driver_params::dump: " << std::endl;
  out_ << datatools::i_tree_dumpable::tags::item() << "help  = "
       << help << "" << std::endl;
  out_ << datatools::i_tree_dumpable::tags::item() << "logging_label  = '"
       << logging_label << "'" << std::endl;
  out_ << datatools::i_tree_dumpable::tags::item() << "input_files  = "
       << input_files.size() << "" << std::endl;
  for (size_t i = 0; i < input_files.size(); ++i) {
    out_ << datatools::i_tree_dumpable::tags::skip_item();
    if (i == input_files.size()-1)
      out_ << datatools::i_tree_dumpable::tags::last_item();
    else
      out_ << datatools::i_tree_dumpable::tags::item();
    out_ << input_files.at(i) << std::endl;
  }
  out_ << datatools::i_tree_dumpable::tags::last_item() << "output format  = "
       << output_type_label << "" << std::endl;
  return;
}

hpd_driver::hpd_driver()
{
  _output_ = OUTPUT_INVALID;
  _initialized_ = false;
  return;
}

hpd_driver::~hpd_driver()
{
  if (is_initialized()) {
    reset();
  }
  return;
}

bool hpd_driver::is_initialized() const
{
  return _initialized_;
}

void hpd_driver::setup(const hpd_driver_params & params_)
{
  DT_THROW_IF(is_initialized(),
              std::logic_error,
              "Driver is already initialized !");
  _params_ = params_;
  return;
}

void hpd_driver::initialize()
{
  DT_THROW_IF(is_initialized(),
              std::logic_error,
              "Driver is already initialized !");
  _logging_ = datatools::logger::get_priority(_params_.logging_label);

  DT_THROW_IF(_params_.input_files.empty(), std::logic_error, "Missing input histogram pool files !");

  if (_params_.output_type_label == "clog") {
    _output_ = OUTPUT_CLOG;
  } else if (_params_.output_type_label == "cout") {
    _output_ = OUTPUT_COUT;
  } else if (_params_.output_type_label == "org") {
    _output_ = OUTPUT_ORG;
  }
  DT_THROW_IF(_output_ == OUTPUT_INVALID,
              std::logic_error,
              "Invalid output label '" << _params_.output_type_label << "' !");

  if (_logging_ >= datatools::logger::PRIO_DEBUG)
    {
      _params_.dump();
    }
  _initialized_ = true;
  return;
}

void hpd_driver::reset()
{
  DT_THROW_IF(! is_initialized(),
              std::logic_error,
              "Driver is not initialized !");
  _params_.reset();
  _initialized_ = false;
  return;
}

void hpd_driver::run()
{
  int error_code = EXIT_SUCCESS;
  DT_THROW_IF(! is_initialized(),
              std::logic_error,
              "Driver is not initialized !");
  DT_LOG_DEBUG(_logging_, "Entering...");

  // Declare an histogram service
  dpp::histogram_service hs;

  for (std::vector<std::string>::const_iterator i = _params_.input_files.begin();
       i != _params_.input_files.end(); ++i) {
    const std::string & a_file = *i;
    DT_LOG_TRACE(_logging_, "Processing file '" << a_file << "'");
    hs.load_from_boost_file(a_file);
    _dump(hs.get_pool(), a_file);
  }

  if (error_code != EXIT_SUCCESS) {
    DT_LOG_ERROR(_logging_, "Error code : " << error_code);
  }
  DT_LOG_DEBUG(_logging_, "Exiting.");
  return;
}

void hpd_driver::_dump(const mygsl::histogram_pool & pool_, const std::string & filename_) const
{
  if (_output_ == OUTPUT_CLOG)
    pool_.tree_dump(std::clog);
  else if (_output_ == OUTPUT_COUT)
    pool_.tree_dump(std::cout);
  else if (_output_ == OUTPUT_ORG) {
    std::vector<std::string> hnames;
    pool_.names(hnames);
    std::set<std::string> hgroups;
    BOOST_FOREACH (const std::string & a_name, hnames) {
      const std::string & a_group = pool_.get_group(a_name);
      if (! a_group.empty()) {
        DT_LOG_DEBUG(_logging_,
                      "Histogram '" << a_name << "' belongs to '" << a_group << "'");
        hgroups.insert(a_group);
      }
    }

    // Process histogram belonging to same group
    if (! hgroups.empty()) {
      BOOST_FOREACH (const std::string & a_group, hgroups) {
        std::vector<std::string> filtered_names;
        pool_.names(filtered_names, "group=" + a_group);
        BOOST_FOREACH (const std::string & a_name, filtered_names) {
          if (pool_.has_1d(a_name)) {
          } else if (pool_.has_2d(a_name)) {
            DT_LOG_WARNING(_logging_, "2D histogram are not currently supported !");
          } else {
            DT_THROW_IF(true, std::logic_error,
                        "Histogram '" << a_name << "' is neither a 1D nor 2D histogram !");
          }
        } // end of filtered names
      } // end of groups
    }

    // Process ungrouped histograms
    BOOST_FOREACH (const std::string & a_name, hnames) {
      if (! pool_.get_group(a_name).empty()) continue;
      DT_LOG_DEBUG(_logging_, "Processing '" << a_name << "' histogram...");
    } // end of histogram names

  } // end of ORG support
  return;
}
