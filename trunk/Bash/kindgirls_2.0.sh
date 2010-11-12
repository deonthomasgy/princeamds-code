#!/bin/bash
for m in {0..1}
do
    NUM=$(/bin/date -d "now - $((m+0)) months" "+%m-%Y")

#NUM=$1
#GAL_ID=823

echo $NUM >> kindgirls.log
#wget -c http://www.kindgirls.com/archive/
wget -c   http://www.kindgirls.com/photo-archive/$NUM
GAL_ID=`cat $NUM |grep gallery |awk 'BEGIN { FS = "href=" } ; { print $2 }'  |awk 'BEGIN { FS = "src=\"/gal-" } ; { print $2 }' |awk 'BEGIN { FS = "/" } ; { print $1 }' |head -1`
GirlGallery=`cat $NUM |grep gallery |awk 'BEGIN { FS = "href=" } ; { print $2 }'  |awk 'BEGIN { FS = " " } ; { print $1 }' |awk 'BEGIN { FS = " " } ; { print $1 }' | sed s/\"/\/g `
site="http://www.kindgirls.com"	
	for img in ${GirlGallery}
	do
		echo $img
		for gallery in $img
		do
			NAMES=`curl -s $site$gallery |grep photo2 |awk 'BEGIN { FS = "href=" } ; { print $2 }'|awk 'BEGIN { FS = "src=" } ; { print $1 }' |awk 'BEGIN { FS = "><img" } ; { print $1 }' | sed s/\'/\/g`
			X=`echo $NAMES | sed 's/ /\n/g' |awk 'BEGIN { FS = "/" } ; { print $7 }' | head -1`
			Y=`echo $NAMES | sed 's/ /\n/g' |awk 'BEGIN { FS = "/" } ; { print $7 }' | tac | head -1`
			echo $gallery
			for LON in $NAMES
			do
				DIR=`echo $gallery |awk 'BEGIN { FS = "/" } ; { print $4 }'`
				mkdir $DIR &>/dev/null
				while [ $X -le $Y ];do
					echo http://www.kindgirls.com/gal-$GAL_ID/$DIR/${DIR}_$X.jpg 
					/usr/bin/wget -c -q http://www.kindgirls.com/gal-$GAL_ID/$DIR/${DIR}_$X.jpg -O $DIR/${DIR}_$X.jpg &
					#cp -a ${DIR}_$X.jpg $DIR
					echo ${DIR}_$X.jpg $DIR >>kg_logs
					X=$((X+1))
				done
				wait
			done
		done
	done
	#NUM=$((NUM+1))
done
