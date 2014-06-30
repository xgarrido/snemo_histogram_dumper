/* hpd_driver.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2014-06-27
 * Last modified : 2014-06-27
 *
 * Copyright (C) 2014 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   The hpd driver.
 *
 */

#ifndef HPD_DRIVER_H_
#define HPD_DRIVER_H_ 1

// Standard library
#include <iostream>
#include <string>

// - Bayeux:
// - datatools
#include <datatools/logger.h>
// - mygsl
#include <mygsl/histogram_1d.h>
#include <mygsl/histogram_2d.h>

namespace mygsl {
  class histogram_pool;
}

enum output_type {
  OUTPUT_INVALID = 0,
  OUTPUT_COUT,
  OUTPUT_CLOG,
  OUTPUT_ORG,
  OUTPUT_DEFAULT = OUTPUT_COUT
};

struct hpd_driver_params
{
  hpd_driver_params();
  void reset();
  void dump(std::ostream & out_ = std::clog);

  bool        help;
  std::string logging_label;
  std::vector<std::string> input_files;
  std::string output_type_label;
};

class hpd_driver {
public:
  hpd_driver();
  ~hpd_driver();

  bool is_initialized() const;
  void setup(const hpd_driver_params &);
  void initialize();
  void run();
  void reset();

  static void histogram2org(const mygsl::histogram_1d & h1d_,
                            std::vector<std::string> & orgtbl_,
                            const bool skip_ranges_ = false);

  // static void histogram2org(const mygsl::histogram_2d & h2d_,
  //                            std::vector<std::string> & orgtbl_,
  //                            const bool skip_ranges_ = false);

protected:

  void _dump(const mygsl::histogram_pool & pool_,
             const std::string & filename_ = "") const;

private:
  bool                        _initialized_;
  datatools::logger::priority _logging_;
  hpd_driver_params           _params_;
  output_type                 _output_;
};

#endif // HPD_DRIVER_H_
