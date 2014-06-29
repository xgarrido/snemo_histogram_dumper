// hpd_driver.cc

// Ourselves
#include <hpd_driver.h>

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
       i != _params_.input_files.end(); ++i)
    {
      const std::string & a_file = *i;
      DT_LOG_TRACE(_logging_, "Processing file '" << a_file << "'");
      hs.load_from_boost_file(a_file);
      _dump(a_file);
    }

  if (error_code != EXIT_SUCCESS) {
    DT_LOG_ERROR(_logging_, "Error code : " << error_code);
  }
  DT_LOG_DEBUG(_logging_, "Exiting.");
  return;
}

void hpd_driver::_dump(const std::string & filename_) const
{

  return;
}
