## Note for insallation and run
- Install some python libraries: sympy (http://sympy.org/), ply (www.dabeaz.com/ply/).
- Install Mathematica and Mathlink for Linux (www.wolfram.com/mathematica/).
- The pcre_tohampi.pm is runnable with Perl-5.10.0, other versions of Perl we haven't tested yet. If you are using another version of Perl, you can switch to Perl-5.10 by using Perlbrew (http://perlbrew.pl/). Note that after installing perlbrew, you may want to add this line "source ~/perl5/perlbrew/etc/bashrc" to your .bashrc file
- You also need openjdk (either version 6 or 7 works, http://openjdk.java.net)

## To compile

Go to `src` folder and run `make`
	
	
	$ cd src
	$ make
	
   
## To run SMC

   ```
   ./RunSMC inputfile
   ```
   
## Replicating the results

1. 	Case Studies & Comparison (Section 4.2, 4.3, Table 5)	
	Run the script test_utilities.sh
	
	```
	$ ./test_utilities.sh
	```

	Output reported on console:
	
	- composition of constraints
	- time
	- bounds

## Top-level directory structure

### case_studies
   Includes several input/output of what we reported in our paper

### src
Source folder

1. automaton

Handles regexes and automatons conversons.
   
2. YAPE
   
Comes with regex library of HAMPI
   
3. Other files

- smc_lexer.py

  The lexer file for SMC language. We use PLY library for parsing the input.

- smc_parser.py

   The main parser of SMC. This module reads and parses the input, converts to CNF and does other tasks as described in the paper. Finally it generates the intermediate language for further processing
   
- core.py

   Contains main data structures for the parsing & inital processing steps.
   
- utils.py

   Contains main utilities for the parsing & inital processing steps. For example, we translalte the constraints to CNF form, build the concat dependency graph and export the constraint to intermediate constraint language in this file, which are called by smc_parser.py above.

- main.c
   
   Reads the generated intermediate file and does the counting job by querying to Mathematica. We read the constraint file twice: the first read is to initialize each variable with their proper length while the second time is to translate the string constraints to GF domain. Finally it returns the results corresponding to the query.
   
- utils.c

   Provide the call and interaction with Mathematica.
   
- match\_regex.c

   Handle regexes operations including handle with group of regex constraints.
   
- contains.c

   Handle contains operations, both single contains and conjunction of contains.
   
- pcre\_tohampi.pm, YAPE, run\_pcre.pl, read\_pcre.py

   Convert and translate pcre regexes to an intermediate language and transform to GFs domain.
   
- smc.conf, smc.kaluza.conf, smc.cs

   Includes serveral configurations for SMC in default, kaluza testcase and case studies respectively. You can configure **MAXN** - the maximum length of string, **ALPHABET_SIZE** - the number of character in alphabets, or the optimization options of concat operator.
   
- config.h
   
   Consists of several consts of the tool.
   
- rawutils.py, raw\_to\_smc_parser.py, raw\_to\_smc\_lexer.py
   
   Translate C String operators, like strcasestr, strlen,... to SMC language. The translation was mentioned in previous work [1], we just self-implemented it again here.

## References
- [1] A Symbolic Execution Framework for JavaScript. Prateek Saxena, Devdatta Akhawe, Steve Hanna, Stephen McCamant, Feng Mao, Dawn Song. 31st IEEE Symposium on Security and Privacy (Oakland 2010), May 2010.




   
