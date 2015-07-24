#!/usr/bin/perl


my $perl = `xde-identify --perl`;
my $wm = {};
print "evaulating: \$wm = $perl;\n";
eval "\$wm = $perl;";
foreach (keys %$wm) {
	print "$_ => $wm->{$_}\n";
}
