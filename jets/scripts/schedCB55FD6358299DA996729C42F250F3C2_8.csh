#!/bin/csh
# -------------------------------------- 
# Script generated on Tue Dec 04 10:42:03 EST 2018 by the STAR Unified Meta Scheduler 1.10.28 and submitted with:
# cd /star/u/zchang/jetAnalysis/jets; condor_submit /star/u/zchang/jetAnalysis/jets/scripts/schedCB55FD6358299DA996729C42F250F3C2_0_9.condor
# --------------------------------------



if ( $?SUMS_EXECUTION ) then
    goto USERCODESECTION
endif





/bin/echo 'We are starting on node: '`/bin/hostname`


# Preparing environment variables
ENVSETUPSECTION:

setenv EVENTS_START "-1"
setenv EVENTS_STOP "-1"
setenv FILEBASENAME "pt55_-1"
setenv FILELIST "/star/u/zchang/jetAnalysis/jets/scripts/schedCB55FD6358299DA996729C42F250F3C2_8.list"
setenv FILELIST_ALL "/star/u/zchang/jetAnalysis/jets/scripts/schedCB55FD6358299DA996729C42F250F3C2.list"
setenv INPUTFILECOUNT "1"
setenv JOBID "CB55FD6358299DA996729C42F250F3C2_8"
setenv JOBINDEX "8"
setenv LOGGING "STD"
setenv REQUESTID "CB55FD6358299DA996729C42F250F3C2"
setenv SUBMITATTEMPT "1"
setenv SUBMITTINGDIRECTORY "/star/u/zchang/jetAnalysis/jets"
setenv SUBMITTINGNODE "rcas6008.rcf.bnl.gov"
setenv SUBMIT_TIME "2018-12-04 15:42:03"
setenv SUMS_AUTHENTICATED_USER "zchang@rhic.bnl.gov"
setenv SUMS_USER "zchang"
setenv SUMS_nProcesses "10"
setenv SUMS_name "pt55_-1.job"
setenv INPUTFILE0 "/star/u/zchang/jetAnalysis/jets/lists_jets/pt55_-1.8.list"

# This big blobk is used by CONDOR where $HOME in not set

if ( ! $?USER ) then
    echo "USER is not defined"
    set USER=`id | sed "s/).*//" | sed "s/.*(//"`
endif
if ( ! $?HOME ) then
    echo "HOME is not defined"

    if ( -x /usr/bin/getent ) then
	# we have getent, should not be on aix, bsd, Tru64 however
	# will work for Linux
	echo "Using getent method"
	setenv HOME `/usr/bin/getent passwd $USER | /bin/sed 's|.*\:.*\:.*\:.*\:\([^\:]*\):.*|\1|'`
    else 
	set PTEST=`which perl`
	if ( "$PTEST" != "" ) then
	    echo "Using perl method"
	    # we have perl defined, we can get info from there
	    /bin/cat <<EOF >test$$.pl
my(\$user) = getpwuid(\$<);
@items = getpwnam(\$user);
print \$items[7];
EOF
	    setenv HOME `$PTEST test$$.pl` && /bin/rm  -f test$$.pl
	else
	    set CTEST=`which cc`
	    if ( "$CTEST" != "" ) then
		echo "Using C code method"
		# use C code for doing this
		/bin/cat <<EOF >test$$.c
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>


int main()
{
  struct passwd *info;
  uid_t          uid;

  /* get process UID */
  uid = getuid();
  info= getpwuid(uid);

  (void) printf("%s\n",info->pw_dir);
  return 0;
} 

EOF
		$CTEST -o test$$ test$$.c
		/bin/chmod +x test$$
		setenv HOME `./test$$` && /bin/rm -f test$$ test$$.c
	    else
		echo "We have no ways to define HOME and it is not defined"
		exit
	    endif
	endif
    endif
endif

echo "HOME is now $HOME"


# Default value for path if not defined.
if ( ! $?PATH ) then
   setenv PATH /usr/local/bin:/bin:/usr/bin
