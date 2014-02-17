#version
./inc_build.sh
./inc_pack.sh
#serv exec filr
rm server/usr/sbin/dnsproxyserver
cp "`cat options/server_path`/dnsproxyserver" server/usr/sbin/
fakeroot dpkg-deb --build server
mv server.deb dnsproxyserver-`cat options/version`-amd64.deb
