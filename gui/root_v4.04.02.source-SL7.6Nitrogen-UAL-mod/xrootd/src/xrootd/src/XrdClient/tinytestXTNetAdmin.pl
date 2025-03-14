#!/usr/local/bin/perl5.8

#  $Id: tinytestXTNetAdmin.pl,v 1.8 2005/02/08 12:48:59 furano Exp $

use XrdClientAdmin;
XrdClientAdmin::XrdInitialize("root://localhost/dummy", "DebugLevel 3\nConnectTimeout 60\nRequestTimeout 60");


$par = "/tmp/vmware.zip";
@ans = XrdClientAdmin::XrdStat($par);
print "\nThe answer of XrdClientAdmin::Stat($par) is: \"$ans[1]\"-\"$ans[2]\"-\"$ans[3]\"-\"$ans[4]\" \n\n\n";

$par = "/prod\n/store/PR/R14/AllEvents/0004/35/14.2.0b/AllEvents_00043511_14.2.0bV00.02E.root\n/tmp";
$ans = XrdClientAdmin::XrdSysStatX($par);
print "\nThe answer of XrdClientAdmin::SysStatX($par) is: \"$ans\" \n\n\n";

$par = "/store/PR/R14/AllEvents/0004/35/14.2.0b/AllEvents_00043511_14.2.0bV00.02E.root";
$ans = XrdClientAdmin::XrdExistFiles($par);
print "\nThe answer of XrdClientAdmin::ExistFiles($par) is: \"$ans\" \n\n\n";

#$par = "/prod\n";
#$ans = XrdClientAdmin::XrdExistDirs($par);
#print "\nThe answer of XrdClientAdmin::ExistDirs($par) is: \"$ans\" \n\n\n";

$par = "/prod\n/store/PR/R14/AllEvents/0004/35/14.2.0b/AllEvents_00043511_14.2.0bV00.02E.root\n/tmp";
$ans = XrdClientAdmin::XrdExistFiles($par);
print "\nThe answer of XrdClientAdmin::ExistFiles($par) is: \"$ans\" \n\n\n";

$par = "/prod\n/store\n/store/PR";
$ans = XrdClientAdmin::XrdExistDirs($par);
print "\nThe answer of XrdClientAdmin::ExistDirs($par) is: \"$ans\" \n\n\n";

$par = "/store/PR/R14/AllEvents/0004/35/14.2.0b/AllEvents_00043511_14.2.0bV00.02E.root";
$ans = XrdClientAdmin::XrdIsFileOnline("$par");
print "\nThe answer of XrdClientAdmin::IsFileOnline($par) is: \"$ans\" \n\n\n";

$par = "/prod/store/PRskims/R14/16.0.1a/AllEvents/23/AllEvents_2301.01.root";
$ans = XrdClientAdmin::XrdGetChecksum("$par");
print "\nThe answer of XrdClientAdmin::GetChecksum($par) is: \"$ans\" \n\n\n";

XrdClientAdmin::XrdTerminate();

