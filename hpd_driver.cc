// hpd_driver.cc

// Ourselves
#include <hpd_driver.h>

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
