#Note for insallation and run:
- Install some python libraries: sympy (http://sympy.org/), ply (www.dabeaz.com/ply/).
- Install Mathematica and Mathlink for Linux (www.wolfram.com/mathematica/).
- The pcre_tohampi.pm is runnable with Perl-5.10.0, other versions of Perl we haven't tested yet. If you are using another version of Perl, you can switch to Perl-5.10 by using Perlbrew (http://perlbrew.pl/). Note that after installing perlbrew, you may want to add this line source ~/perl5/perlbrew/etc/bashrc to your .bashrc file
- You also need JDK.

#To compile
   make
   
#To run SMC
   ./RunSMC inputfile
   

#Top-level directory structure
- README

  Basic documentation
  
- qifs_lexer.py

  The lexer file for SMC language. We use PLY library for parsing the input.

- qifs_parser.py

   The main parser of SMC. This module reads and parses the input, converts to CNF and does other tasks as described in the paper. Finally it generates the intermediate language for further processing
   
- core.py, utils.py

   Supporting files for the main parser.
   
- main.c
   
   Reads the generated intermediate file and does the counting job by querying to Mathematica.
   
- utils.c

   Provide the call and interaction with Mathematica.
   
- match_regex.c

   Handle regexes operations.
   
- contains.c

   Handle contains operations, both single contains and conjunction of contains.
   
- pcre_tohampi.pm, YAPE, run_pcre.pl, read_pcre.py

   Convert and translate pcre regexes to an intermediate language and transform to GFs domain.
   
- automaton

   Handle regexes and automatons conversons.
   
- smc.conf

   Includes serveral configurations for SMC.
- config.h
   
   Configure the attributes of the tool.
   
- rawutils.py, rawparser.py, rawlexer.py
   Translate C String operators, like strcasestr, strlen,... to SMC language.
   
