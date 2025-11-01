#!/usr/bin/perl

# Check if vertices disappear in higher iterations
# Usage: cat reports/hullA.txt reports/hullB.txt | $0

use strict;
use Data::Dumper;

my $hull = 0;
my $vertices = {};

while(<STDIN>) {
    if(/VERTICES OF THE CONVEX HULL/) { $hull++; next; }
    if(/^\/\/ vertex #/ && $_ !~ /from #/) {
        /\[\s*([0-9]+),\s*([0-9]+),\s*([0-9]+)\]\s*\(\s*([\-0-9]+)\)/ or die "cannot parse coords: $_";
        my $x = $1;
        my $y = $2;
        my $z = $3;
        my $iter = $4;
        my $key = "$x:$y:$z($iter)";
        $vertices->{$key}->{$hull} = 1;
    }
}

# print(Dumper($vertices));

foreach my $k (keys(%$vertices)) {
    my $d = $vertices->{$k};
    if($d->{'1'}) {
        print "Vertex $k has disappeared\n" unless $d->{'2'};
    }elsif($d->{'2'}) {
        1;
    }else{
        die "wrong data";
    }
}
