#! /bin/sh

printf "\r\n\r\n"
echo '<html>'
echo '<head>'
echo '<title>Channel 14 messages</title>'
echo '<meta http-equiv="refresh" content="5" />'
echo '</head>'
echo '<body>'
echo '<pre>'
cat  /tmp/channel14.log
echo '</pre>'
echo '<hr/>'
echo '<a href="/index.html"><img src="../images/back.png" alt="Back.."/></a>'
echo '</body>'
echo '</html>'