endif


/usr/bin/test -r $HOME/.cshrc && source $HOME/.cshrc

# Creating the scratch directory, return failure status
set SUMS_try=0
set SUMS_PAD=""

MKDTRY:
setenv SCRATCH "/tmp/$USER$SUMS_PAD/$JOBID"
/bin/mkdir -p $SCRATCH   >& /dev/null
set STS=$status
if (! -d $SCRATCH) then
       #test if porper UID
       set SUMS_IsUID=`(/usr/bin/test -O /tmp/$USER$SUMS_PAD && echo 1) || echo 0`
       if ( $SUMS_try == 0 &&  -e "/tmp/$USER$SUMS_PAD" && ! $SUMS_IsUID) then
            # First try, directory exists but is not owned by $USER
            # Create a different path and try again
            echo "Scheduler:: $SCRATCH not owned by $USER, trying alternative"
            @ try++
            set SUMS_seed=`/bin/date +%j%H%M%S`
            set SUMS_PAD=`expr $SUMS_seed  \* 25973 \% 100`
            goto MKDTRY
        else
		 echo "Scheduler:: Failed to create $SCRATCH on $HOST"
	         exit $STS
        endif

endif


##########################################################
# Detached script pid so that signals will propagate to  #
# child processes such as root4star, giving a chance to  #
# close out open files.                                  #
##########################################################
if ( ! $?SUMS_EXECUTION ) then
    echo "$$ We will spawn ourselves ($0 $*)"
    echo "$$ We BEGIN on [`/bin/date`]"

    onintr SUMS_SIGHANDLER
    ((setenv SUMS_EXECUTION 1 && nohup $0 $*) > $SCRATCH/.output.log ) >& $SCRATCH/.errror.log & 
    set PID=$!

    echo "$$ We are waiting for $PID "
    set loop=0
    set DELAY=1
    set CHLDPID=""
    set CHKP=10
    while (`/bin/ps --no-headers -p $PID` != "")
       if ( $loop % $CHKP == 0 ) then
           # check this only once every CHKP*DELAY seconds
           set GTEST=`/bin/ps -efH | /bin/grep $PID | /bin/grep -v $$ |  /bin/grep -v grep | /usr/bin/head -1 | /usr/bin/awk '{print $2}'`
           if ( "$GTEST" != "" && "$GTEST" != "$CHLDPID") then
               echo "$$ Grandchild PID is $GTEST - found in $loop x $DELAY seconds"
               if ( "$CHLDPID" == "") then
                   echo "$$ Establishing first watcher   on    `/bin/date`"
                   set DELAY=2
               else
                   echo "$$ Grandchild to watch has changed on `/bin/date`"
               endif
               set CHLDPID="$GTEST"
               echo "$$ Process tree is $$ (main) -> $PID (child) -> $CHLDPID (grandchild)"
               /bin/ps -l $CHLDPID
            endif
       endif
       @ loop++
       sleep $DELAY
   end
   cat $SCRATCH/.output.log >> /dev/stdout
    cat $SCRATCH/.errror.log >> /dev/stderr

# Delete the scratch directory
    /bin/rm -fr $SCRATCH
    echo "$$ Execution of ($0 $*) has ended, exiting normally"
    echo "$$ We END on [`/bin/date`]"
    exit 0

    SUMS_SIGHANDLER:
        echo "$$ We received a signal - sending TERM to PID=$PID group"
        kill -TERM -$PID
        echo "$$ We have sent an TERM signal to $PID and waiting for $CHLDPID on `/bin/date`"
        while (`/bin/ps --no-headers -p $CHLDPID` != "")
           sleep 2
        end
        echo "$$ Dumping the output/error channels ---->"
        cat $SCRATCH/.output.log >> /dev/stdout
        cat $SCRATCH/.errror.log >> /dev/stderr
        echo "$$ <---- done with output/error channels"
        echo "$$ We are now leaving with error on [`/bin/date`]"
        exit 1
endif

