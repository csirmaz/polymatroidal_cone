#!/usr/bin/perl

use strict;


my $LIMIT_COORDINATES = 200; # 2*1000*1000;  # limit generated points to have smaller coordinates than this, or None

sub printpoint {
    my $p = shift;
    print "translate(v=[".join(',',@$p)."]){cube(size=[0.1,0.1,0.1], center=true);}\n";
}

sub func_u {
    my $x = shift;
    return ($x+0.5) * log($x);
}

sub func_f {
    my ($u, $v) = @_;
    return func_u($u+$v) - func_u($u) - func_u($v);
}

sub func_point {
    my ($u, $v) = @_;
    return [
        log($u) + func_f($u,$v), # x
        log($v) + func_f($u,$v), # y
        func_f($u,$v) # z
    ];
}

my $points = [
    [0,0,0],
];

for(my $u=0; $u<200; $u++) {
    for(my $v=0; $v<200; $v++) {
        my $u2 = 1. + $u; # 2.**(u/10.)
        my $v2 = 1. + $v; # 2.**(v/10.)
        my $p = func_point($u2, $v2);
        #if abs(p[2]) > LIMIT_COORDINATES: break
        if(abs($p->[0]) <= $LIMIT_COORDINATES and abs($p->[1]) <= $LIMIT_COORDINATES and abs($p->[2]) <= $LIMIT_COORDINATES) {
            push @$points, $p;
        }
    }
}

foreach my $p (@$points) {
    printpoint($p);
}
