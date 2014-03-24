#use the current directory for library includes
# if you have the pcre_tohampi.pm and YAPE folder 
# somewhere else, change this accordingly
use lib '.'; 
use pcre_tohampi; # include the library

#create a object from the particular regular expression.
my $regex = $ARGV[0];
$regex = eval 'qr'.$regex;
print "error: $@\n" if $@;
my $p=pcre_tohampi->new($regex);
$p->parse;
my ($min, $max)=$p->getminmax();
open (newfile, ">pcre_output.txt");
print newfile $p->tothehampi();
die "error calculating minimum for regex" if $min<0;
  
