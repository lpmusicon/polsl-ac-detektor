# Zakończono faze związaną z SMIW

## Projekt SMIW 2020
### Detektor AC

#### Struktura projektu
* Projekt embedded oparty o PlatformIO
* Projekt Frontend oparty o Framework Angular

#### Wymagania
* PlatformIO
* Mikrokontroler ESP32
* Moduł GSM SIM800L
* Układ kontroli zasilania baterii Li-Po np. TP4056 wraz z układem zabezpieczającym baterię przed nadmiernym rozładowaniem
* Układ buck-boost do zasilania modułu GSM - Wymagane 5V (5.1V działa najlepiej w tym wypadku)
* Rezystory do zbicia napięcia 5V na pinach kontrolnych do bezpiecznego poziomu 3.3V
* Bateria Li-Po 1S np. Bateria LiPo 3.7V 3000mAh
* Kondensatory do zabezpieczenia napiecia zasilania przy module SIM800L

#### Schemat podłączeniowy
Widoczny w raporcie

#### Schemat PCB
Widoczny w raporcie
