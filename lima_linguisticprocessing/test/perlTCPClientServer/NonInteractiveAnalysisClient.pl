#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
#!/usr/bin/perl -w -s
# MemoryCollocationClient.pl
# client d'essai de gestionnaire de collocations
# Ga\"el de Chalendar
# 26/10/2000

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
