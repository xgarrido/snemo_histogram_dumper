// hpd_driver.cc

// Ourselves
#include <hpd_driver.h>

// - Bayeux:
// - datatools
#include <datatools/exception.h>

hpd_driver_params::hpd_driver_params()
{
  reset();
  return;
}

void hpd_driver_params::reset()
{
  help = false;
  logging_label = "warning";
  return;
}

void hpd_driver_params::dump(std::ostream & out_)
{
  out_ << "hpd_driver_params::dump: " << std::endl;
  out_ << "|-- " << "help  = "
       << help << "" << std::endl;
  out_ << "|-- " << "logging_label  = '"
       << logging_label << "'" << std::endl;
  return;
}

hpd_driver::hpd_driver()
{
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
  if (error_code != EXIT_SUCCESS) {
    DT_LOG_ERROR(_logging_, "Error code : " << error_code);
  }
  DT_LOG_DEBUG(_logging_, "Exiting.");
  return;
}
