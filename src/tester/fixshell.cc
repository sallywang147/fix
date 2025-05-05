/*
Qs to discuss w/ labmates: 
 0. what does literal of a blob do? How is that number used? 
 1. what do we mean when we say we want to do 1+1 in Fix on 
 Arca? From main.rs, my understanding is that Arca seems to run ELF
 by making a system call. 
        - Fix would return a blob: do we want to compile the returned
        contents in a blob into an ELF and then make a system call in Arca? 
        - this iteration is based on the yes answer to the question above 

2. We start simple by runnning the return value (false) of our checkNumber example 
in Arca: https://github.com/sallywang147/LeanFunFix/tree/main/src/Example/no_main_examples

3. I'm probably missing complexities above, because the problem is likely 
more complex than that, otherwise this would've been solved by now. 
*/

#include <cstdlib>
#include <format>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>

#include "repository.hh"
#include "handle.hh"
#include "object.hh"
#include "base16.hh"
#include "option-parser.hh"

/*
we modify the decode function from fix.cc such that instead of returning
data into a fix API, we return a temp ELF file that can be called 
directrly by Arca. Would love to hear your thoughts on how to best approach this. 

A fix program can return something like below if the program evaluates to false: 
Fix (Expression (Object (Value (Blob (Literal Size : 4 contents: 0x00000000)))))
*/
void decode_to_elf(int argc, char* argv[]) {
        //the same as void decode function in fix.cc 
        OptionParser parser( "decode", commands["decode"].second );
        const char* handle = NULL;
        parser.AddArgument(
          "handle", OptionParser::ArgumentCount::One, [&]( const char* argument ) { handle = argument; } );
        parser.Parse( argc, argv );
        if ( !handle )
          exit( EXIT_FAILURE );
      
        try {
          auto result = base16::decode( handle );
          auto handle = Handle<Fix>::forge( result );
          //new changes starting below: 
          // it's a bit silly, because we treat output as a string 
          //instead of cout, we reformat the handle as a string
          std::stringstream handle_string;
          handle_string << handle;
          std::string rendered = handle_string.str();

           // find the "contents:" pattern
           const std::string search = "contents:";
           auto pos = rendered.find(search);
           if (pos == std::string::npos) {
               std::cerr << "Could not find 'contents:' in handle.\n";
                exit(EXIT_FAILURE);
            }

            std::string hex = rendered.substr(pos + needle.size(), 64); // max 32 bytes = 64 hex digits

            // Trim any trailing characters
            size_t hex_len = hex.find_first_not_of("0123456789abcdefABCDEF");
            if (hex_len != std::string::npos) {
            hex = hex.substr(0, hex_len);
            }

            // parse hex string into raw bytes: 0x00000000 in our case
            std::vector<uint8_t> bytes;
            for (size_t i = 0; i < hex.size(); i += 2) {
            std::string byte_str = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
            bytes.push_back(byte);
            }

            // write raw bytes to file
            std::temp_file = "temp.hex"
            std::ofstream out(temp_file, std::ios::binary);
            if (!out) {
            std::cerr << "Could not open output file.\n";
            exit(EXIT_FAILURE);
            }
         out.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
            out.close();

        //TODO: compile temp.hex to elf
          
        } catch ( std::runtime_error& e ) {
          cerr << "Error: " << e.what() << "\n";
          exit( EXIT_FAILURE );
        }
      
      
      
    

    


