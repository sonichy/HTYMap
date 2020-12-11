s="[Desktop Entry]\nName=海天鹰地图\nComment=谷歌卫星地图\nExec=`pwd`/HTYMap\nIcon=`pwd`/HTYMap.png\nPath=`pwd`\nTerminal=false\nType=Application\nCategories=Image;"
echo -e $s > HTYMap.desktop
cp `pwd`/HTYMap.desktop ~/.local/share/applications/HTYMap.desktop