emetteur et recepteur:
login: lora / root
passwd: lora / root



software:

actuellement, Tx et Rx paramétrés par défaut (voir page interface pour valeurs)

dans la console: ifconfig pour avoir l'adresse IP

RTC:

./set_rtc: pour mettre à l'heure l'orloge temps-reel
./get_rtc: pour lire l'heure sur l'orloge temps-reel

RX:

./multi_lora_rx_v1d <nombre de canaux> <timeout us> <backup hour> <backup min>

	1/ le file system doit être en rw (commande rw) pour le backup
	2/ lancer en tâche de fond pour récupérer la main
	3/ dans un web browser, taper l'adresse IP pour ouvrir la page interface
	4/ backups sur la carte SD ds le répertoire "backups" sous le compte "lora"

fichiers de travail:
	fichiers virtuels sur /tmp (re-générés au démarrage du soft)
	channelxx.index = index courant des messages du canal xx (index du dernier message reçu, 0 si aucun message encore reçu)
	channelxx.log = statistique de chaque message (tous les messages dans ce fichier)

000000000: No message received yet!
000000001: Valid header received on channel 10, central freq = 865.20MHz
Time stamp: 10/10/2018 10:04:14 (dd/mm/yyyy hh:mm:ss) 
Message received
Payload CRC OK (INFO: test relevance depends on transmitter configuration)
Number of bytes received = 9
byte 0 data= 0x74 , t
byte 1 data= 0x65 , e
byte 2 data= 0x73 , s
byte 3 data= 0x74 , t
byte 4 data= 0x5F , _
byte 5 data= 0x63 , c
byte 6 data= 0x68 , h
byte 7 data= 0x31 , 1
byte 8 data= 0x30 , 0
Message as string:test_ch10
RSSI (dBm) = -103



TX:

./multi_lora_tx_v1 <channel number10..17> <message (128 characters max)>
	1/ garder une distance sufisante avec le Rx (> 1m) pour ne pas saturer (pb de redémarrage après saturation non résolu)
	2/ dans un web browser, taper l'adresse IP pour ouvrir la page interface






