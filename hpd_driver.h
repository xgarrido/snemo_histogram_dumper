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

struct hpd_driver_params
{
  hpd_driver_params();
  void reset();
  void dump(std::ostream & out_ = std::clog);

  bool        help;
  std::string logging_label;
};

#endif // HPD_DRIVER_H_
