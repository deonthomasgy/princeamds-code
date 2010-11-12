#!/bin/bash
# http://babene.moskva.com/ ripper

USER_ID=$1
USER_PART=$2
GET_ALL_PAGES()
{
	PAGE_ID=$USER_ID
	PAGE_PART=$USER_PART
	ID="http://babene.moskva.com/?do=Rub&id=$PAGE_ID&Part=$PAGE_PART"
	GET=`curl -s $ID |sed 's/Rub&/\n/g' |grep Part |grep black |sed 's/"/\n/g' |grep Part |sed 's/&/\n/g'| grep Part | sed '1!G;h;$!d' |sed 1q |sed 's/Part=//g'`
	while [ $PAGE_PART -le $GET ]
	do
		echo $PAGE_PART
		echo "PAGE_ID=$PAGE_ID&PAGE_PART=$PAGE_PART" >>logs_$PAGE_ID
		ID="http://babene.moskva.com/?do=Rub&id=$PAGE_ID&Part=$PAGE_PART"
		GET_PAGE_DATA
		PAGE_PART=$[$PAGE_PART+1]
		if [ $GET = $[$PAGE_PART-1] ] ; then
			GET_LAST_PAGE
		fi
		
	done
}

GET_LAST_PAGE()
{
	GET=`curl -s $ID |sed 's/Rub&/\n/g' |grep Part |grep black |sed 's/"/\n/g' |grep Part |sed 's/&/\n/g'| grep Part | sed '1!G;h;$!d' |sed 1q |sed 's/Part=//g'`
}

GET_IMAGES()
{
	DIRECTORY="PAGE_ID_$PAGE_ID/"
	if [ ! -d "$DIRECTORY" ]; then
		mkdir -p $DIRECTORY
	fi
	ITEM_PAGE="http://babene.moskva.com/popup.php?c=classLenta&m=Wallpapers&do=Item&"
	Join_URL="$ITEM_PAGE$item_id&Size=$value"
	URL=`curl -s $Join_URL |sed 's/\;/\n/g' |grep urlx |head -1 |tr '[a-z]' '[n-za-m]' |sed 's/ /\n/g' |grep http | tr "'" " " |sed 's/ //g'`
	echo $ID
	echo $URL
	echo $URL>>logs_$PAGE_ID
	wget -c $URL  -P $DIRECTORY/
}

GET_PAGE_DATA()
{
RAW_ITEMS=`curl -s $ID |grep Wallpapers |sed 's/\}/\n/g'  | sed 's/ /_/g' |awk 'BEGIN { FS = "href=" } ; { print $2 $3 $4 $5 $6}' |grep "Item" |grep "popup"`

for items in $RAW_ITEMS
do
	item_id=`echo $items | sed 's/&/\n/g' |grep id |uniq`
	item_size=`echo $items | sed 's/&/\n/g' |sed 's/\"/\n/g' |grep Size |sed '1!G;h;$!d' `
	
	for value in `echo $item_size|sed 's/Size=//g'`
	do
		if [ $value == "800x600" ]; then
			sleep 0.001
			#GET_IMAGES
			break
		elif [ $value == "1024x768" ]; then
			sleep 0.001
			GET_IMAGES
			break
		elif [ $value == "1280x800" ]; then
			sleep 0.001
			#GET_IMAGES
			break
		elif [ $value == "1280x960" ]; then
			sleep 0.001
			GET_IMAGES
			break
		elif [ $value == "1440x900" ]; then
			sleep 0.001
			#GET_IMAGES
			break
		elif [ $value == "1680x1050" ]; then
			sleep 0.001
			#GET_IMAGES
			break
		elif [ $value == "1920x1200" ]; then
			sleep 0.001
			#GET_IMAGES
			break
		else
			GET_IMAGES
		fi
	done
done
}

GET_ALL_PAGES
