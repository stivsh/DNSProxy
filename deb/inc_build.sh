versions=`cat options/version`
arver=(`echo $versions| tr "-" " "`)
pack_ver=${arver[1]}
versions=${arver[0]}
arver=(`echo $versions | tr "." " "`)
major=${arver[0]}
minor=${arver[1]}
build=${arver[2]}
#increase build
build=$((build+1))
#save version to file
echo "$major.$minor.$build-$pack_ver" > options/version
