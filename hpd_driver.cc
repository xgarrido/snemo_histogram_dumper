// hpd_driver.cc

// Ourselves
#include <hpd_driver.h>

// Standard library
#include <fstream>

// Third party
// - Boost
#include <boost/foreach.hpp>

// - Bayeux:
// - datatools
#include <datatools/exception.h>
#include <datatools/i_tree_dump.h>
// - dpp
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
  output_directory = "/tmp/${USER}/snemo.d";
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
  out_ << datatools::i_tree_dumpable::tags::item() << "output directory  = "
       << output_directory << "" << std::endl;
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

  // Interpreting environment variable
  datatools::fetch_path_with_env(_params_.output_directory);

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
    BOOST_FOREACH(const std::string & a_name, hnames) {
      const std::string & a_group = pool_.get_group(a_name);
      if (! a_group.empty()) {
        DT_LOG_DEBUG(_logging_,
                      "Histogram '" << a_name << "' belongs to '" << a_group << "' group");
        hgroups.insert(a_group);
      }
    }

    // Compute org filename
    std::string filename = filename_;
    filename.erase(0, filename.find_last_of("/"));
    filename.erase(filename.find_last_of("."), std::string::npos);
    filename += ".org";
    DT_LOG_NOTICE(_logging_, "Org filename = " << filename);
    std::ofstream fout(std::string(_params_.output_directory + filename).c_str());

    // Process histogram belonging to same group
    BOOST_FOREACH(const std::string & a_group, hgroups) {
      std::vector<std::string> orgtbl;
      std::vector<std::string> filtered_names;
      pool_.names(filtered_names, "group=" + a_group);
      hpd_driver::_orgtbl_preamble(fout, a_group, filtered_names);
      BOOST_FOREACH(const std::string & a_name, filtered_names) {
        if (pool_.has_1d(a_name)) {
          const mygsl::histogram_1d & h = pool_.get_1d(a_name);
          if (a_name == filtered_names[0]) {
            hpd_driver::_histogram2org(h, orgtbl);
          } else {
            hpd_driver::_histogram2org(h, orgtbl, true);
          }
        } else if (pool_.has_2d(a_name)) {
          DT_LOG_WARNING(_logging_, "2D histogram are not currently supported !");
        } else {
          DT_THROW_IF(true, std::logic_error,
                      "Histogram '" << a_name << "' is neither a 1D nor 2D histogram !");
        }
      } // end of filtered names
      BOOST_FOREACH(const std::string & a_tbl, orgtbl) {
        fout << a_tbl << std::endl;
      }
    } // end of groups

    // Process ungrouped histograms
    BOOST_FOREACH(const std::string & a_name, hnames) {
      if (! pool_.get_group(a_name).empty()) continue;
      DT_LOG_DEBUG(_logging_, "Processing '" << a_name << "' histogram...");
      std::vector<std::string> orgtbl;
      if (pool_.has_1d(a_name)) {
        const mygsl::histogram_1d & h = pool_.get_1d(a_name);
        hpd_driver::_histogram2org(h, orgtbl);
      } else if (pool_.has_2d(a_name)) {
        DT_LOG_WARNING(_logging_, "2D histogram are not currently supported !");
      } else {
        DT_THROW_IF(true, std::logic_error,
                    "Histogram '" << a_name << "' is neither a 1D nor 2D histogram !");
      }
    } // end of histogram names

  } // end of ORG support
  return;
}

void hpd_driver::_orgtbl_preamble(std::ostream & out_, const std::string & tblname_, const std::vector<std::string> & columns_desc_) const
{
  out_ << "* Table for =" << tblname_ << "=" << std::endl;
  out_ << "#+TBLNAME: " << tblname_ << "_data" << std::endl;
  out_ << "|xmin|xmax";
  BOOST_FOREACH(const std::string & a_desc, columns_desc_) {
    out_ << '|' << a_desc.substr(0, a_desc.find(tblname_)-1);
  }
  out_ << std::endl << "|-" << std::endl;;
  return;
}

void hpd_driver::_histogram2org(const mygsl::histogram_1d & h1d_, std::vector<std::string> & orgtbl_, const bool skip_ranges_) const
{
  for (size_t i = 0; i < h1d_.bins(); ++i) {
    const std::pair<double,double> range = h1d_.get_range(i);
    const double value = h1d_.get(i);

    if (! skip_ranges_) {
      std::ostringstream oss;
      oss << '|' << range.first << '|' << range.second << '|' << value << '|';
      orgtbl_.push_back(oss.str());
    } else {
      std::ostringstream oss;
      oss << value << '|';
      orgtbl_.at(i) += oss.str();
    }
  }
  return;
}
