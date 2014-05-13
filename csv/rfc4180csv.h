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
#ifndef RFC4180CSV_H
#define RFC4180CSV_H

#include <cstddef>
#include <utility>
#include <stack>


#include <buffer/buffer.h>

namespace RFC4180 {

  namespace Callback {
    class Base {
      friend class CSV;
      private:
      protected:
        size_t csv_nr_ {0};
        size_t csv_nf_ {0};
        bool break_out_ {0};
      public:
        size_t csv_nr() const { return csv_nr_; }
        void set_csv_nr(size_t x) { csv_nr_ = x; }
        size_t csv_nf() const { return csv_nf_; }
        void set_csv_nf(size_t x) { csv_nf_ = x; }
        bool break_out() const { return break_out_; }
        void set_break_out(bool b) { break_out_ = b; }

        virtual bool csv_not_eof() = 0;
        virtual void csv_prepare_input(
            std::stack<std::pair<const char *, const char *> > &input_stack,
            const char *p, const char *pe) = 0;


        virtual void csv_record_first() = 0;
        virtual void csv_record() = 0;
        virtual void csv_record_last() = 0;
        virtual void csv_field() = 0;
        virtual void csv_field_last() = 0;
    };
    class Null : public Base {
      private:
      public:
        bool csv_not_eof() override;
        void csv_prepare_input(
            std::stack<std::pair<const char *, const char *> > &input_stack,
            const char *p, const char *pe) override;
        void csv_record_first() override;
        void csv_record() override;
        void csv_record_last() override;
        void csv_field() override;
        void csv_field_last() override;
    };
    class Count : public Null {
      friend class CSV;
      private:
      public:
        void csv_record() override;
        void csv_field() override;
    };
  }

  class CSV;
  class Field_Iterator {
    private:
      CSV *csv {nullptr};
      std::pair<const char*, const char *> p {nullptr, nullptr};
    protected:
    public:
      Field_Iterator();
      Field_Iterator(CSV *csv);
      Field_Iterator &operator++();
      std::pair<const char*, const char*> &operator*();
      bool operator==(const Field_Iterator &o) const;
      bool operator!=(const Field_Iterator &o) const;
  };
  class Line {
      CSV *csv {nullptr};
    private:
    protected:
    public:
      Line(CSV *csv);
      Line();
      typedef Field_Iterator iterator;
      iterator begin();
      iterator end();
  };
  class Line_Iterator {
    private:
      CSV *csv {nullptr};
      Line line;
    protected:
    public:
      Line_Iterator();
      Line_Iterator(CSV *csv);
      Line &operator*();
      Line_Iterator &operator++();
      bool operator==(const Line_Iterator &o) const;
      bool operator!=(const Line_Iterator &o) const;
  };

  class CSV {
    private:
      size_t record_ {0};
      size_t field_ {0};

      std::stack<std::pair<const char *, const char *> > input_stack;
      Memory::Buffer::Base &buffer;
      Callback::Base *cb {nullptr};
      Memory::Buffer::Vector field_buffer;
      bool break_out {false};
      bool next_is_last {false};
      bool next_is_first {true};
      Callback::Count count_cb;

      int cs { 0 };
      const char *p {nullptr};
      const char *pe{nullptr};
      const char *eof { nullptr };

      void lex();
    public:
      CSV(Memory::Buffer::Base &buffer, Callback::Base &cb);
      CSV();

      void push_input(const char *begin, const char *end);

      bool next_line();
      bool next_field(std::pair<const char *, const char *> &range);

      bool finished() const;
      void verify_finished() const;

      void start();

      Line_Iterator begin();
      Line_Iterator end();

  };
}

#include <boost/interprocess/file_mapping.hpp>
typedef boost::interprocess::file_mapping file_mapping;
#include <boost/interprocess/mapped_region.hpp>
typedef boost::interprocess::mapped_region mapped_region;

namespace RFC4180 {
  class Mapped_File_Private;
  class Mapped_File {
    private:
      std::string filename_;
      file_mapping mapping;
      mapped_region region;
      std::pair<const char*, const char*> range_ { nullptr, nullptr};
    public:
      Mapped_File();
      Mapped_File(const Mapped_File &) =delete;
      Mapped_File &operator=(const Mapped_File &) =delete;
      Mapped_File(const std::string &filename, bool read_only = true);
      Mapped_File(Mapped_File &&o);
      Mapped_File &operator=(Mapped_File &&o);
      const std::pair<const char*, const char*> &range() const;
      const char *begin() const;
      const char *end() const;
      size_t size() const;
  };

}

#endif
