/*
N2kStream.cpp

Copyright (c) 2015-2021 Timo Lappalainen, Kave Oy, www.kave.fi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

I/O stream used in the NMEA2000 libraries.

*/
#include "N2kStream.h"
#include <string.h>

#ifdef ARDUINO
// Arduino uses its own implementation.
#ifdef SERIAL_OVER_UDP
   void N2kStream::prepare() {
      if (! sending) {
             beginPacket(remote_ip, remote_port);
             sending = true;
      }
   };

   void N2kStream::send() {
      endPacket();
      sending = false;
   };

 // Write data to stream.
   size_t N2kStream::write(const uint8_t* data, size_t size) {
      prepare();
      return WiFiUDP::write(data,size);
   };

   // Print string to stream.
   size_t N2kStream::print(const char* str) {
      prepare();
      return WiFiUDP::print(str);
   }


   // Flash stored string stream for AVR platforms.
   size_t N2kStream::print(const __FlashStringHelper* str) {
      prepare();
      return WiFiUDP::print(str);
   };

   size_t N2kStream::println(const __FlashStringHelper* str) {
      prepare();
      size_t n = WiFiUDP::println(str)  ;
      send();
      return n;
   };
 

   // Print value to stream.
   size_t N2kStream::print(int val, uint8_t radix) {
      prepare();
      return WiFiUDP::print(val, radix);
   }

   // Print string and newline to stream.
   size_t N2kStream::println(const char *str) {
      prepare();
      size_t n = WiFiUDP::println(str) ;
      send();
      return n;      
   }

   // Print value and newline to stream.
   size_t N2kStream::println(int val, uint8_t radix ) {
      prepare();
      size_t n = WiFiUDP::println(val,radix);
      send();
      return n; 
   }

   // Returns first byte if incoming data, or -1 on no available data.
   int N2kStream::read() { 
      WiFiUDP::parsePacket();
      return WiFiUDP::read();
      };

   int N2kStream::peek() { 
      WiFiUDP::parsePacket();
      return WiFiUDP::peek();
   };

   int N2kStream::read(unsigned char* buffer, size_t len) {
      WiFiUDP::parsePacket();
      return WiFiUDP::read(buffer,len);
   };


   int N2kStream::read(char* buffer, size_t len) {
      WiFiUDP::parsePacket();
      return WiFiUDP::read(buffer,len);
   }


#endif

#else
size_t N2kStream::print(const char *str) {
   if(str == 0)
      return 0;

   return write(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

#if defined(__AVR__)
size_t N2kStream::print(const __FlashStringHelper* str) {
   size_t bytes_written = 0;
   PGM_P src = (PGM_P) str;
   for(uint8_t c = pgm_read_byte(src); c != 0; ++src, ++bytes_written)
      write(&c, 1);

   return bytes_written;
}

size_t N2kStream::print(const __FlashStringHelper* str) {
   return print(str) + print("\r\n");
}
#endif


size_t N2kStream::print(int val, uint8_t radix) {

   if(val == 0) {
      // 0 is always 0 regardless of radix.
      return write(reinterpret_cast<const uint8_t*>("0"), 1);
   }

   // Enough for binary representation.
   char buf[8 * sizeof(val) + 1];
   char *ptr = &buf[sizeof(buf) - 1];
   *ptr = '\0';

   do {
      *--ptr="0123456789abcdef"[val % radix];
      val /= radix;
   } while(val != 0);

   return print(ptr);
}

size_t N2kStream::println(const char *str) {
   return print(str) + print("\r\n");
}

size_t N2kStream::println(int val, uint8_t radix) {
   return print(val, radix) + print("\r\n");
}

#endif
