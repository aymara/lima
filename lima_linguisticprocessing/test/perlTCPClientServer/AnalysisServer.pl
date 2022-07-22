# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

package AnalysisServer;

use strict;
use vars qw(@ISA);
use Net::Server::PreFork; # any personality will do
use Carp;
use FileHandle;
use IPC::Open2;

@ISA = qw(Net::Server::PreFork);


local $SIG{__DIE__} = sub
{
  print "[Connection closed by client]\n";
  return;
};
local $SIG{PIPE} = sub
{
    print "[Connection closed by client]\n";
  return;
};


#program version
my $VERSION=sprintf("%d.%02d", q$Revision$ =~ /(\d+)\.(\d+)/);

# Fin de ligne exigee par Internet
my $EOL="\n";
my $BLANK= $EOL x 2;

# config file may be given as argument -config=..
my $configFile=$main::config;
$configFile = $ENV{HOME}."/.analserverrc" unless $configFile;
open CONFIGFILE, "$configFile" || die "Cannot open configuration file $configFile\n";
my %config;
my @langs;

while (<CONFIGFILE>)
{
  chomp;
  if ((! /^#/) && ($_ ne ""))
  {
    my ($key, $arg) = split "=";

    if ($key eq "language"){push @langs, $arg;}
    $config{$key} = $arg;
  }
}
close CONFIGFILE;


my $pid;

eval {
  my $languagesOptions = "";
  foreach my $lang (@langs) {$languagesOptions .= "--language=$lang ";}
  $pid = open2(*Reader, *Writer, "analyzetextservercore $languagesOptions" );
};
if ($@ && $@ =~ /open2:/ ) {
  print "[error during analyzetextservercore opening]\n";
}

my $got = <Reader>;
if ( $got =~ /Connection closed by client/ )
{
  exit 2;
}

while ($got !~ /analyzer/)
{
  chomp $got;
  if ( $got eq "" )
  {
    exit 0;
  }
  $got = <Reader>;
}

# Definit les commandes connues du serveur et leurs nombres d'arguments
my %commands = (
    FINI => 0,
    BYE => 0,
    QUIT => 0,
    abd => 1,
    afxd => 1,
    atd => 1,
    aagxd => 1,
    aexd => 1,
);
# La liste des commandes, pour consultation future
my $commandsList = join "|", (keys %commands);

#Choix d'un port
my $port = $config{TCPPort};


AnalysisServer->run('port'=>$port);
exit;



sub process_request
{
  my $self = shift;
  eval {
    my $client = $self->{server}->{client};
    # Utilise pour verifier que on termine la connexion suite a une commande "fini"
    # du client
    my $ok = 0;
    $client->autoflush(1);
    print $client "This is the CEA LIST/LASTI Lima analyzer server version $VERSION$EOL";
    print $client "OK$EOL";
    $self->log(2, "[Connect from $self->{server}->{peerhost} ($self->{server}->{peeraddr})]\n") ;

#     local $SIG{ALRM} = sub { die "Timed Out!\n" };
#     my $timeout = 30; # give the user 30 seconds to type a line
#
#
#     my $previous_alarm = alarm($timeout);
#
    # Tant que le client envoie des lignes de commande
    while ( my $commandline = <$client>)
    {
      chop $commandline;chop $commandline;
      $self->log(2, "[Server $$ Received command is $commandline]\n");
      # Ignore les lignes vides
      if ($commandline eq "") {next;}
      # Si le client demande de finir, sort de la boucle
      if ($commandline =~ /^FINI|BYE|QUIT$/)
        {$ok = 1; last;}
          # On recupere les elements de la commande
      my @command = split ";", $commandline;
      # On extrait le nom de commande
        my $command = shift @command;
      # Erreur si le nom de commande n'est pas conu du serveur
      if ($command !~ /$commandsList/)
        {unknownCommandError($command);}
      else
      {
        my $lang = shift @command;
        my $file = shift @command;
        if ($command =~ /abd|afxd|atd|aagxd|aexd/i) {$command .= ";$lang;$file";}
        else
        {
          die "$$ ne devrait pas arriver ici !", $EOL;
        }
        $command .= "\n";
  #      print "'$command'";
        print Writer $command;
        my ($answer, $error, $errorstr);
        $error = 0;
        $answer = "BEGIN ANSWER$EOL";
        $errorstr = "BEGIN ERROR$EOL";
        my $got = <Reader>;
        chomp $got;
        while ($got !~ /^analyzer$/)
        {
          if ($got =~ /^ERROR:/)
          {
            $error = 1;
            $got =~ s/^ERROR://;
            $errorstr .= $got;
            $errorstr .= $EOL;
          }
          else
          {
            $answer .= $got;
            $answer .= $EOL;
          }
          $got = <Reader>;
        }
        $answer .= "END ANSWER$EOL";
        $errorstr .= "END ERROR$EOL";
        if ($error) {print $client $errorstr;}
        else {print $client $answer;}
      }
      print $client "OK$EOL";
#       alarm($timeout);
    }
#     alarm($previous_alarm);
    # indique au client que l'on ferme la connection
    # soit a sa demande, soit pour une autre raison
    if ($ok) {print $client "BYE$EOL";}
    else {print $client "KILLED$EOL";}
    close $client;

    sub unknownCommandError
    {
      my $command = shift;

      print $client "BEGIN ERROR$EOL";
      print $client "UnknownCommand: '$command'$EOL";
      print $client "END ERROR$EOL";
    }

    sub nbArgsError
    {
      my ($command, $expected, $got) = @_;

      print $client "BEGIN ERROR$EOL";
      print $client "NbArgs: $command expects $expected and got $got arguments$EOL";
      print $client "END ERROR$EOL";
    }

    sub signaleErreur
    {
      my $erreur = shift;

      print $client "BEGIN ERROR$EOL";
      print $client "$@", $EOL;
      print $client "END ERROR$EOL";
    }
  };
  if( $@=~/timed out/i )
  {
    print STDOUT "Timed Out.\n";
    return;
  }

}

1;
