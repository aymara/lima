# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# CHANGELOG
# 26/10/2000 :
#  - creation d'apres un exemple de man:perlipc(3)
#  - toutes les variables "en dur"

# TODO
#  - mettre les variables et paths en parametres

# Emplacement des modules de gestion du reseau
use strict;
use IO::Socket;
use strict;
use IO::Socket;

my $port = $main::port;
    
my $analHandle = connectToServer("localhost", $port);
  
sub askToServer;
sub connectToServer;

my ($kidpid, $handle, $line);

unless (@ARGV == 3) { die "usage: $0 <language trigram> <file>" }
my ($command, $lang, $file) = @ARGV;
askToServer($analHandle, $command, $lang, $file);

sub askToServer
{
my ($handle, $command, $lang, $file) = @_;
  
  my $EOL="\n";
  my @result = ();
  print $handle "$command;$lang;$file$EOL";
  my $line = <$handle>;
  chop $line;chop $line;
  while ($line !~ /BEGIN/)
  {
#    print STDERR "Got line: $line\n";
    $line = <$handle>;
    chop $line;chop $line;
  }
  my $ok = 0;
  if ($line =~ /BEGIN ANSWER/) {$ok = 1;}
  else {print STDERR $line;}
  $line = <$handle>;
  chop $line;chop $line;
  while ($line !~ /END/)
  {
    if (!$ok) 
    {
      push @result, $line;
#      print STDERR "Got line: $line\n";
    }  
    else 
    {
      push @result, $line;
    }
    $line = <$handle>;
    chop $line;chop $line;
  }
  while ($line !~ /OK/)
  {
    $line = <$handle>;
    chop $line;chop $line;
  }
  return \@result;
}

sub connectToServer
{
  my ($host, $port) = @_;
  my ($handle, $line);
  
  # create a tcp connection to the specified host and port
  $handle = IO::Socket::INET->new(Proto     => "tcp",
                                  PeerAddr  => $host,
                                  PeerPort  => $port)
        or die "can't connect to port $port on $host: $!";
  
  $handle->autoflush(1);              # so output gets there right away
  
  $line = <$handle>;
  chop $line;chop $line;
  while ($line !~ /OK/)
  {
    $line = <$handle>;
    chop $line;chop $line;
  }
  return $handle;  
}