USERCODESECTION:
echo "$$ Stepping into the main SUMS wrapper program with [$*]"




#import packages 
echo '.... building sandbox'
setenv pwd `pwd` 
foreach object ( `find /star/u/zchang/jetAnalysis/jets/ -maxdepth 1 -name 'readMyJetCrsMc.C' ` ) 
 
 
          if($object != '/star/u/zchang/jetAnalysis/jets/') then 
 
                 foreach DIRObject ( `find $object`) 
 
                        setenv NewLink  $SCRATCH/`echo $DIRObject  | /bin/sed 's|/star/u/zchang/jetAnalysis/jets/||'` 
 
                        if(! -e $NewLink) then 
                             if(-f $DIRObject) then 
                                   /bin/ln -s  `echo $DIRObject | /bin/sed "s|\(^[^/]\)\(.*\)|$pwd/\1\2|"`  $NewLink 
                             else 
                                   /bin/mkdir -p   $NewLink 
                             endif 
                        endif 
               end 
 
 
        endif 
 
end 
foreach object ( `find /star/u/zchang/jetAnalysis/jets/ -maxdepth 1 -name '.sl73_gcc485' ` ) 
 
 
          if($object != '/star/u/zchang/jetAnalysis/jets/') then 
 
                 foreach DIRObject ( `find $object`) 
 
                        setenv NewLink  $SCRATCH/`echo $DIRObject  | /bin/sed 's|/star/u/zchang/jetAnalysis/jets/||'` 
 
                        if(! -e $NewLink) then 
                             if(-f $DIRObject) then 
                                   /bin/ln -s  `echo $DIRObject | /bin/sed "s|\(^[^/]\)\(.*\)|$pwd/\1\2|"`  $NewLink 
                             else 
                                   /bin/mkdir -p   $NewLink 
                             endif 
                        endif 
               end 
 
 
        endif 
 
end 


# Used for cleanning up the sandbox later
/bin/ls $SCRATCH > ${SCRATCH}/.sandboxFiles
echo ".sandboxFiles" >> ${SCRATCH}/.sandboxFiles



#Note: The default directory in which jobs start has been fix to $SCRATCH
cd $SCRATCH


echo "--------------------------------"
echo 'STAR Unified Meta Scheduler 1.10.28 we are starting in $SCRATCH :' `/bin/pwd`
echo "--------------------------------"
/bin/ls -l

###################################################
# User command BEGIN ----------------------------->

	
        setenv LHAPDF_DATA_PATH $HOME/public/pdfs/
        echo pt55_-1 readMyJetCrsMc.C
       /usr/bin/time root -b -q readMyJetCrsMc.C\(\"$INPUTFILE0\",\"pt55_-1\.$JOBINDEX\.crs\.root\"\)
    
# <------------------------------User command END
###################################################

#Sandbox cleanup script, so user will not copy back sandbox files by using *.*
foreach SANDBOXFILE (`/bin/cat ${SCRATCH}/.sandboxFiles`)
   echo "Cleaning up sandbox file:  ${SCRATCH}/${SANDBOXFILE}"
   /bin/rm -fr ${SCRATCH}/${SANDBOXFILE}
end


# Copy output files (if any where specified)
    
    ########### Copy Command Block ########### 
    echo "Starting copy block"
    foreach file ( $SCRATCH/pt55_-1.$JOBINDEX.crs.root )
         set i=0
         
         if ( -d $file ) then
            set cpCommand="/bin/cp -r $file /star/u/zchang/data05/Run12JetsCrsUe/pt55_-1/"
         else
            set cpCommand="/bin/cp -fp $file /star/u/zchang/data05/Run12JetsCrsUe/pt55_-1/"
         endif
         
         RETRY_CP:
         echo "Running copy command: " $cpCommand
         $cpCommand
         if ( $status && $i < 15 ) then
             echo "The copy failed, waiting to try again."
             @ i++
             sleep 60
             echo "Trying copy again at "`/bin/date`
             goto RETRY_CP
         endif

    end
    ##########################################
            