#!/usr/bin/perl

use warnings;


sub B {
    my ($a, $b) = @_;
    if($a==0 || $b==0) { return 1; }
    return B($a-1, $b) + B($a, $b-1);
}

die "T" unless B(0,0) == 1;
die "T" unless B(10,0) == 1;
die "T" unless B(0,10) == 1;
die "T" unless B(1,1) == 2;
die "T" unless B(2,1) == 3;

sub X {  # x_S
    my ($a, $b) = @_;
    my $sum = 0;
    for(my $i=0; $i<=$a; $i++) {
        for(my $j=0; $j<=$b; $j++) {
            $sum += B($i, $j);
        }
    }
    return $sum;
}

die "T" unless X(0,0) == 1;
die "T" unless X(0,6) == 7;

sub Y {  # y_S
    my ($a, $b) = @_;
    my $sum = 0;
    for(my $i=0; $i<=$a; $i++) {
        for(my $j=0; $j<=$b; $j++) {
            $sum += $i * B($i, $j);
        }
    }
    return $sum;
}

sub Z {  # z_S
    my ($a, $b) = @_;
    my $sum = 0;
    for(my $i=0; $i<=$a; $i++) {
        for(my $j=0; $j<=$b; $j++) {
            $sum += $j * B($i, $j);
        }
    }
    return $sum;
}

sub pr {
    my $v = shift;
    $v = "$v";
    while(length($v)<6){ $v.=' '; }
    print($v);
}

sub test {
    my $msg = shift;
    my $callback = shift;
    
    die "$msg #1" unless $callback->(1, 1);
    die "$msg #2" unless $callback->(5, 3);
    die "$msg #3" unless $callback->(3, 2);
    die "$msg #4" unless $callback->(4, 8);
}

test("hypothesis", sub{my($a,$b)=@_; X($a,$b) == X($a-1,$b) + X($a,$b-1) + 1; });

# Easy to see this is true
test("[1]", sub{my($a,$b)=@_; X($a,$b) == X($a-1,$b) + X($a,$b-1) - X($a-1,$b-1) + B($a,$b); });
test("[1]", sub{my($a,$b)=@_; X($a,$b) - X($a-1,$b) - X($a,$b-1) + X($a-1,$b-1) == B($a,$b); });

# hypothesis => h2
test("h2",  sub{my($a,$b)=@_; 0 == X($a+1,$b+1) - X($a,$b+1) - X($a+1,$b) - 1; });
test("h2",  sub{my($a,$b)=@_; X($a,$b) == X($a+1,$b+1) - X($a,$b+1) - X($a+1,$b) + X($a,$b) - 1; });
test("h2",  sub{my($a,$b)=@_; X($a,$b) == B($a+1,$b+1) - 1; });

if(0) {
print("FRIST = S_x\n");
for(my $b=0; $b<8; $b++) {
for(my $a=0; $a<8; $a++) {
pr(X($a,$b)." ");
}
print("\n");
}

print("SECOND = S_y\n");
for(my $b=0; $b<8; $b++) {
for(my $a=0; $a<8; $a++) {
pr(Y($a,$b)." ");
}
print("\n");
}

print("THIRD = S_z\n");
for(my $b=0; $b<8; $b++) {
for(my $a=0; $a<8; $a++) {
pr(Z($a,$b)." ");
}
print("\n");
}
}

# This is always true
test("[2]", sub{my($a,$b)=@_; Y($a,$b) == Y($a-1,$b) + Y($a,$b-1) - Y($a-1,$b-1) + $a*B($a,$b); });

test("hy",  sub{my($a,$b)=@_; Y($a,$b) == Y($a-1,$b) + Y($a,$b-1) + X($a-1,$b); });
test("hy",  sub{my($a,$b)=@_; X($a-1,$b) == - Y($a-1,$b-1) + $a*B($a,$b); });
test("hy",  sub{my($a,$b)=@_; B($a,$b+1) - 1 == - Y($a-1,$b-1) + $a*B($a,$b); });
test("hy",  sub{my($a,$b)=@_; Y($a-1,$b-1) == $a*B($a,$b) - B($a,$b+1) + 1; });




