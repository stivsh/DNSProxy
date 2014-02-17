ver=`cat options/version`
sed -i "s/Version:	.*/Version:	$ver/g" server/DEBIAN/control
