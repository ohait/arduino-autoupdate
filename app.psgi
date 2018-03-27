#!/usr/bin/perl
use strict;
use warnings;

use Data::Dumper;
use Plack::Request;

for my $dir (<arduino/*>) {
    $dir =~ m{/(.*)} or next;
    my $bin = "$dir/build/$1.ino.bin";
    -r $bin or next;
    print STDERR "$bin\n";
}

sub dispatch {
    my ($req) = @_;
    if ($req->path =~ m{^/autoupdate/(\w+)}) {
        my $name = $1;
        my $build_file = "arduino/$1/build/$1.ino.bin";
        -f $build_file or die "404 Not Found";
        my $auh = $req->header('x-au');
        my $bin = do {
            local $/;
            open my $F, "<:raw", $build_file;
            <$F>;
        };
        my ($client_ver) = $auh =~ m{%VER%(\d+)%};
        my ($server_ver) = $bin =~ m{%VER%(\d+)%};
        print STDERR "AU '$name' client: ".gmtime($client_ver).", server: ".gmtime($server_ver)."\n";
        if ($client_ver and $server_ver and $client_ver eq $server_ver) {
            die "304 Not Modified";
        }
        my $res = $req->new_response(200);
        $res->body($bin);
        return $res;
    }

    die "404 Not Found";
}

sub {
    my ($req) = Plack::Request->new(@_);
    my $res;
    eval {
        eval {
            $res = dispatch($req);
            1;
        } or do {
            my $err = $@;
            my $code = 500;
            $err =~ s{^(\d\d\d) }{} and $code = $1;
            $res = $req->new_response($code);
            $res->body($err);
        };
        $res->header(author => "oha[at]oha.it");
        1;
    } or do {
        my $err = $@ // 'Error';
        my $code = 500;
        $err =~ s{^([45]\d\d) ?}{} and $code = $1;
        $res = $req->new_response($code);
        warn "$code $err";
        if ($code < 500) {
            $res->header("Cache-Control" => "public, max-age=10, s-maxage=10");
        }
        $res->body("$code $err");
    };
    return $res->finalize;
};

