// Copyright 2014, Georg Sauthoff <mail@georg.so>

/* {{{ GPLv3

    This file is part of librfc4180csv.

    librfc4180csv is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    librfc4180csv is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with librfc4180csv.  If not, see <http://www.gnu.org/licenses/>.

}}} */
#include "rfc4180csv.h"
using namespace RFC4180;

#include <iostream>
#include <fstream>
#include <cstring>
#include <array>
#include <stdexcept>
using namespace std;


#include "buffer/buffer.h"
using namespace Memory;


struct CB : public Callback::Null {
  Buffer::Vector buffer;
  void csv_field() override
  {
    cout << '|';
    cout.write(buffer.begin(), buffer.size());
    cout << '|';
  }
  virtual void csv_record() override
  {
    cout << '\n';
  }
};

int main(int argc, char **argv)
{
  if (argc < 2) {
    cerr << "Call: " << *argv << " file1.csv ...\n";
    return 1;
  }

  for (int i = 1; i<argc; ++i) {
    Mapped_File f(argv[i]);
    CB cb;
    CSV csv(cb.buffer, cb);
    csv.push_input(f.begin(), f.end());
    csv.start();
  }

  return 0;
}
