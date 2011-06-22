#!/bin/bash
for m in {0..6}
do
    NUM=$(/bin/date -d "now - $((m+0)) months" "+%m-%Y")

echo $NUM >> kindgirls.log
wget -c   http://www.kindgirls.com/photo-archive/$NUM
GirlGallery=`cat $NUM |grep gallery |awk 'BEGIN { FS = "href=" } ; { print $2 }' |awk 'BEGIN { FS = " " } ; { print $1 }' |sed s/\"//g |sed 's/\\///'`
site="http://www.kindgirls.com"
	for img in ${GirlGallery}
	do
		echo $NUM
		for gallery in $img
		do
         echo $site$gallery
         NAMES=`curl -s $site/$gallery |grep gallery_list |awk 'BEGIN { FS = "href=" } ; { print $3 }'|awk 'BEGIN { FS = " " } ; { print $1 }' |sed s/\"//g |sed 's/\\///'`
			for LON in $NAMES
			do
            echo $LON
				DIR=`echo $LON |awk 'BEGIN { FS = "/" } ; { print $2 }'`
				mkdir $DIR &>/dev/null
            echo $site/$LON
            wget -c --limit-rate=25k $site/$LON --directory-prefix=`pwd`/$DIR
            echo $LON $DIR >>kg_logs-2.0.log
				done
				wait
			done
		done
	done
done
