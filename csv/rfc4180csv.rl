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
#include <csv/rfc4180csv.h>

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cassert>
using namespace std;

%%{

machine csv;

action buffer_begin
{
  buffer.start(p);
}
action buffer_begin_next
{
  buffer.start(p+1);
}
action buffer_add_char
{
  buffer.resume(p);
  buffer.stop(p+1);
}
action buffer_stop
{
  buffer.stop(p);
}
action buffer_resume
{
  buffer.resume(p);
}
action buffer_end
{
  buffer.finish(p);
}
action buffer_end_prev
{
  buffer.finish();
}


# we use `if (break_out) { pe = p+1; }` instead of fbreak
# because fbreak skips all other actions for that state

action cb_record_first
{
  if (cb) {
    cb->set_break_out(false);
    cb->csv_record_first();
    break_out = cb->break_out();
    if (break_out) {
      pe = p+1;
    }
  }
}
action cb_record
{
  count_cb.csv_record();
  if (cb) {
    cb->set_csv_nr(count_cb.csv_nr());
    cb->set_break_out(false);
    cb->csv_record();
    break_out = cb->break_out();
    if (break_out) {
      pe = p+1;
    }
  }
}
action cb_record_last
{
  if (cb) {
    cb->set_break_out(false);
    cb->csv_record_last();
    break_out = cb->break_out();
    if (break_out) {
      pe = p+1;
    }
  }
}
action cb_field
{
  count_cb.csv_field();
  if (cb) {
    cb->set_csv_nf(count_cb.csv_nf());
    cb->set_break_out(false);
    cb->csv_field();
    break_out = cb->break_out();
    if (break_out) {
      pe = p+1;
    }
  }
}
action cb_field_last
{
  if (cb) {
    cb->set_break_out(false);
    cb->csv_field_last();
    break_out = cb->break_out();
    if (break_out) {
      pe = p+1;
    }
  }
}

lbreak = '\r\n' ;
fieldsep = ',' ;
quote = '"'  ;

# In the following, above each grammar the relevant ABNF snippet from the RFC
# is quoted as comment

# CRLF = CR LF ;as per section 6.1 of RFC 2234 [2]

CRLF = lbreak ;

# LF = %x0A ;as per section 6.1 of RFC 2234 [2]

LF = '\n' ;

# CR = %x0D ;as per section 6.1 of RFC 2234 [2]

CR = '\r' ;

# COMMA = %x2C

COMMA = fieldsep ;

# DQUOTE =  %x22 ;as per section 6.1 of RFC 2234 [2]

DQUOTE = quote ;

# TEXTDATA =  %x20-21 / %x23-2B / %x2D-7E

# using UTF8 variant below
#TEXTDATA = 0x20 .. 0x21 | 0x23 .. 0x2B | 0x2D..0x7E ;


# from
# RFC 3629                         UTF-8                     November 2003
# UTF-8, a transformation format of ISO 10646

#   UTF8-tail   = %x80-BF

UTF8_tail   = 0x80 .. 0xBF ;


#   UTF8-1      = %x00-7F

UTF8_1      = 0x00 .. 0x7F ;

#   UTF8-2      = %xC2-DF UTF8-tail

UTF8_2      = 0xC2 .. 0xDF UTF8_tail ;

#   UTF8-3      = %xE0 %xA0-BF UTF8-tail / %xE1-EC 2( UTF8-tail ) /
#                 %xED %x80-9F UTF8-tail / %xEE-EF 2( UTF8-tail )

UTF8_3      = 0xE0 0xA0..0xBF UTF8_tail | 0xE1..0xEC UTF8_tail{2,2} |
              0xED 0x80..0x9F UTF8_tail | 0xEE..0xEF UTF8_tail{2,2} ;


#   UTF8-4      = %xF0 %x90-BF 2( UTF8-tail ) / %xF1-F3 3( UTF8-tail ) /
#                 %xF4 %x80-8F 2( UTF8-tail )

UTF8_4      = 0xF0 0x90 .. 0xBF UTF8_tail {2,2} | 0xF1 .. 0xF3 UTF8_tail{3,3} |
                 0xF4 0x80 .. 0x8F UTF8_tail{2,2} ;

#   UTF8-char   = UTF8-1 / UTF8-2 / UTF8-3 / UTF8-4

UTF8_char = UTF8_1 | UTF8_2 | UTF8_3 | UTF8_4 ;

# UTF8-octets = *( UTF8-char )

UTF8_octets = UTF8_char * ;


# http://en.wikipedia.org/wiki/Byte_order_mark#UTF-8
# 'The Unicode Standard permits the BOM in UTF-8,[2] but does not require nor
# recommend its use.[3] Byte order has no meaning in UTF-8,[4]'
utf8_byte_order_mark = 0xef 0xbb 0xbf ;


TEXTDATA = UTF8_char - (COMMA | CR | LF | DQUOTE) ;


# non-escaped = *TEXTDATA

non_escaped = TEXTDATA* ;


# escaped = DQUOTE *(TEXTDATA / COMMA / CR / LF / 2DQUOTE) DQUOTE

escaped_not = TEXTDATA | COMMA | CR | LF ;

escaped = DQUOTE 
              @buffer_begin_next
            ( escaped_not >buffer_resume %buffer_stop |
              (DQUOTE DQUOTE @buffer_add_char )  ) ** DQUOTE %buffer_end_prev ;

# field = (escaped / non-escaped)

field = escaped | ( non_escaped >buffer_begin %buffer_end ) ;

# record = field *(COMMA field)

record = field (COMMA @cb_field field)* %cb_field %cb_field_last ;

# name = field

name = field ;

# header = name *(COMMA name)

header = name (COMMA name)* ;

# file = [header CRLF] record *(CRLF record) [CRLF]

#file = (header CRLF)? record (CRLF record)* (CRLF)? ;

file =  record >cb_record_first (CR LF @cb_record record)* (CRLF)? %cb_record %cb_record_last ;

main := utf8_byte_order_mark? file ;

}%%

