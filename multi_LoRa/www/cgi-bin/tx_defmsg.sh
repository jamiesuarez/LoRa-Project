#! /bin/sh
printf "\r\n\r\n"
echo '<html>'
echo '<head>'
echo '<title></title>'
echo '</head>'
echo '<body>'
echo '<p> Sending default BEACON message ...</p>'
echo '<pre>'
/home/lora/multi_lora_tx_v1 $QUERY_STRING
echo '</pre>'
echo '<a href="/index.html"><img src="../images/back.png" alt="Back.."/></a>'
echo '</body>'
echo '</html>'


