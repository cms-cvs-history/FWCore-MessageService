#!/bin/bash

pushd $LOCAL_TMP_DIR

status=0
  
rm -f u10_warnings.log u10_job_report.xml

cmsRun -j u10_job_report.xml -p $LOCAL_TEST_DIR/u10_cfg.py || exit $?
 
for file in u10_warnings.log u10_job_report.xml
do
  sed -i -r -f $LOCAL_TEST_DIR/filter-timestamps.sed $file
  diff $LOCAL_TEST_DIR/unit_test_outputs/$file $LOCAL_TMP_DIR/$file  
  if [ $? -ne 0 ]  
  then
    echo The above discrepancies concern $file 
    status=1
  fi
done

popd

exit $status
