use ExtUtils::MakeMaker;
WriteMakefile(
      'NAME'              => 'SVMTool',
      'VERSION_FROM'      => 'lib/SVMTool/COMMON.pm', # finds $VERSION
      'PREREQ_PM' => {
                       Data::Dumper => 2.12,
                       Benchmark => 1.04,
                       strict => 1.02,
                       IO => 1.20,
                       IO::File => 1.09
                     }
);
