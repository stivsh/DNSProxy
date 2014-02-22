#version
./inc_build.sh
./inc_pack.sh
#serv
rm server/usr/sbin/dnsproxyserver
cp "`cat options/server_path`/dnsproxyserver" server/usr/sbin/
fakeroot dpkg-deb --build server
mv server.deb dnsproxyserver-`cat options/version`-amd64.deb
#client
rm client/usr/bin/dnsproxyclient
cp "`cat options/client_path`/dnsproxyclient" client/usr/bin/
fakeroot dpkg-deb --build client
mv client.deb dnsproxyclient-`cat options/version`-amd64.deb