#include <csv/rfc4180csv.h>

namespace RFC4180 {
  %% write data;

  CSV::CSV(Memory::Buffer::Base &buffer, Callback::Base &cb)
    : buffer(buffer), cb(&cb)
  {
    %% write init;
  }
  CSV::CSV()
    : buffer(field_buffer)
  {
    %% write init;
  }


  bool CSV::finished() const
  {
    return cs >= %%{write first_final;}%%;
  }

  void CSV::verify_finished() const
  {
    if (!finished())
      throw runtime_error("CSV not in final");
  }
  void CSV::push_input(const char *begin, const char *end)
  {
    eof = nullptr;
    input_stack.push(make_pair(begin, end));
  }
  void CSV::start()
  {
    lex();
    verify_finished();
  }
  void CSV::lex()
  {
    for (;;) {
      if (cb) {
        cb->csv_prepare_input(input_stack, p, pe);
      }
      pair<const char*, const char*> range(nullptr, nullptr);
      if (p >= pe) {
        if (input_stack.empty()) {
          break_out = false;
          return;
        } else {
          range = input_stack.top();
          p  = input_stack.top().first;
          pe = input_stack.top().second;
          input_stack.pop();
          if (input_stack.empty() && (!cb || !cb->csv_not_eof())) {
            eof = pe;
          } else
            eof = nullptr;
        }
      }
      buffer.resume(p);
      %% write exec;
      if (cs == %%{write error;}%%) {
        ostringstream o;
        o << "CSV lexer automaton in error state";
        o << " - context: |";
        o.write(p, std::min(20l, pe-p));
        o << "| (record " << (count_cb.csv_nr()+1) << ')';
        throw runtime_error(o.str());
      }
      buffer.stop(pe);
      if (pe < range.second) {
        input_stack.push(make_pair(pe, range.second));
        break_out = false;
        return;
      }
      if (break_out) {
        break_out = false;
        return;
      }
    }
  }
  bool CSV::next_line()
  {
    if (next_is_first) {
      next_is_first = false;
      return true;
    }
    struct Line_CB : public Callback::Null {
      bool first = true;
      bool last = false;
      void csv_record_first() override { first = true; }
      void csv_record()       override { break_out_ = true; }
      void csv_record_last()  override { last = true; }
    };
    Line_CB line_cb;
    cb = &line_cb;
    lex();
    if (p>=pe && pe == eof)
      return false;
    return !line_cb.last;
  }
  bool CSV::next_field(std::pair<const char *, const char *> &range)
  {
    if (next_is_last) {
      next_is_last = false;
      return false;
    }
    struct Field_CB : public Callback::Null {
      bool last = false;
      void csv_field()       override { break_out_ = true; }
      void csv_field_last()  override { last = true; }
    };
    Field_CB field_cb;
    cb = &field_cb;
    field_buffer.clear();
    lex();
    if (p>=pe && pe == eof)
      return false;
    range.first = field_buffer.begin();
    range.second = field_buffer.end();
    next_is_last = field_cb.last;
    return true;
  }


