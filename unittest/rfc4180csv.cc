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
#include <boost/test/unit_test.hpp>

#include <csv/rfc4180csv.h>

#include <cstring>
#include <array>
#include <sstream>
using namespace std;


BOOST_AUTO_TEST_SUITE( rfc4180csv )

  BOOST_AUTO_TEST_SUITE( pull )

    BOOST_AUTO_TEST_CASE( base )
    {
      const char inp[] =
"id,name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,bar,2011,2015\r\n"
"122,baz,2010,2015\r\n"
;
      vector<char> out;
      using namespace RFC4180;
      CSV csv;
      csv.push_input(inp, inp + strlen(inp));
      while (csv.next_line()) {
        std::pair<const char *, const char *> range;
        while (csv.next_field(range)) {
          out.push_back('<');
          out.insert(out.end(), range.first, range.second);
          out.push_back('>');
          const char fld[] = "[FLD]";
          out.insert(out.end(), fld, fld + sizeof(fld)-1);
          string s(range.first, range.second);
          //cout << "<" << s << ">[FLD]";
        }
        //cout << "[EOL]\n";
        const char eol[] = "[EOL]\n";
        out.insert(out.end(), eol, eol + sizeof(eol)-1);
      }
      //cout << "[EOF]\n";
      const char ef[] = "[EOF]\n";
      out.insert(out.end(), ef, ef + sizeof(ef)-1);
      string s(out.begin(), out.end());
      const char ref[] =
"<id>[FLD]<name>[FLD]<from>[FLD]<to>[FLD][EOL]\n"
"<233>[FLD]<foo>[FLD]<2014>[FLD]<2015>[FLD][EOL]\n"
"<42>[FLD]<bar>[FLD]<2011>[FLD]<2015>[FLD][EOL]\n"
"<122>[FLD]<baz>[FLD]<2010>[FLD]<2015>[FLD][EOL]\n"
"[EOF]\n"
;
      BOOST_CHECK_EQUAL(s, ref);
    }

    BOOST_AUTO_TEST_CASE( quote )
    {
      const char inp[] =
"id,name,from,to\r\n"
"233,\"foo\",\"2014\",2015\r\n"
"42,bar,2011,\"20,15\"\r\n"
"\"122\",baz,\"20\n10\",2015\r\n"
;
      vector<char> out;
      using namespace RFC4180;
      CSV csv;
      csv.push_input(inp, inp + strlen(inp));
      while (csv.next_line()) {
        std::pair<const char *, const char *> range;
        while (csv.next_field(range)) {
          out.push_back('<');
          out.insert(out.end(), range.first, range.second);
          out.push_back('>');
          const char fld[] = "[FLD]";
          out.insert(out.end(), fld, fld + sizeof(fld)-1);
          string s(range.first, range.second);
          //cout << "<" << s << ">[FLD]";
        }
        //cout << "[EOL]\n";
        const char eol[] = "[EOL]\n";
        out.insert(out.end(), eol, eol + sizeof(eol)-1);
      }
      //cout << "[EOF]\n";
      const char ef[] = "[EOF]\n";
      out.insert(out.end(), ef, ef + sizeof(ef)-1);
      string s(out.begin(), out.end());
      const char ref[] =
"<id>[FLD]<name>[FLD]<from>[FLD]<to>[FLD][EOL]\n"
"<233>[FLD]<foo>[FLD]<2014>[FLD]<2015>[FLD][EOL]\n"
"<42>[FLD]<bar>[FLD]<2011>[FLD]<20,15>[FLD][EOL]\n"
"<122>[FLD]<baz>[FLD]<20\n10>[FLD]<2015>[FLD][EOL]\n"
"[EOF]\n"
;
      BOOST_CHECK_EQUAL(s, ref);
    }

    BOOST_AUTO_TEST_CASE(escape)
    {
      const char inp[] =
"\"\"\"id\",name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,\"b\"\"ar\",2011,2015\r\n"
"122,baz,2010,\"\"\"2015\"\r\n"
;
      vector<char> out;
      using namespace RFC4180;
      CSV csv;
      csv.push_input(inp, inp + strlen(inp));
      while (csv.next_line()) {
        std::pair<const char *, const char *> range;
        while (csv.next_field(range)) {
          out.push_back('<');
          out.insert(out.end(), range.first, range.second);
          out.push_back('>');
          const char fld[] = "[FLD]";
          out.insert(out.end(), fld, fld + sizeof(fld)-1);
          string s(range.first, range.second);
          //cout << "<" << s << ">[FLD]";
        }
        //cout << "[EOL]\n";
        const char eol[] = "[EOL]\n";
        out.insert(out.end(), eol, eol + sizeof(eol)-1);
      }
      //cout << "[EOF]\n";
      const char ef[] = "[EOF]\n";
      out.insert(out.end(), ef, ef + sizeof(ef)-1);
      string s(out.begin(), out.end());
      const char ref[] =
"<\"id>[FLD]<name>[FLD]<from>[FLD]<to>[FLD][EOL]\n"
"<233>[FLD]<foo>[FLD]<2014>[FLD]<2015>[FLD][EOL]\n"
"<42>[FLD]<b\"ar>[FLD]<2011>[FLD]<2015>[FLD][EOL]\n"
"<122>[FLD]<baz>[FLD]<2010>[FLD]<\"2015>[FLD][EOL]\n"
"[EOF]\n"
;
      BOOST_CHECK_EQUAL(s, ref);
    }

    BOOST_AUTO_TEST_CASE( empty )
    {
      const char inp[] =
",name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,bar,,2015\r\n"
"122,baz,2010,\r\n"
;
      vector<char> out;
      using namespace RFC4180;
      CSV csv;
      csv.push_input(inp, inp + strlen(inp));
      while (csv.next_line()) {
        std::pair<const char *, const char *> range;
        while (csv.next_field(range)) {
          out.push_back('<');
          out.insert(out.end(), range.first, range.second);
          out.push_back('>');
          const char fld[] = "[FLD]";
          out.insert(out.end(), fld, fld + sizeof(fld)-1);
          string s(range.first, range.second);
          //cout << "<" << s << ">[FLD]";
        }
        //cout << "[EOL]\n";
        const char eol[] = "[EOL]\n";
        out.insert(out.end(), eol, eol + sizeof(eol)-1);
      }
      //cout << "[EOF]\n";
      const char ef[] = "[EOF]\n";
      out.insert(out.end(), ef, ef + sizeof(ef)-1);
      string s(out.begin(), out.end());
      const char ref[] =
"<>[FLD]<name>[FLD]<from>[FLD]<to>[FLD][EOL]\n"
"<233>[FLD]<foo>[FLD]<2014>[FLD]<2015>[FLD][EOL]\n"
"<42>[FLD]<bar>[FLD]<>[FLD]<2015>[FLD][EOL]\n"
"<122>[FLD]<baz>[FLD]<2010>[FLD]<>[FLD][EOL]\n"
"[EOF]\n"
;
      BOOST_CHECK_EQUAL(s, ref);
    }

    BOOST_AUTO_TEST_CASE( empty_quoted )
    {
      const char inp[] =
"\"\",name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,bar,\"\",2015\r\n"
"122,baz,2010,\"\"\r\n"
;
      vector<char> out;
      using namespace RFC4180;
      CSV csv;
      csv.push_input(inp, inp + strlen(inp));
      while (csv.next_line()) {
        std::pair<const char *, const char *> range;
        while (csv.next_field(range)) {
          out.push_back('<');
          out.insert(out.end(), range.first, range.second);
          out.push_back('>');
          const char fld[] = "[FLD]";
          out.insert(out.end(), fld, fld + sizeof(fld)-1);
          string s(range.first, range.second);
          //cout << "<" << s << ">[FLD]";
        }
        //cout << "[EOL]\n";
        const char eol[] = "[EOL]\n";
        out.insert(out.end(), eol, eol + sizeof(eol)-1);
      }
      //cout << "[EOF]\n";
      const char ef[] = "[EOF]\n";
      out.insert(out.end(), ef, ef + sizeof(ef)-1);
      string s(out.begin(), out.end());
      const char ref[] =
"<>[FLD]<name>[FLD]<from>[FLD]<to>[FLD][EOL]\n"
"<233>[FLD]<foo>[FLD]<2014>[FLD]<2015>[FLD][EOL]\n"
"<42>[FLD]<bar>[FLD]<>[FLD]<2015>[FLD][EOL]\n"
"<122>[FLD]<baz>[FLD]<2010>[FLD]<>[FLD][EOL]\n"
"[EOF]\n"
;
      BOOST_CHECK_EQUAL(s, ref);
    }

    BOOST_AUTO_TEST_CASE( utf8 )
    {
      const char inp[] =
u8"first,middle,last\r\n"
u8"xäÄüÜöÖß€y,a,b\r\n"
u8"a,xäÄüÜöÖß€y,b\r\n"
u8"xäÄüÜöÖß€y,a,b\r\n"
u8"a,x\u00e4\u00c4\u00fc\u00dc\u00f6\u00d6\u00df\u20acy,b\r\n"
;
      vector<char> out;
      using namespace RFC4180;
      CSV csv;
      csv.push_input(inp, inp + strlen(inp));
      while (csv.next_line()) {
        std::pair<const char *, const char *> range;
        while (csv.next_field(range)) {
          out.push_back('<');
          out.insert(out.end(), range.first, range.second);
          out.push_back('>');
          const char fld[] = "[FLD]";
          out.insert(out.end(), fld, fld + sizeof(fld)-1);
          string s(range.first, range.second);
          //cout << "<" << s << ">[FLD]";
        }
        //cout << "[EOL]\n";
        const char eol[] = "[EOL]\n";
        out.insert(out.end(), eol, eol + sizeof(eol)-1);
      }
      //cout << "[EOF]\n";
      const char ef[] = "[EOF]\n";
      out.insert(out.end(), ef, ef + sizeof(ef)-1);
      string s(out.begin(), out.end());
      const char ref[] =
"<first>[FLD]<middle>[FLD]<last>[FLD][EOL]\n"
"<xäÄüÜöÖß€y>[FLD]<a>[FLD]<b>[FLD][EOL]\n"
"<a>[FLD]<xäÄüÜöÖß€y>[FLD]<b>[FLD][EOL]\n"
"<xäÄüÜöÖß€y>[FLD]<a>[FLD]<b>[FLD][EOL]\n"
"<a>[FLD]<xäÄüÜöÖß€y>[FLD]<b>[FLD][EOL]\n"
"[EOF]\n"
;
      BOOST_CHECK_EQUAL(s, ref);
    }

    BOOST_AUTO_TEST_CASE( utf8_byte_order_mark )
    {
      const char inp[] = "\xef\xbb\xbf#,foo,bar";
      vector<char> out;
      using namespace RFC4180;
      CSV csv;
      csv.push_input(inp, inp + sizeof(inp)-1);
      BOOST_REQUIRE_EQUAL(csv.next_line(), true);
      std::pair<const char *, const char *> range;
      BOOST_REQUIRE_EQUAL(csv.next_field(range), true);
      BOOST_CHECK_EQUAL(string(range.first, range.second), "#");
      BOOST_REQUIRE_EQUAL(csv.next_field(range), true);
      BOOST_CHECK_EQUAL(string(range.first, range.second), "foo");
    }

  BOOST_AUTO_TEST_SUITE_END()

  BOOST_AUTO_TEST_SUITE( all )

    BOOST_AUTO_TEST_CASE( base )
    {
      const char inp[] =
"id,name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,bar,2011,2015\r\n"
"122,baz,2010,2015\r\n"
;
      using namespace RFC4180;
      struct CB : public Callback::Null {
        Memory::Buffer::Vector buffer;
        vector<char> out;

        void insert_buffer()
        {
          out.push_back('|');
          out.insert(out.end(), buffer.begin(), buffer.end());
          out.push_back('|');
        }
        void csv_record_first()
        {
          const char m[] = "{FST}";
          out.insert(out.end(), m, m + strlen(m));
        }
        void csv_record()
        {
          const char m[] = "{REC}";
          out.insert(out.end(), m, m + strlen(m));
          out.push_back('\n');
        }
        void csv_record_last()
        {
          const char m[] = "{EOF}";
          out.insert(out.end(), m, m + strlen(m));
          out.push_back('\n');
        }
        void csv_field()
        {
          insert_buffer();
          const char m[] = "{FLD}";
          out.insert(out.end(), m, m + strlen(m));
        }
        void csv_field_last()
        {
          const char m[] = "{EOL}";
          out.insert(out.end(), m, m + strlen(m));
        }
      };
      CB cb;
      CSV csv(cb.buffer, cb);
      csv.push_input(inp, inp + strlen(inp));
      csv.start();
      string s(cb.out.begin(), cb.out.end());
      const char ref[] =
"{FST}|id|{FLD}|name|{FLD}|from|{FLD}|to|{FLD}{EOL}{REC}\n"
"|233|{FLD}|foo|{FLD}|2014|{FLD}|2015|{FLD}{EOL}{REC}\n"
"|42|{FLD}|bar|{FLD}|2011|{FLD}|2015|{FLD}{EOL}{REC}\n"
"|122|{FLD}|baz|{FLD}|2010|{FLD}|2015|{FLD}{EOL}{REC}\n"
"||{FLD}{EOL}{REC}\n{EOF}\n"
        ;
      BOOST_CHECK_EQUAL(s, ref);
    }

    BOOST_AUTO_TEST_CASE( line_numbers )
    {
      const char inp[] =
"id,name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,bar,2011,2015\r\n"
"122,baz,2010,2015\r\n"
;
      using namespace RFC4180;
      struct CB : public Callback::Null {
        Memory::Buffer::Vector buffer;
        vector<char> out;

        void insert_buffer()
        {
          out.push_back('|');
          out.insert(out.end(), buffer.begin(), buffer.end());
          out.push_back('|');
        }
        void csv_record_first()
        {
          const char m[] = "{FST}";
          out.insert(out.end(), m, m + strlen(m));
        }
        void csv_record()
        {
          const char m[] = "{REC}";
          out.insert(out.end(), m, m + strlen(m));
          ostringstream o;
          o << '#' << csv_nr_;
          string s(o.str());
          out.insert(out.end(), s.begin(), s.end());
          out.push_back('\n');
        }
        void csv_record_last()
        {
          const char m[] = "{EOF}";
          out.insert(out.end(), m, m + strlen(m));
          out.push_back('\n');
        }
        void csv_field()
        {
          insert_buffer();
          const char m[] = "{FLD}";
          out.insert(out.end(), m, m + strlen(m));
          ostringstream o;
          o << '@' << csv_nf_;
          string s(o.str());
          out.insert(out.end(), s.begin(), s.end());
        }
        void csv_field_last()
        {
          const char m[] = "{EOL}";
          out.insert(out.end(), m, m + strlen(m));
        }
      };
      CB cb;
      CSV csv(cb.buffer, cb);
      csv.push_input(inp, inp + strlen(inp));
      csv.start();
      string s(cb.out.begin(), cb.out.end());
      const char ref[] =
"{FST}|id|{FLD}@1|name|{FLD}@2|from|{FLD}@3|to|{FLD}@4{EOL}{REC}#1\n"
"|233|{FLD}@1|foo|{FLD}@2|2014|{FLD}@3|2015|{FLD}@4{EOL}{REC}#2\n"
"|42|{FLD}@1|bar|{FLD}@2|2011|{FLD}@3|2015|{FLD}@4{EOL}{REC}#3\n"
"|122|{FLD}@1|baz|{FLD}@2|2010|{FLD}@3|2015|{FLD}@4{EOL}{REC}#4\n"
"||{FLD}@1{EOL}{REC}#5\n"
"{EOF}\n"
        ;
      BOOST_CHECK_EQUAL(s, ref);
    }

    BOOST_AUTO_TEST_CASE ( non_utf8_error )
    {
      // 0x92, 146, curved quote in Windows CP-1252, illegal in UTF8
      const char inp[] = "foo,the cat\x92s whiskers,foo\n1,2,3\n";
      using namespace RFC4180;
      using namespace Memory;
      struct CB : public Callback::Null {
        Buffer::Vector buffer;
      };
      CB cb;
      CSV csv(cb.buffer, cb);
      csv.push_input(inp, inp + sizeof(inp)-1);
      bool caught = false;
      try {
        csv.start();
      } catch (std::runtime_error) {
        caught = true;
      }
      BOOST_CHECK_EQUAL(caught, true);
    }

    BOOST_AUTO_TEST_CASE( utf8_byte_order_mark )
    {
      const char inp[] = "\xef\xbb\xbf#,foo,bar";
      vector<char> out;
      using namespace RFC4180;
      using namespace Memory;
      struct CB : public Callback::Null {
        Buffer::Vector buffer;
        array<unsigned, 2> a = {{0}};
        void csv_field()
        {
          if (csv_nr() == 0) {
            switch (csv_nf()) {
              case 1:
                {
                  BOOST_CHECK_EQUAL(string(buffer.range().first, buffer.range().second), "#");
                  ++a[0];
                }
                break;
              case 2:
                {
                  BOOST_CHECK_EQUAL(string(buffer.range().first, buffer.range().second), "foo");
                  ++a[1];
                }
                break;
            }
          }
        }
      };
      CB cb;
      CSV csv(cb.buffer, cb);
      csv.push_input(inp, inp + sizeof(inp)-1);
      csv.start();
      BOOST_CHECK_EQUAL(cb.a[0], 1);
      BOOST_CHECK_EQUAL(cb.a[1], 1);
    }

    BOOST_AUTO_TEST_CASE( prepare_buffer)
    {
      const char inp[] =
"id,name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,bar,2011,2015\r\n"
"122,baz,2010,2015\r\n"
;
      using namespace RFC4180;
      struct CB : public Callback::Null {
        Memory::Buffer::Vector buffer;
        vector<char> out;
        vector<char> in_;
        stringstream stream_;

        CB(const char *inp)
          :
            in_(3)
        {
          stream_ << inp;
        }

        void insert_buffer()
        {
          out.push_back('|');
          out.insert(out.end(), buffer.begin(), buffer.end());
          out.push_back('|');
        }
        void csv_record_first() override
        {
          const char m[] = "{FST}";
          out.insert(out.end(), m, m + strlen(m));
        }
        void csv_record() override
        {
          const char m[] = "{REC}";
          out.insert(out.end(), m, m + strlen(m));
          out.push_back('\n');
        }
        void csv_record_last() override
        {
          const char m[] = "{EOF}";
          out.insert(out.end(), m, m + strlen(m));
          out.push_back('\n');
        }
        void csv_field() override
        {
          insert_buffer();
          const char m[] = "{FLD}";
          out.insert(out.end(), m, m + strlen(m));
        }
        void csv_field_last() override
        {
          const char m[] = "{EOL}";
          out.insert(out.end(), m, m + strlen(m));
        }
        void csv_prepare_input(
            std::stack<std::pair<const char *, const char *> > &input_stack,
            const char *p, const char *pe) override
        {
          if (p>=pe) {
            if (!stream_.eof()) {
              stream_.read(in_.data(), in_.size());
              size_t n = stream_.gcount();
              input_stack.push(make_pair(in_.data(), in_.data() + n));
            }
          }
        }
        bool csv_not_eof() override
        {
          return !stream_.eof();
        }
      };
      CB cb(inp);
      CSV csv(cb.buffer, cb);
      csv.start();
      string s(cb.out.begin(), cb.out.end());
      const char ref[] =
"{FST}|id|{FLD}|name|{FLD}|from|{FLD}|to|{FLD}{EOL}{REC}\n"
"|233|{FLD}|foo|{FLD}|2014|{FLD}|2015|{FLD}{EOL}{REC}\n"
"|42|{FLD}|bar|{FLD}|2011|{FLD}|2015|{FLD}{EOL}{REC}\n"
"|122|{FLD}|baz|{FLD}|2010|{FLD}|2015|{FLD}{EOL}{REC}\n"
"||{FLD}{EOL}{REC}\n{EOF}\n"
        ;
      BOOST_CHECK_EQUAL(s, ref);
    }


    BOOST_AUTO_TEST_CASE( prepare_buffer_quote)
    {
      const char inp[] =
"\"i\",name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,bar,2011,2015\r\n"
"122,baz,2010,2015\r\n"
;
      using namespace RFC4180;
      struct CB : public Callback::Null {
        Memory::Buffer::Vector buffer;
        vector<char> out;
        vector<char> in_;
        stringstream stream_;

        CB(const char *inp)
          :
            in_(3)
        {
          stream_ << inp;
        }

        void insert_buffer()
        {
          out.push_back('|');
          out.insert(out.end(), buffer.begin(), buffer.end());
          out.push_back('|');
        }
        void csv_record_first() override
        {
          const char m[] = "{FST}";
          out.insert(out.end(), m, m + strlen(m));
        }
        void csv_record() override
        {
          const char m[] = "{REC}";
          out.insert(out.end(), m, m + strlen(m));
          out.push_back('\n');
        }
        void csv_record_last() override
        {
          const char m[] = "{EOF}";
          out.insert(out.end(), m, m + strlen(m));
          out.push_back('\n');
        }
        void csv_field() override
        {
          insert_buffer();
          const char m[] = "{FLD}";
          out.insert(out.end(), m, m + strlen(m));
        }
        void csv_field_last() override
        {
          const char m[] = "{EOL}";
          out.insert(out.end(), m, m + strlen(m));
        }
        void csv_prepare_input(
            std::stack<std::pair<const char *, const char *> > &input_stack,
            const char *p, const char *pe) override
        {
          if (p>=pe) {
            if (!stream_.eof()) {
              stream_.read(in_.data(), in_.size());
              size_t n = stream_.gcount();
              input_stack.push(make_pair(in_.data(), in_.data() + n));
            }
          }
        }
        bool csv_not_eof() override
        {
          return !stream_.eof();
        }
      };
      CB cb(inp);
      CSV csv(cb.buffer, cb);
      csv.start();
      string s(cb.out.begin(), cb.out.end());
      const char ref[] =
"{FST}|i|{FLD}|name|{FLD}|from|{FLD}|to|{FLD}{EOL}{REC}\n"
"|233|{FLD}|foo|{FLD}|2014|{FLD}|2015|{FLD}{EOL}{REC}\n"
"|42|{FLD}|bar|{FLD}|2011|{FLD}|2015|{FLD}{EOL}{REC}\n"
"|122|{FLD}|baz|{FLD}|2010|{FLD}|2015|{FLD}{EOL}{REC}\n"
"||{FLD}{EOL}{REC}\n{EOF}\n"
        ;
      BOOST_CHECK_EQUAL(s, ref);
    }

  BOOST_AUTO_TEST_SUITE_END()


  BOOST_AUTO_TEST_SUITE( iterator )

    BOOST_AUTO_TEST_CASE ( basic )
    {
      struct D {
        const std::array<unsigned, 5> array = {{1, 2, 3, 4, 5}};
        typedef const unsigned* iterator;
        iterator begin()  { return array.begin(); }
        iterator end() { return array.end(); }
      };
      D d;
      unsigned t = 0;
      for (unsigned x : d) {
        BOOST_CHECK_EQUAL(x, ++t);
      }
      BOOST_CHECK_EQUAL(t, 5);
    }

    BOOST_AUTO_TEST_CASE ( csv )
    {
      const char inp[] =
"id,name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,bar,2011,2015\r\n"
"122,baz,2010,2015\r\n"
;
      using namespace RFC4180;
      CSV csv;
      csv.push_input(inp, inp + strlen(inp));
      Line_Iterator begin(&csv);
      Line_Iterator end;
      Line l(*begin);
      Field_Iterator fb(l.begin());
      Field_Iterator fe(l.end());
      auto p(*fb);
      string s(p.first, p.second);
      BOOST_CHECK_EQUAL(s, "id");
      ++fb;
      auto q(*fb);
      string t(q.first, q.second);
      BOOST_CHECK_EQUAL(t, "name");
      ++begin;
      Line l2(*begin);
      Field_Iterator fb2(l2.begin());
      auto r(*fb2);
      string u(r.first, r.second);
      BOOST_CHECK_EQUAL(u, "233");
    }

    BOOST_AUTO_TEST_CASE( full )
    {
      const char inp[] =
"id,name,from,to\r\n"
"233,foo,2014,2015\r\n"
"42,bar,2011,2015\r\n"
"122,baz,2010,2015\r\n"
;
      vector<char> out;
      using namespace RFC4180;
      CSV csv;
      csv.push_input(inp, inp + strlen(inp));
      for (auto line : csv) {
        for (auto range : line) {
          out.push_back('<');
          out.insert(out.end(), range.first, range.second);
          out.push_back('>');
          const char fld[] = "[FLD]";
          out.insert(out.end(), fld, fld + sizeof(fld)-1);
          string s(range.first, range.second);
          //cout << "<" << s << ">[FLD]";
        }
        //cout << "[EOL]\n";
        const char eol[] = "[EOL]\n";
        out.insert(out.end(), eol, eol + sizeof(eol)-1);
      }
      //cout << "[EOF]\n";
      const char ef[] = "[EOF]\n";
      out.insert(out.end(), ef, ef + sizeof(ef)-1);
      string s(out.begin(), out.end());
      const char ref[] =
"<id>[FLD]<name>[FLD]<from>[FLD]<to>[FLD][EOL]\n"
"<233>[FLD]<foo>[FLD]<2014>[FLD]<2015>[FLD][EOL]\n"
"<42>[FLD]<bar>[FLD]<2011>[FLD]<2015>[FLD][EOL]\n"
"<122>[FLD]<baz>[FLD]<2010>[FLD]<2015>[FLD][EOL]\n"
"[EOF]\n"
;
      BOOST_CHECK_EQUAL(s, ref);
    }

  BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
