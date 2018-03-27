#!/usr/bin/perl
use strict;
use warnings;

our $PROJECTS_DIR = "./arduino";

use Time::HiRes;
use Data::Dumper;
use Plack::Request;

sub dispatch {
    my ($req) = @_;
    if ($req->path =~ m{^/autoupdate/(\w+)}) {
        my $name = $1;
        my $build_file = "$PROJECTS_DIR/$name/build/$name.ino.bin";
        -f $build_file or die "404 Not Found";
        my $auh = $req->header('x-au')//'%VER%0000%';
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
    my $t0 = Time::HiRes::time();
    my ($req) = Plack::Request->new(@_);
    my $ip = $req->address // '0.0.0.0';
    my @xff = split /,\s*/, $req->header('x-forwarded-for')//'';
    while ($ip =~ m{^(127\.)} and @xff) {
        $ip = shift(@xff) // '0.0.0.0';
    }
    my $res;
    eval {
        $res = dispatch($req);
        1;
    } or do {
        my $err = $@ // 'Error';
        my $code = 500;
        $err =~ s{^([345]\d\d) ?}{} and $code = $1;
        $res = $req->new_response($code);
        warn "$code $err";
        if ($code < 500) {
            $res->header("Cache-Control" => "public, max-age=10, s-maxage=10");
        }
        $res->body("$code $err");
    };
    $res->header(author => "oha[at]oha.it");
    my $t1 = Time::HiRes::time();
    printf STDERR "%s [%s] %.2fms %d %s %s\n", $ip, scalar gmtime, ($t1-$t0)*1000., $res->code, $req->method, $req->path;
    return $res->finalize;
};