  namespace Callback {
    bool Null::csv_not_eof() { return false; }
    void Null::csv_prepare_input(
            std::stack<std::pair<const char *, const char *> > &input_stack,
            const char *p, const char *pe) {}
    void Null::csv_record_first() {}
    void Null::csv_record() {}
    void Null::csv_record_last() {}
    void Null::csv_field() {}
    void Null::csv_field_last() {}

    void Count::csv_record() { ++csv_nr_; csv_nf_ = 0; }
    void Count::csv_field()  { ++csv_nf_; }

  }

  Field_Iterator::Field_Iterator()
  {
  }
  Field_Iterator::Field_Iterator(CSV *csv)
    : csv(csv)
  {
    assert(csv);
    assert(this->csv);
    ++(*this);
  }
  Field_Iterator &Field_Iterator::operator++()
  {
    bool r = csv->next_field(p);
    if (!r)
      csv = nullptr;
    return *this;
  }
  pair<const char*, const char*> &Field_Iterator::operator*()
  {
    return p;
  }
  bool Field_Iterator::operator==(const Field_Iterator &o) const
  {
    return csv == o.csv;
  }
  bool Field_Iterator::operator!=(const Field_Iterator &o) const
  {
    return !(*this == o);
  }

  Line::Line()
  {
  }
  Line::Line(CSV *csv)
    : csv(csv)
  {
    assert(csv);
    assert(this->csv);
  }
  Field_Iterator Line::begin()
  {
    return Field_Iterator(csv);
  }
  Field_Iterator Line::end()
  {
    return Field_Iterator();
  }
  Line_Iterator::Line_Iterator()
  {
  }
  Line_Iterator::Line_Iterator(CSV *csv)
    : csv(csv), line(csv)
  {
    assert(csv);
    assert(this->csv);
    ++(*this);
  }
  Line &Line_Iterator::operator*()
  {
    return line;
  }
  Line_Iterator &Line_Iterator::operator++()
  {
    bool r = csv->next_line();
    if (!r)
      csv = nullptr;
    return *this;
  }
  bool Line_Iterator::operator==(const Line_Iterator &o) const
  {
    return csv == o.csv;
  }
  bool Line_Iterator::operator!=(const Line_Iterator &o) const
  {
    return !(*this == o);
  }

  Line_Iterator CSV::begin()
  {
    return Line_Iterator(this);
  }
  Line_Iterator CSV::end()
  {
    return Line_Iterator();
  }

}



#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
namespace ip = boost::interprocess;

namespace RFC4180 {
Mapped_File::Mapped_File()
{
}
Mapped_File::Mapped_File(const std::string &filename, bool read_only)
  : filename_(filename), 
    mapping(filename.c_str(), ip::read_only),
    region(mapping, ip::read_only, 0, fs::file_size(filename.c_str())),
    range_(static_cast<const char*>(region.get_address()),
           static_cast<const char*>(region.get_address()) + region.get_size())
{
  if (!read_only)
    throw logic_error("read write mapping not implemented yet");
}
Mapped_File::Mapped_File(Mapped_File &&o)
  : filename_(std::move(o.filename_)),
  mapping(std::move(o.mapping)),
  region(std::move(o.region)),
  range_(o.range_)
{
  o.range_.first = nullptr;
  o.range_.second = nullptr;
}
Mapped_File &Mapped_File::operator=(Mapped_File &&o)
{
  filename_ = std::move(o.filename_);
  mapping = std::move(o.mapping);
  region = std::move(o.region);
  range_ = o.range_;
  o.range_.first = nullptr;
  o.range_.second = nullptr;
  return *this;
}
const std::pair<const char*, const char*> &Mapped_File::range() const
{
  return range_;
}
const char *Mapped_File::begin() const
{
  return range_.first;
}
const char *Mapped_File::end() const
{
  return range_.second;
}
size_t Mapped_File::size() const
{
  return range_.second - range_.first;
}



}


