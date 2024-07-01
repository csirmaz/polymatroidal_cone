#!/usr/bin/perl

# Strip lines marked for debugging
# Specify patterns to use as command-line arguments
# Also rewrite include directives for local files

my @patterns = @ARGV;

while(<STDIN>) {
    my $skip = undef;
    foreach my $p (@patterns) {
        $skip = 1 if $_ =~ m/\b$p\b/;
    }
    next if $skip;
    # s/#include "([^"]+)\.c"/#include "$1.strp.c"/g;
    s/#include "data\/([^"]+)\.c"/#include "..\/data\/$1.c"/g;
    print;
}
